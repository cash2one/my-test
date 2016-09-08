#ifndef __URL_LOG_H__
#define __URL_LOG_H__

#include<stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
 #include <unistd.h>

#define URL_PATH_LEN 200
#define url_conf_file_path "/gms/tcad/url.conf"
typedef struct url_str{

	FILE* fp;
	char file_path[URL_PATH_LEN];

}Url_str;

extern void add_url_timer(void);
extern int url_init(void); 
extern int write_url_file(char *buf);

#endif
