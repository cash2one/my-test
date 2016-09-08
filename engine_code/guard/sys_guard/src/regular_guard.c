//#define _XOPEN_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include"gms_psql.h"
#include"errno.h"
#include"store_common.h"
#include"config.h"
#include"misc.h"
#include"regular_guard.h"
#include"file_path.h"
#include"make_log.h"
#include"sysstatus_file.h"

/*定期维护任务的个数*/
unsigned int regular_guard_num = 0;
static cfg_desc g_regular_num[] = {
	{"num", "total", &(regular_guard_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 1, 0, "0"},
	{"", "", NULL, 0, 0, 0, 0, ""},
};

int show_regular_guard_conf(regular_guard_conf_t *conf)
{
        unsigned int i = 0;

        for (i = 0; i < regular_guard_num; ++i) {

                printf("\n==============conf node ==========\n");
                printf("[title] %s\n", conf[i].title);
                printf("[name] %s\n", conf[i].name);
                printf("[type] %s\n", conf[i].type);
                printf("[interval] %s\n", conf[i].interval);
		printf("[time_column] %s\n", conf[i].time_column);
        }                                                                                                         
                                                                                                                  
        return 0;                                                                                                 
}                                              

int create_guard_cfg_array(cfg_desc* desc, regular_guard_conf_t *conf, unsigned int conf_num)
{
	unsigned int i = 0;
        int j = 0;
	/*title*/
	for (i = 0; i < conf_num; ++i) {                                                                          
                sprintf(conf[i].title, "%d", i+1);                                                     
        }       
	for (i = 0, j = 0; i < regular_guard_num; ++i) {
		/*name*/
		desc[j].entry_title = conf[i].title;                                                              
                desc[j].key_name = "name";                                                                    
                desc[j].addr = &(conf[i].name);                                                               
                desc[j].maxlen = MAXLEN;                                                                  
                desc[j].type = CFG_TYPE_STR;                                                                    
                desc[j].compulsive = 1;                                                                           
                desc[j].valid = 0;                                                                                
                desc[j].initial_value = "";                                                                      
                j++;               
		/*type*/
		desc[j].entry_title = conf[i].title;
                desc[j].key_name = "type";    
                desc[j].addr = &(conf[i].type);    
                desc[j].maxlen = MAXLEN;                                                                  
                desc[j].type = CFG_TYPE_STR;  
                desc[j].compulsive = 1;
                desc[j].valid = 0;
                desc[j].initial_value = ""; 
                j++;
		/*interval*/
		desc[j].entry_title = conf[i].title;
                desc[j].key_name = "interval";    
                desc[j].addr = &(conf[i].interval);    
                desc[j].maxlen = MAXLEN;                                                                  
                desc[j].type = CFG_TYPE_STR;  
                desc[j].compulsive = 1;
                desc[j].valid = 0;
                desc[j].initial_value = ""; 
                j++;
		/*time_column*/
		desc[j].entry_title = conf[i].title;                                                              
                desc[j].key_name = "time_column";                                                                    
                desc[j].addr = &(conf[i].time_column);                                                               
                desc[j].maxlen = MAXLEN;                                                                          
                desc[j].type = CFG_TYPE_STR;                                                                      
                desc[j].compulsive = 1;                                                                           
                desc[j].valid = 0;                                                                                
                desc[j].initial_value = "";                                                                       
                j++;               
	}
	/*NULL*/
	desc[j].entry_title = "";                                                                                 
        desc[j].key_name = "";                                                                                    
        desc[j].addr = NULL;                                                                                      
        desc[j].maxlen = 0;                                                                                       
        desc[j].type = 0;                                                                                         
        desc[j].compulsive = 0;                                                                                   
        desc[j].valid = 0;                                                                                        
        desc[j].initial_value = "";  
	return 0;
}
int get_sec(char *interval)
{
	char word[2] = {0};
	char time[3] = {0};
	char *p = interval;
	time_t sec;
	p += (int)strlen(interval) - 1;
	strncpy(word, p, 1);
	strncpy(time, interval, (int)strlen(interval)- 1);

	switch(word[0]) {
		case 'd':
			sec = (atoi(time) - 1) * ONE_DAY_SEC;
			break;
		case 'w':
			sec = (atoi(time) - 1) * ONE_WEEK_SEC;
			break;
		case 'm':
			sec = (atoi(time) - 1) * ONE_MONTH_SEC;
            printf("**************month_sec:**************%ld\n", sec);
            break;
		case 'y':
			sec = atoi(time) * ONE_YEAR_SEC;
            printf("**************year_sec:**************%ld\n", sec);
			break;
		default:
			break;
	}	

	return sec;
}
#if 0
int get_regular_guard_conf(void)
{
	int retn = 0;
	int seconds;
	unsigned int i = 0;
	char delete_time[TIME_FORMAT_SIZE] = {0};  
	retn = common_read_conf(GUARD_CONF_PATH, g_regular_num);
        if (retn < 0) {
                printf("Read config %s ERROR", GUARD_CONF_PATH);
                retn = -1;
                goto END;
        }
	cfg_desc * cfg_array = calloc(regular_guard_num*3 + 1, sizeof(cfg_desc));
        regular_guard_conf_t *f_guard_conf = calloc(regular_guard_num, sizeof(regular_guard_conf_t));
        create_guard_cfg_array(cfg_array, f_guard_conf, regular_guard_num);
	retn = common_read_conf(GUARD_CONF_PATH, cfg_array);
        if (retn < 0) {
               	printf("Read config %s ERROR", GUARD_CONF_PATH);
                retn = -1;
                goto END;
        }
	for(i = 0; i < regular_guard_num; i++) {
		if(strcmp(f_guard_conf[i].type, database) == 0) {
			seconds = get_sec(f_guard_conf[i].interval);
			get_delete_time(seconds, delete_time);
			delete_database(f_guard_conf[i].name);
		} else {
			delete_flie();
		}
	}
	//show_regular_guard_conf(f_guard_conf);
	
	free(cfg_array);                                                                                          
        free(f_guard_conf);

END:
	return retn;
}
#endif


int get_delete_time(int *sec, char *delete_time)
{
	struct tm* time_p = NULL;
        struct tm* ntime = NULL;
        time_t now_timestamp, delete_timestamp;
       // int one_day_sec = 3600 * 24;
        char time_format[] = "%Y-%m-%d %X";

        time(&now_timestamp);
        //printf("now_timestamp:%d\n", now_timestamp);
        time_p = localtime(&now_timestamp);
        time_p->tm_sec = time_p->tm_min = time_p->tm_hour = 0;
//	time_p->tm_hour = 1;
        delete_timestamp = mktime(time_p);
        delete_timestamp -= *sec;
        //delete_timestamp -= ONE_MONTH_SEC_30*4;
       // printf("delete_timestamp:%d\n", delete_timestamp);

        ntime = localtime(&delete_timestamp);
        strftime(delete_time, TIME_FORMAT_SIZE - 1, time_format, ntime);	
        printf("*************delete_time:%s\n", delete_time);
	return 0;	
}

int delete_from_database(PGconn* conn, int *sec, char *table_name, char *time)
{
	char delete_time[TIME_FORMAT_SIZE] = {0};
	char delete_cmd[CMD_LEN] = {0};
	/*
	char time_cmd[CMD_LEN] = {0};
	char count_cmd[CMD_LEN] = {0};
	char max_time[TIME_FORMAT_SIZE] = {0};
	int count = 0;
	*/
	//PGresult* res1 = NULL;
	get_delete_time(sec, delete_time);	
	snprintf(delete_cmd, CMD_LEN, "delete from %s where %s <= '%s'", table_name, time, delete_time);
    printf("delete_cmd:%s\n", delete_cmd);
	/*snprintf(count_cmd, CMD_LEN, "select count(*) from %s", table_name);
	strncpy(count, gpq_get_value(conn, count_cmd, 0, 0), 9);
	printf("-----------count:%s\n", count);
	*/
#if 0
	snprintf(time_cmd, CMD_LEN, "select max(%s) from %s", time, table_name);
	//gpq_get_value(conn, time_cmd, 0, 0);
	strncpy(max_time, gpq_get_value(conn, time_cmd, 0, 0), TIME_FORMAT_SIZE - 1);
                                                                     
	printf("++++++++++++max_time:%s\n", max_time);
	snprintf(count_cmd, CMD_LEN, "select * from %s", table_name);
	printf("count_cmd:%s\n", count_cmd);
	//strncpy(count, gpq_get_value(conn, count_cmd, 0, 0), 9);
	count = gpq_get_row(conn, count_cmd);
	printf("-----------count:%d\n", count);
#endif
	int retn = 0;
        retn = gpq_sql_cmd(conn, delete_cmd);                                                                     
                                                                                                                  
        if(retn < 0) {   
                fprintf(stderr, "[-][TEST_MAIN]conn db error\n");    
                retn = -1;                                             
                goto END;                                                                                        
        }
                                                                                                         
END:               
	/*断开数据库*/
        //gpq_disconnect(conn);
	return retn;

}
int get_delete_dirname(char *name, char *interval, char *dirname)
{
	int sub_time = 0;
	char word[2] = {0};
    char time[10] = {0};
    char *p = interval;
	char now_time_str[TIME_FORMAT_SIZE] = {0};
	char now_year[5] = {0};
	char now_month[3] = {0};
	char now_day[3] = {0};
    char now_sub_time[5] = {0};
	int t_time = 0, n_year = 0, n_month = 0, n_day = 0; 
    p += (int)strlen(interval) - 1;
    strncpy(word, p, 1);
    strncpy(time, interval, (int)strlen(interval)- 1);	
	t_time = atoi(time);

    get_last_time(now_time_str);
	//get_systime_str(now_time_str);
	split_time_str(now_time_str, now_year, now_month, now_day, NULL, NULL, NULL);	
	n_year = atoi(now_year);
	n_month = atoi(now_month);
	n_day = atoi(now_day);

	int i = 0;	
	switch(word[0]) {
		case 'd':
			sub_time = n_day - t_time;
			if(sub_time <= 0) {
				i = (-sub_time) / 30;
				n_month -= i + 1;
				sub_time += (i + 1) * 30; 		
			} 
           /* if(n_day >= 25) {
                if(n_month - 10 < 0) {
                    sprintf(now_month, "0%d", n_month);
                } else {
                    sprintf(now_month, "%d", n_month);
                }
            } else {
                if((n_month - 10) < 10) {
                    sprintf(now_month, "0%d", (n_month - 1));
                } else {
                    sprintf(now_month, "%d", (n_month - 1));
                }
            }*/
            if(sub_time - 10 < 0 ) {
                sprintf(now_sub_time, "0%d", sub_time);
            } else {
                sprintf(now_sub_time, "%d", sub_time);
            }
			sprintf(dirname, "%s/%s/%s/%s", name, now_year, now_month, now_sub_time);
			//sprintf(dirname, "%s/%s/%s", name, now_year, now_month);
			break;
		case 'w':
			sub_time = n_day - t_time * 7;
			if(sub_time <= 0) {
				i = (-sub_time) / 30;
				n_month -= i + 1;
				sub_time += (i + 1) * 30;
			}
            if(n_month - 10 < 0) {
                sprintf(now_month, "0%d", n_month);
            } else {
                sprintf(now_month, "%d", n_month);
            }
            if(sub_time - 10 < 0 ) {
                sprintf(now_sub_time, "0%d", sub_time);
            } else {
                sprintf(now_sub_time, "%d", sub_time);
            }   
			//sprintf(dirname, "%s/%s/%s/%s", name, now_year, now_month, now_sub_time);
			sprintf(dirname, "%s/%s/%s", name, now_year, now_month);
			break;
		case 'm':
			sub_time = n_month - t_time;
			if(sub_time <= 0) {
				i = (-sub_time) / 12;
				n_year -= i + 1;
				sub_time += (i + 1) * 12;
			 } 
            if(n_day >= 25) {
                if(sub_time - 10 < 0) {
                    sprintf(now_sub_time, "0%d", sub_time);
                } else {
                    sprintf(now_sub_time, "%d", sub_time);
                }
            } else {
                if((sub_time - 10) < 10) {
                    sprintf(now_sub_time, "0%d", (sub_time - 1));
                } else {
                    sprintf(now_sub_time, "%d", (sub_time - 1));
                }
            }
#if 0
            if(sub_time - 10 < 0 ) {
                sprintf(now_sub_time, "0%d", sub_time);
            } else {
                sprintf(now_sub_time, "%d", sub_time);
            }
#endif
			sprintf(dirname,"%s/%d/%s", name, n_year, now_sub_time); 
			break;
		case 'y':
			sub_time = n_year - t_time;
			sprintf(dirname, "%s/%d", name, sub_time);
			break;
		default:
			break;
	}	
	return 0;
}

int delete_wrong(char *dirname)
{
    int ret = 0;
    int retn = 0;
    FILE *fp = NULL;
    char wrong_time[TIME_FORMAT_SIZE];
    char sys_time[TIME_FORMAT_SIZE];
    //char time_format[] = "%Y-%m-%d %X";
    if(NULL == (fp = fopen(WRONG_TIME_PATH,"rb"))){                                                                
        printf("*********cannot open file! \n");   
        fclose(fp);                                                              
        return -1;
    } else {                                                                                                 
        fseek(fp,0,SEEK_END);                                                                             
        int file_size;                                                                                    
        file_size = ftell(fp);                                                                            
        printf("file_size:%d\n", file_size);                                                                             
        fseek(fp,0,SEEK_SET);                                                                             
        if (fread(wrong_time, file_size, sizeof(char),fp) != 1) {                                          
            if (feof(fp)) {                                                                           
                fclose(fp);                                                                       
            }                                                                                         
        } else {                                                                                          
            fclose(fp);                                                                               
        }
    get_systime_str(sys_time);                                                                                    
    retn = time_cmp(sys_time, wrong_time);  
    if(retn >= ONE_MONTH_SEC) {
        delete_dir(dirname);
        FILE *fp2 = NULL;                                                                                                 
        fp2 = fopen(WRONG_TIME_PATH, "w");                                                                         
        if( NULL == fp2){                                                                                         
                printf("cannot open[%s] file! \n", WRONG_TIME_PATH);                                                                   
                return -1;                                                                                        
        }                                                                                                         
        if(fwrite(sys_time, (int)strlen(sys_time), 1, fp2) != 1) {                                                
                                                                                                                  
                printf("file write error! \n");                                                                   
                fclose(fp2);                                                                                      
        } else {                                                                                                  
                fclose(fp2);                                                                                      
        }
        ret = 0;                 
    } else {
        CA_LOG(GUARD_MODULE, GUARD_PROC, "last wrong time is %s\n", wrong_time);    
        ret = -1;
        goto END;
    } 
}
END:
    return ret;
}

#if 0
int delete_from_disk(char *path)
{	
	int retn = 0;
	retn = remove(path);	
	if(retn < 0) {
		retn = -1;
		goto END;
	}
END:
	return retn;
}
#endif
/*
 * "0"表示所占内存没有超过限制的
 * "-1"表示超过
*/
int get_storage(char *name, char *interval, char *t_interval)
{
	char t_stor_cmd[CMD_LEN] = {0};
	char tmp_store[10] = {0};
	char tmp_store_num[5] = {0};
        char tmp_store_word[2] = {0};
	char limit_store[10] = {0};
	char limit_store_num[5] = {0};
        char *tmp_p = tmp_store;
	char *p = NULL;
	char *end = NULL;
	FILE *fp;
	int retn = 0;
	sprintf(t_stor_cmd, "du -sh %s | awk '{print $1}' > %s", name, TMP_STOR_PATH);
	sys_cmd(t_stor_cmd);
	
	if(NULL == (fp = fopen(TMP_STOR_PATH,"rb"))){
        printf("cannot open file! \n");
    } else {
        fseek(fp,0,SEEK_END);
        int file_size = 0;
        file_size = ftell(fp);
        fseek(fp,0,SEEK_SET);
        if (fread(tmp_store, file_size - 1,sizeof(char),fp) != 1) {
            if (feof(fp)) {
                fclose(fp);
            }
        } else {
            fclose(fp);
        }
        printf("tmp_store:%s\n", tmp_store);
	    remove(TMP_STOR_PATH);
        int t_len = 0;
        t_len = strlen(tmp_store);
        tmp_p += t_len - 1;
        strncpy(tmp_store_word, tmp_p, 1);                                                                        
        printf("tmp_store_word:%s\n", tmp_store_word);  

	    end = interval + (int)strlen(interval);
	    for(p = end; *p != '&'; p--);
	    memcpy(limit_store, p + 1, end - p);
	    int len = 0;
	    len = (int)strlen(limit_store);
	    limit_store[len] = '\0';
	    printf("limit_store:%s\n", limit_store);                                                                  
        strncpy(tmp_store_num, tmp_store, t_len - 1);                                                              
        printf("tmp_store_num:%f\n", atof(tmp_store_num));        
	    printf("*********************************\n");      
        strncpy(t_interval, interval, ((int)strlen(interval) - len - 2));
        printf("t_interval:%s\n", t_interval);                                          
        if(strcmp(tmp_store_word, "G") != 0) {                                                                    
                retn = 0;                                                                                         
                goto END;                                                                                         
        } else {                                                                                                  
                strncpy(tmp_store_num, tmp_store,t_len - 1);                                                      
                printf("tmp_store_num:%s\n", tmp_store_num);                                                      
                if(atof(limit_store_num) - atof(tmp_store_num) >= 0){                                             
                        retn = 0;                                                                                 
                        goto END;                                                                                 
                } else {                                                                                          
                        retn = -1;                                                                                
                        goto END;                                                                                 
                }                                                                                                 
        }    
}            
END:
	return retn;
}

int regular_guard(PGconn* conn)
{	
	//int guard_bit = 0;
        int retn = 0;           
	    int ret = 0;
        int db_ret = 0;                                                                                  
        int seconds;                                                                                              
        unsigned int i = 0;                                                                                       
        char delete_time[TIME_FORMAT_SIZE] = {0};
	    char dirname[CMD_LEN];                
        char t_interval[5] = {0};                                                 
     	retn = common_read_conf(guard_conf_cfg.regular_guard_path, g_regular_num);                                                  
        if (retn < 0) {                                                                                           
                printf("Read config %s ERROR", guard_conf_cfg.regular_guard_path);                                                  
                retn = -1;                                                                                        
                goto END;                                                                                         
        }                                                                                                         
        cfg_desc * cfg_array = calloc(regular_guard_num*4 + 1, sizeof(cfg_desc));                                 
        regular_guard_conf_t *f_guard_conf = calloc(regular_guard_num, sizeof(regular_guard_conf_t));             
        create_guard_cfg_array(cfg_array, f_guard_conf, regular_guard_num);                                       
        retn = common_read_conf(guard_conf_cfg.regular_guard_path, cfg_array);                                                      
        if (retn < 0) {                                                                                           
                printf("Read config %s ERROR", guard_conf_cfg.regular_guard_path);                                                  
                retn = -1;                                                                                        
                goto END;                                                                                         
        }                                                                                                         
        for(i = 0; i < regular_guard_num; i++) {                                                                  
                if(strcmp(f_guard_conf[i].type, "database") == 0) {                                                 
                        seconds = get_sec(f_guard_conf[i].interval);                                              
                        get_delete_time(&seconds, delete_time);                                                    
                        db_ret = delete_from_database(conn, &seconds, f_guard_conf[i].name, f_guard_conf[i].time_column);      
			            if(db_ret == 0) {
                            CA_LOG(GUARD_MODULE, GUARD_PROC, "delete from table:%s, before %s suc!\n", f_guard_conf[i].name, delete_time);			
                        } else if (db_ret < 0) {
                            CA_LOG(GUARD_MODULE, GUARD_PROC, "delete from table:%s, before %s faild!\n", f_guard_conf[i].name, delete_time);			
                        }     
                } else {
                    int wr_ret = 0;
                    if(strcmp(f_guard_conf[i].name, "/data/permdata/wrong") == 0 || strcmp(f_guard_conf[i].name, "/data/bak") == 0) {
                        wr_ret = delete_wrong(f_guard_conf[i].name);
                        if(wr_ret == 0) {
				            CA_LOG(GUARD_MODULE, GUARD_PROC, "delete %s suc!\n", f_guard_conf[i].name);
                        } 
                    } else {     
			            if((int)strlen(f_guard_conf[i].interval) > 5) {
				            ret = get_storage(f_guard_conf[i].name, f_guard_conf[i].interval, t_interval);		
				            if(ret == (-1)) {
					            delete_dir(f_guard_conf[i].name);
					            CA_LOG(GUARD_MODULE, GUARD_PROC, "storage is not enough, delete %s suc!\n", f_guard_conf[i].name);
				            } else {
					            get_delete_dirname(f_guard_conf[i].name, t_interval, dirname);
					            delete_dir(dirname);
					            CA_LOG(GUARD_MODULE, GUARD_PROC, "storage is enough, delete %s suc!\n", dirname);
				            }
			            } else {
				            get_delete_dirname(f_guard_conf[i].name, f_guard_conf[i].interval, dirname);
				            delete_dir(dirname); 
				            CA_LOG(GUARD_MODULE, GUARD_PROC, "delete %s suc!\n", dirname); 
			            }  
                }                                                                                
            }                                                                                                 
        }    
	char last_time[TIME_FORMAT_SIZE] = {0};
	get_systime_str(last_time);
	FILE *fp = NULL;
        fp = fopen(LAST_TIME_PATH, "w");
        if( NULL == fp){
                printf("cannot open file! \n");
                return -1;
        }
        if(fwrite(last_time, (int)strlen(last_time), 1, fp) != 1) {

                printf("file write error! \n");
                fclose(fp);
        } else {
                fclose(fp);
        }	
                      
END:
    
		free(cfg_array);
        free(f_guard_conf);
		return retn;
}
