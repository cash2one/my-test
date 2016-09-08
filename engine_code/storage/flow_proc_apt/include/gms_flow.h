/**
 * @file gms_flow.h
 * @brief  流量存储模块结构定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-01
 */
#ifndef _GMS_FLOW_H_
#define _GMS_FLOW_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "config.h"
#include "flowdb2.h"
#include "flowrec.h"
#include "gms_psql.h"
#include "gms_redis.h"
#include "flow_config.h"
#include "flow_debug.h"
#include "dirwalk.h"
#include "store_file_op.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "make_log.h"

#define NEED_FLOW_TIME_SEC				(60)	
#define POSTGRESQL_DILIMIT				"|"
#define POSTGRESQL_DILIMIT_SIZ			(2)

#define LOG_MODULE                      "storage"
#define LOG_PROC                        "flow"
#define NUM                             15000
#define DEFAULT_FLOW_TIME               "19730101000000"

#define WRONG_FLOW_FILE_PATH			"/filedata/gms/db/wrong/flow/"
#define EVENT_FLOW_FILE_PATH			"/filedata/gms/db/flow"

#define FLOW_EVENT_BUF_SIZE				(1024*1024*10)

#define NIPQUAD_FMT "%u.%u.%u.%u"
#define NIPQUAD(addr) \
	 ((unsigned char*)&addr)[0],\
	 ((unsigned char*)&addr)[1],\
	 ((unsigned char*)&addr)[2],\
	 ((unsigned char*)&addr)[3]

enum flow_threads_no {
	THREAD_FLOW_MAKE=0,
	THREAD_FLOW_STORE ,

	THREAD_FLOW_NUM
};

void show_flow_config(flow_conf_t* conf, flow_debug_info_t* debug);
int flow_store_entry(void);

/* -------------------------------------------*/
/**
 * @brief  数据库连接句柄 和 需要处理的数据文件列表
 */
/* -------------------------------------------*/
typedef struct _flow_conn {
	PGconn *psql_conn;					/* PostgreSQL 链接句柄 */
	redisContext* redis_conn;
	list_t* list;
	list_t* policy_list;

	flowdb2_t *flowdb;				/* flowdb 链接句柄 */
	flowquery_t *flow_query;
	flowqueryops_t qopt;
	flowqueryattr_t qattr;

	/* 时间条件 */
	char flow_condition[64];
	time_t time_slot;

	unsigned char has_data;
	unsigned int data_num;
	/* 生成的流量数据文件 */
	int flow_fd;
	char flow_file[FILENAME_MAX];		/* 生成完毕的文件名 */
	char flow_tmp_file[FILENAME_MAX];	/* 生成中的文件名 */
	time_t flow_time;					/* 生成文件的时间 */

} flow_conn_t;


/* -------------------------------------------*/
/**
 * @brief  流量根据策略统计节点
 */
/* -------------------------------------------*/
typedef struct _flow_statistical_node {

	/* 流量策略 1-协议筛选 2.ip段筛选 */
	unsigned char policy_type;

	/* 协议 当 policy_type == 0 时可用 */
	unsigned char protocol_type;

	/* IP 当 policy_type == 1 时可用 */
	char start_ip_str[IP_STR_SIZE];
	char end_ip_str[IP_STR_SIZE];

	/* 是否实时呈现 (是否入redis库) */
	unsigned char is_display;

	/* redis 表名 */
	char redis_tname[KEY_NAME_SIZ];

	/* 统计时间段 */
	time_t time_slot;

	/* 统计数据包总数  */
	unsigned long packets;

	/* 统计字节流总数 */
	unsigned long bytes;

	/* 是否动态添加 1 是 0 否*/
	unsigned char dynamic;


	declare_list_node;
} f_statistical_t;

typedef struct _flow_redis_key {
	char key[KEY_NAME_SIZ];
	char exist;				//该key是否在策略配置文件中

	declare_list_node;
} f_redis_key_t;

typedef struct _check_store_arg {
	redisContext* redis_conn;
	char *line_head;
	char *protocol_type;
} cs_arg_t;

extern list_t g_f_policy_list;
extern pthread_mutex_t g_f_policy_list_mutex;
void init_statistics_list(list_t *list);
void clear_all_statistics_list(list_t *list);

void *flow_store_busi(void *arg);
void *flow_make_busi(void *arg);


#endif

