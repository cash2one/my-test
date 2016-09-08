#ifndef __UTAF_DDOS_COUNT_H__
#define __UTAF_DDOS_COUNT_H__
#include "list.h"
#include "rwlock.h"
/* detect cycle */
#define DDOS_STATISTIC_DETECT_LIST_MAX  (g_ddos_conf.detect_interval)
/* detect thread number */
#define DDOS_DETECT_THREAD_NUM  (g_ddos_conf.detect_thread_num)

typedef struct ddos_iplist_bucket_tag_s
{
	struct hlist_head head; /* 链表头 */
    //spinlock_t lock;
	rwlock_t lock;
}ddos_iplist_bucket_t;

/* 统计list，每1s统计 */
typedef struct total_data_list_st
{
	struct hlist_node list;
	uint32_t ip;
	uint32_t hash;
	#define DDOS_STATE_ADD_DETECT 0x01
	#define DDOS_STATE_ATTACK     0x02
	uint8_t flag;
	uint8_t finish_times;
	uint32_t attack_type;
	struct { 
		uint64_t conn_new[flow_dir];
		uint64_t conn_new_old[flow_dir];
		uint64_t conn_close[flow_dir];
	}tcp;
	ddos_group_counter_t stTotalData;
}total_data_list;

struct ddos_stat_empty_para
{
	uint32_t tid;
	int index;
};

typedef struct ddos_detect_head
{
	struct hlist_head head;
	rwlock_t lock;
	uint8_t change;
}ddos_detect_head_t;

typedef struct ddos_detect_node
{
	struct hlist_node list;
	time_t last_detect_time;
	void (*free_func)(void *);
	total_data_list *stat_node;
}ddos_detect_node_t;


#endif
