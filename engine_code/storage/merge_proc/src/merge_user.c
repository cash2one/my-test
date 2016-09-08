/**
 * @file merge_attack.c
 * @brief  特征事件合并流程 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-26
 */

#include "gms_merge.h"
#define OLD_FLOW_DELAY_TIME   (120)
static unsigned int g_need_change_time_user = 0;
pthread_mutex_t time_user_config_mutex = PTHREAD_MUTEX_INITIALIZER;



static cfg_desc g_merge_user[] = {

	/* When merge */
	{"CRITICAL_TIME", "merge_hour", &(g_user_info_cfg.merge_hour), CFG_SIZE_UINT8, CFG_TYPE_UINT8, 1, 0, "0"},
	{"CRITICAL_TIME", "merge_min", &(g_user_info_cfg.merge_min), CFG_SIZE_UINT8, CFG_TYPE_UINT8, 0, 0, "15"},

	/* user merge last time */
	{"USER_MERGE_TIME", "tmp_to_min", &(g_user_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"USER_MERGE_TIME", "min_to_hour", &(g_user_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"USER_MERGE_TIME", "hour_to_day", &(g_user_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"USER_MERGE_TIME", "day_to_week", &(g_user_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"USER_MERGE_TIME", "week_to_month", &(g_user_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"USER_MERGE_TIME", "month_to_year", &(g_user_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},


	/* addr merge last time */
	{"ADDR_MERGE_TIME", "tmp_to_min", &(g_addr_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"ADDR_MERGE_TIME", "min_to_hour", &(g_addr_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ADDR_MERGE_TIME", "hour_to_day", &(g_addr_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ADDR_MERGE_TIME", "day_to_week", &(g_addr_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ADDR_MERGE_TIME", "week_to_month", &(g_addr_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"ADDR_MERGE_TIME", "month_to_year", &(g_addr_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/* browser merge last time */
	{"BROWSER_MERGE_TIME", "tmp_to_min", &(g_browser_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"BROWSER_MERGE_TIME", "min_to_hour", &(g_browser_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BROWSER_MERGE_TIME", "hour_to_day", &(g_browser_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BROWSER_MERGE_TIME", "day_to_week", &(g_browser_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BROWSER_MERGE_TIME", "week_to_month", &(g_browser_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BROWSER_MERGE_TIME", "month_to_year", &(g_browser_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/* visit merge last time */
	{"VISIT_MERGE_TIME", "tmp_to_min", &(g_visit_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"VISIT_MERGE_TIME", "min_to_hour", &(g_visit_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISIT_MERGE_TIME", "hour_to_day", &(g_visit_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISIT_MERGE_TIME", "day_to_week", &(g_visit_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISIT_MERGE_TIME", "week_to_month", &(g_visit_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISIT_MERGE_TIME", "month_to_year", &(g_visit_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},


	/* visitnum merge last time */
	{"VISITNUM_MERGE_TIME", "tmp_to_min", &(g_visitnum_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"VISITNUM_MERGE_TIME", "min_to_hour", &(g_visitnum_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISITNUM_MERGE_TIME", "hour_to_day", &(g_visitnum_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISITNUM_MERGE_TIME", "day_to_week", &(g_visitnum_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISITNUM_MERGE_TIME", "week_to_month", &(g_visitnum_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"VISITNUM_MERGE_TIME", "month_to_year", &(g_visitnum_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/*  byte last time */
	{"BYTE_MERGE_TIME", "tmp_to_min", &(g_byte_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"BYTE_MERGE_TIME", "min_to_hour", &(g_byte_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BYTE_MERGE_TIME", "hour_to_day", &(g_byte_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BYTE_MERGE_TIME", "day_to_week", &(g_byte_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BYTE_MERGE_TIME", "week_to_month", &(g_byte_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BYTE_MERGE_TIME", "month_to_year", &(g_byte_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/* tcp merge last time */
	{"TCP_MERGE_TIME", "tmp_to_min", &(g_tcp_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"TCP_MERGE_TIME", "min_to_hour", &(g_tcp_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"TCP_MERGE_TIME", "hour_to_day", &(g_tcp_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"TCP_MERGE_TIME", "day_to_week", &(g_tcp_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"TCP_MERGE_TIME", "week_to_month", &(g_tcp_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"TCP_MERGE_TIME", "month_to_year", &(g_tcp_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/* bank merge last time */
	{"BANK_MERGE_TIME", "tmp_to_min", &(g_bank_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"BANK_MERGE_TIME", "min_to_hour", &(g_bank_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BANK_MERGE_TIME", "hour_to_day", &(g_bank_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BANK_MERGE_TIME", "day_to_week", &(g_bank_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BANK_MERGE_TIME", "week_to_month", &(g_bank_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"BANK_MERGE_TIME", "month_to_year", &(g_bank_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},

	/* dalay merge last time */
	{"DELAY_MERGE_TIME", "tmp_to_min", &(g_delay_info_cfg.att_tmp_to_min), TIME_FORMAT_SIZE, CFG_TYPE_STR, 1, 0, DEFAULT_MERGE_TIME},
	{"DELAY_MERGE_TIME", "min_to_hour", &(g_delay_info_cfg.att_min_to_hour), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"DELAY_MERGE_TIME", "hour_to_day", &(g_delay_info_cfg.att_hour_to_day), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"DELAY_MERGE_TIME", "day_to_week", &(g_delay_info_cfg.att_day_to_week), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"DELAY_MERGE_TIME", "week_to_month", &(g_delay_info_cfg.att_week_to_month), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
	{"DELAY_MERGE_TIME", "month_to_year", &(g_delay_info_cfg.att_month_to_year), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_MERGE_TIME},
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
static void make_merge_user_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,usernum) select '%s',t1.ftime_end,pc_if,t1.sip from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,count(distinct sip) as sip,pc_if from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0  group by pc_if) as t1 ", dst_table_name,start,src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,dev_id) select '%s',max(b.ftime_end),b.pc_if,b.sip from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0 ) as a left join %s b on a.sip=b.sip where ftime_end >= '%s' and ftime_end < '%s' and incr=0  group by b.pc_if,b.sip ", dst_table_name,start,src_table_name, start, end,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,usernum) values ('%s','%s',0,0)", dst_table_name,start,end);
}
static void make_merge_user_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,usernum) select t1.ftime_start,t1.ftime_end,pc_if,t1.usernum from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,sum(usernum) as usernum,pc_if from %s where ftime_start >= '%s' and ftime_start < '%s' group by pc_if) as t1", dst_table_name,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,dev_id) select min(ftime_start),max(ftime_end)as ftime_end,pc_if,distinct sip from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0  group by pc_if) as t1 ", dst_table_name,src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,dev_id) select min(b.ftime_start),max(b.ftime_end),b.pc_if,b.dev_id from (select distinct dev_id as sip from %s where ftime_start >= '%s' and ftime_start < '%s' ) as a left join %s b on a.sip=b.dev_id where ftime_start >= '%s' and ftime_start < '%s'  group by b.pc_if,b.dev_id ", dst_table_name,src_table_name, start, end,src_table_name, start, end);
}
static void make_merge_addr_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,country,province,county,usernum)  select '%s',t1.ftime_end,t1.country,t1.s_province_id,t1.s_city_id,t1.sip from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,country,s_province_id,s_city_id,count(distinct sip) as sip from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0 group by country,s_province_id,s_city_id) as t1", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,country,province,county,usernum)  values ('%s','%s',0,0,0,0)", dst_table_name,start, end);
}
static void make_merge_addr_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,country,province,county,usernum)  select t1.ftime_start,t1.ftime_end,t1.country,t1.province,t1.county,t1.sip from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,country,province,county,sum(usernum) as sip from %s where ftime_start >= '%s' and ftime_start < '%s' group by country,province,county) as t1", dst_table_name,src_table_name, start, end);
}
static void make_merge_browser_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,browser,version,usernum)  select '%s',t1.ftime_end,t1.browser,t1.version,t1.sip from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,browser,version,count(distinct sip) as sip from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0 and  procotol_id=7 group by browser,version) as t1", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,browser,version,usernum)  values ('%s','%s',0,'',0)", dst_table_name, start, end);

}
static void make_merge_browser_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,browser,version,usernum)  select t1.ftime_start,t1.ftime_end,t1.browser,t1.version,t1.sip from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,browser,version,sum(usernum) as sip from %s where ftime_start >= '%s' and ftime_start < '%s'  group by browser,version) as t1", dst_table_name,src_table_name, start, end);
}
static void make_merge_visit_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_time,pc_if) select '%s',max(b.ftime_end),b.s_city_id,b.browser,b.sip,sum(b.visit_time),b.pc_if from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7) a left join %s  b on b.sip=a.sip where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7 group by b.browser,b.s_city_id,b.sip,b.pc_if ", dst_table_name,start,src_table_name, start, end,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_time,pc_if) select '%s',t1.ftime_end,t1.s_city_id,t1.browser,t1.sip,t1.visit_time,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,s_city_id,browser,sip,sum(visit_time) as visit_time,pc_if from %s where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7 group by browser,s_city_id,sip,pc_if) as t1  order by visit_time desc limit 10;", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_time,pc_if) values ('%s','%s',0,0,0,0,0)", dst_table_name, start, end);

}
static void make_merge_visit_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_time,pc_if) select min(b.ftime_start),max(b.ftime_end),b.county,b.browser,b.sip,sum(b.visit_time),b.pc_if from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s') a left join %s  b on b.sip=a.sip where ftime_end >= '%s' and ftime_end < '%s' group by b.browser,b.county,b.sip,b.pc_if", dst_table_name,src_table_name, start, end,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_time,pc_if) select t1.ftime_start,t1.ftime_end,t1.county,t1.browser,t1.sip,t1.visit_time,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,county,browser,sip,sum(visit_time) as visit_time,pc_if from %s where ftime_start >= '%s' and ftime_start < '%s' group by browser,county,sip,pc_if) as t1  order by visit_time desc limit 10;", dst_table_name,src_table_name, start, end);
}
static void make_merge_visitnum_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) select '%s',max(b.ftime_end), b.s_city_id, b.browser,b.sip,count(b.sip),b.pc_if from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7) a left join %s  b on b.sip=a.sip where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7 group by b.browser,b.s_city_id,b.sip,b.pc_if ", dst_table_name,start,src_table_name, start, end,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) select '%s',t1.ftime_end,t1.s_city_id,t1.browser,t1.sip,t1.visit_num,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,s_city_id,browser,sip,count(distinct sip) as visit_num,pc_if from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0 and  procotol_id=7 group by browser,s_city_id,sip,pc_if) as t1 ", dst_table_name,start,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) select '%s',t1.ftime_end,t1.s_city_id,t1.browser,t1.sip,t1.visit_num,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,s_city_id,browser,sip,count(sip) as visit_num,pc_if from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0 and  procotol_id=7 group by browser,s_city_id,sip,pc_if) as t1  order by visit_num desc limit 10;", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) values ('%s','%s',0,0,0,0,0)", dst_table_name, start, end);
}
static void make_merge_visitnum_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) select t1.ftime_start,t1.ftime_end,t1.county,t1.browser,t1.sip,t1.visit_num,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,county,browser,distinct sip,sum(visit_num) as visit_num,pc_if from %s where ftime_start >= '%s' and ftime_start < '%s' group by browser,county,sip,pc_if) as t1", dst_table_name,src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) select min(b.ftime_start),max(b.ftime_end),b.county,b.browser,b.sip,sum(b.visit_num),b.pc_if from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s') a left join %s  b on b.sip=a.sip where ftime_end >= '%s' and ftime_end < '%s' group by b.browser,b.county,b.sip,b.pc_if", dst_table_name,src_table_name, start, end,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,visit_num,pc_if) select t1.ftime_start,t1.ftime_end,t1.county,t1.browser,t1.sip,t1.visit_num,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,county,browser,sip,sum(visit_num) as visit_num,pc_if from %s where ftime_start >= '%s' and ftime_start < '%s' group by browser,county,sip,pc_if) as t1  order by visit_num desc limit 10;", dst_table_name,src_table_name, start, end);
}
static void make_merge_byte_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,ip_byte,ip_pkt,pc_if) select '%s',t1.ftime_end,t1.s_city_id,t1.browser,t1.sip,t1.ip_byte,t1.ip_pkt,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,s_city_id,browser,sip,sum(ip_byte) as ip_byte,sum(ip_pkt) as ip_pkt ,pc_if from %s where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7  group by browser,s_city_id,sip,pc_if) as t1  order by ip_pkt desc limit 10;", dst_table_name,start,src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,ip_byte,ip_pkt,pc_if) select '%s',max(b.ftime_end),b.s_city_id,b.browser,b.sip,sum(b.ip_byte),sum(b.ip_pkt),b.pc_if from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s' and  procotol_id=7) a left join %s b on b.sip=a.sip where ftime_end >= '%s' and ftime_end < '%s'  group by b.browser,b.s_city_id,b.sip,b.pc_if", dst_table_name,start,src_table_name, start, end,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,ip_byte,ip_pkt,pc_if) values ('%s','%s',0,0,0,0,0,0)", dst_table_name, start, end);
}
static void make_merge_byte_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,ip_byte,ip_pkt,pc_if) select t1.ftime_start,t1.ftime_end,t1.county,t1.browser,t1.sip,t1.ip_byte,t1.ip_pkt,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,county,browser,sip,sum(ip_byte) as ip_byte,sum(ip_pkt) as ip_pkt ,pc_if from %s where ftime_start >= '%s' and ftime_start < '%s' group by browser,county,sip,pc_if) as t1  order by ip_pkt desc limit 10;", dst_table_name,src_table_name, start, end);
	//snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,ip_byte,ip_pkt,pc_if) select t1.ftime_start,t1.ftime_end,t1.county,t1.browser,t1.sip,t1.ip_byte,t1.ip_pkt,pc_if from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,county,browser,distinct sip,sum(ip_byte) as ip_byte,sum(ip_pkt) as ip_pkt ,pc_if from %s where ftime_start >= '%s' and ftime_start < '%s' group by browser,county,sip,pc_if) as t1 ;", dst_table_name,src_table_name, start, end);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,county,browser,sip,ip_byte,ip_pkt,pc_if) select min(b.ftime_start),max(b.ftime_end),b.county,b.browser,b.sip,sum(b.ip_byte),sum(b.ip_pkt),b.pc_if from (select distinct sip as sip from %s where ftime_end >= '%s' and ftime_end < '%s') a left join %s b on b.sip=a.sip where ftime_end >= '%s' and ftime_end < '%s'  group by b.browser,b.county,b.sip,b.pc_if", dst_table_name,src_table_name, start, end,src_table_name, start, end);
}
static void make_merge_tcp_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end ,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,tcp_suc,tcp_fail) select '%s',t1.ftime_end,t1.pc_if,t1.tcp_suc,t1.tcp_fail from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,pc_if,sum(tcp_suc) as tcp_suc,count(tcp_suc)-sum(tcp_suc) as tcp_fail from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0  group by pc_if) as t1;", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,tcp_suc,tcp_fail) values ('%s','%s',0,0,0)", dst_table_name,start, end);
}
static void make_merge_tcp_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,tcp_suc,tcp_fail) select t1.ftime_start,t1.ftime_end,t1.pc_if,t1.tcp_suc,t1.tcp_fail from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,pc_if,sum(tcp_suc) as tcp_suc,sum(tcp_fail) as tcp_fail from %s where ftime_start >= '%s' and ftime_start < '%s' group by pc_if) as t1;", dst_table_name,src_table_name, start, end);
}
static void make_merge_bank_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,bank_visit,bank_fail) select '%s',t1.ftime_end,t1.pc_if,t1.bank_suc,t1.bank_fail from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,pc_if,sum(bank_visit) as bank_suc,count(bank_visit)-sum(bank_visit) as bank_fail from %s where ftime_end >= '%s' and ftime_end < '%s' and incr=0 group by pc_if) as t1;", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,bank_visit,bank_fail) values ('%s','%s',0,0,0)", dst_table_name,start, end);
}
static void make_merge_bank_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,bank_visit,bank_fail) select t1.ftime_start,t1.ftime_end,t1.pc_if,t1.bank_suc,t1.bank_fail from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,pc_if,sum(bank_visit) as bank_suc,sum(bank_fail) as bank_fail from %s where ftime_start >= '%s' and ftime_start < '%s' group by pc_if) as t1;", dst_table_name,src_table_name, start, end);
}
static void make_merge_delay_min_sql(char *sql_cmd, char *src_table_name, char *dst_table_name, char *start, char *end,int flag) 
{	
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	if(flag==1)
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,cli_ser) select '%s',t1.ftime_end,t1.pc_if,t1.cli_ser from (select min(ftime_start) as ftime_start,max(ftime_end)as ftime_end,pc_if,avg(cli_ser) as cli_ser from %s where ftime_end >= '%s' and ftime_end < '%s'  group by pc_if) as t1;", dst_table_name,start,src_table_name, start, end);
	else
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (ftime_start,ftime_end,pc_if,cli_ser) values ('%s','%s',0,0)", dst_table_name,start, end);
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
    
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "INSERT INTO %s (devid,ename,nameid, isccserver, etype,begin_time,end_time,handledeadline, eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport,eventdetail, enum,risk_level,query_id, month_id, day_id) SELECT devid,ename,nameid, isccserver, etype, '%s', '%s', '%s', eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport, eventdetail,SUM(enum),risk_level,query_id,month_id,day_id FROM %s WHERE begin_time >= '%s' AND end_time < '%s' GROUP BY devid,ename, nameid, isccserver, etype, eventtype, eventbasetype, eventexttype, daddr,saddr,sport,dport,eventdetail, risk_level,query_id, month_id, day_id", table_name,start, end,start, src_table_name, start, end);

	printf("+++++++++++++sql_cmd:%s\n", sql_cmd);
}
#endif
static void make_delete_min_sql(char *sql_cmd, char *table_name, char *start, char *end)
{
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "DELETE FROM %s WHERE ftime_end >= '%s' AND ftime_end <= '%s'", table_name, start, end);
	printf("+++++++++++sql_cmd:%s\n", sql_cmd);
}
static void make_delete_day_sql(char *sql_cmd, char *table_name, char *start, char *end)
{
	memset(sql_cmd, 0, MERGE_SQL_CMD_SIZE);
	snprintf(sql_cmd, MERGE_SQL_CMD_SIZE, "DELETE FROM %s ", table_name);
	printf("+++++++++++sql_cmd:%s\n", sql_cmd);
}
#if 0
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
	now_critical_time = make_merge_critical_time_year(now_time_str, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	now_critical_time = now_critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);

	critical_time = make_merge_critical_time_year(g_user_info_cfg.att_month_to_year, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	
	time_p = localtime(&critical_time);
	leap = is_leap_year(time_p->tm_year+1900);
	if (leap == 1) {
		one_year_sec = ONE_YEAR_TIME_SEC_366;
	}
	else {
		one_year_sec = ONE_YEAR_TIME_SEC_365;
	}

	if ((now_time - critical_time) < one_year_sec) {
        att_ok = 1;
		goto _3RD_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_year_sec, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "ATTACK [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);

	make_merge_user_min_sql(sql_cmd, TABLE_NAME_ATTACK_MONTH, TABLE_NAME_ATTACK_YEAR, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "ATTACK [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto _3RD_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_year_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "ATTACK_MERGE_TIME", "month_to_year", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

_3RD_MERGE:
	critical_time = make_merge_critical_time_year(g_user_info_cfg._3rd_month_to_year, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

	time_p = localtime(&critical_time);
	leap = is_leap_year(time_p->tm_year+1900);
	if (leap == 1) {
		one_year_sec = ONE_YEAR_TIME_SEC_366;
	}
	else {
		one_year_sec = ONE_YEAR_TIME_SEC_365;
	}

	if ((now_time - critical_time) < one_year_sec) {
        if (att_ok == 1) {
            _3rd_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_year_sec, time_format);

	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "3RD [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge month -> year]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_user_min_sql(sql_cmd, TABLE_NAME_3RD_MONTH, TABLE_NAME_3RD_YEAR, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "3RD [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "3RD [Merge month -> year] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_year_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "3RD_MERGE_TIME", "month_to_year", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

END:
	return retn;
}
#endif
/* -------------------------------------------*/
/**
 * @brief     周表合并月表
 * @returns  0 正常合并，但是所有时间未合并结束 
 *           1 所有时间合并结束
 *           <0 错误
 */
/* -------------------------------------------*/
static int do_merge_week_to_month_user(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
	char sql_cmd[MERGE_SQL_CMD_SIZE];
	struct tm *time_p = NULL;
    int user_ok=0, addr_ok = 0,browser_ok=0;
    time_t one_month_sec = 0;

	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_month(now_time_str, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	now_critical_time = now_critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);

	critical_time = make_merge_critical_time_month(g_user_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        user_ok = 1;
		goto ADDR_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "USER[Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);

	make_merge_user_sql(sql_cmd, USER_WEEK_TABLE, USER_MONTH_TABLE, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "ATTACK[Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"ATTACK[Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto ADDR_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

ADDR_MERGE:
	critical_time = make_merge_critical_time_month(g_addr_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1) {
            addr_ok = 1;
            retn = 1;
        }
		goto BROWSER_MERGE;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_addr_sql(sql_cmd, ADDR_WEEK_TABLE, ADDR_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE, "ADDR [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BROWSER_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BROWSER_MERGE:
	critical_time = make_merge_critical_time_month(g_browser_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_MERGE;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_browser_sql(sql_cmd, BROWSER_WEEK_TABLE, BROWSER_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BROWSER [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BROWSER_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

VISIT_MERGE:
	critical_time = make_merge_critical_time_month(g_visit_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_NUM;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_visit_sql(sql_cmd, VISIT_WEEK_TABLE, VISIT_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISIT [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_NUM;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISIT_NUM:
	critical_time = make_merge_critical_time_month(g_visitnum_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BYTE_MERGE;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_visitnum_sql(sql_cmd, VISITNUM_WEEK_TABLE, VISITNUM_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISITNUM [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BYTE_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BYTE_MERGE:
	critical_time = make_merge_critical_time_month(g_byte_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto TCP_MERGE;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_byte_sql(sql_cmd, BYTE_WEEK_TABLE, BYTE_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BYTE [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto TCP_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
TCP_MERGE:
	critical_time = make_merge_critical_time_month(g_tcp_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BANK_MERGE;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_tcp_sql(sql_cmd, TCP_WEEK_TABLE, TCP_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"TCP [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BANK_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BANK_MERGE:
	critical_time = make_merge_critical_time_month(g_bank_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto DELAY_MERGE;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_bank_sql(sql_cmd, BANK_WEEK_TABLE, BANK_MONTH_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BANK [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto DELAY_MERGE;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
DELAY_MERGE:
	critical_time = make_merge_critical_time_month(g_delay_info_cfg.att_week_to_month, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

    one_month_sec = get_month_sec(&critical_time);
	if ((now_time - critical_time) < one_month_sec) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, one_month_sec, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge week -> month]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_delay_min_sql(sql_cmd, DELAY_WEEK_TABLE, DELAY_MONTH_TABLE, critical_time_str, end_critical_time_str,1);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE, "DELAY [Merge week -> month] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 week_to_month */
    end_time = (now_time - now_critical_time) + critical_time + one_month_sec;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "week_to_month", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
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
static int do_merge_day_to_week_user(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	char sql_cmd[MERGE_SQL_CMD_SIZE];
	struct tm *time_p = NULL;
    int user_ok =0, addr_ok = 0,browser_ok=0;


	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_week(now_time_str, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);

	critical_time = make_merge_critical_time_week(g_user_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        user_ok = 1;
		goto ADDR_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "USER [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);

	make_merge_user_sql(sql_cmd, USER_DAY_TABLE, USER_WEEK_TABLE, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "USER [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"USER [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto ADDR_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

ADDR_MERGE:
	critical_time = make_merge_critical_time_week(g_addr_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1) {
            addr_ok = 1;
            retn = 1;
        }
		goto BROWSER_MERGE;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_addr_sql(sql_cmd, ADDR_DAY_TABLE, ADDR_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"ADDR [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BROWSER_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BROWSER_MERGE:
	critical_time = make_merge_critical_time_week(g_browser_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_MERGE;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_browser_sql(sql_cmd, BROWSER_DAY_TABLE, BROWSER_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BROWSER [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BROWSER_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISIT_MERGE:
	critical_time = make_merge_critical_time_week(g_visit_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_NUM;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_visit_sql(sql_cmd, VISIT_DAY_TABLE, VISIT_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISIT [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_NUM;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

VISIT_NUM:
	critical_time = make_merge_critical_time_week(g_visitnum_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BYTE_MERGE;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_visitnum_sql(sql_cmd, VISITNUM_DAY_TABLE, VISITNUM_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISITNUM [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BYTE_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

BYTE_MERGE:
	critical_time = make_merge_critical_time_week(g_byte_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto TCP_MERGE;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_byte_sql(sql_cmd, BYTE_DAY_TABLE, BYTE_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BYTE [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto TCP_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

TCP_MERGE:
	critical_time = make_merge_critical_time_week(g_tcp_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BANK_MERGE;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_tcp_sql(sql_cmd, TCP_DAY_TABLE, TCP_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"TCP [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BANK_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

BANK_MERGE:
	critical_time = make_merge_critical_time_week(g_bank_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto DELAY_MERGE;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_bank_sql(sql_cmd, BANK_DAY_TABLE, BANK_WEEK_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BANK [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto DELAY_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
DELAY_MERGE:
	critical_time = make_merge_critical_time_week(g_delay_info_cfg.att_day_to_week, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_WEEK_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto END;
	}

    /* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_WEEK_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge day -> week]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_delay_min_sql(sql_cmd, DELAY_DAY_TABLE, DELAY_WEEK_TABLE, critical_time_str, end_critical_time_str,1);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"DELAY [Merge day -> week] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto DELAY_MERGE;
	}

	/* 更新 day_to_week */
    end_time = (now_time - now_critical_time) + critical_time + ONE_WEEK_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "day_to_week", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);



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
static int do_merge_hour_to_day_user(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];
    int user_ok =0, addr_ok = 0 , browser_ok=0;

	printf("_________hour_________to___________day_______\n");
	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_day(now_time_str, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	now_critical_time = now_critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);

	critical_time = make_merge_critical_time_day(g_user_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if ((now_time - critical_time) < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        user_ok = 1;
		goto ADDR_MERGE;
	}
	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "USER [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_user_sql(sql_cmd, USER_HOUR_TABLE, USER_DAY_TABLE, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "USER[Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"USER[Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto ADDR_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

ADDR_MERGE:
	critical_time = make_merge_critical_time_day(g_addr_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1) {
            addr_ok = 1;
            retn = 1;
        }
		goto BROWSER_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_addr_sql(sql_cmd, ADDR_HOUR_TABLE, ADDR_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"ADDR [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BROWSER_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BROWSER_MERGE:
	critical_time = make_merge_critical_time_day(g_browser_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_browser_sql(sql_cmd, BROWSER_HOUR_TABLE, BROWSER_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BROWSER [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BROWSER_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISIT_MERGE:
	critical_time = make_merge_critical_time_day(g_visit_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISITNUM_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_visit_sql(sql_cmd, VISIT_HOUR_TABLE, VISIT_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISIT [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISITNUM_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISITNUM_MERGE:
	critical_time = make_merge_critical_time_day(g_visitnum_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BYTE_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_visitnum_sql(sql_cmd, VISITNUM_HOUR_TABLE, VISITNUM_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISITNUM [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BYTE_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BYTE_MERGE:
	critical_time = make_merge_critical_time_day(g_byte_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto TCP_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_byte_sql(sql_cmd, BYTE_HOUR_TABLE, BYTE_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BYTE [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto TCP_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
TCP_MERGE:
	critical_time = make_merge_critical_time_day(g_tcp_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BANK_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_tcp_sql(sql_cmd, TCP_HOUR_TABLE, TCP_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"TCP [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BANK_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BANK_MERGE:
	critical_time = make_merge_critical_time_day(g_bank_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto DELAY_MERGE;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_bank_sql(sql_cmd, BANK_HOUR_TABLE, BANK_DAY_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BANK [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto DELAY_MERGE;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
DELAY_MERGE:
	critical_time = make_merge_critical_time_day(g_delay_info_cfg.att_hour_to_day, g_user_info_cfg.merge_hour, g_user_info_cfg.merge_min);
	if (now_time - critical_time < ONE_DAY_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok ==1) {
            browser_ok = 1;
            retn = 1;
        }
		goto END;
	}

	/* need 3rd merge */

	/* 得到需要合并数据的时间 */
	critical_time = critical_time - (g_user_info_cfg.merge_hour*3600 + g_user_info_cfg.merge_min*60);
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_DAY_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge hour -> day]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	make_merge_delay_min_sql(sql_cmd, DELAY_HOUR_TABLE, DELAY_DAY_TABLE, critical_time_str, end_critical_time_str,1);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"DELAY [Merge hour -> day] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 hour_to_day */
    end_time = (now_time - now_critical_time) + critical_time + ONE_DAY_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "hour_to_day", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
#if 1
    /* 生成删除sql语句 */
    make_delete_day_sql(sql_cmd, USER_TMP_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "T_FLOW_EVENT [Delete %s] [%s]->[%s] ERROR!\n", USER_TMP_TABLE, critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"T_FLOW_EVENT [Delete %s] [%s]->[%s] ERROR!\n", USER_TMP_TABLE, critical_time_str, end_critical_time_str);
		goto END;
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "T_FLOW_EVENT [Delete %s] [%s]->[%s] SUCC!\n", USER_TMP_TABLE, critical_time_str, end_critical_time_str);

#endif
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
static int do_merge_min_to_hour_user(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];
    int user_ok=0 , addr_ok = 0,browser_ok=0;


	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_hour(now_time_str);

	critical_time = make_merge_critical_time_hour(g_user_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < (ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME)) { // + 300 意味着 延迟5分钟合并
        user_ok = 1;
		goto ADDR_MERGE;
	}

	/* need att merge */

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "====> USER [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_user_sql(sql_cmd, USER_MIN_TABLE, USER_HOUR_TABLE, critical_time_str, end_critical_time_str);

	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "USER[Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"USER[Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto ADDR_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

ADDR_MERGE:
	critical_time = make_merge_critical_time_hour(g_addr_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1) {
            addr_ok = 1;
            retn = 1;
        }
		goto BROWSER_MERGE;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_addr_sql(sql_cmd, ADDR_MIN_TABLE, ADDR_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"ADDR [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BROWSER_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BROWSER_MERGE:
	critical_time = make_merge_critical_time_hour(g_browser_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_MERGE;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_browser_sql(sql_cmd, BROWSER_MIN_TABLE, BROWSER_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BROWSER [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BROWSER_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISIT_MERGE:
	critical_time = make_merge_critical_time_hour(g_visit_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto VISIT_NUM;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_visit_sql(sql_cmd, VISIT_MIN_TABLE, VISIT_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISIT [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_NUM;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISIT_NUM:
	critical_time = make_merge_critical_time_hour(g_visitnum_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BYTE_MERGE;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_visitnum_sql(sql_cmd, VISITNUM_MIN_TABLE, VISITNUM_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"VISITNUM [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BYTE_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BYTE_MERGE:
	critical_time = make_merge_critical_time_hour(g_byte_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto TCP_MERGE;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_byte_sql(sql_cmd, BYTE_MIN_TABLE, BYTE_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BYTE [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto TCP_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
TCP_MERGE:
	critical_time = make_merge_critical_time_hour(g_tcp_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto BANK_MERGE;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_tcp_sql(sql_cmd, TCP_MIN_TABLE, TCP_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"TCP [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BANK_MERGE;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BANK_MERGE:
	critical_time = make_merge_critical_time_hour(g_bank_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto DELAY_MERGE;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_bank_sql(sql_cmd, BANK_MIN_TABLE, BANK_HOUR_TABLE, critical_time_str, end_critical_time_str);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"BANK [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto DELAY_MERGE;
	}
DELAY_MERGE:
	critical_time = make_merge_critical_time_hour(g_delay_info_cfg.att_min_to_hour);
	if ((now_time - critical_time) < ONE_HOUR_TIME_SEC + OLD_FLOW_DELAY_TIME) {
        if (user_ok == 1 && addr_ok == 1) {
            browser_ok = 1;
            retn = 1;
        }
		goto END;
	}
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_HOUR_TIME_SEC, time_format);

	CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge tmp -> hour]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 */
	make_merge_delay_min_sql(sql_cmd, DELAY_MIN_TABLE, DELAY_HOUR_TABLE, critical_time_str, end_critical_time_str,1);
	printf("---------------------hour_sql_cmd:%s\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"DELAY [Merge tmp -> hour] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);


	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_HOUR_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "min_to_hour", NULL, end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

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
static int do_merge_tmp_to_min_user(merge_conn_t *conn)
{
	int retn = 0;
	char time_format[] = "%Y-%m-%d %X";
	time_t critical_time = 0;
	time_t now_time = 0;
	time_t now_critical_time = 0;
    time_t end_time = 0;
	char now_time_str[TIME_FORMAT_SIZE];
	char critical_time_str[TIME_FORMAT_SIZE];
    char end_critical_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
	//char last_time_str[TIME_FORMAT_SIZE];
	struct tm *time_p = NULL;
	char sql_cmd[MERGE_SQL_CMD_SIZE];
	//char time_cmd[MERGE_SQL_CMD_SIZE];
	//char count_cmd[MERGE_SQL_CMD_SIZE];
	//char min_time[TIME_FORMAT_SIZE];
	//char min_zero_time[TIME_FORMAT_SIZE];
	//int sub_time_cp = 0;
	//char min_next_zero_time[TIME_FORMAT_SIZE];
	//char now_zero_time[TIME_FORMAT_SIZE];
	int user_ok=0,addr_ok=0,browser_ok=0,flag=0;
	char sql_null[MERGE_SQL_CMD_SIZE]={0};
	//int count = 0;
	//int i = 0;
	//int sub_time = 0;

	time(&now_time);
	get_systime_str(now_time_str);
	now_critical_time = make_merge_critical_time_min(now_time_str);

	critical_time = make_merge_critical_time_min(g_user_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
			user_ok=1;
            retn = 1;
		goto ADDR_MERGE;
	}
	/* need user merge */
	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "USER [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_start>='%s' and ftime_start<'%s' limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	printf("choose sql=%s\n",sql_null);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_user_min_sql(sql_cmd, USER_TMP_TABLE, USER_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
    printf("--->do {%s}\n", sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "USER [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user() USER [Merge tmp -> min] [%s]->[%s] ERROR!\n",critical_time_str, end_critical_time_str);
		goto ADDR_MERGE;
	}


	CA_LOG(LOG_MODULE, LOG_PROC, "====> USER [Merge tmp -> query]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

ADDR_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_addr_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1)
			addr_ok=1;
            retn = 1;
		goto BROWSER_MERGE;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' and incr=0 limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_addr_min_sql(sql_cmd, ADDR_TMP_TABLE, ADDR_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	printf("flag=%d sql_addr=%s\n",flag,sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "ADDR [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user() ADDR [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BROWSER_MERGE;
	}
#if 0
    /* 生成删除sql语句 */
    make_delete_sql(sql_cmd, USER_TMP_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		MERGE_ERROR(DEBUG_TYPE_ATTACK, "FLOW_EVENT [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
		CA_LOG(LOG_MODULE, LOG_PROC, "T_FLOW_EVENT [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
		goto END;
	}
	MERGE_DEBUG(DEBUG_TYPE_ATTACK, "T_FLOW_EVENT [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
	CA_LOG(LOG_MODULE, LOG_PROC, "T_FLOW_EVENT [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
#endif

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BROWSER_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_browser_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto VISIT_MERGE;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' and  procotol_id=7 limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_browser_min_sql(sql_cmd, BROWSER_TMP_TABLE, BROWSER_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),BROWSER [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_MERGE;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BROWSER_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
VISIT_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_visit_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto VISIT_NUM;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "VISIT [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' and  procotol_id=7 limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_visit_min_sql(sql_cmd, VISIT_TMP_TABLE, VISIT_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BROWSER [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),BROWSER [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto VISIT_NUM;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);

VISIT_NUM:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_visitnum_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto BYTE_MERGE;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' and  procotol_id=7  limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_visitnum_min_sql(sql_cmd, VISITNUM_TMP_TABLE, VISITNUM_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "VISITNUM [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),VISITNUM [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BYTE_MERGE;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BYTE_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_byte_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto TCP_MERGE;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' and  procotol_id=7 limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_byte_min_sql(sql_cmd, BYTE_TMP_TABLE, BYTE_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BYTE [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),BYTE [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto TCP_MERGE;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
TCP_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_tcp_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto BANK_MERGE;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_tcp_min_sql(sql_cmd, TCP_TMP_TABLE, TCP_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "TCP [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),TCP [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto BANK_MERGE;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
BANK_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_bank_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto DELAY_MERGE;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s'  limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_bank_min_sql(sql_cmd, BANK_TMP_TABLE, BANK_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	printf("flag=%d ************************* bank_sql=%S\n",flag,sql_cmd);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "BANK [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),BANK [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str); 
		goto DELAY_MERGE;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
DELAY_MERGE:
	/*用户归属地tmp->min*/
	critical_time = make_merge_critical_time_min(g_delay_info_cfg.att_tmp_to_min);
	if ((now_time - critical_time) < ONE_MIN_TIME_SEC + OLD_FLOW_DELAY_TIME) {
		if(user_ok == 1&& addr_ok == 1 )
			browser_ok=1;
            retn = 1;
		goto END;
	}

	/* 得到需要合并数据的时间 */
	time_p = localtime(&critical_time);
	strftime(critical_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
    get_timestr_offset(end_critical_time_str, critical_time_str, ONE_MIN_TIME_SEC, time_format);
	CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge tmp -> min]\n====> [begin time]%s, [end time]%s <====\n", critical_time_str, end_critical_time_str);
    /* 生成合并sql语句 user and min*/
	memset(sql_null,0,MERGE_SQL_CMD_SIZE);
	snprintf(sql_null,MERGE_SQL_CMD_SIZE,"select id from %s where ftime_end>='%s' and ftime_end<'%s' limit 1 offset 0",USER_TMP_TABLE,critical_time_str,end_critical_time_str);
	flag=gpq_get_row(conn->psql_conn,sql_null);
	make_merge_delay_min_sql(sql_cmd, DELAY_TMP_TABLE, DELAY_MIN_TABLE, critical_time_str, end_critical_time_str,flag);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "DELAY [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),DELAY [Merge tmp -> min] [%s]->[%s] ERROR!\n", critical_time_str, end_critical_time_str);
		goto END;
	}

	/* 更新 min_to_hour */
    end_time = (now_time - now_critical_time) + critical_time + ONE_MIN_TIME_SEC;
	time_p = localtime(&end_time);
	strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
	pthread_mutex_lock(&time_user_config_mutex);
	write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "tmp_to_min", NULL, end_time_str);
    CA_LOG(LOG_MODULE, LOG_PROC, "------------->Update tmp->min: %s <------------------\n", end_time_str);
	pthread_mutex_unlock(&time_user_config_mutex);
#if 1
    /* 生成删除sql语句 */
    make_delete_min_sql(sql_cmd, USER_TMP_TABLE, critical_time_str, end_critical_time_str);
	retn = gpq_sql_cmd(conn->psql_conn, sql_cmd);	
	if (retn < 0) {
		CA_LOG(LOG_MODULE, LOG_PROC, "T_FLOW_EVENT [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
		syslog(LOG_NOTICE,"in do_merge_tmp_to_min_user(),T_FLOW_EVENT [Delete %s] [%s]->[%s] ERROR!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);		
		goto END;
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "T_FLOW_EVENT [Delete %s] [%s]->[%s] SUCC!\n", TABLE_NAME_3RD_TMP, critical_time_str, end_critical_time_str);
#endif
END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief 根据系统时间来逐步调整最后合并时间
 */
/* -------------------------------------------*/
void change_merge_user_time(void)
{
	char time_format[] = "%Y-%m-%d %X";
    int time_x = 0;
    char now_sys_time_str[TIME_FORMAT_SIZE];

    get_systime_str_format(now_sys_time_str, time_format);

#if 1
    /* ----- user 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_user_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]tmp_to_min) [%s] -> [%s]", g_user_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_user_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_user_info_cfg.att_tmp_to_min, g_user_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "min_to_hour", NULL, g_user_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]min_to_hour) [%s] -> [%s]", g_user_info_cfg.att_min_to_hour, g_user_info_cfg.att_tmp_to_min);
        strncpy(g_user_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_user_info_cfg.att_min_to_hour, g_user_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "hour_to_day", NULL, g_user_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]hour_to_day) [%s] -> [%s]", g_user_info_cfg.att_hour_to_day, g_user_info_cfg.att_min_to_hour);
        strncpy(g_user_info_cfg.att_hour_to_day, g_user_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_user_info_cfg.att_hour_to_day, g_user_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "day_to_week", NULL, g_user_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]day_to_week) [%s] -> [%s]", g_user_info_cfg.att_day_to_week, g_user_info_cfg.att_hour_to_day);
        strncpy(g_user_info_cfg.att_day_to_week, g_user_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_user_info_cfg.att_day_to_week, g_user_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "week_to_month", NULL, g_user_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]week_to_month) [%s] -> [%s]", g_user_info_cfg.att_week_to_month, g_user_info_cfg.att_day_to_week);
        strncpy(g_user_info_cfg.att_week_to_month, g_user_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_user_info_cfg.att_week_to_month, g_user_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "month_to_year", NULL, g_user_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]month_to_year) [%s] -> [%s]", g_user_info_cfg.att_month_to_year, g_user_info_cfg.att_week_to_month);
        strncpy(g_user_info_cfg.att_month_to_year, g_user_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

#endif
    /* ----- addr 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_addr_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]tmp_to_min) [%s] -> [%s]", g_addr_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_addr_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_addr_info_cfg.att_tmp_to_min, g_addr_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "min_to_hour", NULL, g_addr_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]min_to_hour) [%s] -> [%s]", g_addr_info_cfg.att_min_to_hour, g_addr_info_cfg.att_tmp_to_min);
        strncpy(g_addr_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_addr_info_cfg.att_min_to_hour, g_addr_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "ADDR_MERGE_TIME", "hour_to_day", NULL, g_addr_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]hour_to_day) [%s] -> [%s]", g_addr_info_cfg.att_hour_to_day, g_addr_info_cfg.att_min_to_hour);
        strncpy(g_addr_info_cfg.att_hour_to_day, g_addr_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_addr_info_cfg.att_hour_to_day, g_addr_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "day_to_week", NULL, g_addr_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]day_to_week) [%s] -> [%s]", g_addr_info_cfg.att_day_to_week, g_addr_info_cfg.att_hour_to_day);
        strncpy(g_addr_info_cfg.att_day_to_week, g_addr_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_addr_info_cfg.att_day_to_week, g_addr_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "week_to_month", NULL, g_addr_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]week_to_month) [%s] -> [%s]", g_addr_info_cfg.att_week_to_month, g_addr_info_cfg.att_day_to_week);
        strncpy(g_addr_info_cfg.att_week_to_month, g_addr_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_addr_info_cfg.att_week_to_month, g_addr_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "month_to_year", NULL, g_addr_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]month_to_year) [%s] -> [%s]", g_addr_info_cfg.att_month_to_year, g_addr_info_cfg.att_week_to_month);
        strncpy(g_addr_info_cfg.att_month_to_year, g_addr_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- browser 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_browser_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BROWSER_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]tmp_to_min) [%s] -> [%s]", g_browser_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_browser_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_browser_info_cfg.att_tmp_to_min, g_browser_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "min_to_hour", NULL, g_browser_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]min_to_hour) [%s] -> [%s]", g_browser_info_cfg.att_min_to_hour, g_browser_info_cfg.att_tmp_to_min);
        strncpy(g_browser_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_browser_info_cfg.att_min_to_hour, g_browser_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "hour_to_day", NULL, g_browser_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]hour_to_day) [%s] -> [%s]", g_browser_info_cfg.att_hour_to_day, g_browser_info_cfg.att_min_to_hour);
        strncpy(g_browser_info_cfg.att_hour_to_day, g_browser_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_browser_info_cfg.att_hour_to_day, g_browser_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "day_to_week", NULL, g_browser_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]day_to_week) [%s] -> [%s]", g_browser_info_cfg.att_day_to_week, g_browser_info_cfg.att_hour_to_day);
        strncpy(g_browser_info_cfg.att_day_to_week, g_browser_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_browser_info_cfg.att_day_to_week, g_browser_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "week_to_month", NULL, g_browser_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]week_to_month) [%s] -> [%s]", g_browser_info_cfg.att_week_to_month, g_browser_info_cfg.att_day_to_week);
        strncpy(g_browser_info_cfg.att_week_to_month, g_browser_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_browser_info_cfg.att_week_to_month, g_browser_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "USER_MERGE_TIME", "month_to_year", NULL, g_browser_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([ATT]month_to_year) [%s] -> [%s]", g_browser_info_cfg.att_month_to_year, g_browser_info_cfg.att_week_to_month);
        strncpy(g_browser_info_cfg.att_month_to_year, g_browser_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- visit 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_visit_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([visit]tmp_to_min) [%s] -> [%s]", g_visit_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_visit_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_visit_info_cfg.att_tmp_to_min, g_visit_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "min_to_hour", NULL, g_visit_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([visit]min_to_hour) [%s] -> [%s]", g_visit_info_cfg.att_min_to_hour, g_visit_info_cfg.att_tmp_to_min);
        strncpy(g_visit_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_user_info_cfg.att_min_to_hour, g_user_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "hour_to_day", NULL, g_visit_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([visit]hour_to_day) [%s] -> [%s]", g_visit_info_cfg.att_hour_to_day, g_visit_info_cfg.att_min_to_hour);
        strncpy(g_visit_info_cfg.att_hour_to_day, g_visit_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_visit_info_cfg.att_hour_to_day, g_visit_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "day_to_week", NULL, g_visit_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([visit]day_to_week) [%s] -> [%s]", g_visit_info_cfg.att_day_to_week, g_visit_info_cfg.att_hour_to_day);
        strncpy(g_visit_info_cfg.att_day_to_week, g_visit_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_visit_info_cfg.att_day_to_week, g_visit_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "week_to_month", NULL, g_visit_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([visit]week_to_month) [%s] -> [%s]", g_visit_info_cfg.att_week_to_month, g_visit_info_cfg.att_day_to_week);
        strncpy(g_visit_info_cfg.att_week_to_month, g_visit_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_visit_info_cfg.att_week_to_month, g_visit_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISIT_MERGE_TIME", "month_to_year", NULL, g_visit_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([visit]month_to_year) [%s] -> [%s]", g_visit_info_cfg.att_month_to_year, g_visit_info_cfg.att_week_to_month);
        strncpy(g_visit_info_cfg.att_month_to_year, g_visit_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- visitnum 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_visitnum_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([vinum]tmp_to_min) [%s] -> [%s]", g_visitnum_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_visitnum_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_visitnum_info_cfg.att_tmp_to_min, g_visitnum_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "min_to_hour", NULL, g_visitnum_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([vinum]min_to_hour) [%s] -> [%s]", g_visitnum_info_cfg.att_min_to_hour, g_visitnum_info_cfg.att_tmp_to_min);
        strncpy(g_visitnum_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_visitnum_info_cfg.att_min_to_hour, g_visitnum_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "hour_to_day", NULL, g_visitnum_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([vinum]hour_to_day) [%s] -> [%s]", g_visitnum_info_cfg.att_hour_to_day, g_visitnum_info_cfg.att_min_to_hour);
        strncpy(g_visitnum_info_cfg.att_hour_to_day, g_visitnum_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_visitnum_info_cfg.att_hour_to_day, g_visitnum_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "day_to_week", NULL, g_visitnum_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([vinum]day_to_week) [%s] -> [%s]", g_visitnum_info_cfg.att_day_to_week, g_visitnum_info_cfg.att_hour_to_day);
        strncpy(g_visitnum_info_cfg.att_day_to_week, g_visitnum_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_visitnum_info_cfg.att_day_to_week, g_visitnum_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "week_to_month", NULL, g_visitnum_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([vinum]week_to_month) [%s] -> [%s]", g_visitnum_info_cfg.att_week_to_month, g_visitnum_info_cfg.att_day_to_week);
        strncpy(g_visitnum_info_cfg.att_week_to_month, g_visitnum_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_visitnum_info_cfg.att_week_to_month, g_visitnum_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "VISITNUM_MERGE_TIME", "month_to_year", NULL, g_visitnum_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([vinum]month_to_year) [%s] -> [%s]", g_visitnum_info_cfg.att_month_to_year, g_visitnum_info_cfg.att_week_to_month);
        strncpy(g_visitnum_info_cfg.att_month_to_year, g_visitnum_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- byte 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_byte_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([byte]tmp_to_min) [%s] -> [%s]", g_byte_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_byte_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_byte_info_cfg.att_tmp_to_min, g_byte_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "min_to_hour", NULL, g_byte_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([byte]min_to_hour) [%s] -> [%s]", g_byte_info_cfg.att_min_to_hour, g_byte_info_cfg.att_tmp_to_min);
        strncpy(g_byte_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_byte_info_cfg.att_min_to_hour, g_byte_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "hour_to_day", NULL, g_byte_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([byte]hour_to_day) [%s] -> [%s]", g_byte_info_cfg.att_hour_to_day, g_byte_info_cfg.att_min_to_hour);
        strncpy(g_byte_info_cfg.att_hour_to_day, g_byte_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_byte_info_cfg.att_hour_to_day, g_byte_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "day_to_week", NULL, g_byte_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([byte]day_to_week) [%s] -> [%s]", g_byte_info_cfg.att_day_to_week, g_byte_info_cfg.att_hour_to_day);
        strncpy(g_byte_info_cfg.att_day_to_week, g_byte_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_byte_info_cfg.att_day_to_week, g_byte_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "week_to_month", NULL, g_byte_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([byte]week_to_month) [%s] -> [%s]", g_byte_info_cfg.att_week_to_month, g_byte_info_cfg.att_day_to_week);
        strncpy(g_byte_info_cfg.att_week_to_month, g_byte_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_byte_info_cfg.att_week_to_month, g_byte_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BYTE_MERGE_TIME", "month_to_year", NULL, g_byte_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([byte]month_to_year) [%s] -> [%s]", g_byte_info_cfg.att_month_to_year, g_byte_info_cfg.att_week_to_month);
        strncpy(g_byte_info_cfg.att_month_to_year, g_byte_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- tcp 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_tcp_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([tcp]tmp_to_min) [%s] -> [%s]", g_tcp_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_tcp_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_tcp_info_cfg.att_tmp_to_min, g_tcp_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "min_to_hour", NULL, g_tcp_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([tcp]min_to_hour) [%s] -> [%s]", g_tcp_info_cfg.att_min_to_hour, g_tcp_info_cfg.att_tmp_to_min);
        strncpy(g_tcp_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_tcp_info_cfg.att_min_to_hour, g_tcp_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "hour_to_day", NULL, g_tcp_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([tcp]hour_to_day) [%s] -> [%s]", g_tcp_info_cfg.att_hour_to_day, g_tcp_info_cfg.att_min_to_hour);
        strncpy(g_tcp_info_cfg.att_hour_to_day, g_tcp_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_tcp_info_cfg.att_hour_to_day, g_tcp_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "day_to_week", NULL, g_tcp_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([tcp]day_to_week) [%s] -> [%s]", g_tcp_info_cfg.att_day_to_week, g_tcp_info_cfg.att_hour_to_day);
        strncpy(g_tcp_info_cfg.att_day_to_week, g_tcp_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_tcp_info_cfg.att_day_to_week, g_tcp_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "week_to_month", NULL, g_tcp_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([tcp]week_to_month) [%s] -> [%s]", g_tcp_info_cfg.att_week_to_month, g_tcp_info_cfg.att_day_to_week);
        strncpy(g_tcp_info_cfg.att_week_to_month, g_tcp_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_tcp_info_cfg.att_week_to_month, g_tcp_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "TCP_MERGE_TIME", "month_to_year", NULL, g_tcp_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([tcp]month_to_year) [%s] -> [%s]", g_tcp_info_cfg.att_month_to_year, g_tcp_info_cfg.att_week_to_month);
        strncpy(g_tcp_info_cfg.att_month_to_year, g_tcp_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- bank 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_bank_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([bank]tmp_to_min) [%s] -> [%s]", g_bank_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_bank_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_bank_info_cfg.att_tmp_to_min, g_bank_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "min_to_hour", NULL, g_bank_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([bank]min_to_hour) [%s] -> [%s]", g_bank_info_cfg.att_min_to_hour, g_bank_info_cfg.att_tmp_to_min);
        strncpy(g_bank_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_bank_info_cfg.att_min_to_hour, g_bank_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "hour_to_day", NULL, g_bank_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([bank]hour_to_day) [%s] -> [%s]", g_bank_info_cfg.att_hour_to_day, g_bank_info_cfg.att_min_to_hour);
        strncpy(g_bank_info_cfg.att_hour_to_day, g_bank_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_bank_info_cfg.att_hour_to_day, g_bank_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "day_to_week", NULL, g_bank_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([bank]day_to_week) [%s] -> [%s]", g_bank_info_cfg.att_day_to_week, g_bank_info_cfg.att_hour_to_day);
        strncpy(g_bank_info_cfg.att_day_to_week, g_bank_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_bank_info_cfg.att_day_to_week, g_bank_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "week_to_month", NULL, g_bank_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([bank]week_to_month) [%s] -> [%s]", g_bank_info_cfg.att_week_to_month, g_bank_info_cfg.att_day_to_week);
        strncpy(g_bank_info_cfg.att_week_to_month, g_bank_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_bank_info_cfg.att_week_to_month, g_bank_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "BANK_MERGE_TIME", "month_to_year", NULL, g_bank_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([bank]month_to_year) [%s] -> [%s]", g_bank_info_cfg.att_month_to_year, g_bank_info_cfg.att_week_to_month);
        strncpy(g_bank_info_cfg.att_month_to_year, g_bank_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }

    /* ----- delay 比较---- */
    /* 比较系统时间和tmp->min时间 */
    time_x = time_cmp_format(now_sys_time_str, g_delay_info_cfg.att_tmp_to_min, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "tmp_to_min", NULL, now_sys_time_str);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([delay]tmp_to_min) [%s] -> [%s]", g_delay_info_cfg.att_tmp_to_min, now_sys_time_str);
        strncpy(g_delay_info_cfg.att_tmp_to_min, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    
    /* 比较tmp->min 和 min->hour 时间*/
    time_x = time_cmp_format(g_delay_info_cfg.att_tmp_to_min, g_delay_info_cfg.att_min_to_hour, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "min_to_hour", NULL, g_delay_info_cfg.att_tmp_to_min);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([delay]min_to_hour) [%s] -> [%s]", g_delay_info_cfg.att_min_to_hour, g_delay_info_cfg.att_tmp_to_min);
        strncpy(g_delay_info_cfg.att_min_to_hour, now_sys_time_str, TIME_FORMAT_SIZE); 
    }
    /* 比较min->hour时间 和 hour->day 时间 */
    time_x = time_cmp_format(g_delay_info_cfg.att_min_to_hour, g_delay_info_cfg.att_hour_to_day, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "hour_to_day", NULL, g_delay_info_cfg.att_min_to_hour);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([delay]hour_to_day) [%s] -> [%s]", g_delay_info_cfg.att_hour_to_day, g_delay_info_cfg.att_min_to_hour);
        strncpy(g_delay_info_cfg.att_hour_to_day, g_delay_info_cfg.att_min_to_hour, TIME_FORMAT_SIZE); 
    }
    /* 比较tmp->day时间 和 day->week 时间 */
    time_x = time_cmp_format(g_delay_info_cfg.att_hour_to_day, g_delay_info_cfg.att_day_to_week, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "day_to_week", NULL, g_delay_info_cfg.att_hour_to_day);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([delay]day_to_week) [%s] -> [%s]", g_delay_info_cfg.att_day_to_week, g_delay_info_cfg.att_hour_to_day);
        strncpy(g_delay_info_cfg.att_day_to_week, g_delay_info_cfg.att_hour_to_day, TIME_FORMAT_SIZE); 
    }
    /* 比较day->week时间 和 week->month 时间 */
    time_x = time_cmp_format(g_delay_info_cfg.att_day_to_week, g_delay_info_cfg.att_week_to_month, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "week_to_month", NULL, g_delay_info_cfg.att_day_to_week);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([delay]week_to_month) [%s] -> [%s]", g_delay_info_cfg.att_week_to_month, g_delay_info_cfg.att_day_to_week);
        strncpy(g_delay_info_cfg.att_week_to_month, g_delay_info_cfg.att_day_to_week, TIME_FORMAT_SIZE); 
    }
    /* 比较week->month时间 和 month->year 时间 */
    time_x = time_cmp_format(g_delay_info_cfg.att_week_to_month, g_delay_info_cfg.att_month_to_year, time_format);
    if (time_x < 0) {
		write_conf(MERGE_TIME_USER_PATH, "DELAY_MERGE_TIME", "month_to_year", NULL, g_delay_info_cfg.att_week_to_month);
        CA_LOG(LOG_MODULE, LOG_PROC, "Change Time:([delay]month_to_year) [%s] -> [%s]", g_delay_info_cfg.att_month_to_year, g_delay_info_cfg.att_week_to_month);
        strncpy(g_delay_info_cfg.att_month_to_year, g_delay_info_cfg.att_week_to_month, TIME_FORMAT_SIZE); 
    }


}


void *merge_user_busi(void *arg)
{
	pthread_t tid;
	tid = pthread_self();

	merge_conn_t user_conn;
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

    con_retn = merge_connect_db(&user_conn, THREAD_USER_NUM);             
    if (con_retn == -1) {                                                                                             
        goto EXIT;                                                                                                
    }                                                                                                             
    else if (con_retn == 1) {                                                                                         
        goto EXIT_1;                                                                                              
    }             


	while (1) {
		/* 读取合并进度时间 */
		pthread_mutex_lock(&time_user_config_mutex);
		retn = common_read_conf(MERGE_TIME_USER_PATH, g_merge_user);
		if (retn < 0) {
			CA_LOG(LOG_MODULE, LOG_PROC, "Read config %s ERROR", MERGE_TIME_USER_PATH);
			pthread_mutex_unlock(&time_user_config_mutex);
			goto EXIT_2;
		}
		pthread_mutex_unlock(&time_user_config_mutex);


        if (strcmp(g_user_info_cfg.att_tmp_to_min, DEFAULT_MERGE_TIME) == 0) {
           get_systime_str_format(g_user_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_user_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_user_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_user_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_user_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_user_info_cfg.att_month_to_year, time_format);

           get_systime_str_format(g_addr_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_addr_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_addr_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_addr_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_addr_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_addr_info_cfg.att_month_to_year, time_format);
           
           get_systime_str_format(g_browser_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_browser_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_browser_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_browser_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_browser_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_browser_info_cfg.att_month_to_year, time_format);
           
           get_systime_str_format(g_visit_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_visit_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_visit_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_visit_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_visit_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_visit_info_cfg.att_month_to_year, time_format);
           
           
           get_systime_str_format(g_visitnum_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_visitnum_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_visitnum_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_visitnum_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_visitnum_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_visitnum_info_cfg.att_month_to_year, time_format);
           
           
           get_systime_str_format(g_byte_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_byte_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_byte_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_byte_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_byte_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_byte_info_cfg.att_month_to_year, time_format);
           
           
           get_systime_str_format(g_tcp_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_tcp_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_tcp_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_tcp_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_tcp_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_tcp_info_cfg.att_month_to_year, time_format);
           
           
           get_systime_str_format(g_bank_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_bank_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_bank_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_bank_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_bank_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_bank_info_cfg.att_month_to_year, time_format);
           
           
           get_systime_str_format(g_delay_info_cfg.att_tmp_to_min, time_format);
           get_systime_str_format(g_delay_info_cfg.att_min_to_hour, time_format);
           get_systime_str_format(g_delay_info_cfg.att_hour_to_day, time_format);
           get_systime_str_format(g_delay_info_cfg.att_day_to_week, time_format);
           get_systime_str_format(g_delay_info_cfg.att_week_to_month, time_format);
           get_systime_str_format(g_delay_info_cfg.att_month_to_year, time_format);
           
           CA_LOG(LOG_MODULE, LOG_PROC, "WARNING!!!!%s is NULL, Reset all merge_time to systime time!\n", MERGE_TIME_USER_PATH);
           retn = create_cfg_file(MERGE_TIME_USER_PATH, g_merge_user);
           if (retn < 0) {
               CA_LOG(LOG_MODULE, LOG_PROC, "Open file %s ERROR!\n", MERGE_TIME_USER_PATH);
               goto EXIT_2;
           }
        }

        /* ---------------判定系统时间是否在各事件各模块最后记录时间之前 --------
        * ---------------如果在之前修改最后记录时间为当前系统时间---------------*/
        if (g_need_change_time_user == 86400/* 1 day */ || g_need_change_time_user == 0) {
            change_merge_user_time();
            g_need_change_time_user = 0;
        }
        ++g_need_change_time_user;

        min_merge_ok = 0;
        hour_merge_ok = 0;
        day_merge_ok = 0;
        week_merge_ok = 0;
        month_merge_ok = 0;
        year_merge_ok = 0;
USER_MIN:
        /* 合并分钟表 */
        min_merge_ok = do_merge_tmp_to_min_user(&user_conn);

        /* 合并小时表 流程 */
        if (min_merge_ok < 0) {
            merge_disconnect_db(&user_conn, THREAD_USER_NUM);
            retn = merge_connect_db(&user_conn, THREAD_USER_NUM);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
			goto USER_MIN;

		}

USER_HOUR:
            hour_merge_ok = do_merge_min_to_hour_user(&user_conn);

		/* 合并天表 流程 */
        if (hour_merge_ok < 0) {
            merge_disconnect_db(&user_conn, THREAD_USER_NUM);
            retn = merge_connect_db(&user_conn, THREAD_USER_NUM);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
			goto USER_HOUR;
		}
USER_DAY:
		    day_merge_ok = do_merge_hour_to_day_user(&user_conn);
        
		/* 合并周表 流程 */
        if (day_merge_ok < 0) {
            merge_disconnect_db(&user_conn, THREAD_USER_NUM);
            retn = merge_connect_db(&user_conn, THREAD_USER_NUM);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
			goto USER_DAY;
        }
USER_WEEK:
		    week_merge_ok = do_merge_day_to_week_user(&user_conn);	

		/* 合并月表 流程 */
        if (week_merge_ok < 0) {
            merge_disconnect_db(&user_conn, THREAD_USER_NUM);
            retn = merge_connect_db(&user_conn, THREAD_USER_NUM);         
            if (retn == -1) {                                                                                     
                goto EXIT;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
			goto USER_WEEK;
        }
USER_MONTH:
		    month_merge_ok = do_merge_week_to_month_user(&user_conn);

			if(month_merge_ok < 0 ){
				merge_disconnect_db(&user_conn, THREAD_USER_NUM);
				retn = merge_connect_db(&user_conn, THREAD_USER_NUM);         
				if (retn == -1) {                                                                                     
					goto EXIT;                                                                                        
				}                                                                                                     
				else if (retn == 1) {                                                                                 
					goto EXIT_1;                                                                                      
				}                                                                                                     
				goto USER_MONTH;

			}
		/* 合并年表 流程 */
        //if (month_merge_ok == 1) {
        //    year_merge_ok = do_merge_month_to_year(&user_conn); 
       // }

		sleep(1);
        ++count;                                                                                                  
        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            merge_disconnect_db(&user_conn, THREAD_USER_NUM);
            retn = merge_connect_db(&user_conn, THREAD_USER_NUM);         
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
	grd_disconnect(user_conn.redis_conn);	
EXIT_1:
	/* 关闭postgreSQL链接 */
	gpq_disconnect(user_conn.psql_conn);	

EXIT:
	pthread_exit((void*)tid);
}
