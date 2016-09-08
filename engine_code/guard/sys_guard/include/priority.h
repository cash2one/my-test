#ifndef _PRIORITY_H
#define _PRIORITY_H
#define SZ 1024
#define NO 10

struct SERVICE{
	char name[100];
	int priority;

};

extern struct SERVICE service[NO];

int split_service(char* str, char* delim, char* dest[SZ], int* count);
int read_service_class(char *info);
int get_service_class(void);
int get_low_priority(char *proc_name);
int sys_guard(void);

#endif 
