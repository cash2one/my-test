/**
 * @file store_abb.c
 * @brief  异常行为事件 存储处理流程 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-06
 */

#include "gms_store.h"
#include "dirwalk.h"


/* 异常行为事件数据缓冲 */
//char g_abb_buf[ABB_EVENT_BUF_SIZE];
/* 异常行为事件批量插入命令 */
char g_abb_copy_cmd[400];


/* -------------------------------------------*/
/**
 * @brief  封装插入实时呈现的数据
 */
/* -------------------------------------------*/
static RVALUES make_abb_show_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line, char *max_time)
{
    RVALUES values = vals;        
    char *pos = NULL;
    char *end = NULL;    
    char  value[VALUES_ID_SIZE];
    char *line_head = NULL;
    unsigned int read_line = 0;
    unsigned int max_len = 0;
    int i = 0;

    end = buf + buf_size;        
    max_len = (line > g_store_info_cfg.abb_show_num)? g_store_info_cfg.abb_show_num: line;

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }

            if (strncmp(max_time, get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), TIME_FORMAT_SIZE) > 0) {
                goto READ_ONE_LINE_DONE;
            }
			memset(values[i], 0, VALUES_ID_SIZE-1);
            /* rulename */
            strncat(values[i], get_value_by_col(line_head, 20, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* timedata */
            strncat(values[i], get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sip */
            strncat(values[i], get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dip */
            strncat(values[i], get_value_by_col(line_head, 4, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* proto */
            strncat(values[i], get_value_by_col(line_head, 8, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* devid */
            strncat(values[i], get_value_by_col(line_head, 1, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);

            //STORE_DEBUG(DEBUG_TYPE_ATT, "Redis Push Values[%d]:%s\n", i, values[i]);

            *val_num = *val_num + 1;
            ++i;
READ_ONE_LINE_DONE:
            ++read_line;
            if (read_line == max_len) {
                /* 读完了 最大展示行数 */
                break;
            }
        }
    }

    return values;    
}
static RVALUES make_abb_mysql_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line) 
{
    RVALUES values = vals;        
    char *pos = NULL;
    char *end = NULL;    
    char pdaddr[50];
    char psaddr[50];
    char  value[VALUES_ID_SIZE];
    char *line_head = NULL;
    unsigned int max_len = 0;
    int i = 0;
	int fd = 0;
	//fd = open(TMP_ATT_EVENT_PATH, O_RDWR | O_CREAT | O_APPEND);

    end = buf + buf_size;        

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }
#if 1 
			memset(values[i], 0, VALUES_ID_SIZE-1);
			memset(pdaddr,0,50);
			memset(psaddr,0,50);
            /* rulename */
            strncat(values[i], get_value_by_col(line_head, 20, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* etype */
            strncat(values[i], "11", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* event_exttype */
            strncat(values[i], "null", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);

            /* risk_level */
	    strncat(values[i], "null", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
	    /* timedata */
            strncat(values[i], get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* daddr */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 4, value, VALUES_ID_SIZE-1, 0),pdaddr), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* saddr */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0),psaddr), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* payload */
            strncat(values[i], get_value_by_col(line_head, 23, value, VALUES_DETAIL_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
			
			/*url*/
			strncat(values[i], "null", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
			/*data_resource*/
			strncat(values[i], "2", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
			/*gms type*/
			strncat(values[i], "5", VALUES_ID_SIZE-1);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);

			fd = open(TMP_ABB_EVENT_PATH, O_RDWR | O_CREAT | O_APPEND);
			if(write(fd, values[i], strlen(values[i])) != (int)strlen(values[i])) {
		    	printf("write error!\n");
				close(fd);
		    } else {
				write(fd, "\n", 1);
            	close(fd);
			}
            //STORE_DEBUG(DEBUG_TYPE_ATT, "Redis Push Values[%d]:%s\n", i, values[i]);
#endif
            *val_num = *val_num + 1;
            ++i;
        }
    }

    return values;    
}
static int store_redis_abb(store_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line)
{
    int retn = 0;
    RVALUES push_values = NULL;
    int val_num = 0;
    char current_max_time[TIME_FORMAT_SIZE];

#if 1
    time_t key_del_time;

    /* 0. 设置今日统计 今日top5  生命周期 */
    key_del_time = get_next_zero_time();
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_COUNT)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_COUNT, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_ABB, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
            CA_LOG(LOG_MODULE, LOG_PROC, "*** redis Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
        }
    }
#endif

    /* 1. 今日事件统计表 */
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_ABB, line);    
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ABB, "Store Redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        CA_LOG(LOG_MODULE, LOG_PROC, "Store Redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        goto END;
    }
    

    /* 3. 事件展示表 */
    push_values = calloc(g_store_info_cfg.abb_show_num, VALUES_ID_SIZE);    
    if (push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_ABB, "Calloc redis vals buf ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****Calloc redis vals buf ERROR!\n");
        goto END;
    }

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_ABB, 1, g_store_info_cfg.abb_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "ABB Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_ABB);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    /* 批量命令行入库 */
    make_abb_show_values(push_values, &val_num, buf, buf_size, line, current_max_time);
    if (val_num == 0) {
        goto END1;
    }
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_ABB, push_values, val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Store redis lpush  %s ERROR!\n", KEY_EVENT_SHOW_ABB);
        CA_LOG(LOG_MODULE, LOG_PROC, "*****Store redis lpush  %s ERROR!*******\n", KEY_EVENT_SHOW_ABB);
        goto END1;
    }

    /* 截断超过最大呈现的数据 */
    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_ABB);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ABB, "Get num of %s ERROR!\n", KEY_EVENT_SHOW_ABB);
        CA_LOG(LOG_MODULE, LOG_PROC, "***** Get num of %s ERROR!******\n", KEY_EVENT_SHOW_ABB);
        goto END1;
    }
    if ((unsigned int)retn > g_store_info_cfg.abb_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_ABB, 0, g_store_info_cfg.abb_show_num -1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_ABB, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_ABB);
            CA_LOG(LOG_MODULE, LOG_PROC, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_ABB);
            goto END1;
        }
    }
    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_SHOW_ABB);

