/**
 * @file store_cfg.c
 * @brief  存储模块配置信息模块 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-18
 */
#include "gms_store.h"


trace_init_t        g_store_trace;                        /* trace 全局 */
unsigned int        g_store_debug        = 0;            /* 存储模块debug配置 */
store_debug_info_t    g_store_debug_cfg;            

store_conf_t        g_store_info_cfg;                        /* 存储模块 基本配置信息 */    

char * ip_int_to_char(char *pint,char *pchar)
{
	long long lnum;
	struct in_addr addr;
	
	lnum=atoll(pint);
	addr.s_addr=(uint32_t)ntohl(lnum);
	pchar=inet_ntoa(addr);
	return pchar;
}
void show_store_config(store_conf_t* conf, store_debug_info_t* debug)
{
    printf("-------  CONFIG INFORMATION -------\n");
#if 1
    printf("\n=====> ++++ debug ++++\n");
    printf("debug_all :\t%d\n", debug->debug_all);
    printf("debug_trunk:\t%d\n", debug->debug_trunk);
    printf("debug_attack:\t%d\n", debug->debug_attack);
    printf("debug_3rd:\t%d\n", debug->debug_3rd);
    printf("debug_av:\t%d\n", debug->debug_av);
    printf("debug_abb:\t%d\n", debug->debug_abb);
    printf("debug_black:\t%d\n", debug->debug_black);
    printf("debug_dev_status:\t%d\n", debug->debug_dev_status);
    printf("debug_spendtime:\t%d\n", debug->debug_spendtime);
#endif
    printf("\n Database conf: %s\n", conf->database_path);

    printf("\n=====> ++++ postgreSQL ++++\n");
    printf("db_name:\t%s\n", conf->psql_dbname);
    printf("user:\t\t%s\n", conf->psql_user);
    printf("pwd:\t\t%s\n", conf->psql_pwd);
    printf("ip:\t\t%s\n", conf->psql_ip);
    printf("port:\t\t%s\n", conf->psql_port);

    printf("\n=====> ++++ Redis ++++\n");
    printf("ip :\t\t%s\n", conf->redis_ip);
    printf("port:\t\t%s\n", conf->redis_port);
    printf("\n--------------------------------\n");

	printf("\n=====> ++++ MYSQL ++++\n");
    printf("db_name:\t%s\n", conf->mysql_dbname);
    printf("user:\t\t%s\n", conf->mysql_user);
    printf("pwd:\t\t%s\n", conf->mysql_pwd);
    printf("ip:\t\t%s\n", conf->mysql_ip);
    printf("port:\t\t%d\n", conf->mysql_port);
    printf("table_name:\t\t%s\n", conf->mysql_table_name);
    printf("mysql_switch:\t\t%d\n", conf->mysql_switch);


    printf("\n=====> ++++ file path ++++\n");
    printf("\nattack_path:\t\t%s\n", conf->event_attack_file_path);
    printf("\n3rd_path:\t\t%s\n", conf->event_3rd_file_path);
    printf("\nabb_path:\t\t%s\n", conf->event_abb_file_path);
    printf("\nav_path:\t\t%s\n", conf->event_av_file_path);
    printf("\nblack_url_path:\t\t%s\n", conf->event_burl_file_path);
    printf("\nblack_dns_path:\t\t%s\n", conf->event_bdns_file_path);
    printf("\ndev_status_path:\t\t%s\n", conf->event_dev_status_file_path);
    printf("\ncomm_conf_path:\t\t%s\n", conf->comm_conf_path);
    printf("\ndevid_path:\t\t%s\n", conf->dev_id_path);

    printf("\n=====> ++++ WRONG file path ++++\n");
    printf("\nattack_path:\t\t%s\n", conf->wrong_attack_file_path);
    printf("\n3rd_path:\t\t%s\n", conf->wrong_3rd_file_path);
    printf("\nabb_path:\t\t%s\n", conf->wrong_abb_file_path);
    printf("\nav_path:\t\t%s\n", conf->wrong_av_file_path);
    printf("\nblack_url_path:\t\t%s\n", conf->wrong_burl_file_path);
    printf("\nblack_dns_path:\t\t%s\n", conf->wrong_bdns_file_path);
    printf("\ndev_status_path:\t\t%s\n", conf->wrong_dev_status_file_path);
}

/* -------------------------------------------*/
/**                                              
 * @brief  存储主函数需要读的配置文件          
 */                                              
