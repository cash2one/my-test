/**
 * @file flow_store.c
 * @brief  流量存储流程 
 * @author zhang_dawei <zhangdawei@chanct.com>
 * @version 2.0
 * @date 2015-03-04
 */

#include "gms_flow.h"
#include "gms_udp_sock.h"
pthread_mutex_t mutex_flow_event=PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex_flow_tmp_event=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_flow_tmp2_event=PTHREAD_MUTEX_INITIALIZER;
/* 流量批量插入命令 */
char g_event_copy_cmd[400];
/**"Mozilla/"*/
char Mozilla[50][10]={"MSIE","Netscape","Opera","Safari","Firefox","Galeon","Epiphany","Konqueror","Debian",""};
char Mozi_brow[50][10]={"IE","Netscape","Opera","Safari","Firefox","Galeon","Epiphany","Konqueror","Debian",""};
/****"Opera/"*******/
/*"Avant Browser/"**/
/*"Googlebot/"*/
/*"wget/"*/
/***"Msnbot/"******/
/*****"ELinks/"****************/
/*****"Links/"*******/
char browbuf[10][15]={"Opera/","Avant Browser/","Googlebot/","Wget/","Msnbot/","ELinks/","Links/",""};
extern uint32_t jenkins_one_at_a_time_hash(char *key, size_t len);
int mystrcmp(char *src,char*match,char*vernum,unsigned int len,unsigned int flag)
{
	int ret;
	unsigned int i,j;
	char *ptr=src;
	char *pch=match;
	for(i=0;i<strlen(ptr);i++)
	{
		if(ptr[i]==pch[0])
		{
			if(!strncmp(&ptr[i],pch,strlen(pch)))
			{
				if(vernum)
				{
					ret=0;
					for(j=i+strlen(pch)+flag;(ptr[j]!=' ')&&(ptr[j]!=';')&&((j-(i+strlen(pch))+flag)<len)&&(ptr[j]!='\0')&&(ptr[j]!='\n')&&(ptr[j]!='\r');j++)
					{
						if((ptr[j]=='.')&&(ret==1))
							return 0;
						vernum[j-(i+strlen(pch))-flag]=ptr[j];
						if(ptr[j]=='.')
							ret=1;
					}
				}
				return 0;
			}

		}
	}
	return -1;
}	
int detch_ver(char *src,char *browser,char *vernum)
{
	char * text = NULL;
//	char * pattern = NULL;
//   char *next = NULL;
	int ret;
	unsigned int i;
	char ver[10]={0};
	if(!strcmp(src,""))
	{
		return -1;
	}
	else
	{
		text =src ;
	}
	if(!mystrcmp(text,"Mozilla/",NULL,0,0))
	{
		for(i=0;i<sizeof(Mozilla)/sizeof(Mozilla[0])&&(strlen(browbuf[i])>0);i++)
		{
			ret=0;
			if(!mystrcmp(text,Mozilla[i],ver,10,1))
			{
				ret=1;
				break;
			}
		}
		if(ret==1)
		{
			//printf("browser=%s   ver=%s\n",Mozi_brow[i],ver);
			strncpy(browser,Mozi_brow[i],strlen(Mozi_brow[i]));
			strncpy(vernum,ver,strlen(ver));
		}
		else
		{
			strncpy(browser,"not fonud",strlen("not fonud"));
			strncpy(vernum,"0",2);
		}
	}
	else
	{
		for(i=0;i<sizeof(browbuf)/sizeof(browbuf[0])&&(strlen(browbuf[i])>0);i++)
		{
			ret=0;
			if(!mystrcmp(text,browbuf[i],ver,10,0))
			{
				ret=1;
				strncpy(browser,browbuf[i],strlen(browbuf[i])-1);
				strncpy(vernum,ver,strlen(ver)+1);
				//printf("browser=%s  ver=%s\n",buf,ver);
				//printf("sucess....\n");
				break;

			}
		}
		if(ret==0)
		{
			strncpy(browser,"not fonud",strlen("not fonud"));
			strncpy(vernum,"0",2);
		
		}

	}
	return 0;
}
static int store_postgresql_event(flow_conn_t *conn, file_data_t *file, char *buf, unsigned long buf_size)
{
    int retn = 0;
    char table_name[TABLE_NAME_SIZE];

    /* 根据月份选择分区表 */
    memset(table_name, 0 , TABLE_NAME_SIZE);
    strcat(table_name, EVENT_TABLE_NAME);
#if 0
    strncat(table_name, TABLE_SUFFIX, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);
#endif

    init_event_copy_cmd(g_event_copy_cmd, 400, table_name);
    printf("file is [%s],buf is [%s]\n",file,buf);

    /* 执行批量插入命令到MINU表 */
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_event_copy_cmd, buf, buf_size);
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
		if (remove(file->path) != 0) {
			perror("remove file");
			FLOW_DEBUG(DEBUG_TYPE_STORE, "remove file %s error!\n", file->path);
			CA_LOG(LOG_MODULE, LOG_PROC, "remove file %s error!\n", file->path);
			/* TODO XXX 挂起 */
		}

