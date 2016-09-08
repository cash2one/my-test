/*includes*/
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "hash.h"
#include "module_debug.h"
#include "misc.h"

#define END_OF_LIST	-1
#define INVALID_KEY	((hkey_t)(-1))

#define MIN_PRIME 53
#define MIN_HASH_ITEMS MIN_PRIME

#ifdef DEBUG_HASH_STAT
static FILE *hash_log = NULL;
#endif

#ifdef DEBUG_HASH
static inline int __check_equal(hkey_t key, void *data_ht, void *arg);
#endif

/********************************************
 * �ڲ�����
 ********************************************/
/**************************************************************************************
__verify_key()
����:
	��֤���key�ڱ�����û����ͬ��key������֤����ʱ�������
	key�ڱ����ǲ����Ѿ����ڡ�

����:
	key :
	
����ֵ:
	 0 :�ڱ���û���������ͬ��key,��Ψһ��key��
	-2 :�ڱ������������ͬ��key��

��ע��
	Ŀǰ�������������֤���ܣ���ΪTCP���������Ӧ����������ͬ��keyֵͬʱ������hash���У�
	����ht_data��ָ����Ǹ���Ԫ�鲻������hash�����ظ�
***************************************************************************************/
static inline int __verify_key(hkey_t key, hash_t *h, int index)
{
	return 0;
}

/**************************************************************************************
__hash_key2idx()
����:
	����hashֵ��hash����
����:
	key
	tablelen :hash��
����:
	���е�����
***************************************************************************************/
static inline int __hash_key2idx(hkey_t key, int tablelen)
{
	if (key < (hkey_t) tablelen) {
		return key;
	} else {
		return (key % (hkey_t) tablelen);
	}
}

/**************************************************************************************
����:
	��hash���и���key������Ӧ����Ŀ,��������֤��
	��Ҫ���ڳ�ͻ���Ͻ��в��ҡ�
	�ڲ����������õ�ʱ��ض����ڳ�ͻ��
����:
	h  :
	key:
	arg: ��Ҫ�ɵ�����ָ������֤��������֤�Ĳ���

����ֵ:
	-1: û���ҵ�Ҫ�ҵ���Ŀ
	-2: ָ������֤����arg��������֤����ָ���ǿյģ����Ǹ��쳣
	�����Ǹ�ֵ:�ҵ�����Ŀ��hash���е�����
***************************************************************************************/
static int __hash_find(hash_t *h, int index, hkey_t key, void *arg)
{
	int retval = -1;
	hpool_item_t *pool_table = h->pool_table;
	htable_item_t *h_table = h->h_table + index;
	int pool_index = END_OF_LIST;
	int element_counter = 0;

/* 	if (unlikely(!h->h_eqfunc && arg)) {
 * 		merror(debug_hash, "h_eqfunc is NULL but arg is not NULL\n");
 * 		return -2;
 * 	}
 * 
 */
	hash_equal_func equal_func = h->h_eqfunc;
	int need_func = 1;
	if (!arg || !equal_func) {
		need_func = 0;
	}

	/* ���ݳ�ͻ��(collision chain)˳�β��� */
	for (pool_index = h_table->head_index; 
			pool_index != END_OF_LIST; 
			pool_index = pool_table[pool_index].next) {
		if (++element_counter > h_table->element_counter) {
			mwarn(debug_hash,
				"hash element_counter is larger than element_counter, "
				"now_counter: %d, table->counter: %d\n",
				element_counter, h_table->element_counter);
			break;
		}

		if (pool_table[pool_index].ht_key == key
				&& (!need_func || equal_func(key, pool_table[pool_index].ht_data, arg))) {
			retval = pool_index;
			break;
		}
#ifdef DEBUG_HASH_STAT
		h->stat.collisiontimes_find++;
#endif
	}

	return retval;		//�������ֵ��-1,�����ڳ�ͻ��ɽû��Ҫ�ҵ�key
}

