/*************************************************************************
	> File Name: ddos_trie.h
	> Created Time: Tue 25 Aug 2015 10:57:06 AM ICT
 ************************************************************************/
#ifndef _DDOS_TRIE_H_
#define _DDOS_TRIE_H_
#include "list.h"

typedef unsigned int t_key;
#define T_TNODE 0
#define T_LEAF  1
#define NODE_TYPE_MASK	0x1UL
#define NODE_TYPE(node) ((node)->parent & NODE_TYPE_MASK)

#define IS_TNODE(n) (!(n->parent & T_LEAF))
#define IS_LEAF(n) (n->parent & T_LEAF)
#define KEYLENGTH (8*sizeof(t_key))


struct callback_head {
	struct callback_head *next;
	void (*func)(struct callback_head *head);
};
#define rcu_head callback_head

struct tnode {
	unsigned long parent;
	t_key key;
	unsigned char pos;		/* 2log(KEYLENGTH) bits needed */
	unsigned char bits;		/* 2log(KEYLENGTH) bits needed */
	unsigned int full_children;	/* KEYLENGTH bits needed */
	unsigned int empty_children;	/* KEYLENGTH bits needed */
	union {
		struct rcu_head rcu;
		struct tnode *tnode_free;
	};
	struct rt_trie_node *child[0];
};

struct leaf {
	unsigned long parent;
	t_key key;
	struct hlist_head list;
	struct rcu_head rcu;
};

struct leaf_info {
	struct hlist_node hlist;
	int plen;
	unsigned int  mask_plen; /* ntohl(inet_make_mask(plen)) */
	unsigned long long  counter;
	struct rcu_head rcu;
};

struct rt_trie_node {
	unsigned long parent;
	t_key key;
};

struct trie_use_stats {
	unsigned int gets;
	unsigned int backtrack;
	unsigned int semantic_match_passed;
	unsigned int semantic_match_miss;
	unsigned int null_node_hit;
	unsigned int resize_node_skipped;
};

struct trie{
	struct rt_trie_node *trie;
	struct trie_use_stats stats;
};

struct trie_ip_cfg{
	unsigned int ip;
	int plen;
};

extern struct trie *g_ddos_iplist_trie;
extern struct trie *g_ddos_whitelist_trie;


extern int trie_table_lookup(struct trie *t, unsigned int ip);
extern int trie_table_insert(struct trie *t, struct trie_ip_cfg *cfg);
extern int trie_table_delete(struct trie *t, struct trie_ip_cfg *cfg);
extern int trie_table_flush (struct trie *t);

extern  int ddos_trie_init(void);
extern void ddos_trie_fini(void);

#endif /* _DDOS_TRIE_H_ */

