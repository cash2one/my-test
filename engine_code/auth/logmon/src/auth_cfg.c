#include<stdio.h>
#include<string.h>
#include"auth_cfg.h"
#include"misc.h"
#include"config.h"

auth_conf_t     auth_conf_cfg;

cfg_desc g_auth_cfg[] = {
        {"reg", "file_path", &(auth_conf_cfg.reg_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/reg_dat/reg.dat"},
        {"comm_main", "file_path", &(auth_conf_cfg.comm_main_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/comm/comm_main.py"},
        {"comm_cloud", "file_path", &(auth_conf_cfg.comm_cloud_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/comm/cloud/comm_cloud.py"},
        {"mtx", "file_path", &(auth_conf_cfg.mtx_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/mtx/mtx"},
        {"auth_check", "file_path", &(auth_conf_cfg.auth_check_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/doc/conf/auth/"},
        {"wtl", "file_path", &(auth_conf_cfg.wtl_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/mtx/conf/wtl.conf"},
        {"flow_num", "file_path", &(auth_conf_cfg.flow_num_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/mtx/pag.conf"},
        {"xml", "file_path", &(auth_conf_cfg.xml_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/comm/conf/comm_interface_conf.xml"},
        {"apt_online", "file_path", &(auth_conf_cfg.apt_online_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/apt/online/apt"},
        {"apt_offline", "file_path", &(auth_conf_cfg.apt_offline_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/apt/offlineline/offline"},
        {"vds_dd", "file_path", &(auth_conf_cfg.vds_dd_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/vds/dd"},
        {"vds_apc", "file_path", &(auth_conf_cfg.vds_apc_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/vds/apc"},
        {"vds_dm", "file_path", &(auth_conf_cfg.vds_dm_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/vds/dm"},
        {"vds_monitor", "file_path", &(auth_conf_cfg.vds_monitor_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/vds/script/monitor"},
        {"url_detect", "file_path", &(auth_conf_cfg.url_detect_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/url_detect/url_detect.py"},
        {"store_proc", "file_path", &(auth_conf_cfg.store_proc_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/storage/store/store_proc"},
        {"merge_proc", "file_path", &(auth_conf_cfg.merge_proc_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/storage/merge/merge_proc"},
        {"flow_proc", "file_path", &(auth_conf_cfg.flow_proc_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/storage/flow/flow_proc"},
        {"sys_guard", "file_path", &(auth_conf_cfg.sys_guard_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/sys_guard"},
        {"term", "file_path", &(auth_conf_cfg.term_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/termctrl/term"},
        {"upgrade", "file_path", &(auth_conf_cfg.upgrade_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/updata/upgrade/Upgrade.jar"},
        {"remote_trans", "file_path", &(auth_conf_cfg.remote_trans_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/comm/remote/remote_trans.py"},
        {"", "", NULL, 0, 0, 0, 0, ""},
};
