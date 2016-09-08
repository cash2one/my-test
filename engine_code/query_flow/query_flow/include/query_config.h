#ifndef _QUERY_CONFIG_H_
#define _QUERY_CONFIG_H_

#include "query.h"

#define STORE_CONFIG_PATH   "/gms/storage/conf/storage.conf"
#define QUERY_CONFIG_PATH   "/gms/query_flow/conf/query_info.conf"


typedef struct _query_conf {
    
	/* dev id */
	char dev_id[128];
    char dev_id_path[ABSOLUT_FILE_PATH_SIZE];

    /* custom flow path */
    char custom_flow_path[ABSOLUT_FILE_PATH_SIZE];
    /* flowdb ini path */
    char flowdb_ini_path[ABSOLUT_FILE_PATH_SIZE];

    char dst_file_path[ABSOLUT_FILE_PATH_SIZE];    /* 存放flow事件的文件目标目录 */
} query_flow_conf_t;


extern cfg_desc g_query_cfg[];
extern cfg_desc g_query_path_cfg[];
extern query_flow_conf_t			g_query_info_cfg;		

#endif