END1:
    free(push_values);
END:
    return retn;
}


static int store_postgresql_abb(store_conn_t* conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;

    /* . 执行批量插入命令 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, TABLE_NAME_ABB, g_abb_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ABB, "copy %s error!\n", TABLE_NAME_ABB);
        CA_LOG(LOG_MODULE, LOG_PROC, "copy %s error!\n", TABLE_NAME_ABB);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_abb_file_path, file->file_name);
        goto END;
    }
    else {
        /*. 命令执行成功 删除数据文件 */
        STORE_DEBUG(DEBUG_TYPE_ABB, "copy %s succ!\n", TABLE_NAME_ABB);
        CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] to table[%s] succ!\n", file->path, TABLE_NAME_ABB);
        if (remove(file->path) != 0) {
            perror("remove file");
            /* 删除失败 */
            STORE_ERROR(DEBUG_TYPE_ABB, "remove file %s error!\n", file->path);
            CA_LOG(LOG_MODULE, LOG_PROC, "remove file %s error!\n", file->path);
                    
            /* TODO XXX 挂起 */
        }
    }
END:
    return retn;
}
/*--------------------------------------------*/
/**
 *@brief mysql入库
 */
/*--------------------------------------------*/

static int store_mysql_abb(store_conn_t* conn, char *buf, unsigned long buf_size, unsigned int line)
{
	int retn = 0;
	RVALUES values = NULL;
	char sql_cmd[1024] = {0};
	int val_num = 0;
	pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
	values = calloc(line, VALUES_ID_SIZE);
	make_abb_mysql_values(values, &val_num, buf, buf_size, line);
	if ( mysql_set_character_set( conn->mysql_conn, "utf8" ) ) { 
            fprintf ( stderr , "error:%s/n" , mysql_error( conn->mysql_conn) ) ; 
        } 
	sprintf(sql_cmd, "load data local infile \'%s\' into table %s character set utf8 fields terminated by \'||\'(ename,etype,event_exttype,risk_level,event_time,daddr,saddr,sport,dport,event_detail,url,data_resource,gmstype);", TMP_ABB_EVENT_PATH, g_store_info_cfg.mysql_table_name);
	//printf("sql_cmd:%s\n", sql_cmd);
	pthread_mutex_lock(&mutex2);
	retn = mysql_query(conn->mysql_conn, sql_cmd);
	if (retn != 0) {
        	CA_LOG(LOG_MODULE, LOG_PROC, "load error! %d:%s\n", mysql_errno(conn->mysql_conn), mysql_error(conn->mysql_conn));
    		retn = -1;
		goto END;
	} 
END:
	pthread_mutex_unlock(&mutex2);
	free(values);
	remove(TMP_ABB_EVENT_PATH);
	return retn;
}
/* -------------------------------------------*/
/**
 * @brief  异常行为事件 入库实现
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_abb_data(void *data, void *arg)
{
    int flag = 0;
    int buffer_offset = 0;
    int onedata_buffer_length = 0;
    file_data_t* file = NULL;    
    store_conn_t* conn = NULL;
    unsigned int sum_line = 0;
//    unsigned long buf_size = 0;
    unsigned long file_size = 0;
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

    //memset(g_abb_buf, 0, ABB_EVENT_BUF_SIZE);

    STORE_DEBUG(DEBUG_TYPE_ABB, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);


//	char *abb_malloc=NULL;
    /*  将文件数据读到内存缓冲中 */
