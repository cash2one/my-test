#ifndef _REGULAR_GUARD_H_
#define _REGULAR_GUARD_H_
#include"store_common.h"
#include"config.h"
#include"gms_psql.h"
//#define GUARD_CONF_PATH   "/gms/guard/conf/cronjob.conf"
#define TMP_STOR_PATH     "/gms/guard/conf/tmp_stor"
#define DATA_STOR_PATH     "/gms/guard/conf/data_stor"
#define LAST_TIME_PATH        "/gms/guard/conf/last_time"
#define WRONG_TIME_PATH        "/gms/guard/conf/wrong_time"
#define MAXLEN 		1024
#define ONE_DAY_SEC       		   (86400)//24*60*60
#define ONE_WEEK_SEC                       (604800) //3600*24*7
#define ONE_MONTH_SEC			  (2592000) //3600*24*30
#define HALF_YEAR_SEC                   (15552000) //3600*24*30*6
#define ONE_YEAR_SEC                   (31104000) //3600*24*30*12
#define TIME_FORMAT_SIZE		   25
#define TABLE_NAME "t_event_attacktmp"
#define CMD_LEN 		1024
/* ³£ÓÃ³£Á¿ */
#define IP_STR_SIZE                                     16      /* IPµØÖ·×Ö·û´®³¤¶È */
#define PORT_STR_SIZE                           10      /* PORT×Ö·û´®³¤¶È */
#define DBNAME_SIZE                                     50      /* Êý¾Ý¿âÃû³¤¶È */
#define USER_NAME_SIZE                          50      /* ÓÃ»§Ãû³¤¶È */
#define PASSWORD_SIZE                           100     /* ÃÜÂë³¤¶È */
//#define PQL_CONF_PATH 		"/gms/storage/conf/storage.cfg"

#if 0
typedef struct _pql_conf {

    /* PostgreSQL */
    char psql_dbname[DBNAME_SIZE];
    char psql_user[USER_NAME_SIZE];
    char psql_pwd[PASSWORD_SIZE];
    char psql_ip[IP_STR_SIZE];
    char psql_port[PORT_STR_SIZE];
} pql_conf_t;
extern pql_conf_t pql_conf;

#endif

typedef struct _regular_guard_config {
	char title[10];
	char name[MAXLEN];
	char type[10];
	char interval[10];
	char time_column[15];

} regular_guard_conf_t;

int show_regular_guard_conf(regular_guard_conf_t *conf);
int create_guard_cfg_array(cfg_desc *desc, regular_guard_conf_t *conf, unsigned int conf_num);
int get_sec(char *interval);
int get_delete_time(int *sec, char *delete_time);
int delete_from_database(PGconn* conn, int *sec, char *table_name, char *time);
int delete_from_disk(char *path);  
int regular_guard(PGconn* conn);      
int get_delete_dirname(char *name, char *interval, char *dirname);
int is_dir(char * filename);
int delete_dir(char * dirname);
int sys_cmd(char *cmd);
int get_last_time_one_sec(time_t *diff_time);
#endif
