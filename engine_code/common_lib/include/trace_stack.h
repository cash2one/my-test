#ifndef TRACE_STACK_H
#define TRACE_STACK_H
#include <execinfo.h>
#include <unistd.h>
#include <signal.h>
void **get_stack_info(int signum, siginfo_t *info, void *ptr,void **ip);
void log_stack_info(char *path, void **bp, void *ip);
void print_stack_info(void **bp, void *ip);
char **get_stack_trace(size_t *size);
void log_stack_trace(char *path, size_t size, char **strings);
void print_stack_trace(size_t size, char **strings);
void record_err_info(siginfo_t *info, char *path);
#define MAX_DIAG_FILE_SIZE      524288 //500KB bytes
#endif	/*TRACE_STACK_H*/
