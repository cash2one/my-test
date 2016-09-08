/*
 * $Id: task_queue.h,v 1.2 2009/09/18 02:39:12 dengwei Exp $
 */
#ifndef _TASK_QUEUE_H
#define _TASK_QUEUE_H
#include <sys/ipc.h>
#include <sys/shm.h>
#include "misc.h"

#define MAX_WRITER_NUM	1
#define MAX_READER_NUM	16
#define MAX_WORKER_NUM	32

#define MAX_TASK_NUM	100000

/* task struct */
typedef struct task_struct {
	int shmid;
	int worker_id;
	long packet_offset;
	int packet_len;
} task_t;

// 每段共享内存的进展信息。动态
typedef struct _table_msg {
	int tq_occupied;	// 是否占用字段，初始化为0，占用的话为 -1；
	pid_t tq_pid;		// pid of mt
//      long    tq_total_num;   // 当前占用的节点
	int tq_start;
	int tq_length;		// 各段共享内存的长度
	int tq_writer;		// 各段共享内存中写的位置，就是原来的 tq_tail
	int tq_reader;		// mt 的读取位置。当dt启动时，到该部分搜索已经读取
	// 到什么位置。停止dt时，再将读取位置保存到该变量
	// 中。初始化为 0。
} table_msg;

/* task queue struct */
typedef struct task_queue_struct {
	int tq_semid;
	int tq_ready;
	int tq_occupied;
	int tq_tail;
	int tq_writer_num;
	int tq_reader_num;	// reader num, not more than task_num
	int tq_task_nmax;
	int tq_shmid;		/* shmid >=0, means task queue is placed in shmmem, or else in process */
	// MAX_WORKER_NUM为 32 ，表示 worker 的最大个数
	struct _table_msg tq_table_msg[MAX_WORKER_NUM];
	int task_num;		// total task number
	task_t tq_task_table[1];
} tskque_t;

typedef struct task_queue_reader_struct {
	int tqr_index;
} tskque_reader_t;

typedef struct task_queue_writer_struct {
	int tqw_index;
} tskque_writer_t;

#define offset_of(str, member)  ((char *)(&((str *)0)->member) - (char *)0)

#define INCREASE_TSKQUE_INDEX(q, idx)	do {\
						idx = (idx + 1) % (q)->tq_task_nmax;\
					} while (0)
#define	INCREASE_TSKQUE_READER_INDEX(q, reader)	INCREASE_TSKQUE_INDEX((q), (reader)->tqr_index)
#define	INCREASE_TSKQUE_WRITER_INDEX(q, writer)	INCREASE_TSKQUE_INDEX((q), (writer)->tqw_index)

#define INCREASE_TSKQUE_INDEX_NEW(q, idx)	do {\
							if ((q)->tq_length != 0)\
								(idx) = ((idx) + 1) % (q)->tq_length;\
						} while (0)
#define	INCREASE_TSKQUE_READER_INDEX_NEW(q, reader)	INCREASE_TSKQUE_INDEX_NEW((q), (reader)->tqr_index)
#define	INCREASE_TSKQUE_WRITER_INDEX_NEW(q, writer)	INCREASE_TSKQUE_INDEX_NEW((q), (writer)->tqw_index)

#define INCREASE_TABLEMSG_INDEX_NEW(q, idx)	do {\
							if ((q)->tq_length != 0)\
								(q)->idx = (((q)->idx) + 1) % ((q)->tq_length);\
						} while (0)

/* task queue */
tskque_t *tskque_new(int nmax, key_t key);
void tskque_free(tskque_t * tskque);
tskque_t *tskque_open(key_t key);
void tskque_close(tskque_t * que);

void tskque_recondition(tskque_t * que, int task_num);

/* writer */
tskque_writer_t *tskque_writer_new(tskque_t * tskque);
void tskque_writer_free(tskque_t * tskque, tskque_writer_t * writer);
inline task_t *tskque_writer_getslot(tskque_t * que, tskque_writer_t * writer);
inline int tskque_writer_putstuff(tskque_t * que, tskque_writer_t * writer);

//inline task_t *tskque_writer_getslot_new(volatile tskque_t *que, int para_worker_id);
//inline int tskque_writer_putstuff_new(volatile tskque_t *que, int para_worker_id);

inline task_t *tskque_writer_getslot_new(task_t * task_start,
					 volatile table_msg * p_table);
inline int tskque_writer_putstuff_new(volatile table_msg * p_table);

/* reader */
tskque_reader_t *tskque_reader_new(tskque_t * tskque);
void tskque_reader_free(tskque_t * tskque, tskque_reader_t * reader);
inline task_t *tskque_reader_getstuff(volatile tskque_t * que,
				      volatile tskque_reader_t * reader);
inline int tskque_reader_putslot(tskque_t * que, tskque_reader_t * reader);

inline task_t *tskque_reader_getstuff_new(task_t * task_start,
					  volatile int *tq_writer,
					  volatile tskque_reader_t * reader);
//inline task_t *tskque_reader_getstuff_new(volatile tskque_t *que, volatile table_msg *p_table, volatile tskque_reader_t *reader);

inline int tskque_reader_putslot_new(volatile table_msg * p_table,
				     volatile tskque_reader_t * reader);
void tskque_reader_free_new(tskque_t * tskque, int worker_id,
			    tskque_reader_t * reader, int reader_added);

/* add by dengwei */
int tskque_get_workerid(tskque_t * tskque);
table_msg *tskque_get_worker_table(tskque_t * tskque, int para_worker_id);
void tskque_get_reader(table_msg * p_table, tskque_reader_t * reader);

void print_task(task_t * task);
void print_table(table_msg * p_table);
void print_all_table(tskque_t * que);
void print_que_head(tskque_t * que);

#endif				//_TASK_QUEUE_H
