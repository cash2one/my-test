/**
 * @file redis_hash.c
 * @brief  redis 操作hash接口实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-04
 */

#include "gms_redis.h"
#include <string.h>
#include <stdlib.h>


/* 封装一个 hmset 命令 */
static char* make_hmset_command(char* key, unsigned int element_num, RFIELDS fields, RVALUES values)
{
	char *cmd = NULL;	
	unsigned int buf_size = 0;
	unsigned int use_size = 0;
	unsigned int i = 0;

	cmd = (char*)malloc(1024*1024);
	if (cmd == NULL) {
		goto END;
	}
	memset(cmd, 0, 1024*1024);
	buf_size += 1024*1024;

	strncat(cmd, "hmset", 6);
	use_size += 5;
	strncat(cmd, " ", 1);
	use_size += 1;

	strncat(cmd, key, 200);
	use_size += 200;

	for (i = 0; i < element_num; ++i) {

		strncat(cmd, " ", 1);
		use_size += 1;
		if (use_size >= buf_size) {
			cmd = realloc(cmd, use_size + 1024*1024);
			if (cmd == NULL) {
				goto END;
			}
			buf_size += 1024*1024;
		}

		strncat(cmd, fields[i], FIELD_ID_SIZE);
		use_size += strlen(fields[i]);
		if (use_size >= buf_size) {
			cmd = realloc(cmd, use_size + 1024*1024);
			if (cmd == NULL) {
				goto END;
			}
			buf_size += 1024*1024;
		}


		strncat(cmd, " ", 1);
		use_size += 1;
		if (use_size >= buf_size) {
			cmd = realloc(cmd, use_size + 1024*1024);
			if (cmd == NULL) {
				goto END;
			}
			buf_size += 1024*1024;
		}

		strncat(cmd, values[i], VALUES_ID_SIZE);
		use_size += strlen(values[i]);
		if (use_size >= buf_size) {
			cmd = realloc(cmd, use_size + 1024*1024);
			if (cmd == NULL) {
				goto END;
			}
			buf_size += 1024*1024;
		}

	}

END:
	return cmd;
}


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
int grd_hash_set_append(redisContext *conn, char *key, RFIELDS fields, RVALUES values, int val_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;

	/* 批量插入命令到缓冲命令管道 */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "hset %s %s %s", key, fields[i], values[i]);
        if (retn != REDIS_OK) {
            fprintf(stderr, "[-][GMS_REDIS]HSET %s %s %s ERROR!\n", key, fields[i], values[i]);
            CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]HSET %s %s %s ERROR![%s]\n", key, fields[i], values[i], conn->errstr);
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
			fprintf(stderr, "[-][GMS_REDIS]Commit HSET %s %s %s ERROR!\n", key, fields[i], values[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Commit HSET %s %s %s ERROR![%s]\n", key, fields[i], values[i], conn->errstr);
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
 * @brief			创建或者覆盖一个HASH表
 *
 * @param conn				已建立好的链接
 * @param key				hash 表名
 * @param element_num		hash 表区域个数
 * @param fields			hash 表区域名称数组char(*)[FIELD_ID_SIZE]
 * @param values			hash 表区域值数组  char(*)[VALUES_ID_SIZE]
 *
 * @returns   
 *			0   成功	
 *			-1  失败
 */
/* -------------------------------------------*/
int grd_create_or_replace_hash_table(redisContext* conn,
									 char* key, 
									 unsigned int element_num, 
									 RFIELDS fields, 
									 RVALUES values)
{
	int retn = 0;
	redisReply *reply = NULL;			

	char *cmd = make_hmset_command(key, element_num, fields, values);		
	if (cmd == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]create hash table %s error\n", key);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]create hash table %s error\n", key);
		retn = -1;
		goto END_WITHOUT_FREE;
	}

	reply = redisCommand(conn, cmd);
//	grd_test_reply_type(reply);
	if (strcmp(reply->str, "OK") != 0) {
		fprintf(stderr, "[-][GMS_REDIS]Create hash table %s Error:%s\n", key, reply->str);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Create hash table %s Error:%s,%s\n", key, reply->str, conn->errstr);
		
		retn = -1;
		goto END;
	}

	printf("[+][GMS_REDIS]Create hash table %s SUCCESS!\n", key);

END:
	free(cmd);
	freeReplyObject(reply);

END_WITHOUT_FREE:

	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  给指定的hash表 指定的field对应的value自增num
 *
 * @param conn			已建立好的链接
 * @param key			hash表名
 * @param field			hash表下的区域名	
 *
 * @returns   
 *			0		succ
 *			-1		fail
 */
/* -------------------------------------------*/
int grd_hincrement_one_field(redisContext *conn, char *key, char *field, unsigned int num)
{
	int retn = 0;

	redisReply *reply = NULL;

	reply = redisCommand(conn, "HINCRBY %s %s %d", key, field, num);
	if (reply == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]increment %s %s error %s\n", key, field, conn->errstr);	
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]increment %s %s error %s\n", key, field, conn->errstr);	
		retn =  -1;
		goto END;
	}

END:
	freeReplyObject(reply);

	return retn;
}
