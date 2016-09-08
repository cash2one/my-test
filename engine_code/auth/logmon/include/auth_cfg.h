#ifndef _AUTH_CFG_H_
#define _AUTH_CFG_H_

#include"config.h"
#define AUTH_CFG_PATH	"/gms/auth/conf/auth.conf"
#define FILE_PATH_SIZE          1024

typedef struct _auth_conf {

    char reg_path[FILE_PATH_SIZE];
    char comm_main_path[FILE_PATH_SIZE];
    char comm_cloud_path[FILE_PATH_SIZE];
    char url_detect_path[FILE_PATH_SIZE];
    char mtx_path[FILE_PATH_SIZE];
    char auth_check_path[FILE_PATH_SIZE];
	char wtl_path[FILE_PATH_SIZE];
	char flow_num_path[FILE_PATH_SIZE];
	char xml_path[FILE_PATH_SIZE];
	char apt_online_path[FILE_PATH_SIZE];
	char apt_offline_path[FILE_PATH_SIZE];
	char vds_dd_path[FILE_PATH_SIZE];
	char vds_apc_path[FILE_PATH_SIZE];
	char vds_dm_path[FILE_PATH_SIZE];
	char vds_monitor_path[FILE_PATH_SIZE];
	char store_proc_path[FILE_PATH_SIZE];
	char merge_proc_path[FILE_PATH_SIZE];
	char flow_proc_path[FILE_PATH_SIZE];
	char sys_guard_path[FILE_PATH_SIZE];
	char term_path[FILE_PATH_SIZE];
	char upgrade_path[FILE_PATH_SIZE];
	char remote_trans_path[FILE_PATH_SIZE];

} auth_conf_t;
extern auth_conf_t     auth_conf_cfg;
extern cfg_desc g_auth_cfg[];

#endif
