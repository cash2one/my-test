/*************************************************************************
	> File Name: ddos_trie.c
	> Created Time: Tue 25 Aug 2015 10:58:11 AM ICT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "ddos_trie.h"

extern struct trie *net_iplist_trie;
static struct tnode *tnode_free_head;
static size_t tnode_free_size;
struct trie *g_ddos_iplist_trie = NULL;
struct trie *g_ddos_whitelist_trie = NULL;
static const int halve_threshold = 25;
static const int inflate_threshold = 50;
static const int halve_threshold_root = 15;
static const int inflate_threshold_root = 30;

static void tnode_put_child_reorg(struct tnode *tn, int i, 
		struct rt_trie_node *n, int wasfull);
static struct rt_trie_node *resize(struct trie *t, struct tnode *tn);
static struct tnode *inflate(struct trie *t, struct tnode *tn);
static struct tnode *halve(struct trie *t, struct tnode *tn);

#define trie_dbg(fmt, arg...) do\
{\
			printf("[%s:%d]", __func__, __LINE__);\
			printf(fmt, ##arg);\
}while(0)

#define rcu_assign_pointer(p, v) \
	(p) = (typeof(*v)*)(v)

#define rtnl_dereference(p)	\
	((typeof(*p)*)(p))
	
#define rcu_dereference(p) \
	((typeof(*p)*)(p))
	
#define rcu_dereference_rtnl(p)	\
	((typeof(*p)*)(p))


#define MAX_ERRNO	4095
#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)
static inline long  IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

static inline t_key mask_pfx(t_key k, unsigned int l)
{
	return (l == 0) ? 0 : k >> (KEYLENGTH-l) << (KEYLENGTH-l);
}

static inline unsigned long __fls(unsigned long word)
{
	return (sizeof(word) * 8) - 1 - __builtin_clzl(word);
}

/* Same as rcu_assign_pointer
 *  * but that macro() assumes that value is a pointer.
 *   */
static inline void node_set_parent(struct rt_trie_node *node, struct tnode *ptr)
{
	//smp_wmb();
	node->parent = (unsigned long)ptr | NODE_TYPE(node);
}

/*
 *  * Check whether a tnode 'n' is "full", i.e. it is an internal node
 *   * and no bits are skipped. See discussion in dyntree paper p. 6
 *    */

static inline int tnode_full(const struct tnode *tn, const struct rt_trie_node *n)
{	
	if (n == NULL  || IS_LEAF(n))
		return 0;

	return ((struct tnode *) n)->pos == tn->pos + tn->bits;
}


/*
 *   * Add a child at position i overwriting the old value.
 *     * Update the value of full_children and empty_children.
 *       */

static void tnode_put_child_reorg(struct tnode *tn, int i, struct rt_trie_node *n,
		int wasfull)
{
	struct rt_trie_node *chi = rtnl_dereference(tn->child[i]);
	int isfull;

	/* update emptyChildren */
	if (n == NULL && chi != NULL)
		tn->empty_children++;
	else if (n != NULL && chi == NULL)
		tn->empty_children--;

	/* update fullChildren */
	if (wasfull == -1)
		wasfull = tnode_full(tn, chi);

	isfull = tnode_full(tn, n);
	if (wasfull && !isfull)
		tn->full_children--;
	else if (!wasfull && isfull)
		tn->full_children++;

	if (n)
		node_set_parent(n, tn);

	rcu_assign_pointer(tn->child[i], n);
}

static inline void put_child(struct tnode *tn, int i,
					     struct rt_trie_node *n)
{
		tnode_put_child_reorg(tn, i, n, -1);
}

static __inline__ unsigned int  inet_make_mask(int logmask)
{
	if (logmask)
		return htonl(~((1U<<(32-logmask))-1));
	return 0;
}

static inline int tkey_equals(t_key a, t_key b)
{
	return a == b;
}

