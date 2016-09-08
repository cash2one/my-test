/**
 * @file flow_cfg.c
 * @brief  流量存储配置文件
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-03
 */

#include "gms_flow.h"

trace_init_t			g_flow_trace;
unsigned int			g_flow_debug = 0;
flow_conf_t				g_flow_info_cfg;
flow_debug_info_t		g_flow_debug_cfg;


cfg_desc  g_flow_cfg[] = {
    /* ------------ debug ----------*/
    {"FLOW_DEBUG", "debug_all", &(g_flow_debug_cfg.debug_all), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"FLOW_DEBUG", "debug_trunk", &(g_flow_debug_cfg.debug_trunk), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"FLOW_DEBUG", "debug_store", &(g_flow_debug_cfg.debug_store), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"FLOW_DEBUG", "debug_make", &(g_flow_debug_cfg.debug_make), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"FLOW_DEBUG", "debug_spendtime", &(g_flow_debug_cfg.debug_spendtime), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"FLOW_DEBUG", "store_enable", &(g_flow_debug_cfg.store_enable), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "yes"},
    {"FLOW_DEBUG", "statistics_enable", &(g_flow_debug_cfg.statistics_enable), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "yes"},
    {"FLOW_DEBUG", "flowevent_store_disenable", &(g_flow_debug_cfg.flowevent_store_disenable), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    /* ------------ debug end ----------*/


    /* ------------ dev -------- */
#if 0
    {"DEVPARAM", "dev_id", &(g_flow_info_cfg.dev_id), 30, CFG_TYPE_STR, 0, 0, "dev_id"},
#endif
    {"DEVPARAM", "query_id", &(g_flow_info_cfg.query_id), CFG_SIZE_YN_UINT8, CFG_TYPE_UINT8, 0, 0, "0"},
    /* ------------ dev end-------- */

    /* ------------- flow_show_num ------------- */
    {"EVENT_FLOW", "show_flow_num", &(g_flow_info_cfg.flow_show_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "360"},
    {"EVENT_FLOW", "dst_file_path", &(g_flow_info_cfg.dst_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/comm/flow"},
    {"EVENT_FLOW", "src_file_path", &(g_flow_info_cfg.src_file_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/flow"},
    {"EVENT_FLOW", "wrong_path", &(g_flow_info_cfg.wrong_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/db/wrong/flow/"},
    /* ------------- flow_show_num ------------- */

    /* config_path */
    {"CONFIG_PATH", "database", &(g_flow_info_cfg.database_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/doc/conf/database.conf"},
    {"CONFIG_PATH", "uuid", &(g_flow_info_cfg.dev_id_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/doc/conf/uuid.cfg"},
    {"CONFIG_PATH", "comm_interface", &(g_flow_info_cfg.comm_conf_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/comm/conf/comm_interface_conf.xml"},
    {"CONFIG_PATH", "custom_flow", &(g_flow_info_cfg.custom_flow_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/custom_flow.conf"},
    {"CONFIG_PATH", "flowdb_ini", &(g_flow_info_cfg.flowdb_ini_path), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/home/apt/config/fdb.ini"},


    {"", "", NULL, 0, 0, 0, 0, ""},
};

/* -------------------------------------------*/
/**                                              
 * @brief  存储的数据库配置文件
 */                                              
/* -------------------------------------------*/
cfg_desc g_database_cfg[] = {

    /* ----------- postgreSQL ----- */
    {"POSTGRESQL", "db_name", &(g_flow_info_cfg.psql_dbname), IP_STR_SIZE, CFG_TYPE_STR, 1, 0, "postgres"},
    {"POSTGRESQL", "user", &(g_flow_info_cfg.psql_user), USER_NAME_SIZE, CFG_TYPE_STR, 1, 0, "postgres"},
    {"POSTGRESQL", "pwd", &(g_flow_info_cfg.psql_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, ""},
    {"POSTGRESQL", "ip", &(g_flow_info_cfg.psql_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
    {"POSTGRESQL", "port", &(g_flow_info_cfg.psql_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "5432"},
    {"POSTGRESQL", "unix", &(g_flow_info_cfg.psql_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"POSTGRESQL", "domain_sock", &(g_flow_info_cfg.psql_domain), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/.s.PGSQL.5432"},
    /* ----------- postgreSQL end ----- */

    /* ----------- redis --------- */
    {"REDIS", "ip", &(g_flow_info_cfg.redis_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
    {"REDIS", "port", &(g_flow_info_cfg.redis_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "6379"},
    {"REDIS", "pwd", &(g_flow_info_cfg.redis_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, "123456"},
    {"REDIS", "unix", &(g_flow_info_cfg.redis_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
    {"REDIS", "domain_sock", &(g_flow_info_cfg.redis_domain), ABSOLUT_FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/redis.sock"},
    /* ----------- redis end ----- */

    {"", "", NULL, 0, 0, 0, 0, ""},
};



void show_flow_config(flow_conf_t* conf, flow_debug_info_t* debug)
{
    printf("-------  CONFIG INFORMATION -------\n");
    printf("\n=====> ++++ debug ++++\n");
    printf("debug_all :\t%d\n",	debug->debug_all);
    printf("debug_trunk:\t%d\n", debug->debug_trunk);
    printf("debug_store:\t%d\n", debug->debug_store);
    printf("debug_spendtime:\t%d\n", debug->debug_spendtime);

    printf("\n Database path : %s\n", conf->database_path);
    printf("\n Uuid_path:\t\t%s\n", conf->dev_id_path);

    printf("\n=====> ++++ postgreSQL ++++\n");
    printf("db_name:\t%s\n", conf->psql_dbname);
    printf("user:\t\t%s\n", conf->psql_user);
    printf("pwd:\t\t%s\n", conf->psql_pwd);
    printf("ip:\t\t%s\n", conf->psql_ip);
    printf("port:\t\t%s\n", conf->psql_port);

    printf("\n=====> ++++ Redis ++++\n");
    printf("ip :\t\t%s\n", conf->redis_ip);
    printf("port:\t\t%s\n", conf->redis_port);

    printf("\n=====> ++++ dev ++++\n");
    printf("dev_id:\t\t%s\n", conf->dev_id);
    printf("query_id:\t\t%d\n", conf->query_id);
    printf("flow_show_num:\t\t%d\n", conf->flow_show_num);
    printf("flow_src_file_path:\t\t%s\n", conf->src_file_path);
    printf("flow_dst_file_path:\t\t%s\n", conf->dst_file_path);
    printf("flow_wrong_path:\t\t%s\n", conf->wrong_path);
    printf("comm_conf_path:\t\t%s\n", conf->comm_conf_path);
    printf("\n--------------------------------\n");
}

void init_flowcopy_cmd(char *cmd, unsigned int cmd_max_len, char *table_name)
{
    snprintf(cmd, cmd_max_len-1, "COPY %s (dev_id,protocol_id,ftime,pps,bps,sip,dip,query_id,month_id) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'", table_name);
}

int create_flow_proc_dir(void)
{
    int retn = 0;
    
    retn = create_dir_ex(g_flow_info_cfg.dst_file_path);
    if (retn < 0) {
        FLOW_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_flow_info_cfg.dst_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_flow_info_cfg.dst_file_path);

    retn = create_dir_ex(g_flow_info_cfg.src_file_path);
    if (retn < 0) {
        FLOW_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_flow_info_cfg.src_file_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_flow_info_cfg.src_file_path);

    retn = create_dir_ex(g_flow_info_cfg.wrong_path);
    if (retn < 0) {
        FLOW_ERROR(DEBUG_TYPE_TRUNK, "Create dir [%s] ERROR!\n", g_flow_info_cfg.wrong_path);
        retn = -1;
    }
    printf("[+] Create [%s] SUCC!\n", g_flow_info_cfg.wrong_path);


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
