/**
 * @file store_black.c
 * @brief  黑名单DNS URL 入库处理流程
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-22
 */

#include "gms_store.h"
#include "dirwalk.h"


/* 黑名单事件数据缓冲 */
//char g_black_buf[BLACK_EVENT_BUF_SIZE];
/* DNS事件批量插入命令 */
char g_dns_copy_cmd[400];
/* URL事件批量插入命令 */
char g_url_copy_cmd[400];

/* -------------------------------------------*/
/**
 * @brief  封装插DNS入实时呈现的数据
 */
/* -------------------------------------------*/
static RVALUES make_bdns_show_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line, char *max_time)
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
    max_len = (line > g_store_info_cfg.dns_show_num)? g_store_info_cfg.dns_show_num: line;

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }

            if (strncmp(max_time, get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), TIME_FORMAT_SIZE) > 0) {
                goto READ_ONE_LINE_DONE;
            }

#if 0
            /* devid */
            strncat(values[i], get_value_by_col(line_head, 1, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sip */
            strncat(values[i], get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dip */
            strncat(values[i], get_value_by_col(line_head, 4, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dnsip */
            strncat(values[i], get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dns */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* ctime */
            strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
#endif

            //STORE_DEBUG(DEBUG_TYPE_BLACK, "Redis Push Values[%d]:%s\n", i, values[i]);

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
static RVALUES make_url_top5_values(RVALUES vals, char *buf, unsigned long buf_size)
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

            /* daddr */
            strncat(values[i], get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
        printf("daddr:%s\n", values[i]);
            //STORE_DEBUG(DEBUG_TYPE_ATT, "Redis top5  daddr[%d]:%s\n", i, values[i]);
            ++i;
        }
    }

    return values;    
}
/* -------------------------------------------*/
/**
 * @brief  封装URL插入实时呈现的数据
 */
/* -------------------------------------------*/
static RVALUES make_burl_show_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line, char *max_time)
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
    max_len = (line > g_store_info_cfg.url_show_num)? g_store_info_cfg.url_show_num: line;

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {

            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }

            if (strncmp(max_time, get_value_by_col(line_head, 9, value, VALUES_ID_SIZE-1, 0), TIME_FORMAT_SIZE) > 0) {
                printf("max_time %s > %s\n", max_time, get_value_by_col(line_head, 9, value, VALUES_ID_SIZE-1, 0));
                goto READ_ONE_LINE_DONE;
            }

            memset(values[i], 0, VALUES_ID_SIZE);
            memset(value, 0, VALUES_ID_SIZE);
            /* devid */
            strncat(values[i], get_value_by_col(line_head, 1, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sip */
            strncat(values[i], get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dip */
            strncat(values[i], get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 10, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 11, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* url */
            strncat(values[i], get_value_by_col(line_head, 4, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* method */
            strncat(values[i], get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* host */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* user_agent */
            strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* param */
            strncat(values[i], get_value_by_col(line_head, 8, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* timenow */
            strncat(values[i], get_value_by_col(line_head, 9, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* referer */
            strncat(values[i], get_value_by_col(line_head, 14, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);

            STORE_DEBUG(DEBUG_TYPE_BLACK, "Redis Push Values[%d]:%s\n", i, values[i]);

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
static RVALUES make_burl_mysql_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line) 
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
			memset(pdaddr,0,50);
			memset(psaddr,0,50);
            /* ename */
            strncat(values[i], "null", VALUES_ID_SIZE-1);
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
            if (strcmp(get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), "5") != 0) {
            	strncat(values[i], "2", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    } else {
            	strncat(values[i], "1", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    }
	    printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
	    /* timenow */
            strncat(values[i], get_value_by_col(line_head, 9, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* dip */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 3, value, VALUES_ID_SIZE-1, 0),pdaddr), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* sip */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0),psaddr), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 10, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 11, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
            /* eventdetail */
            strncat(values[i], "null", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);
			
	   /*url*/
	    strncat(values[i], get_value_by_col(line_head, 4, value, VALUES_DETAIL_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	   /*data_resource*/
	   strncat(values[i], "3", VALUES_ID_SIZE-1);
	   printf("len: %d values[:%d]:%s\n", strlen(values[i]), i, values[i]);

	   fd = open(TMP_BURL_EVENT_PATH, O_RDWR | O_CREAT | O_APPEND);
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
 * @brief  URL redis 入库
 */
/* -------------------------------------------*/
static int store_redis_black_url(store_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line)
{
    int retn = 0;
    RVALUES top5_values = NULL;
    RVALUES push_values = NULL;
    int val_num = 0;
    char current_max_time[TIME_FORMAT_SIZE];

#if 1
    time_t key_del_time;

    /* 0. 设置今日统计  top5 生命周期 */
    key_del_time = get_next_zero_time();
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_COUNT)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_COUNT, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_BLACK, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
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
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_BURL, line);    
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Store Redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        CA_LOG(LOG_MODULE, LOG_PROC, "Store Redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        goto END;
    }
    /* 2. 今日被攻击top5 统计表 */
    top5_values = calloc(line, VALUES_ID_SIZE);
    if (top5_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Calloc redis top5 vals buf ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "*** Calloc redis top5 vals buf ERROR***\n");
        goto END1;
    }
    /* 批量命令行入库 */
    make_url_top5_values(top5_values, buf, buf_size);    
    retn = grd_zset_increment_append(conn->redis_conn, KEY_EVENT_ATTACK_TOP5, top5_values, line);        
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "Store redis top5 %s ERROR!\n", KEY_EVENT_ATTACK_TOP5);
        CA_LOG(LOG_MODULE, LOG_PROC, "****Store redis top5 %s ERROR!****\n", KEY_EVENT_ATTACK_TOP5);
        goto END1;
    }
    /* 3. 事件展示表 */
    push_values = calloc(g_store_info_cfg.url_show_num, VALUES_ID_SIZE);    
    if (push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Calloc redis vals buf ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****Calloc redis vals buf ERROR!\n");
        goto END1;
    }

    memset(push_values, 0, g_store_info_cfg.url_show_num*VALUES_ID_SIZE);

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_BLACK_URL, 10, g_store_info_cfg.url_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "SURL Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_BLACK_URL);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    /* 批量命令行入库 */
    make_burl_show_values(push_values, &val_num, buf, buf_size, line, current_max_time);
    if (val_num == 0) {
        goto END1;
    }
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_BLACK_URL, push_values, val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Store redis lpush  %s ERROR!\n", KEY_EVENT_SHOW_BLACK_URL);
        CA_LOG(LOG_MODULE, LOG_PROC, "*****Store redis lpush  %s ERROR!*******\n", KEY_EVENT_SHOW_BLACK_URL);
        goto END1;
    }

    /* 截断超过最大呈现的数据 */
    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_BLACK_URL);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Get num of %d ERROR!\n", KEY_EVENT_SHOW_BLACK_URL);
        CA_LOG(LOG_MODULE, LOG_PROC, "***** Get num of %s ERROR!******\n", KEY_EVENT_SHOW_BLACK_URL);
        goto END1;
    }
    if ((unsigned int)retn > g_store_info_cfg.url_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_BLACK_URL, 0, g_store_info_cfg.url_show_num -1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_BLACK, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_BLACK_URL);
            CA_LOG(LOG_MODULE, LOG_PROC, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_BLACK_URL);
            goto END1;
        }
    }


    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_SHOW_BLACK_URL);
END1:
	free(top5_values);
    free(push_values);
END:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  DNS redis 入库
 */
/* -------------------------------------------*/
static int store_redis_black_dns(store_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line)
{
    time_t key_del_time;
    int retn = 0;
    RVALUES push_values = NULL;
    int val_num = 0;
    char current_max_time[TIME_FORMAT_SIZE];

    /* 0. 设置今日统计 今日top5  生命周期 */
    key_del_time = get_next_zero_time();
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_COUNT)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_COUNT, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_BLACK, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
        }
    }

    /* 1. 今日事件统计表 */
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_BDNS, line);    
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Store Redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        goto END;
    }

    /* 2. 事件展示表 */
    push_values = calloc(g_store_info_cfg.dns_show_num, VALUES_ID_SIZE);    
    if (push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Calloc redis vals buf ERROR!\n");
        goto END;
    }

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_BLACK_DNS, 8, g_store_info_cfg.dns_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "DNS Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_BLACK_DNS);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    /* 批量命令行入库 */
    make_bdns_show_values(push_values, &val_num, buf, buf_size, line, current_max_time);
    if (val_num == 0) {
        goto END1;
    }
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_BLACK_DNS, push_values, val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Store redis lpush  %s ERROR!\n", KEY_EVENT_SHOW_BLACK_DNS);
        goto END1;
    }

    /* 截断超过最大呈现的数据 */
    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_BLACK_DNS);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "Get num of %d ERROR!\n", KEY_EVENT_SHOW_BLACK_DNS);
        goto END1;
    }
    if ((unsigned int)retn > g_store_info_cfg.dns_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_BLACK_DNS, 0, g_store_info_cfg.dns_show_num -1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_BLACK, "Trim list %s ERROR!\n", KEY_EVENT_SHOW_BLACK_DNS);
            goto END1;
        }
    }

