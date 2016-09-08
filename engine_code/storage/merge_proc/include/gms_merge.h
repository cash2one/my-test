/**
 * @file gms_merge.c
 * @brief  合并流程模块结构定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-15
 */
#ifndef _GMS_MERGE_H_
#define _GMS_MERGE_H_

#include <errno.h>
#include <syslog.h>
#include "gms_psql.h"
#include "gms_redis.h"
#include "merge_debug.h"
#include "merge_config.h"
#include "merge_time_op.h"
#include "make_log.h"

#define NEED_MERGE_TIME_SEC				(300)  //5*60
#define NUM   60

#define LOG_MODULE                      "storage"
#define LOG_PROC                        "merge"
#define DEFAULT_MERGE_TIME              "1973-01-01 00:00:00"
#define USER_TMP_TABLE					"t_flow_event"
#define ADDR_TMP_TABLE					"t_flow_event"
#define BROWSER_TMP_TABLE				"t_flow_event"
#define USER_MIN_TABLE					"t_user_min"
#define USER_HOUR_TABLE					"t_user_hour"
#define USER_DAY_TABLE					"t_user_day"
#define USER_WEEK_TABLE					"t_user_week"
#define USER_MONTH_TABLE				"t_user_month"
#define ADDR_MIN_TABLE					"t_addr_min"
#define ADDR_HOUR_TABLE					"t_addr_hour"
#define ADDR_DAY_TABLE					"t_addr_day"
#define ADDR_WEEK_TABLE					"t_addr_week"
#define ADDR_MONTH_TABLE				"t_addr_month"
#define BROWSER_MIN_TABLE				"t_browser_min"
#define BROWSER_HOUR_TABLE				"t_browser_hour"
#define BROWSER_DAY_TABLE				"t_browser_day"
#define BROWSER_WEEK_TABLE				"t_browser_week"
#define BROWSER_MONTH_TABLE				"t_browser_month"
#define VISIT_MIN_TABLE					"t_visit_min"
#define VISIT_HOUR_TABLE				"t_visit_hour"
#define VISIT_DAY_TABLE					"t_visit_day"
#define VISIT_WEEK_TABLE				"t_visit_week"
#define VISIT_MONTH_TABLE				"t_visit_month"
#define VISIT_TMP_TABLE					"t_flow_event"
#define VISITNUM_MIN_TABLE				"t_visitnum_min"
#define VISITNUM_HOUR_TABLE				"t_visitnum_hour"
#define VISITNUM_DAY_TABLE				"t_visitnum_day"
#define VISITNUM_WEEK_TABLE				"t_visitnum_week"
#define VISITNUM_MONTH_TABLE			"t_visitnum_month"
#define VISITNUM_TMP_TABLE				"t_flow_event"

#define BYTE_TMP_TABLE					"t_flow_event"
#define BYTE_MIN_TABLE					"t_flow_min"
#define BYTE_HOUR_TABLE					"t_flow_hour"
#define BYTE_DAY_TABLE					"t_flow_day"
#define BYTE_MONTH_TABLE				"t_flow_month"
#define BYTE_WEEK_TABLE					"t_flow_week"

#define TCP_TMP_TABLE					"t_flow_event"
#define TCP_MIN_TABLE					"t_tcp_min"
#define TCP_HOUR_TABLE					"t_tcp_hour"
#define TCP_DAY_TABLE					"t_tcp_day"
#define TCP_WEEK_TABLE					"t_tcp_week"
#define TCP_MONTH_TABLE					"t_tcp_month"

#define BANK_TMP_TABLE					"t_flow_event"
#define BANK_MIN_TABLE					"t_bank_min"
#define BANK_HOUR_TABLE					"t_bank_hour"
#define BANK_DAY_TABLE					"t_bank_day"
#define BANK_WEEK_TABLE					"t_bank_week"
#define BANK_MONTH_TABLE				"t_bank_month"

#define DELAY_TMP_TABLE					"t_flow_event"
#define DELAY_MIN_TABLE					"t_delay_min"
#define DELAY_HOUR_TABLE				"t_delay_hour"
#define DELAY_DAY_TABLE					"t_delay_day"
#define DELAY_WEEK_TABLE				"t_delay_week"
#define DELAY_MONTH_TABLE				"t_delay_month"
enum merge_threads_no {
	THREAD_FLOW_MERGE = 0,
	THREAD_ATTACK_MERGE,
	THREAD_MERGE_NUM,
   	/*THREAD_ADDR,
	THREAD_BROWSER,
	THREAD_VISIT,
	THREAD_VISIT_NUM,
	THREAD_FLOW_NUM,
	THREAD_TCP,
	THREAD_BANK,
	THREAD_DELAY,*/	

	THREAD_USER_NUM
};

/* -------------------------------------------*/
/**
 * @brief  数据库连接句柄 和 需要处理的数据文件列表
 */
/* -------------------------------------------*/
typedef struct _merge_conn {
	PGconn *psql_conn;					/* PostgreSQL 链接句柄 */
	redisContext* redis_conn;
} merge_conn_t;

void *merge_flow_busi(void *arg);
void *merge_attack_busi(void *arg);
void *merge_user_busi(void *arg);
void *merge_addr_busi(void *arg);
void *merge_browser_busi(void *arg);
void *merge_visit_busi(void *arg);
void *merge_visitnum_busi(void *arg);

int merge_connect_db(merge_conn_t *db_conn, char thread_no);
void merge_disconnect_db(merge_conn_t *db_conn, char thread_no);

#endif
