#ifndef __HEART_BEAT_H__
#define __HEART_BEAT_H__
#include <sys/types.h>
#include <signal.h>

typedef int (* record_pro_info)();
void record_coredump_info(record_pro_info fun, int signum, siginfo_t *info, void * ptr, char *path);
int write_core_file(char *buf, int buf_len, char *path, int mode);

#endif

