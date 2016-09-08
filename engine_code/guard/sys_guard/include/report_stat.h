#ifndef _REPORT_STAT_H
#define _REPORT_STAT_H
#define  GEN_STATUS_PATH "/gms/guard/conf/GenStat.sh"
#define  SYS_STATUS_PATH "/gms/guard/conf/stat.txt"
#define MEM_PATH  "/gms/guard/conf/drop_cache.sh"
#define SIZE 3
#define NUM 5 

struct SYS_INFO{
	char name[10];
	float percent;
};

extern struct SYS_INFO sys_info[NUM];
int split(char* str, char* delim, char* dest[SIZE]);
int read_sys_file(char *info);
#endif