END1:
    free(push_values);
END:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  URL postgresql 入库
 */
/* -------------------------------------------*/
static int store_postgresql_black_url(store_conn_t *conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;

    /*  执行批量插入命令 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, TABLE_NAME_BURL, g_url_copy_cmd, buf, buf_size);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "copy %s error!\n", TABLE_NAME_BURL);
        STORE_ERROR(DEBUG_TYPE_BLACK, "copy %s error!\n", TABLE_NAME_BURL);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_burl_file_path, file->file_name);
    }
    else {
        /* 命令执行成功 删除数据文件 */
        STORE_DEBUG(DEBUG_TYPE_BLACK, "copy %s succ!\n", TABLE_NAME_BURL);
        CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] to table[%s] succ!\n", file->path, TABLE_NAME_BURL);
        if (remove(file->path) != 0) {
            perror("remove file");
            /* 删除失败 */
            STORE_ERROR(DEBUG_TYPE_BLACK, "remove file %s error!\n", file->path);
            CA_LOG(LOG_MODULE, LOG_PROC, "remove file %s error!\n", file->path);
                    
            /* TODO XXX 挂起 */
        }
    }
}

/* -------------------------------------------*/
/**
 * @brief  DNS postgresql 入库
 */
/* -------------------------------------------*/
static int store_postgresql_black_dns(store_conn_t *conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;

    /*  执行批量插入命令 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, TABLE_NAME_BDNS, g_dns_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "copy %s error!\n", TABLE_NAME_BDNS);
        STORE_ERROR(DEBUG_TYPE_BLACK, "copy %s error!\n", TABLE_NAME_BDNS);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_bdns_file_path, file->file_name);
    }
    else {
        /* 命令执行成功 删除数据文件 */
        STORE_DEBUG(DEBUG_TYPE_BLACK, "copy %s succ!\n", TABLE_NAME_BDNS);
        CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] to table[%s] succ!\n", file->path, TABLE_NAME_BDNS);
        if (remove(file->path) != 0) {
            perror("remove file");
            /* 删除失败 */
            STORE_ERROR(DEBUG_TYPE_BLACK, "remove file %s error!\n", file->path);
            CA_LOG(LOG_MODULE, LOG_PROC, "remove file %s error!\n", file->path);
                    
            /* TODO XXX 挂起 */
        }
    }
}
/*--------------------------------------------*/
/**
 *@brief mysql入库
 */
