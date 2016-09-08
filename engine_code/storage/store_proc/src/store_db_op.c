/**
 * @file store_db_op.c
 * @brief  数据库链接实现操作
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2014-04-21
 */
#include "gms_store.h"
#include<pthread.h>
/* -------------------------------------------*/
/**
 * @brief  链接数据库
 *
 * @param db_conn   链接的数据库句柄
 * @param thread_no 线程序列号
 * 
 * @returns   
 *          -1 postgres redis 均连接失败
 *          1  postgres 链接失败
 */
/* -------------------------------------------*/
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; 
int store_connect_db(store_conn_t *db_conn, char thread_no)
{
    int retn = 0;
    int debug_type = 0;

    switch (thread_no) {
        case THREAD_ATTACK:
            debug_type = DEBUG_TYPE_ATT;
            break;
        case THREAD_3RD:
            debug_type = DEBUG_TYPE_3RD;
            break;
        case THREAD_AV:
            debug_type = DEBUG_TYPE_AV;
            break;
        case THREAD_ABB:
            debug_type = DEBUG_TYPE_ABB;
            break;
        case THREAD_BLACK:
            debug_type = DEBUG_TYPE_BLACK;
            break;
        case THREAD_DEV_STATUS:
            debug_type = DEBUG_TYPE_DEV_STATUS;
            break;
    }

    if (g_store_info_cfg.psql_unix == 1) {
        db_conn->psql_conn = gpq_connectdb_host(g_store_info_cfg.psql_domain,
                                                    g_store_info_cfg.psql_dbname, g_store_info_cfg.psql_user,                           
                                                    g_store_info_cfg.psql_pwd);                                                         
    }
    else {
        db_conn->psql_conn = gpq_connectdb(g_store_info_cfg.psql_ip, g_store_info_cfg.psql_port,                   
                                               g_store_info_cfg.psql_dbname, g_store_info_cfg.psql_user,                           
                                               g_store_info_cfg.psql_pwd);                                                         
    }

    if (db_conn->psql_conn == NULL) {                                                                          
        STORE_ERROR(debug_type, "Connect postgreSQL Server error!\n");                                        
        CA_LOG(LOG_MODULE, LOG_PROC, "****Connect postgreSQL Server ERROR!!****");                                
        retn = -1;
        goto EXIT;                                                                                              
    }

    if (gpq_sql_cmd(db_conn->psql_conn, "set standard_conforming_strings=on") < 0) {                           
        STORE_ERROR(debug_type, "PSQL Command: set standard_conforming_strings=on ERROR");                    
        CA_LOG(LOG_MODULE, LOG_PROC, "****PSQL Command: set standard_conforming_strings=on ERROR!!****");         
        retn = 1;
        goto EXIT;                                                                                              
    }

    if (g_store_info_cfg.redis_unix == 1) {
        db_conn->redis_conn = grd_connectdb_unix(g_store_info_cfg.redis_domain, g_store_info_cfg.redis_pwd);
    }
    else {
        db_conn->redis_conn = grd_connectdb(g_store_info_cfg.redis_ip, g_store_info_cfg.redis_port, g_store_info_cfg.redis_pwd);
    }

    if (db_conn->redis_conn == NULL) {                                                                         
        STORE_ERROR(debug_type, "Connect Redis Server error!\n");                                             
        CA_LOG(LOG_MODULE, LOG_PROC, "****Connect Redis Server ERROR!!****");                                     
        retn = 1;
        goto EXIT;                                                                                              
    }

    if(g_store_info_cfg.mysql_switch == 1) {
		    printf("+++++++++++now connect mysql!\n");
		    printf("ip:%s user:%s\n", g_store_info_cfg.mysql_ip, g_store_info_cfg.mysql_user);
		    db_conn->mysql_conn = mysql_init((MYSQL*)NULL);
		    mysql_thread_init();
		    pthread_mutex_lock(&mutex1);
		    if(mysql_real_connect(db_conn->mysql_conn, g_store_info_cfg.mysql_ip, g_store_info_cfg.mysql_user, g_store_info_cfg.mysql_pwd,g_store_info_cfg.mysql_dbname, g_store_info_cfg.mysql_port, NULL, 0) == NULL) {
			    pthread_mutex_unlock(&mutex1);
			    CA_LOG(LOG_MODULE, LOG_PROC, "connect error %s\n", mysql_error(db_conn->mysql_conn));
			    retn= -3;
				goto EXIT;
		    } else {
			    printf("mysql connect succ!\n");
			    /*mysql_query(db_conn->mysql_conn, "load data local infile \'/gms/storage/conf/att.tmp\' into table t_event character set gbk fields terminated by \'||\'(ename,etype,event_exttype,risk_level,event_time,daddr,saddr,sport,dport,event_detail,url,data_resource);");*/
			    CA_LOG(LOG_MODULE, LOG_PROC, "****Connect mysql Server SUCC!!****");   
		    }
		    pthread_mutex_unlock(&mutex1);	
    }
EXIT:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  断连数据库
 *
 * @param db_conn
 * @param thread_no
 */
/* -------------------------------------------*/
void store_disconnect_db(store_conn_t *db_conn, char thread_no)
{
    int debug_type = 0;

    switch (thread_no) {
        case THREAD_ATTACK:
            debug_type = DEBUG_TYPE_ATT;
            break;
        case THREAD_3RD:
            debug_type = DEBUG_TYPE_3RD;
            break;
        case THREAD_AV:
            debug_type = DEBUG_TYPE_AV;
            break;
        case THREAD_ABB:
            debug_type = DEBUG_TYPE_ABB;
            break;
        case THREAD_BLACK:
            debug_type = DEBUG_TYPE_BLACK;
            break;
        case THREAD_DEV_STATUS:
            debug_type = DEBUG_TYPE_DEV_STATUS;
            break;
    }

    grd_disconnect(db_conn->redis_conn);                                     
    STORE_DEBUG(debug_type, "DISCONNCET redis connection SUCCESS!\n");          
    CA_LOG(LOG_MODULE, LOG_PROC, "DISCONNCET redis connection SUCCESS!\n");                                        
    gpq_disconnect(db_conn->psql_conn);            
    STORE_DEBUG(debug_type, "DISCONNCET posgresql connection SUCCESS!\n");  
    CA_LOG(LOG_MODULE, LOG_PROC, "DISCONNCET posgresql connection SUCCESS!\n");  
	if(g_store_info_cfg.mysql_switch==1)
{
    mysql_close(db_conn->mysql_conn);
	mysql_thread_end();
	printf("now close mysql!\n");
	CA_LOG(LOG_MODULE, LOG_PROC, "DISCONNCET mysql connection SUCCESS!\n");	
}
}

int store_get_redistable_last_time(char *time_str, redisContext *conn, char *key, int col, int max_count)
{
    int retn = 0;
    int value_num = 0;
    int i = 0;
    RVALUES redis_values = NULL;        
    int count = 0;
    char *get_str = NULL;
    char *next_str = NULL;
    char get_value[VALUES_ID_SIZE];
    memset(get_value, 0, VALUES_ID_SIZE);

    redis_values = malloc(max_count * VALUES_ID_SIZE);
    if (redis_values == NULL) {
        CA_LOG(LOG_MODULE, LOG_PROC, "Malloc redis values Error!\n");
        retn = -1;
        goto EXIT;
    }

    retn = grd_range_list(conn, key, max_count, redis_values, &value_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "lrange redis %s error!\n", key);
        retn = -1;
        goto EXIT;
    }

    for (i = 0; i < value_num; ++i) {
        memset(get_value, 0, VALUES_ID_SIZE);

        count = 0;
        get_str = NULL;
        next_str = NULL;
        next_str =  redis_values[i];

        while ((get_str = strtok_r(next_str, REDIS_DILIMT, &next_str)) != NULL) {

            if (count == col) {
                if (strncmp(get_value, get_str, 20) < 0) {
                    strncpy(get_value, get_str, 20);
                }
            }
            ++count;
        }
    }

    strncpy(time_str, get_value, 20);  

    
EXIT:
    if (redis_values != NULL) {
        free(redis_values);
    }
    return retn;
}

