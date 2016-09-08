#include<stdio.h>
#include<string.h>
#include<time.h>
#include"sysstatus_file.h"
#include"store_common.h"
#include"report_stat.h"
#include"priority.h"
#include<fcntl.h>
#include<unistd.h>
#include"file_path.h"
#include"make_log.h"

#if 1
int get_libversion(char *libversion)
{
    char version_info[30] = {0};                                                                                      
    FILE *fp = NULL;       
	char *delim = ":";                                                                                
    char *dest[8];       	
    if((fp = fopen( guard_conf_cfg.libversion_path, "rb")) == NULL) {                                            
    	printf("cannot open [%s]file! \n", guard_conf_cfg.libversion_path);     
		fclose(fp);				
        return -1;                                                                                        
    }                                                                                                         
    while(fgets(version_info, 30, fp) != NULL) {                                                                    
    	split(version_info, delim, dest);                                                                     
        if(strcmp(dest[0], "#version") == 0) {                                                              
            printf("dest[0]:%s\n", dest[0]);                                                                  
            printf("dest[1]:%s\n", dest[1]);                                                                  
            strncpy(libversion, dest[1], (int)strlen(dest[1]));       
            libversion[(int)strlen(libversion) - 1] = '\0';                                          
        } else {
            break;
        }                                                                 
    }                                                                                                         
            //strncpy(libversion, dest[2], (int)strlen(dest[2]));                                                 
        printf("libversion:%s\n", libversion);                                          
        fclose(fp);                   

    return 0;
}

int get_sysversion(char *sysversion)
{
    int retn;                                                                                                     
    retn = common_read_conf(guard_conf_cfg.pql_conf_path, version_cfg);                                                      
        if (retn < 0) {                                                                                               
            fprintf(stderr, "[-][PQL_CONF] Read config %s ERROR!\n", PQL_CONF_PATH);                                               

    }          
    return 0;
}

#endif 

int read_cfg(char *path, char *name)
{
	FILE *fp = NULL;
	if(NULL == (fp = fopen(path, "rb"))) {
		printf("cannot open file!\n");
		fclose(fp);
		return -1;
	} else {
	    fseek(fp, 0, SEEK_END);
        int file_size = 0;
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
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
int get_devid(char *src,char *dst)
{
	FILE *fp;
	memset(dst,0,20);
	fp=popen("python /gms/gapi/modules/auth/genkey.pyc","r");
	fgets(dst,20,fp);
	pclose(fp);
	

	return 0;
}


int make_sysfile(char *file_str)
{
	extern struct SYS_INFO sys_info[NUM];
//	char file_str[CMD_LEN];
	//clock_t finish;
	//int runtime;
	char devid[38], libversion[20], sysversion[20];
//	start = clock();
	
	memset(devid, 0, 38);
	memset(libversion, 0, 20);
	memset(sysversion, 0, 20);
#if 0
	int retn;
        retn = common_read_conf(GUARD_CFG_PATH, g_guard_cfg);
    if (retn < 0) {
        fprintf(stderr, "[-][GUARD_CONF] Read config %s ERROR!\n", GUARD_CFG_PATH);

    }
#endif	
	get_devid(guard_conf_cfg.devid_path, devid);
	printf("devid[%d]:%s\n", (int)strlen(devid), devid);
	//read_cfg(guard_conf_cfg.libversion_path, libversion);
	get_libversion(libversion);
    printf("libversion[%d]:%s\n", (int)strlen(libversion), libversion);
//	read_cfg(guard_conf_cfg.sysversion_path, sysversion);
/*
    int retn;
    retn = common_read_conf(guard_conf_cfg.sysversion_path, version_cfg);
        if (retn < 0) {
            fprintf(stderr, "[-][PQL_CONF] Read config %s ERROR!\n", guard_conf_cfg.sysversion_path);                              
                                                                                                                  
    } 
*/                    
    char node_name[20] = {0}; 
    strncpy(node_name, "SysVersion", (int)strlen("SysVersion"));
    parse_xml(sysversion, node_name);
	printf("sysversion[%d]:%s\n",(int)strlen(sysversion), sysversion);
	
	char now_time_str[25];
    get_systime_str(now_time_str);	

//	extern int runtime;	
	//finish = clock();
//	printf("finish:%lu\n", finish);
//	runtime = (int)((finish - start) / CLOCKS_PER_SEC);
//	printf("runtime: %d\n", runtime);
	
	int state;
	if(sys_info[0].percent > 80 || sys_info[1].percent > 80 || sys_info[2].percent > 80 || sys_info[4].percent > 80 || sys_info[5].percent > 80) {
		state = 0;
         CA_LOG(GUARD_MODULE, GUARD_PROC, "state is:%d, now start sys_guard!\n", state);
        sys_guard();
	} else {
		state = 1;
	}
	
	sprintf(file_str, "\"%s\"|%s|%.2f|%.2f|%.2f|%d|%d|\"%s\"|\"%s\"|%.2f|%.2f\n", devid, now_time_str, sys_info[0].percent, sys_info[1].percent, sys_info[2].percent, state, (int)sys_info[3].percent, libversion, sysversion, sys_info[4].percent, sys_info[5].percent);
    //int len = (int)strlen(file_str);
	printf("file_str[%d]:%s\n",(int)strlen(file_str), file_str);
	CA_LOG(GUARD_MODULE, GUARD_PROC, "disk:%f, cpu:%f, mem:%f\n", sys_info[0].percent, sys_info[1].percent, sys_info[2].percent);
	CA_LOG(GUARD_MODULE, GUARD_PROC,"file_str[%d]:%s\n", (int)strlen(file_str), file_str);
		
	char old_file_name[CMD_SIZE], new_file_name[CMD_SIZE];
	memset(old_file_name, 0, CMD_SIZE - 1);
	memset(new_file_name, 0, CMD_SIZE - 1);
	char time_format[] = "%Y%m%d%H%M%S";
	char now_time[25];
	get_systime_str_format(now_time, time_format);	

	sprintf(old_file_name, "%s%s_%s.ok.tmp", guard_conf_cfg.sysstatus_file_path, now_time, devid);
	printf("old_file_name:%s\n", old_file_name);
	sprintf(new_file_name, "%s%s_%s.ok", guard_conf_cfg.sysstatus_file_path, now_time, devid);

	FILE *fp2;
	fp2 = fopen(old_file_name, "w");
        if( NULL == fp2){
                printf("+++++++++cannot open file! \n");
                return -1;
        }
        if(fwrite(file_str, strlen(file_str), 1, fp2) != 1) {

                printf("file write error! \n");
                fclose(fp2);
        } else {
                fclose(fp2);
		rename(old_file_name, new_file_name);
		printf("new_file_name:%s\n", new_file_name);
        }
		
#if 1
	int fd;
	fd = open(TMP_STATUS_PATH, O_RDWR | O_CREAT | O_APPEND);
	if(write(fd, file_str, strlen(file_str)) != (int)strlen(file_str)) {
		printf("write error!\n");
		close(fd);
	} else {	
		write(fd, "\n", 1);
		close(fd);
	}
#endif
	return 0;
}