/*--------------------------------------------*/

static int store_mysql_burl(store_conn_t* conn, char *buf, unsigned long buf_size, unsigned int line)
{
	int retn = 0;
	RVALUES values = NULL;
	char sql_cmd[1024] = {0};
	int val_num = 0;
	char table_name[] = {"t_event"}; 
	pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
	values = calloc(line, VALUES_ID_SIZE);
	make_burl_mysql_values(values, &val_num, buf, buf_size, line);
	if ( mysql_set_character_set( conn->mysql_conn, "utf8" ) ) { 
            fprintf ( stderr , "error, %s/n" , mysql_error( conn->mysql_conn) ) ; 
        } 
	sprintf(sql_cmd, "load data local infile \'%s\' into table %s character set utf8 fields terminated by \'||\'(ename,etype,event_exttype,risk_level,event_time,daddr,saddr,sport,dport,event_detail,url,data_resource);", TMP_BURL_EVENT_PATH, g_store_info_cfg.mysql_table_name);
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
	remove(TMP_BURL_EVENT_PATH);
	return retn;
}
/* -------------------------------------------*/
/**
 * @brief  bdns 入库事件
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_bdns_data(void *data, void *arg)
{
    file_data_t *file = NULL;
    store_conn_t *conn = NULL;
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
    //memset(g_black_buf, 0, BLACK_EVENT_BUF_SIZE);

    STORE_DEBUG(DEBUG_TYPE_BLACK, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);
    /*  将文件数据读到内存缓冲中 */
    //buf_size = read_file_to_data(file->path, g_black_buf);
	
	char *black_malloc=NULL;
    /*  将文件数据读到内存缓冲中 */
    buf_size = read_file_to_data_m(file->path, (void **)&black_malloc);
	char g_black_buf[buf_size+1];
    memset(g_black_buf, 0, buf_size);
	strncpy(g_black_buf,black_malloc,buf_size);
	if (NULL != black_malloc)
		free(black_malloc);
    /*  得到数据行数 */
    line = get_event_num(g_black_buf, buf_size);    
    if (line == 0) {
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_bdns_file_path, file->file_name);
        goto END;
    }
    /*----------------mysql 入库--------------------*/
    if(g_store_info_cfg.mysql_switch == 1) {
    if(g_store_info_cfg.mysql_apt == 1) {
    	store_mysql_burl(conn, g_black_buf, buf_size, line);
    }
    }

    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_black_dns(conn, file, g_black_buf, buf_size);    
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        goto END;
    }

    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_black_dns(conn, g_black_buf, buf_size, line);

    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buf_size, line, test_time.sec);
    }