static inline int tkey_sub_equals(t_key a, int offset, int bits, t_key b)
{
	if (bits == 0 || offset >= KEYLENGTH)
		return 1;
	bits = bits > KEYLENGTH ? KEYLENGTH : bits;
	return ((a ^ b) << offset) >> (KEYLENGTH - bits) == 0;
}
static inline t_key tkey_extract_bits(t_key a, unsigned int offset, unsigned int bits)
{
	if (offset < KEYLENGTH)
		return ((t_key)(a << offset)) >> (KEYLENGTH - bits);
	else
		return 0;
}

static inline struct rt_trie_node *tnode_get_child(const struct tnode *tn, unsigned int i)
{
	return  rtnl_dereference(tn->child[i]);
}

static struct leaf *trie_find_node(struct trie *t, unsigned int key)
{
	int pos;
	struct tnode *tn;
	struct rt_trie_node *n;

	pos = 0;
	n = t->trie;
	while (n != NULL &&  NODE_TYPE(n) == T_TNODE) {
		tn = (struct tnode *) n;

		if (tkey_sub_equals(tn->key, pos, tn->pos-pos, key)) {
			pos = tn->pos + tn->bits;
			n = tnode_get_child(
					tn, 
					tkey_extract_bits(key, tn->pos,tn->bits)
				);
		} 
		else
			break;
	}

	/* Case we have found a leaf. Compare prefixes */

	if (n != NULL && IS_LEAF(n) && tkey_equals(key, n->key))
		return (struct leaf *)n;

	return NULL;
}

static struct leaf_info *find_leaf_info(struct leaf *l, int plen)
{
	struct hlist_head *head = &l->list;
	struct leaf_info *li;
	struct hlist_node *n;

	hlist_for_each_entry(li, n,  head, hlist){
		if (li->plen == plen)
			return li;
	}

	return NULL;
}

static struct leaf *leaf_new(void)
{
	struct leaf *l = (struct leaf *)malloc(sizeof(struct leaf));
	memset(l, 0x0, sizeof(struct leaf));
	
	if (l) {
		l->parent = T_LEAF;
		INIT_HLIST_HEAD(&l->list);
	}
	return l;
}
static inline void free_leaf(struct leaf *l)
{
	free(l);
	l = NULL;
}

static struct leaf_info *leaf_info_new(int plen)
{
	struct leaf_info *li = (struct leaf_info*)malloc(sizeof(struct leaf_info));
	memset(li, 0x0, sizeof(struct leaf_info));
	
	if (li) {
		li->plen = plen;
		li->mask_plen = ntohl(inet_make_mask(plen));
		li->counter = 0;
	}		
	return li;
}
static inline void free_leaf_info(struct leaf_info *leaf)
{
	free(leaf);
	leaf = NULL;
}

static void insert_leaf_info(struct hlist_head *head, struct leaf_info *new)
{
	struct leaf_info *li = NULL, *last = NULL;
	struct hlist_node *n;
	if (hlist_empty(head)) {
		hlist_add_head(&new->hlist, head);
	} 
	else {
		hlist_for_each_entry(li, n, head, hlist) {
			if (new->plen > li->plen)
				break;

			last = li;
		}
		if (last)
			hlist_add_after(&last->hlist, &new->hlist);
		else
			hlist_add_before(&new->hlist, &li->hlist);
	}
}

static inline int tkey_mismatch(t_key a, int offset, t_key b)
{
	t_key diff = a ^ b;
	int i = offset;

	if (!diff)
		return 0;
	while ((diff << i) >> (KEYLENGTH-1) == 0)
		i++;
	return i;
}
static struct tnode *tnode_new(t_key key, int pos, int bits)
{
	size_t sz = sizeof(struct tnode) + (sizeof(struct rt_trie_node *) << bits);
	struct tnode *tn = (struct tnode*)malloc(sz);
	memset(tn, 0x0, sz); //// zb-bug, why ??

