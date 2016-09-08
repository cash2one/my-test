/**
 * @file test_main.c
 * @brief  主要进行PostgreSQL的链接基本操作测试
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 
 * @date 2013-10-28
 */
#include <string.h>
#include "gms_psql.h"
//#include "test_helper.h"

void test_exp()
{
	static char result_tmp[25];
	char *p;
	p = PQgetvalue(NULL, 0, 0);
	strncpy(result_tmp,p,strlen(p));
	 printf("int func gpq_get_value(): PQgetvalue result is %s\n",result_tmp);
}

int main(int argc, char **argv)
{
	int retn = 0;
	struct gpq_conn_info conn_info;	

	memset(&conn_info, 0, GPQ_CONN_INFO_SIZE);

	/* 初始化 链接信息 */
	strncpy(conn_info.host_ip, "127.0.0.1", IP_STR_SIZE-1);
	strncpy(conn_info.host_port, "5432", PORT_STR_SIZE-1);
	strncpy(conn_info.dbname, "gms_db", DBNAME_SIZE-1);
	strncpy(conn_info.user, "gms_user", USER_NAME_SIZE-1);
	strncpy(conn_info.pwd, "", PASSWORD_SIZE-1);



	/* 链接数据库 */
#if 0
	PGconn *conn = gpq_connectdb(conn_info.host_ip,
								 conn_info.host_port,
								 conn_info.dbname,
								 conn_info.user,
								 conn_info.pwd);
	if (conn == NULL) {
		fprintf(stderr, "[-][TEST_MAIN]conn db error\n");
		goto EXIT;
	}
#endif
#if 0
	/* 查询例子, 无参数sql语句 */
	retn = test_select_print(conn);
	if (retn < 0) {
		goto EXIT;
	}
#endif

#if 0
	/* 查询例子， 带参数sql语句 */
	retn = test_select_print_params_sql(conn);
	if (retn < 0) {
		goto EXIT;
	}
#endif
test_exp();
	return 0;

#if 0
	/* copy from 例子 */
	retn = test_copy_in(conn);			
	if (retn < 0) {
		goto EXIT;
	}
#endif


#if 0
	/* copy to 例子*/
	retn = test_copy_to(conn);
	if (retn < 0) {
		goto EXIT;
	}
#endif
	

//EXIT:
	/* 断开数据库 */
	//gpq_disconnect(conn);

	return 0;
}



