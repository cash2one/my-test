#ifndef __HASH_H
#define __HASH_H

#include <sys/types.h>
#include <stdlib.h>
#include "misc.h"

typedef unsigned int hkey_t;

/* type definition */
typedef int (*hash_equal_func) (hkey_t key, void *data_ht, void *arg);

typedef struct {
	int collisiontimes_find;
	int collisiontimes;
	int collisiontimes_put;
} hash_stat_t;

// hash table head
typedef struct {
	int head_index;		// idx
	int element_counter;	// counter of collision nodes
} htable_item_t;

// data node
typedef struct {
	int h_index;		// �ýڵ����ĸ�hash_table, 
				// ���ڿ��ٵĴ�pool_item�ж�λ��table
				// ��ֵΪ-1����ʾ�ýڵ���free��
	int prev;
	int next;

	hkey_t ht_key;	// ע������ʹ�õ���������
	void *ht_data;		// table of data

#ifdef _VREENTRANT_V
	lock_t item_lock;
#endif
} hpool_item_t;

// �±�һ��ʹ��int����Ϊ���Ա���ֻ��0x6fffffff��С���ڴ��������õ�, ����int�㹻��
// key �� unsigned int, ��Ϊ������ ip ����
/* structures */
typedef struct {
	int h_items;
	int h_max_items;

	htable_item_t *h_table;
	hpool_item_t *pool_table;	// the type of each node is pool_item

	int avail_head;	// free head
	int avail_num;	// free number

	hash_equal_func h_eqfunc;

#ifdef _VREENTRANT_V
	lock_t h_lock;
#endif

#ifdef DEBUG_HASH_STAT
	hash_stat_t stat;
#endif
} hash_t;

/********************************************************************
 *               inline function
 ********************************************************************/

/*********************
 * lock function
 *********************/
static inline void hash_lock(hash_t *h)
{
#ifdef _VREENTRANT_V
	v_lock(h->h_lock);
#endif
}
static inline void hash_unlock(hash_t *h)
{
#ifdef _VREENTRANT_V
	v_unlock(h->h_lock);
#endif
}

/*****************************
 * hash information function
 *****************************/
/************************************************
 *         Name: hash_count
 *  Description: �鿴��ǰhash���е���Ŀ������
 *		 
 *       Return: item_num
 ************************************************/
static inline int hash_count(hash_t * h)
{
	int retval = 0;

	hash_lock(h);
	retval = h->h_items;
	hash_unlock(h);
	return retval;
}

/************************************************
 *         Name: hash_full
 *  Description: hash�Ƿ�����
 *		 
 *       Return: 1: full; 0: not full
 ************************************************/
static inline int hash_full(hash_t *h)
{
	int retval = 0;

	hash_lock(h);
	retval = (h->h_items >= h->h_max_items);
	hash_unlock(h);
	return retval;
}

/********************************************************************
 *           �ӿڶ���
 ********************************************************************/

/************************************************
 *         Name: choose_table_size
 *  Description: ��ò�С��seed����С����
 *		 
 *       Return: 
 ************************************************/
//unsigned int choose_table_size(unsigned int seed);

/**************************************************************************************
����:
	��ʼ��hash_t *h

����:
	int max_items : ������ϣ����hash��ĳ���
	hash_equal_func eqfunc : �ɵ�����ע�����֤����, 
				 ��֤�������ط�0ֵ������֤�ɹ�,����0ֵ������֤���ɹ�

����ֵ:
	hash_t *h : ����hash���������Ϣ
	NULL: ʧ��
***************************************************************************************/
hash_t *hash_init(int max_items, hash_equal_func eqfunc);

/**************************************************************************************
����:	�ͷ�hash����ռ���ڴ�ռ䡣
����: 	����hash���������Ϣ�����ݽṹ
**************************************************************************************/
void hash_free(hash_t *h);

/**************************************************************************************
hash_put()

����:
	��hash���в���һ����Ŀ,����__hash_put(),__hash_put()�������
	����ĺ��Ĺ���

����:
	h : hash�����Ϣ
	key : �������key
	data : �������key���Ӧ������ָ�� �����ָ��Ҳ�洢��hash�����Ŀ��

����ֵ:
	0  : ��hash����putһ��key,�ɹ�
	-2 : ��hash����putһ��key,���ɹ�;
	     ��Ϊhash�����Ѿ����˸�keyֵ,����Ϊ����֤
	     �����ߴ�������key��Ψһ�Ե�
	-1 : �����Ŀ�������Ĳ��ɹ���put
***************************************************************************************/
int hash_put(hash_t *h, hkey_t key, void *data);

/**************************************************************************************
hash_get()
����:
	�����ṩ��key��hash�����ҵ���Ӧ����Ŀ��������Ӧ��ֵ

����:
	arg  :��Ҫ�ɵ�����ָ������֤��������֤�Ĳ���

����ֵ:
	һ���ǿյ�ַ: Ҫ�ҵ�key���Ӧ����ֵ��ָ��
	NULL :�ڱ��� û���ҵ���key
***************************************************************************************/
void *hash_get(hash_t *h, hkey_t key, void *arg);

