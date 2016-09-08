#include <stdlib.h>
#include <stdio.h>

#include "list.h"

//#define DEBUG_LIST

static list_node_t *_list_node_remove(list_t * list, list_node_t * node);
static list_node_t *_locate_list(list_t * list, int pos);
static void _list_node_push(list_t *list, list_node_t *node);
static void _list_node_enqueue(list_t *list, list_node_t *node);
static void _list_insert_node_at(list_t *list, int pos, list_node_t *node);
static void _list_insert_node_before(list_t *list, 
			list_node_t *list_node, list_node_t *insert_node);
static void _list_insert_node_after(list_t *list,
			list_node_t *list_node, list_node_t *insert_node);

#define INSERT_BEFORE(list_node, insert_node)\
do {\
	insert_node->prev = list_node->prev;\
	(list_node->prev)->next = insert_node;\
	insert_node->next = list_node;\
	list_node->prev = insert_node;\
}while(0)

#define INSERT_AFTER(list_node, insert_node)\
do {\
	insert_node->next = list_node->next;\
	(list_node->next)->prev = insert_node;\
	insert_node->prev = list_node;\
	list_node->next = insert_node;\
}while(0)

int _list_init(list_t * list, int offset)
{
//	if (list == NULL) {
//		list = calloc(1, sizeof(list_t));
//		if (list == NULL) {
//			return -1;
//		}
//		
//		list->mem_alloc = 1;
//	} else {
//		list->mem_alloc = 0;
//	}

	list->offset = offset;
	list->head = NULL;
	list->tail = NULL;
	list->node_num = 0;

	return 0;
}

void list_clean(list_t *list)
{
//	if (list->mem_alloc) {
//		free(list);
//	}
}

/* insert before head */
int list_push(list_t * list, void *add_node)
{
	list_node_t *list_node = (list_node_t *) ((char *) add_node + list->offset);
	_list_node_push(list, list_node);

	return 0;
}

/* insert after tail */
int list_enqueue(list_t * list, void *add_node)
{
	list_node_t *list_node = (list_node_t *) ((char *) add_node + list->offset);
	_list_node_enqueue(list, list_node);

	return 0;
}

/*
 * del the list head
 * return: 
 * 	node address;
 * NULL: 
 * 	no node in list
 */
/* pop at head */

void *list_pop(list_t * list)
{
	if (unlikely(list->node_num == 0)) {
		return NULL;
	}

	list_node_t *ret_val = _list_node_remove(list, list->head);

	return ((char *) ret_val - list->offset);
}
void *g_list_pop(list_t * list)
{
	if (unlikely(list->node_num == 0)) {
		return NULL;
	}

	list_node_t *ret_val = _list_node_remove(list, list->head);

	return ((char *) ret_val - list->offset);
}

/*del the list tail*/
#define list_dequeue g_list_pop

int list_insert_at(list_t * list, int pos, void *insert_data)
{
	if (unlikely(pos < 0)) {
#ifdef DEBUG_LIST
		printf("the pos is %d (< 0)\n", pos);
#endif
		return 0;
	}

	list_node_t *insert_node = (list_node_t *) ((char *) insert_data + list->offset);
	_list_insert_node_at(list, pos, insert_node);	

	return 1;
}

int list_insert_before(list_t *list, void *data, void *insert_data) 
{
	if (unlikely(NULL == data)) {
		return -1;
	}

	if (data == NULL) {
		// insert tail
		list_enqueue(list, insert_data);
	} else {
		list_node_t *list_node = (list_node_t *)((char *)data + list->offset);
		list_node_t *insert_node = (list_node_t *)((char *)insert_data + list->offset);
		_list_insert_node_before(list, list_node, insert_node);
	}

	return 0;
}

