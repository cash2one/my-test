/**
 * @file gms_flow.h
 * @brief  流量存储模块结构定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-01
 */
#ifndef _GMS_FLOW_H_
#define _GMS_FLOW_H_

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "flowstat.h"
#include "flowerr.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "config.h"
#include "flowrec.h"
#include "misc.h"
#include "store_common.h"

#include "gms_psql.h"
#include "gms_redis.h"
#include "flow_config.h"
#include "flow_debug.h"
#include "dirwalk.h"
#include "store_file_op.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "make_log.h"
#include <signal.h>
#include <sys/time.h>

#define SAM_STR_SIZE					256
#include "flow_common.h"

#define BROWSER_SIZE					40
#define NOTE_NUM						10
#define TIME_SIZE						60
#define EVENT_SIZE						300
#define EVENT_FLOW_MAX					(1024*1024)
#define FLOW_SIZE						1000
#define NEED_FLOW_TIME_SEC				(60)	
#define POSTGRESQL_DILIMIT				"|"
#define POSTGRESQL_DILIMIT_SIZ			(2)

#define LOG_MODULE                      "storage"
#define LOG_PROC                        "flow"
#define NUM                             1500
#define DEFAULT_FLOW_TIME               "19730101000000"

#define WRONG_FLOW_FILE_PATH			"/filedata/gms/db/wrong/flow/"
#define EVENT_FLOW_FILE_PATH			"/filedata/gms/db/flow"

#define FLOW_EVENT_BUF_SIZE				(1024*5)
#define EVENT_TABLE_NAME				"t_flow_event"
#define NET_TABLE_NAME					"t_net_audit"
#define TIME_OUT_TABLE_NAME				"t_flow_event"


#define NIPQUAD_FMT "%u.%u.%u.%u"
#define NIPQUAD(addr) \
	 ((unsigned char*)&addr)[0],\
	 ((unsigned char*)&addr)[1],\
	 ((unsigned char*)&addr)[2],\
	 ((unsigned char*)&addr)[3]

