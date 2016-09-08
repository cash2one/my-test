/**
 * @file flow_store.c
 * @brief  流量存储流程 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-04
 */

#include "gms_flow.h"

/* 流量数据缓冲 */
char g_flow_buf[FLOW_EVENT_BUF_SIZE];
/* 流量批量插入命令 */
char g_flow_copy_cmd[400];
char g_flow_select_cmd[400];


/* -------------------------------------------*/
/**
 * @brief  存放 flow 流量的 redis key表名链表
 */
/* -------------------------------------------*/
static list_t g_redis_flow_key_list;
pthread_mutex_t g_redis_f_key_list_mutex = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------*/
/**
 * @brief  遍历redis key 链表 exsit 成员置0
 */
/* -------------------------------------------*/
int clear_exist_flag(void *data, void *arg)
{
    f_redis_key_t *node = (f_redis_key_t*)data;

    node->exist = 0;

    return 0;
}

/* -------------------------------------------*/
/**
 * @brief  将文件移动到指定其他的目录下
 *
 * @param file_dir
 * @param other_dir
 * @param file_name
 */
/* -------------------------------------------*/
void move_file_to_other_path(char *file_dir, char *other_dir, char *file_name)
{
    char other_path[ABSOLUT_FILE_PATH_SIZE];
    char other_tmp_path[ABSOLUT_FILE_PATH_SIZE];

    memset(other_path, 0, ABSOLUT_FILE_PATH_SIZE);
    memset(other_tmp_path, 0, ABSOLUT_FILE_PATH_SIZE); 

    strncat(other_path, other_dir, ABSOLUT_FILE_PATH_SIZE);
    strncat(other_tmp_path, other_dir, ABSOLUT_FILE_PATH_SIZE);

    strncat(other_tmp_path, file_name, FILE_NAME_SIZE);  
    strcat(other_tmp_path, ".tmp"); 
    strncat(other_path, file_name, FILE_NAME_SIZE);

    rename(file_dir, other_tmp_path);
    rename(other_tmp_path, other_path);
    FLOW_DEBUG(DEBUG_TYPE_STORE, "@@MOVE@@ [%s] -> [%s].\n", file_dir, other_path);
    CA_LOG(LOG_MODULE, LOG_PROC, "@@MOVE@@ [%s] -> [%s].\n", file_dir, other_path);
}

/* -------------------------------------------*/
/**
 * @brief  遍历redis key 链表 返回找到
 *            的对应redis_tname节点
 */
/* -------------------------------------------*/
int is_key_exist(void *data, void *arg)
{
    f_redis_key_t *node = (f_redis_key_t*)data;    

    if (strncmp(node->key, (char*)arg, KEY_NAME_SIZ) == 0) {
        return 1;
    }
    return 0;
}

int show_redis_key(void *data, void *arg)
{
    f_redis_key_t *node = (f_redis_key_t*)data;

    printf("**************  key name : %s, exist: %d *************\n", node->key, node->exist);

    return 0;
}

/* -------------------------------------------*/
/**
 * @brief  找到exist 为 0 节点
 */
/* -------------------------------------------*/
int find_not_exist_node(void *data, void *arg)
{
    f_redis_key_t *node = (f_redis_key_t*)data;

    if (node->exist == 0) {
        return 1;
    }

    return 0;
}

/* -------------------------------------------*/
/**
 * @brief  截断 redis key list 中的各个表
 */