int list_insert_after(list_t *list, void *data, void *insert_data) 
{
	if (unlikely(NULL == data)) {
		return -1;
	}

	if (data == NULL) {
		// insert head
		list_push(list, insert_data);
	} else {
		list_node_t *list_node = (list_node_t *)((char *)data + list->offset);
		list_node_t *insert_node = (list_node_t *)((char *)insert_data + list->offset);
		_list_insert_node_after(list, list_node, insert_node);
	}

	return 0;
}

void *list_at(list_t * list, int pos)
{
	if (unlikely(0 == list->node_num || pos < 0)) {
#ifdef DEBUG_LIST
		printf("list at pos is < 0, pos = %x\n", pos);
		printf("list->node_num = %d\n", list->node_num);
#endif
		return NULL;
	}
	
	list_node_t *node = NULL;
	node = _locate_list(list, pos);
	if (NULL == node) {
		node = list->tail;
	}

	return ((char *) node - list->offset);
}

void *list_remove(list_t * list, void *rm_data)
{
	if (unlikely(0 == list->node_num || NULL == rm_data)) {
#ifdef DEBUG_LIST
		printf("the remove data is null\n");
		printf("list->node_num = %d\n", list->node_num);
#endif
		return NULL;
	}

	list_node_t *list_node = (list_node_t *) ((char *) rm_data + list->offset);
	_list_node_remove(list, list_node);

	return rm_data;
}

void *list_remove_at(list_t * list, int pos)
{
	if (unlikely(0 == list->node_num || pos < 0)) {
#ifdef DEBUG_LIST
		printf("list remove at pos = %d\n(< 0)", pos);
		printf("list->node_num = %d\n", list->node_num);
#endif
		return NULL;
	}

	list_node_t *node = _locate_list(list, pos);

	if (NULL == node) {
		return NULL;
	}

	_list_node_remove(list, node);

	return ((char *) node - list->offset);
}

static int _list_replace(list_t *list, list_node_t *ori_node, list_node_t *replace_node)
{
	memcpy(replace_node, ori_node, sizeof(list_node_t));
	if (ori_node->prev != NULL) {
		(ori_node->prev)->next = replace_node;
	} else {
		// head
		list->head = replace_node;
	}

	if (ori_node->next != NULL) {
		(ori_node->next)->prev = replace_node;
	} else {
		// tail
		list->tail = replace_node;
	}

	return 0;
}

int list_replace(list_t *list, void *ori_data, void *replace_data)
{
	if (unlikely(ori_data == NULL || replace_data == NULL)) {
		return -1;
	}

	list_node_t *ori_node = (list_node_t *) ((char *) ori_data + list->offset);
	list_node_t *replace_node = (list_node_t *) ((char *) replace_data + list->offset);
	
	return (_list_replace(list, ori_node, replace_node));
}

/*
 * direction: 0 : from head to tail
 * 	      1 : from tail to head
 * */
void * list_iterate(list_t *list, int direction, 
		list_node_action action, void *arg)
{
	if(unlikely(0 == list->node_num || NULL == action)) {
		return NULL;
	}

	list_node_t *list_node = NULL;
	void *data = NULL;

	if(LIST_FORWARD == direction) {
		list_node = list->head;
	} else {
		list_node = list->tail;
	}

	if(unlikely(NULL == list_node)) {
		return NULL;
	}
	
	while(list_node != NULL) {
		data = (char *)list_node - list->offset;
		if (action(data, arg)) {
			return data;
		}

		if (LIST_FORWARD == direction) {
			list_node = list_node->next;
		} else {
			list_node = list_node->prev;
		}

	} 

	return NULL;
}

/*clear the list*/
void list_flush(list_t * list)
{
	list->head = NULL;
	list->tail = NULL;
	list->node_num = 0;
}


