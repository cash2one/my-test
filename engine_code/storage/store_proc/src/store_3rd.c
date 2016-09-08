/**
 * @file store_3rd.c
 * @brief  特征事件（网站，恶感，其他）
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-03
 */

#include "gms_store.h"

/* 其他三类事件数据缓冲 */
//char g_3rd_buf[_3RD_EVENT_BUF_SIZE];
/* 其他三类事件批量插入命令 */
char g_3rd_copy_cmd[400];


/* -------------------------------------------*/
/**
 * @brief  封装TOP5的数据
 */
/* -------------------------------------------*/
RVALUES make_3rd_top5_values(RVALUES vals, char *buf, unsigned long buf_size)
{
    RVALUES values = vals;        
    char *pos = NULL;
    char *end = NULL;    
    char  value[VALUES_ID_SIZE];
    char etype[VALUES_ID_SIZE];
    char *line_head = NULL;
    int i = 0;

    end = buf + buf_size;        

    /*  从尾部遍历 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            line_head = pos+1;

            get_value_by_col(line_head, 5, etype, VALUES_ID_SIZE-1,0);

            if (!strcmp(etype, EVENT_TYPE_WEB)) {
                    /* saddr */
                    strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            }
            else if (!strcmp(etype, EVENT_TYPE_MALICIOUS)) {
                    /* saddr */
                    strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            }
            else if (!strcmp(etype, EVENT_TYPE_ELSE)) {
                    /* saddr */
                    strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            }

            //STORE_DEBUG(DEBUG_TYPE_3RD, "Redis top5  daddr[%d]:%s\n", i, values[i]);
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
static RVALUES make_3rd_show_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line, char *etype, unsigned int show_num, char *max_time)
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
    
    max_len = (line > show_num)? show_num: line;

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            if (*pos == '\n') {
                line_head = pos+1;
            }
            else {
                line_head = pos;
            }

            if (!strcmp(etype, 
                        get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1,0))) {

                if (strncmp(max_time, get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), TIME_FORMAT_SIZE) > 0) {
                    printf("max_time %s > %s\n", max_time, get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0));
                    goto READ_ONE_LINE_DONE;
                }

             //   printf("max_time %s\n", max_time);
				memset(values[i], 0, VALUES_ID_SIZE-1);

                /* begin_time */
                strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
			//	printf("values[%d]:%s\n", i, values[i]);
                /* end_time */
                strncat(values[i], get_value_by_col(line_head, 7, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* devid */
                strncat(values[i], get_value_by_col(line_head, 1, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* ename */
                strncat(values[i], get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* daddr */
                strncat(values[i], get_value_by_col(line_head, 12, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* saddr */
                strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* sport */
                strncat(values[i], get_value_by_col(line_head, 14, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* dport */
                strncat(values[i], get_value_by_col(line_head, 15, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* risk_level */
                strncat(values[i], get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
                strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		//		printf("values[%d]:%s\n", i, values[i]);
                /* eventdetail */
                strncat(values[i], get_value_by_col(line_head, 16, value, VALUES_DETAIL_SIZE-1, 0), VALUES_ID_SIZE-1);

                STORE_DEBUG(DEBUG_TYPE_ATT, "Redis Push Values[%d]:%s\n", i, values[i]);
				printf("+++++++++redis push values[%d]:%s\n", i, values[i]);


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
    }

    return values;    
}
static RVALUES make_3rd_mysql_values(RVALUES vals, int *val_num, char *buf, unsigned long buf_size, unsigned int line) 
{
    RVALUES values = vals;        
    char *pos = NULL;
    char *end = NULL;    
    char  value[VALUES_ID_SIZE];
    char *line_head = NULL;
    char pdaddr[50];
    char psaddr[50];
    unsigned int max_len = 0;
    int i = 0;
    int fd = 0;
	char type_flag[5];

    end = buf + buf_size;        

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
		memset(type_flag,0,5);
            /* ename */
            strncat(values[i], get_value_by_col(line_head, 2, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* etype */
	    if (!strcmp(EVENT_TYPE_MALICIOUS,
                         get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1,0))) {
				snprintf(type_flag,5,"%s",EVENT_TYPE_MALICIOUS);
            	strncat(values[i], "1", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    } else if (!strcmp(EVENT_TYPE_WEB,
                        get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1,0))) {
				snprintf(type_flag,5,"%s",EVENT_TYPE_WEB);
            	strncat(values[i], "6", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    } else {	
				snprintf(type_flag,5,"%s",EVENT_TYPE_ELSE);
            	strncat(values[i], "11", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    }
            /* event_exttype */
            strncat(values[i], get_value_by_col(line_head, 11, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* risk_level */
	    if (strcmp(get_value_by_col(line_head, 18, value, VALUES_ID_SIZE-1, 0), "5") != 0) {
            	strncat(values[i], "2", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    } else {
            	strncat(values[i], "1", VALUES_ID_SIZE-1);
            	strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    }
	    /* begin_time */
            strncat(values[i], get_value_by_col(line_head, 6, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* daddr */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 12, value, VALUES_ID_SIZE-1, 0),pdaddr), VALUES_ID_SIZE-1);
           // strncat(values[i], get_value_by_col(line_head, 12, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* saddr */
            strncat(values[i], ip_int_to_char(get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0),psaddr), VALUES_ID_SIZE-1);
            //strncat(values[i], get_value_by_col(line_head, 13, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* sport */
            strncat(values[i], get_value_by_col(line_head, 14, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* dport */
            strncat(values[i], get_value_by_col(line_head, 15, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
            /* eventdetail */
            strncat(values[i], get_value_by_col(line_head, 16, value, VALUES_ID_SIZE-1, 0), VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
			
	    /*url*/
	    strncat(values[i], "null", VALUES_ID_SIZE-1);
            strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
	    /*data_resource*/
			strncat(values[i], "0", VALUES_ID_SIZE-1);
			strncat(values[i], REDIS_DILIMT, REDIS_DILIMT_SIZ);
		/*gms type*/
			strncat(values[i], type_flag, VALUES_ID_SIZE-1);
	    //printf("_______values[%d]:%s\n", i, values[i]);

	    fd = open(TMP_3RD_EVENT_PATH, O_RDWR | O_CREAT | O_APPEND);
	    if(write(fd, values[i], strlen(values[i])) != (int)strlen(values[i])) {
	    	printf("write error!\n");
	        close(fd);
	    } else {
	    	write(fd, "\n", 1);
            	close(fd);
                printf("write success!:%s\n",TMP_3RD_EVENT_PATH);
	    }
            //STORE_DEBUG(DEBUG_TYPE_ATT, "Redis Push Values[%d]:%s\n", i, values[i]);
#endif
            *val_num = *val_num + 1;
            ++i;
        }
    }

    return values;    
}

static int get_line_by_etype(char *buf, unsigned long buf_size, unsigned int *web_line, unsigned int *code_line, unsigned int *else_line)
{
    char  value[VALUES_ID_SIZE];
    char *line_head = NULL;
    char *pos = NULL;
    char *end = NULL;    
    int i = 0;

    end = buf + buf_size;        

    /* 大于展示最大数目 从尾部遍历 截取出最大数目条数 */
    for (i = 0, pos = end-2; pos >= buf; --pos) {
        if (*pos == '\n' || pos == buf) {
            line_head = pos+1;

            if (!strcmp(EVENT_TYPE_MALICIOUS, 
                        get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1,0))) {
                *code_line = *code_line + 1;
            }
            if (!strcmp(EVENT_TYPE_WEB,
                        get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1,0))) {
                *web_line = *web_line + 1;
            }
            if (!strcmp(EVENT_TYPE_ELSE, 
                        get_value_by_col(line_head, 5, value, VALUES_ID_SIZE-1,0))) {
                *else_line = *else_line + 1;
            }
        }
    }
}

/* -------------------------------------------*/
/**
 * @brief  redis 入库
 */
/* -------------------------------------------*/
static int store_redis_3rd(store_conn_t *conn, char *buf, unsigned long buf_size, unsigned int line) {
    int retn = 0;
    RVALUES top5_values;

    RVALUES web_push_values;
    unsigned int web_line = 0;
    int web_val_num = 0;
    RVALUES code_push_values;
    unsigned int code_line = 0;
    int code_val_num = 0;
    RVALUES else_push_values;
    unsigned int else_line = 0;
    int else_val_num = 0;
    char current_max_time[TIME_FORMAT_SIZE];
	 char time_format[] = "%Y-%m-%d %X"; 
	 char tmp_time[TIME_FORMAT_SIZE] = {0};
	 struct tm *time_p = NULL;

#if 1
    time_t key_del_time;
	time_t week_top10_del;

    /* 0. 设置今日统计 今日top5 生命周期 */
    key_del_time = get_next_zero_time();
#if 0
	week_top10_del = get_next_week_zero_time();
#endif
	time_p = localtime(&key_del_time);
	strftime(tmp_time, TIME_FORMAT_SIZE - 1, time_format, time_p);
#if 0
	printf("----------------------\n");
	printf("+++++++++++key_del_time:%ld\n", key_del_time);
	printf("+++++++++++tmp_time:%s\n", tmp_time);
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_ATTACK_WEEK_TOP10)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_ATTACK_WEEK_TOP10, week_top10_del);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_3RD, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_WEEK_TOP10);
            CA_LOG(LOG_MODULE, LOG_PROC, "*** redis Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_WEEK_TOP10);
        }
	}
#endif
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_COUNT)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_COUNT, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_3RD, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
            CA_LOG(LOG_MODULE, LOG_PROC, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
        }
    }
    if (grd_is_key_exist(conn->redis_conn, KEY_EVENT_ATTACK_TOP5)) {
        retn = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_ATTACK_TOP5, key_del_time);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_3RD, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
            CA_LOG(LOG_MODULE, LOG_PROC, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
        }
    }
#endif

    get_line_by_etype(buf, buf_size, &web_line, &code_line, &else_line);

    /*  今日被攻击top5 事件 */
    top5_values = calloc(line, VALUES_ID_SIZE);
    if (top5_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD Calloc redis top5 vals ERROR!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD Calloc redis top5 vals ERROR!\n");
        goto END_TOP5;
    }
    make_3rd_top5_values(top5_values, buf, buf_size);
    retn = grd_zset_increment_append(conn->redis_conn, KEY_EVENT_ATTACK_TOP5, top5_values, line);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD Store redis top5 %s ERROR\n", KEY_EVENT_ATTACK_TOP5);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD Store redis top5 %s ERROR\n", KEY_EVENT_ATTACK_TOP5);
        goto END_TOP5;
    }
#if 0
    retn = grd_zset_increment_append(conn->redis_conn, KEY_EVENT_ATTACK_WEEK_TOP10, top5_values, line);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD Store redis top5 %s ERROR\n", KEY_EVENT_ATTACK_WEEK_TOP10);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD Store redis top5 %s ERROR\n", KEY_EVENT_ATTACK_WEEK_TOP10);
        goto END_TOP5;
    }
#endif
END_TOP5:
    if (top5_values != NULL)
        free(top5_values);

    if (web_line <= 0) {
        goto BEGIN_CODE;
    }
    /* ================== I. 网站事件 =====================*/
    /* 1. 今日事件统计表 */
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_WEB, web_line);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(web) store redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(web) store redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        goto BEGIN_CODE;
    }
    /* 2. 事件展示表 */
    web_push_values = calloc(g_store_info_cfg.web_show_num, VALUES_ID_SIZE);
    if (web_push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(web) Calloc redis push vals error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(web) Calloc redis push vals error!\n");
        goto END_WEB;
    }

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_WEB, 0, g_store_info_cfg.web_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(web) Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_WEB);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    make_3rd_show_values(web_push_values, &web_val_num, buf, buf_size, web_line, EVENT_TYPE_WEB, g_store_info_cfg.web_show_num, current_max_time);
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_WEB, web_push_values, web_val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(web) redis lpush %s ERROR!\n", KEY_EVENT_SHOW_WEB);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(web) redis lpush %s ERROR!\n", KEY_EVENT_SHOW_WEB);
        goto END_WEB;
    }
    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_WEB);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(web) Get num of %s ERROR!\n", KEY_EVENT_SHOW_WEB);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(web) Get num of %s ERROR!\n", KEY_EVENT_SHOW_WEB);
        goto END_WEB;
    }
    if ((unsigned int)retn > g_store_info_cfg.web_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_WEB, 0, g_store_info_cfg.web_show_num-1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_3RD, "3RD(web) Trim list %s ERROR\n", KEY_EVENT_SHOW_WEB);
            CA_LOG(LOG_MODULE, LOG_PROC, "3RD(web) Trim list %s ERROR\n", KEY_EVENT_SHOW_WEB);
            goto END_WEB;
        }
    }

    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_SHOW_WEB);
