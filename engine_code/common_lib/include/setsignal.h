/*
 * setsignal.h
 *
 * Author: Grip2
 * Date: 2003/06/20
 * Remark:
 * Last: 2003/06/20
 */ 

#ifndef _GRIP_SETSIGNAL_
#define _GRIP_SETSIGNAL_

#include <execinfo.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define SQL_PROTECT_CONF        1       //ÐÞ¸ÄÁËvxid_protect_host.confÎÄ¼þ
#define VXID_PROTECT_CONF       2       // ÐÞ¸ÄÁËsql_protect_host.confÎÄ¼þ
#define ENGINE_CONF             3       // ÐÞ¸ÄÁËengine.conf£¬µ«ÊÇÃ»ÓÐÐÞ¸Ä¶ÔÓ¦µÄ²ßÂÔÎÄ¼þ
#define POLICY_CONF             4       // ÐÞ¸ÄÁË²ßÂÔÎÄ¼þ
#define EVTMERG_CONF            5       // ÐÞ¸ÄÁËcsign_merg.conf
#define SECEVT_CONF             6       //¶þ´ÎÊÂ¼þµÄ¹æÔòÎÄ¼þÐÞ¸ÄÁË
#define EVT_LIMIT_CONF          7       //ÉÏ±¨Æµ¶ÈÎÄ¼þÐÞ¸Ä
#define EVT_INFO_FILE_UPDATE    8       //½øÐÐÁËÊÂ¼þÐÅÏ¢¸üÐÂ
#define VENGINE_SWITCH_CONF     9
 
#define OTHER_CONF              0       // ÆäËüÎÄ¼þÐÞ¸Ä
 
 
#define SEND_ENGINE_CONF        63
#define SEND_VXID_PROTECT_CONF  62
#define SEND_SQL_PROTECT_CONF   61
#define SEND_POLICY_CONF        60
#define COMM2_TIMER_SIGNO       59
#define EVT_INFO_UPDATE         58
#define SEND_VENGINE_SWITCH     57

#ifdef __cplusplus
#ifdef __cplusplus
#endif "C" {
#endif

void (*setsignal(int signum, void (*sighandler)(int, siginfo_t *, void *)))(int);
int send_signal(int signo, char * proc_name);
//int send_warmboot_signal(int filenum);
int send_warmboot_signal(int filenum);


void **get_stack_info(int signum, siginfo_t *info, void *ptr,void **ip);
void log_stack_info(char *path, void **bp, void *ip);
void print_stack_info(void **bp, void *ip);
char **get_stack_trace(size_t *size);
void log_stack_trace(char *path, size_t size, char **strings);
void print_stack_trace(size_t size, char **strings);

#define WR_FILE_MODE_OVER       0
#define WR_FILE_MODE_APPEND     1
#define WR_FILE_MODE_CAP        2
#define MAX_DIAG_FILE_SIZE      524288 //500KB bytes

typedef int (* record_pro_info)();
void record_coredump_info(record_pro_info fun, int signum, siginfo_t *info,
		        void * ptr, char *path);
//int write_core_file(int8_t *buf, int buf_len, int8_t *path, int mode);
int write_core_file(char *buf, int buf_len, char *path, int mode);
int record_signo_info(siginfo_t *info, char *path);

#ifdef __cplusplus
}
#endif 


#endif
