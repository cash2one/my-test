/**
 * @file merge_time_op.c
 * @brief  有关计算合并时间的函数实现 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-27
 */
#include "gms_merge.h"

/* -------------------------------------------*/
/**
 * @brief  取出合并分钟表的临界真值时间
 *
 * @param time_str
 * @param hour		
 * @param min
 *
 * @returns   
 *			日历时间
 */
/* -------------------------------------------*/
time_t make_merge_critical_time_min(char *time_str)
{
	struct tm this_time;
	char time_format[] = "%Y-%m-%d %X";

	memset(&this_time, 0, sizeof(struct tm));
	strptime(time_str, time_format, &this_time);

	this_time.tm_sec = 0;

	return mktime(&this_time);
}

/* -------------------------------------------*/
/**
 * @brief  取出合并小时表的临界真值时间
 *
 * @param time_str
 * @param hour		
 * @param min
 *
 * @returns   
 *			日历时间
 */
/* -------------------------------------------*/
time_t make_merge_critical_time_hour(char *time_str)
{
	struct tm this_time;
	char time_format[] = "%Y-%m-%d %X";

	memset(&this_time, 0, sizeof(struct tm));
	strptime(time_str, time_format, &this_time);

	this_time.tm_sec = 0;
	this_time.tm_min = 0;

	return mktime(&this_time);
}

/* -------------------------------------------*/
/**
 * @brief  取出合并天表的临界真值时间
 *
 * @param time_str
 * @param hour		
 * @param min
 *
 * @returns   
 *			日历时间
 */
/* -------------------------------------------*/
time_t make_merge_critical_time_day(char *time_str, unsigned int hour, unsigned int min)
{
	struct tm this_time;
	char time_format[] = "%Y-%m-%d %X";	
	char too_late = 0;
	unsigned int late_sec, critical_sec;

	memset(&this_time, 0, sizeof(struct tm));
	strptime(time_str, time_format, &this_time);

	late_sec = 3600 * this_time.tm_hour + 60 * this_time.tm_min;
	critical_sec = 3600 * hour + 60 * min;

	if (late_sec < critical_sec) {
		too_late = 1;
	}

	this_time.tm_sec = 0;
	this_time.tm_min = min;
	this_time.tm_hour = hour;

	if (too_late == 1) {
		return (mktime(&this_time) - ONE_DAY_TIME_SEC);
	}

	return mktime(&this_time);		
}

/* -------------------------------------------*/
/**
 * @brief  取出合并周表的临界真值时间
 *
 * @param time_str
 * @param hour		
 * @param min
 *
 * @returns   
 *			日历时间
 */
/* -------------------------------------------*/
time_t make_merge_critical_time_week(char *time_str, unsigned int hour, unsigned int min)
{
	struct tm this_time;
	char time_format[] = "%Y-%m-%d %X";
	int days = 0;
	char too_late = 0;
	unsigned int late_sec, critical_sec;

	memset(&this_time, 0, sizeof(struct tm));
	strptime(time_str, time_format, &this_time);

	late_sec = 3600 * this_time.tm_hour + 60 * this_time.tm_min;
	critical_sec = 3600 * hour + 60 * min;
	if (late_sec < critical_sec) {
		too_late = 1;
	}

	this_time.tm_sec = 0;
	this_time.tm_min = min;
	this_time.tm_hour = hour;


	days = this_time.tm_wday - 1;
	if (days == -1) {
		days = 6;
	}
	if (too_late == 1) {
		return (mktime(&this_time) - (ONE_DAY_TIME_SEC*days) - ONE_WEEK_TIME_SEC);
	}

	return (mktime(&this_time) - (ONE_DAY_TIME_SEC*days));
}

/* -------------------------------------------*/
/**
 * @brief  取出合并月表的临界真值时间
 *
 * @param time_str
 * @param hour		
 * @param min
 *
 * @returns   
 *			日历时间
 */
/* -------------------------------------------*/
time_t make_merge_critical_time_month(char *time_str, unsigned int hour, unsigned int min)
{
	struct tm this_time;
	char time_format[] = "%Y-%m-%d %X";

	memset(&this_time, 0, sizeof(struct tm));
	strptime(time_str, time_format, &this_time);

	this_time.tm_sec = 0;
	this_time.tm_min = min;
	this_time.tm_hour = hour;
	this_time.tm_mday = 1;

	return (mktime(&this_time));
}

/* -------------------------------------------*/
/**
 * @brief  取出合并年表表的临界真值时间
 *
 * @param time_str
 * @param hour		
 * @param min
 *
 * @returns   
 *			日历时间
 */
/* -------------------------------------------*/
time_t make_merge_critical_time_year(char *time_str, unsigned int hour, unsigned int min)
{
	struct tm this_time;			
	char time_format[] = "%Y-%m-%d %X";

	memset(&this_time, 0, sizeof(struct tm));
	strptime(time_str, time_format, &this_time);

	this_time.tm_sec = 0;
	this_time.tm_min = min;
	this_time.tm_hour = hour;

	this_time.tm_mon = 0;
	this_time.tm_mday = 1;

	return (mktime(&this_time));		
}


/* -------------------------------------------*/
/**
 * @brief  判断概念是否为润年
 *
 * @param year
 *
 * @returns   
 *			1 润年
 *			0 平年
 */
/* -------------------------------------------*/
int is_leap_year(unsigned int year)
{
	if ((year%4==0 && year%100!=0) || (year%400==0)) {
		return 1;
	}
	return 0;
}


/* -------------------------------------------*/
/**
 * @brief  得到一个月的秒数
 *
 * @param time
 *
 * @returns   
 *				秒数
 */
/* -------------------------------------------*/
time_t get_month_sec(time_t *time)
{
	struct tm *time_p = NULL;
	int leap = 0;
	time_t sec = 0;
	struct tm  tmp_time;

	time_p = localtime_r(time,&tmp_time);	

	leap = is_leap_year((time_p->tm_year)+1900);	

	//printf("month: %d\n", (time_p->tm_mon) + 1);

	switch ((time_p->tm_mon)+1) {
		case 1:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		case 2:
			if (leap) {
				sec = ONE_MONTH_TIME_SEC_29;
			}
			else {
				sec = ONE_MONTH_TIME_SEC_28;
			}
			break;
		case 3:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		case 4:
			sec = ONE_MONTH_TIME_SEC_30;
			break;
		case 5:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		case 6:
			sec = ONE_MONTH_TIME_SEC_30;
			break;
		case 7:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		case 8:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		case 9:
			sec = ONE_MONTH_TIME_SEC_30;
			break;
		case 10:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		case 11:
			sec = ONE_MONTH_TIME_SEC_30;
			break;
		case 12:
			sec = ONE_MONTH_TIME_SEC_31;
			break;
		default:
			break;
	}

	return sec;
}
