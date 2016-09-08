#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"priority.h"
#include"regular_guard.h"
#include"report_stat.h"
#include"errno.h"
#include"file_path.h"
#include"make_log.h"

int split_service(char* str, char* delim, char* dest[SZ], int* count)
{
        int num = 0;
        for(dest[num] = strtok(str, delim); dest[num] != NULL; dest[++num] = strtok(NULL, delim));
        *count = num;

        return 0;
}

int read_service_class(char *info)
{
        FILE *fp;
        if((fp = fopen(guard_conf_cfg.service_class_path,"rb")) == NULL){
                printf("cannot open file! \n");
		fclose(fp);
                return -1;
        }
        fseek(fp,0,SEEK_END);
        int file_size;
        file_size = ftell(fp);
        printf("length:%d\n",file_size);
        fseek(fp,0,SEEK_SET);
        if(fread(info,file_size,sizeof(char),fp) !=1){
                if(feof(fp)){
                        fclose(fp);
                        return 0;
                }
        } else {
		fclose(fp);
	}
        printf("info:%s\n",info);

        return 0;
}

int get_service_class(void)
{                                                                                                                 
        extern int dest_num;                                                                                      
        char info[100];                                                                                           
        memset(info, 0, 99);                                                                                      
        read_service_class(info);                                                                                      
        char *delim = ";";                                                                                        
        char* dest[SZ];                                                                                         
        split_service(info, delim, dest, &dest_num);  
		int i,j = 0;      
		printf(">>>>>>>>>>>>>>>>.\n");                                                                                          
        for(i = 0; i < dest_num-1; i++){                                                                          
                if(i % 2 == 0){                                                                                   
                        service[j].priority = atoi(dest[i]);   
			printf("service[%d].priority:%d\n", j, service[j].priority);                   
		} else {
			strncpy(service[j].name, dest[i], (int)strlen(dest[i]));  
			printf("service[%d].name:%s\n", j, service[j].name);
                        j++;                                                                                      
        	}                                                                                                 
                                                                                                                  
        }                                                                                                         
        return 0;                                                                                                 
}

int get_low_priority(char *proc_name)
{
        get_service_class();

        extern int dest_num;
        int i;
        int MAX = service[0].priority;
        for(i = 0; i < (dest_num - 1) / 2; i++) {
                if(service[i].priority > MAX)
                        MAX = service[i].priority;
	}
	strcpy(proc_name, service[MAX - 1].name);
        printf("proc_name:%s\n",proc_name);
        return 0;
}              

int sys_guard(void)
{
	//extern struct SYS_INFO sys_info[NUM];
	char proc_name[1024];
    char cmd[1024] = {0};
    memset(proc_name, 0, 1023);
	get_low_priority(proc_name);
    sprintf(cmd, "killall %s", proc_name);
	//if(sys_info[1].percent > 80|| sys_info[2].percent > 80) {
		sys_cmd(cmd);
	//}
    CA_LOG(GUARD_MODULE, GUARD_PROC, "killed the low proc %s!\n", proc_name);
    return 0;
}    
