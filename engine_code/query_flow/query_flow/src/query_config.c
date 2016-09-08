/**
 * @file query_config.c
 * @brief  查询流量模块配置读写实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2014-05-12
 */


#include "query_config.h"

cfg_desc g_query_cfg[] = {
    /* config-path */
    {"CONFIG_PATH", "custom_flow", &(g_query_info_cfg.custom_flow_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/custom_flow.conf"},
    {"CONFIG_PATH", "flowdb_ini", &(g_query_info_cfg.flowdb_ini_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/apt/config/fdb.ini"},
    {"CONFIG_PATH", "uuid", &(g_query_info_cfg.dev_id_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/uuid.conf"},

    {"", "", NULL, 0, 0, 0, 0, ""},
};

cfg_desc g_query_path_cfg[] = {
    /* dst path */
    {"FLOW_RESULT_PATH", "dst_path", &(g_query_info_cfg.dst_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/tmpdata/comm/query/flow/up/"},

    {"", "", NULL, 0, 0, 0, 0, ""},
};


query_flow_conf_t		g_query_info_cfg;		


int create_query_proc_dir(void)
{
    int retn = 0;
    
    retn = create_dir_ex(g_query_info_cfg.dst_file_path);
    if (retn < 0) {
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_query_info_cfg.dst_file_path);

    return retn;
}