/**************************************************************************************
����:
	����hash�����ķ���ֵ���Լ���ǰ�ı��е�ռ�����
	�ҵ�һ����λ�ã�����ǰ��key���뵽hash����ȥ��

��������:
	h : hash�����Ϣ
	key : �������key
	data : �������key���Ӧ������ָ�� �����ָ��Ҳ�洢��hash�����Ŀ��
	
����ֵ:
	0 : ��hash����putһ��key,�ɹ�
	-1: �����Ŀ�������Ĳ��ɹ���put
	-2: ��hash����putһ��key,���ɹ�;��Ϊhash�����Ѿ����˸�keyֵ,����Ϊ����֤
		�����ߴ�������key��Ψһ�Ե�
***************************************************************************************/
static int __hash_put(hash_t *h, int index, hkey_t key, void *data)
{
	hpool_item_t *pool_table = h->pool_table;	//Ϊ���ٶ�
	int avail_head = h->avail_head;	//Ϊ���ٶ�
	hpool_item_t *get_node = pool_table + avail_head;

	/* �ӿհ���������ͷ��ȡ��һ����� */
	if (get_node->next != END_OF_LIST) {
		//�����ɾ������Ŀ��������β��
		pool_table[get_node->next].prev = END_OF_LIST;
	}

	h->avail_head = get_node->next;
	h->avail_num--;

	htable_item_t *h_table = h->h_table + index;

#ifdef DEBUG_HASH_STAT
	if (h_table.element_counter > 0)
		h->stat.collisiontimes++;
#endif

	// ���뵽��ͻ��ͷ
	if (h_table->head_index != END_OF_LIST) {
		pool_table[h_table->head_index].prev = avail_head;
	}

	get_node->prev = END_OF_LIST;
	get_node->next = h_table->head_index;

	h_table->head_index = avail_head;
	h_table->element_counter++;

	h->h_items++;

	/* �����´ӿհ�����ժ��������Ŀ�ĸ����� */
	get_node->ht_data = data;
	get_node->ht_key = key;
	get_node->h_index = index;

	return __verify_key(key, h, index);
}

/********************************************
 * �������жϺͼ���
 ********************************************/
// �ж�candidate�Ƿ�Ϊ����
static inline int is_prime(int candidate)
{
	/* No even number and none less than 10 will be passed here.  */
	int divn = 3;
	int sq = divn * divn;

	while (sq < candidate && candidate % divn != 0) {
		++divn;
		sq += (divn << 2);
		++divn;
	}

	return (candidate % divn != 0);
}

// ��ò�С��seed����С����
static inline int choose_table_size(int seed)
{
	if ((int)seed < MIN_PRIME) {
		return MIN_PRIME;
	}

	// compute the length of seed
	int power_two = 0;
	int tmp_seed = seed;
	while (tmp_seed > 0) {
		tmp_seed >>= 1;
		power_two++;
	}

	power_two = (1 << power_two);

	int new_seed = 0;
	do {
		/* Make it definitely odd.  */
		new_seed = ((power_two + (power_two >> 1)) >> 1) | 1;
		while (!is_prime(new_seed)) {
			new_seed += 2;
		}

		if (new_seed < seed) {
			power_two <<= 1;
		}
	} while (new_seed < seed);

	return new_seed;
}

