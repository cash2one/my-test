/**
 * @file psql_conn.c
 * @brief  PostgreSQL相关链接建立，关闭，捕获状态等
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 
 * @date 2013-10-28
 */

#include "ddos_psql.h"
#include "ddos_make_log.h"
/* -------------------------------------------*/
/**
 * @brief  与数据库建立链接 TCP/IP（阻塞模式）
 *		   
 *
 * @param host		数据库服务器地址
 * @param port		数据库服务器端口
 * @param dbname	数据库名称
 * @param user		登陆用户名
 * @param pwd		登陆密码
 *
 * @returns   
 *		成功 返回与数据库的链接句柄
 *		失败 返回NULL
 */
/* -------------------------------------------*/
PGconn* gpq_connectdb(IN const char* host, 
					  IN const char* port, 
					  IN const char* dbname,
					  IN const char* user,
					  IN const char* pwd)
{
	PGconn* conn = PQsetdbLogin(host,
								port,
								NULL,
								NULL,
								dbname,
								user,
								pwd);	
	switch (PQstatus(conn)) {
		case CONNECTION_OK:
			fprintf(stderr, "[+][GMS_PSQL]connect to %s:%s, db_name=%s, user=%s SUCCESS!\n", host, port, dbname, user);
			//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[+][GMS_PSQL]connect to %s:%s, db_name=%s, user=%s SUCCESS!\n", host, port, dbname, user);
			break;

		case CONNECTION_BAD:
			fprintf(stderr, "[-][GMS_PSQL]connect to %s:%s, db_name=%s, user=%s ERROR!:%s\n", host, port, dbname, user, PQerrorMessage(conn));	
			//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]connect to %s:%s, db_name=%s, user=%s ERROR!:%s id=%d################\n", host, port, dbname, user, PQerrorMessage(conn),CONNECTION_BAD);	
			PQfinish(conn);
			conn = NULL;
			break;

		default:
			fprintf(stderr, "[-][GMS_PSQL]connect to %s:%s, db_name=%s, user=%s Other ERROR!:%s\n", host, port, dbname, user, PQerrorMessage(conn));
			//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]connect to %s:%s, db_name=%s, user=%s Other ERROR!:%s\n", host, port, dbname, user, PQerrorMessage(conn));
			PQfinish(conn);
			conn = NULL;
			break;
	}

	return conn;
}


static char *gpq_make_conninfo(OUT char *conninfo,
                               IN const char *host,
                               IN const char *dbname,
                               IN const char *user,
                               IN const char *pwd)
{
    char *conn_string = conninfo;

    if (conn_string == NULL) {
        goto END;
    }

    memset(conn_string, 0, GPQ_CONN_INFO_STRING_MAX);
    sprintf(conn_string, "host=%s dbname=%s user=%s password=%s", 
             host, dbname, user, pwd);

END:
    return conn_string;
}

/* -------------------------------------------*/
/**
 * @brief  与数据库建立链接Unix domain（阻塞模式）
 *		   
 * @param host       unix_sock path
 * @param dbname	数据库名称
 * @param user		登陆用户名
 * @param pwd		登陆密码
 *
 * @returns   
 *		成功 返回与数据库的链接句柄
 *		失败 返回NULL
 */
/* -------------------------------------------*/
PGconn* gpq_connectdb_host(IN const char *host,
                        IN const char* dbname,
					    IN const char* user,
					    IN const char* pwd)
{
    char conninfo[GPQ_CONN_INFO_STRING_MAX];

    gpq_make_conninfo(conninfo, host, dbname, user, pwd);
    //printf("conn_info = %s\n", conninfo);
    
    PGconn *conn = PQconnectdb(conninfo);

	switch (PQstatus(conn)) {
		case CONNECTION_OK:
			fprintf(stderr, "[+][GMS_PSQL]connect to domain=%s, db_name=%s, user=%s SUCCESS!\n", host, dbname, user);
			//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[+][GMS_PSQL]connect to domain=%s, db_name=%s, user=%s SUCCESS!\n", host, dbname, user);
			break;

		case CONNECTION_BAD:
			fprintf(stderr, "[-][GMS_PSQL]connect to domain=%s, db_name=%s, user=%s ERROR!:%s\n", host, dbname, user, PQerrorMessage(conn));	
			//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]connect to domain=%s, db_name=%s, user=%s ERROR!:%s\n", host, dbname, user, PQerrorMessage(conn));	
			PQfinish(conn);
			conn = NULL;
			break;

		default:
			fprintf(stderr, "[-][GMS_PSQL]connect to domain=%s, db_name=%s, user=%s Other ERROR!:%s\n", host, dbname, user, PQerrorMessage(conn));
			//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]connect to domain=%s, db_name=%s, user=%s Other ERROR!:%s\n", host, dbname, user, PQerrorMessage(conn));
			PQfinish(conn);
			conn = NULL;
			break;
	}

	return conn;
}

