#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "trace_api.h"
#include "misc.h"


/***** 	var and prototype declaration 	******/

typedef int (*poutput_init)();
typedef int (*poutput_put)(FILE *, char *);
typedef int (*poutput_clean)(void);

typedef struct _output_table_t {
	poutput_init init;
	poutput_put output;
	poutput_clean clean;
}output_table_t;

#define MAX_OUTPUT_NUM 3
static int output_num;
static int real_output_num;
static output_table_t output_table[MAX_OUTPUT_NUM];
extern pthread_mutex_t mutex;

/************* function declare ****************/
/*extern int output_sock_send(FILE *fp,char *aBuf);*/

extern int stdout_open();
extern int stdout_put(FILE *fp, char *msg);
extern int stdout_close();

extern int file_open();
extern int file_put(FILE *fp, char *msg);
extern int file_close(void);

extern int syslog_open();
extern int syslog_put(FILE *fp, char *msg);
extern int syslog_close();

/************	function definition	******************/

/*
 * return 0 success
 * 	  -1 failed
 * */
int trace_thread_lock(void)
{
#ifdef _REENTRANT
	int errno_save = errno;

	if(0 == pthread_mutex_lock(&mutex)) {
		errno=errno_save;
//		return 0;
		goto ret;
	} else { 
		printf("pthread_mutex_lock:errno:%d,%s\n",errno,strerror(errno));
		errno=errno_save;
		goto err;
//		return -1;
	}
ret:
	return 0;
err:
	return -1;
#else
	return 0;
#endif
}

int trace_thread_unlock(void)
{
#ifdef _REENTRANT
	int errno_save = errno;

	if(pthread_mutex_unlock(&mutex)) {
		printf("pthread_mutex_unlock:errno:%d,%s\n",errno,strerror(errno));
		errno=errno_save;
		goto err;
	} else {
		errno = errno_save;
		goto ret;
	}
ret:
	return 0;
err:
	return -1;
#else
	return 0;
#endif
}



/* register output pipes */
static int register_output(void)
{
#ifdef VENUS_TRACE_DEBUG
	printf("in register_output\n");
#endif
	output_num =0;

	if ( OUTPUT_FILE & trace_info.trace_output ) {
		output_table[output_num].init = file_open;
		output_table[output_num].output = file_put;
		output_table[output_num].clean = file_close;
		output_num++;
	}

	if ( OUTPUT_SYSLOG & trace_info.trace_output ) {
		output_table[output_num].init = syslog_open;
		output_table[output_num].output = syslog_put;
		output_table[output_num].clean = syslog_close;
		output_num++;
	}
	if ( OUTPUT_STDOUT & trace_info.trace_output ) {
		output_table[output_num].init = stdout_open;
		output_table[output_num].output = stdout_put;
		output_table[output_num].clean = stdout_close;
		output_num++;
	}
/*	if ( OUTPUT_UDPSOCKET & trace_info.trace_output ) {
		output_table[output_num].init = trace_udp_init;
		output_table[output_num].output = NULL;
		output_table[output_num].clean = trace_udp_clean;
		output_num++;
	}*/

	return 0;
}


int output_init()
{
	int i = 0;
	int failed_num = 0;

#ifdef VENUS_TRACE_DEBUG
	printf("in output_init\n");
#endif
	register_output();
/*	for( j = 0,i = 0 ;i < output_num; j++,i++ ) {
		if(output_table[j].init() < 0) {
			printf("%d output method init error\n",i);
			j--;	// keep the correct order 
			continue;
		}
	}
	real_output_num = j;*/

	for(i = 0;i < output_num; i++) {
		if(output_table[i].init() < 0) {
			failed_num ++;
		}else if(failed_num > 0) {
			output_table[i-failed_num] = output_table[i];
		}
	}
	real_output_num = output_num - failed_num;
	return (0 !=  real_output_num)?0:-1;
}


int output_put(FILE *fp,char *msg)
{
	int i = 0;

	for ( i =0;i < real_output_num;i ++) {
		output_table[i].output(fp,msg);
	}

	return 0;
}

int output_clean()
{
	int i=0;
	
	for (i =0;i < real_output_num;i++) {
		output_table[i].clean();
	}

	return 0;
}
