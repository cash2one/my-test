/**
 * @file store_dev_status.c
 * @brief   设备状态 存储入库流程
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2014-01-21
 */
#include "gms_store.h"


/* 设备状态事件数据缓冲*/
//char g_dev_status_buf[DEV_STATUS_EVENT_BUF_SIZE];
/* 设备状态事件批量插入命令 */
char g_dev_status_copy_cmd[400];


/* -------------------------------------------*/
/**
 * @brief  封装设备注册的redis插入数据
 */
/* -------------------------------------------*/
static RVALUES make_dev_status_values(RFIELDS flds, RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line)
{
    RVALUES values = vals;        
    RFIELDS fields = flds;
    char *pos = NULL;
    char *end = NULL;    
    char  value[VALUES_ID_SIZE];
    char field[FIELD_ID_SIZE];
    char *line_head = NULL;
    unsigned int read_line = 0;
    unsigned int max_len = 0;
    int i = 0;

    end = buf + buf_size;        
    max_len = line;

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }
			
			memset(values[i], 0, VALUES_ID_SIZE-1);
            /* devid */
            strncat(fields[i], get_value_by_col(line_head, 1, field, FIELD_ID_SIZE-1, 1), FIELD_ID_SIZE-1);

            /* c_time */
            strncat(values[i], get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* disk */
            strncat(values[i], get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* cpu */
            strncat(values[i], get_value_by_col(line_head, 4, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* mem */
            strncat(values[i], get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* state */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* runtime */
            strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* libversion */
            strncat(values[i], get_value_by_col(line_head, 8, value, VALUES_ID_SIZE-1, 1), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sysversion */
            strncat(values[i], get_value_by_col(line_head, 9, value, VALUES_ID_SIZE-1, 1), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /*data*/
			strncat(values[i], get_value_by_col(line_head, 10, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /*db*/
			strncat(values[i], get_value_by_col(line_head, 11, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
			printf("values[%d]:%s\n", i, values[i]);

            //STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "Redis HASH SET fields[%d]:%s Values[%d]:%s\n", i, fields[i], i, values[i]);

            *val_num = *val_num + 1;
            ++read_line;
            ++i;
            if (read_line == max_len) {
                /* 读完了 最大展示行数 */
                break;
            }
        }
    }

    return values;    
}

/* -------------------------------------------*/
/**
 * @brief  redis 入库
 */
/* -------------------------------------------*/
static int store_redis_dev_status(store_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line)
{
    int retn = 0;
    RFIELDS devid_values = NULL;
    RVALUES set_values = NULL;
    int val_num = 0;

    devid_values = calloc(line, FIELD_ID_SIZE);
    set_values = calloc(line, VALUES_ID_SIZE);
    if (devid_values == NULL || set_values == NULL ) {
        STORE_ERROR(DEBUG_TYPE_DEV_STATUS, "Calloc redis dev status vals buf ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "Calloc redis dev status vals buf ERROR!\n");
        goto END;
    }

    /* 批量命令行入库 */
    make_dev_status_values(devid_values, set_values, &val_num, buf, buf_size, line);
    retn = grd_hash_set_append(conn->redis_conn, KEY_EVENT_DEVMAP, devid_values, set_values, val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_DEV_STATUS, "Store redis %s Error!\n", KEY_EVENT_DEVMAP);
        CA_LOG(LOG_MODULE, LOG_PROC, "Store redis %s Error!\n", KEY_EVENT_DEVMAP);
        goto END1;
    }
    
    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_DEVMAP);
END1:
    free(devid_values);
    free(set_values);
END:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  postgresql 入库
 */
/* -------------------------------------------*/
static int store_postgresql_dev_status(store_conn_t* conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;
    char table_name[TABLE_NAME_SIZE];


    /* 根据月份选择分区表 */
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, TABLE_NAME_DEV_STATUS);
    strncat(table_name, TABLE_SUFFIX, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);

    change_dev_status_copy_cmd(g_dev_status_copy_cmd, 400, table_name);

    /*  执行批量插入命令 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_dev_status_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_DEV_STATUS, "copy %s error!\n", table_name);
        CA_LOG(LOG_MODULE, LOG_PROC, "copy %s error!\n", table_name);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_dev_status_file_path, file->file_name);
    }
    else {
        /* 命令执行成功 删除数据文件 */
        STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "copy %s succ!\n", table_name);

        CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] to table[%s] succ!\n", file->path, TABLE_NAME_DEV_STATUS);


        //if (strncmp(g_store_info_cfg.is_comm_monitor, "1", 1) != 0 && (check_file_is_localdev(file->file_name, g_store_info_cfg.dev_id) != 0)/*文件不是本机产生的*/) {
        if (check_file_is_localdev(file->file_name, g_store_info_cfg.dev_id) != 0/*文件不是本机产生的*/) {
            printf("************dev is not monitor Do not need move to comm path,  DELETE iT!!**********\n");
		    if (remove(file->path) != 0) {
			    perror("remove file");
			    STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "remove file %s error!\n", file->path);
			    CA_LOG(LOG_MODULE, LOG_PROC, "remove file %s error!\n", file->path);
			    /* TODO XXX 挂起 */
		    }
        }
        else {
            move_file_to_wrong_path(file->path, g_store_info_cfg.comm_dev_status_file_path, file->file_name);
            CA_LOG(LOG_MODULE, LOG_PROC, "Move file [%s] to path [%s]\n", file->file_name, g_store_info_cfg.comm_dev_status_file_path);
            STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "Move file [%s] to path [%s]\n", file->file_name, g_store_info_cfg.comm_dev_status_file_path);
        }
    }


    return retn;
}
/* -------------------------------------------*/
/**
 * @brief  设备状态事件 入库实现
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_dev_status_data(void *data, void *arg)
{
    file_data_t* file = NULL;    
    store_conn_t* conn = NULL;
    unsigned long buf_size = 0;
    unsigned int line = 0;
    int retn = 0;

    test_stat_t test_time;
    TEST_VARS;

    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        /* 时间测试 */
        memset(&test_time, 0, sizeof(test_stat_t));
        TEST_START(test_time);
    }

    file = (file_data_t*)data;
    conn = (store_conn_t*)arg;
    //memset(g_dev_status_buf, 0, DEV_STATUS_EVENT_BUF_SIZE);

    STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);

    /* 将文件数据读到内存缓冲中 */
    //buf_size = read_file_to_data(file->path, g_dev_status_buf);
	char *dev_malloc=NULL;
    buf_size = read_file_to_data_m(file->path, (void **)&dev_malloc);
	char g_dev_status_buf[buf_size+1];
    memset(g_dev_status_buf, 0, buf_size);
	strncpy(g_dev_status_buf,dev_malloc,buf_size);
	if(dev_malloc != NULL)
		free(dev_malloc);

    STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "[%s] SIZE:%d\n", file->path, buf_size);
    /* 得到数据行数 */
    line = get_event_num(g_dev_status_buf, buf_size);    
    if (line == 0) {
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_dev_status_file_path, file->file_name);
        goto END;    
    }

    STORE_DEBUG(DEBUG_TYPE_DEV_STATUS, "[%s] SIZE:%d\n", file->path, buf_size);

    /*  ----------------- postgres 入库 -------------------- */    
    retn = store_postgresql_dev_status(conn, file, g_dev_status_buf, buf_size);
    if (retn < 0) {
        goto END;
    }

    /*  ------------------ reids 入库 -----------------------*/
    retn = store_redis_dev_status(conn, g_dev_status_buf, buf_size, line);

    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buf_size, line, test_time.sec);
    }