	if (tn) {
		tn->parent = T_TNODE;
		tn->pos = pos;
		tn->bits = bits;
		tn->key = key;
		tn->full_children = 0;
		tn->empty_children = 1<<bits;
	}

	return tn;
}

static void __tnode_free_rcu(struct rcu_head *head)
{
	struct tnode *tn = container_of(head, struct tnode, rcu);

	free(tn);
	tn = NULL;
}

static inline void tnode_free(struct tnode *tn)
{
	
	if (IS_LEAF(tn))
		free_leaf((struct leaf *) tn);
	else{
		
		__tnode_free_rcu(&(tn->rcu));
	}
}

static inline int tnode_child_length(const struct tnode *tn)
{
	return 1 << tn->bits;
}
static void tnode_free_safe(struct tnode *tn)
{
	tn->tnode_free = tnode_free_head;
	tnode_free_head = tn;
	tnode_free_size += sizeof(struct tnode) +
		(sizeof(struct rt_trie_node *) << tn->bits);
}

static void tnode_clean_free(struct tnode *tn)
{
	int i;
	struct tnode *tofree;

	for (i = 0; i < tnode_child_length(tn); i++) {
		tofree = (struct tnode *)rtnl_dereference(tn->child[i]);
		if (tofree)
			tnode_free(tofree);
	}
	tnode_free(tn);
}

#define PAGE_SIZE 4096
static const int sync_pages = 128;
static void tnode_free_flush(void)
{
	struct tnode *tn;

	while ((tn = tnode_free_head)) {
		tnode_free_head = tn->tnode_free;
		tn->tnode_free = NULL;
		tnode_free(tn);
	}
	
	if (tnode_free_size >= PAGE_SIZE * sync_pages) {
		tnode_free_size = 0;
	}
}

static struct tnode *inflate(struct trie *t, struct tnode *tn)
{
	struct tnode *oldtnode = tn;
	int olen = tnode_child_length(tn);
	int i;

	tn = tnode_new(oldtnode->key, oldtnode->pos, oldtnode->bits + 1);

	if (!tn)
		return (void*)-1;

	for (i = 0; i < olen; i++) {
		struct tnode *inode;

		inode = (struct tnode *) tnode_get_child(oldtnode, i);
		if (inode &&
				IS_TNODE(inode) &&
				inode->pos == oldtnode->pos + oldtnode->bits &&
				inode->bits > 1) {
			struct tnode *left, *right;
			t_key m = ~0U << (KEYLENGTH - 1) >> inode->pos;

			left = tnode_new(inode->key&(~m), inode->pos + 1,
					inode->bits - 1);
			if (!left)
				goto nomem;

			right = tnode_new(inode->key|m, inode->pos + 1,
					inode->bits - 1);

			if (!right) {
				tnode_free(left);
				goto nomem;
			}

			put_child(tn, 2*i, (struct rt_trie_node *) left);
			put_child(tn, 2*i+1, (struct rt_trie_node *) right);
		}
	}

