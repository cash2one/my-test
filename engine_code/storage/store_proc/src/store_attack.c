/**
 * @file store_attack.c
 * @brief  MTX引擎回传特征攻击事件 入库处理流程
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-06
 */

#include "gms_store.h"
#include "dirwalk.h"

/* 攻击事件数据缓冲 */
char g_att_buf[ATTACK_EVENT_BUF_SIZE];
/* 攻击事件批量插入命令 */
char g_att_copy_cmd[400];

/* -------------------------------------------*/
/**
 * @brief  在链表中打印文件路径
 *
 * @param data
 * @param arg
 *
 * @returns   
 */
/* -------------------------------------------*/
int show_file_data(void *data, void *arg)
{
    file_data_t* file = NULL;

    file = (file_data_t*)data;

    printf("path: %s\n", file->path);    

    return 0;
}

#if 0
static int get_max_sequence_attack(store_conn_t* conn, unsigned long sequence_max) 
{
    int retn = 0;

    return retn;
}
#endif

/* -------------------------------------------*/
/**
 * @brief  封装TOP5的数据
 */
/* -------------------------------------------*/
static RVALUES make_att_top5_values(RVALUES vals, char *buf, unsigned long buf_size)
{
    RVALUES values = vals;        
    char *pos = NULL;
    char *end = NULL;    
    char  value[VALUES_ID_SIZE];
    char *line_head = NULL;
    int i = 0;

    end = buf + buf_size;        

    /*  从尾部遍历 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            line_head = pos+1;

            /* saddr */
            strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
	    //printf("daddr:%s\n", values[i]);
            //STORE_DEBUG(DEBUG_TYPE_ATT, "Redis top5  daddr[%d]:%s\n", i, values[i]);
            ++i;
        }
    }

    return values;    
}

/* -------------------------------------------*/
/**
 * @brief  封装插入实时呈现的数据
 */