/* -------------------------------------------*/
/**
 * @brief  关闭一个已链接的数据库链接句柄
 *
 * @param conn		链接句柄
 */
/* -------------------------------------------*/
void gpq_disconnect(IN PGconn *conn)
{
	if (conn == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Disconnect NULL conn object!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Disconnect NULL conn object!\n");
		return;
	}

	PQfinish(conn);
	fprintf(stderr, "[+][GMS_PSQL]Disconnect SUCCESS!\n");
}

/* -------------------------------------------*/
/**
 * @brief  执行一条完整的sql语句
 *		   本函数只是单纯的输入命令给数据库，不能捕获返回
 *			数据
 *			
 *
 * @param conn		与数据库的链接句柄
 * @param sql_cmd	完整的sql指令
 *
 * @returns   
 *		0		SUCC
 *		-1		fail	
 */
/* -------------------------------------------*/
int gpq_sql_cmd(IN PGconn *conn, IN const char *sql_cmd)
{
	int retn = 0;
	PGresult* res = NULL;

	if (conn == NULL || sql_cmd == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Use NULL pointer!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Use NULL pointer!\n");
		retn = -1;
		goto END;
	}

	res = PQexec(conn, sql_cmd);		
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "[-][GMS_PSQL]command :%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]command :%s ERROR!:%s  id:%d**************************\n", sql_cmd, PQerrorMessage(conn),PQresultStatus(res));
		retn = -1;
		if(PQresultStatus(res) == 7 )
		retn= -2;
		goto END;
	}
	
END:
	PQclear(res);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  执行一条完整的sql语句
 *		   本函数通过sql语句等到数据存放在PGresult
 *			数据结构中，通过注册func回调函数来对
 *			结果进行操作
 *			
 *
 * @param conn		与数据库的链接句柄
 * @param sql_cmd	完整的sql指令
 * @param func		通过sql的语句值进行一系列操作的回调函数
 *
 * @returns   
 *		0		SUCC
 *		-1		fail	
 */
/* -------------------------------------------*/
int gpq_sql_cmd_dump(IN PGconn *conn, IN const char *sql_cmd, int (*func)(PGresult*))
{
	int retn = 0;
	PGresult* res = NULL;

	if (conn == NULL || sql_cmd == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Use NULL pointer!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Use NULL pointer!\n");
		retn = -1;
		goto END;
	}

	res = PQexec(conn, sql_cmd);		
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "[-][GMS_PSQL]command :%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]command :%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

	/* 并无 注册回调函数 */
	if (func == NULL) {
		goto END;
	}

	retn = func(res);
	if (retn == -1) {
		fprintf(stderr, "[-][GMS_PSQL]callback_func ERROR!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]callback_func ERROR!\n");
		retn = -1;
		goto END;
	}
	
END:
	PQclear(res);
	return retn;
}


/* -------------------------------------------*/
/**
 * @brief  执行一条完整的sql语句
 *		   本函数通过sql语句等到数据存放在PGresult
 *			数据结构中，通过注册func回调函数来对
 *			结果进行操作
 *			
 *
 * @param conn		与数据库的链接句柄
 * @param sql_cmd	完整的sql指令
 * @param func		通过sql的语句值进行一系列操作的回调函数
 * @param arg		对于func的传入或者传出参数
 *
 * @returns   
 *		0		SUCC
 *		-1		fail	
 */
