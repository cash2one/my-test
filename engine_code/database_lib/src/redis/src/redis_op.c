/**
 * @file redis_op.c
 * @brief  redis 操作基本接口和key的操作实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-25
 */

#include "gms_redis.h"


/* -------------------------------------------*/
/**
 * @brief  选择redis一个数据库
 *
 * @param conn		已链接的数据库链接
 * @param db_no		redis数据库编号
 *
 * @returns   
 *			-1 失败
 *			0  成功
 */
/* -------------------------------------------*/
int grd_selectdatabase(redisContext *conn, unsigned int db_no)
{
	int retn = 0;
	redisReply *reply = NULL;

	/* 选择一个数据库 */
	reply = redisCommand(conn, "select %d", db_no);
	if (reply == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Select database %d error!\n", db_no);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Select database %d error!%s\n", db_no, conn->errstr);
		retn = -1;
		goto END;
	}

	printf("[+][GMS_REDIS]Select database %d SUCCESS!\n", db_no);
	CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[+][GMS_REDIS]Select database %d SUCCESS!\n", db_no);

END:
	freeReplyObject(reply);
	return retn;
}


/* -------------------------------------------*/
/**
 * @brief			清空当前数据库所有信息(慎用)
 *
 * @param conn		已链接的数据库链接
 *
 * @returns   
 *			-1 失败
 *			0  成功
 */
/* -------------------------------------------*/
int grd_flush_database(redisContext *conn)
{
	int retn = 0;	
	redisReply *reply = NULL;

	reply = redisCommand(conn, "FLUSHDB");
	if (reply == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Clear all data error\n");
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Clear all data error\n");
		retn = -1;
		goto END;
	}

	printf("[+][GMS_REDIS]Clear all data!!\n");
	CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC,"[+][GMS_REDIS]Clear all data!!\n");

END:
	freeReplyObject(reply);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  判断key值是否存在
 *
 * @param conn		已经建立的链接
 * @param key		需要寻找的key值
 *
 * @returns   
 *				-1 失败
 *				1 存在
 *				0 不存在
 */
/* -------------------------------------------*/
int grd_is_key_exist(redisContext *conn, char* key)
{
	int retn = 0;	

	redisReply *reply = NULL;

	reply = redisCommand(conn, "EXISTS %s", key);
	//grd_test_reply_type(reply);
	if (reply->type != REDIS_REPLY_INTEGER) {
		fprintf(stderr, "[-][GMS_REDIS]is key exist get wrong type!\n");
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]is key exist get wrong type! %s\n", conn->errstr);
		retn = -1;
		goto END;
	}

	if (reply->integer == 1) {
		retn = 1;	
	}
	else {
		retn = 0;
	}

END:
	freeReplyObject(reply);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief			删除一个key
 *
 * @param conn		已经建立的链接
 * @param key		
 *
 * @returns   
 *				-1 失败
 *				0 成功
 */
/* -------------------------------------------*/
int grd_del_key(redisContext *conn, char *key)
{
	int retn = 0;
	redisReply *reply = NULL;

	reply = redisCommand(conn, "DEL %s", key);
	if (reply->type != REDIS_REPLY_INTEGER) {
		fprintf(stderr, "[-][GMS_REDIS] DEL key %s ERROR\n", key);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS] DEL key %s ERROR %s\n", key, conn->errstr);
		retn = -1;
		goto END;
	}

	if (reply->integer > 0) {
		retn = 0;	
	}
	else {
		retn = -1;
	}

END:
	freeReplyObject(reply);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  设置一个key的删除时间 ，系统到达一定时间
 *			将会自动删除该KEY
 *
 * @param conn				已经建立好的链接
 * @param delete_time		到期事件 time_t 日历时间
 *
 * @returns   
 *		0	SUCC
 *		-1  FAIL
 */