/* -------------------------------------------*/
static RVALUES make_att_show_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line, char *max_time)
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
    max_len = (line > g_store_info_cfg.attack_show_num)? g_store_info_cfg.attack_show_num: line;

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }


            if (strncmp(max_time, get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), TIME_FORMAT_SIZE) > 0) {
                goto READ_ONE_LINE_DONE;
            }
			
			memset(values[i], 0, VALUES_ID_SIZE-1);
            /* begin_time */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	   // printf("values[%d]:%s\n", i, values[i]);
            /* end_time */
            strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	   // printf("values[%d]:%s\n", i, values[i]);
            /* devid */
            strncat(values[i], get_value_by_col(line_head, 1, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	   // printf("values[%d]:%s\n", i, values[i]);
            /* ename */
            strncat(values[i], get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	   // printf("values[%d]:%s\n", i, values[i]);
            /* daddr */
            strncat(values[i], get_value_by_col(line_head, 12, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    //printf("values[%d]:%s\n", i, values[i]);
            /* saddr */
            strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    //printf("values[%d]:%s\n", i, values[i]);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 14, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    //printf("values[%d]:%s\n", i, values[i]);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 15, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	   // printf("values[%d]:%s\n", i, values[i]);
            /* risk_level */
            strncat(values[i], get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	  //  printf("values[%d]:%s\n", i, values[i]);
            /* eventdetail */
            strncat(values[i], get_value_by_col(line_head, 16, value, VALUES_DETAIL_SIZE-1, 0), VALUES_ID_SIZE-1);
	   // printf("values[%d]:%s\n", i, values[i]);

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

static RVALUES make_att_mysql_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line) 
{
    RVALUES values = vals;        
    char *pos = NULL;
    char *end = NULL;    
    char psaddr[50];
    char pdaddr[50];
    char  value[VALUES_ID_SIZE];
    char *line_head = NULL;
    unsigned int max_len = 0;
    int i = 0;
	int fd = 0;
	//fd = open(TMP_ATT_EVENT_PATH, O_RDWR | O_CREAT | O_APPEND);

    end = buf + buf_size;        
    max_len = (line > g_store_info_cfg.attack_show_num)? g_store_info_cfg.attack_show_num: line;

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
			memset(pdaddr, 0, 50);
			memset(psaddr, 0, 50);

            /* ename */
            strncat(values[i], get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* etype */
            strncat(values[i], "11", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* event_exttype */
            strncat(values[i], get_value_by_col(line_head, 11, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);

            /* risk_level */
            if (strcmp(get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), "5") != 0) {
            	strncat(values[i], "2", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    } else {
            	strncat(values[i], "1", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    }
	    //printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
	    /* begin_time */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* daddr */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 12, value, VALUES_ID_SIZE-1, 0),pdaddr), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* saddr */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0),psaddr), VALUES_ID_SIZE-1);
            //strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 14, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 15, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* eventdetail */
            strncat(values[i], get_value_by_col(line_head, 16, value, VALUES_DETAIL_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
			
			/*url*/
			strncat(values[i], "null", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
			/*data_resource*/
			strncat(values[i], "0", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
			/*gms type*/
			strncat(values[i], "3", VALUES_ID_SIZE-1);

		//printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);

			fd = open(TMP_ATT_EVENT_PATH, O_RDWR | O_CREAT | O_APPEND);
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


/* -------------------------------------------*/
/**
 * @brief  redis 入库
 */
/* -------------------------------------------*/
static int store_redis_attack(store_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line)
{
    int retn = 0;
    RVALUES top5_values = NULL;
    RVALUES push_values = NULL;
    int val_num = 0;
    char current_max_time[TIME_FORMAT_SIZE];

#if 1
    time_t key_del_time;
	time_t week_top10_del;

    /* 0. 设置今日统计 今日top5  生命周期 */
    key_del_time = get_next_zero_time();
#if 0
	week_top10_del = get_next_week_zero_time();
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_ATTACK_WEEK_TOP10)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_ATTACK_WEEK_TOP10, week_top10_del);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_ATT, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_WEEK_TOP10);
            CA_LOG(LOG_MODULE, LOG_PROC, "*** redis Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_WEEK_TOP10);
        }
    }
#endif
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_COUNT)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_COUNT, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_ATT, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
            CA_LOG(LOG_MODULE, LOG_PROC, "*** redis Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
        }
    }
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_ATTACK_TOP5)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_ATTACK_TOP5, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_ATT, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
            CA_LOG(LOG_MODULE, LOG_PROC, "*** redis Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
        }
    }
#endif

    /* 1. 今日事件统计表 */
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_ATTACK, line);    
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Store Redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        CA_LOG(LOG_MODULE, LOG_PROC, "*** redis Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
        goto END;
    }
    
    /* 2. 今日被攻击top5 统计表 */
    top5_values = calloc(line, VALUES_ID_SIZE);
    if (top5_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Calloc redis top5 vals buf ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "*** Calloc redis top5 vals buf ERROR***\n");
        goto END;
    }
    /* 批量命令行入库 */
    make_att_top5_values(top5_values, buf, buf_size);    
    retn = grd_zset_increment_append(conn->redis_conn, KEY_EVENT_ATTACK_TOP5, top5_values, line);        
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Store redis top5 %s ERROR!\n", KEY_EVENT_ATTACK_TOP5);
        CA_LOG(LOG_MODULE, LOG_PROC, "****Store redis top5 %s ERROR!****\n", KEY_EVENT_ATTACK_TOP5);
        goto END1;
    }
#if 0
    retn = grd_zset_increment_append(conn->redis_conn, KEY_EVENT_ATTACK_WEEK_TOP10, top5_values, line);        
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Store redis top5 %s ERROR!\n", KEY_EVENT_ATTACK_WEEK_TOP10);
        CA_LOG(LOG_MODULE, LOG_PROC, "****Store redis top5 %s ERROR!****\n", KEY_EVENT_ATTACK_WEEK_TOP10);
        goto END1;
    }
#endif

    /* 3. 事件展示表 */
    push_values = calloc(g_store_info_cfg.attack_show_num, VALUES_ID_SIZE);    
    if (push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Calloc redis vals buf ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****Calloc redis vals buf ERROR!\n");
        goto END1;
    }

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_ATTACK, 0, g_store_info_cfg.attack_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "ATT Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_ATTACK);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    /* 批量命令行入库 */
    make_att_show_values(push_values, &val_num, buf, buf_size, line, current_max_time);
    if (val_num == 0) {
        goto END2;
    }
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_ATTACK, push_values, val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Store redis lpush  %s ERROR!\n", KEY_EVENT_SHOW_ATTACK);
        CA_LOG(LOG_MODULE, LOG_PROC, "*****Store redis lpush  %s ERROR!*******\n", KEY_EVENT_SHOW_ATTACK);
        goto END2;
    }

    /* 截断超过最大呈现的数据 */
    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_ATTACK);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Get num of %s ERROR!\n", KEY_EVENT_SHOW_ATTACK);
        CA_LOG(LOG_MODULE, LOG_PROC, "***** Get num of %s ERROR!******\n", KEY_EVENT_SHOW_ATTACK);
        goto END2;
    }
    if ((unsigned int)retn > g_store_info_cfg.attack_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_ATTACK, 0, g_store_info_cfg.attack_show_num -1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_ATT, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_ATTACK);
            CA_LOG(LOG_MODULE, LOG_PROC, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_ATTACK);
            goto END2;
        }
    }
    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_SHOW_ATTACK);

