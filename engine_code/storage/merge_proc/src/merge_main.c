/**
 * @file merge_main.c
 * @brief  存储合并流程主入口
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-06
 */
#include "gms_merge.h"


/* -------------------------------------------*/
/**
 * @brief  根据线程索引值创建合并流程需要各个表业务线程
 *
 * @param tid			得到线程的线程ID
 * @param thread_no		线程索引值
 *
 * @returns   
 *			0  成功
 *			!0 失败
 */
/* -------------------------------------------*/
int merge_create_pthreads(OUT pthread_t *tid, IN int thread_no)
{
	int retn = 0;

	switch (thread_no) {

		case THREAD_FLOW_MERGE:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_flow_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_flow_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_flow_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_ATTACK_MERGE:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_attack_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_attack_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_attack_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_USER_NUM:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_user_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_user_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_user_busi error");
				retn = -1;
				goto END;
			}
			break;
#if 0
		case THREAD_ADDR:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_addr_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_addr_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_addr_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_BROWSER:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_browser_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_browser_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_browser_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_VISIT:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_visit_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_visit_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_vist_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_VISIT_NUM:
			retn = pthread_create(&(tid[thread_no]), NULL, merge_visitnum_busi, NULL);
			if (retn != 0) {
				MERGE_ERROR(DEBUG_TYPE_TRUNK, "Create thread: merge_visitnum_busi error");
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: merge_visitnum_busi error");
				retn = -1;
				goto END;
			}
			break;
#endif
		default:
			break;
	}
END:
	return retn;
}

int main(int argc, char **argv)
{
	pthread_t tid[THREAD_MERGE_NUM] = {0};
	void *tret;
	int retn = 0;
	int i = 0;
	/*打开syslog*/
	openlog("merge_proc",LOG_CONS | LOG_PID,0);
	syslog(LOG_NOTICE,"user program %s start!\n",argv[0]);
	
	/* 初始化环境 */
	/* 读取配置信息 */
	retn = common_read_conf(MERGE_CONFIG_PATH, g_merge_cfg);
	if (retn < 0) {
		fprintf(stderr, "[-][MERGE_PROC] Read config %s ERROR!\n", MERGE_CONFIG_PATH);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][MERGE_PROC] Read config %s ERROR!\n", MERGE_CONFIG_PATH);
		goto EXIT;
	}

    /* 读取数据库信息 */
    retn = common_read_conf(g_merge_info_cfg.database_path, g_database_cfg);
	if (retn < 0) {
		fprintf(stderr, "[-][MERGE_PROC] Read config %s ERROR!\n", g_merge_info_cfg.database_path);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][MERGE_PROC] Read config %s ERROR!\n", g_merge_info_cfg.database_path);
		goto EXIT;
	}

	
	show_merge_config(&g_merge_info_cfg, &g_merge_debug_cfg);

	/* 初始化debug信息 */
	g_merge_trace.conf_file = MERGE_TRACE_FILE_PATH;
	g_merge_trace.entry_name = "merge_proc";
	INIT_TRACE_MODULE(g_merge_trace);
	MERGE_SET_DEBUG(g_merge_debug_cfg);

	/* 开启各个业务表线程 */
	for (i = 0; i < THREAD_MERGE_NUM-1; ++i) {
		retn = merge_create_pthreads(tid, i);
		if (retn != 0) {
			goto EXIT;
		}
	}
	/*	retn = merge_create_pthreads(tid, 2);
		if (retn != 0) {
			goto EXIT;
		}*/

	/* 维护线程 */
	while (1) {
	
		for (i = 0; i < THREAD_MERGE_NUM-1; ++i) {
			retn = pthread_tryjoin_np(tid[i], &tret);
			if (retn != 0) {
				continue;	
			}
			MERGE_DEBUG(DEBUG_TYPE_TRUNK, "=== thread %d return! ===");
			CA_LOG(LOG_MODULE, LOG_PROC, "=== thread %d return! ===",i);
			retn = merge_create_pthreads(tid, i);
			if (retn != 0) {
					goto EXIT;
			}
		}
		usleep(50000);
		//sleep(10);
	}

EXIT:
	closelog();
	CLEAN_TRACE_MODULE();

	return 0;
}

