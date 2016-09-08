/**
 * @file gms_store.h
 * @brief  存储流程模块结构定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-7
 */
#ifndef _GMS_STORE_H_
#define _GMS_STORE_H_

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gms_psql.h"
#include "gms_redis.h"
#include "dirwalk.h"
#include "store_debug.h"
#include "store_config.h"
#include "store_file_op.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
 #include <sys/socket.h>
#include <netinet/in.h>
 #include <arpa/inet.h>
#include "make_log.h"
#include <mysql/mysql.h>
#define LOG_MODULE                      "storage"
#define LOG_PROC                        "store"

#define WRONG_ATTACK_FILE_PATH          "/filedata/gms/db/wrong/att/"
#define WRONG_3RD_FILE_PATH             "/filedata/gms/db/wrong/3rd/"
#define WRONG_AV_FILE_PATH              "/filedata/gms/db/wrong/ve/"
#define WRONG_ABB_FILE_PATH             "/filedata/gms/db/wrong/ae/"
#define WRONG_BDNS_FILE_PATH            "/filedata/gms/db/wrong/dns/"
#define WRONG_BURL_FILE_PATH            "/filedata/gms/db/wrong/url/"
#define WRONG_DEV_STATUS_FILE_PATH      "/filedata/gms/db/wrong/devstatus/"

#define EVENT_ATTACK_FILE_PATH          "/filedata/gms/db/me/att"
#define EVENT_3RD_FILE_PATH             "/filedata/gms/db/me/3rd"
#define EVENT_AV_FILE_PATH              "/filedata/gms/db/ve"
#define EVENT_ABB_FILE_PATH             "/filedata/gms/db/ae"
#define EVENT_BDNS_FILE_PATH            "/filedata/gms/db/dns"
#define EVENT_BURL_FILE_PATH            "/filedata/gms/db/url"
#define EVENT_DEV_STATUS_FILE_PATH      "/filedata/gms/db/devstatus"

#define TMP_ATT_EVENT_PATH		"/gms/storage/conf/att.tmp"
#define TMP_3RD_EVENT_PATH		"/gms/storage/conf/3rd.tmp"
#define TMP_BURL_EVENT_PATH		"/gms/storage/conf/burl.tmp"
#define TMP_ABB_EVENT_PATH		"/gms/storage/conf/abb.tmp"
#define TMP_AV_EVENT_PATH		"/gms/storage/conf/av.tmp"

#define EMAIL_PATH                      "/data/tmpdata/comm/mtx/email/"
#define EMAIL_PATH2                     "/data/tmpdata/comm/alarm_event/email"

#define ATTACK_EVENT_BUF_SIZE           (1024*1024*500)
#define _3RD_EVENT_BUF_SIZE             (1024*1024*500)
#define AV_EVENT_BUF_SIZE               (1024*1024*10)
#define ABB_EVENT_BUF_SIZE              (1024*1024*10)
#define BLACK_EVENT_BUF_SIZE            (1024*1024*10)
#define DEV_STATUS_EVENT_BUF_SIZE       (1024*1024*10)
#define NUM  12000

enum thread_no {
    THREAD_ATTACK =0,           /* 特征事件（攻击）线程 */
    THREAD_3RD,                 /* 特征事件（网站，恶意感染，其他）线程 */
    THREAD_AV,                  /* 恶意代码传播 事件 线程 */
    THREAD_ABB,                 /* 异常行为事件 线程 */
    THREAD_BLACK,               /* 黑名单DNS URL 线程 */
    THREAD_DEV_STATUS,          /* 设备状态事�?线程 */

    THREAD_NUM,                 /* 线程数目 */


    THREAD_BLACK_URL,           /* URL 流程标识 */
    THREAD_BLACK_DNS            /* DNS 流程标识 */
};


/* -------------------------------------------*/
/**
 * @brief  数据库连接句�?�?需要处理的数据文件列表
 */
/* -------------------------------------------*/
typedef struct _store_conn {
    PGconn *psql_conn;                    /* PostgreSQL 链接句柄 */
    redisContext* redis_conn;
	MYSQL* mysql_conn;
    list_t* list;                        /* 遍历出来的数据文件列�?*/
} store_conn_t;



void *store_attack_busi(void *arg);
void *store_3rd_busi(void *arg);
void *store_av_busi(void *arg);
void *store_abb_busi(void *arg);
void *store_black_busi(void *arg);
void *store_dev_status_busi(void* arg);


/* -------------------------------------------*/
/**
 * @brief   根据不同的表封装不同的批量插入语�?
 *
 * @param thread_no        线程ID
 * @param cmd            封装好的SQL
 * @param cmd_max_len    SQL最大长�? *
 */
/* -------------------------------------------*/
void init_copy_cmd(char thread_no, char *cmd, unsigned int cmd_max_len);


void change_att_copy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name);

void change_dev_status_copy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name);
int create_store_proc_dir(void);


int store_connect_db(store_conn_t *db_conn, char thread_no);
void store_disconnect_db(store_conn_t *db_conn, char thread_no);

/* -------------------------------------------*/
/**
 * @brief  得到当前redis表中最大时间戳字符�? *
 * @param time_str 得到的时间戳
 * @param conn    已建立的链接
 * @param key     redis表名
 * @param col     时间在第几列字段（从0开�?
 * @param max_count redis表中的最大条�? *
 * @returns   
 *          0  成功
 *          -1 失败
 */
/* -------------------------------------------*/
int store_get_redistable_last_time(char *time_str, redisContext *conn, char *key, int col, int max_count);


#endif
