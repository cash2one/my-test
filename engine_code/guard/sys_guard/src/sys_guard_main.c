#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include"report_stat.h"
#include"priority.h"
#include "gms_psql.h"
#include"store_common.h"
#include"table.h"
#include"sysstatus_file.h"
#include"errno.h"
#include"regular_guard.h"
#include"process_src.h"
#include"file_path.h"
#include"process_src.h"

int test_select_print_params_sql(PGconn* conn);
struct SYS_INFO sys_info[NUM];
struct SERVICE service[NO];
proc_src * l_proc_src;
proc_src * gen_proc_src;
int dest_num;
/*获取上次删除的时间*/
int get_last_time(char *last_time)
{   
	FILE *fp = NULL;
    int file_size = 0;                                                                                    
   	
	if(NULL == (fp = fopen(LAST_TIME_PATH,"rb"))){                                                            
        printf("*********cannot open file! \n");    
        fclose(fp);
        return -1;                                                           
    }                                                                                                 
    
	fseek(fp,0,SEEK_END);                                                                             
    file_size = ftell(fp);    
    printf("file_size:%d\n", file_size);                                                                        
	fseek(fp,0,SEEK_SET);                                                                             
    if (fread(last_time, file_size, sizeof(char), fp) != 1) {                                       
        if (feof(fp)) {                                                                           
            fclose(fp);                                                                       
        }                                                                                         
    } else {                                                                                          
        fclose(fp);                                                                               
    }
    return 0;
}
int get_last_time_one_sec(time_t *diff_time)
{
	time_t last_one_time, now_time;
    int retn = 0;
	struct tm *time_p = NULL;
	char last_time[TIME_FORMAT_SIZE];
    char sys_time[TIME_FORMAT_SIZE];
	char time_format[] = "%Y-%m-%d %X";
    
	get_last_time(last_time);
	printf("@@@@@@@@@@@@@@@last_time[%d]:%s\n", (int)strlen(last_time), last_time);
    get_systime_str(sys_time);
    retn = time_cmp(sys_time, last_time);
    if(retn <= 0) {
		FILE *fp2 = NULL;
        fp2 = fopen(LAST_TIME_PATH, "w");
        if( NULL == fp2){
        	printf("cannot open file! \n");
            return -1;
        }
        if(fwrite(sys_time, (int)strlen(sys_time), 1, fp2) != 1) {

                printf("file write error! \n");
                fclose(fp2);
        } else {
                fclose(fp2);
        }
        *diff_time = time(&now_time); 
    } else {          
    	printf("-------------------\n"); 
		last_one_time = get_time_sec_by_str(last_time, time_format);
		time_p = localtime(&last_one_time);
		time_p->tm_sec = time_p->tm_min = 0;
        time_p->tm_hour = 1;
		*diff_time = mktime(time_p);
    	printf("+++++++++++++++++diff_time:%ld\n", *diff_time);
    }
    return 0;
}

/*data的占用率超过90%，告警维护*/

int warning_guard(void)
{
    char now_time_str[TIME_FORMAT_SIZE] = {0};                                                                
    char now_year[5] = {0};                                                                                   
    char now_month[3] = {0};                                                                                  
    char now_day[3] = {0};                                                                                    
    char delete_flow[FILE_PATH_SIZE] = {0};                                                                   
    
	get_last_time(now_time_str);                                                                              
    split_time_str(now_time_str, now_year, now_month, now_day, NULL, NULL, NULL);                             
    sprintf(delete_flow, "%s/%s/", guard_conf_cfg.flow_path, now_year);                                         
    
	delete_dir(delete_flow);                                                                                  
    delete_dir(guard_conf_cfg.dns_path);                                                                      
    delete_dir(guard_conf_cfg.URL_path);                                                                      
    delete_dir(guard_conf_cfg.abb_online_path);                                                               
    delete_dir(guard_conf_cfg.abb_offline_path);                                                              
    delete_dir(guard_conf_cfg.mtd_path);                                                                      
    delete_dir(guard_conf_cfg.url_path);                                                                      
    delete_dir(guard_conf_cfg.virus_path);                                                                    
    delete_dir(guard_conf_cfg.wrong_path);     
    return 0;
}