hash_t *hash_init(int max_items, hash_equal_func eqfunc)
{
	hash_t *h = calloc(1, sizeof(hash_t));
	massert(debug_hash, h == NULL);

	if (max_items >= MIN_HASH_ITEMS) {
		h->h_max_items = choose_table_size(max_items);	//define the table length
		if (unlikely(h->h_max_items < 0)) {
			merror(debug_hash, "max_item %d is out of bounds\n", max_items);
			goto err;
		}
	} else {
		h->h_max_items = MIN_HASH_ITEMS;
	}

	mdump(debug_hash, "h_max_items = %d.\n", h->h_max_items);

	h->h_items = 0;
	h->h_eqfunc = eqfunc;
	h->avail_head = 0;
	h->avail_num = h->h_max_items;

	h->h_table = (htable_item_t *) calloc(h->h_max_items, sizeof(htable_item_t));
	massert(debug_hash, h->h_table == NULL);

	int i = 0;

	for (i = 0; i < h->h_max_items; i++) {
		h->h_table[i].head_index = END_OF_LIST;
	}

	h->pool_table = (hpool_item_t *) calloc(h->h_max_items, sizeof(hpool_item_t));
	massert(debug_hash, h->pool_table == NULL);

	//��ʼ����Ŀհ������м䲿��, �ӵ�1������max_items - 2��
	for (i = 0; i < h->h_max_items; i++) {
		h->pool_table[i].ht_key = INVALID_KEY;
		h->pool_table[i].h_index = END_OF_LIST;
		h->pool_table[i].prev = i - 1;
		h->pool_table[i].next = i + 1;
	}

	//��ʼ����Ŀհ�����β��
	h->pool_table[h->h_max_items - 1].next = END_OF_LIST;

#ifdef DEBUG_HASH_STAT
	if ((hash_log = fopen("./hash.log", "w")) == NULL) {
		printf("ww exceptional quit -- open file error!\n");
	}
#endif

#ifdef _REENTRANT_V
	v_lock_init(&(h->h_lock));
#endif

	return h;

err:
	merror(debug_hash, "hash init error\n");
	hash_free(h);
	return NULL;
}

void hash_free(hash_t *h)
{
	massert(debug_hash, h == NULL);

#ifdef DEBUG_HASH
	char OutPutStrTemp[200];

	sprintf(OutPutStrTemp, "%d", h->stat.collisiontimes_find);
	strcat(OutPutStrTemp, " : stat.collisiontimes_find \n\0");
	fwrite(OutPutStrTemp, sizeof(char), strlen(OutPutStrTemp), hash_log);

	sprintf(OutPutStrTemp, "%d", h->stat.collisiontimes_put);
	strcat(OutPutStrTemp, " : stat.collisiontimes_put \n\0");
	fwrite(OutPutStrTemp, sizeof(char), strlen(OutPutStrTemp), hash_log);

	sprintf(OutPutStrTemp, "%d", h->stat.collisiontimes);
	strcat(OutPutStrTemp, " : stat.collisiontimes \n\0");
	fwrite(OutPutStrTemp, sizeof(char), strlen(OutPutStrTemp), hash_log);

	fclose(hash_log);
#endif

	if (h->h_table) {
		free(h->h_table);
	}

	if (h->pool_table) {
		free(h->pool_table);
	}

	free(h);
	h = NULL;

err:
	return;
}

int hash_put(hash_t *h, hkey_t key, void *data)
{
	int retval = 0;

	hash_lock(h);
	massert(debug_hash, h == NULL || h->h_items == h->h_max_items || data == NULL || h->avail_num == 0);

 	int hv = __hash_key2idx(key, h->h_max_items);
	retval = __hash_put(h, hv, key, data);	//retval������0(�ɹ�)����-2(���ɹ�)
	hash_unlock(h);
	return retval;

err:
	hash_unlock(h);
	return -1;
}

int hash_getput(hash_t *h, hkey_t key, void *data)
{
	int retval = -1;

	hash_lock(h);
	massert(debug_hash, h == NULL || data == NULL);

 	int hv = __hash_key2idx(key, h->h_max_items);
	if (h->h_table[hv].head_index != END_OF_LIST && (__hash_find(h, hv, key, data)) >= 0) {
		mwarn(debug_hash, "hash_getput fail, get key = %u\n", key);
		goto err;
	}

	retval = __hash_put(h, hv, key, data);	//retval������0(�ɹ�)����-2(���ɹ�)

err:
	hash_unlock(h);
	return retval;
}

void *hash_get(hash_t *h, hkey_t key, void *arg)
{
	int posi = 0;
	void *retval = NULL;

	hash_lock(h);
	massert(debug_hash, h == NULL);

	int hv = __hash_key2idx(key, h->h_max_items);
	if (h->h_table[hv].head_index == END_OF_LIST || (posi = __hash_find(h, hv, key, arg)) < 0) {
		mwarn(debug_hash, "hash_get fail, key = %u\n", key);
		retval = NULL;
	} else {
		retval = h->pool_table[posi].ht_data;
	}

err:
	hash_unlock(h);
	return retval;
}