	for (i = 0; i < olen; i++) {
		struct tnode *inode;
		struct rt_trie_node *node = tnode_get_child(oldtnode, i);
		struct tnode *left, *right;
		int size, j;

		/* An empty child */
		if (node == NULL)
			continue;

		/* A leaf or an internal node with skipped bits */

		if (IS_LEAF(node) || ((struct tnode *) node)->pos >
				tn->pos + tn->bits - 1) {
			if (tkey_extract_bits(node->key,
						oldtnode->pos + oldtnode->bits,
						1) == 0)
				put_child(tn, 2*i, node);
			else
				put_child(tn, 2*i+1, node);
			continue;
		}

		/* An internal node with two children */
		inode = (struct tnode *) node;

		if (inode->bits == 1) {
			put_child(tn, 2*i, rtnl_dereference(inode->child[0]));
			put_child(tn, 2*i+1, rtnl_dereference(inode->child[1]));

			tnode_free_safe(inode);
			continue;
		}

		/* An internal node with more than two children */

		left = (struct tnode *) tnode_get_child(tn, 2*i);
		put_child(tn, 2*i, NULL);


		right = (struct tnode *) tnode_get_child(tn, 2*i+1);
		put_child(tn, 2*i+1, NULL);

		size = tnode_child_length(left);
		for (j = 0; j < size; j++) {
			put_child(left, j, rtnl_dereference(inode->child[j]));
			put_child(right, j, rtnl_dereference(inode->child[j + size]));
		}
		put_child(tn, 2*i, resize(t, left));
		put_child(tn, 2*i+1, resize(t, right));

		tnode_free_safe(inode);
	}
	tnode_free_safe(oldtnode);
	return tn;
nomem:
	tnode_clean_free(tn);
	return (void*)-1;
}
static struct tnode *halve(struct trie *t, struct tnode *tn)
{
	struct tnode *oldtnode = tn;
	struct rt_trie_node *left, *right;
	int i;
	int olen = tnode_child_length(tn);

	tn = tnode_new(oldtnode->key, oldtnode->pos, oldtnode->bits - 1);

	if (!tn)
		return (void*)-1;

	/*
	 *	 * Preallocate and store tnodes before the actual work so we
	 *		 * don't get into an inconsistent state if memory allocation
	 *			 * fails. In case of failure we return the oldnode and halve
	 *				 * of tnode is ignored.
	 *					 */

	for (i = 0; i < olen; i += 2) {
		left = tnode_get_child(oldtnode, i);
		right = tnode_get_child(oldtnode, i+1);

		/* Two nonempty children */
		if (left && right) {
			struct tnode *newn;

			newn = tnode_new(left->key, tn->pos + tn->bits, 1);

			if (!newn)
				goto nomem;

			put_child(tn, i/2, (struct rt_trie_node *)newn);
		}

	}

	for (i = 0; i < olen; i += 2) {
		struct tnode *newBinNode;

		left = tnode_get_child(oldtnode, i);
		right = tnode_get_child(oldtnode, i+1);

		/* At least one of the children is empty */
		if (left == NULL) {
			if (right == NULL)    /* Both are empty */
				continue;
			put_child(tn, i/2, right);
			continue;
		}

		if (right == NULL) {
			put_child(tn, i/2, left);
			continue;
		}

		/* Two nonempty children */
		newBinNode = (struct tnode *) tnode_get_child(tn, i/2);
		put_child(tn, i/2, NULL);
		put_child(newBinNode, 0, left);
		put_child(newBinNode, 1, right);
		put_child(tn, i/2, resize(t, newBinNode));
	}
	tnode_free_safe(oldtnode);
	return tn;
nomem:
	tnode_clean_free(tn);
	return (void*)-1;
}

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))
#define rcu_dereference_index_check(p) \
	({ \
		typeof(p) p1 = ACCESS_ONCE(p); p1; \
	})
	
static inline struct tnode *node_parent(const struct rt_trie_node *node)
{
	unsigned long parent = rcu_dereference_index_check(node->parent);
	return (struct tnode *)((parent) & ~NODE_TYPE_MASK);
}

#define MAX_WORK 10
static struct rt_trie_node *resize(struct trie *t, struct tnode *tn)
{
	int i;
	struct tnode *old_tn;
	int inflate_threshold_use;
	int halve_threshold_use;
	int max_work;

	if (!tn)
		return NULL;

	/* No children */
	if (tn->empty_children == tnode_child_length(tn)) {
		tnode_free_safe(tn);
		return NULL;
	}
	/* One child */
	if (tn->empty_children == tnode_child_length(tn) - 1)
		goto one_child;