enum flow_threads_no {
	THREAD_SOCK_FLOW = 0,
	THREAD_FLOW_BAK_STORE ,
	THREAD_FLOW_STORE,
	THREAD_FLOW_NUM,
	THREAD_EVENT_SOCK,
	THREAD_EVENT_STORE1,
	THREAD_EVENT_STORE2,
	THREAD_EVENT_STORE3,
	THREAD_EVENT_STORE4,
	THREAD_EVENT_BAK_STORE

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
/*flow的字段结构体*/
typedef struct flow_struct{
	uint8_t     protocol_id;        /* 协议id 1 代表ALL 2代表TCP 3代表UDP 4代表OTHERS */
	time_t      ftime;              /* 时间以秒为单位 */
	uint64_t    pps;                /* 收包数 */
	uint64_t    bps;                /* 字节数 */
	uint32_t    start_ip;           /* 除自定义流量外,其他为0 */
	uint32_t    end_ip;             /* 除自定义流量外,其他为0 */
	struct flow_struct *next;
	struct flow_struct *pre;
}*Pflows,flows;
flows queue_head;
typedef struct queue_buf{
	char devid_id[32];
	Pflows head;
	Pflows tail;
	pthread_mutex_t mutex;
	pthread_cond_t cond_noempty;
	pthread_cond_t cond_nofull;
	pthread_cond_t cond_pause;
}*Pmessage,Message;
Pmessage message;

/*flow_event的字段结构体*/
#if 0
typedef struct event_struct{
	time_t ftime_start;					/*开始时间*/
	time_t ftime_end;					/*结束时间*/
	char country[STR_SIZE];				/*所属国家*/
	char province[STR_SIZE];			/*所属省*/
	char county[STR_SIZE];				/*所属县*/
	char isp[SAM_STR_SIZE];				/*所属isp*/
	char browser[SAM_STR_SIZE];			/*浏览器类型，例：IE、chrome 等*/
	char version[SAM_STR_SIZE];			/*浏览器版本*/
	unsigned int  sip;					/*用户ip*/
	unsigned int  usersport;			/*用户源端口号*/
	unsigned int  userdport;			/*用户目的端口号*/
	unsigned int dip;					/*目的ip*/
	short procotol_id;					/*协议id*/
	float visit_time;					/*访问时长*/
	short pc_if;						/*0 为pc端 ；1 为移动端*/
	double ip_byte;						/*字节数*/
	double ip_pkt;						/*数据包*/
	short 		tcp_suc;				/*tcp连接是否成功 0 tcp连接失败，1为连接成功*/
	unsigned int cli_ser;				/*端到端的时延:所有包的平均值*/
	char bank_visit;					/*业务访问是否成功：如：http 返回200认为成功*/
	char flag;							/*1 为长连接 */
	char incr;							/*0为增量日志，1为老化*/ 	
	struct event_struct *next;
	struct event_struct *pre;

}*Pflow_event,flow_event;
typedef struct event_struct{
	time_t          ftime_start;            /* 开始时间，自1970.1.1年以来的秒数 */
	time_t          ftime_end;              /* 结束时间，自1970.1.1年以来的秒数 */
	time_t          visit_time;             /* 访问时长，单位秒 */
	uint32_t        sip;                    /* 用户ip */
	uint32_t        dip;                    /* 目的ip */
	uint16_t        usersport;              /* 用户源端口号 */
	uint16_t        userdport;              /* 目的端口号 */
	uint16_t         procotol_id;            /* 协议id: 2代表TCP 3代表UDP 4代表OTHERS */
	uint8_t         country;                /* 国家 */
	uint8_t         pc_if;                  /* 0 为pc端 ；1 为移动端 */
	uint8_t         tcp_suc;                /* tcp连接是否成功 0 tcp连接失败，1为连接成功 */
	uint8_t         bank_visit;             /* 业务访问是否成功：如：http 返回200认为成功 */
	uint8_t         flag;                   /* 0为非长链接，1 为长连接 */
	uint8_t         incr;                   /* 0表示增量日志，1表示老化 */
	uint16_t        s_isp_id;               /* 源运营商ID */
	uint16_t        d_isp_id;               /* 目的运营商ID */
	uint32_t        s_province_id;          /* 源省ID */
	uint32_t        d_province_id;          /* 目的省ID */
	uint32_t        s_city_id;              /* 源地市ID */
	uint32_t        d_city_id;              /* 目的地市ID */
	uint64_t        ip_byte;                /* 字节数 */
	uint64_t        ip_pkt;                 /* 数据包 */
	time_t          cli_ser;                /* 端到端的时延:所有包的平均值，单位毫秒 */
	int   			browser;				/* 浏览器类型，例：IE、chrome 等 */
	unsigned char   version[SAM_STR_SIZE+1];/* 浏览器版本 */
	struct event_struct *next;
	struct event_struct *pre;
}*Pflow_event,flow_event;
#endif
//flow_event event_info_head;
//flow_event event_info_head_cp;
#if 0
typedef struct queue_event{
	Pflow_event head;
	Pflow_event tail;
	pthread_mutex_t mutex_event;
	pthread_cond_t cond_event_noempty;
	pthread_cond_t cond_event_nofull;
	pthread_cond_t cond_event_pause;
}*Pevent_mess,event_mess;
Pevent_mess event_sion;
//Pevent_mess event_sion_cp;
#endif

extern list_t g_f_policy_list;
extern pthread_mutex_t g_f_policy_list_mutex;
void init_statistics_list(list_t *list);
void clear_all_statistics_list(list_t *list);

void *flow_sock_busi(void *arg);
void *flow_store_busi(void *arg);
void *flow_store_bak_busi(void *arg);
int flow_event_sock_busi(void);
void* flow_store_event_busi(void *arg);
void* flow_store_event_bak_busi(void *arg);
int flow_connect_db(flow_conn_t *flow_conn);
int make_flow_file_bak(char *buf,char *flow_file,char *tmp_file);
void move_file_to_other_path(char *file_dir, char *other_dir, char *file_name);
flows delete_message(void);
//flow_event delete_event_sion(event_mess*,flow_event*);
int do_event_store_bak(flow_conn_t *conn,flow_event *store_buf,int num);
void reload_config(void);
#endif

