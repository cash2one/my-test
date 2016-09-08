/**
 * @file query_main.c
 * @brief  查询流量主入口
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2014-05-12
 */

#include "query.h"

int verbose = 2;

int get_dev_id(char *src,char *dst)
{
	FILE *fp;
	memset(dst,0,20);
	fp=popen("python /gms/gapi/modules/auth/genkey.pyc","r");
	fgets(dst,20,fp);
	pclose(fp);
	

	return 0;
}
int main(int argc, char *argv[])
{
    int retn = 0;


    if (argc != 2) {
        fprintf(stderr, "[-][QUERY_FLOW] argv is not equal 2.! Please Usage: ./query_flow query_cmd_file\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][QUERY_FLOW] argv is not equal 2.! Please Usage: ./query_flow query_cmd_file\n");
        goto EXIT;
    }
    
    /* 读取配置信息 */
	retn = common_read_conf(STORE_CONFIG_PATH, g_query_cfg);
	if (retn < 0) {
		fprintf(stderr, "[-][QUERY_FLOW] Read config %s ERROR!\n", STORE_CONFIG_PATH);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][QUERY_FLOW] Read config %s ERROR!\n", STORE_CONFIG_PATH);
		goto EXIT;
	}
	retn = common_read_conf(QUERY_CONFIG_PATH, g_query_path_cfg);
	if (retn < 0) {
		fprintf(stderr, "[-][QUERY_FLOW] Read config %s ERROR!\n", QUERY_CONFIG_PATH);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][QUERY_FLOW] Read config %s ERROR!\n", QUERY_CONFIG_PATH);
		goto EXIT;
	}

    /* 读取设备ID(UUID) */
    //retn = read_file_to_data(g_query_info_cfg.dev_id_path, g_query_info_cfg.dev_id);
    retn = get_dev_id(g_query_info_cfg.dev_id_path, g_query_info_cfg.dev_id);
    if (retn < 0) {
        fprintf(stderr, "[-][QUERY_FLOW] Read %s ERROR!\n", g_query_info_cfg.dev_id_path);
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][QUERY_FLOW] Read %s ERROR!\n", g_query_info_cfg.dev_id_path);
        goto EXIT;
    }
    g_query_info_cfg.dev_id[36] = '\0';

    /* 读取flow数据库信息 */

    /* 创建必要目录 */
    retn = create_query_proc_dir();
    if (retn < 0) {
        fprintf(stderr, "[-][QUERY_FLOW] Create QUERY DIR ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][QUERY_FLOW] Create QUERY DIR ERROR!\n");
        goto EXIT;
    }

    /* 初始化链表 */
	//init_statistics_list(&g_f_policy_list);

    /* 进入查询主业务 */
    retn = query_entry(argv[1]);
    if (retn < 0) {
        fprintf(stderr, "[-][QUERY_FLOW] query_entry ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][QUERY_FLOW] query_entry ERROR!\n");
        goto EXIT;
    }

EXIT:
    /* 清空链表 */
    return retn;
}