int main(void)
{
	while(1) {
		printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
		int retnn = 0;                                                                                                 
		char cmd[100];
        int ret = 0;           
		char info[10], file_str[1024];
        
		retnn = common_read_conf(GUARD_CFG_PATH, g_guard_cfg);                                                     
   		if (retnn < 0) {                                                                                               
        	fprintf(stderr, "[-][GUARD_CONF] Read config %s ERROR!\n", GUARD_CFG_PATH);                                                 }
        memset(cmd, 0, 99);
        sprintf(cmd, "%s > %s", GEN_STATUS_PATH, SYS_STATUS_PATH);
		ret = sys_cmd(cmd);

		printf("**************sys_info************\n");
		read_sys_file(info);
		memset(file_str, 0, 1024);
		make_sysfile(file_str);
		//guard_process();

/*读取数据库配置文件*/

		int retn = 0;	
		retn = common_read_conf(guard_conf_cfg.pql_conf_path, pql_cfg);                                                      
    	if (retn < 0) {                                                                                               
        	fprintf(stderr, "[-][PQL_CONF] Read config %s ERROR!\n", PQL_CONF_PATH);                                                        
    	}      

/*连接数据库*/	
	#if 0
    struct gpq_conn_info conn_info;                                                                           
                                                                                                                  
        memset(&conn_info, 0, GPQ_CONN_INFO_SIZE);  
        strncpy(conn_info.host_ip, pql_conf.psql_ip, IP_STR_SIZE-1);                                               
        strncpy(conn_info.host_port, pql_conf.psql_port, PORT_STR_SIZE-1);                                                    
        strncpy(conn_info.dbname, pql_conf.psql_dbname, DBNAME_SIZE-1);                                                       
        strncpy(conn_info.user, pql_conf.psql_user, USER_NAME_SIZE-1);                                                    
        strncpy(conn_info.pwd, pql_conf.psql_pwd, PASSWORD_SIZE-1); 
        strncpy(conn_info.domain, pql_conf.psql_domain, FILE_PATH_SIZE-1); 
        #endif
        PGconn *conn = NULL;
        if(pql_conf.psql_unix == 1) {                                                                                           
            conn = gpq_connectdb_host(pql_conf.psql_domain,                                                          
                                                                 pql_conf.psql_dbname,                                
                                                                 pql_conf.psql_user,                                  
                                                                 pql_conf.psql_pwd);    
        } else {
            conn = gpq_connectdb(pql_conf.psql_ip,                                                       
                                                                 pql_conf.psql_port,                             
                                                                 pql_conf.psql_dbname,                                
                                                                 pql_conf.psql_user,                                  
                                                                 pql_conf.psql_pwd);                                  
        }                            
		if (conn == NULL) {                                                                                       
                fprintf(stderr, "[-][TEST_MAIN]conn db error\n");                                                 
                goto EXIT;                                                                                        
        }

/*执行定期维护任务*/
		time_t last_time_one, now_time;
    		char clear_cmd[1024] = {0};
	
		get_last_time_one_sec(&last_time_one);	
		time(&now_time);
		if(difftime(now_time, last_time_one) > ONE_DAY_SEC) {
			regular_guard(conn);
        	#if 1
			sprintf(clear_cmd, "%s", "vacuum analyze");
        		gpq_sql_cmd(conn, clear_cmd);
		#endif
		}
    
		int up_ret = 0;
		char libversion[20] = {0};
		char update_cmd[1024] = {0};
    	
		get_libversion(libversion);
    	sprintf(update_cmd, "update t_conf_curver set cur_feature_ver = '%s'", libversion);
    	up_ret = gpq_sql_cmd(conn, update_cmd); 
    	if (up_ret < 0) {
        	fprintf(stderr, "[-][GMS_PSQL]update t_conf_curver ERROR!\n");
        	up_ret = -1;
        	goto EXIT;
    	}
	EXIT:                                                                                                                   
        /*断开数据库*/                                                                                            
        
	gpq_disconnect(conn); 	

/*维护告警时执行维护任务*/
    FILE *fp = NULL;
    char data_stor_cmd[CMD_LEN] = {0};
    char data_store[3] = {0};
    char format[] = {"[ %]+"};
    
	sprintf(data_stor_cmd, "df -h | grep /data | awk -F '%s' '{print $5}' > %s", format, DATA_STOR_PATH);
    printf("data_stor_cmd:%s\n", data_stor_cmd);
    sys_cmd(data_stor_cmd);
    
    if(NULL == (fp = fopen(DATA_STOR_PATH,"rb"))){
        printf("cannot open file! \n");
        fclose(fp);
    } else {
        fseek(fp,0,SEEK_END);
        int file_size = 0;
        file_size = ftell(fp);
        fseek(fp,0,SEEK_SET);
        if (fread(data_store, file_size - 1,sizeof(char),fp) != 1) {
            if (feof(fp)) {
                fclose(fp);
            }
        } else {
            fclose(fp);
        }
        printf("data_store:%s\n", data_store);
        
        remove(DATA_STOR_PATH);    
        if((atoi(data_store) - 90) >= 0) {
	   warning_guard();
	   CA_LOG(GUARD_MODULE, GUARD_PROC, "data_store:%s\n", data_store);      
        } 
    }        
	sleep(60);
	}
	return 0;
}