/* -------------------------------------------*/
cfg_desc g_store_cfg[] = {

    /* ---------- debug ----------- */
    {"STORE_DEBUG", "debug_all", &(g_store_debug_cfg.debug_all), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_trunk", &(g_store_debug_cfg.debug_trunk), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_attack", &(g_store_debug_cfg.debug_attack), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_3rd", &(g_store_debug_cfg.debug_3rd), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_av", &(g_store_debug_cfg.debug_av), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_abb", &(g_store_debug_cfg.debug_abb), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_black", &(g_store_debug_cfg.debug_black), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_dev_status", &(g_store_debug_cfg.debug_dev_status), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"STORE_DEBUG", "debug_spendtime", &(g_store_debug_cfg.debug_spendtime), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    /* ---------- debug end --------*/


    /* --------event attack -----------*/
    {"EVENT_ATTACK", "show_event_num", &(g_store_info_cfg.attack_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    {"EVENT_ATTACK", "file_path", &(g_store_info_cfg.event_attack_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/me/att"},
    {"EVENT_ATTACK", "wrong_path", &(g_store_info_cfg.wrong_attack_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/att/"},
    /* --------event attack end--------*/

    /* --------   event 3rd -----------*/
    {"EVENT_3RD", "file_path", &(g_store_info_cfg.event_3rd_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/me/3rd"},
    {"EVENT_3RD", "wrong_path", &(g_store_info_cfg.wrong_3rd_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/3rd/"},
    /* event web */
    {"EVENT_WEB", "show_event_num", &(g_store_info_cfg.web_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    /* event code */
    {"EVENT_CODE", "show_event_num", &(g_store_info_cfg.code_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    /* event else */
    {"EVENT_ELSE", "show_event_num", &(g_store_info_cfg.else_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    /* --------   event 3rd -----------*/

    /* --------   event abb -----------*/
    {"EVENT_ABB", "show_event_num", &(g_store_info_cfg.abb_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    {"EVENT_ABB", "file_path", &(g_store_info_cfg.event_abb_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/ae"},
    {"EVENT_ABB", "wrong_path", &(g_store_info_cfg.wrong_abb_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/ae/"},
    /* --------   event abb    --------*/

    /* ---------- event av ----------- */
    {"EVENT_AV", "show_event_num", &(g_store_info_cfg.av_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    {"EVENT_AV", "file_path", &(g_store_info_cfg.event_av_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/ve"},
    {"EVENT_AV", "wrong_path", &(g_store_info_cfg.wrong_av_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/ve/"},
    /* ---------- event av end-------- */

    /* ---------- event black ----------- */
    {"EVENT_BLACK_URL", "show_event_num", &(g_store_info_cfg.url_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    {"EVENT_BLACK_URL", "file_path", &(g_store_info_cfg.event_burl_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/url"},
    {"EVENT_BLACK_URL", "wrong_path", &(g_store_info_cfg.wrong_burl_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/url/"},
    {"EVENT_BLACK_DNS", "show_event_num", &(g_store_info_cfg.dns_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "20"},
    {"EVENT_BLACK_DNS", "file_path", &(g_store_info_cfg.event_bdns_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/dns"},
    {"EVENT_BLACK_DNS", "wrong_path", &(g_store_info_cfg.wrong_bdns_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/dns/"},
    /* ---------- event black end-------- */

    /* ---------- event dev status -------- */
    {"EVENT_DEV_STATUS", "file_path", &(g_store_info_cfg.event_dev_status_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/devstatus"},
    {"EVENT_DEV_STATUS", "comm_path", &(g_store_info_cfg.comm_dev_status_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/comm/devstat/up/"},
    {"EVENT_DEV_STATUS", "wrong_path", &(g_store_info_cfg.wrong_dev_status_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/devstatus/"},
    /* ---------- event dev status end-------- */

    /* ---------- config path ---------------- */
    {"CONFIG_PATH", "database", &(g_store_info_cfg.database_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/doc/conf/database.conf"},
    {"CONFIG_PATH", "comm_interface", &(g_store_info_cfg.comm_conf_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/comm_interface_conf.xml"},
    {"CONFIG_PATH", "uuid", &(g_store_info_cfg.dev_id_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/uuid.conf"},
    /* ---------- config path end ---------------- */

    {"", "", NULL, 0, 0, 0, 0, ""},
};

/* -------------------------------------------*/
/**                                              
 * @brief  存储的数据库配置文件
 */                                              
/* -------------------------------------------*/
cfg_desc g_database_cfg[] = {

    /* ----------- postgreSQL ----- */
    {"POSTGRESQL", "db_name", &(g_store_info_cfg.psql_dbname), IP_STR_SIZE, CFG_TYPE_STR, 1, 0, "postgres"},
    {"POSTGRESQL", "user", &(g_store_info_cfg.psql_user), USER_NAME_SIZE, CFG_TYPE_STR, 1, 0, "postgres"},
    {"POSTGRESQL", "pwd", &(g_store_info_cfg.psql_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, ""},
    {"POSTGRESQL", "ip", &(g_store_info_cfg.psql_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
    {"POSTGRESQL", "port", &(g_store_info_cfg.psql_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "5432"},
    {"POSTGRESQL", "unix", &(g_store_info_cfg.psql_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"POSTGRESQL", "domain_sock", &(g_store_info_cfg.psql_domain), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/.s.PGSQL.5432"},
    /* ----------- postgreSQL end ----- */

    /* ----------- redis --------- */
    {"REDIS", "ip", &(g_store_info_cfg.redis_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
    {"REDIS", "port", &(g_store_info_cfg.redis_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "6379"},
    {"REDIS", "pwd", &(g_store_info_cfg.redis_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, "123456"},
    {"REDIS", "unix", &(g_store_info_cfg.redis_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"REDIS", "domain_sock", &(g_store_info_cfg.redis_domain), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/redis.sock"},
    /* ----------- redis end ----- */
#if 1
	/* ----------- MYSQL ----- */
    {"MYSQL", "db_name", &(g_store_info_cfg.mysql_dbname), IP_STR_SIZE, CFG_TYPE_STR, 1, 0, "gms_db"},
    {"MYSQL", "user", &(g_store_info_cfg.mysql_user), USER_NAME_SIZE, CFG_TYPE_STR, 1, 0, "root"},
    {"MYSQL", "pwd", &(g_store_info_cfg.mysql_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, "123456"},
    {"MYSQL", "ip", &(g_store_info_cfg.mysql_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
    {"MYSQL", "port", &(g_store_info_cfg.mysql_port), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "3306"},
    {"MYSQL", "table_name", &(g_store_info_cfg.mysql_table_name), IP_STR_SIZE, CFG_TYPE_STR, 1, 0, "t_event"},
    {"MYSQL", "mtx", &(g_store_info_cfg.mysql_mtx), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"MYSQL", "apt", &(g_store_info_cfg.mysql_apt), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"MYSQL", "vds", &(g_store_info_cfg.mysql_vds), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"MYSQL", "switch", &(g_store_info_cfg.mysql_switch), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 1, 0, "1"},
	/* ----------- MYSQL end----- */
#endif
    {"", "", NULL, 0, 0, 0, 0, ""},
};



/* -------------------------------------------*/
/**
 * @brief   根据不同的表封装不同的批量插入语句 
 *
 * @param thread_no        线程ID
 * @param cmd            封装好的SQL
 * @param cmd_max_len    SQL最大长度
 *
 */
/* -------------------------------------------*/
void init_copy_cmd(char thread_no, char *cmd, unsigned int cmd_max_len)
{
    switch (thread_no) {
        case THREAD_ATTACK:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,ename,nameid,isccserver,etype,begin_time,end_time,handledeadline,eventtype,eventbasetype,eventexttype,daddr,saddr,sport,dport,eventdetail,enum,risk_level,query_id, month_id, day_id) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_ATTACK_TMP);
            break;
        case THREAD_3RD:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,ename,nameid,isccserver,etype,begin_time,end_time,handledeadline,eventtype,eventbasetype,eventexttype,daddr,saddr,sport,dport,eventdetail,enum,risk_level,query_id, month_id, day_id) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_3RD_TMP);
            break;
        case THREAD_AV:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,ts,proc,vxid,vxname,engine,sip,dip,sport,dport,pid,tdir,adir,cid,risk,idir,fid,etype,btype,stype,smac,dmac,apn,imsi,msisdn,imei,ext_ptr) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_AV);
            break;
        case THREAD_ABB:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,etype,sip,dip,iplen,sport,dport,proto,domain,dnsip,timenow,year,month,day,hour,min,sec,timedata,ruleid,rulename,sip_str,dip_str,payload,reserved1,reserved2,reserved3,reserved4,reserved5,reserved6) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_ABB);
            break;
        case THREAD_BLACK_URL:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,sip,dip,url,method,host,user_agent,param,timenow,sport,dport,sip_str,dip_str,referer) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_BURL);
            break;
        case THREAD_BLACK_DNS:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,etype,sip,dip,dnsip,dns,ctime,sport,dport,resources,sip_str,dip_str) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_BDNS);
            break;
        case THREAD_DEV_STATUS:
            snprintf(cmd, cmd_max_len-1, "COPY %s (devid,c_time,disk,cpu,mem,state,runtime,libversion,sysversion) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", TABLE_NAME_DEV_STATUS);
            break;
        default:
            break;
    }
}

void change_att_copy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name)
{
    snprintf(cmd, cmd_max_len-1, "COPY %s (devid,ename,nameid,isccserver,etype,begin_time,end_time,handledeadline,eventtype,eventbasetype,eventexttype,daddr,saddr,sport,dport,eventdetail,enum,risk_level,query_id, month_id, day_id) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", table_name);
}

void change_dev_status_copy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name)
{
    snprintf(cmd, cmd_max_len-1, "COPY %s (devid,c_time,disk,cpu,mem,state,runtime,libversion,sysversion,data,db) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", table_name);
}

/* -------------------------------------------*/
/**
* @brief  创建存储进程必要的目录
*
* @returns   
*           0 成功
*           -1 有失败的目录
*/
/* -------------------------------------------*/
int create_store_proc_dir(void)
{
    int retn = 0;
    
    /* 1. attack */
    retn = create_dir_ex(g_store_info_cfg.event_attack_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_attack_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_attack_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_attack_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_attack_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_attack_file_path);

    /* 3rd */
    retn = create_dir_ex(g_store_info_cfg.event_3rd_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_3rd_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_3rd_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_3rd_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_3rd_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_3rd_file_path);

    /* av */
    retn = create_dir_ex(g_store_info_cfg.event_av_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_av_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_av_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_av_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_av_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_av_file_path);

    /* abb */
    retn = create_dir_ex(g_store_info_cfg.event_abb_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_abb_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_abb_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_abb_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_abb_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_abb_file_path);

    /* black */
    retn = create_dir_ex(g_store_info_cfg.event_burl_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_burl_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_burl_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_burl_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_burl_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_burl_file_path);

    retn = create_dir_ex(g_store_info_cfg.event_bdns_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_bdns_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_bdns_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_bdns_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_bdns_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_bdns_file_path);

    /* devstatus */
    retn = create_dir_ex(g_store_info_cfg.event_dev_status_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.event_dev_status_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.event_dev_status_file_path);

    retn = create_dir_ex(g_store_info_cfg.wrong_dev_status_file_path);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_store_info_cfg.wrong_dev_status_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_store_info_cfg.wrong_dev_status_file_path);

    return retn;
}

int read_comm_interface_conf(char* comm_conf_path, char *monitor)
{
    int retn = 0;
    memset(monitor, 0, 2);
    xmlDocPtr doc; //定义解析文档指针
    xmlNodePtr curNode; //定义结点指针(你需要它为了在各个结点间移动)
    xmlChar *szKey; //临时字符串变量
    char *szDocName = comm_conf_path;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(szDocName); //解析文件
    if (NULL == doc) {
        printf("Path: %s Document not parsed successfully\n", comm_conf_path);
        retn = -1;
        goto END;
    }
    curNode = xmlDocGetRootElement(doc); //确定文档根元素
    if (NULL == curNode) {
        printf("empty document\n");
        retn = -1;
        goto END_1;
    }
    if (xmlStrcmp(curNode->name, BAD_CAST "cominfo")) {
        printf("document of the wrong type, root node != cominfo\n");
        retn = -1;
        goto END_1;
    }

    curNode = curNode->xmlChildrenNode;
    curNode = curNode->xmlChildrenNode;
    curNode = curNode->xmlChildrenNode;

    while(curNode != NULL) {
        //取出节点中的内容
        if((!xmlStrcmp(curNode->name, (const xmlChar *)"monitor"))) {
            szKey = xmlNodeGetContent(curNode);
           // printf("name=%s  content = %s\n", curNode->name, szKey);
            strncpy(monitor, szKey, 1);
            xmlFree(szKey);
        }
        curNode = curNode->next;
    }
END_1:
    xmlFreeDoc(doc);
END:
    return retn;
}