/* -------------------------------------------*/
int gpq_sql_cmd_dump_arg(IN PGconn *conn, IN const char *sql_cmd, int (*func)(IN PGresult*, void*), void* arg)
{
	int retn = 0;
	PGresult* res = NULL;

	if (conn == NULL || sql_cmd == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Use NULL pointer!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Use NULL pointer!\n");
		retn = -1;
		goto END;
	}

	res = PQexec(conn, sql_cmd);		
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "[-][GMS_PSQL]command :%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]command :%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

	/* 并无 注册回调函数 */
	if (func == NULL) {
		goto END;
	}

	retn = func(res, arg);
	if (retn == -1) {
		fprintf(stderr, "[-][GMS_PSQL]callback_func ERROR!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]callback_func ERROR!\n");
		retn = -1;
		goto END;
	}
	
END:
	PQclear(res);
	return retn;

}

/* -------------------------------------------*/
/**
 * @brief  执行一个带参数的sql指令,和 
 *		    gpq_sql_cmd类似
 *
 * @param conn			与数据库的链接句柄
 * @param sql_cmd		含有参数的sql语句
 * @param prm_num		参数个数
 * @param prm_values	参数值数组，均以字符串形式存放
 * @param prm_lengths	参数长度数组
 * @param prm_formats	参数类型数组
 *
 * @returns   
 *			0 SUCC
 *			-1 fail
 */
/* -------------------------------------------*/
int gpq_cmd_params(IN PGconn *conn, 
				   IN const char * sql_cmd, 
				   IN int prm_num,
				   IN const char* const* prm_values,
				   IN const int* prm_lengths,
				   IN const int* prm_formats)
{
	
	int retn = 0;
	PGresult *res = NULL;

	if (conn == NULL || sql_cmd == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Use NULL pointer!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Use NULL pointer!\n");
		retn = -1;
		goto END;
	}

	res = PQexecParams(conn, 
								sql_cmd, 
								prm_num,
								NULL,/*此参数为 传入参数prm_values中各个参数的绑定类型*/
									 /*设置为NULL 为让系统自动识别类型*/
								prm_values,
								prm_lengths,
								prm_formats,
								PSQL_RESULT_FORMAT_BYTE/* 返回结果的类型 设置为二进制 */);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "[-][GMS_PSQL]command with params:%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]command with params:%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

END:
	PQclear(res);
	
	return retn;
}


/* -------------------------------------------*/
/**
 * @brief  执行一个带参数的sql指令
 *		   本函数通过sql语句等到数据存放在PGresult
 *			数据结构中，通过注册func回调函数来对
 *			结果进行操作 对于func有参数
 *		    gpq_sql_cmd_dump类似
 *
 * @param conn			与数据库的链接句柄
 * @param sql_cmd		含有参数的sql语句
 * @param prm_num		参数个数
 * @param prm_values	参数值数组，均以字符串形式存放
 * @param prm_lengths	参数长度数组
 * @param prm_formats	参数类型数组
 * @param func			通过sql的语句值进行一系列操作的回调函数
 *
 * @returns   
 *			0 SUCC
 *			-1 fail
 */
/* -------------------------------------------*/
int gpq_cmd_params_dump(IN PGconn *conn, 
				   IN const char * sql_cmd, 
				   IN int prm_num,
				   IN const char* const* prm_values,
				   IN const int* prm_lengths,
				   IN const int* prm_formats,
				   IN int (*func)(PGresult*))
{
	
	int retn = 0;
	PGresult *res = NULL;

	if (conn == NULL || sql_cmd == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Use NULL pointer!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Use NULL pointer!\n");
		retn = -1;
		goto END;
	}

	res = PQexecParams(conn, 
								sql_cmd, 
								prm_num,
								NULL,/*此参数为 传入参数prm_values中各个参数的绑定类型*/
									 /*设置为NULL 为让系统自动识别类型*/
								prm_values,
								prm_lengths,
								prm_formats,
								PSQL_RESULT_FORMAT_BYTE/* 返回结果的类型 设置为二进制 */);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "[-][GMS_PSQL]command with params:%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]command with params:%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

	/* 并无 注册回调函数 */
	if (func == NULL) {
		goto END;
	}

	retn = func(res);
	if (retn == -1) {
		fprintf(stderr, "[-][GMS_PSQL]params_dump callback_func ERROR!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]params_dump callback_func ERROR!\n");
		retn = -1;
		goto END;
	}

END:
	PQclear(res);
	
	return retn;
}