/*    buf_size = read_file_to_data_m(file->path, (void **)&abb_malloc);
	char g_abb_buf[buf_size+1];
    memset(g_abb_buf, 0, buf_size);

	strncpy(g_abb_buf,abb_malloc,buf_size);
	if (NULL != abb_malloc)
		free(abb_malloc);*/
    /*  得到数据行数 */
//    line = get_event_num(g_abb_buf, buf_size);    

    
    FILE* readstream = fopen(file->path,"r+");
    if(readstream == NULL){
        STORE_ERROR(DEBUG_TYPE_ATT, "\nstore abb fopen File[%s] error!\n",file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "****abb inject_abb_data fopen ERROR!!****");
        return -1;       
    }

    fseek(readstream,0,SEEK_END);
    file_size = ftell(readstream);
    fseek(readstream,0,SEEK_SET);

    char* onedata_buffer = (char *)malloc(10240);
    char* tendata_buffer = (char *)malloc(102400);
    if(onedata_buffer == NULL || tendata_buffer == NULL){
        STORE_ERROR(DEBUG_TYPE_ATT, "malloc buffer error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****abb inject_abb_data malloc ERROR!!****");
    }
    memset(onedata_buffer,0,10240);
    memset(tendata_buffer,0,102400);

//    STORE_DEBUG(DEBUG_TYPE_ABB, "[%s] SIZE:%d\n", file->path, buffer_offset);
    while(fgets(onedata_buffer,10240,readstream)!=NULL){
        sum_line++;
        flag++;    
        onedata_buffer_length = strlen(onedata_buffer);
        memcpy(tendata_buffer+buffer_offset,onedata_buffer,onedata_buffer_length);
        buffer_offset += onedata_buffer_length;
        if(flag >= 10){
         /*----------------mysql 入库--------------------*/
        if(g_store_info_cfg.mysql_switch == 1) {
            if(g_store_info_cfg.mysql_apt == 1) {
                store_mysql_abb(conn, tendata_buffer, buffer_offset, flag);
            }
        }
        /* --------------- postgres 入库 ---------------*/    
        retn = store_postgresql_abb(conn, file, tendata_buffer, buffer_offset);
        if (retn < 0) {
            /* postgres 入库失败 redis 无需入库 */
            goto END;
        }

        /* ----------------- reids 入库 ------------------  */
        retn = store_redis_abb(conn, tendata_buffer, buffer_offset, flag);


        memset(onedata_buffer,0,10240);
        memset(tendata_buffer,0,102400);

        flag = 0;
        buffer_offset = 0;
        }
    }
    if (sum_line == 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "file :%s, line is 0\n", file->path);
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_abb_file_path, file->file_name);
        goto END;
    }
	if (flag !=0 )
	{
    //剩下的不超过10条的buffer中的数据也需要入库
    /*----------------mysql 入库--------------------*/
    if(g_store_info_cfg.mysql_switch == 1) {
            if(g_store_info_cfg.mysql_apt == 1) {
                store_mysql_abb(conn, tendata_buffer, buffer_offset, flag);
            }
        }
    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_abb(conn, file, tendata_buffer, buffer_offset);
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        goto END;
    }

    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_abb(conn, tendata_buffer, buffer_offset, flag);
	}