	/* Keep root node larger  */
	if (!node_parent((struct rt_trie_node *)tn)) {
		inflate_threshold_use = inflate_threshold_root;
		halve_threshold_use = halve_threshold_root;
	} else {
		inflate_threshold_use = inflate_threshold;
		halve_threshold_use = halve_threshold;
	}

	max_work = MAX_WORK;
	while ((tn->full_children > 0 &&  max_work-- &&
				50 * (tn->full_children + tnode_child_length(tn)
					- tn->empty_children)
				>= inflate_threshold_use * tnode_child_length(tn))) {

		old_tn = tn;
		tn = inflate(t, tn);

		if (IS_ERR(tn)) {
			tn = old_tn;
			t->stats.resize_node_skipped++;
			break;
		}
	}

	/* Return if at least one inflate is run */
	if (max_work != MAX_WORK)
		return (struct rt_trie_node *) tn;

	/*
	 *	 * Halve as long as the number of empty children in this
	 *		 * node is above threshold.
	 *			 */

	max_work = MAX_WORK;
	while (tn->bits > 1 &&  max_work-- &&
			100 * (tnode_child_length(tn) - tn->empty_children) <
			halve_threshold_use * tnode_child_length(tn)) {

		old_tn = tn;
		tn = halve(t, tn);
		if (IS_ERR(tn)) {
			tn = old_tn;
			t->stats.resize_node_skipped++;
			break;
		}
	}


	/* Only one child remains */
	if (tn->empty_children == tnode_child_length(tn) - 1) {
one_child:
		for (i = 0; i < tnode_child_length(tn); i++) {
			struct rt_trie_node *n;

			n = rtnl_dereference(tn->child[i]);
			if (!n)
				continue;

			/* compress one level */

			node_set_parent(n, NULL);
			tnode_free_safe(tn);
			return n;
		}
	}
	return (struct rt_trie_node *) tn;
}


static void trie_rebalance(struct trie *t, struct tnode *tn)
{
	int wasfull;
	t_key cindex, key;
	struct tnode *tp;

	key = tn->key;

	while (tn != NULL && (tp = node_parent((struct rt_trie_node *)tn)) != NULL) {
		cindex = tkey_extract_bits(key, tp->pos, tp->bits);
		wasfull = tnode_full(tp, tnode_get_child(tp, cindex));
		tn = (struct tnode *)resize(t, tn);

		tnode_put_child_reorg(tp, cindex, (struct rt_trie_node *)tn, wasfull);

		tp = node_parent((struct rt_trie_node *) tn);
		if (!tp)
			rcu_assign_pointer(t->trie, (struct rt_trie_node *)tn);

		tnode_free_flush();
		if (!tp)
			break;
		tn = tp;
	}

	/* Handle last (top) tnode */
	if (IS_TNODE(tn))
		tn = (struct tnode *)resize(t, tn);

	rcu_assign_pointer(t->trie, (struct rt_trie_node *)tn);
	tnode_free_flush();
}

static struct leaf_info *trie_insert_node(struct trie *t, unsigned int  key, int plen)
{
	int pos, newpos;
	struct tnode *tp = NULL, *tn = NULL;
	struct rt_trie_node *n;
	struct leaf *l;
	int missbit;
	struct leaf_info *li;
	t_key cindex;

	pos = 0;
	n = t->trie;

	while (n != NULL &&  NODE_TYPE(n) == T_TNODE) {
		tn = (struct tnode *) n;

		if (tkey_sub_equals(tn->key, pos, tn->pos-pos, key)) {
			tp = tn;
			pos = tn->pos + tn->bits;
			n = tnode_get_child(
					tn,
					tkey_extract_bits(key, tn->pos, tn->bits)
					);
		}
		else
			break;
	}

	/* Case 1: n is a leaf. Compare prefixes */
	if (n != NULL && IS_LEAF(n) && tkey_equals(key, n->key)) {
		l = (struct leaf *) n;
		li = leaf_info_new(plen);

		if (!li)
			return NULL;

		insert_leaf_info(&l->list, li);
		goto done;
	}