/* -------------------------------------------*/
int trim_flow_data(void *data, void *arg)
{
    f_redis_key_t *node = (f_redis_key_t*)data;
    flow_conn_t *conn = (flow_conn_t*)arg;

	char cmd[1024]={0};
	int res = 0;
	PGconn* pgsql_conn = NULL;

    int retn = 0;

    retn = grd_get_list_cnt(conn->redis_conn, node->key);
    if (retn < 0) {
        FLOW_ERROR(DEBUG_TYPE_STORE, "Get num of %s ERROR!\n", node->key);
        CA_LOG(LOG_MODULE, LOG_PROC, "Get num of %s ERROR!\n", node->key);
        retn = 0;
        goto END;
    }

    if ((unsigned int)retn > g_flow_info_cfg.flow_show_num) {
#if 0
        if (strncmp(g_flow_info_cfg.is_comm_monitor, "1", 2) == 0){
            retn = grd_trim_list(conn->redis_conn, node->key, 0, g_flow_info_cfg.flow_show_num-1);
            if (retn < 0) {
                FLOW_ERROR(DEBUG_TYPE_STORE, "Trim list %s ERROR!\n", node->key);
                CA_LOG(LOG_MODULE, LOG_PROC, "Trim list %s ERROR!\n", node->key);
                retn = 0;
                goto END;
            }
        }
        else{
#endif
		 	snprintf(cmd, 1023, "select * from t_conf_device where dtype = '2'");					
			printf("cmd [%s]\n",cmd);
	
			res =  gpq_get_row(pgsql_conn, cmd);
			if (res > 0){
				printf("res = %d\n",res);
				retn = grd_trim_list(conn->redis_conn, node->key, 0, (g_flow_info_cfg.flow_show_num*res)-1);
            	if (retn < 0) {
            	    FLOW_ERROR(DEBUG_TYPE_STORE, "Manager Trim list %s ERROR!\n", node->key);
            	    CA_LOG(LOG_MODULE, LOG_PROC, "Manager Trim list %s ERROR!\n", node->key);
            	    retn = 0;
            	    goto END;
            	}
			}
	   // } 

    }

END:
    return 0;
}

/* -------------------------------------------*/
/**
 * @brief 动态管理redis 流量表 
 *        并制作一条流浪数据插入数据库
 */
/* -------------------------------------------*/
void make_and_store_flow_redis_data(f_statistical_t* st_node, cs_arg_t *arg_t)
{
    char value[VALUES_ID_SIZE];
    char col_value[VALUES_ID_SIZE];
    f_redis_key_t *key_node = NULL;
    char *line_head = arg_t->line_head;

    key_node = NULL;
    pthread_mutex_lock(&g_redis_f_key_list_mutex);
    key_node = list_iterate(&g_redis_flow_key_list, LIST_FORWARD, is_key_exist, st_node->redis_tname);
    pthread_mutex_unlock(&g_redis_f_key_list_mutex);

    if (key_node != NULL) {
        key_node->exist = 1;
    }
    else {
        key_node = calloc(1, sizeof(f_redis_key_t));
        strncpy(key_node->key, st_node->redis_tname, KEY_NAME_SIZ);
        key_node->exist = 1;

        /* 加入该节点到 redis key 链表 */
        pthread_mutex_lock(&g_redis_f_key_list_mutex);
        list_enqueue(&g_redis_flow_key_list, key_node);
        pthread_mutex_unlock(&g_redis_f_key_list_mutex);
    }

    /* 制作redis数据 */
    memset(value, 0, VALUES_ID_SIZE);    
    memset(col_value, 0, VALUES_ID_SIZE);    
    /* flow time */
    strncat(value, get_value_by_col(line_head, 3, col_value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
    strncat(value, REDIS_DILIMT, REDIS_DILIMT_SIZ);
    /* bps */
    strncat(value, get_value_by_col(line_head, 5, col_value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
    strncat(value, REDIS_DILIMT, REDIS_DILIMT_SIZ);
    /* pps */
    strncat(value, get_value_by_col(line_head, 4, col_value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);

    /* 将value 存入redis 数据库 */
    grd_list_push(arg_t->redis_conn, st_node->redis_tname, value);

    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", st_node->redis_tname);
}


/* -------------------------------------------*/
/**
 * @brief  核心存储redis流量业务
 */
/* -------------------------------------------*/
int flow_check_and_store(void *data, void *arg)
{
    f_statistical_t *st_node = (f_statistical_t*)data;
    cs_arg_t *arg_t = (cs_arg_t*)arg;
    char protocol_type = *(arg_t->protocol_type);
    char *line_head = arg_t->line_head;
    int retn = 0;


    if (protocol_type < 0) {
        /*  IP 段统计策略 */
        if (st_node->policy_type != 2) {
            goto END;
        }

        /* 根据数据内容得到 redis 表名 */
        char start_ip_value[VALUES_ID_SIZE];
        char end_ip_value[VALUES_ID_SIZE];
        unsigned int start_ip, end_ip;
        char start_ip_str[IP_STR_SIZE];
        char end_ip_str[IP_STR_SIZE];
        char key_name[KEY_NAME_SIZ];

        /* sip */
        get_value_by_col(line_head, 6, start_ip_value, VALUES_ID_SIZE-1, 0);
        /* dip */
        get_value_by_col(line_head, 7, end_ip_value, VALUES_ID_SIZE-1, 0);

        start_ip = atoi(start_ip_value);
        end_ip = atoi(end_ip_value);

        start_ip = htonl(start_ip);
        end_ip = htonl(end_ip);

        sprintf(start_ip_str, NIPQUAD_FMT, NIPQUAD(start_ip));
        printf("start ip %s\n", start_ip_str);
        sprintf(end_ip_str, NIPQUAD_FMT, NIPQUAD(end_ip));
        printf("end ip %s\n", end_ip_str);

        memset(key_name, 0, KEY_NAME_SIZ);

        strncat(key_name, start_ip_str, IP_STR_SIZE-1);
        strncat(key_name, "-", 2);
        strncat(key_name, end_ip_str, IP_STR_SIZE-1);

        if (strcmp(key_name, st_node->redis_tname) == 0) {
            retn = 1;// find, no need to continue
            //printf("key = %s EQUAL\n", key_name);
            make_and_store_flow_redis_data(st_node, arg_t);
        }
    }
    else {
        /* 协议统计策略 */

        if (st_node->policy_type != 1) {
            goto END;    
        }

        if (st_node->protocol_type == protocol_type) {
            retn = 1;// find, no need to continue
            make_and_store_flow_redis_data(st_node, arg_t);
        }
    }

END:
    return retn;
}


/* -------------------------------------------*/
/**
 * @brief  删除 redis key list中 exist为0的节点
 */
/* -------------------------------------------*/
void delete_not_exist_key_node(list_t *list, flow_conn_t *conn)
{
    int list_num = list->node_num;
    int i = 0;
    f_redis_key_t *remove_data = NULL;

    for (i = 0; i < list_num; ++i) {
        remove_data = NULL;
        //TODO 锁
        remove_data = list_iterate(list, LIST_FORWARD, find_not_exist_node, list);
        if (remove_data != NULL) {
            /* 从数据库删除该key */
            grd_del_key(conn->redis_conn, remove_data->key);    
            printf("++++++++++++++++++++++++== delete key from redis key list: %s\n", remove_data->key);

            list_remove(list, remove_data);
            free(remove_data);
        }
    }
}

/* -------------------------------------------*/
/**
 * @brief  redis 入库
 */
/* -------------------------------------------*/
static int store_redis_flow(flow_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line)
{
    char *pos = NULL;
    char *end = NULL;
    char *line_head = NULL;
    int i = 0 ;
    char value[VALUES_ID_SIZE];
    char protocol_type = 0;
    cs_arg_t arg;

    end = buf + buf_size;

    pthread_mutex_lock(&g_redis_f_key_list_mutex);
    list_iterate(&g_redis_flow_key_list, LIST_FORWARD, clear_exist_flag, NULL);
    pthread_mutex_unlock(&g_redis_f_key_list_mutex);

    /*  从数据文件尾部遍历逐条数据 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos + 1;
            }
            else {
                line_head = pos;
            }

            /* 
             * 0. 得到该条数据的协议编号 
             */
            get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0);    
            protocol_type = atoi(value);


            arg.protocol_type = &protocol_type;
            arg.line_head = line_head;
            arg.redis_conn = conn->redis_conn;

            /* 
             * 1. 如果在f_statistical_t 链表中存在 ,不存在则创建
             *    redis key链表，并且exsit设置为1, 并且向redis key中添加数据 
             */
            pthread_mutex_lock(&g_f_policy_list_mutex);
            list_iterate(conn->policy_list, LIST_FORWARD, flow_check_and_store, &arg);    
            pthread_mutex_unlock(&g_f_policy_list_mutex);

            ++i;
        }
    }

    /* 
     * 2. 遍历redis key链表, 本轮中 exsit不为1 的节点从链表中删除 
     *    并且删除数据库中对应的key 
     */
    delete_not_exist_key_node(&g_redis_flow_key_list, conn);


    /* 3. 遍历redis key链表， 截断各链表长度为 g_flow_info_cfg.flow_show_num */
    pthread_mutex_lock(&g_redis_f_key_list_mutex);
    list_iterate(&g_redis_flow_key_list, LIST_FORWARD, trim_flow_data, conn);
    pthread_mutex_unlock(&g_redis_f_key_list_mutex);

    return 0;        
}

/* -------------------------------------------*/
/**
 * @brief  postgresql 入库
 */
/* -------------------------------------------*/
static int store_postgresql_flow(flow_conn_t *conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;
    char table_name[TABLE_NAME_SIZE];

    /* 根据月份选择分区表 */
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, TABLE_NAME_FLOW_MIN);
#if 1
    strncat(table_name, TABLE_SUFFIX, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);
#endif

    init_flowcopy_cmd(g_flow_copy_cmd, 400, table_name);
    printf("file is [%s],buf is [%s]\n",file,buf);

    /* 执行批量插入命令到MINU表 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_flow_copy_cmd, buf, buf_size);
    if (retn < 0) {
        FLOW_ERROR(DEBUG_TYPE_STORE, "copy to %s error!\n", table_name);
        CA_LOG(LOG_MODULE, LOG_PROC, "copy to %s error!\n", table_name);
        /* 将文件移到错误目录下 */
        move_file_to_other_path(file->path, g_flow_info_cfg.wrong_path, file->file_name);
    }
    else {
        /* 将文件移动到通讯目录中 */
        FLOW_DEBUG(DEBUG_TYPE_STORE, "copy %s succ!\n", table_name);
        printf("^^^^^^^^^^copy %s succ!^^^^^^^^^^\n",table_name);
		printf("^^^^^^^^^^insert postgresql succeed !!!!!^^^^^^^^^^\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] -> table[%s] succ!\n", file->path, table_name);

        if (strncmp(g_flow_info_cfg.is_comm_monitor, "1", 2) != 0) {
            printf("dev is not monitor Do not make flow file! DELETE iT!!\n");
			printf("postgresql database!!!\n");
            if (remove(file->path) != 0) {
                perror("remove file");
                FLOW_DEBUG(DEBUG_TYPE_STORE, "remove file %s error!\n", file->path);
                CA_LOG(LOG_MODULE, LOG_PROC, "remove file %s error!\n", file->path);
                /* TODO XXX 挂起 */
            }
        }
       else {
            move_file_to_other_path(file->path, g_flow_info_cfg.dst_file_path, file->file_name);
        }
#if 0
#endif
    }

    return retn;
}

/* -------------------------------------------*/
/**
 * @brief   流量 入库实现
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_flow_data(void *data, void *arg)
{
    file_data_t* file = NULL;    
    flow_conn_t* conn = NULL;
    long buf_size = 0;
    unsigned int line = 0;
    int retn = 0;
    test_stat_t test_time;
    TEST_VARS;

    if (IS_FLOW_SPENDTIME_DEBUG_ON) {
        /* 时间测试 */
        memset(&test_time, 0, sizeof(test_stat_t));
        TEST_START(test_time);
    }

    file = (file_data_t*)data;
    conn = (flow_conn_t*)arg;
    memset(g_flow_buf, 0, FLOW_EVENT_BUF_SIZE);

    FLOW_DEBUG(DEBUG_TYPE_STORE, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);

    /*  将文件数据读到内存缓冲中 */
    buf_size = read_file_to_data(file->path, g_flow_buf);
    if (buf_size < 0) {
        fprintf(stderr, "%s null!\n", file->path); 
        goto END;
    }
    /*  得到数据行数 */
    line = get_event_num(g_flow_buf, buf_size);    
    FLOW_DEBUG(DEBUG_TYPE_STORE, "line = %d\n", line);
    if (line == 0) {
        FLOW_ERROR(DEBUG_TYPE_STORE, "***ERROR*** %s line is ZERO!!!\n", file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "***ERROR*** %s line is ZERO!!!\n", file->path);
        move_file_to_other_path(file->path, g_flow_info_cfg.wrong_path, file->file_name);
        goto END;
    }

    FLOW_DEBUG(DEBUG_TYPE_STORE, "[%s] SIZE:%d\n", file->path, buf_size);

    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_flow(conn, file, g_flow_buf, buf_size);    
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        FLOW_ERROR(DEBUG_TYPE_STORE, "***ERROR*** postgresql store error [%s] \n", file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "***ERROR*** postgresql store error [%s] \n", file->path);
        move_file_to_other_path(file->path, g_flow_info_cfg.wrong_path, file->file_name);
        goto END;
    }


    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_flow(conn, g_flow_buf, buf_size, line);

    if (IS_FLOW_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        FLOW_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buf_size, line, test_time.sec);
    }

