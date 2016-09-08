/*************************************************************************
  > File Name: log_out.h
  > Author: xuziquan
  > Mail: xuziquan@chanct.com 
  > Created Time: Thu 20 Aug 2015 09:57:33 AM ICT
 ************************************************************************/
#ifndef __DDOS_LOG_OUT_H__
#define __DDOS_LOG_OUT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include "spinlock.h"
#include "ddos.h"
#include "atomic.h"
#include "utaf_ddos_count.h"
#include "list.h"
#include "utaf_session.h"
#include "ip4.h"
#include "app.h"
#include "ddos_psql.h"
#include "misc.h"
#include "iplib.h"
#include "ddos_log_out_rbtree.h"
#define TOPNUM 100
#define TOP_OUTPUT_INTERVAL 300 //output top sip interval 300 seconds
#define LIST_MOVE_INTERVAL 5
#define LOG_OUT_THREAD_NUM 5
#define DEBUG_OUT_LOG 1
//#define RBTREE 1
#define SIP_TREE_MAX_DEEP 12 //max depth of rbtree
#endif


#define INIT_LOG_HLIST_HEAD(ptr) (ptr = NULL)
#define DDOS_MIN(x, y) ((x < y) ? (x) : (y))

//typedef struct {
//	volatile int locked; /**< lock status 0 = unlocked, 1 = locked */
//} spinlock_t;

static pthread_mutex_t min_store_lock;


typedef struct ddos_log_empty_para
{
	int index;
	int tid;
	PGconn *ddos_conn;
}ddos_log_empty_para;

typedef struct log_hash_list_head{
	//struct node_destIP_outddos_data *head; /* 链表头 */
	struct rbtree_node_destIP_outddos_data *head; /* 链表头 */
    spinlock_t lock;
}log_hash_list_head;

typedef struct attck_detail
{
	uint64_t tcp_in_packets;
	uint64_t tcp_out_packets;
	uint64_t tcp_in_bytes;
	uint64_t tcp_out_bytes;

	// Additional details about one of most popular atatck type
	uint64_t tcp_syn_in_packets;
	uint64_t tcp_syn_out_packets;
	uint64_t tcp_syn_in_bytes;
	uint64_t tcp_syn_out_bytes;

	uint64_t tcp_ack_in_packets;
	uint64_t tcp_ack_out_packets;
	uint64_t tcp_ack_in_bytes;
	uint64_t tcp_ack_out_bytes;
/*
	int64_t tcp_new_num_ps;
	int64_t tcp_live_num_ps;
*/
	int64_t tcp_new_old_first;
	//int64_t tcp_new_old;
	int64_t tcp_new;
	int64_t tcp_close;

	uint64_t icmp_in_packets;
	uint64_t icmp_out_packets;
	uint64_t icmp_in_bytes;
	uint64_t icmp_out_bytes;

	uint64_t dns_in_packets;
	uint64_t dns_out_packets;
	uint64_t dns_in_bytes;
	uint64_t dns_out_bytes;
	//
	uint64_t ntp_in_packets;
	uint64_t ntp_out_packets;
	uint64_t ntp_in_bytes;
	uint64_t ntp_out_bytes;

	uint64_t ssdp_in_packets;
	uint64_t ssdp_out_packets;
	uint64_t ssdp_in_bytes;
	uint64_t ssdp_out_bytes;

	uint64_t snmp_in_packets;
	uint64_t snmp_out_packets;
	uint64_t snmp_in_bytes;
	uint64_t snmp_out_bytes;

	uint64_t chargen_in_packets;
	uint64_t chargen_out_packets;
	uint64_t chargen_in_bytes;
	uint64_t chargen_out_bytes;

	uint64_t flow_in_packets;
	uint64_t flow_out_packets;
	uint64_t flow_in_bytes;
	uint64_t flow_out_bytes;
}attck_detail; 


typedef struct node_sip_outddos_data{
	uint32_t unit_id;//id of linking sip output and dip out put
	uint32_t sip;
	time_t start_time;
	time_t end_time;

	uint32_t attack_id;
//	uint32_t query_id;

	uint32_t s_country_id;
	uint32_t s_city_id;
	float s_latitude;
	float s_longitude;

	uint32_t dip;
	uint32_t d_country_id;
	uint32_t d_city_id;
	float d_latitude;
	float d_longitude;

	struct attck_detail sip_attack_detail;

	struct node_sip_outddos_data *left_child;
	struct node_sip_outddos_data *right_child;
	struct node_sip_outddos_data *parent;

}node_sip_outddos_data;