/* -------------------------------------------*/
/**
 * @brief  执行一个带参数的sql指令
 *		   本函数通过sql语句等到数据存放在PGresult
 *			数据结构中，通过注册func回调函数来对
 *			结果进行操作 对于func有参数
 *		    gpq_sql_cmd_dump类似
 *
 * @param conn			与数据库的链接句柄
 * @param sql_cmd		含有参数的sql语句
 * @param prm_num		参数个数
 * @param prm_values	参数值数组，均以字符串形式存放
 * @param prm_lengths	参数长度数组
 * @param prm_formats	参数类型数组
 * @param func			通过sql的语句值进行一系列操作的回调函数
 * @param arg			对于func的传入或者传出参数
 *
 * @returns   
 *			0 SUCC
 *			-1 fail
 */
/* -------------------------------------------*/
int gpq_cmd_params_dump_arg(IN PGconn *conn, 
				   IN const char * sql_cmd, 
				   IN int prm_num,
				   IN const char* const* prm_values,
				   IN const int* prm_lengths,
				   IN const int* prm_formats,
				   IN int (*func)(PGresult*, void*),
				   IN void *arg)
{
	
	int retn = 0;
	PGresult *res = NULL;

	if (conn == NULL || sql_cmd == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]Use NULL pointer!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]Use NULL pointer!\n");
		retn = -1;
		goto END;
	}

	res = PQexecParams(conn, 
								sql_cmd, 
								prm_num,
								NULL,/*此参数为 传入参数prm_values中各个参数的绑定类型*/
									 /*设置为NULL 为让系统自动识别类型*/
								prm_values,
								prm_lengths,
								prm_formats,
								PSQL_RESULT_FORMAT_BYTE/* 返回结果的类型 设置为二进制 */);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "[-][GMS_PSQL]command with params:%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]command with params:%s ERROR!:%s\n", sql_cmd, PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

	/* 并无 注册回调函数 */
	if (func == NULL) {
		goto END;
	}

	retn = func(res, arg);
	if (retn == -1) {
		fprintf(stderr, "[-][GMS_PSQL]params_dump_arg callback_func ERROR!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]params_dump_arg callback_func ERROR!\n");
		retn = -1;
		goto END;
	}

END:
	PQclear(res);
	
	return retn;
}


