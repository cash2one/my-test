/**
 * @file flow_main.c
 * @brief  流量存储主入口
 * @author zhangdawei <zhangdawei@chanct.com>
 * @version 1.0
 * @date 2015-02-05
 */

#include "gms_flow.h"

extern long ip_find_id(unsigned int userip);
/* -------------------------------------------*/
/**
 * @brief  根据线程索引值创建流量流程需要各个业务线程
 *
 * @param tid			得到线程的线程ID
 * @param thread_no		线程索引值
 *
 * @returns   
 *			0  成功
 *			!0 失败
 */
/* -------------------------------------------*/
int flow_create_pthreads(OUT pthread_t* tid, IN int thread_no)
{
	int retn = 0;

	switch (thread_no) {
		case THREAD_SOCK_FLOW:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_sock_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_sock_busi error");
				retn = -1;
				goto END;
			}
			break;
#if 1
		case THREAD_FLOW_BAK_STORE:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_bak_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_bak_busi error");
				retn = -1;
				goto END;
			}
			break;
#endif
		case THREAD_FLOW_STORE:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_busi error");
				retn = -1;
				goto END;
			}
			break;
#if 0
		case THREAD_EVENT_SOCK:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_event_sock_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_event_sock_busi error");
				retn = -1;
				goto END;
			}
			break;
#endif
		case THREAD_EVENT_STORE1:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_event_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_event_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_EVENT_STORE2:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_event_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_event_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_EVENT_STORE3:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_event_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_event_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_EVENT_STORE4:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_event_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_event_busi error");
				retn = -1;
				goto END;
			}
			break;
		case THREAD_EVENT_BAK_STORE:
			retn = pthread_create(&(tid[thread_no]), NULL, flow_store_event_bak_busi, NULL);
			if (retn != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Create thread: flow_store_event_bak_busi error");
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
int main(int argc, char *argv)
{
	pthread_t tid[THREAD_FLOW_NUM] = {0};
	void *tret;
	int retn = 0;
	int i = 0;
	//int thread_num = THREAD_FLOW_NUM;
		
	/* 初始化环境 */
	/* 读取配置信息 */
	retn = common_read_conf(FLOW_CONFIG_PATH, g_flow_cfg);
	if (retn < 0) {
		fprintf(stderr, "[-][FLOW_PROC] Read config %s ERROR!\n", FLOW_CONFIG_PATH);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][FLOW_PROC] Read config %s ERROR!\n", FLOW_CONFIG_PATH);
		goto EXIT;
	}

    /* 读取数据库信息 */
    retn = common_read_conf(g_flow_info_cfg.database_path, g_database_cfg);
    if (retn < 0) {
        fprintf(stderr, "[-][FLOW_PROC] Read config %s ERROR!\n", g_flow_info_cfg.database_path);
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][FLOW_PROC] Read config %s ERROR!\n", g_flow_info_cfg.database_path);
        goto EXIT;
    }
//	printf("flowevent_buf_len=%llu\n",g_flow_debug_cfg.flowevent_buf_len);
//	printf("flowevent_num=%u\n",g_flow_debug_cfg.flowevent_num);
//getchar();
    /* 读取设备ID（UUID）*/
    //retn = read_file_to_data(g_flow_info_cfg.dev_id_path, g_flow_info_cfg.dev_id);
    retn = get_dev_id(g_flow_info_cfg.dev_id_path, g_flow_info_cfg.dev_id);
    if (retn < 0) {
        fprintf(stderr, "[-][FLOW_PROC] Read %s ERROR!\n", g_flow_info_cfg.dev_id_path);
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][FLOW_PROC] Read %s ERROR!\n", g_flow_info_cfg.dev_id_path);
        goto EXIT;
    }
    g_flow_info_cfg.dev_id[36] = '\0';

    /* 创建必要的目录 */
    retn = create_flow_proc_dir();
    if (retn < 0) {
        fprintf(stderr, "[-][FLOW_PROC] Create FLOW DIR ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "[-][FLOW_PROC] Create FLOW DIR ERROR!\n");
        goto EXIT;
    }

	/* 初始化链表 */
	init_statistics_list(&g_f_policy_list);
	/*** by zdw add init session_table**/
	if(UTAF_OK != net_init())
	{
		return UTAF_FAIL;
	}

	//flow_event_sock_busi(NULL);
#if 0
	/* 初始化debug信息 */
	g_flow_trace.conf_file = FLOW_TRACE_FILE_PATH;
	g_flow_trace.entry_name = "flow_proc";
	INIT_TRACE_MODULE(g_flow_trace);
	FLOW_SET_DEBUG(g_flow_debug_cfg);
#endif
	show_flow_config(&g_flow_info_cfg, &g_flow_debug_cfg);
#if 0
	if (g_flow_debug_cfg.flowevent_store_disenable == 1) {
		thread_num = 2; 
		}
#endif
#if 1
	/* 开启各个业务线程 */
	for (i = 0; i < THREAD_FLOW_NUM; ++i) {
		retn = flow_create_pthreads(tid, i);
		if (retn != 0) {
			goto EXIT;
		}
	}

	/* init udp sock, recv ip, account */
	gms_udp_sock_init();

#if 0
	sleep(3);
	printf("test)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\n");
	long p;
	while(1)
	{
	 p=ip_find_id(3232292452);
     if(0 == p)
		printf("host not can found ip's id &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");	 
	 else
		 printf("host found suc id = [%ld] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",p);
	 sleep(1);
	 p=ip_find_id(1692313792);
     if(0 == p)
		printf("net not can found ip's id &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");	 
	 else
		 printf("net found suc id = [%ld] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",p);
	 sleep(1);

	}
#endif
	/* 维护线程 */

	while (1) {
		for (i = 0; i < THREAD_FLOW_NUM; ++i) {
			retn = pthread_tryjoin_np(tid[i], &tret);
			if (retn != 0) {
				continue;
			}

			//FLOW_DEBUG(DEBUG_TYPE_TRUNK, "===== thread %d return! ======");
            CA_LOG(LOG_MODULE, LOG_PROC, "====== thread %d return! ====",i);
			retn = flow_create_pthreads(tid, i);
			if (retn != 0) {
				goto EXIT;
			}
		}
		sleep(5);
	}

#endif
EXIT:
	clear_all_statistics_list(&g_f_policy_list);
	net_mnt_ip_finish();
	Destroy_Message();
	close(sockfd);
	//CLEAN_TRACE_MODULE();

	gms_udp_sock_exit();
	return 0;
}