typedef struct node_curr_top100{
	time_t start_time;
	time_t end_time;

	uint32_t attack_id;
	uint32_t sip;
	uint32_t s_country_id;
	uint32_t s_city_id;
	float s_latitude;
	float s_longitude;
	uint32_t dip;
	uint32_t d_country_id;
	uint32_t d_city_id;
	float d_latitude;
	float d_longitude;

//	uint32_t ddos_type;

	struct attck_detail curr_top_detail;

	uint64_t flow_bps;
	uint64_t flow_pps;

}node_curr_top100;

/*typedef struct node_destIP_outddos_data{
	uint32_t unit_id;//id of linking sip output and dip out put
	time_t start_time;
	time_t end_time;
	//Uint32 sip;
	uint32_t dip;
	uint32_t ddos_type;
	uint32_t sip_node_count;
	
	struct attck_detail dip_attack_detail;

	node_sip_outddos_data *sourceip_tree_root;

	node_sip_outddos_data *curr_top_ptr[TOPNUM]; 
	struct node_curr_top100 curr_top[TOPNUM];
	struct node_curr_top100 att_top[TOPNUM];

	struct node_destIP_outddos_data* prev;
	struct node_destIP_outddos_data* next;

}node_destIP_outddos_data;*/

typedef struct node_destIP_outddos_data{
//	uint32_t unit_id;
	uint32_t sip_node_count;
	uint32_t dip;
	
	uint32_t curr_top_num;
	uint32_t att_top_num;

	uint32_t query_id;//id of linking sip output and dip out put
	uint32_t tm_index;
	
//	struct attck_detail dip_attack_detail;
//	uint64_t dev_id;
	time_t start_time;
	time_t end_time;

	uint64_t flow_bps;
	uint64_t flow_pps; 
	uint32_t attack_id;
	

	node_sip_outddos_data *sourceip_tree_root;
	pthread_rwlock_t rwlock;

	node_sip_outddos_data *curr_top_ptr[TOPNUM]; 
	struct node_curr_top100 curr_top[TOPNUM];
	struct node_curr_top100 att_top[TOPNUM];

	struct node_destIP_outddos_data* prev;
	struct node_destIP_outddos_data* next;

}node_destIP_outddos_data;
/*typedef struct node_list_head{
	node_destIP_outddos_data *head;
}node_list_head;*/

typedef struct rbtree_node_sip_outddos_data{
	uint32_t unit_id;//id of linking sip output and dip out put
	uint32_t sip;
	time_t start_time;
	time_t end_time;

	uint32_t attack_id;
//	uint32_t query_id;

	uint32_t s_country_id;
	uint32_t s_city_id;
	float s_latitude;
	float s_longitude;

	uint32_t dip;
	uint32_t d_country_id;
	uint32_t d_city_id;
	float d_latitude;
	float d_longitude;

	struct attck_detail sip_attack_detail;
    struct rb_node rbtree_node; //used for rbtree

/*	struct node_sip_outddos_data *left_child;
	struct node_sip_outddos_data *right_child;
	struct node_sip_outddos_data *parent;*/
}rbtree_node_sip_outddos_data;

typedef struct rbtree_node_destIP_outddos_data{
//	uint32_t unit_id;
	uint32_t sip_node_count;
	uint32_t dip;
	
	uint32_t curr_top_num;
	uint32_t att_top_num;

	uint32_t query_id;//id of linking sip output and dip out put
	uint32_t tm_index;
	
//	struct attck_detail dip_attack_detail;
//	uint64_t dev_id;
	time_t start_time;
	time_t end_time;

	uint64_t flow_bps;
	uint64_t flow_pps; 
	uint32_t attack_id;
	

//	rbtree_node_sip_outddos_data *sip_tree_root;
	struct rb_root rbtree;
	pthread_rwlock_t rwlock;

	rbtree_node_sip_outddos_data *curr_top_ptr[TOPNUM]; 
	struct node_curr_top100 curr_top[TOPNUM];
	struct node_curr_top100 att_top[TOPNUM];

	struct rbtree_node_destIP_outddos_data* prev;
	struct rbtree_node_destIP_outddos_data* next;

}rbtree_node_destIP_outddos_data;
enum {
        TCP_FLOOD = 1,
        TCP_SYN_FLOOD ,
        TCP_ACK_FLOOD ,
        ICMP_FLOOD ,
        CHARGEN_FLOOD ,
        SSDP_FLODD ,
        NTP_FLOOD ,
        DNS_FLOOD ,
        TCP_LINK ,
        TCP_SNMP,
        BLENDED_ATT ,
};