#if 0
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
            move_file_to_other_path(file->path, g_flow_info_cfg.event_dst_path, file->file_name);
        }
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
static int inject_event_data(void *data, void *arg)
{
	/* 流量数据缓冲 */
	char g_event_buf[g_flow_debug_cfg.flowevent_buf_len];
    file_data_t* file = NULL;    
    flow_conn_t* conn = NULL;
    long buf_size = 0;
    unsigned int line = 0;
    int retn = 0;

    file = (file_data_t*)data;
    conn = (flow_conn_t*)arg;
    memset(g_event_buf, 0, g_flow_debug_cfg.flowevent_buf_len);

    FLOW_DEBUG(DEBUG_TYPE_STORE, "[file_path]:%s,[file_name]:%s, [year]:%s, [month]:%s, [day]:%s\n", file->path, file->file_name, file->year, file->month, file->day);

    /*  将文件数据读到内存缓冲中 */
    buf_size = read_file_to_data(file->path, g_event_buf);
    if (buf_size < 0) {
        fprintf(stderr, "%s null!\n", file->path); 
        goto END;
    }
    /*  得到数据行数 */
    line = get_event_num(g_event_buf, buf_size);    
    //FLOW_DEBUG(DEBUG_TYPE_STORE, "line = %d\n", line);
    if (line == 0) {
       // FLOW_ERROR(DEBUG_TYPE_STORE, "***ERROR*** %s line is ZERO!!!\n", file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "***ERROR*** %s line is ZERO!!!\n", file->path);
        move_file_to_other_path(file->path, g_flow_info_cfg.event_wrong_path, file->file_name);
        goto END;
    }

    //FLOW_DEBUG(DEBUG_TYPE_STORE, "[%s] SIZE:%d\n", file->path, buf_size);

    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_event(conn, file, g_event_buf, buf_size);    
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        //FLOW_ERROR(DEBUG_TYPE_STORE, "***ERROR*** postgresql store error [%s] \n", file->path);
        CA_LOG(LOG_MODULE, LOG_PROC, "***ERROR*** postgresql store error [%s] \n", file->path);
        move_file_to_other_path(file->path, g_flow_info_cfg.event_wrong_path, file->file_name);
        goto END;
    }

#if 0
    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_flow(conn, g_event_buf, buf_size, line);

    if (IS_FLOW_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        FLOW_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buf_size, line, test_time.sec);
    }
#endif
END:
    return 0;

}


