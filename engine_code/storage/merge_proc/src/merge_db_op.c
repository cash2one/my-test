/**
 * @file merge_db_op.c
 * @brief  数据库链接实现操作
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2014-04-22
 */

#include "gms_merge.h"

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
int merge_connect_db(merge_conn_t *db_conn, char thread_no)
{
    int retn = 0;
    int debug_type = 0;

    switch (thread_no) {
        case THREAD_FLOW_MERGE:
            debug_type = DEBUG_TYPE_ATTACK;
            break;
        case THREAD_ATTACK_MERGE:
            debug_type = DEBUG_TYPE_FLOW;
            break;
    }

    if (g_merge_info_cfg.psql_unix == 1) {
        db_conn->psql_conn = gpq_connectdb_host(g_merge_info_cfg.psql_domain,
                                                    g_merge_info_cfg.psql_dbname, g_merge_info_cfg.psql_user,                           
                                                    g_merge_info_cfg.psql_pwd);                                                         
    }
    else {
        db_conn->psql_conn = gpq_connectdb(g_merge_info_cfg.psql_ip, g_merge_info_cfg.psql_port,                   
                                               g_merge_info_cfg.psql_dbname, g_merge_info_cfg.psql_user,                           
                                               g_merge_info_cfg.psql_pwd);                                                         
    }

    if (db_conn->psql_conn == NULL) {                                                                          
        MERGE_ERROR(debug_type, "Connect postgreSQL Server error!\n");                                        
        CA_LOG(LOG_MODULE, LOG_PROC, "****Connect postgreSQL Server ERROR!!****");                                
        retn = -1;
        goto EXIT;                                                                                              
    }

    if (gpq_sql_cmd(db_conn->psql_conn, "set standard_conforming_strings=on") < 0) {                           
        MERGE_ERROR(debug_type, "PSQL Command: set standard_conforming_strings=on ERROR");                    
        CA_LOG(LOG_MODULE, LOG_PROC, "****PSQL Command: set standard_conforming_strings=on ERROR!!****");         
        retn = 1;
        goto EXIT;                                                                                              
    }

    if (g_merge_info_cfg.redis_unix == 1) {
        db_conn->redis_conn = grd_connectdb_unix(g_merge_info_cfg.redis_domain, g_merge_info_cfg.redis_pwd);
    }
    else {
        db_conn->redis_conn = grd_connectdb(g_merge_info_cfg.redis_ip, g_merge_info_cfg.redis_port, g_merge_info_cfg.redis_pwd);
    }

    if (db_conn->redis_conn == NULL) {                                                                         
        MERGE_ERROR(debug_type, "Connect Redis Server error!\n");                                             
        CA_LOG(LOG_MODULE, LOG_PROC, "****Connect Redis Server ERROR!!****");                                     
        retn = 1;
        goto EXIT;                                                                                              
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
void merge_disconnect_db(merge_conn_t *db_conn, char thread_no)
{
    int debug_type = 0;

    switch (thread_no) {
        case THREAD_FLOW_MERGE:
            debug_type = DEBUG_TYPE_ATTACK;
            break;
        case THREAD_ATTACK_MERGE:
            debug_type = DEBUG_TYPE_FLOW;
            break;
    }

    grd_disconnect(db_conn->redis_conn);                                     
    MERGE_DEBUG(debug_type, "DISCONNCET redis connection SUCCESS!\n");          
    CA_LOG(LOG_MODULE, LOG_PROC, "DISCONNCET redis connection SUCCESS!\n");                                        
    gpq_disconnect(db_conn->psql_conn);            
    MERGE_DEBUG(debug_type, "DISCONNCET posgresql connection SUCCESS!\n");  
    CA_LOG(LOG_MODULE, LOG_PROC, "DISCONNCET posgresql connection SUCCESS!\n");   
}
