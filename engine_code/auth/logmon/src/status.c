#include"logmon.h"
#include"authfile_info.h"
#include"dump.h"
#include"auth_cfg.h"
#include"make_log.h"
#include<time.h>
#include<sys/stat.h>
char process_name[MAX_PROG_NUM][MAX_PROG_NAME_LEN];
/**                                                                                                               
 * *  purpose: check if the process name include in cmdline file                                                     
 * *  input:                                                                                                         
 * *       file_name       cmdline filename                                                                          
 * *       name            process name                                                                              
 * *  return:                                                                                                        
 * *       equal           in cmdline file                                                                           
 * *       not_equal       not in cmdline file                                                                       
 * *
 * */               

int check_proc_name(char * file_name, char * name)                                                                    
{                                                                                                                 
	FILE *fp = NULL;                                                                                          
                                                                                                                  
    char buf[MAX_BUF_LEN];                                                                                    
    char comm_buf[MAX_BUF_LEN];                                                                                    
	memset(comm_buf, 0, MAX_BUF_LEN);
                                                                                                                  
    if ( NULL == file_name || '\0' == file_name[0] ||                                                         
        NULL == name || '\0' == name[0] ) {                                                       
        goto NOT_EQUAL;                                                                                   
    }                                                                                                         
                                                                                                                  
    if ( NULL == ( fp = fopen(file_name, "r") ) ) {                                                           
        DUMP("open file: %s failed,errno:%d(%s)\n",                                                       
                        file_name, errno, strerror(errno));                                               
        goto NOT_EQUAL;                                                                                   
    }                                                                                                         
                                                                                                                  
    //if ( NULL ==  fgets(buf, MAX_BUF_LEN, fp) ) {                                                             
    if ( 0 !=  fread(buf, (size_t) MAX_BUF_LEN, (size_t)1, fp)) {                                                             
		printf("not equal : buf %s\n", buf);
        goto NOT_EQUAL;                                                                                   
    } else {                                                                                                  
        char *p = NULL; 
		char *head = NULL;                                                                                   
		char *q = NULL;
                
        if(strcmp(buf, "java") == 0) {
			
            for(head = buf, q = buf; *q != '\0'; ++q);
			q++;
			for(head = q; *q != '\0'; ++q);
            q++;
            for(head = q; *q != '\0'; ++q);
			memcpy(comm_buf, head, q - head);

			if ( NULL != (p = strrchr(comm_buf,'/') ) ) {
                p++;
                if ( 0 == strncasecmp(p, name, strlen(name)) ) {
				    printf("upgrade:%s\n", comm_buf);
                    goto EQUAL;
                } else {
                    goto NOT_EQUAL;
                }
            } else {
                if ( 0 == strncasecmp(comm_buf, name, strlen(name)) ) {
					printf("upgrade:%s\n", comm_buf);
                    goto EQUAL;
                } else {
                    goto NOT_EQUAL;
                }
            }
		} else if(strcmp(buf, "/nmsmw/python-2.6.8/bin/python") == 0) {
			for(head = buf, q = buf; *q != '\0'; ++q);
			q++;
			for(head = q; *q != '\0'; ++q);
			memcpy(comm_buf, head, q - head);
			if ( NULL != (p = strrchr(comm_buf,'/') ) ) {
                p++;
                if ( 0 == strncasecmp(p, name, strlen(name)) ) {
					printf("event_export:%s\n", comm_buf);
                    goto EQUAL;
                } else {
                    goto NOT_EQUAL;
                }
            } else {
                if ( 0 == strncasecmp(comm_buf, name, strlen(name)) ) {
					printf("event_export:%s\n", comm_buf);
                    goto EQUAL;
                } else {
                    goto NOT_EQUAL;
                }
            }
		} else if(strcmp(buf, "python") == 0) {
			for(head = buf, q = buf; *q != '\0'; ++q);
			q++;
			for(head = q; *q != '\0'; ++q);
			memcpy(comm_buf, head, q - head);
			if ( NULL != (p = strrchr(comm_buf,'/') ) ) {
                p++;
                if ( 0 == strncasecmp(p, name, strlen(name)) ) {
					printf("comm_buf:%s\n", comm_buf);
                    goto EQUAL;
                } else {
                    goto NOT_EQUAL;
                }
            } else {
                if ( 0 == strncasecmp(comm_buf, name, strlen(name)) ) {
					printf("comm_buf:%s\n", comm_buf);
                    goto EQUAL;
                } else {
                    goto NOT_EQUAL;
                }
            }
		} else {
			if ( NULL != (p = strrchr(buf,'/') ) ) {                                                          
                p++;                                                                                      
                                                                                                                  
                if ( 0 == strncasecmp(p, name, strlen(name)) ) {                                          
                    goto EQUAL;                                                                       
                } else {                                                                                  
                    goto NOT_EQUAL;                                                                   
                } 
			} else {                                                                                          
                if ( 0 == strncasecmp(buf, name, strlen(name)) ) {                                        
                    goto EQUAL;                                                                       
                } else {                                                                                  
                    goto NOT_EQUAL;                                                                   
                }
			}                                                                                                 
        }                                                                                                         
    }                            
                                                                                                                  
EQUAL:                                                                                                            
        if ( NULL != fp ) {                                                                                       
                fclose(fp);                                                                                       
        }                                                                                                         
	printf("========EQUAL========buf : %s\n proc_name: %s\n file : %s\n", buf, name, file_name);
                                                                                                                  
        return NAME_EQUAL;                                                                                        
                                                                                                                  
NOT_EQUAL:                                                                                                        
        if ( NULL != fp ) {                                                                                       
                fclose(fp);                                                                                       
        }                                                                                                         
	//printf("========NO EQUAL========buf : %s\n proc_name: %s\n file : %s\n", buf, name, file_name);
                                                                                                                  
        return NAME_NOT_EQUAL;                                                                                    
}                        
/**                                                                                                               
 * *  purpose: check the /proc file system to compare if the process name in cmdline file                            
 * *  input:                                                                                                         
 * *       proc_name       process name like dt, comm2,etc                                                           
 * *                                                                                                                 
 * *       return:                                                                                                   
 * *       0       running                                                                                           
 * *       -1      not running or other error                                                                        
 * *                                                                                                                
 * *
 * */          