END:
    return 0;

}


static int do_flow_store(flow_conn_t *conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    /* 遍历 flow 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_flow_info_cfg.src_file_path, conn->list);
    if (retn < 0) {
        FLOW_ERROR(DEBUG_TYPE_STORE, "travel flow dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;

    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_flow_data, (void*)conn);
    }


    /* 清空链表 */
    for (i = 0; i < list_num; ++i) {
        tmp_rm_file = list_pop(conn->list);
        free(tmp_rm_file);
    }

EXIT:
    return retn;
}

int flow_connect_db(flow_conn_t *flow_conn)
{
    int retn = 0; 

    if (g_flow_info_cfg.psql_unix == 1) {
        flow_conn->psql_conn = gpq_connectdb_host(g_flow_info_cfg.psql_domain,
                g_flow_info_cfg.psql_dbname, g_flow_info_cfg.psql_user,                           
                g_flow_info_cfg.psql_pwd);                                                         
    }
    else {
        flow_conn->psql_conn = gpq_connectdb(g_flow_info_cfg.psql_ip, g_flow_info_cfg.psql_port,                       
                g_flow_info_cfg.psql_dbname, g_flow_info_cfg.psql_user,                                   
                g_flow_info_cfg.psql_pwd);                                                                

    }
    if (flow_conn->psql_conn == NULL) {                                                                            
        FLOW_ERROR(DEBUG_TYPE_STORE, "Connect PostgreSql Server error!\n");                                       
        CA_LOG(LOG_MODULE, LOG_PROC, "Connect PostgreSql Server error!\n");                                       
        retn = -1;
        goto EXIT;                                                                                                
    }

    if (gpq_sql_cmd(flow_conn->psql_conn, "set standard_conforming_strings=on") < 0) {                             
        FLOW_ERROR(DEBUG_TYPE_STORE, "PSQL Command: set standard_conforming_strings=on ERROR");                   
        CA_LOG(LOG_MODULE, LOG_PROC,"PSQL Command: set standard_conforming_strings=on ERROR");                   
        retn = 1;
        goto EXIT;                                                                                              
    }

    if (g_flow_info_cfg.redis_unix == 1) {
        flow_conn->redis_conn = grd_connectdb_unix(g_flow_info_cfg.redis_domain, g_flow_info_cfg.redis_pwd);
    }
    else {
        flow_conn->redis_conn = grd_connectdb(g_flow_info_cfg.redis_ip, g_flow_info_cfg.redis_port, g_flow_info_cfg.redis_pwd);                   
    }

    if (flow_conn->redis_conn == NULL) {                                                                           
        FLOW_ERROR(DEBUG_TYPE_STORE, "Connect Redis Server error!\n");                                            
        CA_LOG(LOG_MODULE, LOG_PROC, "Connect Redis Server error!\n");                                            
        retn = 1;
        goto EXIT;                                                                                              
    }

EXIT:
    return retn;
}

