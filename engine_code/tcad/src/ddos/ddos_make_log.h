#include <stdint.h>
#ifndef  _MAKE_LOG_H_
#define  _MAKE_LOG_H
#define LOG_MODULE "ddos"
#define LOG_PROC "tcad_gms"

int out_put_file(char *path, char *buf);
int make_path(char *path, char *module_name, char *proc_name);
int dumpmsg_to_file(char *module_name, char *proc_name, const char *filename,
                        int line, const char *funcname, char *fmt, ...);
char *log_ip_ntoa(uint32_t ip);
#ifndef _LOG
#define CA_LOG(module_name, proc_name, x...) \
        do{ \
		dumpmsg_to_file(module_name, proc_name, __FILE__, __LINE__, __FUNCTION__, ##x);\
	}while(0)
#else
#define CA_LOG(module_name, proc_name, x...)
#endif

extern pthread_mutex_t ca_log_lock;

#endif