int watch_proc(char *proc_name, pid_t *pid)                                                                       
{                                                                                                                 
        DIR *dirp = NULL;                                                                                         
        struct dirent *direntp =NULL;                                                                             
                                                                                                                  
        char file[MAX_BUF_LEN];                                                                                   
        int flag =0; 
		if (strcmp(proc_name,"store_proc")==0)
		{
			strcpy(proc_name,"valgrind --tool=memcheck --leak-check=full --log-file=/data/store_log.log ./store_proc");
		}		
		if (strcmp(proc_name,"flow_proc")==0)
		{
			strcpy(proc_name,"valgrind --tool=memcheck --leak-check=full --log-file=/data/flow_log.log ./flow_proc");
		}		
		if (strcmp(proc_name,"merge_proc")==0)
		{
			strcpy(proc_name,"valgrind --tool=memcheck --leak-check=full --log-file=/data/merge_proc.log ./merge_proc");
		}		
		if (strcmp(proc_name,"sys_guard")==0)
		{
			strcpy(proc_name,"valgrind --tool=memcheck --leak-check=full --log-file=/data/guard_log.log ./sys_guard");
		}		
                                                                                                                 
        if ( NULL == proc_name || '\0' == proc_name[0] ) {                                                        
                goto NOT_RUNING;                                                                                  
        }                                                                                                         
                                                                                                                  
        if( NULL == (dirp = opendir( PROC ) ) ) {                                                                 
                ERROR("open dir :%s failed,errno:%d(%s)\n",                                                       
                                        PROC,errno,strerror(errno));                                              
                goto NOT_RUNING;                                                                                  
        }                                                                                                         
                                                                                                                  
        while( NULL != ( direntp = readdir( dirp ) ) )  { 
		if ( '.' == direntp->d_name[0] ||                                                                 
                                ('.' == direntp->d_name[0] && '.' == direntp->d_name[1] ) ) {                     
                        continue;                                                                                 
                }             
		if ( 0 == atoi(direntp->d_name) ) {                                                               
                        continue;                                                                                 
                }
                                                                                                 
                bzero(file, MAX_BUF_LEN);                                                                         
                snprintf(file, MAX_BUF_LEN,"%s/%s/%s", PROC,direntp->d_name,CMDLINE);                             
                if ( NAME_EQUAL == check_proc_name( file, proc_name) ) {  
			 flag = 1;                                                                                 
                        if ( NULL != pid ) {                                                                      
                                *pid = atoi(direntp->d_name);                                                     
                        }                                                                                         
                                                                                                                  
                        break;                                                                                    
                }                                                                                                 
        }                                                                                                         
        if( 1 == flag ) {                      
		 goto RUNNING;                                                                                     
        } else {              
		 goto NOT_RUNING;                                                                                  
        }                                                                                                         
                                                                                                                  
NOT_RUNING:                                                                                                       
        if ( NULL != dirp ) {                                                                                     
                closedir(dirp);                                                                                   
        }                                                                                                         
                                                                                                                  
        return PROG_NOT_RUNNING;                                                                                  
                                                                                                                  
RUNNING:                                                                                                          
        if ( NULL != dirp ) {                                                                                     
                closedir(dirp);                                                                                   
        }                              
	 return PROG_RUNNING;                                                                                      
}               

