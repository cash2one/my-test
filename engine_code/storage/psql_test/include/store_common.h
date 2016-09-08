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
#define	IP_STR_SIZE					16	/* IP地址字符串长度 */
#define PORT_STR_SIZE				10	/* PORT字符串长度 */
#define DBNAME_SIZE					50	/* 数据库名长度 */
#define	USER_NAME_SIZE				50	/* 用户名长度 */
#define PASSWORD_SIZE				100	/* 密码长度 */


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
     double sec;		/* 运行测试阶段耗费的时间*/
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
 * @param src_path	文件路径
 * @param dst_data	得到的文件buf
 *
 * @returns   
 *			>=0 得到的buf长度
 *			-1 失败
 */
/* -------------------------------------------*/
int read_file_to_data(IN char *src_path, OUT void *dst_data);

/* -------------------------------------------*/
/**
 * @brief  将指定buf中数据内容写到文件中去
 *
 * @param src_data	数据源buf
 * @param data_len	数据长度
 * @param dst_path	目标文件 如果没有则创建
 *
 * @returns   
 *			>=0 写进文件的长度
 *			-1  失败
 */
/* -------------------------------------------*/
int write_data_to_file(IN void *src_data, IN unsigned int data_len, char *dst_path);

#endif
