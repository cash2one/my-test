#ifndef _QUERY_FLOW_H_
#define _QUERY_FLOW_H_


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "config.h"
#include "store_common.h"
#include "query_config.h"
#include "make_log.h"
#include "flowdb2.h"
#include "flowerr.h"
#include "flowrec.h"
#include "flowstat.h"


#define LOG_MODULE       "query"
#define LOG_PROC         "flow"


typedef struct _query_conn {
    int fd;
    flowdb2_t *flowdb;
	flowquery_t *flow_query;
	flowqueryops_t qopt;
	flowqueryattr_t qattr;
	char flow_condition[64];
    char result_file[ABSOLUT_FILE_PATH_SIZE];
    char result_tmp_file[ABSOLUT_FILE_PATH_SIZE];
} query_conn_t;

int query_entry(char *query_cmd_file);
int create_query_proc_dir(void);

#endif