END2:
    free(push_values);
END1:
    free(top5_values);
END:
    return retn;
}


/* -------------------------------------------*/
/**
 * @brief  postgresql 入库
 */
/* -------------------------------------------*/
static int store_postgresql_attack(store_conn_t* conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;
    char table_name[TABLE_NAME_SIZE];

    /* 根据月份选择t_event_attackhis_p分区表 */
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, TABLE_NAME_ATTACK_HIS);
    strncat(table_name, TABLE_SUFFIX, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);
//    printf("table_name : %s\n", table_name);

    change_att_copy_cmd(g_att_copy_cmd, 400, table_name);

    /* 执行批量插入命令到t_event_attackhis_p表 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_att_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "copy %s to %s error!\n", file->file_name, table_name);
        CA_LOG(LOG_MODULE, LOG_PROC, "*****copy %s to %s error!******\n", file->file_name, table_name);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_attack_file_path, file->file_name);
        goto END;
    }

    /*根据月，天选择t_event_attackquery_m_d分区表*/
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, TABLE_NAME_ATTACK_QUERY);
    strncat(table_name, TABLE_SUFFIX_MONTH, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);
    strncat(table_name, TABLE_SUFFIX_DAY, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->day);

    change_att_copy_cmd(g_att_copy_cmd, 400, table_name);
    /* 执行批量插入命令到t_event_attackquery_m_d表 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_att_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "copy %s to %s error!\n", file->file_name, table_name);
        CA_LOG(LOG_MODULE, LOG_PROC, "*****copy %s to %s error!******\n", file->file_name, table_name);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_attack_file_path, file->file_name);
        goto END;
    }
#if 0
    /*插入t_event_attacktmp表*/
    change_att_copy_cmd(g_att_copy_cmd, 400, TABLE_NAME_ATTACK_TMP);

    /*  执行批量插入命令 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, TABLE_NAME_ATTACK_TMP, g_att_copy_cmd, buf, buf_size);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "*****copy %s to %s error!******\n", file->file_name, TABLE_NAME_ATTACK_TMP);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_attack_file_path, file->file_name);
    }
    else {
        /* 命令执行成功 删除数据文件 */
        STORE_DEBUG(DEBUG_TYPE_ATT, "copy %s succ!\n", TABLE_NAME_ATTACK_TMP);
        CA_LOG(LOG_MODULE, LOG_PROC, "Posgresql Database--> copy file[%s] -> table[%s] succ!\n", file->file_name, TABLE_NAME_ATTACK_TMP);
 /*       if (remove(file->path) != 0) {
            perror("remove file");
            // 删除失败 
            STORE_ERROR(DEBUG_TYPE_ATT, "remove file %s error!\n", file->path);
            CA_LOG(LOG_MODULE, LOG_PROC, "*****remove file %s ERROR!!******\n", file->path);
                    
            //TODO XXX 挂起 
        }*/
    }
#endif
END:
    return retn;
}


/*--------------------------------------------*/
/**
 *@brief mysql入库
 */
/*--------------------------------------------*/

