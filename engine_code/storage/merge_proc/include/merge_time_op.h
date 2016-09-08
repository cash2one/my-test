/**
 * @file merge_time_op.h
 * @brief  合并时间操作函数声明
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-27
 */
#ifndef _MERGE_TIME_OP_H_
#define _MERGE_TIME_OP_H_
#include <time.h>

#define ONE_MIN_TIME_SEC                (60)
#define ONE_HOUR_TIME_SEC				(3600)
#define ONE_DAY_TIME_SEC				(86400)//3600*24
#define ONE_WEEK_TIME_SEC				(604800) //3600*24*7
#define ONE_MONTH_TIME_SEC_28			(2419200) //3600*24*28
#define ONE_MONTH_TIME_SEC_29			(2505600) //3600*24*29
#define ONE_MONTH_TIME_SEC_30			(2592000) //3600*24*30
#define ONE_MONTH_TIME_SEC_31			(2678400) //3600*24*31
#define ONE_YEAR_TIME_SEC_365			(31536000) //3600*24*365
#define ONE_YEAR_TIME_SEC_366			(31622400) //3600*24*366

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
time_t make_merge_critical_time_day(char *time_str, unsigned int hour, unsigned int min);

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
time_t make_merge_critical_time_week(char *time_str, unsigned int hour, unsigned int min);

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
time_t make_merge_critical_time_month(char *time_str, unsigned int hour, unsigned int min);

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
time_t make_merge_critical_time_year(char *time_str, unsigned int hour, unsigned int min);

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
int is_leap_year(unsigned int year);

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
time_t get_month_sec(time_t *time);

#endif