typedef struct  log_tm_list_node{   
//	node_destIP_outddos_data *dip_node;
	rbtree_node_destIP_outddos_data *dip_node;
	struct log_tm_list_node* prev;
	struct log_tm_list_node* next;
}log_tm_list_node;

typedef struct log_list_head{
	log_tm_list_node *head;
	spinlock_t lock;
}log_list_head;

#ifndef MIN_T
#define MIN_T(type,x,y) \
({ \
	type __x = (x); \
	type __y = (y); \
	__x < __y ? __x: __y; \
})
#endif

#ifndef MAX_T
#define MAX_T(type,x,y) \
({ \
	type __x = (x); \
	type __y = (y); \
	__x > __y ? __x: __y; \
})
#endif

extern int traverse_tree(node_destIP_outddos_data* dip_node,
				node_sip_outddos_data* TreeNode,	
				int (*calculate_top_buf)(node_destIP_outddos_data* dip_node,node_sip_outddos_data* TreeNode));

extern int ddos_traverse_tree(node_destIP_outddos_data *node);

extern int calculate_top_buf(node_destIP_outddos_data* dip_node,node_sip_outddos_data* TreeNode);

//extern rbtree_node_destIP_outddos_data *utaf_ddos_log_insert_node(session_item_t* count_data,total_data_list *statis_node,node_destIP_outddos_data **head);
extern rbtree_node_destIP_outddos_data *utaf_ddos_log_insert_node(session_item_t* count_data,total_data_list *statis_node,struct rbtree_node_destIP_outddos_data **head);


extern int insert_node_into_tree(node_destIP_outddos_data* node,session_item_t *count_data,total_data_list *statis_node);

extern int traverse_tree_putin_topbuf(node_sip_outddos_data *curr_top[],uint64_t flow_in_bytes);

//extern int update_attack_top(node_destIP_outddos_data *node);

extern int clean_tree(node_destIP_outddos_data *tm_node_head);

extern int get_sip_sum_detail(node_sip_outddos_data *node,session_item_t *count_data,total_data_list *statis_node);

extern int update_attack_top(rbtree_node_destIP_outddos_data *node);

//extern int insert_att_topbuf(int insert_loc,node_destIP_outddos_data* dip_node,int num);

extern node_sip_outddos_data *create_tree_node(session_item_t *count_data,total_data_list *statis_node);

//ddos_conn.c
extern int ddos_log_ddosip_store(PGconn *conn,rbtree_node_destIP_outddos_data *buf);

extern int ddos_log_top100_5min_store(struct rbtree_node_destIP_outddos_data *buf);
extern uint32_t get_attack_id(uint32_t attack_type);
extern int ddos_log_remove_monitorip(uint32_t dip);

extern rbtree_node_sip_outddos_data *create_rbtree_node(session_item_t *count_data,total_data_list *statis_node);
extern int rbtree_get_sip_sum_detail(rbtree_node_sip_outddos_data *node,session_item_t *count_data,total_data_list *statis_node);
extern int rbtree_insert_curr_topbuf(int insert_loc,int num,rbtree_node_destIP_outddos_data* dip_node,rbtree_node_sip_outddos_data* TreeNode);
extern int rbtree_calculate_top_buf(rbtree_node_destIP_outddos_data* dip_node,rbtree_node_sip_outddos_data* TreeNode);
extern int rbtree_insert_node_into_tree(rbtree_node_destIP_outddos_data* node,session_item_t *count_data,total_data_list *statis_node);
extern int traverse_rbtree(rbtree_node_destIP_outddos_data* dip_node);
extern rbtree_node_destIP_outddos_data *create_rbtree_dip_node(session_item_t *count_data,total_data_list *statis_node);
extern int rbtree_get_dip_sum_detail(rbtree_node_destIP_outddos_data *node,session_item_t *count_data,total_data_list *statis_node);



//extern void flow_disconnect_db(PGconn *conn);

//extern PGconn * flow_connect_db(struct gpq_conn_info g_flow_info_cfg)
