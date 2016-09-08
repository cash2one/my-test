/**
 * @file merge_cfg.c
 * @brief  存储合并流程配置
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-25
 */
#include "gms_merge.h"

trace_init_t			g_merge_trace;				/* trace 全局 */
unsigned int			g_merge_debug = 0;			/* 模块debug配置标识 */
merge_conf_t			g_merge_info_cfg;			/* 合并模块 基本配置信息 */
merge_debug_info_t		g_merge_debug_cfg;			

pthread_mutex_t time_config_file_mutex = PTHREAD_MUTEX_INITIALIZER;

cfg_desc g_merge_cfg[] = {

	/* ------------ debug ----------*/
	{"MERGE_DEBUG", "debug_all", &(g_merge_debug_cfg.debug_all), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
	{"MERGE_DEBUG", "debug_trunk", &(g_merge_debug_cfg.debug_trunk), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
	{"MERGE_DEBUG", "debug_flow", &(g_merge_debug_cfg.debug_flow), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
	{"MERGE_DEBUG", "debug_attack", &(g_merge_debug_cfg.debug_attack), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
	/* ------------ debug end ----------*/

    /* ------------ config path --------*/
	{"CONFIG_PATH", "database", &(g_merge_info_cfg.database_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/doc/conf/database.conf"},
    /* ------------ config path end --------*/

	{"", "", NULL, 0, 0, 0, 0, ""},
};

cfg_desc g_database_cfg[] = {

	/* ----------- postgreSQL ----- */
	{"POSTGRESQL", "db_name", &(g_merge_info_cfg.psql_dbname), IP_STR_SIZE, CFG_TYPE_STR, 1, 0, "postgres"},
	{"POSTGRESQL", "user", &(g_merge_info_cfg.psql_user), USER_NAME_SIZE, CFG_TYPE_STR, 1, 0, "postgres"},
	{"POSTGRESQL", "pwd", &(g_merge_info_cfg.psql_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, ""},
	{"POSTGRESQL", "ip", &(g_merge_info_cfg.psql_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
	{"POSTGRESQL", "port", &(g_merge_info_cfg.psql_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "5432"},
    {"POSTGRESQL", "unix", &(g_merge_info_cfg.psql_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"POSTGRESQL", "domain_sock", &(g_merge_info_cfg.psql_domain), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp"},
	/* ----------- postgreSQL end ----- */

	/* ----------- redis --------- */
	{"REDIS", "ip", &(g_merge_info_cfg.redis_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
	{"REDIS", "port", &(g_merge_info_cfg.redis_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "6379"},
    {"REDIS", "pwd", &(g_merge_info_cfg.redis_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, "123456"},
    {"REDIS", "unix", &(g_merge_info_cfg.redis_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"REDIS", "domain_sock", &(g_merge_info_cfg.redis_domain), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/redis.sock"},
	/* ----------- redis end ----- */

	{"", "", NULL, 0, 0, 0, 0, ""},
};



void show_merge_config(merge_conf_t* conf, merge_debug_info_t* debug)
{
	printf("-------  CONFIG INFORMATION -------\n");
	printf("\n=====> ++++ debug ++++\n");
	printf("debug_all :\t%d\n",	debug->debug_all);
	printf("debug_trunk:\t%d\n", debug->debug_trunk);

    printf("\n Database path : %s\n", conf->database_path);

	printf("\n=====> ++++ postgreSQL ++++\n");
	printf("db_name:\t%s\n", conf->psql_dbname);
	printf("user:\t\t%s\n", conf->psql_user);
	printf("pwd:\t\t%s\n", conf->psql_pwd);
	printf("ip:\t\t%s\n", conf->psql_ip);
	printf("port:\t\t%s\n", conf->psql_port);

	printf("\n=====> ++++ Redis ++++\n");
	printf("ip :\t\t%s\n", conf->redis_ip);
	printf("port:\t\t%s\n", conf->redis_port);
	printf("\n--------------------------------\n");
}

void init_merge_sql_cmd()
{
	
}

