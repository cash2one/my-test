/**
 * @file merge_config.h
 * @brief  合并流程配置声明定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-25
 */

#include "config.h"

#define MERGE_CONFIG_PATH			"./conf/storage.conf"
#define MERGE_TIME_CONFIG_PATH		"./conf/merge_time.conf"
#define MERGE_TIME_USER_PATH		"./conf/merge_user_time.conf"
#define MERGE_SQL_CMD_SIZE			(800)
#define ATTACK_EVENT_DELAY_TIME         (600) 

extern pthread_mutex_t time_config_file_mutex;

typedef struct _merge_conf {
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

	/* critical time */
	char flow_ctime[TIME_FORMAT_SIZE];
	unsigned char merge_hour;
	unsigned char merge_min;

	/* attack merge last time */
	char att_tmp_to_min[TIME_FORMAT_SIZE];
	char att_min_to_hour[TIME_FORMAT_SIZE];
	char att_hour_to_day[TIME_FORMAT_SIZE];
	char att_day_to_week[TIME_FORMAT_SIZE];
	char att_week_to_month[TIME_FORMAT_SIZE];
	char att_month_to_year[TIME_FORMAT_SIZE];

	/* 3rd merge last time */
	char _3rd_tmp_to_min[TIME_FORMAT_SIZE];
	char _3rd_min_to_hour[TIME_FORMAT_SIZE];
	char _3rd_hour_to_day[TIME_FORMAT_SIZE];
	char _3rd_day_to_week[TIME_FORMAT_SIZE];
	char _3rd_week_to_month[TIME_FORMAT_SIZE];
	char _3rd_month_to_year[TIME_FORMAT_SIZE];

	/* flow merge last time */
	char flow_min_to_hour[TIME_FORMAT_SIZE];
	char flow_hour_to_day[TIME_FORMAT_SIZE];

    /* databse config path */
    char database_path[ABSOLUT_FILE_PATH_SIZE];

} merge_conf_t;

typedef struct _merge_user_conf {
	unsigned char merge_hour;
	unsigned char merge_min;
	/* merge last time */
	char att_tmp_to_min[TIME_FORMAT_SIZE];
	char att_min_to_hour[TIME_FORMAT_SIZE];
	char att_hour_to_day[TIME_FORMAT_SIZE];
	char att_day_to_week[TIME_FORMAT_SIZE];
	char att_week_to_month[TIME_FORMAT_SIZE];
	char att_month_to_year[TIME_FORMAT_SIZE];

} merge_user_conf_t;

extern merge_conf_t					g_merge_info_cfg;			/* 合并模块 基本配置信息 */
 merge_user_conf_t			g_user_info_cfg;   /*合并模块 用户数合并*/			
 merge_user_conf_t			g_addr_info_cfg;   /*合并模块 地址合并*/			
 merge_user_conf_t			g_browser_info_cfg;   /*合并模块 浏览器版本合并*/			
 merge_user_conf_t			g_visit_info_cfg;   /*合并模块 访问时长合并*/			
 merge_user_conf_t			g_visitnum_info_cfg;   /*合并模块 访问次数合并*/			
 merge_user_conf_t			g_byte_info_cfg;   /*合并模块 流量合并*/			
 merge_user_conf_t			g_tcp_info_cfg;   /*合并模块 tcp连接数合并*/			
 merge_user_conf_t			g_bank_info_cfg;   /*合并模块 网银合并*/			
 merge_user_conf_t			g_delay_info_cfg;   /*合并模块 端到端的延时合并*/			
extern cfg_desc g_merge_cfg[];
extern cfg_desc g_database_cfg[];