/* -------------------------------------------*/
int grd_set_key_lifecycle(redisContext *conn, char *key, time_t delete_time)
{
	int retn = 0;
	redisReply *reply = NULL;		
	
	reply = redisCommand(conn, "EXPIREAT %s %d", key, delete_time);
	if (reply->type != REDIS_REPLY_INTEGER) {
		fprintf(stderr, "[-][GMS_REDIS]Set key:%s delete time ERROR!\n", key);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Set key:%s delete time ERROR! %s\n", key, conn->errstr);
		retn = -1;
	}
	if (reply->integer == 1) {
		/* 成功 */
		retn = 0;
	}
	else {
		/* 错误 */
		retn = -1;
	}


	freeReplyObject(reply);	
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  打印库中所有匹配pattern的key
 *
 * @param conn		已建立好的链接
 * @param pattern	匹配模式，pattern支持glob-style的通配符格式，
 *					如 *表示任意一个或多个字符，
 *					   ?表示任意字符，
 *				    [abc]表示方括号中任意一个字母。
 */
/* -------------------------------------------*/
void grd_show_keys(redisContext *conn, char* pattern)
{
	int i = 0;
	redisReply *reply = NULL;

	reply = redisCommand(conn, "keys %s", pattern);
	if (reply->type != REDIS_REPLY_ARRAY) {
		fprintf(stderr, "[-][GMS_REDIS]show all keys and data wrong type!\n");
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]show all keys and data wrong type! %s\n", conn->errstr);
		goto END;
	}

	for (i = 0; i < reply->elements; ++i) {
		printf("======[%s]======\n", reply->element[i]->str);
	}


END:
	freeReplyObject(reply);
}

/* -------------------------------------------*/
/**
 * @brief  批量执行已经封装好的redis 命令
 *
 * @param conn		已建立好的链接
 * @param cmds		封装好的命令数组
 * @param cmd_num	命令个数
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int grd_redis_append(redisContext *conn, RCOMMANDS cmds, int cmd_num)
{
	int retn = 0;
	int i = 0;
	redisReply *reply = NULL;


	/* 批量插入命令到缓冲命令管道 */
	for (i = 0; i < cmd_num; ++i) {
		retn = redisAppendCommand(conn, cmds[i]);
		if (retn != REDIS_OK) {
			fprintf(stderr, "[-][GMS_REDIS]Append Command: %s ERROR!\n", cmds[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Append Command: %s ERROR! %s\n", cmds[i], conn->errstr);
			retn = -1;
			goto END;
		}
		retn = 0;
	}

	/* 提交命令 */
	for (i = 0; i < cmd_num; ++i) {
		retn = redisGetReply(conn, (void**)&reply);
		if (retn != REDIS_OK) {
			retn = -1;
			fprintf(stderr, "[-][GMS_REDIS]Commit Command:%s ERROR!\n", cmds[i]);
			CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Commit Command:%s ERROR! %s\n", cmds[i], conn->errstr);
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
 * @brief  执行单向命令 无返回值 命令自行输入
 *
 * @param conn		已建立的链接
 * @param cmd		封装好的命令
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int grd_redis_command(redisContext *conn, char *cmd)
{
	int retn = 0;

	redisReply *reply = NULL;

	reply = redisCommand(conn, cmd);
	if (reply == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Command : %s ERROR!\n", cmd);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Command : %s ERROR!%s\n", cmd, conn->errstr);
		retn = -1;
	}

	freeReplyObject(reply);

	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  测试一个reply的结果类型
 *			得到对应的类型用对应的方法获取数据
 *
 * @param reply		返回的命令结果
 */
/* -------------------------------------------*/
void grd_test_reply_type(redisReply *reply)
{
	switch (reply->type) {
		case REDIS_REPLY_STATUS:
			printf("[+][GMS_REDIS]=REDIS_REPLY_STATUS=[string] use reply->str to get data, reply->len get data len\n");
			break;
		case REDIS_REPLY_ERROR:
			printf("[+][GMS_REDIS]=REDIS_REPLY_ERROR=[string] use reply->str to get data, reply->len get date len\n");
			break;
		case REDIS_REPLY_INTEGER:
			printf("[+][GMS_REDIS]=REDIS_REPLY_INTEGER=[long long] use reply->integer to get data\n");
			break;
		case REDIS_REPLY_NIL:
			printf("[+][GMS_REDIS]=REDIS_REPLY_NIL=[] data not exist\n");
			break;
		case REDIS_REPLY_ARRAY:
			printf("[+][GMS_REDIS]=REDIS_REPLY_ARRAY=[array] use reply->elements to get number of data, reply->element[index] to get (struct redisReply*) Object\n");
			break;
		case REDIS_REPLY_STRING:
			printf("[+][GMS_REDIS]=REDIS_REPLY_string=[string] use reply->str to get data, reply->len get data len\n");
			break;
		default:
			printf("[-][GMS_REDIS]Can't parse this type\n");
			break;
	}
}

