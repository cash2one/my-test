/**
 * @file flow_config.h
 * @brief  流量存储配置声明定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-03
 */
#ifndef _FLOW_CONFIG_H_
#define _FLOW_CONFIG_H_

#include "gms_flow.h"

#define FLOW_CONFIG_PATH		"./conf/storage.conf"
#define FLOW_TIME_CONFIG_PATH	"./conf/flow_time.conf"


#define PROTOCOL_TYPE_ALL			(255)
#define PROTOCOL_TYPE_TCP			(6)
#define PROTOCOL_TYPE_UDP			(17)
#define PROTOCOL_TYPE_OTHER			(0)
#define PROTOCOL_TYPE_FTP			(9999)	//9999 为不支持 flowdb中并不可查询
#define PROTOCOL_TYPE_HTTP			(9999)
#define PROTOCOL_TYPE_MAIL			(9999)
#define PROTOCOL_TYPE_DNS			(9999)
#define PROTOCOL_TYPE_NUM			(8)

enum user_defined_protocol {
	PRO_ALL_ID = 1,
	PRO_TCP_ID,
	PRO_UDP_ID,
	PRO_OTHERS_ID,
	PRO_FTP_ID,
	PRO_HTTP_ID,
	PRO_MAIL_ID,
	PRO_DNS_ID
};

typedef struct _flow_conf {
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

	/*socket*/
	char socket_ip[IP_STR_SIZE];
   	unsigned int  flow_port;	
   	unsigned int  event_port;	

	/* critical time */
	char flow_ctime[TIME_FORMAT_SIZE];

	/* dev id */
	char dev_id[128];
    char dev_id_path[ABSOLUT_FILE_PATH_SIZE];
	/* query id */
	char query_id;

    /* comm_interface_conf.xml path */
    char comm_conf_path[ABSOLUT_FILE_PATH_SIZE];
    
    /* database config path */
    char database_path[ABSOLUT_FILE_PATH_SIZE];

    /* custom flow path */
    char custom_flow_path[ABSOLUT_FILE_PATH_SIZE];
    /* flowdb ini path */
    char flowdb_ini_path[ABSOLUT_FILE_PATH_SIZE];

    /* is self monitor */
    char is_comm_monitor[2];

	/*is self root*/
	char is_root[2];

	/* show flow num */
	unsigned int flow_show_num;

    char src_file_path[ABSOLUT_FILE_PATH_SIZE]; /* 数据库读取flow事件源目录 */
    char dst_file_path[ABSOLUT_FILE_PATH_SIZE];    /* 存放flow事件的文件目标目录 */
    char wrong_path[ABSOLUT_FILE_PATH_SIZE];   /* 存放错误flow事件的文件目录*/
	char event_src_path[ABSOLUT_FILE_PATH_SIZE];/*流老化的事件*/
	char event_dst_path[ABSOLUT_FILE_PATH_SIZE];
	char event_wrong_path[ABSOLUT_FILE_PATH_SIZE];
	char timeout_src_path[ABSOLUT_FILE_PATH_SIZE];/*长连接事件*/
	char timeout_dst_path[ABSOLUT_FILE_PATH_SIZE];
	char timeout_wrong_path[ABSOLUT_FILE_PATH_SIZE];
	char ip_str[ABSOLUT_FILE_PATH_SIZE];

	/*** 建立老化流表 **/
	int thread_num;
	int session_len;

} flow_conf_t;


typedef struct _flow_custom_config {
	char title[100];
	char cus_type;
	char is_display;
	char protocol_type;
	char start_ip[IP_STR_SIZE];
	char end_ip[IP_STR_SIZE];
	char redis_tname[KEY_NAME_SIZ];
} flow_custom_conf_t;

extern flow_conf_t		g_flow_info_cfg;			/* 合并模块 基本配置信息 */
extern cfg_desc g_flow_cfg[];
extern cfg_desc g_database_cfg[];

void init_flowcopy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name);
void init_event_copy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name);
int read_comm_interface_conf(char* comm_conf_path, char *monitor);
int read_comm_interface_conf_isroot(char* comm_conf_path, char *root);
void init_net_copy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name);
int create_flow_proc_dir(void);

#endif
