#ifndef _FILE_PATH_H_
#define _FILE_PATH_H_

#include "config.h"
#define GUARD_CFG_PATH		"/gms/guard/conf/guard.conf"
#define PQL_CONF_PATH           "/gms/storage/conf/storage.cfg"
#define FILE_PATH_SIZE 		1024
#define VERSION_SIZE    10
#include"store_common.h"
#include"gms_psql.h"
#define GUARD_MODULE	"guard"
#define GUARD_PROC	"sys_guard"

typedef struct _guard_conf {

	char regular_guard_path[FILE_PATH_SIZE];
	char service_class_path[FILE_PATH_SIZE];
	char devid_path[FILE_PATH_SIZE];
	char libversion_path[FILE_PATH_SIZE];
	char sysversion_path[FILE_PATH_SIZE];
	char process_path[FILE_PATH_SIZE];
	char pql_conf_path[FILE_PATH_SIZE];
	char sysstatus_file_path[FILE_PATH_SIZE];
	char flow_path[FILE_PATH_SIZE];
	char dns_path[FILE_PATH_SIZE];
	char URL_path[FILE_PATH_SIZE];
	char abb_online_path[FILE_PATH_SIZE];
	char abb_offline_path[FILE_PATH_SIZE];
	char mtd_path[FILE_PATH_SIZE];
	char url_path[FILE_PATH_SIZE];
	char virus_path[FILE_PATH_SIZE];
	char wrong_path[FILE_PATH_SIZE];

} guard_conf_t;

typedef struct _pql_conf {
    /* PostgreSQL */
    char psql_dbname[DBNAME_SIZE];
    char psql_user[USER_NAME_SIZE];
    char psql_pwd[PASSWORD_SIZE];
    char psql_ip[IP_STR_SIZE];
    char psql_port[PORT_STR_SIZE];
    unsigned char psql_unix;
    char psql_domain[FILE_PATH_SIZE];
} pql_conf_t;

typedef struct _version_conf {
    
    char sysversion[VERSION_SIZE];    

} version_conf_t;

extern pql_conf_t pql_conf;
extern guard_conf_t 	guard_conf_cfg;
extern version_conf_t  version_conf;
extern cfg_desc g_guard_cfg[]; 
extern cfg_desc pql_cfg[];
extern cfg_desc version_cfg[];

#endif