static int do_event_store(flow_conn_t *conn)
{
    int retn = 0;
    int i = 0;
    int list_num = 0;
    file_data_t *tmp_rm_file = NULL;

    /* 遍历 flow 目录 寻找是否有新数据 */
    retn = dirwalk_current_list(g_flow_info_cfg.event_src_path, conn->list);
    if (retn < 0) {
        //FLOW_ERROR(DEBUG_TYPE_STORE, "travel flow dir error!\n");
        goto EXIT;
    }
    list_num = conn->list->node_num;

    if (list_num > 0) {
        /* 依次解析得到的数据文件, 插入PostgreSQL 和 redis  */
        list_iterate(conn->list, LIST_FORWARD, inject_event_data, (void*)conn);
    }


    /* 清空链表 */
    for (i = 0; i < list_num; ++i) {
        tmp_rm_file = list_pop(conn->list);
        free(tmp_rm_file);
    }

EXIT:
    return retn;
}
static int store_postgresql_event_bak(flow_conn_t *conn, file_data_t *file, char *buf, unsigned long buf_size,char *event_file,char *tmp_file,short flag)
{
    int retn = 0;
    char table_name[TABLE_NAME_SIZE];

    /* 根据月份选择分区表 */
    memset(table_name, 0 , TABLE_NAME_SIZE);
#if 0
	if(flag==1)
    strcat(table_name, TIME_OUT_TABLE_NAME);/*t_flow_timeout*/
	else
#endif
    strcat(table_name, EVENT_TABLE_NAME);/*t_flow_event*/
#if 0
    strncat(table_name, TABLE_SUFFIX_MONTH, TABLE_SUFFIX_SIZ);
    strcat(table_name, file->month);
#endif
	pthread_mutex_lock(&mutex_flow_event);

    init_event_copy_cmd(g_event_copy_cmd, 400, table_name);
    //printf("file is [%s],buf is [%s]\n",event_file,buf);

    /* 执行批量插入命令到MINU表 */
	//pthread_mutex_lock(&mutex_flow_event);
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_event_copy_cmd, buf, buf_size);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "copy to %s error!\n", table_name);
        /* 将文件移到错误目录下 */
    }
    else {
        /* 将文件移动到通讯目录中 */
        printf("^^^^^^^^^^copy %s succ!^^^^^^^^^^\n",table_name);
		printf("^^^^^^^^^^insert postgresql succeed !!!!!^^^^^^^^^^\n");
       // CA_LOG(LOG_MODULE, LOG_PROC, "Postgresql Database --> copy file[%s] -> table[%s] succ!\n", event_file, table_name);

            //move_file_to_other_path(file->path, g_flow_info_cfg.dst_file_path, file->file_name);
		if(g_flow_debug_cfg.flowevent_file == 1)
		{
		 make_flow_file_bak(buf,event_file,tmp_file);
		}
    }
	//usleep(500000);
	//pthread_mutex_unlock(&mutex_flow_event);
	pthread_mutex_unlock(&mutex_flow_event);

    return retn;
}
static int inject_event_data_bak(void *data, void *arg,char *event_file,char *tmp_file,short flag,file_data_t *file)
{
    char* buf =(char *)data;    
    flow_conn_t* conn = (flow_conn_t*)arg;
    long buf_size = strlen(buf);
//    unsigned int line = 0;
    int retn = 0;
#if 0
    /*  得到数据行数 */
    line = get_event_num(buf, buf_size);    
    if (line == 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "***ERROR*** %s line is ZERO!!!\n", file->path);
        goto END;
    }
#endif
	printf("file tmp %s ,buf is [%s]\n",tmp_file,buf);

    /* --------------- postgres 入库 ---------------*/    
    retn = store_postgresql_event_bak(conn, file, buf, buf_size,event_file,tmp_file,flag);    
    if (retn < 0) {
        /* postgres 入库失败 redis 无需入库 */
        CA_LOG(LOG_MODULE, LOG_PROC, "***ERROR*** postgresql store error [%s] \n", tmp_file);
        goto END;
    }