END:
    return 0;
}


/* -------------------------------------------*/
/**
 * @brief  burl 入库事件
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_burl_data(void *data, void *arg)
{
    file_data_t *file = NULL;
    store_conn_t *conn = NULL;
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
    //memset(g_black_buf, 0, BLACK_EVENT_BUF_SIZE);

    STORE_DEBUG(DEBUG_TYPE_BLACK, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);
	char *black_malloc=NULL;
    /*  将文件数据读到内存缓冲中 */
    buf_size = read_file_to_data_m(file->path, (void **)&black_malloc);
	char g_black_buf[buf_size+1];
    memset(g_black_buf, 0, buf_size);
	strncpy(g_black_buf,black_malloc,buf_size);
	if (NULL != black_malloc)
		free(black_malloc);
    /*  将文件数据读到内存缓冲中 */
    //buf_size = read_file_to_data(file->path, g_black_buf);
    /*  得到数据行数 */
    line = get_event_num(g_black_buf, buf_size);    
    if (line == 0) {
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_burl_file_path, file->file_name);
        goto END;
    }
    /* --------------- mysql 入库 ---------------*/    
    if(g_store_info_cfg.mysql_switch == 1) {
	    if(g_store_info_cfg.mysql_apt == 1) {
		    store_mysql_burl(conn, g_black_buf, buf_size, line);
	    }
    }

    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_black_url(conn, file, g_black_buf, buf_size);    
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        goto END;
    }

    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_black_url(conn, g_black_buf, buf_size, line);

    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buf_size, line, test_time.sec);
    }

END:
    return 0;
}

#if 1
static int do_black_dns_store(store_conn_t* conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    //XXX
    goto EXIT;

    /* 遍历 dns 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_store_info_cfg.event_bdns_file_path, conn->list);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "travel dns dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;
    
    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_bdns_data, (void*)conn);
    }


    /* 清空链表 */
    for (i = 0; i < list_num; ++i) {
        tmp_rm_file = g_list_pop(conn->list);
        free(tmp_rm_file);
    }

EXIT:
    return retn;
}
#endif

static int do_black_url_store(store_conn_t* conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    /* 遍历 dns 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_store_info_cfg.event_burl_file_path, conn->list);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_BLACK, "travel dns dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;
    
    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_burl_data, (void*)conn);
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
 * @brief   黑名单DNS URL 
 *            存储处理流程
 *
 * @param arg
 *
 * @returns   
 *        
 *        该线程ID
 */
/* -------------------------------------------*/
void *store_black_busi(void *arg)
{
    int count = 0;
    int retn = 0;
    pthread_t tid;
    tid = pthread_self();

    store_conn_t black_conn;
    list_t black_list;

    /*-------------  初始化配置信息 ------------- */
    /* 1. 初始化链表 */
    list_init(&(black_list), file_data_t);
    black_conn.list = &black_list;
    /* 2. 初始化COPY命令 */
    init_copy_cmd(THREAD_BLACK_URL, g_url_copy_cmd, 400);
    init_copy_cmd(THREAD_BLACK_DNS, g_dns_copy_cmd, 400);

    retn = store_connect_db(&black_conn, THREAD_BLACK);
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

        do_black_dns_store(&black_conn);

        do_black_url_store(&black_conn);

        usleep(50000);
        ++count;
    
        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            store_disconnect_db(&black_conn, THREAD_BLACK);
            retn = store_connect_db(&black_conn, THREAD_BLACK);
            if (retn == -1) {                                                                                     
                goto EXIT_0;                                                                                        
            }                                                                                                     
   			 else if (retn == -3) {                                                                                         
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
		    mysql_close(black_conn.mysql_conn);
		    mysql_thread_end();
    }
EXIT_3:
    /* 关闭redis链接 */
    grd_disconnect(black_conn.redis_conn);    

EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(black_conn.psql_conn);    

EXIT_0:
    pthread_exit((void*)tid);
}

