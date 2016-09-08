#ifndef _LIST_N_H
#define _LIST_N_H

#include <sys/types.h>
#include <stdio.h>

//#ifdef _REENTRANT
//#include <pthread.h>
//#endif
#include "misc.h"

typedef struct list_node_t_ {
	struct list_node_t_ *prev;
	struct list_node_t_ *next;
} list_node_t;

typedef struct {
	list_node_t *head;
	list_node_t *tail;
	int node_num;
	int offset;

//	int mem_alloc;
} list_t;

#define LIST_FORWARD 	0
#define LIST_BACKWARD 	1

#ifdef __cplusplus
extern "C"{
#endif

/* 功能:遍历链表,对链表进行list_node_action操作
 * data: 为链表中控制的数据, arg:用户传入的数据 
 * 返回值:
 * 1: 找到满足条件的数据, 根据该值停止遍历
 * 0: 没有找到满足条件的数据,继续遍历
 */
typedef int (*list_node_action)(void *data, void *arg);

#define declare_list_node \
	list_node_t node

int _list_init(list_t * list, int offset);

// return int
#define list_init(list, data_type) \
	_list_init(list, offset_of(typeof(data_type), node))

void list_clean(list_t *list);

/*add to the list head */
int list_push(list_t * list, void *add_node);
/*get the list head and del it from list*/
void *list_pop(list_t * list);
void *g_list_pop(list_t * list);

/*add to the list tail*/
int list_enqueue(list_t * list, void *add_node);
#define list_dequeue g_list_pop

/*return the data at pos*/
void *list_at(list_t * list, int pos);
/*return the data removed*/
void *list_remove(list_t * list, void *rm_data);
/*return the data at pos and remove it*/
void *list_remove_at(list_t * list, int pos);
/* replace ori_data with replace_data */
int list_replace(list_t *list, void *ori_data, void *replace_data);

/*return : success : 1; fail: 0
 * */
int list_insert_at(list_t * list, int pos, void *insert_data);

/*return: succecc: 1, fail:0*/
int list_insert_before(list_t *list, void *data, void *insert_data);
/* return: 1: success, 0: fail*/
int list_insert_after(list_t *list, void *data, void *insert_data); 

/* 遍历链表, 执行用户定义的action操作, 返回满足条件的节点
 * return: success: 满足条件的节点地址
 *   fail: NULL
 *   arg : direction: 
		FOREWARD: from head to tail 
		BACKWARD: from tail to head
 * */
void * list_iterate(list_t *list, int direction, 
		list_node_action action, void *arg);
/*clear the list*/
void list_flush(list_t * list);

#ifdef __cplusplus
}
#endif
static inline void *get_list_head(list_t * list);
static inline void *get_next_node(list_t * list, void *node);

static inline void *get_list_tail(list_t * list);
static inline void *get_prev_node(list_t * list, void *node);

static inline int list_count(list_t * list);
static inline char is_list_empty(list_t * list);

static inline void *get_list_head(list_t * list)
{
	return ((list->head != NULL)
			? (((char *)(list->head)) - list->offset)
			: NULL);
}

static inline void *get_list_tail(list_t * list)
{
	return ((list->tail != NULL)
			? (((char *)(list->tail)) - list->offset) 
			: NULL);
}

static inline void *get_next_node(list_t * list, void *node)
{
	if(unlikely(NULL == node)) {
		return NULL;
	}

	list_node_t *p_tag = (list_node_t *) ((char *)node + list->offset);

	return (((p_tag->next) != NULL)
			? ((char *) (p_tag->next) - list->offset)
			: NULL);
}

static inline void *get_prev_node(list_t * list, void *node)
{
	if (unlikely(NULL == node)) {
		return NULL;
	}

	list_node_t *p_tag = (list_node_t *) ((char *) node + list->offset);

	return (((p_tag->prev) != NULL)
			? ((char *) (p_tag->prev) - list->offset)
			: NULL);
}

static inline int list_count(list_t * list)
{
	return (list->node_num);
}

/*judge whether the list is empty or not*/
static inline char is_list_empty(list_t * list)
{
	return ((0 == list->node_num)? 1 : 0);
}

#ifndef offset_of
#define offset_of(str, member) ((char *)(&((str *)0)->member) - (char *)0)
#endif

#endif