/************************************************
 *         Name: hash_getput
 *  Description: hash_get��hash_put��ͳһ��, ��������򷵻�, 
 *  		 ����, ����
 *		 
 *       Return: 0: ����ɹ�
 *       	 -1: hash_get�ɹ�
 *       	 -2: ����ʧ��, ��hash_put
 ************************************************/
int hash_getput(hash_t *h, hkey_t key, void *data);

/************************************************
 *         Name: hash_remove_node
 *  Description: ������pool_table�е��±꣬ɾ���ڵ�
 *		 rm_item_index: ��pool_table���±�
 *
 *       Return: ��ɾ���Ľڵ��data; ��ýڵ�Ϊfree, 
 *       	 �򷵻�NULL
 ************************************************/
void *hash_remove_node(hash_t *h, int rm_item_index);

/**************************************************************************************
����:
	����key�ҵ���Ӧ����Ŀ����֮��hash����ɾ������������put��
	ɾ����Ԫ�����ڵĳ�ͻ���ϵ�������Ŀ��
	�书����_hash_remove_nodeʵ��

����:
	arg :��Ҫ��֤�Ĳ���

����ֵ:
	NULL :hash����û�и�key
	!NULL : hash���и�key ���ڵ�λ�õ�����ht_data
***************************************************************************************/
void *hash_remove(hash_t *h, hkey_t key, void *arg);

/************************************************
 *         Name: hash_change
 *  Description: ��λ��Ϊkey�������滻Ϊto_data
 *		 
 *       Return: 0: �ɹ��滻
 *       	 -1: ʧ��, δ���ҵ��ڵ�
 ************************************************/
int hash_change(hash_t *h, hkey_t key, void *arg, void *to_data);

/************************************************
 *         Name: hash_replace
 *  Description: ����滻ĳ��Ԫ��;
 *  	   note: ���ﲻ����key�ظ��Լ��
 ************************************************/
void hash_replace(hash_t *h, hkey_t key, void *new_data, int (*get_idx) (int length));

/************************************************
 *         Name: hash_used_iterate
 *  Description: ��ʹ�õ�hash�ڵ���б���
 *		 
 *       Return: 0; ���action_nodeΪNULL, �򷵻�-1
 ************************************************/
#if 0
typedef void (*action_h_table_t) (htable_item_t *h_table);
#endif
typedef void (*action_node_t) (hkey_t key, void *data, void *arg);
int hash_used_iterate(hash_t *h, action_node_t action_node, void *arg);

/************************************************
 *         Name: hash_clean_iterate
 *  Description: ɾ���Ѿ�����Ľڵ���data���ڴ�
 *  	   note: ����hash_used_iterate
 ************************************************/
int hash_clean_iterate(hash_t * h);

/************************************************
 *         Name: hash_freenode_print
 *  Description: ��ӡ���սڵ������
 ************************************************/
int hash_freenode_print(hash_t *h);

/********************************************************************
 *           hash_link_interface
 ********************************************************************/
#ifdef _DEF_HASH_LINK
typedef struct {
	hash_t *i_hash;
	int i_pos;
	hkey_t i_key;
	void *i_arg;
} hash_link_t;

/**************************************************************************************
hash_link_set()

���ܣ�
	ָ��Ҫ�鿴�ľ�����ͬ��keyֵ����Ŀ��keyֵ����֤������

������
	i:��¼Ҫ���ҵ���Ŀ����Ϣ:key,arg
	h:
	key:��Ҫ���ҵ�key
	arg����֤����
***************************************************************************************/
void hash_link_set(hash_link_t *i, hash_t *h, hkey_t key, void *arg);

/**************************************************************************************
hash_link_get()

���ܣ�
	�鿴Ҫ�鿴�ľ�����ͬ��keyֵ����Ŀ���õ�����dataֵ��ÿ����һ�Σ�����У�����һ����hash
	���е���Ŀ��dataֵ���ٵ���һ�Σ��õ���һ�����и���ͬ��keyֵ����Ŀ��data��
	��hash_link_set()֮���������ñ�����(ÿ�η���һ��)�����Եõ�ӵ�и���ͬ��keyֵ����Ŀ
	��data��

������
	i:��¼Ҫ���ҵ���Ŀ����Ϣ:key,arg

����ֵ��
	NULL������Ҳ���ӵ��������keyֵ����Ŀ��
	!NULL :ӵ�и�keyֵ����Ŀ��dataֵ�ĵ�ַ
***************************************************************************************/
void *hash_link_get(hash_link_t *i);

/************************************************
 *         Name: hash_link_end
 *  Description: ���hash_link_t
 ************************************************/
void hash_link_end(hash_link_t *i);

void hash_link_lock(hash_link_t *i);
void hash_link_unlock(hash_link_t *i);
#endif

#endif