static int store_mysql_attack(store_conn_t* conn, char *buf, unsigned long buf_size, unsigned int line)
{
	int ret = 0;
	RVALUES values = NULL;
	char sql_cmd[1024] = {0};
	int val_num = 0;
	pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
	values = calloc(line, VALUES_ID_SIZE);
	make_att_mysql_values(values, &val_num, buf, buf_size, line);
	if ( mysql_set_character_set( conn->mysql_conn, "utf8" ) ) { 
            fprintf ( stderr , "错误, %s/n" , mysql_error( conn->mysql_conn) ) ; 
        } 
	sprintf(sql_cmd, "load data local infile \'%s\' into table %s character set utf8 fields terminated by \'||\'(ename,etype,event_exttype,risk_level,event_time,daddr,saddr,sport,dport,event_detail,url,data_resource,gmstype);", TMP_ATT_EVENT_PATH, g_store_info_cfg.mysql_table_name);
	printf("sql_cmd:%s\n", sql_cmd);
#if 1
	pthread_mutex_lock(&mutex2);
	ret = mysql_query(conn->mysql_conn, sql_cmd);
	if (ret != 0) {
        	CA_LOG(LOG_MODULE, LOG_PROC, "load error! %d:%s\n", mysql_errno(conn->mysql_conn), mysql_error(conn->mysql_conn));
            printf("mysql load error!%d:%s\n",mysql_errno(conn->mysql_conn), mysql_error(conn->mysql_conn));
    		ret = -1;
		goto END;
	} 
#endif
END:
	pthread_mutex_unlock(&mutex2);
	free(values);
	remove(TMP_ATT_EVENT_PATH);
	return ret;
}


/* -------------------------------------------*/
/**
 * @brief  特征攻击事件 入库实现
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_attack_data(void *data, void *arg)
{
    int flag = 0;
    int buffer_offset = 0;
    int onedata_buffer_length = 0;
    file_data_t* file = NULL;    
    store_conn_t* conn = NULL;
//    unsigned long buf_size = 0;
    unsigned long file_size = 0;
    unsigned int sum_line = 0;
    int retn = 0;
	char email_path[100] = EMAIL_PATH;
	char mtx[] = "mtx";
    test_stat_t test_time;
    TEST_VARS;
#if 0
    unsigned long sequence_max = 0;
#endif

    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        /* 时间测试 */
        memset(&test_time, 0, sizeof(test_stat_t));
        TEST_START(test_time);
    }

    file = (file_data_t*)data;
    conn = (store_conn_t*)arg;
    //memset(g_att_buf, 0, ATTACK_EVENT_BUF_SIZE);

    STORE_DEBUG(DEBUG_TYPE_ATT, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);

	
    /*  将文件数据读到内存缓冲中 */
//	char *att_malloc=NULL;
    /*
	buf_size = read_file_to_data_m(file->path, (void **)&att_malloc);
	char g_att_buf[buf_size+1];
    memset(g_att_buf, 0, buf_size);
	strncpy(g_att_buf,att_malloc,buf_size);
	if (NULL != att_malloc)
		free(att_malloc);
    //  得到数据行数
    line = get_event_num(g_att_buf, buf_size);
    */
    //为防止内存占用过大，一次读取10条数据到内存缓冲中
    FILE* readstream = fopen(file->path,"r+");
    if(readstream == NULL){
        STORE_ERROR(DEBUG_TYPE_ATT, "\nstore att fopen File[%s] error!\n",file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "****att inject_attack_data fopen File[%s] ERROR!!****",file->path);
        return -1;       
    }
    fseek(readstream,0,SEEK_END);
    file_size = ftell(readstream);
    fseek(readstream,0,SEEK_SET);
    //rewind(readstream);
    char* onedata_buffer = (char *)malloc(10240);
    char* tendata_buffer = (char *)malloc(102400);
    if(onedata_buffer == NULL || tendata_buffer == NULL){
        STORE_ERROR(DEBUG_TYPE_ATT, "malloc buffer error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****att inject_attack_data malloc ERROR!!****");
    }
    memset(onedata_buffer,0,10240);
    memset(tendata_buffer,0,102400);

    
    while(fgets(onedata_buffer,10240,readstream)!=NULL){
        sum_line++;
        flag++;    
        onedata_buffer_length = strlen(onedata_buffer);
        memcpy(tendata_buffer+buffer_offset,onedata_buffer,onedata_buffer_length);
        buffer_offset += onedata_buffer_length;
        //够10条数据就入库一次
        if(flag >= 10){
			/*----------------mysql 入库--------------------*/
            if(g_store_info_cfg.mysql_switch == 1) {
	           if(g_store_info_cfg.mysql_mtx == 1) {
		          store_mysql_attack(conn, tendata_buffer, buffer_offset, flag);
	           }
            }
            /* --------------- postgres 入库 ---------------*/    
            retn = store_postgresql_attack(conn, file, tendata_buffer, buffer_offset);    
            if (retn < 0) {
                /* postgres 入库失败 redis 无需入库 */
                goto END;
            }


            /* ----------------- reids 入库 ------------------  */
            retn = store_redis_attack(conn, tendata_buffer, buffer_offset, flag);

/*            if (IS_STORE_SPENDTIME_DEBUG_ON) {
                TEST_END(test_time);
                STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, file_size, sum_line, test_time.sec);
            }*/
            memset(onedata_buffer,0,10240);
            memset(tendata_buffer,0,102400);
            flag = 0;
            buffer_offset = 0;
        }
    }
    if (sum_line == 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "file :%s, sum_line is 0\n", file->path);
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_attack_file_path, file->file_name);
        goto END;
    }
	if (flag != 0)
	{
    //剩下的不超过10条的buffer中的数据也需要入库
		/*----------------mysql 入库--------------------*/
    if(g_store_info_cfg.mysql_switch == 1) {
        if(g_store_info_cfg.mysql_mtx == 1) {
            store_mysql_attack(conn, tendata_buffer, buffer_offset, flag);
        }
    }
     /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_attack(conn, file, tendata_buffer, buffer_offset);    
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        goto END;
    }


    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_attack(conn, tendata_buffer, buffer_offset, flag);

