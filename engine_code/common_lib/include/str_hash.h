#ifndef _STR_HASH_H
#define _STR_HASH_H

#include <sys/types.h>
#include "hash.h"

//#define MAX_STRLEN 128

typedef struct {
	void *data_node_pool;
	void *data_hash;

	void *str_pool;
	int alloc_str_mem;
	int max_str_len;
} str_hash_t;
#ifdef __cplusplus
extern "C"{
#endif

/************************************************
 *         Name: str_hash_init
 *  Description: ��ʼ��
 *	  param: max_items: ������
 *	         alloc_str_mem: �Ƿ�Ҫ����ռ�����ַ����洢
 *		 
 *       Return: str_hash�ṹ; NULL, ʧ��
 ************************************************/
str_hash_t *str_hash_init(int max_items, int max_str_len, int alloc_str_mem);

/************************************************
 *         Name: str_hash_clean
 *  Description: ���
 *		 
 *        Param: free_string: �Ƿ���Ҫ�ͷŴ��������ַ���ָ��.
 *			      ��alloc_str_memì��
 ************************************************/
void str_hash_clean(str_hash_t *str_hash, int free_string);

/************************************************
 *         Name: str_hash_put
 *  Description: ���<string, data>��MAP�ڵ�
 *		 
 *       Return: �ɹ�0, ʧ��-1����-2
 ************************************************/
int str_hash_put(str_hash_t *str_hash, char *str_key, void *data);

/************************************************
 *         Name: str_hash_get
 *  Description: ͨ���ַ���str_key, ��ȡdata
 *		 
 *       Return: dataָ��
 ************************************************/
void *str_hash_get(str_hash_t *str_hash, char *str_key);

/************************************************
 *         Name: str_hash_remove
 *  Description: ɾ��һ���ڵ�, ��str_keyΪ�ؼ���
 *		 
 *       Return: ��ɾ���Ľڵ�data
 ************************************************/
void *str_hash_remove(str_hash_t *str_hash, char*str_key);

/************************************************
 *         Name: str_hash_change
 *  Description: �޸�һ���ڵ�����
 *		 
 *       Return: �ɹ�0; ʧ��-1
 ************************************************/
int str_hash_change(str_hash_t *str_hash, char *str_key, void *to_data);

/************************************************
 *         Name: str_hash_count
 *  Description: ��ȡ�ڵ����
 *		 
 *       Return: �ڵ����
 ************************************************/
static inline int str_hash_count(str_hash_t *str_hash)
{
	return (hash_count(str_hash->data_hash));
}

#endif

