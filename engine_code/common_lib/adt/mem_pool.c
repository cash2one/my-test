/*****************************
 * 使用堆栈的方式管理free链表
 ****************************/

#include <stdio.h>
#include <stdlib.h>

#include "misc.h"
#include "module_debug.h"
#include "mem_pool.h"

#ifndef offset_of
#define offset_of(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define MEM_ALIGNMENT	4

#ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE(size, align) (((size) + (align) - 1) & ~((align) - 1))
#endif

#define NODE_NOUSE	0
#define NODE_INUSE 	1

typedef struct _bucket_t {
	struct _bucket_t *prev;
	struct _bucket_t *next;

	char used;

	unsigned char data[0];	// data
} bucket_t;

static inline bucket_t *getnode_byid(mempool_t *mempool, int id)
{
	return ((bucket_t *)(mempool->bucket_pool + id * mempool->unit_size));
}

#define OFF_STACK(type, off_bucket) \
	do { \
		if ((off_bucket)->next != (off_bucket)) { \
			((off_bucket)->prev)->next = (off_bucket)->next;\
			((off_bucket)->next)->prev = (off_bucket)->prev;\
			\
			if ((off_bucket) == mempool->type##_head) { \
				mempool->type##_head = ((off_bucket)->next);\
			} \
		} else { \
			/* 只有一个节点 */ \
			mempool->type##_head = NULL;\
		} \
		\
		mempool->type##_num--; \
	} while (0);