void* flow_store_busi(void *arg)
{

    pthread_t tid;
    tid = pthread_self();

    if (g_flow_debug_cfg.store_enable == 0) {
        fprintf(stderr, "STORE_ENABLE = NO!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "STORE_ENABLE = NO!");
        goto EXIT;
    }

    flow_conn_t flow_conn;
    list_t flow_list;
    int retn = 0;
    int count = 0;


    /* ------------- 初始化配置信息 ------------------- */
    /* 1. 初始化链表 */
    list_init(&(flow_list), file_data_t);
    flow_conn.list = &flow_list;
    list_init(&(g_redis_flow_key_list), f_redis_key_t);
    flow_conn.policy_list = &g_f_policy_list;

    retn = flow_connect_db(&flow_conn);
    if (retn == -1) {
        goto EXIT; 
    }
    else if (retn == 1) {
        goto EXIT_1;
    }

    while (1) {

        do_flow_store(&flow_conn);
        usleep(40000);
        ++count;

        if(count >= NUM) {
            count = 0;
            grd_disconnect(flow_conn.redis_conn); 
            gpq_disconnect(flow_conn.psql_conn);
            retn = flow_connect_db(&flow_conn);
            if (retn == -1) {
                goto EXIT; 
            }
            else if (retn == 1) {
                goto EXIT_1;
            }
        }
    }

    /* 关闭redis链接 */
    grd_disconnect(flow_conn.redis_conn);    
EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(flow_conn.psql_conn);    

EXIT:
    pthread_exit((void*)tid);
}