END:
    return 0;
}


/* -------------------------------------------*/
/**
 * @brief  链接数据库
 * 
 * @returns   
 *          -1 postgres redis 均连接失败
 *          1  postgres 链接失败
 */
/* -------------------------------------------*/
static int do_dev_status_store(store_conn_t *conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    /* 遍历 dev_status 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_store_info_cfg.event_dev_status_file_path, conn->list);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_DEV_STATUS, "travel devstatus dir error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "travel devstatus dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;
    
    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_dev_status_data, (void*)conn);
    }


    /* 清空链表 */
    for (i = 0; i < list_num; ++i) {
        tmp_rm_file = g_list_pop(conn->list);
        free(tmp_rm_file);
    }

EXIT:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  设备状态 事件存储处理流程 
 *
 * @param arg
 *
 * @returns   
 *        
 *        该线程ID
 */
/* -------------------------------------------*/
void *store_dev_status_busi(void* arg)
{
    int retn = 0;
    int count = 0;
    pthread_t tid;
    tid = pthread_self();

    store_conn_t dev_conn;
    list_t dev_list;    

    /* --------- 初始化配置信息 ----------- */
    /* 1. 初始化链表 */
    list_init(&(dev_list), file_data_t);
    /* 2. 初始化COPY命令 */
    init_copy_cmd(THREAD_DEV_STATUS, g_dev_status_copy_cmd, 400);

    dev_conn.list = &dev_list;

    retn = store_connect_db(&dev_conn, THREAD_DEV_STATUS);
    if (retn == -1) {                                                                                             
        goto EXIT_0;                                                                                                
    }                                                                                                             
    else if (retn == 1) {                                                                                         
        goto EXIT_1;                                                                                              
    }
    retn = read_comm_interface_conf(g_store_info_cfg.comm_conf_path, g_store_info_cfg.is_comm_monitor);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_DEV_STATUS, "Parser COMM XML FILE ERROR!!!!");
        CA_LOG(LOG_MODULE, LOG_PROC, "Parser COMM XML FILE[%s] ERROR!!!!", g_store_info_cfg.comm_conf_path);
        goto EXIT_2;
    }             
    while (1) {
        /* 读取通讯配置文件 看是否是检测节点 */
        

        
        do_dev_status_store(&dev_conn);
        
        usleep(50000);
        ++count;

        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            store_disconnect_db(&dev_conn, THREAD_DEV_STATUS);
            retn = store_connect_db(&dev_conn, THREAD_DEV_STATUS);
            if (retn == -1) {                                                                                     
                goto EXIT_0;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
        }                                                                                                                       
    }
EXIT_2:
    grd_disconnect(dev_conn.redis_conn);    
EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(dev_conn.psql_conn);    
EXIT_0:
    pthread_exit((void*)tid);
}
