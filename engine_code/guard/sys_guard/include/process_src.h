#ifndef _PROCESS_SRC_H_
#define _PROCESS_SRC_H_
//#define PROC_CONF_PATH	"/gms/guard/conf/process.conf"
#define PROC_PATH	"/gms/guard/conf/proc_src.txt"
#define GEN_PROC_PATH   "/gms/guard/conf/process_source.sh"
#define PROCESS_NUM 3

typedef struct process_src{
	float cpu_pre;
	float mem_pre;
	char proc_name[4];

} proc_src;

proc_src * l_proc_src;
proc_src * gen_proc_src;

int read_process_conf(proc_src *l_proc_src);
int get_proc_src(proc_src *gen_proc_src);
int guard_process(void);  

#endif