#if 0
    /* ----------------- reids 入库 ------------------  */
    retn = store_redis_flow(conn, g_event_buf, buf_size, line);

    if (IS_FLOW_SPENDTIME_DEBUG_ON) {
        TEST_END(test_time);
        FLOW_DEBUG(DEBUG_TYPE_SPENDTIME, "\n===> File:[%s], Size:[%d], lines:[%d]\n===> Spend:[%f] sec.\n",file->path, buf_size, line, test_time.sec);
    }
#endif
END:
    return retn;

}
int ipcmd(char *src_str)
{
	int j=0;
	unsigned int i = 0;
	char ipcut[17]={0};
	if(!strcmp(g_flow_info_cfg.ip_str,""))
		return 1;
	for(i=0;i<=strlen(g_flow_info_cfg.ip_str);i++)
	{
		if(g_flow_info_cfg.ip_str[i]==';'||g_flow_info_cfg.ip_str[i]=='\0')
		{
			if(!strncmp(ipcut,src_str,16))
				return 1;
			i++;
			j=0;
			memset(ipcut,0,sizeof(ipcut));
		}
		ipcut[j]=g_flow_info_cfg.ip_str[i];
		j++;
	}
	return 0;
}
int do_net_postgresql(flow_conn_t *conn,char *buf,int hash_id)
{
	char g_net_copy_cmd[400];
	char table_name[TABLE_NAME_SIZE];
	int buf_size=strlen(buf);
	int retn=0;
	

	if (g_flow_debug_cfg.flow_audit_switch == 0)
	{
		return 0;
	}


	memset(table_name, 0 , TABLE_NAME_SIZE);
	//strcat(table_name, NET_TABLE_NAME);
 	snprintf(table_name,TABLE_NAME_SIZE,"%s_%d",NET_TABLE_NAME,hash_id);	
	init_net_copy_cmd(g_net_copy_cmd, 400, table_name);
    retn = gpq_copy_from_buf_sql(conn->psql_conn, table_name, g_net_copy_cmd, buf, buf_size);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "copy net to %s error!\n", table_name);
        /* 将文件移到错误目录下 */
        //move_file_to_other_path(file->path, g_flow_info_cfg.wrong_path, file->file_name);
    }
    else {
        /* 将文件移动到通讯目录中 */
        printf("^^^^^^^^^^copy net %s succ!^^^^^^^^^^\n",table_name);
        //CA_LOG(LOG_MODULE, LOG_PROC, "copy net to %s postgresql succ ...!\n", table_name);
		}

	return retn;
}
long ip_find_id(PGconn *conn,unsigned int userip,char *buf)
{
	char *p=NULL;
	char user_id[5]={0};	
	char sql_cmd[100]={0};
	snprintf(sql_cmd,100,"select user_name,user_id from t_net_id where user_ip=%u limit 1;",userip);
	/*find id use ip*/
//p=gms_get_account(userip);
//gpq_get_row_col_value(PGconn *conn, char *sql_cmd, int row, int col,char *result_tmp);
    memset(buf,0,32);
    p=gpq_get_row_col_value(conn,sql_cmd,buf,user_id);
    //CA_LOG(LOG_MODULE, LOG_PROC, "mmmmmmmmmmmmmmmmmmmmm user_name=[%s] user_id[%s]\n", buf,user_id);
	
	if (NULL == p)
	{
		return -1;
	}
	else
	{
		//snprintf(buf,32,"%s",p);
		/*当账户为数字串时，作如下操作，如果为ascii，就不能运行*/
		return atoi(user_id);	
	}
}
int do_net_store(flow_conn_t *conn,flow_event *store_buf,int num)
{
	int n=0;
	int i;
	int hash_id=0;
	long up_bytes;
	long down_bytes;
	char flow_buf[g_flow_debug_cfg.flowevent_buf_len];
	flow_event flow_st;
	char userip[16]={0};
	char flow_buftmp[EVENT_SIZE];
	//file_data_t file[1];
	struct in_addr addr;
	//long user_id;
	char user_name[32];
	memset(flow_buf,0,g_flow_debug_cfg.flowevent_buf_len);
#if 1
	for(i = 0 ; i < num && store_buf[i].ftime_start != 0;i++)
	{
		bzero(flow_buftmp,EVENT_SIZE);
		memset(&addr,0,sizeof(struct in_addr));
		memset(userip,0,sizeof(userip));
		flow_st=store_buf[i];
		if ( flow_st.direction_flag == flow_in ){
			addr.s_addr=flow_st.dip;
			strncpy(userip,inet_ntoa(addr),16);
			up_bytes = flow_st.down_bytes;
			down_bytes = flow_st.up_bytes;
		}else{
			addr.s_addr=flow_st.sip;
			strncpy(userip,inet_ntoa(addr),16);
			up_bytes = flow_st.up_bytes;
			down_bytes = flow_st.down_bytes;
		}
		//user_id=ip_find_id(ntohl(addr.s_addr),user_name);
		hash_id=ip_find_id(conn->psql_conn,ntohl(addr.s_addr),user_name);
		if (hash_id == -1)
		{
        //CA_LOG(LOG_MODULE, LOG_PROC, "user_id = 00000000000000000000000000000000000000000000");
		continue;
		}
        //CA_LOG(LOG_MODULE, LOG_PROC, "user_id = 1111111111111111111111111111111111111111111111111");
		//hash_id = jenkins_one_at_a_time_hash(user_name,4);

		snprintf(flow_buftmp,EVENT_SIZE,"%s|%ld|%ld|%u|%u|%u|%u|%ld|%ld|%s|%s|%d\n",g_flow_info_cfg.dev_id,
				flow_st.ftime_start,
				flow_st.ftime_end,
				flow_st.pc_if,
				flow_st.service_group_id,
				flow_st.service_type_id,
				flow_st.procotol_id,
				up_bytes,
				down_bytes,
				userip,
				user_name,
				hash_id
					);
		//strncat(flow_buf,flow_buftmp,EVENT_SIZE);
	n = do_net_postgresql(conn,flow_buftmp,hash_id);
	}
	//n = do_net_postgresql(conn,flow_buf);
#if 0
	if(!strcmp(flow_buf,""))
		goto EXIT;
	strptime(start_time, time_format, &time1);
	snprintf(file->month,3,"%d",time1.tm_mon+1);
	snprintf(file->day,3,"%d",time1.tm_mday);


	strftime(file_time, TIME_FORMAT_SIZE-1, time_format_dst, &time1);

	strcpy(flow_file, g_flow_info_cfg.event_dst_path);
	strncat(flow_file, "/", 2);
	strcpy(flow_tmp_file, g_flow_info_cfg.event_dst_path);
	strncat(flow_tmp_file, "/", 2);
	strcat(flow_file, file_time);
	strncat(flow_file, "_", 2);
	strcat(flow_file, g_flow_info_cfg.dev_id);
	strcat(flow_tmp_file, file_time);
	strncat(flow_tmp_file, "_", 2);
	strcat(flow_tmp_file, g_flow_info_cfg.dev_id);

	strcat(flow_file, ".ok");
	strcat(flow_tmp_file, ".ok.writing");
	//printf("\nflow tmp file :[%s], flow_file is [%s]\n", flow_tmp_file, flow_file);

	n=inject_event_data_bak(flow_buf,(void*)conn,flow_file,flow_tmp_file,flow_st.flag,file);
#endif
#endif

EXIT:
    return n;
}