int hash_change(hash_t *h, hkey_t key, void *arg, void *to_data)
{
	int hv = 0, posi = 0;

	hash_lock(h);
	massert(debug_hash, h == NULL);

	hv = __hash_key2idx(key, h->h_max_items);
	if (h->h_table[hv].head_index == END_OF_LIST || (posi = __hash_find(h, hv, key, arg)) < 0) {
		mwarn(debug_hash, "hash_change fail, key = %u\n", key);
		goto err;
	} else {
		h->pool_table[posi].ht_data = to_data;
	}

	hash_unlock(h);
	return 0;

err:
	hash_unlock(h);
	return -1;
}

static inline void *_hash_remove_node(hash_t *h, int pool_index)
{
	void *retval = NULL;

	hpool_item_t *pool_table = h->pool_table;
	int avail_head = h->avail_head;

	hpool_item_t *rm_item = pool_table + pool_index;

	if (rm_item->h_index == -1) {
		mwarn(debug_hash, "can not remove a free node, pool_index: %d, h_index: %d\n", pool_index, rm_item->h_index);
		goto ret;
	}
	htable_item_t *h_table = h->h_table + rm_item->h_index;

	retval = rm_item->ht_data;

	/* �ӳ�ͻ������ɾ��һ����Ŀ */
	if (rm_item->prev != END_OF_LIST) {
		pool_table[rm_item->prev].next = rm_item->next;
	} else {
		h_table->head_index = rm_item->next;
	}

	if (rm_item->next != END_OF_LIST) {
		pool_table[rm_item->next].prev = rm_item->prev;
	}
	h_table->element_counter--;
	h->h_items--;

	// ���ýڵ㸳ֵ
	rm_item->ht_key = INVALID_KEY;
	rm_item->h_index = END_OF_LIST;

	/* ���հ�������������ոձ�ɾ������Ŀ */
	rm_item->prev = END_OF_LIST;
	rm_item->next = avail_head;
	if (avail_head != END_OF_LIST) {
		pool_table[avail_head].prev = pool_index;
	}
	h->avail_head = pool_index;
	h->avail_num++;

ret:
	return retval;
}

void *hash_remove_node(hash_t *h, int pool_index)
{
	void *retval = NULL;

	hash_lock(h);
	massert(debug_hash, h == NULL);

	retval = _hash_remove_node(h, pool_index);

err:
	hash_unlock(h);
	return retval;
}

void *hash_remove(hash_t *h, hkey_t key, void *arg)
{
	void *retval = NULL;
	int pool_index = 0;

	hash_lock(h);
	massert(debug_hash, h == NULL);

	int h_table_index = __hash_key2idx(key, h->h_max_items);
	htable_item_t *h_table = h->h_table + h_table_index;

	/*      to find the entry in hash_table */
	if (h_table->head_index == END_OF_LIST || (pool_index = __hash_find(h, h_table_index, key, arg)) < 0) {
		mwarn(debug_hash, "can not find collision node in index %d\n", h_table_index);
	} else {
		retval = _hash_remove_node(h, pool_index);
	}

err:
	hash_unlock(h);
	return retval;
}