END_WEB:
    if (web_push_values != NULL)
        free(web_push_values);

BEGIN_CODE:
    if (code_line <= 0) {
        goto BEGIN_ELSE;
    }
    /* ================= II. 恶意代码 ===================*/
    /* 1. 今日事件统计表 */
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_MALICIOUS, code_line);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(code) store redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) store redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        goto BEGIN_ELSE;
    }
    /* 2. 事件展示表 */
    code_push_values = calloc(g_store_info_cfg.code_show_num, VALUES_ID_SIZE);
    if (code_push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(code) Calloc redis push vals error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) Calloc redis push vals error!\n");
        goto END_CODE;
    }

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_CODE, 0, g_store_info_cfg.code_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_CODE);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    make_3rd_show_values(code_push_values, &code_val_num, buf, buf_size, code_line, EVENT_TYPE_MALICIOUS, g_store_info_cfg.code_show_num, current_max_time);
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_CODE, code_push_values, code_val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(code) redis lpush %s ERROR!\n", KEY_EVENT_SHOW_CODE);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) redis lpush %s ERROR!\n", KEY_EVENT_SHOW_CODE);
        goto END_CODE;
    }

    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_CODE);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(code) Get num of %s ERROR!\n", KEY_EVENT_SHOW_CODE);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) Get num of %s ERROR!\n", KEY_EVENT_SHOW_CODE);
        goto END_CODE;
    }
    if ((unsigned int)retn > g_store_info_cfg.code_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_CODE, 0, g_store_info_cfg.code_show_num-1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_3RD, "3RD(code) Trim list %s ERROR\n", KEY_EVENT_SHOW_CODE);
            CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) Trim list %s ERROR\n", KEY_EVENT_SHOW_CODE);
            goto END_CODE;
        }
    }

    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_SHOW_CODE);
