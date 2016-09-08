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
 *  Description: 初始化
 *	  param: max_items: 个数；
 *	         alloc_str_mem: 是否要分配空间进行字符串存储
 *		 
 *       Return: str_hash结构; NULL, 失败
 ************************************************/
str_hash_t *str_hash_init(int max_items, int max_str_len, int alloc_str_mem);

/************************************************
 *         Name: str_hash_clean
 *  Description: 清空
 *		 
 *        Param: free_string: 是否需要释放传进来的字符串指针.
 *			      和alloc_str_mem矛盾
 ************************************************/
void str_hash_clean(str_hash_t *str_hash, int free_string);

/************************************************
 *         Name: str_hash_put
 *  Description: 添加<string, data>的MAP节点
 *		 
 *       Return: 成功0, 失败-1或者-2
 ************************************************/
int str_hash_put(str_hash_t *str_hash, char *str_key, void *data);

/************************************************
 *         Name: str_hash_get
 *  Description: 通过字符串str_key, 获取data
 *		 
 *       Return: data指针
 ************************************************/
void *str_hash_get(str_hash_t *str_hash, char *str_key);

/************************************************
 *         Name: str_hash_remove
 *  Description: 删除一个节点, 以str_key为关键字
 *		 
 *       Return: 被删除的节点data
 ************************************************/
void *str_hash_remove(str_hash_t *str_hash, char*str_key);

/************************************************
 *         Name: str_hash_change
 *  Description: 修改一个节点内容
 *		 
 *       Return: 成功0; 失败-1
 ************************************************/
int str_hash_change(str_hash_t *str_hash, char *str_key, void *to_data);

/************************************************
 *         Name: str_hash_count
 *  Description: 获取节点个数
 *		 
 *       Return: 节点个数
 ************************************************/
static inline int str_hash_count(str_hash_t *str_hash)
{
	return (hash_count(str_hash->data_hash));
}

#endif

