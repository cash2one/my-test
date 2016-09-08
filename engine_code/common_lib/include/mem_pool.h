#ifndef _MEM_POOL_H
#define _MEM_POOL_H

#include <sys/types.h>

typedef struct _mempool_t
{
	int total_num;	// total num
	int unit_size;	// include the size of header
	int data_size;	// the size of pure data

	// 双向循环链表
	void	*used_head;	// 使用过的数据表头
	int used_num;	// 使用过的数据个数

	// 双向循环链表
	void	*free_head;	// 未使用的数据表头
	int free_num;	// 未使用的数据个数

	unsigned char *bucket_pool;	// 实际存放数据的位置
} mempool_t;

/*
 * 功能：初始化
 * 失败：返回NULL
 * 成功：返回mempool指针
 */
mempool_t *mempool_init(int nmemb, int unit_size);

/*
 * 功能：清空
 */
void mempool_clean(mempool_t *mempool);

/************************************************
 *         Name: mempool_reset
 *  Description: 完成mempool的复位功能
 ************************************************/
void mempool_reset(mempool_t *mempool);

/*
 * 功能：从共享内存中分配一块空间
 * 失败：返回NULL
 * 成功：返回申请的空间
 */
void *mempool_malloc(mempool_t *mempool);

/*
 * 功能：从共享内存中分配一块空间，清空空间
 * 失败：返回NULL
 * 成功：返回申请的空间
 */
void *mempool_calloc(mempool_t *mempool);

/*
 * 功能：释放data所指向的空间
 */
void mempool_free(mempool_t *mempool, void *data);

/* 
 * 获取最早申请的数据
 */
void *mempool_oldest_data(mempool_t *mempool);

/* 
 * 获取最晚申请的数据
 */
void *mempool_newest_data(mempool_t *mempool);

/*
 * 功能：遍历占用链节点，对每个元素执行node_action函数
 * data：每个节点的数据，需要由使用者进行指定
 * arg：处理需要的其他参数
 * 返回值：0：停止遍历；1：继续遍历
 */
typedef int (*node_action)(void *data,void *arg);
#define OLD_DIR 0
#define NEW_DIR 1
/* 
 * direction: 0 向老数据方向移动 1 向新数据方向移动
 * begin_data: 起始位置所指向的数据
 */
int mempool_iterate(mempool_t *mempool, void *begin_data, int direction, 
				node_action action, void *arg);

/*
 * 功能：打印mempool占用链节点的信息；
 * verbose=0则只打印mempool结构信息； 
 * verbose=1则不仅能够打印mempool结构信息，
 * 		而且能够打印每个节点的信息
 */
void mempool_print(mempool_t *mempool, int verbose);

/*
 * 功能：根据id获取节点的data；
 * 如果该id未使用，则返回NULL
 */
void *mempool_dataget_byid(mempool_t *mempool, int id);

/*
 * 功能：查看id所指向的节点是否已经使用
 */ 
int mempool_dataused(mempool_t *mempool, int id);

/*
 * 功能：取得占用链的第一个节点
 */
void *mempool_dataget(mempool_t *mempool);

#endif