END_CODE:
    if (code_push_values != NULL)
        free(code_push_values);

BEGIN_ELSE:
    if (else_line <= 0) {
        goto END;
    }
    /* ================= III. 其他事件 ==================*/
    /* 1. 今日事件统计表 */
    retn = grd_hincrement_one_field(conn->redis_conn, KEY_EVENT_COUNT, EVENT_TYPE_ELSE, else_line);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(else) store redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(else) store redis increment %s ERROR!\n", KEY_EVENT_COUNT);
        goto END_ELSE;
    }
    /* 2. 事件展示表 */
    else_push_values = calloc(g_store_info_cfg.else_show_num, VALUES_ID_SIZE);
    if (else_push_values == NULL) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(else) Calloc redis push vals error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(else) Calloc redis push vals error!\n");
        goto END_ELSE;
    }

    memset(current_max_time, 0, TIME_FORMAT_SIZE);
    retn = store_get_redistable_last_time(current_max_time, conn->redis_conn, KEY_EVENT_SHOW_ELSE, 0, g_store_info_cfg.else_show_num);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(code) Get MAX time of %s ERROR!\n", KEY_EVENT_SHOW_ELSE);
        memset(current_max_time, 0, TIME_FORMAT_SIZE);
    }

    make_3rd_show_values(else_push_values, &else_val_num, buf, buf_size, else_line, EVENT_TYPE_ELSE, g_store_info_cfg.else_show_num, current_max_time);
    retn = grd_list_push_append(conn->redis_conn, KEY_EVENT_SHOW_ELSE, else_push_values, else_val_num);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(else) redis lpush %s ERROR!\n", KEY_EVENT_SHOW_ELSE);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(else) redis lpush %s ERROR!\n", KEY_EVENT_SHOW_ELSE);
        goto END_ELSE;
    }
    retn = grd_get_list_cnt(conn->redis_conn, KEY_EVENT_SHOW_ELSE);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "3RD(else) Get num of %s ERROR!\n", KEY_EVENT_SHOW_ELSE);
        CA_LOG(LOG_MODULE, LOG_PROC, "3RD(else) Get num of %s ERROR!\n", KEY_EVENT_SHOW_ELSE);
        goto END_ELSE;
    }
    if ((unsigned int)retn > g_store_info_cfg.else_show_num) {
        retn = grd_trim_list(conn->redis_conn, KEY_EVENT_SHOW_ELSE, 0, g_store_info_cfg.else_show_num-1);
        if (retn < 0) {
            STORE_ERROR(DEBUG_TYPE_3RD, "3RD(else) Trim list %s ERROR\n", KEY_EVENT_SHOW_ELSE);
            CA_LOG(LOG_MODULE, LOG_PROC, "3RD(else) Trim list %s ERROR\n", KEY_EVENT_SHOW_ELSE);
            goto END_ELSE;
        }
    }

    CA_LOG(LOG_MODULE, LOG_PROC, "Redis Database --> Insert Key[%s] SUCCESS!", KEY_EVENT_SHOW_ELSE);
