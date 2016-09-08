/**
 * @file store_main.c
 * @brief  存储流程主入口函数
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-06
 */
#include <pthread.h>
#include <stdio.h>
#include "gms_store.h"



/* -------------------------------------------*/
/**
 * @brief  根据线程索引值创建存储流程需要各个表业务线程
 *
 * @param tid            得到线程的线程ID
 * @param thread_no        线程索引值
 *
 * @returns   
 *            0  成功
 *            !0 失败
 */
/* -------------------------------------------*/
int store_create_pthreads(OUT pthread_t* tid, IN int thread_no)
{
    int retn = 0;

    switch (thread_no) {
        case THREAD_ATTACK:
            retn = pthread_create(&(tid[thread_no]), NULL, store_attack_busi, NULL);
            if (retn != 0) {
                STORE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: store_attack_busi error");
                CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: store_attack_busi ERROR!!****");
                retn = -1;
                goto END;
            }
            break;
        case THREAD_3RD:
            retn = pthread_create(&(tid[thread_no]), NULL, store_3rd_busi, NULL);
            if (retn != 0) {
                STORE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: store_3rd_busi error");
                CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: store_3rd_busi ERROR!!****");
                retn = -1;
                goto END;
            }
            break;
        case THREAD_AV:
            retn = pthread_create(&(tid[thread_no]), NULL, store_av_busi, NULL);
            if (retn != 0) {
                STORE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: store_av_busi error");
                CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: store_av_busi ERROR!!****");
                retn = -1;
                goto END;
            }
            break;
        case THREAD_ABB:
            retn = pthread_create(&(tid[thread_no]), NULL, store_abb_busi, NULL);
            if (retn != 0) {
                STORE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: store_abb_busi error");
                CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: store_abb_busi ERROR!!****");
                retn = -1;
                goto END;
            }
            break;
        case THREAD_BLACK:
            retn = pthread_create(&(tid[thread_no]), NULL, store_black_busi, NULL);
            if (retn != 0) {
                STORE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: store_black_busi error");
                CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: store_black_busi ERROR!!****");
                retn = -1;
                goto END;
            }
            break;
        case THREAD_DEV_STATUS:
            retn = pthread_create(&(tid[thread_no]), NULL, store_dev_status_busi, NULL);
            if (retn != 0) {
                STORE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: store_dev_status_busi error");
                CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: store_dev_status_busi ERROR!!****");
                retn = -1;
                goto END;
            }
            break;
        default:
            break;
    }

END:
    return retn;
}
int get_dev_id(char *src,char *dst)
{
	FILE *fp;
	memset(dst,0,20);
	fp=popen("python /gms/gapi/modules/auth/genkey.pyc","r");
	fgets(dst,20,fp);
	pclose(fp);
	

	return 0;
}



int main(int argc, char **argv)
{
    int retn = 0;
    pthread_t tid[THREAD_NUM] = {0};
    void *tret;
    int i = 0;


    /* 读取配置信息 */
    retn = common_read_conf(STORE_CONFIG_PATH, g_store_cfg);
    if (retn < 0) {
        fprintf(stderr, "[-][SOTRE_PROC] Read config %s ERROR!\n", STORE_CONFIG_PATH);
        CA_LOG(LOG_MODULE, LOG_PROC, "****Create thread: Read config %s ERROR!****", STORE_CONFIG_PATH);
        goto EXIT;
    }
    /* 读取数据库信息 */
    retn = common_read_conf(g_store_info_cfg.database_path, g_database_cfg);
    if (retn < 0) {
        fprintf(stderr, "[-][STORE_PROC] Read config %s ERROR!\n", g_store_info_cfg.database_path);
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][STORE_PROC] Read config %s ERROR!\n", g_store_info_cfg.database_path);
        goto EXIT;
    }
     printf("mysql switch %d\n",g_store_info_cfg.mysql_switch);

    show_store_config(&g_store_info_cfg, &g_store_debug_cfg);

    /* TODO 初始化环境 */
    /* 1. 初始化debug信息 */
    g_store_trace.conf_file = STORE_TRACE_FILE_PATH;
    g_store_trace.entry_name = "store_proc";
    INIT_TRACE_MODULE(g_store_trace);
    STORE_SET_DEBUG(g_store_debug_cfg);        

    /* 2. 创建必要目录 */
    retn = create_store_proc_dir();
    if (retn < 0) {
        fprintf(stderr, "[-][SOTRE_PROC] Create Dir ERROR\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****Create Dir ERROR!****");
        goto EXIT;
    }

    /* 3. 读取设备uuid */
	//char *id_malloc=NULL;
    //retn = read_file_to_data(g_store_info_cfg.dev_id_path, (void **)&id_malloc);
    retn = get_dev_id(g_store_info_cfg.dev_id_path, g_store_info_cfg.dev_id);
    if (retn < 0) {
        fprintf(stderr, "[-][STORE_PROC] Read %s ERROR!\n", g_store_info_cfg.dev_id_path);
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][STORE_PROC] Read %s ERROR!\n", g_store_info_cfg.dev_id_path);
        goto EXIT;
    }
	mysql_library_init(0, NULL, NULL);
	//strncpy(g_store_info_cfg.dev_id,id_malloc,retn);
	//if (NULL != id_malloc)
	//	free(id_malloc);

    /* 开启各个业务表线程 */
    for (i = 0; i < THREAD_NUM; ++i) {
        retn = store_create_pthreads(tid, i);
        if (retn != 0) {
            goto EXIT;
        }
    }
	mysql_library_end();


    /* 维护线程 */
    while (1) {
    
        for (i = 0; i < THREAD_NUM; ++i) {
    //    for (i = 0; i < 2; ++i) {
            retn = pthread_tryjoin_np(tid[i], &tret);
            if (retn != 0) {
                continue;    
            }
            
            STORE_DEBUG(DEBUG_TYPE_TRUNK, "=== thread %d return! ===");
            CA_LOG(LOG_MODULE, LOG_PROC, "=== thread %d return! ===",i);

            retn = store_create_pthreads(tid, i);
            if (retn != 0) {
                    goto EXIT;
            }
        }
        sleep(10);
    }

EXIT:
    CLEAN_TRACE_MODULE();

    return 0;
}
