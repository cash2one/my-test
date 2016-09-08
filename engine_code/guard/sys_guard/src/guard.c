#include<stdio.h>
#include<string.h>
#include"priority.h"
#include"report_stat.h"
#include"errno.h"

int get_low_priority(char *proc_name)
{
	get_service_class();
	
	extern int dest_num;	
	int i;
	int MAX = service[0].priority;
	for(i = 0; i < (dest_num - 1) / 2; i++)
		if(service[i].priority > MAX)
			MAX = service[i].priority;
	//printf("service[%d].name:%s\n", MAX - 1,service[MAX - 1].name);
	strcpy(proc_name, service[MAX - 1].name);
	printf("proc_name:%s\n",proc_name);
	return 0;
}

#if 0
int sys_cmd(char *cmd)
{
	int ret = system(cmd);
	if(ret < 0) {
                printf("error:%s\n", strerror(errno));
        } else if(WIFSIGNALED(ret)) {
                printf("abnormal termination, signal number = %d\n", WTERMSIG(ret));
        } else if(WIFSTOPPED(ret)) {
                printf("process stopped, signal number = %d\n", WSTOPSIG(ret));
        } else if(WIFEXITED(ret)) {
                printf("normal termination, exit status = %d\n", WEXITSTATUS(ret));
	}
	return ret;
}


int restart_proc(char *proc_name)
{                               
	                                                                             
        int8_t cmd[MAX_BUF_LEN];
	int ret1;
        bzero(cmd , MAX_BUF_LEN);
        snprintf(cmd, MAX_BUF_LEN,"%s%s 0>/dev/null 1>/dev/null 2>/dev/null &", PATH, proc_name);
        printf("cmd :%s\n", cmd);
        ret1 = sys_cmd(cmd);

}


int guard_cpu(void)
{
	
	char info[100], proc_name[20];
	read_sys_file(info);
	if(sys_info[1].percent > 80) {
		get_low_priority(proc_name);
		restart_proc(proc_name);
	}		
	return 0;
}

int guard_mem(void)
{
	if(sys_info[0].percent > 80) {
		sys_cmd(MEM_PATH);
	}
	
	return 0;
}
#endif