	l = leaf_new();
	if (!l)
		return NULL;

	l->key = key;
	li = leaf_info_new(plen);
	if (!li) {
		free_leaf(l);
		return NULL;
	}
	insert_leaf_info(&l->list, li);

	if (t->trie && n == NULL) {
		/* Case 2: n is NULL, and will just insert a new leaf */
		node_set_parent((struct rt_trie_node *)l, tp);
		cindex = tkey_extract_bits(key, tp->pos, tp->bits);
		
		put_child(tp, cindex, (struct rt_trie_node *)l);
	} 
	else {
		/* Case 3: n is a LEAF or a TNODE and the key doesn't match. */
		 /*	*  Add a new tnode here
		 *	*  first tnode need some special handling
		 **/

		if (tp)
			pos = tp->pos+tp->bits;
		else
			pos = 0;

		if (n) {
			newpos = tkey_mismatch(key, pos, n->key);
			tn = tnode_new(n->key, newpos, 1);
		} else {
			newpos = 0;
			tn = tnode_new(key, newpos, 1); /* First tnode */
		}

		if (!tn) {
			free_leaf_info(li);
			free_leaf(l);
			return NULL;
		}

		node_set_parent((struct rt_trie_node *)tn, tp);

		missbit = tkey_extract_bits(key, newpos, 1);
		
		put_child(tn, missbit, (struct rt_trie_node *)l);
		put_child(tn, 1-missbit, n);

		if (tp) {
			cindex = tkey_extract_bits(key, tp->pos, tp->bits);
			
			put_child(tp, cindex, (struct rt_trie_node *)tn);
		} else {
			rcu_assign_pointer(t->trie, (struct rt_trie_node *)tn);
			tp = tn;
		}
	}

	/* Rebalance the trie */
	trie_rebalance(t, tp);
done:
	return li;
}
static int check_leaf(struct trie *t, struct leaf *l, t_key key)
{
	struct leaf_info *li;
	struct hlist_head *hhead = &l->list;
	struct hlist_node *n;
	hlist_for_each_entry(li, n, hhead, hlist) {
		if (l->key == (key & li->mask_plen)) {
			t->stats.semantic_match_passed++;
			li->counter++;
			return 0;
		}
	}
	t->stats.semantic_match_miss++;

	return 1;
}