int sys_cmd(char *cmd)                                                                                            
{                                                                                                                 
        int ret = 0;                                                                                              
        ret = system(cmd);                                                                                        
        if(ret < 0) {                                                                                             
                printf("error:%s\n", strerror(errno));                                                            
        } else if(WIFSIGNALED(ret)) {                                                                             
                printf("abnormal termination, signal number = %d\n", WTERMSIG(ret));                              
        } else if(WIFSTOPPED(ret)) {                                                                              
                printf("process stopped, signal number = %d\n", WSTOPSIG(ret));                                   
        } else if(WIFEXITED(ret)) {                                                                               
                printf("normal termination, exit status = %d\n", WEXITSTATUS(ret));                               
        }                                                                                                         
        return 0;                                                                                                 
}          

int split_path(char *path, char *file_path, char *name)
{
	char *p = NULL;
	char *head = path;
	if ( NULL != (p = strrchr(path,'/') ) ) {
		p++;
		memcpy(file_path, head, p - head);
		strncpy(name, p, (int)strlen(path) - (int)strlen(file_path));
		printf("file_path:%s\n", file_path);
		printf("name:%s\n", name);
	}
	return 0;
}
       
int restart_proc(char *proc_name)                                                                                     
{                                                                                                                 
#if 1                                                                                                             
    char cmd[MAX_BUF_LEN] = {0}; 
	char comm[2] = {0};      
	char comm_name[] = {"cloudcon"};                                                                          
    char r_num[2] = {0};                                                                                      
    char r_name[] = {"root"};                                                                                 
    //char comm_proc_name[] = {"event_export.py"};                                                                   
    parse_xml(comm, comm_name);                                                                               
    parse_xml(r_num, r_name);                                                                             
	
	char file_path[MAX_BUF_LEN] = {0};
	char name[MAX_BUF_LEN] = {0};        

	if(strcmp(proc_name, "apt") == 0) {
		split_path(auth_conf_cfg.apt_online_path, file_path, name);
		snprintf(cmd, MAX_BUF_LEN,"cd %s;./%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		printf("qqqqqqqqqqqqqqqq  apt_online_cmd:%s\n", cmd);
		sys_cmd(cmd);
	} else if(strcmp(proc_name, "offline") == 0) {
        split_path(auth_conf_cfg.apt_offline_path, file_path, name);
        snprintf(cmd, MAX_BUF_LEN,"cd %s;./%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
        printf("qqqqqqqqqqqqqqqq  apt_offline_cmd:%s\n", cmd);
        sys_cmd(cmd);
    } else if(strcmp(proc_name, "dd") == 0 || strcmp(proc_name, "apc") == 0 || strcmp(proc_name, "dm") == 0) {
		split_path(auth_conf_cfg.vds_monitor_path, file_path, name);
		snprintf(cmd, MAX_BUF_LEN,"cd %s;nohup ./%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		sys_cmd(cmd);
	}
    #if 0 
    else if(strcmp(proc_name, "apc") == 0) {
		split_path(auth_conf_cfg.vds_apc_path, file_path, name);
		snprintf(cmd, MAX_BUF_LEN,"cd %s;./%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		sys_cmd(cmd);
	}
    #endif
        else if(strcmp(proc_name, "comm_main.py") == 0) {
		    split_path(auth_conf_cfg.comm_main_path, file_path, name);
		    snprintf(cmd, MAX_BUF_LEN, "cd %s;nohup python %s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
            sys_cmd(cmd);
		#if 0
            if(strcmp(r_num, "1") == 0 && strcmp(comm, "0") == 0) {
            printf("######################\n");
			sys_cmd("/nmsmw/python-2.6.8/bin/python /nmsmw/daemon/mtx_export/event_export.py 0>/dev/null 1>/dev/null 2>/dev/null &");
		} else {
			kill_process(comm_proc_name);
		}
        #endif
	    } else if(strcmp(proc_name, "event_export.py") == 0) {
            sys_cmd("/nmsmw/python-2.6.8/bin/python /nmsmw/daemon/mtx_export/event_export.py &");
            printf("####################\n");
        } else if(strcmp(proc_name, "comm_cloud.py") == 0) {
            if(strcmp(r_num, "1") == 0) {
		        split_path(auth_conf_cfg.comm_cloud_path, file_path, name);
		        snprintf(cmd, MAX_BUF_LEN, "cd %s;nohup python %s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		        sys_cmd(cmd);
		    //printf("************cloud_cmd**************:%s\n", cmd);
        }
	} else if(strcmp(proc_name, "url_detect.py") == 0){
        split_path(auth_conf_cfg.url_detect_path, file_path, name);
        snprintf(cmd, MAX_BUF_LEN, "cd %s;python %s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
        sys_cmd(cmd);
    } else if(strcmp(proc_name, "valgrind --tool=memcheck --leak-check=full --log-file=/data/store_log.log ./store_proc") == 0) {
		split_path(auth_conf_cfg.store_proc_path, file_path, name);
		snprintf(cmd, MAX_BUF_LEN,"cd %s;%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		sys_cmd(cmd);
	} else if(strcmp(proc_name, "valgrind --tool=memcheck --leak-check=full --log-file=/data/merge_log.log ./merge_proc") == 0) {
		split_path(auth_conf_cfg.merge_proc_path, file_path, name);
        snprintf(cmd, MAX_BUF_LEN,"cd %s;%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
        sys_cmd(cmd);
	} else if(strcmp(proc_name, "valgrind --tool=memcheck --leak-check=full --log-file=/data/flow_log.log ./flow_proc") == 0) {
		split_path(auth_conf_cfg.flow_proc_path, file_path, name);
        snprintf(cmd, MAX_BUF_LEN,"cd %s;%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
        sys_cmd(cmd);
	} else if(strcmp(proc_name, "valgrind --tool=memcheck --leak-check=full --log-file=/data/guard_log.log ./sys_guard") == 0) {
		split_path(auth_conf_cfg.sys_guard_path, file_path, name);
        snprintf(cmd, MAX_BUF_LEN,"cd %s;%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		sys_cmd(cmd);
	} else if(strcmp(proc_name, "term") == 0) {
		split_path(auth_conf_cfg.term_path, file_path, name);
        snprintf(cmd, MAX_BUF_LEN,"cd %s;./%s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);
		sys_cmd(cmd);
	} 
#if 0
	else if(strcmp(proc_name, "Upgrade.jar") == 0) {
        split_path(auth_conf_cfg.upgrade_path, file_path, name);                                                     
        snprintf(cmd, MAX_BUF_LEN,"cd %s;java -jar %s -Xms800m -Xmx800m 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);  
        printf("java cmd:%s\n", cmd); 
        sys_cmd(cmd);
    }
#endif
   	else if(strcmp(proc_name, "remote_trans.py") == 0) {
        split_path(auth_conf_cfg.remote_trans_path, file_path, name);                                                     
        snprintf(cmd, MAX_BUF_LEN,"cd %s;python %s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name);  
        printf("remote cmd:%s\n", cmd); 
        sys_cmd(cmd);
    }
 
   return 0;                                                                                     
                                                                                                                  
#endif                     
}

int is_file_exist(char *path)
{
	char file_path[MAX_BUF_LEN] = {0};                                                                        
    char name[MAX_BUF_LEN] = {0}; 
    struct dirent *ptr;
    DIR *dir;
    int retn = 0;
	split_path(path, file_path, name);
    dir = opendir(file_path);
	if (dir == NULL) {
        fprintf(stderr, "open dir %s error NULL is return\n", file_path);
        CA_LOG(AUTH_MODULE, LOGMON_PROC, "open dir %s error NULL is return\n", file_path);
	}
    while((ptr = readdir(dir)) !=NULL) {
        if(strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0)
            continue;
            //printf("%s\n", ptr->d_name);

            if(strcmp(ptr->d_name, name) == 0) {
                printf("equal: %s\n", ptr->d_name);
                retn = 1;
                goto END;
            } 
        // else {
        //       *ret = 0;
        //printf("not equal : %s\n", ptr->d_name);
        
    }
        
END:
    
	    closedir(dir);
        return retn;
}

#if 0
int is_file_modified(int *t)
{
    char time_info[TIME_FORMAT_SIZE] = {0};
    struct stat buf;
    int result;
    char time_format[] = "%Y-%m-%d %X";
    char m_time[TIME_FORMAT_SIZE] = {0};
    struct tm* mtime = NULL;
    result = stat(auth_conf_cfg.wtl_path, &buf);
    if(result != 0) {
        printf("show file info error!\n");
    } else{
        printf("---------------modified time-----------:%ld\n", ctime(&buf.st_mtime));
        mtime = localtime(&buf.st_mtime);
        strftime(m_time, TIME_FORMAT_SIZE - 1, time_format, mtime);
        printf("mtime:%s\n", m_time);
    }
    FILE *fp;
    if((fp = fopen(MTIME_WTL_PATH, "rb")) == NULL) {
        printf("open %s failed!\n", MTIME_WTL_PATH);
        return -1;
    } 
    fseek(fp,0,SEEK_END);
    int file_size = 0;
    file_size = ftell(fp);
    printf("file_size:%d\n", file_size);
    fseek(fp,0,SEEK_SET);
   /* if(file_size <= 0) {
        if(fwrite(m_time, (int)strlen(m_time), 1, fp) != 1) {

                printf("file write error! \n");
                fclose(fp);
        } else {
                fclose(fp);
        }
    } else {*/
        if(fread(time_info,file_size ,sizeof(char),fp) !=1){
                if(feof(fp)){
                        fclose(fp);
                        return 0;
                }
        } else {
        fclose(fp);
        }
        printf("time_info:%s\n", time_info);
        *t = time_cmp_format(time_info, m_time);
        if(*t != 0) {
            if((fp = fopen(MTIME_WTL_PATH, "wb")) == NULL) {
            printf("open %s failed!\n", MTIME_WTL_PATH);
            return -1;
            }
            if(fwrite(m_time, (int)strlen(m_time), 1, fp) != 1) {

                printf("file write error! \n");
                fclose(fp);
            } else {
                fclose(fp);
            }        
        }

/* 
    else {
       FILE *fp;
        fp = fopen(MODIFIED_TIME_PATH, "w+");
        if( NULL == fp){
                printf("cannot open file! \n");
                return -1;
        }
        if(fwrite(ctime(&buf.st_mtime), (int)strlen(last_time), 1, fp) != 1) {

                printf("file write error! \n");
                fclose(fp);
        } else {
                fclose(fp);
        }
 
    }*/
    return 0;
}
#endif
int get_flow_num(char *flow_num)
{
	char pag_info[15] = {0};
    FILE *fp;
    int count = 0;
    if((fp = fopen( auth_conf_cfg.flow_num_path, "rb")) == NULL) {
        printf("cannot open [%s]file! \n", auth_conf_cfg.flow_num_path);
        //fclose(fp);
        return -1;
    } else {
        while(fgets(pag_info, 15, fp) !=NULL) {
            char *delim = " "; 
		    char *dest[11];
		    split(pag_info, delim, dest, &count);
		    if(strcmp(dest[0], "stream_num") == 0) {
                printf("dest[0]:%s\n", dest[0]);
                printf("dest[2]:%s\n", dest[2]);
			    strncpy(flow_num, dest[2], (int)strlen(dest[2]));
			    printf("??????????????????????flow_num:%s\n", flow_num);
		    }
        }	
        fclose(fp);
    }
	return 0;
}

int restart_mtx(int *w_retn)
{
	int r_retn = 0;
    int f_retn = 0;
	char mtx_cmd[MAX_BUF_LEN];
	char reg_id[9] = {0};
//	char mtd_format[] = "/data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-%i.mtd";
	char wtl[] = "--no_wtl";
	char m_num[2] = {0};
	char m_name[] = {"monitor"};
	char flow_num[3] = {0};
	parse_xml(m_num, m_name);
    if(strcmp(m_num, "1") != 0) {
        printf("m_num is not 1!\n");
		CA_LOG(AUTH_MODULE, LOGMON_PROC, "monitor is %s\n", m_num);
    }

	char file_path[MAX_BUF_LEN] = {0};
    char name[MAX_BUF_LEN] = {0};
	split_path(auth_conf_cfg.mtx_path, file_path, name);	
	r_retn = is_file_exist(auth_conf_cfg.reg_path);
    printf("r_retn:%d\n", r_retn);
    if(r_retn != 1) {
        printf("reg_dat is not exist!\n");
        CA_LOG(AUTH_MODULE, LOGMON_PROC, "reg_dat is not exist!\n");
    }
    f_retn = is_file_exist(auth_conf_cfg.flow_num_path);
    printf("f_retn:%d\n", f_retn);
    if(f_retn != 1) {
        printf("flow_num is not exist!\n");
        CA_LOG(AUTH_MODULE, LOGMON_PROC, "flow_num is not exist!\n");
    }
	get_flow_num(flow_num);
    if(r_retn == 1) {
	    get_reg_id(reg_id);
        printf("++++++reg_id:%d\n", (int)strlen(reg_id));
    }
    char mtd_for[] = "/data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-";
	char mtd_format[1024] = {0}; 
    sprintf(mtd_format, "%s%s.mtd", mtd_for, reg_id);
    //printf("++++++++++++++++++++++\n");
    //printf("mtd_format:%s\n", mtd_format);
	if(f_retn == 1 && r_retn == 1 && strcmp(reg_id, "\\N") != 0 && (int)strlen(reg_id) != 0 && strcmp(m_num, "1") == 0) {
		//w_retn = is_file_exist(auth_conf_cfg.wtl_path);
		if(*w_retn == 1) {
		    snprintf(mtx_cmd, MAX_BUF_LEN, "cd %s;./%s  -d special --devid %s -t %s --flow_num %s -w %s -G 30 --dynpool_size 200M --ypool_size 400M --mtdver  3  --back_trace --emergency -I  -o 10240 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name, reg_id, flow_num, flow_num, mtd_format);	
		} else if(*w_retn == 0) {
			snprintf(mtx_cmd, MAX_BUF_LEN, "cd %s;./%s  -d special --devid %s -t %s --flow_num %s -w %s -G 30 --dynpool_size 200M --ypool_size 400M --mtdver  3  --back_trace --emergency -I  -o 10240 %s 0>/dev/null 1>/dev/null 2>/dev/null &", file_path, name, reg_id, flow_num, flow_num, mtd_format, wtl);
		}
		printf("mtx_cmd:%s\n", mtx_cmd);
		CA_LOG(AUTH_MODULE, LOGMON_PROC, "mtx_cmd:%s\n", mtx_cmd);
		system(mtx_cmd);
	}
	return 0;
}
#if 0
int restart_comm(void)
{
	char comm[2] = {0};
	char comm_cmd[MAX_BUF_LEN];
	char cloud_cmd[MAX_BUF_LEN];
	char comm_name[] = {"cloudcon"};
	char r_num[2] = {0};
        char r_name[] = {"root"};
	char proc_name[] = {"event_export.py"};
	parse_xml(comm, comm_name);
	parse_xml(r_num, r_name);
	memset(comm_cmd, 0, MAX_BUF_LEN);
	if(strcmp(r_num, "0") == 0) {
		snprintf(comm_cmd, MAX_BUF_LEN, "python %s 0>/dev/null 1>/dev/null 2>/dev/null &", auth_conf_cfg.comm_main_path);
		sys_cmd(comm_cmd);
               	printf("=====cmd=====: %s \n", comm_cmd);  
		kill_process(proc_name);
	} else if(strcmp(r_num, "1") == 0 && strcmp(comm, "1") == 0) {
		snprintf(comm_cmd, MAX_BUF_LEN, "python %s 0>/dev/null 1>/dev/null 2>/dev/null &", auth_conf_cfg.comm_main_path);
		snprintf(cloud_cmd, MAX_BUF_LEN, "python %s 0>/dev/null 1>/dev/null 2>/dev/null &", auth_conf_cfg.comm_cloud_path);
		printf("****************cloud_cmd**************: %s \n", cloud_cmd);
		sys_cmd(comm_cmd);
		sys_cmd(cloud_cmd);
		kill_process(proc_name);
	} else if(strcmp(r_num, "1") == 0 && strcmp(comm, "0") == 0) {
		snprintf(comm_cmd, MAX_BUF_LEN, "python %s 0>/dev/null 1>/dev/null 2>/dev/null &", auth_conf_cfg.comm_main_path);
		sys_cmd(comm_cmd);
		sys_cmd("/nmsmv/python-2.6.8/bin/python /nmsmv/daemon/mtx_export/event_export.py 0>/dev/null 1>/dev/null 2>/dev/null &");
	}
	return 0;
}
#endif
                                    
int kill_process(char *proc_name)                                                                                     
{                                                                                                                 
        pid_t pid = 0;                                                                                            
                                                                                                                  
        if ( NULL == proc_name || '\0' == proc_name[0] ) {                                                        
                goto err;                                                                                         
        }                                                                                                         
                                                                                                                  
        if ( PROG_RUNNING != watch_proc(proc_name, &pid) ) {                                                  
                //fatalmsg("Process: %s not runnung !!!\n",&proc_name[2]);                                          
                goto err;                                                                                         
        }                                                                                                         
                                                                                                                  
                                                                                                                  
        if ( -1 == kill( pid, SIGKILL ) ) {                                                                       
               // fatalmsg("kill process:%s failed,errno:%d(%s)\n",                                                 
                        //proc_name,errno,strerror(errno));                                                         
                goto err;                                                                                         
        }                                                                                                         
                                                                                                                  
    //    fatalmsg("kill process: %s [pid:%d] success !!\n", proc_name, pid);                                       
                                                                                                                  
        return 0;                                                                                                 
err:                                                                                                              
        return -1;                                                                                                
}                    
# if 1 
int read_file(char *path, char *name)
{
        FILE *fp;
        if(NULL == (fp = fopen(path, "rb"))) {
            printf("cannot open[%s] file!\n", path);
            return -1;
        } else {
            fseek(fp, 0, SEEK_END);
            int file_size = 0;
            file_size = ftell(fp);
            fseek(fp,0,SEEK_SET);
            if(fread(name, file_size, sizeof(char),fp) != 1) {
                if(feof(fp)) {
                        fclose(fp);
                        return 0;
                }
	        } else {
		        fclose(fp);
	        }
        name[file_size - 1] = '\0';
        printf("name:%s\n", name);
    }
        return 0;
}
 
int get_reg_id(char *reg_id)
{
	char reg_id_cmd[MAX_BUF_LEN] = {0};
	sprintf(reg_id_cmd, "cat %s | awk -F \"|\" '{print $8}' > %s", auth_conf_cfg.reg_path, REG_ID_PATH);
	system(reg_id_cmd);
	read_file(REG_ID_PATH, reg_id);
    //remove(REG_ID_PATH);
	return 0;
}

#endif
int check_process_status(char *status, char *name)                                                                                  
{       
	int t = 0;
        if ( NULL == name || '\0' == name[0] ) {                                                                  
                return -1;                                                                                           
        }                                       
/*	if(strcmp(name, "comm") == 0){
		strncpy(name, "comm_main.py", 12);
	}      */ 

	int w_retn = 0;
	extern int org_wretn;
	w_retn = is_file_exist(auth_conf_cfg.wtl_path);
	char r_num[2] = {0};
    char r_name[] = {"root"};                              
	parse_xml(r_num, r_name);        
                                           
    if ( PROG_NOT_RUNNING == watch_proc(name, NULL) ) {     
		printf("closed name : %s\n", name);
		CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is not start!\n", name);
        /*	if ( 0 != access(name, F_OK) ) {                                                                  
        ERROR("access %s failed,errno:%d(%s)\n",                                                  
                                    name, errno,strerror(errno));                                     
        } else {*/
		if(strcmp(status, "1") == 0) {    
			if(strcmp(name, "mtx") == 0) {
				printf("000000000000000000000000000000\n");
				org_wretn = w_retn;
				restart_mtx(&w_retn);	
				CA_LOG(AUTH_MODULE, LOGMON_PROC, "restart %s suc!\n", name);
			} 
			//else if(strcmp(name, "comm_main.py") == 0) {
			//	restart_comm();
			//} 
			else {                                                 
                restart_proc(name);
				CA_LOG(AUTH_MODULE, LOGMON_PROC, "restart %s suc!\n", name);
			}
		} else if(strcmp(status, "1") != 0 && strcmp(status, "0") != 0) {
			t = time_cmp(status);
			printf("status1: %s\n", status);
			
			if(t <= 0) {
				if(strcmp(name, "mtx") == 0) {
                                        	restart_mtx(&w_retn);
				CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is not timeout, restart suc!\n", name);
                } 
				//else if(strcmp(name, "comm") == 0) {
                    //restart_comm();
					//CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is not timeout, restart suc!\n", name);
                //	} 
				else {
                    restart_proc(name);                                                       
					CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is not timeout, restart suc!\n", name);
				}
            }       
		}
	} else {		
		printf("starting name: %s\n", name);
		int r_retn = 0;
        char m_num[2] = {0};
        char m_name[] = {"monitor"};
        char comm[2] = {0};
        char comm_name[] = {"cloudcon"};
        //char comm_proc_name[] = {"event_export.py"};
        parse_xml(m_num, m_name);
        parse_xml(comm, comm_name);
		if(strcmp(status, "1") == 0) {	
			if(strcmp(name, "mtx") == 0) {
				char reg_id[MAX_BUF_LEN] = {0};
				r_retn = is_file_exist(auth_conf_cfg.reg_path);
                if(r_retn == 1) {
				    get_reg_id(reg_id);
                }
               	CA_LOG(AUTH_MODULE, LOGMON_PROC,"reg_id:%d\n", (int)strlen(reg_id));
                if(strcmp(m_num, "1") != 0) {
                    kill_process(name);
                    CA_LOG(AUTH_MODULE, LOGMON_PROC, "monitor is %s , killed %s suc!\n", m_num, name);
                } else if(r_retn == 1 && strcmp(reg_id, "\\N") == 0) {
					kill_process(name);
					CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is illegal, killed %s suc!\n", reg_id, name);
				} else if(r_retn == 1 && (int)strlen(reg_id) == 0){
                    kill_process(name);
                    CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is null, killed %s suc!\n", reg_id, name);
                } else if(r_retn == 0) {
					printf("****************************\n");
					kill_process(name);
                    CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is not exist, killed %s suc!\n", auth_conf_cfg.reg_path, name);
		} else if(w_retn != org_wretn) {
			kill_process(name);
			org_wretn = w_retn;
			restart_mtx(&w_retn);
			CA_LOG(AUTH_MODULE, LOGMON_PROC, "w_retn :%d, add wtl, restart mtx\n", w_retn);
		} 
			} else if(strcmp(name, "comm_cloud.py") == 0 && strcmp(r_num, "0") == 0) {
				kill_process("comm_cloud.py");
			} 
		} else if(strcmp(status, "0") == 0) {
				kill_process(name);
				printf("illegal then killed!\n");
				CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s illegal start, killed suc!\n", name);
		} else if(strcmp(status, "0") != 0 && strcmp(status, "1") !=0) {
			printf("status2: %s\n", status);
			t = time_cmp(status);
			printf("t:%d\n", t);
			//printf("status2: %s\n", status);
			if(strcmp(name, "mtx") == 0) {
				char reg_id[MAX_BUF_LEN] = {0};
                int r_retn = 0;
                r_retn = is_file_exist(auth_conf_cfg.reg_path);
				get_reg_id(reg_id);
				if(t >= 0 || r_retn == 0 || strcmp(reg_id, "\\N") == 0){
					kill_process(name);
                    if(t >= 0) {
					    CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s timeout, killed suc!\n", name);
				    } else if(r_retn == 0) {
                        CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is not exist, killed suc!\n", auth_conf_cfg.reg_path);
                    } else if(strcmp(reg_id, "\\N") == 0) {
                        CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s is exist, but reg_id is null, killed suc!\n", auth_conf_cfg.reg_path);
                    } 
                }
			} else if(t >= 0) {
				kill_process(name);
				printf("%s:timeout then killed!\n", name);
				CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s timeout, killed suc!\n", name);
			}
		}                                    	
	}                                                                             
        return 0;                                                                                                   

}                                  

int check_all_process(void)                                                                                           
{                                                                                                                 
    int i;
	extern int dest_num;
	extern struct FILE_INFO file_info;
	//get_file_info(&file_info, &dest_num);
	printf("..............dest_num:%d\n", dest_num);         
	int num;
	num = (dest_num + 8) / 2;                                                                                         
        for ( i = 0 ; i <= num; i++)
	{
		check_process_status(file_info.srv_array[i].status, file_info.srv_array[i].name);    
	}                                                                                                                                                      
        return 0;                                                                                                 
}                          

#ifdef demo
int main(void) 
{
	check_all_process();
	return 0;
}          

#endif
