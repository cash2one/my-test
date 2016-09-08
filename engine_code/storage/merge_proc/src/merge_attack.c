/**
 * @file merge_attack.c
 * @brief  特征事件合并流程 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-26
 */

#include "gms_merge.h"


#define ATTACK_EVENT_DELAY_TIME         (600)

static unsigned int g_need_change_time = 0;

static cfg_desc g_merge_attack[] = {

	/* When merge */
	{"CRITICAL_TIME", "merge_hour", &(g_merge_info_cfg.merge_hour), CFG_SIZE_UINT8, CFG_TYPE_UINT8, 1, 0, "0"},
	{"CRITICAL_TIME", "merge_min", &(g_merge_info_cfg.merge_min), CFG_SIZE_UINT8, CFG_TYPE_UINT8, 0, 0, "15"},

	/* attack merge last time */
	{"ATTACK_MERGE_TIME", "tmp_to_min", &(g_merge_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"ATTACK_MERGE_TIME", "min_to_hour", &(g_merge_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ATTACK_MERGE_TIME", "hour_to_day", &(g_merge_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ATTACK_MERGE_TIME", "day_to_week", &(g_merge_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ATTACK_MERGE_TIME", "week_to_month", &(g_merge_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ATTACK_MERGE_TIME", "month_to_year", &(g_merge_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/* 3rd merge last time */
	{"3RD_MERGE_TIME", "tmp_to_min", &(g_merge_info_cfg._3rd_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"3RD_MERGE_TIME", "min_to_hour", &(g_merge_info_cfg._3rd_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"3RD_MERGE_TIME", "hour_to_day", &(g_merge_info_cfg._3rd_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"3RD_MERGE_TIME", "day_to_week", &(g_merge_info_cfg._3rd_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"3RD_MERGE_TIME", "week_to_month", &(g_merge_info_cfg._3rd_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"3RD_MERGE_TIME", "month_to_year", &(g_merge_info_cfg._3rd_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	{"", "", NULL, 0, 0, 0, 0, ""},
};

/* -------------------------------------------*/
/**
 * @brief  合并特征事件表到更高级的表的数据库语句
 *		   begin_time 取最小
 *		   end_time 取最大
 *
 * @param sql_cmd
 * @param src_table_name	
 * @param dst_table_name
 * @param start				筛选的时间起点
 * @param end				筛选的时间终点
 */
/* -------------------------------------------*/
/*static void make_merge_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
//	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid,etype,begin_time,end_time,isccserver,daddr,saddr,sport,dport,enum,risk_level,query_id) select devid,ename,nameid,etype,min(begin_time),max(end_time),isccserver,daddr,saddr,sport,dport,sum(enum),risk_level,query_id from %s where begin_time between '%s' and '%s' group by devid,ename,etype,daddr,saddr,sport,dport,nameid,isccserver,risk_level,query_id", dst_table_name, src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid,etype,begin_time,end_time,isccserver,daddr,saddr,sport,dport,enum,risk_level,query_id) SELECT devid,ename,nameid,etype,MIN(begin_time),MAX(end_time),isccserver,daddr,saddr,sport,dport,SUM(enum),risk_level,query_id FROM %s WHERE begin_time >= '%s' AND end_time < '%s' GROUP BY devid,ename,etype,daddr,saddr,sport,dport,nameid,isccserver,risk_level,query_id", dst_table_name, src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid,etype,begin_time,end_time,isccserver,daddr,saddr,sport,dport,enum,risk_level,query_id) SELECT devid,ename,nameid,etype,MIN(begin_time),MAX(end_time),isccserver,daddr,saddr,sport,dport,SUM(enum),risk_level,query_id FROM %s WHERE begin_time >= '%s' AND end_time < '%s' GROUP BY devid,ename,etype,daddr,saddr,sport,dport,nameid,isccserver,risk_level,query_id", dst_table_name, src_table_name, start, end);
}*/
static void make_merge_sql_handletime(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
//	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid,etype,begin_time,end_time,isccserver,daddr,saddr,sport,dport,enum,risk_level,query_id) select devid,ename,nameid,etype,min(begin_time),max(end_time),isccserver,daddr,saddr,sport,dport,sum(enum),risk_level,query_id from %s where begin_time between '%s' and '%s' group by devid,ename,etype,daddr,saddr,sport,dport,nameid,isccserver,risk_level,query_id", dst_table_name, src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid,etype,begin_time,end_time,isccserver,daddr,saddr,sport,dport,enum,risk_level,query_id) SELECT devid,ename,nameid,etype,MIN(begin_time),MAX(end_time),isccserver,daddr,saddr,sport,dport,SUM(enum),risk_level,query_id FROM %s WHERE begin_time >= '%s' AND end_time < '%s' GROUP BY devid,ename,etype,daddr,saddr,sport,dport,nameid,isccserver,risk_level,query_id", dst_table_name, src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid,etype,begin_time,end_time,handledeadline,isccserver,daddr,saddr,sport,dport,enum,risk_level,query_id) SELECT devid,ename,nameid,etype,MIN(begin_time),MAX(end_time),MAX(handledeadline),isccserver,daddr,saddr,sport,dport,SUM(enum),risk_level,query_id FROM %s WHERE handledeadline >= '%s' AND handledeadline < '%s' GROUP BY devid,ename,etype,daddr,saddr,sport,dport,nameid,isccserver,risk_level,query_id", dst_table_name, src_table_name, start, end);
}
#if 0
static void make_merge_query_sql(char *sql_cmd, char* src_table_name, char *dst_table_name, char *start, char *end) 
{
    char table_name[TABLE_NAME_SIZE];
    struct tm this_time;
	const char *time_format = "%Y-%m-%d %X";	

    memset(table_name, 0 , TABLE_NAME_SIZE);

    /* get month_id and day_id */
	memset(&this_time, 0, sizeof(struct tm));
    strptime(start, time_format, &this_time);

    
    snprintf(table_name, TABLE_NAME_SIZE, "%s%s%d%s%d", dst_table_name, TABLE_SUFFIX_MONTH, this_time.tm_mon+1, TABLE_SUFFIX_DAY, this_time.tm_mday);
    
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
    
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid, isccserver, etype,begin_time,end_time,handledeadline, eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport, eventdetail, enum,risk_level,query_id, month_id, day_id) SELECT devid,ename,nameid, isccserver, etype, MIN(begin_time), MAX(end_time), MIN(handledeadline), eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport, MAX(eventdetail), SUM(enum),risk_level,query_id,month_id,day_id FROM %s WHERE begin_time >= '%s' AND end_time < '%s' GROUP BY devid,ename, nameid, isccserver, etype, eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport, risk_level,query_id, month_id, day_id", table_name, src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid, isccserver, etype,begin_time,end_time,handledeadline, eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport,eventdetail, enum,risk_level,query_id, month_id, day_id) SELECT devid,ename,nameid, isccserver, etype, MIN(begin_time), MAX(end_time), MIN(handledeadline), eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport, eventdetail,SUM(enum),risk_level,query_id,month_id,day_id FROM %s WHERE begin_time >= '%s' AND end_time < '%s' GROUP BY devid,ename, nameid, isccserver, etype, eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport,eventdetail, risk_level,query_id, month_id, day_id", table_name, src_table_name, start, end);

	printf("+++++++++++++sql_cmd:%s\n", sql_cmd);
}
#endif
/*static void make_delete_sql(char *sql_cmd, char *table_name, char *start, char *end)
{
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "DELETE FROM %s WHERE begin_time >= '%s' AND end_time <= '%s'", table_name, start, end);
	printf("+++++++++++sql_cmd:%s\n", sql_cmd);
}*/
static void make_delete_sql_handletime(char *sql_cmd, char *table_name, char *start, char *end)
{
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "DELETE FROM %s WHERE handledeadline >= '%s' AND handledeadline <= '%s'", table_name, start, end);
	printf("+++++++++++sql_cmd:%s\n", sql_cmd);
}

/* -------------------------------------------*/
/**
 * @brief     月表合并年表
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_month_to_year(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t critical_time_zero = 0;
	struct tm  tmp_time;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
	char sql_cmd[MERGE_SQL_CMD_SIZE];
	struct tm *time_p = NULL;
	int leap = 0;
	time_t one_year_sec = 0;
    int att_ok , _3rd_ok = 0;

	
	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_year(now_time_str, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	now_critical_time = now_critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);

	critical_time = make_merge_critical_time_year(g_merge_info_cfg.att_month_to_year, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	
	time_p = localtime_r(&critical_time,&tmp_time);
	leap = is_leap_year(time_p->tm_year+1900);
	if (leap == 1) {
		one_year_sec = ONE_YEAR_TIME_SEC_366;
	}
	else {
		one_year_sec = ONE_YEAR_TIME_SEC_365;
	}

	if ((now_time - critical_time) < one_year_sec + ATTACK_EVENT_DELAY_TIME) {
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time_zero = critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_year_sec, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);

	make_merge_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_MONTH, TABLE_NAME_ATTACK_YEAR, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_year_sec;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "month_to_year", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_year(g_merge_info_cfg._3rd_month_to_year, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);

	time_p = localtime_r(&critical_time,&tmp_time);
	leap = is_leap_year(time_p->tm_year+1900);
	if (leap == 1) {
		one_year_sec = ONE_YEAR_TIME_SEC_366;
	}
	else {
		one_year_sec = ONE_YEAR_TIME_SEC_365;
	}

	if ((now_time - critical_time) < one_year_sec + ATTACK_EVENT_DELAY_TIME) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time_zero= critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_year_sec, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_3RD_MONTH, TABLE_NAME_3RD_YEAR, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_year_sec;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "month_to_year", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief     周表合并月表
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_week_to_month(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t critical_time_zero = 0;
	struct tm  tmp_time;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
	char sql_cmd[MERGE_SQL_CMD_SIZE];
	struct tm *time_p = NULL;
    int att_ok, _3rd_ok = 1;
    time_t one_month_sec = 0;

	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_month(now_time_str, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	now_critical_time = now_critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);

	critical_time = make_merge_critical_time_month(g_merge_info_cfg.att_week_to_month, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec + ATTACK_EVENT_DELAY_TIME) {
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time_zero = critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK[Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);

	make_merge_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_DAY, TABLE_NAME_ATTACK_MONTH, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_month(g_merge_info_cfg._3rd_week_to_month, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec + ATTACK_EVENT_DELAY_TIME) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

    /* need 3rd merge */
	/* 得到需要合并数据的时间 */
	critical_time_zero= critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_3RD_DAY, TABLE_NAME_3RD_MONTH, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief     天表合并周表
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_day_to_week(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t critical_time_zero = 0;
	struct tm  tmp_time;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	char sql_cmd[MERGE_SQL_CMD_SIZE];
	struct tm *time_p = NULL;
    int att_ok , _3rd_ok = 0;


	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_week(now_time_str, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);

	critical_time = make_merge_critical_time_week(g_merge_info_cfg.att_day_to_week, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time_zero = critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);

	make_merge_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_DAY, TABLE_NAME_ATTACK_WEEK, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_week(g_merge_info_cfg._3rd_day_to_week, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time_zero = critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_3RD_DAY, TABLE_NAME_3RD_WEEK, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief    小时表合并天表 
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_hour_to_day(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t critical_time_zero = 0;
	struct tm  tmp_time;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];
    int att_ok , _3rd_ok = 0;


	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_day(now_time_str, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	now_critical_time = now_critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);

	critical_time = make_merge_critical_time_day(g_merge_info_cfg.att_hour_to_day, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	if ((now_time - critical_time) < ONE_DAY_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time_zero= critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time_zero,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_HOUR, TABLE_NAME_ATTACK_DAY, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_day(g_merge_info_cfg._3rd_hour_to_day, g_merge_info_cfg.merge_hour, g_merge_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_merge_info_cfg.merge_hour*3600 + g_merge_info_cfg.merge_min*60);
	time_p = localtime_r(&critical_time,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_3RD_HOUR, TABLE_NAME_3RD_DAY, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief    分钟表合并小时表 
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_min_to_hour(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	struct tm  tmp_time;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];
    int att_ok , _3rd_ok = 0;


	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_hour(now_time_str);

	critical_time = make_merge_critical_time_hour(g_merge_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < (ONE_HOUR_TIME_SEC + ATTACK_EVENT_DELAY_TIME)) { // + 300 意味着 延迟5分钟合并
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	time_p = localtime_r(&critical_time,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "====> ATTACK [Merge min -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "====> ATTACK [Merge min -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_MIN, TABLE_NAME_ATTACK_HOUR, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge min -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge min -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_hour(g_merge_info_cfg._3rd_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	time_p = localtime_r(&critical_time,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge min -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge min -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_3RD_MIN, TABLE_NAME_3RD_HOUR, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge min -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge min -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief    TMP表合并分钟表 
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_tmp_to_min(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	struct tm  tmp_time;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
//	char last_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];
    int att_ok , _3rd_ok = 0;
	//char time_cmd[MERGE_SQL_CMD_SIZE];
	//char count_cmd[MERGE_SQL_CMD_SIZE];
	//char min_time[TIME_FORMAT_SIZE];
	//char min_zero_time[TIME_FORMAT_SIZE];
	//int sub_time_cp = 0;
	//char min_next_zero_time[TIME_FORMAT_SIZE];
	//char now_zero_time[TIME_FORMAT_SIZE];
	//int count = 0;
	//int i = 0;
	//int sub_time = 0;

	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_min(now_time_str);

	critical_time = make_merge_critical_time_min(g_merge_info_cfg.att_tmp_to_min);
//	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC ) {
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	time_p = localtime_r(&critical_time,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "====> ATTACK [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "====> ATTACK [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_TMP, TABLE_NAME_ATTACK_MIN, critical_time_str, end_critical_time_str);
	//make_merge_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, TABLE_NAME_ATTACK_MIN, critical_time_str, end_critical_time_str);
	printf("#########################%s#######################\n",sql_cmd);


	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"\nmerge_attack.c: in do_merge_tmp_to_min(),ATTACK[Merge tmp -> min] [%s]->[%s] ERROR!\n",critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}
	printf("tmp->min  ********* finsh\n");
	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "====> ATTACK [Merge tmp -> query]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "====> ATTACK [Merge tmp -> query]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	/*删除合并过的attacktmp表*/
	//make_delete_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
	make_delete_sql_handletime(sql_cmd, TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
		if (retn < 0) {
			MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
			CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP,  critical_time_str, end_critical_time_str);
			syslog(LOG_NOTICE,"\nmerge_attack.c: in do_merge_tmp_to_min(),ATTACK[Delete %s] [%s]->[%s] ERROR!\n",TABLE_NAME_ATTACK_TMP,  critical_time_str, end_critical_time_str);
			goto _3RD_MERGE;
		}
	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP,  critical_time_str, end_critical_time_str);


#if 0
    /* 生成 query 表合并语句 */

	printf("+++++++++++++new code++++++++++\n");
	//snprintf(count_cmd, MERGE_SQL_CMD_SIZE, "select count(*) from %s", TABLE_NAME_ATTACK_TMP);	
	//count = gpg_get_value(conn->psql_conn, count_cmd,0,0);
	//printf("------------count:%d\n", count);
		snprintf(time_cmd, MERGE_SQL_CMD_SIZE, "select min(%s) from %s", TMP_TIME_NAME, TABLE_NAME_ATTACK_TMP);
		strncpy(min_time, gpq_get_value(conn->psql_conn, time_cmd, 0, 0), TIME_FORMAT_SIZE - 1);
		printf("timenim=%s*************************\n",min_time);
	for(i = 0; i < 365; ++i) {
		printf("i=%d******************%s**********\n",i,min_time);
		get_current_zero_time(min_time, min_zero_time);
		get_current_zero_time(now_time_str, now_zero_time);
		get_current_next_zero_time(min_zero_time, min_next_zero_time);
		//critical_sub_time = time_cmp(now_zero_time, min_zero_time);
		sub_time = time_cmp(g_merge_info_cfg.att_tmp_to_min, min_time);
		//day_num = critical_sub_time / ONE_DAY_TIME_SEC;
		if(sub_time > 0) {
			/*strptime(min_time, time_format, time_p);
			if((day_num / 30) < 1) {
				time_p->tm_mday -= day_num;
				strftime(last_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
			} else {
				time_p->tm_mon = (time_p->tm_mon + 1) - (day_num / 30);
				time_p->tm_mday = time_p->tm_mday+((day_num/30)*30) - day_num;
				strftime(last_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
			} */
			sub_time_cp=time_cmp(min_next_zero_time,g_merge_info_cfg.att_tmp_to_min);
			if(sub_time_cp > 0)
			{
				strcpy(min_next_zero_time,g_merge_info_cfg.att_tmp_to_min);
			}
			/*store_proc 中已入t_event_attackquery_m_d,此处不再入*/
/*    		make_merge_query_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, TABLE_NAME_ATTACK_QUERY, min_zero_time, min_next_zero_time);
			printf("1111111111111\n");	
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) {
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n",min_zero_time, min_next_zero_time);
				CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n", min_zero_time, min_next_zero_time);
				goto _3RD_MERGE;
			}*/
    		make_delete_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, min_zero_time, min_next_zero_time);
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) {
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, min_zero_time, min_next_zero_time);
				CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP,  min_zero_time, min_next_zero_time);
				syslog(LOG_NOTICE,"\nmerge_attack.c: in do_merge_tmp_to_min(),ATTACK[Delete %s] [%s]->[%s] ERROR!\n",TABLE_NAME_ATTACK_TMP,  min_zero_time, min_next_zero_time);
				goto _3RD_MERGE;
			}
			MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP,  min_zero_time, min_next_zero_time);
			//CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP,  min_zero_time, min_next_zero_time);
		strncpy(min_time,min_next_zero_time, TIME_FORMAT_SIZE - 1);
		printf("min_time=%s  g_merge_info_cfg.att_tmp_to_min=%s\n",min_time,g_merge_info_cfg.att_tmp_to_min);


		} else {
			/*store_proc 中已入t_event_attackquery_m_d,此处不再入*/
/*    		make_merge_query_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, TABLE_NAME_ATTACK_QUERY, critical_time_str, end_critical_time_str);
			printf("22222222222222\n");	
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) {
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
				CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
				syslog(LOG_NOTICE,"\nmerge_attack.c: in do_merge_tmp_to_min(),ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
				goto _3RD_MERGE;

			}*/
			make_delete_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
			//make_delete_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, critical_time_str, min_zero_time);
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) {
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
				CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
				syslog(LOG_NOTICE,"\nmerge_attack.c: in do_merge_tmp_to_min(),ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
			goto _3RD_MERGE;
			}
			MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK[Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
			//CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
			break;
		}
	}	
#endif

#if 0
	/*attack合并query*/
	make_merge_query_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, TABLE_NAME_ATTACK_QUERY, critical_time_str, end_critical_time_str);
	//make_merge_query_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, TABLE_NAME_ATTACK_QUERY, critical_time_str, min_time);
	printf("22222222222222\n");	
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
		goto _3RD_MERGE;

	}
    /* 生成删除sql语句 */
    make_delete_sql(sql_cmd, TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}
	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK[Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_ATTACK_TMP, critical_time_str, end_critical_time_str);
#endif


	/* 更新 tmp_to_min */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_min(g_merge_info_cfg._3rd_tmp_to_min);
//	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + ATTACK_EVENT_DELAY_TIME) {
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	time_p = localtime_r(&critical_time,&tmp_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_sql_handletime(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_MIN, critical_time_str, end_critical_time_str);
    printf("--->do {%s}\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"3RD [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}


//	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "====> 3RD [Merge tmp -> query]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
//	CA_LOG(LOG_MODULE, LOG_PROC, "====> 3RD [Merge tmp -> query]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	/*删除合并过的3rdtmp表*/
	make_delete_sql_handletime(sql_cmd, TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP,  critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP,  critical_time_str, end_critical_time_str);
	}
    /* 生成 query 表合并语句 */
   
#if 0
	//snprintf(count_cmd, MERGE_SQL_CMD_SIZE, "select count(*) from %s", TABLE_NAME_3RD_TMP);
	//printf("*********count_cmd:%s\n", count_cmd);	
	//count = gpq_get_value(conn->psql_conn, count_cmd,0,0);
	//printf("++++++++++++3rd_count:%d\n", count);
	snprintf(time_cmd, MERGE_SQL_CMD_SIZE, "select min(%s) from %s", TMP_TIME_NAME, TABLE_NAME_3RD_TMP);
	strncpy(min_time, gpq_get_value(conn->psql_conn, time_cmd, 0, 0), TIME_FORMAT_SIZE - 1);
	printf("+++++++++++++++min_time:%s\n", min_time);
	for(i = 0; i<365; ++i) {
		get_current_zero_time(min_time, min_zero_time);
		get_current_zero_time(now_time_str, now_zero_time);
		get_current_next_zero_time(min_zero_time, min_next_zero_time);
		sub_time = time_cmp(g_merge_info_cfg._3rd_tmp_to_min, min_time);
	//	day_num = sub_time / ONE_DAY_TIME_SEC;
		if(sub_time > 0) {
			/*strptime(now_time_str, time_format, time_p);
			if((day_num / 30) < 1) {
				time_p->tm_mday -= day_num;
				strftime(last_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
			} else {
				time_p->tm_mon = (time_p->tm_mon + 1) - (day_num / 30);
				time_p->tm_mday = time_p->tm_mday+((day_num/30)*30) - day_num;
				strftime(last_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
			} */
			sub_time_cp=time_cmp(min_next_zero_time, g_merge_info_cfg.att_tmp_to_min);
			if(sub_time_cp > 0)
			{
				strcpy(min_next_zero_time,g_merge_info_cfg._3rd_tmp_to_min);
			}
			/*store_proc 中已入t_event_3rdquery_m_d,此处不再入*/
			/*printf("1111111111111111111111\n");
    		make_merge_query_sql(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_QUERY, min_zero_time, min_next_zero_time);
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) { 
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", min_zero_time, min_next_zero_time);
				CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", min_zero_time, min_next_zero_time);
				syslog(LOG_NOTICE,"3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", min_zero_time, min_next_zero_time);
				goto END;
			}*/
			make_delete_sql(sql_cmd, TABLE_NAME_3RD_TMP, min_zero_time, min_next_zero_time);
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) {
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, min_zero_time, min_next_zero_time);
				CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP,  min_zero_time, min_next_zero_time);
				syslog(LOG_NOTICE,"3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP,  min_zero_time, min_next_zero_time);
				goto END;
			}
			MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP,  min_zero_time, min_next_zero_time);
			//CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP,  min_zero_time, min_next_zero_time);
		strncpy(min_time, min_next_zero_time, TIME_FORMAT_SIZE - 1);

		    
		} else {
			/*store_proc 中已入t_event_3rdquery_m_d,此处不再入*/
			/*printf("22222222222222222222222\n");
    		make_merge_query_sql(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_QUERY, critical_time_str, end_critical_time_str);
    		//make_merge_query_sql(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_QUERY, critical_time_str, min_zero_time);
			retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
			if (retn < 0) {
				MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
				CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
				syslog(LOG_NOTICE,"3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
				goto END;


			}*/
		make_delete_sql(sql_cmd, TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
				retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
				if (retn < 0) {
					MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
					CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
					syslog(LOG_NOTICE,"3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
				goto END;
			}
				MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
				//CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
				break;


		}
	}
#if 0	
	make_merge_query_sql(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_QUERY, critical_time_str, end_critical_time_str);
    printf("--->do {%s}\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge tmp -> query] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge tmp -> query] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}
#endif
#endif


#if 0
	make_merge_query_sql(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_QUERY, critical_time_str, end_critical_time_str);
	//make_merge_query_sql(sql_cmd, TABLE_NAME_3RD_TMP, TABLE_NAME_3RD_QUERY, critical_time_str, min_zero_time);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Merge tmp -> query] [%s]->[%s] ERROR!\n", last_time_str, last_time_str);
		goto END;


	}
    /* 生成删除sql语句 */
    make_delete_sql(sql_cmd, TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD[Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}
	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
#endif

	/* 更新 tmp_to_min */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime_r(&end_time,&tmp_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_config_file_mutex);
	write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_config_file_mutex);
END:

	return retn;
}

/* -------------------------------------------*/
/**
 * @brief 根据系统时间来逐步调整最后合并时间
 */
/* -------------------------------------------*/
void change_merge_attack_time(void)
{
	char time_format[] = "%Y-%m-%d %X";
    int time_x = 0;
    char now_sys_time_str[TIME_FORMAT_SIZE];

    get_systime_str_format(now_sys_time_str, time_format);

#if 1
    /* ----- att 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_merge_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]tmp_to_min) [%s] -> [%s]", g_merge_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_merge_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_merge_info_cfg.att_tmp_to_min, g_merge_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "min_to_hour", NULL, g_merge_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]min_to_hour) [%s] -> [%s]", g_merge_info_cfg.att_min_to_hour, g_merge_info_cfg.att_tmp_to_min);
        strncpy(g_merge_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_merge_info_cfg.att_min_to_hour, g_merge_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "hour_to_day", NULL, g_merge_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]hour_to_day) [%s] -> [%s]", g_merge_info_cfg.att_hour_to_day, g_merge_info_cfg.att_min_to_hour);
        strncpy(g_merge_info_cfg.att_hour_to_day, g_merge_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_merge_info_cfg.att_hour_to_day, g_merge_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "day_to_week", NULL, g_merge_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]day_to_week) [%s] -> [%s]", g_merge_info_cfg.att_day_to_week, g_merge_info_cfg.att_hour_to_day);
        strncpy(g_merge_info_cfg.att_day_to_week, g_merge_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_merge_info_cfg.att_day_to_week, g_merge_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "week_to_month", NULL, g_merge_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]week_to_month) [%s] -> [%s]", g_merge_info_cfg.att_week_to_month, g_merge_info_cfg.att_day_to_week);
        strncpy(g_merge_info_cfg.att_week_to_month, g_merge_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_merge_info_cfg.att_week_to_month, g_merge_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "ATTACK_MERGE_TIME", "month_to_year", NULL, g_merge_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]month_to_year) [%s] -> [%s]", g_merge_info_cfg.att_month_to_year, g_merge_info_cfg.att_week_to_month);
        strncpy(g_merge_info_cfg.att_month_to_year, g_merge_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /*---- 3rd 比较 ----*/
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_merge_info_cfg._3rd_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([3RD]tmp_to_min) [%s] -> [%s]", g_merge_info_cfg._3rd_tmp_to_min, now_sys_time_str);
        strncpy(g_merge_info_cfg._3rd_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE);
    }
    /* tmp->min时间  和 min->hour */
    time_x = time_cmp_format(g_merge_info_cfg._3rd_tmp_to_min, g_merge_info_cfg._3rd_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "min_to_hour", NULL, g_merge_info_cfg._3rd_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([3RD]min_to_hour) [%s] -> [%s]", g_merge_info_cfg._3rd_min_to_hour, g_merge_info_cfg._3rd_tmp_to_min);
        strncpy(g_merge_info_cfg._3rd_min_to_hour, g_merge_info_cfg._3rd_tmp_to_min, TIME_FORMAT_SIZE);
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_merge_info_cfg._3rd_min_to_hour, g_merge_info_cfg._3rd_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "hour_to_day", NULL, g_merge_info_cfg._3rd_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([3RD]hour_to_day) [%s] -> [%s]", g_merge_info_cfg._3rd_hour_to_day, g_merge_info_cfg._3rd_min_to_hour);
        strncpy(g_merge_info_cfg._3rd_hour_to_day, g_merge_info_cfg._3rd_day_to_week, TIME_FORMAT_SIZE);
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_merge_info_cfg._3rd_hour_to_day, g_merge_info_cfg._3rd_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "day_to_week", NULL, g_merge_info_cfg._3rd_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([3RD]day_to_week) [%s] -> [%s]", g_merge_info_cfg._3rd_day_to_week, g_merge_info_cfg._3rd_hour_to_day);
        strncpy(g_merge_info_cfg._3rd_day_to_week, g_merge_info_cfg._3rd_hour_to_day, TIME_FORMAT_SIZE);
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_merge_info_cfg._3rd_day_to_week, g_merge_info_cfg._3rd_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "week_to_month", NULL, g_merge_info_cfg._3rd_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([3RD]week_to_month) [%s] -> [%s]", g_merge_info_cfg._3rd_week_to_month, g_merge_info_cfg._3rd_day_to_week);
        strncpy(g_merge_info_cfg._3rd_week_to_month, g_merge_info_cfg._3rd_day_to_week, TIME_FORMAT_SIZE);
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_merge_info_cfg._3rd_week_to_month, g_merge_info_cfg._3rd_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_CONFIG_PATH, "3RD_MERGE_TIME", "month_to_year", NULL, g_merge_info_cfg._3rd_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([3RD]month_to_year) [%s] -> [%s]", g_merge_info_cfg._3rd_month_to_year, g_merge_info_cfg._3rd_week_to_month);
        strncpy(g_merge_info_cfg._3rd_month_to_year, g_merge_info_cfg._3rd_week_to_month, TIME_FORMAT_SIZE);
    }
#endif

}


void *merge_attack_busi(void *arg)
{
	pthread_t tid;
	tid = pthread_self();

	merge_conn_t attack_conn;
	char time_format[] = "%Y-%m-%d %X";
	int retn = 0;
    int min_merge_ok = 0;
    int hour_merge_ok = 0;
    int day_merge_ok = 0;
    int week_merge_ok = 0;
    int month_merge_ok = 0;
    int year_merge_ok = 0;
    int count = 0;                                                                                                
    int con_retn = 0; 

    con_retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);             
    if (con_retn == -1) {                                                                                             
        goto EXIT;                                                                                                
    }                                                                                                             
    else if (con_retn == 1) {                                                                                         
        goto EXIT_1;                                                                                              
    }             


	while (1) {
		/* 读取合并进度时间 */
		pthread_mutex_lock(&time_config_file_mutex);
		retn = common_read_conf(MERGE_TIME_CONFIG_PATH, g_merge_attack);
		if (retn < 0) {
			MERGE_ERROR(DEBUG_TYPE_ATTACK, "Read config %s ERROR", MERGE_TIME_CONFIG_PATH);
			CA_LOG(LOG_MODULE, LOG_PROC, "Read config %s ERROR", MERGE_TIME_CONFIG_PATH);
			pthread_mutex_unlock(&time_config_file_mutex);
			goto EXIT_2;
		}
		pthread_mutex_unlock(&time_config_file_mutex);


        if (strcmp(g_merge_info_cfg.att_tmp_to_min, DEFAULT_MERGE_TIME) == 0) {
           get_systime_str_format(g_merge_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_merge_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_merge_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_merge_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_merge_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_merge_info_cfg.att_month_to_year, time_format);

           get_systime_str_format(g_merge_info_cfg._3rd_tmp_to_min, time_format);
           get_systime_str_format(g_merge_info_cfg._3rd_min_to_hour, time_format);
           get_systime_str_format(g_merge_info_cfg._3rd_hour_to_day, time_format);
           get_systime_str_format(g_merge_info_cfg._3rd_day_to_week, time_format);
           get_systime_str_format(g_merge_info_cfg._3rd_week_to_month, time_format);
           get_systime_str_format(g_merge_info_cfg._3rd_month_to_year, time_format);
           
           CA_LOG(LOG_MODULE, LOG_PROC, "WARNING!!!!%s is NULL, Reset all merge_time to systime time!\n", MERGE_TIME_CONFIG_PATH);
           retn = create_cfg_file(MERGE_TIME_CONFIG_PATH, g_merge_attack);
           if (retn < 0) {
               MERGE_ERROR(DEBUG_TYPE_ATTACK, "Open file %s ERROR!\n", MERGE_TIME_CONFIG_PATH);
               CA_LOG(LOG_MODULE, LOG_PROC, "Open file %s ERROR!\n", MERGE_TIME_CONFIG_PATH);
               goto EXIT_2;
           }
        }

        /* ---------------判定系统时间是否在各事件各模块最后记录时间之前 --------
        * ---------------如果在之前修改最后记录时间为当前系统时间---------------*/
        if (g_need_change_time == 86400/* 1 day */ || g_need_change_time == 0) {
            change_merge_attack_time();
            g_need_change_time = 0;
        }
        ++g_need_change_time;

        min_merge_ok = 0;
        hour_merge_ok = 0;
        day_merge_ok = 0;
        week_merge_ok = 0;
        month_merge_ok = 0;
        year_merge_ok = 0;

        /* 合并分钟表 */
		//printf("min_merge_ok*******************************************\n");
ATT_MIN:		
        min_merge_ok = do_merge_tmp_to_min(&attack_conn);

        /* 合并小时表 流程 */
        if ((min_merge_ok < 0) && (min_merge_ok != -2)) {
#if 1
            merge_disconnect_db(&attack_conn, THREAD_ATTACK_MERGE);
            retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }
#endif		
			goto  ATT_MIN;	
        }
ATT_HOUR:
            hour_merge_ok = do_merge_min_to_hour(&attack_conn);

		/* 合并天表 流程 */
        if ((hour_merge_ok < 0) && (hour_merge_ok != -2)) {
            merge_disconnect_db(&attack_conn, THREAD_ATTACK_MERGE);
            retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }
			goto  ATT_HOUR;			
        }
ATT_DAY:
		    day_merge_ok = do_merge_hour_to_day(&attack_conn);
        
		/* 合并周表 流程 */
        if ((day_merge_ok < 0) && (day_merge_ok != -2)) {
            merge_disconnect_db(&attack_conn, THREAD_ATTACK_MERGE);
            retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }
			goto  ATT_DAY;			
        }
ATT_WEEK:
		    week_merge_ok = do_merge_day_to_week(&attack_conn);	

		/* 合并月表 流程 */
        if ((week_merge_ok < 0) && (week_merge_ok != -2)) {
            merge_disconnect_db(&attack_conn, THREAD_ATTACK_MERGE);
            retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }
			goto  ATT_WEEK;			
        }
ATT_MONTH:
		    month_merge_ok = do_merge_week_to_month(&attack_conn);

		/* 合并年表 流程 */
        if ((month_merge_ok < 0) && (month_merge_ok != -2)) {
            merge_disconnect_db(&attack_conn, THREAD_ATTACK_MERGE);
            retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }
			goto  ATT_MONTH;			
        }
            year_merge_ok = do_merge_month_to_year(&attack_conn); 

		sleep(1);
        ++count;                                                                                                  
        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            merge_disconnect_db(&attack_conn, THREAD_ATTACK_MERGE);
            retn = merge_connect_db(&attack_conn, THREAD_ATTACK_MERGE);         
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
	grd_disconnect(attack_conn.redis_conn);	
EXIT_1:
	/* 关闭postgreSQL链接 */
	gpq_disconnect(attack_conn.psql_conn);	

EXIT:
	pthread_exit((void*)tid);
}