int trie_table_lookup(struct trie *t, unsigned int ip)
{
	int ret;
	struct rt_trie_node *n;
	struct tnode *pn;
	unsigned int pos, bits;
	t_key key = ntohl(ip);
	unsigned int chopped_off;
	t_key cindex = 0;
	unsigned int current_prefix_length = KEYLENGTH;
	struct tnode *cn;
	t_key pref_mismatch;


	n = rcu_dereference(t->trie);
	if (!n){
		trie_dbg("error t->trie is null. \n");
		goto failed;
	}
	t->stats.gets++;

	/* Just a leaf? */
	if (IS_LEAF(n)) {
		ret = check_leaf(t, (struct leaf *)n, key);
		goto found;
	}

	pn = (struct tnode *) n;
	chopped_off = 0;

	while (pn) {
		pos = pn->pos;
		bits = pn->bits;

		if (!chopped_off)
			cindex = tkey_extract_bits(mask_pfx(key, current_prefix_length),
					pos, bits);

		n = tnode_get_child(pn, cindex);

		if (n == NULL) {
			t->stats.null_node_hit++;
			goto backtrace;
		}

		if (IS_LEAF(n)) {
			ret = check_leaf(t, (struct leaf *)n, key);
			if (ret > 0)
				goto backtrace;
			goto found;
		}

		cn = (struct tnode *)n;

		if (current_prefix_length < pos+bits) {
			if (tkey_extract_bits(cn->key, current_prefix_length,
						cn->pos - current_prefix_length)
					|| !(cn->child[0]))
				goto backtrace;
		}

		pref_mismatch = mask_pfx(cn->key ^ key, cn->pos);

		if (pref_mismatch) {
			/* fls(x) = __fls(x) + 1 */
			int mp = KEYLENGTH - __fls(pref_mismatch) - 1;

			if (tkey_extract_bits(cn->key, mp, cn->pos - mp) != 0)
				goto backtrace;

			if (current_prefix_length >= cn->pos)
				current_prefix_length = mp;
		}

		pn = (struct tnode *)n; /* Descend */
		chopped_off = 0;
		continue;

backtrace:
		chopped_off++;

		/* As zero don't change the child key (cindex) */
		while ((chopped_off <= pn->bits)
				&& !(cindex & (1<<(chopped_off-1))))
			chopped_off++;

		/* Decrease current_... with bits chopped off */
		if (current_prefix_length > pn->pos + pn->bits - chopped_off)
			current_prefix_length = pn->pos + pn->bits
				- chopped_off;

		/*
		 *		 * Either we do the actual chop off according or if we have
		 *				 * chopped off all bits in this tnode walk up to our parent.
		 *						 */

		if (chopped_off <= pn->bits) {
			cindex &= ~(1 << (chopped_off-1));
		} else {
			struct tnode *parent = node_parent((struct rt_trie_node *) pn);
			if (!parent){
				goto failed;
			}

			/* Get Child's index */
			cindex = tkey_extract_bits(pn->key, parent->pos, parent->bits);
			pn = parent;
			chopped_off = 0;

			t->stats.backtrack++;
			goto backtrace;
		}
	}
failed:
	ret = 1;
found:
	return ret;
}


int trie_table_insert(struct trie *t, struct trie_ip_cfg *cfg)
{
	int plen = cfg->plen;
	struct leaf_info *li = NULL;
	unsigned int  key, mask;
	int err;
	struct leaf *l;

	key = ntohl(cfg->ip); 
	trie_dbg("Inser %08x/%d\n", key, plen);

	mask = ntohl(inet_make_mask(plen));
	if (key & ~mask){
		trie_dbg("err key:%u, mask:%u\n", key, mask);
		return -1;
	}
	key = key & mask;

	l = trie_find_node(t, key);
	if (l) {
		li = find_leaf_info(l, plen);
	}
	if (li) {
		err = 0;
		trie_dbg("ip-plen exist.\n");
		goto err;
	} 
	/*
	 *	 * Insert new entry to the list.
	 **/
	li = trie_insert_node(t, key, plen);
	if (unlikely(!li)) {
		err = -1;
		trie_dbg("no mem .\n");
		goto err;
	}

	return 0;
err:
	return err;
}

static void trie_leaf_remove(struct trie *t, struct leaf *l)
{
	struct tnode *tp = node_parent((struct rt_trie_node *) l);

	if (tp) {
		t_key cindex = tkey_extract_bits(l->key, tp->pos, tp->bits);
		put_child(tp, cindex, NULL);
		trie_rebalance(t, tp);
	} 
	else
		t->trie =  NULL;

	free_leaf(l);
}

int trie_table_delete(struct trie *t, struct trie_ip_cfg *cfg)
{
	unsigned int  key, mask;
	int plen = cfg->plen;
	struct leaf *l;
	struct leaf_info *li;

	key = ntohl(cfg->ip);
	mask = ntohl(inet_make_mask(plen));

	if (key & ~mask)
		return -1;

	key = key & mask;
	l = trie_find_node(t, key);

	if (!l)
		return -1;

	li = find_leaf_info(l, plen);

	if (!li)
		return -1;

	hlist_del_rcu(&li->hlist);
	free_leaf_info(li);

	if (hlist_empty(&l->list))
		trie_leaf_remove(t, l);

	return 0;
}

/*
 *  * Scan for the next right leaf starting at node p->child[idx]
 *   * Since we have back pointer, no recursion necessary.
 *    */