int do_event_store_bak(flow_conn_t *conn,flow_event *store_buf,int num)
{
//	struct sockaddr_in addr_tmp;
	int n=0;
	char flow_buf[g_flow_debug_cfg.flowevent_buf_len];
	char time_format[]="%Y-%m-%d %X";
	const char *time_format_dst = "%Y%m%d-%H%M%S"; 
	char file_time[TIME_FORMAT_SIZE] = {0};
	char start_time[TIME_FORMAT_SIZE] = {0};
	char end_time[TIME_FORMAT_SIZE] = {0};
	char sip[16]={0};
	char dip[16]={0};
	struct tm time1;
	struct tm timetmp[1]={0};
	struct in_addr addr;
	char flow_file[FILENAME_MAX];
	char flow_tmp_file[FILENAME_MAX];
	char flow_buftmp[EVENT_SIZE];
	int i;
	flow_event flow_st;
//	socklen_t addr_len = sizeof(struct sockaddr_in);
	file_data_t file[1];
	if (g_flow_debug_cfg.flow_analyze == 0)
	{
		return 0;
	}

#if 1
	for(i=0;i< num && store_buf[i].ftime_start != 0;i++)
	{
//	pthread_mutex_lock(&mutex_flow_tmp2_event);
		bzero(flow_buftmp,EVENT_SIZE);
		memset(&addr,0,sizeof(struct in_addr));
		memset(sip,0,sizeof(sip));
		memset(dip,0,sizeof(dip));
		flow_st=store_buf[i];
		addr.s_addr=flow_st.sip;
		strncpy(sip,inet_ntoa(addr),16);
		addr.s_addr=flow_st.dip;
		strncpy(dip,inet_ntoa(addr),16);
		//detch_ver(flow_st.browser,browser,vernum);
#if 0
		if(!(ipcmd(sip)||ipcmd(dip)))
			continue;
#endif
		memset(start_time,0,TIME_FORMAT_SIZE);
		memset(end_time,0,TIME_FORMAT_SIZE);
//	pthread_mutex_lock(&mutex_flow_tmp2_event);
		localtime_r(&flow_st.ftime_start,timetmp);
		strftime(start_time, TIME_FORMAT_SIZE-1, time_format, timetmp);
		
//	pthread_mutex_unlock(&mutex_flow_tmp2_event);
//	pthread_mutex_lock(&mutex_flow_tmp2_event);
		localtime_r(&flow_st.ftime_end,timetmp);
		strftime(end_time, TIME_FORMAT_SIZE-1, time_format, timetmp);
//	pthread_mutex_unlock(&mutex_flow_tmp2_event);
		snprintf(flow_buftmp,EVENT_SIZE,"%s|%s|%s|%u|%s|%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%llu|%llu|%u|%d|%s\n",g_flow_info_cfg.dev_id,
				start_time,
				end_time,
				flow_st.visit_time,
				sip,
				dip,
				flow_st.usersport,
				flow_st.userdport,
				flow_st.service_type_id,
				flow_st.country,
				flow_st.pc_if,
				flow_st.tcp_suc,
				flow_st.bank_visit,
				flow_st.flag,
				flow_st.incr,
				0,
				flow_st.s_isp_id,
				flow_st.d_isp_id,
				flow_st.s_province_id,
				flow_st.d_province_id,
				flow_st.s_city_id,
				flow_st.d_city_id,
				flow_st.ip_byte,
				flow_st.ip_pkt,
				flow_st.cli_ser,
				flow_st.browser,
				flow_st.version
					);
		//printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&rev=%s\n",flow_buftmp);
		strncat(flow_buf,flow_buftmp,EVENT_SIZE);
//	pthread_mutex_unlock(&mutex_flow_tmp2_event);
	}
#if 1
	if(!strcmp(flow_buf,""))
		goto EXIT;
	strptime(start_time, time_format, &time1);
	snprintf(file->month,3,"%d",time1.tm_mon+1);
	snprintf(file->day,3,"%d",time1.tm_mday);


	strftime(file_time, TIME_FORMAT_SIZE-1, time_format_dst, &time1);

	strcpy(flow_file, g_flow_info_cfg.event_dst_path);
	strncat(flow_file, "/", 2);
	strcpy(flow_tmp_file, g_flow_info_cfg.event_dst_path);
	strncat(flow_tmp_file, "/", 2);
	strcat(flow_file, file_time);
	strncat(flow_file, "_", 2);
	strcat(flow_file, g_flow_info_cfg.dev_id);
	strcat(flow_tmp_file, file_time);
	strncat(flow_tmp_file, "_", 2);
	strcat(flow_tmp_file, g_flow_info_cfg.dev_id);

	strcat(flow_file, ".ok");
	strcat(flow_tmp_file, ".ok.writing");
	//printf("\nflow tmp file :[%s], flow_file is [%s]\n", flow_tmp_file, flow_file);

	n=inject_event_data_bak(flow_buf,(void*)conn,flow_file,flow_tmp_file,flow_st.flag,file);
#endif
#endif

EXIT:
    return n;
}