/* -------------------------------------------*/
/**
 * @brief  将buf中的批量数据插入指定的table_name表中
 *			 使用自定义copy语句 
 *
 * @param conn			链接句柄
 * @param table_name	表名
 * @param copy_cmd		自定义copy语句
 * @param buf			存放的数据
 * @param buf_size		数据长度
 *
 * @returns   
 *			0   SUCC
 *			-1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_from_buf_sql(IN PGconn *conn,
						  IN char* table_name,
						  IN char* copy_cmd,
						  IN void* buf,
						  IN unsigned long buf_size)
{
	int retn = 0;	

	char *errormsg = NULL;
	
	if (copy_cmd == NULL) {
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	if (buf_size <= 0) {
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	PGresult* res = PQexec(conn, copy_cmd);
	if (PQresultStatus(res) != PGRES_COPY_IN) {
		fprintf(stderr, "[-][GMS_PSQL]copy_from buf, table: %s, sql: Error:{%s}\n", table_name,PQerrorMessage(conn));
		
		//CA_LOG(LOG_MODULE,LOG_PROC,"unknown attack_type:%d!\n",statis_node->attack_type);
		CA_LOG(LOG_MODULE,LOG_PROC, "[-][GMS_PSQL]copy_from buf, sql:%s Error:{%s}\n", copy_cmd, PQerrorMessage(conn));
		retn = -1;
		goto END;
	}
	PQclear(res);

	retn = PQputCopyData(conn, buf, buf_size);
	if (retn < 0) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, put copy data error!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy data error!\n");
		CA_LOG(LOG_MODULE,LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy data error!\n");
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	retn = PQputCopyEnd(conn, errormsg);
	if (retn < 0) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, put copy end error!:%s\n", PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy end error!:%s\n", PQerrorMessage(conn));		
		CA_LOG(LOG_MODULE,LOG_PROC, "[-][GMS_PSQL]copy_from buf, put copy end error!:%s\n", PQerrorMessage(conn));
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	if (errormsg != NULL) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, put copy end errormsg != NULL:%s", errormsg);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy end errormsg != NULL:%s", errormsg);
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	res = PQgetResult(conn);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, get result failed:%s\n", PQerrorMessage(conn));
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][GMS_PSQL]copy from buf, get result failed:%s\n", PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

END:
	PQclear(res);

END_WITHOUT_CLEAR:

	return retn;

}

/* -------------------------------------------*/
/**
 * @brief  将buf中的批量数据插入指定的table_name表中
 *			 使用固定的copy语句 
 *	COPY table_name FROM STDIN CSV DELIMITER '|' NULL '\N' ESCAPE '\' HEADER
 *
 * @param conn			链接句柄
 * @param table_name	表名
 * @param buf			存放的数据
 * @param buf_size		数据长度
 *
 * @returns   
 *			0   SUCC
 *			-1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_from_buf(IN PGconn* conn, 
		              IN char* table_name, 
					  IN void* buf, 
					  IN unsigned long buf_size)
{
	int retn = 0;	
	char copy_cmd[400];

	char *errormsg = NULL;

	if (buf_size <= 0) {
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	
	sprintf(copy_cmd, "COPY %s (devid,ename,etype,begin_time,end_time,saddr,sport,daddr,dport,deal_status,num,h_num,risk_level,query_id,remark,resp) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\' HEADER", table_name);
	//sprintf(copy_cmd, "COPY %s FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\' HEADER", table_name);

	PGresult* res = PQexec(conn, copy_cmd);
	if (PQresultStatus(res) != PGRES_COPY_IN) {
		fprintf(stderr, "[-][GMS_PSQL]copy_from buf, sql:%s Error!\n", copy_cmd);
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy_from buf, sql:%s Error!\n", copy_cmd);
		retn = -1;
		goto END;
	}
	PQclear(res);

	retn = PQputCopyData(conn, buf, buf_size);
	if (retn < 0) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, put copy data error!\n");
        //CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy data error!\n");
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	retn = PQputCopyEnd(conn, errormsg);
	if (retn < 0) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, put copy end error!:%s\n", PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy end error!:%s\n", PQerrorMessage(conn));
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}

	if (errormsg != NULL) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, put copy end errormsg != NULL:%s", errormsg);
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from buf, put copy end errormsg != NULL:%s", errormsg);
		retn = -1;
		goto END_WITHOUT_CLEAR;
	}


	res = PQgetResult(conn);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "[-][GMS_PSQL]copy from buf, get result failed:%s\n", PQerrorMessage(conn));
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from buf, get result failed:%s\n", PQerrorMessage(conn));
		retn = -1;
		goto END;
	}

END:
	PQclear(res);

END_WITHOUT_CLEAR:

	return retn;
}


/* -------------------------------------------*/
/**
 * @brief  将指定文件的批量数据插入指定的table_name表中
 *
 * @param conn			链接句柄
 * @param table_name	表名
 * @param file_path		文件路径
 *
 * @returns   
 *			0   SUCC
 *			-1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_from_file(IN PGconn* conn, IN char* table_name, IN char* file_path)
{
	int retn = 0;	
	char copy_cmd[400];

	sprintf(copy_cmd, "copy %s (devid,ename,etype,begin_time,end_time,saddr,sport,daddr,dport,deal_status,num,h_num,risk_level,query_id,remark_resp) from '%s' CSV DELIMITER '|' NULL '\\N' HEADER", table_name, file_path);

	retn = gpq_sql_cmd(conn, copy_cmd);
	if (retn < 0) {
		fprintf(stderr, "[-][GMS_PSQL]copy from file, ERROR!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy from file, ERROR!\n");
		retn = -1;
		goto END;
	}

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  将指定表中的数据的批量存入指定的file文件中
 *
 * @param conn			链接句柄
 * @param table_name	表名
 * @param file_path		文件路径
 *
 * @returns   
 *			0   SUCC
 *			-1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_to_file(IN PGconn* conn, IN char* table_name, IN char* file_path)
{
	int retn = 0;	
	char copy_cmd[400];

	sprintf(copy_cmd, "copy %s to '%s' CSV NULL '\\N' HEADER", table_name, file_path);

	retn = gpq_sql_cmd(conn, copy_cmd);
	if (retn < 0) {
		fprintf(stderr, "[-][GMS_PSQL]copy to file, ERROR!\n");
		//CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]copy to file, ERROR!\n");
		retn = -1;
		goto END;
	}

END:
	return retn;
}