END_ELSE:
    if (else_push_values != NULL)
        free(else_push_values);

END:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  postgresql 入库
 */
/* -------------------------------------------*/
static int store_postgresql_3rd(store_conn_t *conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;

    char table_name[TABLE_NAME_SIZE];

    /* 根据月份选择分区表 */
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, TABLE_NAME_3RD_HIS);
    strncat(table_name, TABLE_SUFFIX, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);

    change_att_copy_cmd(g_3rd_copy_cmd, 400, table_name);

    /* 执行批量插入命令到HIS表 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_3rd_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "copy %s to %s error!\n", file->file_name, table_name);
        CA_LOG(LOG_MODULE, LOG_PROC, "copy %s to %s error!\n", file->file_name, table_name);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_3rd_file_path, file->file_name);
        goto END;
    }

    /*根据月，天选择t_event_3rdquery_m_d分区表*/
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, TABLE_NAME_3RD_QUERY);
    strncat(table_name, TABLE_SUFFIX_MONTH, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);
    strncat(table_name, TABLE_SUFFIX_DAY, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->day);

    change_att_copy_cmd(g_3rd_copy_cmd, 400, table_name);
    /* 执行批量插入命令到t_event_3rdquery_m_d表 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_3rd_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_ATT, "copy %s to %s error!\n", file->file_name, table_name);
        CA_LOG(LOG_MODULE, LOG_PROC, "*****copy %s to %s error!******\n", file->file_name, table_name);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_attack_file_path, file->file_name);
        goto END;
    }
#if 0
    /*插入t_event_3rdtmp表*/
    change_att_copy_cmd(g_3rd_copy_cmd, 400, TABLE_NAME_3RD_TMP);

    /*  执行批量插入命令 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, TABLE_NAME_3RD_TMP, g_3rd_copy_cmd, buf, buf_size);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "copy %s to %s error!\n", file->file_name, TABLE_NAME_3RD_TMP);
        CA_LOG(LOG_MODULE, LOG_PROC, "copy %s to %s error!\n", file->file_name, TABLE_NAME_3RD_TMP);
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_3rd_file_path, file->file_name);
    }
    else {
        /* 命令执行成功 删除数据文件 */
        STORE_DEBUG(DEBUG_TYPE_3RD, "copy %s succ!\n", TABLE_NAME_3RD_TMP);
        CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] to table[%s] succ!\n", file->path, TABLE_NAME_3RD_TMP);
    }
