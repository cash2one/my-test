/**
 * @file store_config.h
 * @brief  存储模块配置信息声明
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-18
 */
#ifndef _STORE_CONFIG_H_
#define _STORE_CONFIG_H_

#include "config.h"
#include "make_log.h"
#include "gms_psql.h"
#include "gms_redis.h"


#define    STORE_CONFIG_PATH            "./conf/storage.conf"

/* -------------------------------------------*/
/**
 * @brief  存储模块 基本配置文件
 */
/* -------------------------------------------*/
typedef struct _store_conf {

    /* PostgreSQL */
    char psql_dbname[DBNAME_SIZE];
    char psql_user[USER_NAME_SIZE];
    char psql_pwd[PASSWORD_SIZE];
    char psql_ip[IP_STR_SIZE];
    char psql_port[PORT_STR_SIZE];
    unsigned char psql_unix;
    char psql_domain[ABSOLUT_FILE_PATH_SIZE]; 

    /* Redis */
    char redis_ip[IP_STR_SIZE];
    char redis_port[IP_STR_SIZE];
    char redis_pwd[PASSWORD_SIZE];
    unsigned char redis_unix; 
    char redis_domain[ABSOLUT_FILE_PATH_SIZE]; 

	/* MYSQL */
    char mysql_dbname[DBNAME_SIZE];
    char mysql_user[USER_NAME_SIZE];
    char mysql_pwd[PASSWORD_SIZE];
    char mysql_ip[IP_STR_SIZE];
    unsigned int mysql_port;
    char mysql_table_name[TABLE_NAME_SIZE];
    unsigned char mysql_mtx; 
    unsigned char mysql_apt; 
    unsigned char mysql_vds; 
    unsigned int mysql_switch;

    /* event attack */
    unsigned int attack_show_num;
    char event_attack_file_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_attack_file_path[ABSOLUT_FILE_PATH_SIZE];

    /* event web */
    unsigned int web_show_num;
    /* event code */
    unsigned int code_show_num;
    /* event else */
    unsigned int else_show_num;

    char event_3rd_file_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_3rd_file_path[ABSOLUT_FILE_PATH_SIZE];

    /* event av */
    unsigned int av_show_num;    /* 事实呈现界面的最大条数 */
    char event_av_file_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_av_file_path[ABSOLUT_FILE_PATH_SIZE];

    /* event abb */
    unsigned int abb_show_num;
    char event_abb_file_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_abb_file_path[ABSOLUT_FILE_PATH_SIZE];

    /* event black */
    unsigned int url_show_num;
    char event_burl_file_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_burl_file_path[ABSOLUT_FILE_PATH_SIZE];

    unsigned int dns_show_num;
    char event_bdns_file_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_bdns_file_path[ABSOLUT_FILE_PATH_SIZE];

    /* dev status */
    char event_dev_status_file_path[ABSOLUT_FILE_PATH_SIZE];//存储便利目录
    char comm_dev_status_file_path[ABSOLUT_FILE_PATH_SIZE]; //给通讯传输的目录
    char wrong_dev_status_file_path[ABSOLUT_FILE_PATH_SIZE];//错误目录

    /* comm conf */
    char comm_conf_path[ABSOLUT_FILE_PATH_SIZE];
    char is_comm_monitor[2];

    /* config path */
    char database_path[ABSOLUT_FILE_PATH_SIZE];

	/* dev id */
	char dev_id[128];
    char dev_id_path[ABSOLUT_FILE_PATH_SIZE];

} store_conf_t;

extern store_conf_t        g_store_info_cfg;    /* 存储模块 基本配置信息 */    
extern cfg_desc g_store_cfg[];                /* 基本配置文件读取规则 */
extern cfg_desc g_database_cfg[];                /* 数据库配置读取规则 */
extern store_debug_info_t    g_store_debug_cfg;    /* debug配置变量 */    
extern trace_init_t        g_store_trace;                        /* trace 全局 */

int read_comm_interface_conf(char* comm_conf_path, char *monitor);
char * ip_int_to_char(char *pint,char *pchar);

#endif
