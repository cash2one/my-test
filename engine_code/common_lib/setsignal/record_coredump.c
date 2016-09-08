#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "setsignal.h"
#include "misc.h"

#define BUF_LEN			256

#define WR_FILE_MODE_OVER	0
#define WR_FILE_MODE_APPEND	1
#define WR_FILE_MODE_CAP	2
#define MAX_COREDUMP_FILE_SIZE	524288

int record_signo_info(siginfo_t *info, char *path)
{
	if ((info == NULL) || (path == NULL)) {
		return -1;
	}

	char buf[128] = {0};
	char *time_str;
	time_t now;
	struct tm  *timenow;

	time(&now);
	timenow = localtime(&now);
	time_str = asctime(timenow);
	write_core_file(time_str, strlen(time_str),
			path, WR_FILE_MODE_APPEND);
	//log_core_info(time_str, strlen(time_str));
	//write_core_file(time_str, strlen(time_str), csign_conf.coredump_file,
	//              WR_FILE_MODE_APPEND);
	memset(buf, 0, sizeof(buf));
	snprintf(buf, 127, "si_pid = %d, signo = %d\n",
			info->si_pid, info->si_signo);

	write_core_file(buf, strlen(buf), path, WR_FILE_MODE_APPEND);

	return 0;
}

int write_core_file(char *buf, int buf_len, char *path, int mode)
{
	int num  = 0;
	FILE *fp = NULL;
	char bak_path[BUF_LEN] = {0};
	struct stat sbuf;

	snprintf(bak_path, BUF_LEN-1, "%s.bak", path);

	if (stat(path, &sbuf) == 0) {
		if (sbuf.st_size > MAX_COREDUMP_FILE_SIZE) {
			if (mode == WR_FILE_MODE_APPEND) {
				rename(path, bak_path);
			}
		}

	}

	if ( mode == WR_FILE_MODE_APPEND) {

		if (NULL == (fp = fopen(path, "a+"))) {
			printf("%s:%s fopen error\n", __FUNCTION__, path);
			goto err;
		}

	} else if (mode == WR_FILE_MODE_OVER) {

		rename(path, bak_path);

		if (NULL == (fp = fopen(path, "wb+"))) {
			printf("%s:%s fopen error\n", __FUNCTION__, path);
			goto err;
		}
	} else {

		goto err;
	} 

	num = fwrite(buf, sizeof(char), buf_len, fp);
	if (num <= 0) {
		printf("%s:%s fwrite error\n", __FUNCTION__, path);
		goto err;
	}

	fclose(fp);
	fp = NULL;

	return 0;

err:
	if (fp)
		fclose(fp);

	return -1;	
}

void record_coredump_info(record_pro_info fun, int signum, siginfo_t *info, 
		void * ptr, char *path) //core dump monitor
{
	char buf[128];
	static int entered = 0;
	char *time_str;
	time_t now; 
	struct tm  *timenow; 
	void *ip = NULL;
	void **bp = NULL;
	
	time(&now); 
	timenow = localtime(&now); 
	time_str = asctime(timenow); 
	write_core_file(time_str, strlen(time_str), path,		
				WR_FILE_MODE_APPEND);
	//¸ù¾ÝÓÃ»§×Ô¼º¶¨ÒåµÄº¯Êý,¿ÉÒÔ¼ÇÂ¼ÌØ¶¨µÄÐÅÏ¢
	if (fun) {
		fun();
	}
	if (entered == 0) {
		snprintf(buf, 128, "coredump signum:%d\n",signum);
		printf("%s", buf);
		++entered;
		write_core_file(buf, strlen(buf), path,		
				WR_FILE_MODE_APPEND);
		bp = get_stack_info(signum, info, ptr,&ip);
		log_stack_info(path, bp, ip);
		exit(1);
	} else {
		snprintf(buf, 128, "coredump again signum:%d, exit now!\n", signum);
		printf("%s", buf);
		write_core_file(buf, strlen(buf), path,	WR_FILE_MODE_APPEND);
		exit(2);
	}
}