static list_node_t *_list_node_remove(list_t * list, list_node_t * node)
{
	if (1 == list->node_num) {	//only one node
		if (node == list->head) {
			list->tail = NULL;
			list->head = NULL;
		} else {
		#ifdef DEBUG_LIST
			printf("the node is not in list\n");
		#endif
			return NULL;
		}
	} else {
		if (NULL == node->next) { // the tail node
			(list->tail) = (list->tail)->prev;
			(list->tail)->next = NULL;
		} else if (NULL == node->prev) { // the head node
			(list->head) = (list->head)->next;
			(list->head)->prev = NULL;
		} else { // the middle mode
			(node->next)->prev = node->prev;
			(node->prev)->next = node->next;
		}
	}

	list->node_num--;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

static list_node_t *_locate_list(list_t * list, int pos)
{
	list_node_t *p_tag = NULL;

	if (pos < ((list->node_num) >> 1)) {
		p_tag = list->head;
		while ((p_tag != NULL) && pos--) {
			p_tag = p_tag->next;
		}
	} else {
		if (pos >= list->node_num) {
#ifdef DEBUG_LIST
			printf("the pos is over the list node number\n");
#endif
			return NULL;
		}

		p_tag = list->tail;
		pos = list->node_num - pos - 1;
		while ((p_tag != NULL) && pos--) {
			p_tag = p_tag->prev;
		}
	}

	return p_tag;
}


static void _list_node_push(list_t *list, list_node_t *node)
{
	node->next = (list->head);
	node->prev = NULL;

	if (list->node_num != 0) {
		(list->head)->prev = node;
		list->head = node;
	} else {
		list->head = list->tail = node; // add the first node
	}

	(list->node_num)++;
}


static void _list_node_enqueue(list_t *list, list_node_t *node)
{
	node->prev = list->tail;
	node->next = NULL;

	if (list->node_num != 0) {
		(list->tail)->next = node;
		list->tail = node;
	} else {
		list->head = list->tail = node;
	}

	(list->node_num)++;
}



static void _list_insert_node_at(list_t *list, int pos, list_node_t *node)
{
	list_node_t *p_tag = NULL;

	node->next = node->prev = NULL;
	if (unlikely(0 == list->node_num)) { //add the first node
		list->head = list->tail = node;
	} else {
		if (0 == pos) { // add to head
			_list_node_push(list, node);
			return;
		}

		p_tag = _locate_list(list, pos);
		
		if (NULL == p_tag) { // add to tail
			_list_node_enqueue(list, node);
			return;
		}
		INSERT_BEFORE(p_tag, node);
	}

	(list->node_num)++;
	return;
}


static void _list_insert_node_before(list_t *list,
		list_node_t *list_node, list_node_t *insert_node)
{
	insert_node->next = insert_node->prev = NULL;

	if (NULL == list_node->prev) { // the head
		_list_node_push(list, insert_node);
		return;
	}

	INSERT_BEFORE(list_node, insert_node);
	(list->node_num)++;
}

static void _list_insert_node_after(list_t *list,
		list_node_t *list_node, list_node_t *insert_node)
{
	insert_node->next = insert_node->prev = NULL;

	if (NULL == list_node->next) { // the tail 
		_list_node_enqueue(list, insert_node);
		return;
	}

	INSERT_AFTER(list_node, insert_node);
	list->node_num++;
}

#if 0
typedef struct {
	int m;
	int n;
	declare_list_node;
} user_data_t;

void print_data(list_t *list)
{
	user_data_t *node = get_list_head(list);
	while (node != NULL) {
		printf("************\n");
		printf("m = %d\n", node->m);
		printf("n = %d\n", node->n);
		node = get_next_node(list, node);
	}
}
list_t list;
int32_t print(void *data, void *arg)
{
	if (((user_data_t *)data)->m == ((user_data_t *)arg)->m) {
		return 1;
	}	
	printf("m = %d, n = %d\n", ((user_data_t *)data)->m, ((user_data_t *)data)->n);
	return 0;
}
int main()
{
#define INTERATE
//#define DEBUG_POP
//#define DEBUG_REMOVE
//#define DEBUG_POP_PUSH
	user_data_t *add_node = NULL;

	list_init(&list, user_data_t);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 100;
	add_node->n = 100;
	//list_enqueue(&list, add_node);
	list_push(&list, add_node);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 200;
	add_node->n = 200;
	//list_enqueue(&list, add_node);
	list_push(&list, add_node);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 300;
	add_node->n = 300;
	list_enqueue(&list, add_node);
	//list_push(&list, add_node);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 400;
	add_node->n = 400;
	list_enqueue(&list, add_node);
	//list_push(&list, add_node);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 500;
	add_node->n = 500;
	//list_insert_at(&list, 0, add_node);
	//list_insert_at(&list, 1, add_node);
#ifdef INTERATE
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 500;
	add_node->n = 30;
	//list_remove_at(&list, 0);
	user_data_t *get_data = list_iterate(&list, 1, print, add_node);
	if (NULL == get_data) {
		printf("get_data = NULL\n");
	}
	if(get_data != NULL) {
	printf("get_data = m = %d, n = %d\n", get_data->m, get_data->n);
	}
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 800;
	add_node->n = 800;
	//list_insert_before(&list, get_data, add_node);
	list_insert_after(&list, get_data, add_node);
	
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 600;
	add_node->n = 600;
	list_insert_before(&list, get_data, add_node);
	print_data(&list);  
#endif

#ifdef DEBUG_POP
	int32_t i = 0;
	user_data_t *pop_node = NULL;
	for (i = 0; i < list.node_num; i++) {
		printf("node_num = %d\n", list.node_num);
		//      pop_node = list_at(&list, i);
		//list_remove_at(&list, i);
		printf("m = %d\n", pop_node->m);
		//printf("n = %d\n", pop_node->n);
		//      list_remove(&list, pop_node);
	}
//              list_remove_at(&list, 6);
	while (1) {
		printf("***********pop 1************\n");
		pop_node = g_list_pop(&list);
		if (NULL == pop_node)
			break;
		printf("m = %d\n", pop_node->m);
		printf("n = %d\n", pop_node->n);
	}
#endif

#ifdef DEBUG_REMOVE
	pop_node = get_list_tail(&list);
	int32_t num = 0;

	while (pop_node) {
		printf("node location = %d\n", num);
		printf("m = %d\n", pop_node->m);
		printf("n = %d\n", pop_node->n);
		num++;
		pop_node = get_prev_node(&list, pop_node);
	}
	/*
	   for(i = 0; i < list.node_num; i++) {
	   printf("node_num = %d\n",list.node_num);
	   pop_node = list_at(&list, i);
	   //list_remove(&list, pop_node);
	   //   list_remove_at(&list, i);
	   printf("m = %d\n", pop_node->m);
	   printf("n = %d\n", pop_node->n);
	   }
	 */
	//list_remove_at(&list, 2);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 800;
	add_node->n = 800;
	//list_insert_at(&list, 3, add_node);
	for (i = 0; i < list.node_num; i++) {
		printf("node_num = %d\n", list.node_num);
		pop_node = list_at(&list, i);
		list_remove(&list, pop_node);
		//      list_remove_at(&list, i);
		printf("m = %d\n", pop_node->m);
		printf("n = %d\n", pop_node->n);
	}
	pop_node = list_remove_at(&list, -1);
	//printf("remove node m = %d, n = %d\n", pop_node->m, pop_node->n);

#endif

#ifdef DEBUG_POP_PUSH

	pop_node = g_list_pop(&list);
	pop_node = g_list_pop(&list);
	pop_node = g_list_pop(&list);
	add_node = calloc(1, sizeof(user_data_t));
	add_node->m = 800;
	add_node->n = 800;
	list_push(&list, add_node);
	printf("node num = %d\n", list.node_num);
	for (i = 0; i < list.node_num; i++) {
		printf("node_num = %d\n", list.node_num);
		pop_node = list_at(&list, i);
		//list_remove(&list, pop_node);
		//      list_remove_at(&list, i);
		printf("m = %d\n", pop_node->m);
		printf("n = %d\n", pop_node->n);
	}
#endif

	return 0;
}
#endif