/*    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buffer_offset, sum_line, test_time.sec);
    }*/
    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===>read & store File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, file_size, sum_line, test_time.sec);
    }
    fclose(readstream);
    if (remove(file->path) != 0) {
        perror("remove file");
        /* 删除失败 */
        STORE_ERROR(DEBUG_TYPE_3RD, "remove file %s error!\n", file->path);                
        /* TODO XXX 挂起 */
    }

END:
    free(onedata_buffer);
    free(tendata_buffer);
    return 0;
}



static int do_abb_store(store_conn_t* conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    /* 遍历 abb 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_store_info_cfg.event_abb_file_path, conn->list);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ABB, "travel abb dir error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "travel abb dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;
    
    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_abb_data, (void*)conn);
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
 * @brief   异常行为事件 
 *
 * @param arg
 *
 * @returns   
 *        
 *        该线程ID
 */
/* -------------------------------------------*/
void *store_abb_busi(void* arg)
{
    int retn = 0;
    int count = 0;
    pthread_t tid;
    tid = pthread_self();

    store_conn_t abb_conn;
    list_t abb_list;

    /* ------------- 初始化配置信息 ------------------- */
    /* 1. 初始化链表 */
    list_init(&(abb_list), file_data_t);
    abb_conn.list = &abb_list;
    /* 2. 初始化COPY命令 */
    init_copy_cmd(THREAD_ABB, g_abb_copy_cmd, 400);
    
    retn = store_connect_db(&abb_conn, THREAD_ABB);
    if (retn == -1) {                                                                                             
        goto EXIT_0;                                                                                                
    }                                                                                                             
    else if (retn == -3) {                                                                                         
        goto EXIT_3;                                                                                              
    }          
    else if (retn == 1) {                                                                                         
        goto EXIT_1;                                                                                              
    }       
    while (1) {
        /* 是否需要删除redis数据 */

        do_abb_store(&abb_conn);

        usleep(50000);
        ++count;                                                                                                  
                                                                                                                  
        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            store_disconnect_db(&abb_conn, THREAD_ABB);
            retn = store_connect_db(&abb_conn, THREAD_ABB);
            if (retn == -1) {                                                                                     
                goto EXIT_0;                                                                                        
            }                                                                                                     
            if (retn == -3) {                                                                                     
                goto EXIT_3;                                                                                        
            }                                                                                                     
            else if (retn == 1) {                                                                                 
                goto EXIT_1;                                                                                      
            }                                                                                                     
        }                    
    }


	if(g_store_info_cfg.mysql_switch==1)
{
    mysql_close(abb_conn.mysql_conn);
	mysql_thread_end();
}
EXIT_3:
    /* 关闭redis链接 */
    grd_disconnect(abb_conn.redis_conn);    
EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(abb_conn.psql_conn);    
EXIT_0:
    pthread_exit((void*)tid);
}
