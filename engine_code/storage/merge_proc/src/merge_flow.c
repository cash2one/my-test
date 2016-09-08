/**
 * @file merge_flow.c
 * @brief  流量合并流程
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-26
 */
#include "gms_merge.h"


#define FLOW_EVENT_DELAY_TIME       (120)

static cfg_desc g_merge_flow[] = {

	/* critical time */
	{"FLOW_MERGE_TIME", "min_to_hour", &(g_merge_info_cfg.flow_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	/* flow merge last time */
	{"FLOW_MERGE_TIME", "hour_to_day", &(g_merge_info_cfg.flow_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	{"", "", NULL, 0, 0, 0, 0, ""},
};

void make_flow_merge_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end)
{
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (dev_id,protocol_id,ftime,pps,bps,sip,dip,query_id) SELECT dev_id,protocol_id,'%s',AVG(pps),AVG(bps),sip,dip,query_id FROM %s WHERE ftime < '%s' and ftime >= '%s' GROUP BY dev_id,protocol_id,sip,dip,query_id", dst_table_name, end, src_table_name, end, start);
}


/* -------------------------------------------*/
/**
 * @brief  流量 分钟表 合并 小时表 
 * @returns   
 */
/* -------------------------------------------*/
static int do_merge_hour_to_day(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	char end_merge_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];

	critical_time = make_merge_critical_time_day(g_merge_info_cfg.flow_hour_to_day,0, 0);
	time(&now_time);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + FLOW_EVENT_DELAY_TIME) {
        retn = 1;
		goto END;
	}

	/* need merge */

	/* 得到合并起始时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);

	/* 得到合并终止时间 */
	get_timestr_offset(end_merge_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_FLOW, "\n====> FLOW [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_merge_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "\n====> FLOW [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_merge_time_str);
	make_flow_merge_sql(sql_cmd, TABLE_NAME_FLOW_HOUR, TABLE_NAME_FLOW_DAY, critical_time_str, end_merge_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_FLOW, "\nFLOW [Merge hour -> DAY] [%s]->[%s] ERROR!\n", critical_time_str, end_merge_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "\nFLOW [Merge hour -> DAY] [%s]->[%s] ERROR!\n", critical_time_str, end_merge_time_str);
		syslog(LOG_NOTICE,"\nmerge_flow.c: in do_merge_hour_to_day(),FLOW [Merge hour -> DAY] [%s]->[%s] ERROR!\n",critical_time_str, end_merge_time_str);
		goto END;
	}

	/* 更新 hour_to_day */
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "FLOW_MERGE_TIME", "hour_to_day", NULL, end_merge_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;

}

/* -------------------------------------------*/
/**
 * @brief  流量 分钟表 合并 小时表 
 * @returns   
 */
/* -------------------------------------------*/
static int do_merge_min_to_hour(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	char end_merge_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];

	critical_time = make_merge_critical_time_hour(g_merge_info_cfg.flow_min_to_hour);
	time(&now_time);
	if (now_time - critical_time < (ONE_HOUR_TIME_SEC + FLOW_EVENT_DELAY_TIME)) {//+120意味着 延迟2分钟合并
        retn = 1;
		goto END;
	}

	/* need merge */

	/* 得到起始合并时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);

	/* 得到结束合并时间 */
	get_timestr_offset(end_merge_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_FLOW, "\n====> FLOW [Merge min -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_merge_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "\n====> FLOW [Merge min -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_merge_time_str);
	make_flow_merge_sql(sql_cmd, TABLE_NAME_FLOW_MIN, TABLE_NAME_FLOW_HOUR, critical_time_str, end_merge_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_FLOW, "\nFLOW[Merge min -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_merge_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "\nFLOW[Merge min -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_merge_time_str);
		syslog(LOG_NOTICE,"\nmerge_flow.c: in do_merge_min_to_hour(),FLOW[Merge min -> hour] [%s]->[%s] ERROR!\n",critical_time_str, end_merge_time_str);
		goto END;
	}

	/* 更新 min_to_hour */
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "FLOW_MERGE_TIME", "min_to_hour", NULL, end_merge_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief 根据系统时间来逐步调整最后合并时间
 */
/* -------------------------------------------*/
void change_merge_flow_time(void)
{
	char time_format[] = "%Y-%m-%d %X";
    int time_x = 0;
    char now_sys_time_str[TIME_FORMAT_SIZE];

    /* 如果合并进度时间大于当前系统时间，需要将合并进度时间调至当前系统时间 */
    get_systime_str_format(now_sys_time_str, time_format);
    time_x = time_cmp_format(now_sys_time_str, g_merge_info_cfg.flow_min_to_hour, time_format); 
    if (time_x < 0) {
        write_conf(MERGE_TIME_CONFIG_PATH, "FLOW_MERGE_TIME", "min_to_hour", NULL, now_sys_time_str);
        strncpy(g_merge_info_cfg.flow_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([FLOW]min_to_hour) [%s] -> [%s]", g_merge_info_cfg.flow_min_to_hour, now_sys_time_str);
    }
    time_x = time_cmp_format(g_merge_info_cfg.flow_min_to_hour, g_merge_info_cfg.flow_hour_to_day, time_format); 
    if (time_x < 0) {
        write_conf(MERGE_TIME_CONFIG_PATH, "FLOW_MERGE_TIME", "hour_to_day", NULL, g_merge_info_cfg.flow_min_to_hour);
        strncpy(g_merge_info_cfg.flow_hour_to_day, g_merge_info_cfg.flow_min_to_hour, TIME_FORMAT_SIZE);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([FLOW]hour_to_day) [%s] -> [%s]", g_merge_info_cfg.flow_hour_to_day, g_merge_info_cfg.flow_min_to_hour);
    }

}

void *merge_flow_busi(void *arg)
{
	pthread_t tid;
	tid = pthread_self();

	merge_conn_t flow_conn;
	char time_format[] = "%Y-%m-%d %X";
	int retn = 0;
    int hour_merge_ok = 0;
    int day_merge_ok = 0;
    int count = 0;
    int con_retn = 0;

    con_retn = merge_connect_db(&flow_conn, THREAD_FLOW_MERGE);
    if (con_retn == -1) {                                                                                             
        goto EXIT;                                                                                                
    }                                                                                                             
    else if (con_retn == 1) {                                                                                         
        goto EXIT_1;                                                                                              
    }       
	while (1) {    
		/* 读取合并进度时间 */
		pthread_mutex_lock(&time_config_file_mutex);
		retn = common_read_conf(MERGE_TIME_CONFIG_PATH, g_merge_flow);
		if (retn < 0) {
			MERGE_ERROR(DEBUG_TYPE_FLOW, "Read config %s ERROR", MERGE_TIME_CONFIG_PATH);
			CA_LOG(LOG_MODULE, LOG_PROC, "Read config %s ERROR", MERGE_TIME_CONFIG_PATH);
			pthread_mutex_unlock(&time_config_file_mutex);
			goto EXIT_2;
		}
		pthread_mutex_unlock(&time_config_file_mutex);


        /* 如果 进度配置文件 为空文件 则创建 */
        if (strcmp(g_merge_info_cfg.flow_min_to_hour, DEFAULT_MERGE_TIME) == 0) {
           get_systime_str_format(g_merge_info_cfg.flow_min_to_hour, time_format);
           get_systime_str_format(g_merge_info_cfg.flow_hour_to_day, time_format);
           
           CA_LOG(LOG_MODULE, LOG_PROC, "WARNING!!!!%s is NULL, Reset all merge_flow_time to systime time!\n", MERGE_TIME_CONFIG_PATH);
           retn = create_cfg_file(MERGE_TIME_CONFIG_PATH, g_merge_flow);
           if (retn < 0) {
               MERGE_ERROR(DEBUG_TYPE_ATTACK, "Open file %s ERROR!\n", MERGE_TIME_CONFIG_PATH);
               CA_LOG(LOG_MODULE, LOG_PROC, "Open file %s ERROR!\n", MERGE_TIME_CONFIG_PATH);
               goto EXIT_2;
           }
        }


        change_merge_flow_time();

		/* 合并分钟表 到 小时表 */
		hour_merge_ok = do_merge_min_to_hour(&flow_conn);

		/* 合并小时表 到 天表 */
        if (hour_merge_ok == 1) {
		    day_merge_ok = do_merge_hour_to_day(&flow_conn);
        }

		sleep(1);
        ++count;
        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            merge_disconnect_db(&flow_conn, THREAD_FLOW_MERGE);
            retn = merge_connect_db(&flow_conn, THREAD_FLOW_MERGE);
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
        }              
	}

EXIT_2:
	/* 关闭redis链接 */
	grd_disconnect(flow_conn.redis_conn);	
EXIT_1:
	/* 关闭postgreSQL链接 */
	gpq_disconnect(flow_conn.psql_conn);	

EXIT:
	pthread_exit((void*)tid);
}