#endif
END:
    return retn;
}
static int store_mysql_3rd(store_conn_t* conn, char *buf, unsigned long buf_size, unsigned int line)
{
	RVALUES values = NULL;
	char sql_cmd[2048] = {0};
	int val_num = 0;
	int retn = 0;
	pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
	values = calloc(line, VALUES_ID_SIZE);
	make_3rd_mysql_values(values, &val_num, buf, buf_size, line);
	sprintf(sql_cmd, "load data local infile \'%s\' into table %s character set utf8 fields terminated by \'||\'(ename,etype,event_exttype,risk_level,event_time,daddr,saddr,sport,dport,event_detail,url,data_resource,gmstype);", TMP_3RD_EVENT_PATH, g_store_info_cfg.mysql_table_name);
	printf("sql_cmd:%s\n", sql_cmd);
	pthread_mutex_lock(&mutex2);
	retn = mysql_query(conn->mysql_conn, sql_cmd);
	printf("++++++ret:%d\n", retn);
	if (retn != 0) {
        	CA_LOG(LOG_MODULE, LOG_PROC, "load error! %d:%s\n", mysql_errno(conn->mysql_conn), mysql_error(conn->mysql_conn));
    		retn = -1;
		goto END;
	} 
END:
	pthread_mutex_unlock(&mutex2);
	free(values);
	remove(TMP_3RD_EVENT_PATH);
	return retn;
}

