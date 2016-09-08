/**
 * @file redis_zset.c
 * @brief  redis 操作zset接口实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-04
 */

#include "gms_redis.h"
#include <string.h>

/* -------------------------------------------*/
/**
 * @brief		将指定的zset表，对应的成员，值自增1
 *				（key 或 成员不存在 则创建）
 *
 * @param conn		已建立的链接
 * @param key		zset表名
 * @param member	zset成员名
 *
 * @returns   
 *			0			succ
 *			-1			fail
 */
/* -------------------------------------------*/
int grd_zset_increment(redisContext *conn, char* key, char* member)
{
	int retn = 0;	

	redisReply *reply = NULL;

	reply = redisCommand(conn, "ZINCRBY %s 1 %s", key, member);
	//grd_test_reply_type(reply);
	if (strcmp(reply->str, "OK") != 0) {
		fprintf(stderr, "[-][GMS_REDIS]Add or increment table: %s,member: %s Error:%s\n", key, member,reply->str);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Add or increment table: %s,member: %s Error:%s,%s\n", key, member,reply->str, conn->errstr);
		
		retn = -1;
		goto END;
	}

END:
	freeReplyObject(reply);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief	 批量将指定的zset表，对应的成员，值自增1
 *				（key 或 成员不存在 则创建）
 *
 * @param conn		已建立好的链接
 * @param key		有序集合名称
 * @param values	封装好的成员数组
 * @param val_num	数据个数
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int grd_zset_increment_append(redisContext *conn, char *key, RVALUES values, int val_num)
{
	int retn = 0;
	int i = 0;
	redisReply *reply = NULL;

	/* 批量命令到缓冲管道 */
	for (i = 0; i < val_num; ++i) {
		retn = redisAppendCommand(conn, "ZINCRBY %s 1 %s", key, values[i]);
		if (retn != REDIS_OK) {
			fprintf(stderr, "[-][GMS_REDIS]ZINCRBY %s 1 %s ERROR!\n", key, values[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]ZINCRBY %s 1 %s ERROR! %s\n", key, values[i], conn->errstr);
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
			fprintf(stderr, "[-][GMS_REDIS]Commit ZINCRBY %s 1 %s ERROR!\n", key, values[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Commit ZINCRBY %s 1 %s ERROR!%s\n", key, values[i], conn->errstr);
			freeReplyObject(reply);
			break;
		}
		freeReplyObject(reply);
		retn = 0;
	}

END: 
	return retn;
}

