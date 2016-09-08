/**
 * @file test_db_helper.c
 * @brief  一些测试数据库操作的例子
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 
 * @date 2013-11-19
 */
#include "test_helper.h"
#include <netinet/in.h>
#include <arpa/inet.h>


static int print_result_info(PGresult* res)
{
	int i = 0, j = 0;
	int cols = 0, rows = 0;

	cols = gpq_rlt_get_col(res);
	rows = gpq_rlt_get_row(res);

	/* 打印字段名 */
	for (i = 0; i < cols; ++i) {
		printf("%-15s|", gpq_rlt_fname(res, i));
	}
	printf("\n\n");

	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			printf("%-15s|", gpq_rlt_getvalue(res, i, j));
		}
		printf("\n");
	}

	return 0;
}

static int show_binary_results(PGresult *res)
{
	int         i,
				j;
	int         i_fnum,
				t_fnum,
				b_fnum;

	/* 使用 PQfnumber 来避免对结果中的字段顺序进行假设 */
	i_fnum = PQfnumber(res, "i");
	t_fnum = PQfnumber(res, "t");
	b_fnum = PQfnumber(res, "b");

	for (i = 0; i < PQntuples(res); i++)
	{
		char       *iptr;
		char       *tptr;
		char       *bptr;
		int         blen;
		int         ival;

		/* 获取字段值（我们忽略了它们可能为空的这个可能！）*/
		iptr = PQgetvalue(res, i, i_fnum);
		tptr = PQgetvalue(res, i, t_fnum);
		bptr = PQgetvalue(res, i, b_fnum);

		/*
		 * INT4 的二进制表现形式是网络字节序，
		 * 我们最好转换成本地字节序。
		 */
		ival = ntohl(*((uint32_t *) iptr));

		/*
		 * TEXT 的二进制表现形式是，嗯，文本，因为 libpq 好的会给它附加一个字节零，
		 * 因此把它看做 C 字串就挺好。
		 *
		 * BYTEA 的二进制表现形式是一堆字节，里面可能包含嵌入的空值，
		 * 因此我们必须注意字段长度。
		 */
		blen = PQgetlength(res, i, b_fnum);

		printf("tuple %d: got\n", i);
		printf(" i = (%d bytes) %d\n",
				PQgetlength(res, i, i_fnum), ival);
		printf(" t = (%d bytes) '%s'\n",
				PQgetlength(res, i, t_fnum), tptr);
		printf(" b = (%d bytes) ", blen);
		for (j = 0; j < blen; j++)
			printf("\\%03o", bptr[j]);
		printf("\n\n");
	}

	return 0;
}


int test_select_print(PGconn * conn)
{
	int retn = 0;

	retn = gpq_sql_cmd(conn, "BEGIN");
	if (retn < 0) {
		fprintf(stderr, "[-][TEST_MAIN]cmd BEGIN error\n");
		retn = -1;
		goto EXIT;
	}

	retn = gpq_sql_cmd(conn, "DECLARE mycurosr CURSOR FOR SELECT * FROM t_event_attacktmp");
	if (retn < 0) {
		fprintf(stderr, "[-][TEST_MAIN]cmd select error\n");
		retn = -1;
		goto EXIT;
	}

	retn = gpq_sql_cmd_dump(conn, "FETCH ALL in mycurosr", &print_result_info);
	if (retn < 0) {
		fprintf(stderr, "[-][TEST_MAIN]cmd fetch all error\n");
		retn = -1;
		goto EXIT;
	}

	retn = gpq_sql_cmd(conn, "CLOSE mycurosr");
	if (retn < 0) {
		fprintf(stderr, "[-][TEST_MAIN]cmd CLOSE mycurosr error\n");
		retn = -1;
		goto EXIT;
	}

	retn = gpq_sql_cmd(conn, "END");
	if (retn < 0) {
		fprintf(stderr, "[-][TEST_MAIN]cmd END error\n");
		retn = -1;
		goto EXIT;
	}

EXIT:
	return retn;
}


int test_select_print_params_sql(PGconn* conn)
{
	int retn = 0;	

	const char *prm_values[1];	
	int			prm_lengths[1];
	int			prm_formats[1];



	prm_values[0] = "joe's place";

	retn = gpq_cmd_params_dump(conn,
							   "select * from test1 where t=$1",
							   1,
							   prm_values,
							   NULL,/* 因为是文本，所以不必须要参数长度 */
							   NULL,/* 默认为文本参数 */
							   &show_binary_results);	
	if (retn < 0 ) {
		fprintf(stderr, "[-][TEST_PARAMS_SQL] select error\n");	
		retn = -1;
		goto END;
	}

	uint32_t binary_int = htonl((uint32_t)2);
	prm_values[0] = (char*)&binary_int;
	prm_lengths[0] = sizeof(binary_int);
	prm_formats[0] = PSQL_RESULT_FORMAT_BYTE;

	retn = gpq_cmd_params_dump(conn,
							   "select * from test1 where i=$1::int4",
							   1,
							   prm_values,
							   prm_lengths,
							   prm_formats,
							   &show_binary_results);
	if (retn < 0 ) {
		fprintf(stderr, "[-][TEST_PARAMS_SQL] select error\n");	
		retn = -1;
		goto END;
	}

END:
	return retn;
}


int test_copy_in(PGconn *conn)
{
	int retn = 0;
	test_stat_t test_copy;
	char buf[1024*1024*10];
	unsigned long buf_size;

	TEST_VARS;
	memset(&test_copy, 0, sizeof(test_stat_t));

	TEST_START(test_copy);

	/* 9.2 之前的版本copy命令 需要把这个开关打开 */
	retn = gpq_sql_cmd(conn, "set standard_conforming_strings=on");
	if (retn < 0) {
		fprintf(stderr, "set error!\n"); 

	}


	buf_size = read_file_to_data("/tmp/psql_data/20130603-111122.ok", buf);

	printf("buf size = %ld\n", buf_size);

	retn = gpq_copy_from_buf(conn, "t_event_attacktmp", buf, buf_size);
	if (retn < 0) {
		retn = -1;
		goto END;
	}

	TEST_END(test_copy);
    fprintf(stdout, "spend [%f] sec.\n", test_copy.sec);

END:
	return retn;
}

int test_copy_to(PGconn *conn)
{
	int retn = 0;
	
	retn = gpq_copy_to_file(conn, "weather", "/tmp/weather.data");
	if (retn < 0) {
		retn = -1;
		goto END;
	}

END:
	return retn;
}