/* -------------------------------------------*/
/**
 * @brief  特征其他3事件 入库实现
 *
 * @param data        链表中的数据
 * @param arg        传进来的参数
 *
 * @returns   
 *        0  遍历停止
 *        1  遍历继续
 */
/* -------------------------------------------*/
static int inject_3rd_data(void *data, void *arg)
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
//	char *tmp_malloc=NULL;
    test_stat_t test_time;
    TEST_VARS;
    
    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        /* 时间测试 */
        memset(&test_time, 0, sizeof(test_stat_t));
        TEST_START(test_time);
    }

    file = (file_data_t*)data;
    conn = (store_conn_t*)arg;
    //memset(g_3rd_buf, 0, _3RD_EVENT_BUF_SIZE);

    STORE_DEBUG(DEBUG_TYPE_3RD, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);

    /*  将文件数据读到内存缓冲中 */
/*    buf_size = read_file_to_data_m(file->path, (void **)&tmp_malloc);
	char g_3rd_buf[buf_size+1];
    memset(g_3rd_buf, 0, buf_size);
	strncpy(g_3rd_buf,tmp_malloc,buf_size);
	if (NULL != tmp_malloc)
		free(tmp_malloc);*/

    FILE* readstream = fopen(file->path,"r+");
    if(readstream == NULL){
        STORE_ERROR(DEBUG_TYPE_ATT, "\nstore 3rd fopen File[%s] error!\n",file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "****3rd inject_3rd_data fopen ERROR!!****");
        return -1;       
    }
    fseek(readstream,0,SEEK_END);
    file_size = ftell(readstream);
    fseek(readstream,0,SEEK_SET);

    char* onedata_buffer = (char *)malloc(10240);
    char* tendata_buffer = (char *)malloc(102400);
    if(onedata_buffer == NULL || tendata_buffer == NULL){
        STORE_ERROR(DEBUG_TYPE_ATT, "malloc buffer error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "****att inject_attack_data malloc ERROR!!****");
    }
    memset(onedata_buffer,0,10240);
    memset(tendata_buffer,0,102400);

    /*  得到数据行数 */
//    line = get_event_num(g_3rd_buf, buf_size);    
//    if (line == 0) {
        /* 将文件移到错误目录下 */
//        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_3rd_file_path, file->file_name);
//        goto END;
 //   }

/*    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===GET LINE===  line:%d Spend:[%f] sec.\n",sum_line, test_time.sec);
    }*/
    
//    STORE_DEBUG(DEBUG_TYPE_3RD, "[%s] SIZE:%d, line:%d\n", file->path, buffer_offset, sum_line);

/*    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        memset(&test_time, 0, sizeof(test_stat_t));
        TEST_START(test_time);
    }*/

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
            store_mysql_3rd(conn, tendata_buffer, buffer_offset, flag);
                }
            }
            /* --------------- postgres 入库 ---------------*/ 
            retn = store_postgresql_3rd(conn, file, tendata_buffer, buffer_offset);
            if (retn < 0) {
                goto END;
            }
 /*           if (IS_STORE_SPENDTIME_DEBUG_ON) {
                TEST_END(test_time);
                STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n==POSTGRESQL=> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buffer_offset, sum_line, test_time.sec);
            }*/

/*            if (IS_STORE_SPENDTIME_DEBUG_ON) {
                memset(&test_time, 0, sizeof(test_stat_t));
                TEST_START(test_time);
            }*/

            /* 4. 向reids中插入数据 */
            retn = store_redis_3rd(conn, tendata_buffer, buffer_offset, flag);