static struct leaf *leaf_walk_rcu(struct tnode *p, struct rt_trie_node *c)
{
	do {
		t_key idx;

		if (c)
			idx = tkey_extract_bits(c->key, p->pos, p->bits) + 1;
		else
			idx = 0;

		while (idx < 1u << p->bits) {
			c = tnode_get_child(p, idx++);
			if (!c)
				continue;

			if (IS_LEAF(c))
				return (struct leaf *) c;

			/* Rescan start scanning in new node */
			p = (struct tnode *) c;
			idx = 0;
		}

		/* Node empty, walk back up to parent */
		c = (struct rt_trie_node *) p;
	} while ((p = node_parent(c)) != NULL);

	return NULL; /* Root of trie */
}

static struct leaf *trie_firstleaf(struct trie *t)
{
	struct tnode *n = (struct tnode *)rcu_dereference_rtnl(t->trie);

	if (!n)
		return NULL;

	if (IS_LEAF(n))          /* trie is just a leaf */
		return (struct leaf *) n;

	return leaf_walk_rcu(n, NULL);
}

static struct leaf *trie_nextleaf(struct leaf *l)
{
	struct rt_trie_node *c = (struct rt_trie_node *) l;
	struct tnode *p = node_parent(c);

	if (!p)
		return NULL;	/* trie with just one leaf */

	return leaf_walk_rcu(p, c);
}


/* static struct leaf *trie_leafindex(struct trie *t, int index)
{
	struct leaf *l = trie_firstleaf(t);

	while (l && index-- > 0)
		l = trie_nextleaf(l);

	return l;
}
*/

static int trie_flush_leaf(struct leaf *l)
{
	int found = 0;
	struct hlist_head *lih = &l->list;
	struct hlist_node *tmp, *n;
	struct leaf_info *li = NULL;

	hlist_for_each_entry_safe(li, tmp, n, lih, hlist) {
		hlist_del(&li->hlist);
		free_leaf_info(li);
		found++;
	}
	return found;
}


int trie_table_flush (struct trie *t)
{
	struct leaf *l, *ll = NULL;
	int found = 0;

	for (l = trie_firstleaf(t); l; l = trie_nextleaf(l)) {
		found += trie_flush_leaf(l);

		if (ll && hlist_empty(&ll->list))
			trie_leaf_remove(t, ll);
		ll = l;
	}

	if (ll && hlist_empty(&ll->list))
		trie_leaf_remove(t, ll);

	return found;
}
int ddos_trie_init(void)
{
	
	g_ddos_iplist_trie = (struct trie*)malloc(sizeof(struct trie));
	if(NULL == g_ddos_iplist_trie){
		trie_dbg("g_ddos_iplist_trie malloc error.\n");
		return -1;
	}
	g_ddos_whitelist_trie = (struct trie*)malloc(sizeof(struct trie));
	if(NULL == g_ddos_whitelist_trie){
		trie_dbg("g_ddos_whitelist_trie malloc error.\n");
		return -1;
	}
	memset(g_ddos_iplist_trie, 0x0, sizeof(struct trie));
	memset(g_ddos_whitelist_trie, 0x0, sizeof(struct trie));

	return 0;
}

void ddos_trie_fini(void)
{
	if (net_iplist_trie)
	{
	trie_table_flush(net_iplist_trie);
	free(net_iplist_trie);
	net_iplist_trie = NULL;
	}
	if (g_ddos_iplist_trie)
	{
	trie_table_flush(g_ddos_iplist_trie);
	free(g_ddos_iplist_trie);
	g_ddos_iplist_trie = NULL;
	}

	if(g_ddos_whitelist_trie)
	{
	trie_table_flush(g_ddos_whitelist_trie);
	free(g_ddos_whitelist_trie);
	g_ddos_whitelist_trie = NULL;
	}

}