#define APPEND_STACK(type, append_bucket) \
	do { \
		if (mempool->type##_head != NULL) { \
			bucket_t *head = (bucket_t *)(mempool->type##_head); \
			(append_bucket)->next = head; \
			(append_bucket)->prev = head->prev; \
			(head->prev)->next = (append_bucket); \
			head->prev = (append_bucket); \
		} else { \
			(append_bucket)->next = (append_bucket)->prev = (append_bucket);\
		} \
		mempool->type##_head = (append_bucket); \
		mempool->type##_num++; \
	} while (0);

mempool_t *mempool_init(int nmemb, int unit_size)
{
	mempool_t *mempool = NULL;
	massert(debug_mempool, nmemb <= 0 || unit_size <= 0);

	mempool = calloc(1, sizeof(mempool_t));
	massert(debug_mempool, mempool == NULL);

	mempool->unit_size = MEM_ALIGN_SIZE(unit_size + offset_of(bucket_t, data), MEM_ALIGNMENT);
	mempool->data_size = mempool->unit_size - offset_of(bucket_t, data);
	mempool->free_num = mempool->total_num = nmemb;

	mempool->bucket_pool = calloc(mempool->total_num, mempool->unit_size);
	massert(debug_mempool, mempool->bucket_pool == NULL);

	mempool->free_head = mempool->bucket_pool;

	// node one
	bucket_t *bucket = (bucket_t *)(mempool->bucket_pool);
	bucket->prev = getnode_byid(mempool, nmemb - 1);
	bucket->next = getnode_byid(mempool, 1);

	int idx = 0;
	for (idx = 1; idx < mempool->free_num; idx++) {
		bucket = getnode_byid(mempool, idx);
		bucket->next = getnode_byid(mempool, idx + 1);
		bucket->prev = getnode_byid(mempool, idx - 1);
	}

	// last node
	bucket->next = (bucket_t *)(mempool->bucket_pool);

	return mempool;

err:
	mempool_clean(mempool);
	return NULL;
}

void mempool_reset(mempool_t *mempool)
{
	if (unlikely(mempool == NULL)) {
		return;
	}

	mempool->used_head = NULL;
	mempool->used_num = 0;

	mempool->free_num = mempool->total_num;
	memset(mempool->bucket_pool, 0, mempool->total_num * mempool->unit_size);
	mempool->free_head = mempool->bucket_pool;

	// 串链
	// node one
	bucket_t *bucket = (bucket_t *)(mempool->bucket_pool);
	bucket->prev = getnode_byid(mempool, mempool->total_num - 1);
	bucket->next = getnode_byid(mempool, 1);

	int idx = 0;
	for (idx = 1; idx < mempool->free_num; idx++) {
		bucket = getnode_byid(mempool, idx);
		bucket->next = getnode_byid(mempool, idx + 1);
		bucket->prev = getnode_byid(mempool, idx - 1);
	}

	// last node
	bucket->next = (bucket_t *)(mempool->bucket_pool);

	return;
}

void mempool_clean(mempool_t *mempool)
{
	if (mempool) {
		if (mempool->bucket_pool) {
			free(mempool->bucket_pool);
		}

		free(mempool);
	}

	return;
}

static inline void *_mempool_malloc(mempool_t *mempool)
{
	bucket_t *bucket = (bucket_t *)(mempool->free_head);

	OFF_STACK(free, bucket);
	APPEND_STACK(used, bucket);
	bucket->used = NODE_INUSE;

	return (bucket->data);
}

void *mempool_calloc(mempool_t *mempool)
{
	massert(debug_mempool, 
			mempool == NULL
			|| mempool->free_num == 0);

	void *data = _mempool_malloc(mempool);
	memset(data, 0, mempool->data_size);

	return (data);

err:
	return NULL;
}

void *mempool_malloc(mempool_t *mempool)
{
	massert(debug_mempool, 
			mempool == NULL
			|| mempool->free_num == 0);

	return _mempool_malloc(mempool);

err:
	return NULL;
}

void mempool_free(mempool_t *mempool, void *data)
{
	massert(debug_mempool, 
		mempool == NULL
		|| data == NULL 
		|| mempool->used_num == 0);

	bucket_t *bucket = (bucket_t *) ((unsigned char *) data - offset_of(bucket_t, data));
	massert(debug_mempool, bucket->used == NODE_NOUSE);	// 可以防止一些越界

	OFF_STACK(used, bucket);
	APPEND_STACK(free, bucket);
	bucket->used = NODE_NOUSE;

err:
	return;
}

static inline void bucket_print(bucket_t *bucket)
{
	printf("prev: %p, next: %p, used: %d\n", 
			bucket->prev, bucket->next, 
			bucket->used);
}

void mempool_print(mempool_t *mempool, int verbose)
{
	printf("====== mempool information ========\n");
	printf("mempool: free_head: %p, free_num: %d, used_head: %p, used_num: %d\n", 
			mempool->free_head, mempool->free_num, 
			mempool->used_head, mempool->used_num);

	if (verbose) {
		int idx = 0;
		bucket_t *bucket = NULL;
		for (idx = 0; idx < mempool->total_num; idx++) {
			bucket = getnode_byid(mempool, idx);
			bucket_print(bucket);
		}
	}

	return;
}

void *mempool_oldest_data(mempool_t *mempool)
{
	massert(debug_mempool, 
			mempool == NULL 
			|| mempool->used_head == NULL);

	return ((((bucket_t *)(mempool->used_head))->prev)->data);

err:
	return NULL;
}

void *mempool_newest_data(mempool_t *mempool)
{
	massert(debug_mempool, 
			mempool == NULL 
			|| mempool->used_head == NULL);

	return (((bucket_t *)(mempool->used_head))->data);

err:
	return NULL;
}

/* 
 * direction: 0 向老数据方向移动 1 向新数据方向移动
 * begin_data: 起始位置所指向的数据
 */
int mempool_iterate(mempool_t *mempool, void *begin_data, int direction, 
				node_action action, void *arg)
{
	int action_times = 0;

	massert(debug_mempool, 
			mempool == NULL 
			|| mempool->used_head == NULL
			|| begin_data == NULL);

	bucket_t *bucket = (bucket_t *)((unsigned char *)begin_data - offset_of(bucket_t, data));
	void *data = NULL;
	void *end_bucket = mempool->used_head;

	if (direction != OLD_DIR) {
		end_bucket = ((bucket_t *)(mempool->used_head))->prev;
	}

	do {
		data = bucket->data;
		if (direction == OLD_DIR) {
			bucket = bucket->next;
		} else {
			bucket = bucket->prev;
		}

		if (action) {
			if(action(data,arg) <= 0){
				break;
			}

			action_times++;
		}
	} while (bucket->used == NODE_INUSE && bucket != end_bucket);

err:
	return action_times;
}

int mempool_dataused(mempool_t *mempool, int id)
{
	massert(debug_mempool, mempool == NULL || id > mempool->total_num);
	bucket_t *bucket = getnode_byid(mempool, id);

	return (bucket->used);

err:
	return NODE_INUSE;
}

static inline void *_mempool_get_data(bucket_t *bucket)
{
	if (bucket->used == NODE_NOUSE) {
		return NULL;
	} else {
		return bucket->data;
	}
}

void *mempool_dataget_byid(mempool_t *mempool, int id)
{
	massert(debug_mempool, mempool == NULL 
			|| mempool->used_head == NULL
			|| id > mempool->total_num);

	bucket_t *bucket = getnode_byid(mempool, id);
	return (_mempool_get_data(bucket));

err:
	return NULL;
}

// the first node of used head
void *mempool_dataget(mempool_t *mempool)
{
	massert(debug_mempool, 
		mempool == NULL 
		|| mempool->used_head == NULL);

	return (_mempool_get_data((bucket_t *)(mempool->used_head)));

err:
	return NULL;
}

#if	0

#include <stdio.h>
#include "mem_pool.h"
mempool_t *memhandle = NULL;

int print_node(void *data, void *arg)
{
	printf("%s\n", (unsigned char *)data);
//	mempool_free(memhandle, data);
	return 1;
}

char *data[16] = {
	"0abcd",
	"1defg",
	"2hijk",
	"3lmnq",
	"4abcd",
	"5defg",
	"6hijk",
	"7lmnq"
};

int main(void)
{
	int i = 0;
	char *ps[16];

	memhandle = mempool_init(10, 30);
	if (memhandle == NULL) {
		printf("memhandle is NULL\n");
		return -1;
	}

	for (i = 0; i < 16; i++) {
		ps[i] = NULL;
	}

	for (i = 0; i < 8; i++) {
		ps[i] = mempool_calloc(memhandle);
		if (ps[i] != NULL) {
			strcpy(ps[i], data[i]);
			printf("copy %s\n", ps[i]);
		} else {
			printf("aaaa, overflow\n");
		}
	}

	for (i = 0; i < 8; i++) {
		mempool_free(memhandle, ps[i]);
	}

	mempool_iterate(memhandle, mempool_newest_data(memhandle), 0, print_node, NULL);
	mempool_clean(memhandle);

	return 0;
}

#endif