/*            if (IS_STORE_SPENDTIME_DEBUG_ON) {
                TEST_END(test_time);
                STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n==REDIS=> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buffer_offset, sum_line, test_time.sec);
            }*/
            memset(onedata_buffer,0,10240);
            memset(tendata_buffer,0,102400);

            flag = 0;
            buffer_offset = 0;
        }
    }
    if (sum_line == 0) {
        /* 将文件移到错误目录下 */
        move_file_to_wrong_path(file->path, g_store_info_cfg.wrong_3rd_file_path, file->file_name);
        goto END;
    }
	if (flag !=0)
	{
    //剩下的不超过10条的buffer中的数据也需要入库
    /*----------------mysql 入库--------------------*/
    if(g_store_info_cfg.mysql_switch == 1) {
        if(g_store_info_cfg.mysql_mtx == 1) {
            store_mysql_3rd(conn, tendata_buffer, buffer_offset, flag);
            }
    }
    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_3rd(conn, file, tendata_buffer, buffer_offset);
    if (retn < 0) {
        goto END;
    }

/*    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n==POSTGRESQL=> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buffer_offset, sum_line, test_time.sec);
    }*/

/*    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        memset(&test_time, 0, sizeof(test_stat_t));
        TEST_START(test_time);
    }*/

    /* 4. 向reids中插入数据 */
    retn = store_redis_3rd(conn, tendata_buffer, buffer_offset, flag);
	}
    if (IS_STORE_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
    //    STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===read & store 3rd ===  Spend:[%f] sec.\n",test_time.sec);
        STORE_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===>read & store File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, file_size, sum_line, test_time.sec);
    }


    strcat(email_path,mtx);
	strcat(email_path,file->file_name);//move file to the email path

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


static int do_3rd_store(store_conn_t* conn)
{
    int retn = 0;
	int ret = 0;
    int i = 0;
    int list_num = 0;
	time_t key_del_time;
    file_data_t *tmp_rm_file = NULL;
	char time_format[] = "%Y-%m-%d %X";
	char tmp_time[TIME_FORMAT_SIZE] = {0};
	struct tm *time_p = NULL;

    /* 遍历 3rd 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_store_info_cfg.event_3rd_file_path, conn->list);
    if (retn < 0) {
        STORE_ERROR(DEBUG_TYPE_3RD, "travel 3rd dir error!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "travel 3rd dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;
    
    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_3rd_data, (void*)conn);
    } else {
		//printf("====================\n");
		key_del_time = get_next_zero_time();
		//printf("key_Del_time:%ld\n", key_del_time);
		time_p = localtime(&key_del_time);                                                                        
		strftime(tmp_time, TIME_FORMAT_SIZE - 1, time_format, time_p); 
		//printf("tmp_time:%s\n", tmp_time);
		ret = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_COUNT, key_del_time);
		/*if (ret < 0) {
			STORE_ERROR(DEBUG_TYPE_3RD, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
			CA_LOG(LOG_MODULE, LOG_PROC, "Set %s delete time ERROR!\n", KEY_EVENT_COUNT);
		}*/
		ret = grd_set_key_lifecycle(conn->redis_conn, KEY_EVENT_ATTACK_TOP5, key_del_time);
		/*if (ret < 0) {
			STORE_ERROR(DEBUG_TYPE_3RD, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
			CA_LOG(LOG_MODULE, LOG_PROC, "Set %s delete time ERROR!\n", KEY_EVENT_ATTACK_TOP5);
		}*/
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
 * @brief  mtx 引擎 产生 特征事件（网站，恶感，其他）
 *           3种事件 存储处理流程 
 *
 * @param arg
 *
 * @returns   
 *        该线程ID 
 */
/* -------------------------------------------*/
void *store_3rd_busi(void *arg)
{
    int retn = 0;
    int count = 0;
    pthread_t tid;
    tid = pthread_self();

    store_conn_t _3rd_conn;
    list_t _3rd_list;

    /* ------------- 初始化配置信息 ------------------- */
    /* 1. 初始化链表 */
    list_init(&(_3rd_list), file_data_t);
    _3rd_conn.list = &_3rd_list;
    /* 2. 初始化COPY命令 */
    init_copy_cmd(THREAD_3RD, g_3rd_copy_cmd, 400);
    

    retn = store_connect_db(&_3rd_conn, THREAD_3RD);
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

        do_3rd_store(&_3rd_conn);

        usleep(50000);
        ++count;
        if(count >= NUM) {                                                                                        
            count = 0;                                                                                            
            store_disconnect_db(&_3rd_conn, THREAD_3RD);
            retn = store_connect_db(&_3rd_conn, THREAD_3RD);  
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


    /* 关闭mysql链接下次重新连接 */
    if(g_store_info_cfg.mysql_switch==1)
    {
		    mysql_close(_3rd_conn.mysql_conn);
		    mysql_thread_end();
    }
EXIT_3:
    /* 关闭redis链接 */
    grd_disconnect(_3rd_conn.redis_conn);    
EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(_3rd_conn.psql_conn);    

EXIT_0:
    pthread_exit((void*)tid);
}
