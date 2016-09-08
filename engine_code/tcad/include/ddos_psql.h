/**
 * @file gms_psql.h
 * @brief  PostgreSQL数据库操作基本函数接口
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-20
 */
#ifndef _GMS_PSQL_H_
#define _GMS_PSQL_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libpq-fe.h"
#define IN
#define OUT
/* log 模块日志 */

/* 带参数执行函数 返回值类型 */
#define PSQL_RESULT_FORMAT_TEXT                0
#define PSQL_RESULT_FORMAT_BYTE                1


#define GPQ_CONN_INFO_STRING_MAX        200

#define DDOS_MIN_TABLE				"t_event_ddos_show"
#define	DDOS_END_TABLE				"t_event_ddos_his"
#define DDOS_IP_TABLE				"t_event_ddosquery"
#define DDOS_5MIN_SQL_SIZE 			1024
#define DDOS_END_SQL_SIZE			800
#define DDOS_IP_SQL_SIZE			800



/* -------------------------------------------*/
/**
 * @brief  与数据库建立链接 TCP/IP（阻塞模式）
 *           
 *
 * @param host        数据库服务器地址
 * @param port        数据库服务器端口
 * @param dbname    数据库名称
 * @param user        登陆用户名
 * @param pwd        登陆密码
 *
 * @returns   
 *        成功 返回与数据库的链接句柄
 *        失败 返回NULL
 */
/* -------------------------------------------*/
PGconn* gpq_connectdb(IN const char* host, 
                      IN const char* port, 
                      IN const char* dbname,
                      IN const char* user,
                      IN const char* pwd);

/* -------------------------------------------*/
/**
 * @brief  与数据库建立链接Unix domain（阻塞模式）
 *		   
 *
 * @param host		数据库服务器套接字路径
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
					       IN const char* pwd);

/*XXX=========================  对 PGconn 的操作 =====================XXX*/

/* -------------------------------------------*/
/**
 * @brief  关闭一个已链接的数据库链接句柄
 *
 * @param conn        链接句柄
 */
/* -------------------------------------------*/
void gpq_disconnect(IN PGconn *conn);

/* -------------------------------------------*/
/**
 * @brief  执行一条完整的sql语句
 *           本函数只是单纯的输入命令给数据库，不能捕获返回
 *            数据
 *            
 *
 * @param conn        与数据库的链接句柄
 * @param sql_cmd    完整的sql指令
 *
 * @returns   
 *        0        SUCC
 *        -1        fail    
 */
/* -------------------------------------------*/
int gpq_sql_cmd(IN PGconn *conn, IN const char *sql_cmd);

/* -------------------------------------------*/
/**
 * @brief  执行一条完整的sql语句
 *           本函数通过sql语句等到数据存放在PGresult
 *            数据结构中，通过注册func回调函数来对
 *            结果进行操作
 *            
 *
 * @param conn        与数据库的链接句柄
 * @param sql_cmd    完整的sql指令
 * @param func        通过sql的语句值进行一系列操作的回调函数
 *
 * @returns   
 *        0        SUCC
 *        -1        fail    
 */
/* -------------------------------------------*/
int gpq_sql_cmd_dump(IN PGconn *conn, IN const char *sql_cmd, int (*func)(PGresult*));

/* -------------------------------------------*/
/**
 * @brief  执行一条完整的sql语句
 *           本函数通过sql语句等到数据存放在PGresult
 *            数据结构中，通过注册func回调函数来对
 *            结果进行操作 对于func有参数
 *            
 *
 * @param conn        与数据库的链接句柄
 * @param sql_cmd    完整的sql指令
 * @param func        通过sql的语句值进行一系列操作的回调函数
 * @param arg        对于func的传入或者传出参数
 *
 * @returns   
 *        0        SUCC
 *        -1        fail    
 */
/* -------------------------------------------*/
int gpq_sql_cmd_dump_arg(IN PGconn *conn, IN const char *sql_cmd, int (*func)(IN PGresult*, void*), void* arg);


/* -------------------------------------------*/
/**
 * @brief  执行一个带参数的sql指令,和 
 *            gpq_sql_cmd类似
 *
 * @param conn            与数据库的链接句柄
 * @param sql_cmd        含有参数的sql语句
 * @param prm_num        参数个数
 * @param prm_values    参数值数组，均以字符串形式存放
 * @param prm_lengths    参数长度数组
 * @param prm_formats    参数类型数组
 *
 * @returns   
 *            0 SUCC
 *            -1 fail
 */
/* -------------------------------------------*/
int gpq_cmd_params(IN PGconn *conn, 
                   IN const char * sql_cmd, 
                   IN int prm_num,
                   IN const char* const* prm_values,
                   IN const int* prm_lengths,
                   IN const int* prm_formats);


/* -------------------------------------------*/
/**
 * @brief  执行一个带参数的sql指令
 *           本函数通过sql语句等到数据存放在PGresult
 *            数据结构中，通过注册func回调函数来对
 *            结果进行操作 对于func有参数
 *            gpq_sql_cmd_dump类似
 *
 * @param conn            与数据库的链接句柄
 * @param sql_cmd        含有参数的sql语句
 * @param func            通过sql的语句值进行一系列操作的回调函数
 * @param prm_num        参数个数
 * @param prm_values    参数值数组，均以字符串形式存放
 * @param prm_lengths    参数长度数组
 * @param prm_formats    参数类型数组
 *
 * @returns   
 *            0 SUCC
 *            -1 fail
 */