void hash_replace(hash_t *h, hkey_t key, void *new_data, int(*get_idx) (int length))
{
	hash_lock(h);
	massert(debug_hash, h == NULL);

	int hv = __hash_key2idx(key, h->h_max_items);
	htable_item_t *h_table = h->h_table + hv;
	hpool_item_t *pi = NULL;
	hpool_item_t *pool_table = h->pool_table;

	// step 1: find the node to be replaced by
	if (h_table->element_counter > 0) {
		// find the oldest node
		pi = pool_table + h_table->head_index;

		while (pi->next != END_OF_LIST) {
			pi = pool_table + pi->next;
		}
	} else {
		// ȡ���ֵ
		int pool_idx = get_idx(h->h_max_items);

		pi = pool_table + pool_idx;

		htable_item_t *h_table_rm = h->h_table + pi->h_index;

		/* �ӳ�ͻ������ɾ��һ����Ŀ */
		if (pi->prev != END_OF_LIST) {
			pool_table[pi->prev].next = pi->next;
		} else {
			h_table_rm->head_index = pi->next;
		}

		if (pi->next != END_OF_LIST) {
			pool_table[pi->next].prev = pi->prev;
		}
		h_table_rm->element_counter--;

		// �����ͻ��
		pi->next = pi->prev = END_OF_LIST;
		pi->h_index = hv;

		h_table->head_index = pool_idx;
		h_table->element_counter = 1;
	}

	// step 2: replace the node with new data pointer and key
	pi->ht_data = new_data;
	pi->ht_key = key;

err:
	hash_unlock(h);
}

int hash_freenode_print(hash_t * h)
{
	int idx = h->avail_head;

	printf("free idx:");
	while (idx != END_OF_LIST) {
		printf(" %d", idx);
		idx = (h->pool_table[idx]).next;
	}

	printf("\n");

	return 0;
}

int hash_used_iterate(hash_t *h, action_node_t action_node, void *arg)
{
	massert(debug_hash, h == NULL || action_node == NULL);

	int idx = 0;
	int c_idx = 0;

	for (idx = 0; idx < h->h_max_items; idx++) {
		if (h->h_table[idx].element_counter > 0) {
			c_idx = h->h_table[idx].head_index;

			while (c_idx != END_OF_LIST) {
				action_node(h->pool_table[c_idx].ht_key, 
					h->pool_table[c_idx].ht_data, arg);

				c_idx = h->pool_table[c_idx].next;
			}
		}
	}

	return 0;

err:
	return -1;
}

static inline void free_node_mem(hkey_t key, void *data, void *arg)
{
	if (data) {
		free(data);
	}
}

int hash_clean_iterate(hash_t * h)
{
	return (hash_used_iterate(h, free_node_mem, NULL));
}

#ifdef _DEF_HASH_LINK
inline void hash_link_set(struct hash_link *i, hash_t *h, hkey_t key, void *arg)
{
	int hv = -1;

	i->i_hash = h;
	i->i_key = key;
	i->i_arg = arg;

	hv = __hash_key2idx(i->i_key, h->h_max_items);
	i->i_pos = h->h_table[hv].head_index;	//�п���h->h_table[hv].head_index��-1
}

void *hash_link_get(struct hash_link *i)
{
	hash_t *h;
	void *data;

	h = i->i_hash;
	data = NULL;

	hash_lock(h);

	if ((unsigned int) i->i_pos != 0xFFFFFFFF) {
		while (h->pool_table[i->i_pos].next != END_OF_LIST) {
			if (h->pool_table[i->i_pos].ht_key == i->i_key)
				break;
			i->i_pos = h->pool_table[i->i_pos].next;
		}

		if (h->pool_table[i->i_pos].ht_key == i->i_key) {
			data = h->pool_table[i->i_pos].ht_data;
			i->i_pos = h->pool_table[i->i_pos].next;
		}
	}

	hash_unlock(h);
	return data;
}

inline void hash_link_end(struct hash_link *i)
{
	i->i_hash = NULL;
	i->i_key = 0;
	i->i_pos = 0;
	i->i_arg = NULL;
}

void hash_link_lock(struct hash_link *i)
{
	hash_lock(i->i_hash);
}

void hash_link_unlock(struct hash_link *i)
{
	hash_unlock(i->i_hash);
}
#endif

/**************************************************************************************
__check_equal()

���ܣ�
	�����õ���֤����
***************************************************************************************/
#ifdef DEBUG_HASH
static inline int __check_equal(hkey_t key, void *data_ht, void *arg)
{
	if (*(int *) data_ht == *(int *) arg)
		return 1;
	else
		return 0;
}
#endif

