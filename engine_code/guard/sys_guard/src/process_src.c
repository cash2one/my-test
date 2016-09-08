#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#define PROC_CONF_PATH 		"/gms/guard/conf/process.conf"
#include"process_src.h"
//#define PROC_PATH       "/gms/guard/conf/proc_src.txt"
//#define GEN_PROC_PATH   "/home/xiedongling/svn/GMS/trunk/engine_code/guard/sys_guard/src/process_source.sh"
#include"file_path.h"
#include"regular_guard.h"
#include"report_stat.h"
#include"make_log.h"

int read_process_conf(proc_src *l_proc_src)
{
	char proc_info[40] = {0};
	FILE *fp;
	int i = 0;
	if((fp = fopen( guard_conf_cfg.process_path,"rb")) == NULL) { 
                printf("cannot open file! \n");
		//fclose(fp);
                return -1;
        } 
            while(fgets(proc_info, 40, fp) !=NULL) {
            printf("info:%s", proc_info);
            char *delim = ";";
            char* dest[20] = {0};
		    char* dest_name[20] = {0};
		    char* dest_percent[5] = {0};
		    char* proc_name[10] = {0};
            split(proc_info, delim, dest);
		    delim = "<";
		    split(dest[0], delim, dest_name);
		    split(dest[1], delim, dest_percent);
	
		    delim = ":";
		    split(dest_name[0], delim, proc_name);		
		    printf("proc_name[0]:%s\n", proc_name[0] );

		    l_proc_src[i].cpu_pre = atof(dest_name[1]);
            printf("sys_info[%d]:%f\n",i,l_proc_src[i].cpu_pre);
		    printf("dest_percent:%s\n", dest_percent[0]);
		    l_proc_src[i].mem_pre = atof(dest_percent[1]);
            printf("sys_info[%d]:%f\n",i,l_proc_src[i].mem_pre);
		    strncpy(l_proc_src[i].proc_name, proc_name[0], (int)(strlen(proc_name[0])));
		    printf("proc_name:%s\n", l_proc_src[i].proc_name);
            i++;

        }
	    fclose(fp);
	return 0;
}

int get_proc_src(proc_src *gen_proc_src)
{
	char cmd[CMD_LEN];
	sprintf(cmd, "%s > %s", GEN_PROC_PATH, PROC_PATH);
	system(cmd);
	
	FILE *fp = NULL;
	char gen_proc_info[40] = {0};
    if((fp = fopen( PROC_PATH,"rb")) == NULL){
        printf("cannot open[%s] file! \n", PROC_PATH);
		fclose(fp);
        return -1;
    }
        char name [3] = {0}; 
        int i = 0;;
        while(fgets(gen_proc_info, 40, fp) !=NULL) {
            strncpy(name, gen_proc_info, 3);
            printf("*******name:%s\n", name);
            if(i == 0) {
                if (strcmp(name, "mtx") != 0 && strcmp(name, "apt") == 0) {
                    printf("0000000\n");
                    gen_proc_src[i].cpu_pre = 0;
                    gen_proc_src[i].mem_pre = 0;
                    if (strcmp(name, "apt") == 0) {
                        i = 1;
                    } else if(strcmp(name, "vds") == 0) {
                        i = 2;
                    }
                } 
            } else if(i == 1) {
                printf("11111111\n");
                if (strcmp(name, "apt") != 0 && strcmp(name, "vds") == 0) {
                    gen_proc_src[i].cpu_pre = 0;
                    gen_proc_src[i].mem_pre = 0;
                    i = 3;
                }
            } 

            printf("info:%s", gen_proc_info);
            char *delim = ";";
            char* dest[20];
		    char* dest_cpu[20] = {0};
            char* dest_mem[20] = {0};
            split(gen_proc_info, delim, dest);
		    printf("dest[0]:%s\n", dest[0]);		
		    printf("dest[1]:%s\n", dest[1]);		
	            printf("1111111111111\n");
		    CA_LOG(GUARD_MODULE, GUARD_PROC, "%s;%s\n", dest[0], dest[1]);
	            printf("2222222222222\n");

		    delim = ":";
		    split(dest[0], delim, dest_cpu);
		    split(dest[1], delim, dest_mem);
	        printf("0000::%.2f\n", atof(dest_cpu[1]));
		    gen_proc_src[i].cpu_pre = atof(dest_cpu[1]);
		    printf("process_info[%d]:%f\n",i,gen_proc_src[i].cpu_pre);
		    gen_proc_src[i].mem_pre = atof(dest_mem[1]);
		    printf("process_info[%d]:%f\n",i,gen_proc_src[i].mem_pre);		
            i++;

        }	
	fclose(fp);
	return 0;
}
int guard_process(void)
{	
	int i;
	int ret1 = 0;
	int ret2 = 0;
	int stat = 0;
	int retn = 0;
	char cmd[CMD_LEN] = {0};
	//proc_src * l_proc_src = calloc(3, sizeof(proc_src));
	l_proc_src = calloc(3, sizeof(proc_src));
    read_process_conf(l_proc_src);

	//proc_src * gen_proc_src = calloc(3, sizeof(proc_src));
	gen_proc_src = calloc(3, sizeof(proc_src));
    get_proc_src(gen_proc_src);

	for(i =0; i < 3; i++) {
		ret1 = gen_proc_src[i].cpu_pre - l_proc_src[i].cpu_pre;
		ret2 = gen_proc_src[i].mem_pre - l_proc_src[i].mem_pre;
		sprintf(cmd, "killall %s", l_proc_src[i].proc_name);
		//printf("cmd:%s\n", cmd);
	#if 1
		if(ret1 > 0 || ret2 > 0) {
			stat = 0;
			sys_cmd(cmd);
		printf("cmd:%s\n", cmd);
		}
	#endif
		printf("ret1[%d]:%d\n", i, ret1);
		printf("ret2[%d]:%d\n", i, ret2);
	}
		
	FILE *fp = NULL;
        char gen_proc_info[1024] = {0};
        if((fp = fopen( PROC_PATH,"rb")) == NULL) {
                printf("cannot open[%s] file! \n", PROC_PATH);
		fclose(fp);
		retn = -1; 
		goto END;
	}
                
	    fseek(fp,0,SEEK_END);
        int file_size;
        file_size = ftell(fp);
        //printf("length:%d\n",file_size);
        fseek(fp,0,SEEK_SET);
        if(fread(gen_proc_info,file_size - 1,sizeof(char),fp) !=1) {
            if(feof(fp)) {
                fclose(fp);
		retn = -1; 
 	        goto END;      
            } 
            /*else {
                printf("gen_proc_info:%s\n",gen_proc_info);
	            CA_LOG(GUARD_MODULE, GUARD_PROC, "%s\n", gen_proc_info);
            }*/
        } 
    /*else {
		fclose(fp);
	}*/

    fclose(fp);	
END:
	free(l_proc_src);
	free(gen_proc_src);
	return retn;
}
