/**
 * @file store_common.h
 * @brief  存储基础公用模块
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-21
 */
#ifndef _STORE_COMMON_H_
#define _STORE_COMMON_H_

#include <stdint.h>
#include <assert.h>
#include <sys/time.h>

#define IN
#define OUT

/* 常用常量 */
#define IP_STR_SIZE                 16    /* IP地址字符串长度 */
#define PORT_STR_SIZE               10    /* PORT字符串长度 */
#define DBNAME_SIZE                 50    /* 数据库名长度 */
#define USER_NAME_SIZE              50    /* 用户名长度 */
#define PASSWORD_SIZE               100    /* 密码长度 */
#define TIME_FORMAT_SIZE            25
#define ABSOLUT_FILE_PATH_SIZE      200
#define DIR_FILE_SIZE               10
#define FILE_NAME_SIZE              100


/* -------------------------------------------*/
/**
 * @brief  测试的结构
 */
/* -------------------------------------------*/
typedef struct _test_stats
{
     uint64_t usec_start;
     uint64_t usec_end;
     uint64_t usec;
     double sec;        /* 运行测试阶段耗费的时间*/
} test_stat_t;

/* -------------------------------------------*/
/**
 * @brief  初始化测试变量
 */
/* -------------------------------------------*/
#define TEST_VARS \
    struct timeval _test_tv = {0, 0}; 

/* -------------------------------------------*/
/**
 * @brief  开始执行测试
 *
 * @param  test_stat_t 类型 测试变量
 *
 */
/* -------------------------------------------*/
#define TEST_START(stat) \
        gettimeofday(&_test_tv, NULL); \
        stat.usec_start = _test_tv.tv_sec * 1000000 + _test_tv.tv_usec;

/* -------------------------------------------*/
/**
 * @brief 结束测试
 *
 * @param  test_stat_t 类型 测试变量
 *
 */
/* -------------------------------------------*/
#define TEST_END(stat) \
        gettimeofday(&_test_tv, NULL); \
        stat.usec_end = _test_tv.tv_sec * 1000000 + _test_tv.tv_usec; \
        stat.usec += stat.usec_end - stat.usec_start; \
        stat.sec = (double) stat.usec / 1000000;       \
        stat.usec_start = stat.usec_end = 0;

/* -------------------------------------------*/
/**
 * @brief  读取指定文件到指定buf中，要求buf足够大
 *
 * @param src_path    文件路径
 * @param dst_data    得到的文件buf
 *
 * @returns   
 *            >=0 得到的buf长度
 *            -1 失败
 */
/* -------------------------------------------*/
int read_file_to_data(IN char *src_path, OUT void *dst_data);
int read_file_to_data_m(IN char *src_path, OUT void **dst_data);

/* -------------------------------------------*/
/**
 * @brief  将指定buf中数据内容写到文件中去
 *
 * @param src_data    数据源buf
 * @param data_len    数据长度
 * @param dst_path    目标文件 如果没有则创建
 *
 * @returns   
 *            >=0 写进文件的长度
 *            -1  失败
 */
/* -------------------------------------------*/
int write_data_to_file(IN void *src_data, IN unsigned int data_len, char *dst_path);

/* -------------------------------------------*/
/**
 * @brief  将时间格式“2013-11-03 06:24:59”类似
 *            时间字符串 拆分出每个单位的字符串
 *            如果不想得到的时间单位 可以传NULL
 *
 * @param time_str    原完整时间字符串
 * @param year        得到的年
 * @param month        得到的月
 * @param day        得到的天
 * @param hour        得到的小时
 * @param minute    得到的分钟
 * @param second    得到的秒
 */
/* -------------------------------------------*/
void split_time_str(char *time_str, 
                    char *year, 
                    char *month, 
                    char *day, 
                    char *hour, 
                    char *minute, 
                    char *second);

/* -------------------------------------------*/
/**
 * @brief  以‘/’为分层次级别， 得到对应级别的目录名字
 *           例如 “/gms/events/ae/2013/11/12”
 *           0级别为12，1级别为11， 2级别为2013
 *
 * @param src_name    原绝对路径
 * @param name        得到的对应级别的名字
 * @param level        级别
 *
 * @returns   
 *                    得到的对应几倍的名字
 */
/* -------------------------------------------*/
char *name_by_level(char *src_name, char *name, char level);

/* -------------------------------------------*/
/**
 * @brief  获取当前系统时间 格式为1999-01-22 11:02:03
 *
 * @param now_time_str
 *
 * @returns   
 *        获得的时间字符串 首地址
 */
/* -------------------------------------------*/
char* get_systime_str(char *now_time_str);
char* get_systime_str_format(char *now_time_str, const char *format);
char* get_systime_str_format_offset(char *out_time_str, const char *format, int offset_sec);

/* -------------------------------------------*/
/**
 * @brief  根据当前系统时间 返回第二天凌晨的
 *            日历时间
 *
 * @returns   
 *            第二天凌晨日历时间
 */
/* -------------------------------------------*/
time_t get_next_zero_time(void);

/* -------------------------------------------*/
/**
 * @brief  比较两个时间相差秒数 格式为1999-01-22 11:02:03
 *
 * @param time_str1    
 * @param time_str2
 *
 * @returns   
 *            相差的秒数，>0 time_str1 晚于 time_str2 多少秒
*                        <0 time_str1 早于 time_str2 多少秒
 */
/* -------------------------------------------*/
int time_cmp(char *time_str1, char *time_str2);
int time_cmp_format(char *time_str1, char *time_str2, const char *format);

/* -------------------------------------------*/
/**
 * @brief  获取当前字符串时间 的 偏移字符串时间
 *
 * @param out_time_str    
 * @param in_time_str
 * @param offset_sec    正数为将来 负数为过去
 * @param format
 *
 * @returns   
 *        获得的时间字符串 首地址
 */
/* -------------------------------------------*/
char *get_timestr_offset(char *out_time_str, char *in_time_str, int offset_sec, const char *format);
char *get_critical_timestr_offset(char *out_time_str, char *in_time_str, int offset_sec, const char *format);

/* -------------------------------------------*/
/**
 * @brief  通过制定格式的时间字符串返回time_t时间
 *
 * @param time_str
 * @param format
 *
 * @returns   
 */
/* -------------------------------------------*/
time_t get_time_sec_by_str(char *time_str, const char *format);


/* -------------------------------------------*/
/**
 * @brief  比较两个整型IP的大小
 *           定义 192.168.1.2 > 192.168.1.1
 *           权重自左向右递减
 *
 * @returns   
 *            0   相等
 *            1  ip1 > ip2 
 *            -1 ip1 < ip2 
 */
/* -------------------------------------------*/
int ip_cmp(unsigned int ip1, unsigned int ip2);

/* -------------------------------------------*/
/**
 * @brief  创建一个多层次目录
 *
 * @param path 目录
 *
 * @returns   
 *          -1 错误
 *          0  成功 
 */
/* -------------------------------------------*/
int create_dir_ex(char *path);
#endif
