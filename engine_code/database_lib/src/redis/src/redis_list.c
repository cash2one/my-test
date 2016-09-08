/**
 * @file redis_list.c
 * @brief  redis 链表相关操作接口实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-19
 */

#include "gms_redis.h"
#include <string.h>
#include <stdlib.h>

/* -------------------------------------------*/
/**
 * @brief  批量执行链表插入命令 插入链表头部
 *
 * @param conn		已建立好的链接
 * @param key		链表名
 * @param values	封装好的值数组
 * @param val_num	值个数
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int grd_list_push_append(redisContext *conn, char *key, RVALUES values, int val_num)
{
	int retn = 0;
	int i = 0;
	redisReply *reply = NULL;


	/* 批量插入命令到缓冲命令管道 */
	for (i = 0; i < val_num; ++i) {
		retn = redisAppendCommand(conn, "lpush %s %s", key, values[i]);
		if (retn != REDIS_OK) {
			fprintf(stderr, "[-][GMS_REDIS]PLUSH %s %s ERROR!\n", key, values[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]PLUSH %s %s ERROR! %s\n", key, values[i], conn->errstr);
			retn = -1;
			goto END;
		}
		retn = 0;
	}

	/* 提交命令 */
	for (i = 0; i < val_num; ++i) {
		retn = redisGetReply(conn, (void**)&reply);
		if (retn != REDIS_OK) {
			retn = -1;
			fprintf(stderr, "[-][GMS_REDIS]Commit LPUSH %s %s ERROR!\n", key, values[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Commit LPUSH %s %s ERROR! %s\n", key, values[i], conn->errstr);
			freeReplyObject(reply);
			break;
		}
		freeReplyObject(reply);
		retn = 0;
	}
	
END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  单条数据插入链表
 *
 * @param conn		已建立好的链接
 * @param key		链表名
 * @param value		数据
 *
 * @returns   
 */
/* -------------------------------------------*/
int grd_list_push(redisContext *conn, char *key, char *value)
{
	int retn = 0;
	redisReply *reply = NULL;

	reply = redisCommand(conn, "LPUSH %s %s", key, value);
	//grd_test_reply_type(reply);	
	if (reply->type != REDIS_REPLY_INTEGER) {
		fprintf(stderr, "[-][GMS_REDIS]LPUSH %s %s error!\n", key, value);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]LPUSH %s %s error!%s\n", key, value, conn->errstr);
		retn = -1;
	}

	freeReplyObject(reply);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  得到链表中元素的个数
 *
 * @param conn	链接句柄
 * @param key	链表名
 *
 * @returns   
 *			>=0 个数
 *			-1 fail
 */
/* -------------------------------------------*/
int grd_get_list_cnt(redisContext *conn, char *key)
{
	int cnt = 0;

	redisReply *reply = NULL;

	reply = redisCommand(conn, "LLEN %s", key);
	if (reply->type != REDIS_REPLY_INTEGER) {
		fprintf(stderr, "[-][GMS_REDIS]LLEN %s error\n", key);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]LLEN %s error %s\n", key, conn->errstr);
		cnt = -1;
		goto END;
	}

	cnt = reply->integer;

END:
	freeReplyObject(reply);
	return cnt;
}


/* -------------------------------------------*/
/**
 * @brief  按照一定范围截断链表中的数据
 *
 * @param conn		已经建立的链接
 * @param key		链表名
 * @param begin		阶段启示位置 从 0 开始
 * @param end		阶段结束位置 从 -1 开始
 *
 *					这里的范围定义举例 
 *					如果得到全部范围(0, -1)
 *					除了最后一个元素范围(0, -2)
 *					前20各数据范围(0, 19)
 *
 * @returns   
 *			0  SUCC
 *			-1 FAIL
 */
/* -------------------------------------------*/
int grd_trim_list(redisContext *conn, char *key, int begin, int end)
{
	int retn = 0;
	redisReply *reply = NULL;

	reply = redisCommand(conn, "LTRIM %s %d %d", key, begin, end);
	if (reply->type != REDIS_REPLY_STATUS) {
		fprintf(stderr, "[-][GMS_REDIS]LTRIM %s %d %d error!\n", key, begin, end);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]LTRIM %s %d %d error!%s\n", key, begin, end, conn->errstr);
		retn = -1;
	}

	freeReplyObject(reply);
	return retn;
}


/* -------------------------------------------*/
/**
 * @brief          得到链表中的数据
 *
 * @param conn		已经建立的链接
 * @param key		链表名
 *
 * @returns   
 *			0  SUCC
 *			-1 FAIL
 */
/* -------------------------------------------*/
int grd_range_list(redisContext *conn, char *key, int count, RVALUES values, int *get_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;
    int max_count = 0;

    reply = redisCommand(conn, "LRANGE %s 0 %d", key, count);
//    grd_test_reply_type(reply);
	if (reply->type != REDIS_REPLY_ARRAY) {
		fprintf(stderr, "[-][GMS_REDIS]LRANGE %s error!\n", key);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]LRANGE %s  error!%s\n", key, conn->errstr);
		retn = -1;
	}


    max_count = (reply->elements > count) ? count: reply->elements;
    *get_num = max_count;


    for (i = 0; i < max_count; ++i) {
        strncpy(values[i], reply->element[i]->str, VALUES_ID_SIZE-1);
    }

	freeReplyObject(reply);
	return retn;
}
