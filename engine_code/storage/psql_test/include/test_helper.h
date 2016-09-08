#ifndef _TEST_HELPER_H_
#define _TEST_HELPER_H_


#include "gms_psql.h"
#include "store_common.h"


int test_select_print(PGconn * conn);
int test_select_print_params_sql(PGconn* conn);
int test_copy_in(PGconn *conn);
int test_copy_to(PGconn *conn);


#endif
