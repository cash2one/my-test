/**
 * @file psql_result.c
 * @brief  PostgreSQL 对于结果的基本操作
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-10-30
 */


#include "gms_psql.h"


/* -------------------------------------------*/
/**
 * @brief  得到查询结果的行数（元组个数）
 *
 * @param res		查询结果
 *
 * @returns   
 *			 行数
 *			 -1 error
 */
/* -------------------------------------------*/
int gpq_rlt_get_row(PGresult* res)
{
	int retn = 0;

	if (res == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]get result rows is NULL\n");
		CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]get result rows is NULL\n");
		retn = -1;
		goto END;
	}

	retn = PQntuples(res);

END:
	return retn;
}

int gpq_get_row(PGconn* conn, char *sql_cmd)
{
	int retn = 0;

	PGresult* res = NULL;
	res = PQexec(conn, sql_cmd);
      
	if (res == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]get result rows is NULL\n");
		CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]get result rows is NULL\n");
		retn = -1;
		goto END;
	}

	retn = PQntuples(res);
	PQclear(res);

END:
	return retn;
}
/* -------------------------------------------*/
/**
 * @brief  得到查询结果的列数（字段个数）
 *
 * @param res		查询结果
 *
 * @returns   
 *			 行数
 *			 -1 error
 */
/* -------------------------------------------*/
int gpq_rlt_get_col(PGresult* res)
{
	int retn = 0;

	if (res == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]get result column is NULL\n");
		CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]get result column is NULL\n");
		retn = -1;
		goto END;
	}

	retn = PQnfields(res);

END:
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  得到对应的列的名称(列序号从0开始)
 *
 * @param res  查询结果
 * @param col  列号 
 *
 * @returns   
 *			name 
 */
/* -------------------------------------------*/
char* gpq_rlt_fname(PGresult* res, int col)
{
	return PQfname(res, col);
}

/* -------------------------------------------*/
/**
 * @brief	返回一个PGresult 里面一行的单独的一个字段的值。 
 *   行和字段编号从 0 开始。调用者不应该直接释放结果。
 *    在把 PGresult 句柄传递给 PQclear 之后，结果会被自动释放。
 *	  对于文本格式数据，返回一个字符串。
 *	  对于二进制格式数据，返回结果未测试。TODO
 *    
 *
 * @param res   查询结果
 * @param row	行号
 * @param col	列号
 *
 * @returns   
 *			查询到的数据
 */
/* -------------------------------------------*/
char* gpq_rlt_getvalue(PGresult* res, int row, int col)
{
	return PQgetvalue(res, row, col);
}


char* gpq_get_value(PGconn *conn, char *sql_cmd, int row, int col)
{	
	char *p;
	static char result_tmp[25];
	PGresult* res = NULL;
	PQsendQuery(conn, sql_cmd);
    	PQconsumeInput(conn);     
	PQisBusy(conn);
	res = PQgetResult(conn);
	if (res == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]get result rows is NULL\n");
		CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]get result rows is NULL\n");
	PQclear(res);
		return NULL;
	}
	p = PQgetvalue(res, row, col);	
	printf("int func gpq_get_value(): PQgetvalue result is %s\n",p);
	strcpy(result_tmp,p);
	PQclear(res);
	return result_tmp;

}
char* gpq_get_row_col_value(PGconn *conn, char *sql_cmd, char *user_name,char *user_id)
{	
	char *p;
	PGresult* res = NULL;
	PQsendQuery(conn, sql_cmd);
    	PQconsumeInput(conn);     
	PQisBusy(conn);
	res = PQgetResult(conn);
	if (res == NULL) {
		fprintf(stderr, "[-][GMS_PSQL]get result rows is NULL\n");
		CA_LOG(PSQL_LOG_MODULE, PSQL_LOG_PROC, "[-][GMS_PSQL]get result rows is NULL\n");
	PQclear(res);
		return NULL;
	}
	p = PQgetvalue(res, 0, 0);	
	if(p){
	strncpy(user_name,p,32);
	}
	else{
	PQclear(res);
		return NULL;
}
	p = PQgetvalue(res, 0, 1);	
	if(p){
	strncpy(user_id,p,5);
	}
	else{
	PQclear(res);
		return NULL;
}

	PQclear(res);
	return user_id;

}