//    free(onedata_buffer);
//    free(tendata_buffer);
	}
    STORE_DEBUG(DEBUG_TYPE_ATT, "[%s] SIZE:%d\n", file->path, buffer_offset);
    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===>read & store File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, file_size, sum_line, test_time.sec);
    }
	strcat(email_path,mtx);
	strcat(email_path,file->file_name);//move file to the email path
	

#if 0
    /* 获得当前最大主键sequence */
    retn = get_max_sequence_attack(conn, &sequence_max);
    if (retn < 0) {
        goto END;
    }
#endif

	if (rename(file->path,email_path) != 0) {
        perror("rename file");
        /* 删除失败 */
        STORE_ERROR(DEBUG_TYPE_3RD, "rename file %s error!remove file\n", file->path);  
		remove(file->path);
        /* TODO XXX 挂起 */
    }

END:

    fclose(readstream);

    free(onedata_buffer);
    free(tendata_buffer);
    return 0;
}



static int do_attack_store(store_conn_t* conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    /* 遍历 attack 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_store_info_cfg.event_attack_file_path, conn->list);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "travel att dir error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****travel att dir ERROR!!****");
        goto EXIT;
    }
    list_num = conn->list->node_num;
    
    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_attack_data, (void*)conn);
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
 * @brief  mtx 引起 产生 特征事件（攻击）
 *            存储处理流程
 *
 * @param arg
 *
 * @returns   
 *        
 *        该线程ID
 */
/* -------------------------------------------*/
void *store_attack_busi(void* arg)
{
    int retn = 0;                                                                                                 
    int count = 0;
    pthread_t tid;
    tid = pthread_self();

    store_conn_t attack_conn;
    list_t attack_list;
    

    /*-------------  初始化配置信息 ------------- */
    /* 1. 初始化链表 */
    list_init(&(attack_list), file_data_t);
    attack_conn.list = &attack_list;
    /* 2. 初始化COPY命令 */
    init_copy_cmd(THREAD_ATTACK, g_att_copy_cmd, 400);

    retn = store_connect_db(&attack_conn, THREAD_ATTACK); 
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

        do_attack_store(&attack_conn);

        usleep(50000);

        ++count;

        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            store_disconnect_db(&attack_conn, THREAD_ATTACK);
            retn = store_connect_db(&attack_conn, THREAD_ATTACK); 
            if (retn == -1) {   
                goto EXIT_0;
            } 
			else if(retn == -3){
				goto EXIT_3;
			}			
            else if (retn == 1) {                                               
                goto EXIT_1;    
            }                                                                                                     
        }       
    }


    /* 关闭mysql链接 */
    if(g_store_info_cfg.mysql_switch==1)
    {
		    mysql_close(attack_conn.mysql_conn);
		    mysql_thread_end();
    }
EXIT_3:
    /* 关闭redis链接 */
    grd_disconnect(attack_conn.redis_conn);    

EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(attack_conn.psql_conn);    

EXIT_0:
    pthread_exit((void*)tid);
}


#if 0
void *func(void *arg)
{
    pid_t pid;
    pthread_t tid;
    pid = getpid();
    tid = pthread_self();
    printf("%s pid %u tid %u(0x%x)\n", arg, (unsigned int)pid, (unsigned int)tid, (unsigned int)tid);

    pthread_exit((void*)tid);
}
#endif