void* flow_store_event_busi(void *arg)
{
	sleep(1);
	int ret=0;
    pthread_t tid;
    tid = pthread_self();

    if (g_flow_debug_cfg.store_enable == 0) {
        fprintf(stderr, "STORE_ENABLE = NO!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "STORE_ENABLE = NO!");
        goto EXIT;
    }
	if (g_flow_debug_cfg.flowevent_store_disenable == 1) {
        fprintf(stderr, "FLOWEVENT_STORE_DISENABLE = YES!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "FLOWEVENT_STORE_DISENABLE = YES!");
        goto EXIT;
    }
	ret=read_comm_interface_conf(g_flow_info_cfg.comm_conf_path, g_flow_info_cfg.is_comm_monitor);
	if(ret<0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "Parser COMM XML FILE ERROR!!!!");
		goto EXIT;
	}
	if (strncmp(g_flow_info_cfg.is_comm_monitor, "1", 2) != 0) {
		printf("dev is not monitor Do not revice UDP packs!\n");
		goto EXIT;
	}

    flow_conn_t flow_conn;
    list_t flow_list;
    int retn = 0;
    int retdb = 0;
    int count = 0;


    /* ------------- 初始化配置信息 ------------------- */
    /* 1. 初始化链表 */
    list_init(&(flow_list), file_data_t);
    flow_conn.list = &flow_list;
    //list_init(&(g_redis_flow_key_list), f_redis_key_t);
    //flow_conn.policy_list = &g_f_policy_list;

    retn = flow_connect_db(&flow_conn);
    if (retn == -1) {
        goto EXIT; 
    }
    else if (retn == 1) {
        goto EXIT_1;
    }

    while (1) {

        //retdb=do_event_store_bak(&flow_conn);
        retdb=0;
#if 1
       // usleep(40000);
       // ++count;
       // if(count >= NUM*10) {
       if(retdb < 0) {
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
#endif
    }

    /* 关闭redis链接 */
    grd_disconnect(flow_conn.redis_conn);    
EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(flow_conn.psql_conn);    

EXIT:
    pthread_exit((void*)tid);
}
void* flow_store_event_bak_busi(void *arg)
{
	int ret=0;
    pthread_t tid;
    tid = pthread_self();

    if (g_flow_debug_cfg.store_enable == 0) {
        fprintf(stderr, "STORE_ENABLE = NO!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "STORE_ENABLE = NO!");
        goto EXIT;
    }
	if (g_flow_debug_cfg.flowevent_store_disenable == 1) {
        fprintf(stderr, "FLOWEVENT_STORE_DISENABLE = YES\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "FLOWEVENT_STORE_DISENABLE = YES!");
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
    //list_init(&(g_redis_flow_key_list), f_redis_key_t);
    //flow_conn.policy_list = &g_f_policy_list;

    retn = flow_connect_db(&flow_conn);
    if (retn == -1) {
        goto EXIT; 
    }
    else if (retn == 1) {
        goto EXIT_1;
    }

    while (1) {

        do_event_store(&flow_conn);
	#if 1
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
	#endif
    }

    /* 关闭redis链接 */
    grd_disconnect(flow_conn.redis_conn);    
EXIT_1:
    /* 关闭postgreSQL链接 */
    gpq_disconnect(flow_conn.psql_conn);    

EXIT:
    pthread_exit((void*)tid);
}


