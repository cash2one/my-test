/**
 * @file redis_api.c
 * @brief  redis 链接基本接口实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-03
 */

#include "gms_redis.h"


/* -------------------------------------------*/
/**
 * @brief  redis tcp模式链接
 *
 * @param ip_str	redis服务器ip
 * @param port_str	redis服务器port
 *
 * @returns   
 *			成功返回链接句柄 
 *			失败返回NULL
 */
/* -------------------------------------------*/
redisContext* grd_connectdb(char *ip_str, char* port_str, char *pwd)
{
	redisContext *conn = NULL;
	uint16_t port = atoi(port_str);
    char auth_cmd[REDIS_COMMAND_SIZE];

	conn = redisConnect(ip_str, port);

	if (conn  == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Connect %s:%d Error:Can't allocate redis context!\n", ip_str, port);		
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Connect %s:%d Error:Can't allocate redis context!\n", ip_str, port);		
		goto END;
	}

	if (conn->err) {
		fprintf(stderr, "[-][GMS_REDIS]Connect %s:%d Error:%s\n", ip_str, port, conn->errstr);	
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Connect %s:%d Error:%s\n", ip_str, port, conn->errstr);	
		redisFree(conn);
		conn = NULL;
		goto END;
	}

    redisReply *reply = NULL;
    sprintf(auth_cmd, "auth %s", pwd);

    reply = redisCommand(conn, auth_cmd);
	if (reply == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Command : auth %s ERROR!\n", pwd);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Command : auth %s ERROR!\n", pwd);
        conn = NULL;
        goto END;
	}
    freeReplyObject(reply);

	
	printf("[+][GMS_REDIS]Connect %s:%d SUCCESS!\n", ip_str, port);
	CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC,"[+][GMS_REDIS]Connect %s:%d SUCCESS!\n", ip_str, port);

END:
	return conn;
}

/* -------------------------------------------*
**
 * @brief  redis unix域模式链接
 *
 * @param ip_str	unix域sock文件
 *
 * @returns   
 *			成功返回链接句柄 
 *			失败返回NULL
 */
/* -------------------------------------------*/
redisContext* grd_connectdb_unix(char *sock_path, char *pwd)
{
	redisContext *conn = NULL;
    char auth_cmd[REDIS_COMMAND_SIZE];

	conn = redisConnectUnix(sock_path);
	if (conn  == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Connect domain-unix:%s Error:Can't allocate redis context!\n", sock_path);		
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Connect domain-unix:%s Error:Can't allocate redis context!\n", sock_path);		
		goto END;
	}

	if (conn->err) {
		fprintf(stderr, "[-][GMS_REDIS]Connect domain-unix:%s Error:%s\n", sock_path, conn->errstr);	
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Connect domain-unix:%s Error:%s\n", sock_path, conn->errstr);	
		redisFree(conn);
		conn = NULL;
		goto END;
	}

    redisReply *reply = NULL;
    sprintf(auth_cmd, "auth %s", pwd);
    reply = redisCommand(conn, auth_cmd);
	if (reply == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Command : auth %s ERROR!\n", pwd);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Command : auth %s ERROR!\n", pwd);
        conn = NULL;
        goto END;
	}
    freeReplyObject(reply);
	
	printf("[+][GMS_REDIS]Connect domain-unix:%s SUCCESS!\n", sock_path);
	CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC,"[+][GMS_REDIS]Connect domain-unix:%s SUCCESS!\n", sock_path);

END:
	return conn;
}

/* -------------------------------------------*/
/**
 * @brief  tcp 链接redis超时等待模式，timeval链接超时
 *			返回
 *
 * @param ip_str		redis 服务器ip
 * @param port_str		redis 服务器端口
 * @param timeval		最大超时等待时间
 *
 * @returns   
 *		成功返回链接句柄
 *		失败返回NULL
 */
/* -------------------------------------------*/
redisContext* grd_connectdb_timeout(char* ip_str, char *port_str, struct timeval *timeout)
{
	redisContext *conn = NULL;
	uint16_t port = atoi(port_str);


	conn = redisConnectWithTimeout(ip_str, port, *timeout);

	if (conn  == NULL) {
		fprintf(stderr, "[-][GMS_REDIS]Connect %s:%d Error:Can't allocate redis context!\n", ip_str, port);
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Connect %s:%d Error:Can't allocate redis context!\n", ip_str, port);
		goto END;
	}

	if (conn->err) {
		fprintf(stderr, "[-][GMS_REDIS]Connect %s:%d Error:%s\n", ip_str, port, conn->errstr);	
		CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[-][GMS_REDIS]Connect %s:%d Error:%s\n", ip_str, port, conn->errstr);	
		redisFree(conn);
		conn = NULL;
		goto END;
	}
	
	printf("[+][GMS_REDIS]Connect %s:%d SUCCESS!\n", ip_str, port);
	CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC,"[+][GMS_REDIS]Connect %s:%d SUCCESS!\n", ip_str, port);

END:
	return conn;
}

/* -------------------------------------------*/
/**
 * @brief  关闭指定的链接句柄
 *
 * @param conn	已建立好的链接
 */
/* -------------------------------------------*/
void grd_disconnect(redisContext* conn)
{
	if (conn == NULL) {
		return ;
	}
	redisFree(conn);
	
	printf("[+][GMS_REDIS]Disconnect SUCCESS!\n");
	CA_LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC,"[+][GMS_REDIS]Disconnect SUCCESS!\n");
}