/* -------------------------------------------*/
int gpq_cmd_params_dump(IN PGconn *conn, 
                   IN const char * sql_cmd, 
                   IN int prm_num,
                   IN const char* const* prm_values,
                   IN const int* prm_lengths,
                   IN const int* prm_formats,
                   IN int (*func)(PGresult*));


/* -------------------------------------------*/
/**
 * @brief  执行一个带参数的sql指令
 *           本函数通过sql语句等到数据存放在PGresult
 *            数据结构中，通过注册func回调函数来对
 *            结果进行操作 对于func有参数
 *            gpq_sql_cmd_dump类似
 *
 * @param conn            与数据库的链接句柄
 * @param sql_cmd        含有参数的sql语句
 * @param prm_num        参数个数
 * @param prm_values    参数值数组，均以字符串形式存放
 * @param prm_lengths    参数长度数组
 * @param prm_formats    参数类型数组
 * @param func            通过sql的语句值进行一系列操作的回调函数
 * @param arg            对于func的传入或者传出参数
 *
 * @returns   
 *            0 SUCC
 *            -1 fail
 */
/* -------------------------------------------*/
int gpq_cmd_params_dump_arg(IN PGconn *conn, 
                   IN const char * sql_cmd, 
                   IN int prm_num,
                   IN const char* const* prm_values,
                   IN const int* prm_lengths,
                   IN const int* prm_formats,
                   IN int (*func)(PGresult*, void*),
                   IN void *arg);

/* -------------------------------------------*/
/**
 * @brief  将指定文件的批量数据插入指定的table_name表中
 *
 * @param conn            链接句柄
 * @param table_name    表名
 * @param file_path        文件路径
 *
 * @returns   
 *            0   SUCC
 *            -1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_from_file(IN PGconn* conn, IN char* table_name, IN char* file_path);

/* -------------------------------------------*/
/**
 * @brief  将buf中的批量数据插入指定的table_name表中
 *             使用固定的copy语句 
 *    COPY table_name FROM STDIN CSV DELIMITER '|' NULL '\N' ESCAPE '\' HEADER
 *
 * @param conn            链接句柄
 * @param table_name    表名
 * @param buf            存放的数据
 * @param buf_size        数据长度
 *
 * @returns   
 *            0   SUCC
 *            -1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_from_buf(IN PGconn* conn, 
                      IN char* table_name, 
                      IN void* buf, 
                      IN unsigned long buf_size);

/* -------------------------------------------*/
/**
 * @brief  将buf中的批量数据插入指定的table_name表中
 *             使用自定义copy语句 
 *
 * @param conn            链接句柄
 * @param table_name    表名
 * @param copy_cmd        自定义copy语句
 * @param buf            存放的数据
 * @param buf_size        数据长度
 *
 * @returns   
 *            0   SUCC
 *            -1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_from_buf_sql(IN PGconn *conn,
                          IN char* table_name,
                          IN char* copy_cmd,
                          IN void* buf,
                          IN unsigned long buf_size);

/* -------------------------------------------*/
/**
 * @brief  将指定表中的数据的批量存入指定的file文件中
 *
 * @param conn            链接句柄
 * @param table_name    表名
 * @param file_path        文件路径
 *
 * @returns   
 *            0   SUCC
 *            -1  FAIL
 */
/* -------------------------------------------*/
int gpq_copy_to_file(IN PGconn* conn, IN char* table_name, IN char* file_path);


/*XXX=========================  对 PGresult 的操作 =====================XXX*/

/* -------------------------------------------*/
/**
 * @brief  得到查询结果的行数（元组个数）
 *
 * @param res        查询结果
 *
 * @returns   
 *             行数
 *             -1 error
 */
/* -------------------------------------------*/
int gpq_rlt_get_row(PGresult* res);
int gpq_get_row(PGconn* conn, char *sql_cmd);

/* -------------------------------------------*/
/**
 * @brief  得到查询结果的列数（字段个数）
 *
 * @param res        查询结果
 *
 * @returns   
 *             行数
 *             -1 error
 */
/* -------------------------------------------*/
int gpq_rlt_get_col(PGresult* res);

/* -------------------------------------------*/
/**
 * @brief  得到对应的列的名称(列序号从0开始)
 *
 * @param res  查询结果
 * @param col  列号 
 *
 * @returns   
 *            name 
 */
/* -------------------------------------------*/
char* gpq_rlt_fname(PGresult* res, int col);

/* -------------------------------------------*/
/**
 * @brief    返回一个PGresult 里面一行的单独的一个字段的值。 
 *   行和字段编号从 0 开始。调用者不应该直接释放结果。
 *    在把 PGresult 句柄传递给 PQclear 之后，结果会被自动释放。
 *      对于文本格式数据，返回一个字符串。
 *      对于二进制格式数据，返回结果未测试。TODO
 *    
 *
 * @param res   查询结果
 * @param row    行号
 * @param col    列号
 *
 * @returns   
 *            查询到的数据
 */
/* -------------------------------------------*/
char* gpq_rlt_getvalue(PGresult* res, int row, int col);

char* gpq_get_value(PGconn *conn, char *sql_cmd, int row, int col);
#endif
