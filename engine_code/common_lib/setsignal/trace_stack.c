
#define _GNU_SOURCE
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <ucontext.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#if defined(REG_RIP)
# define SIGSEGV_STACK_IA64
# define REGFORMAT "%016lx"
#elif defined(REG_EIP)
# define SIGSEGV_STACK_X86
# define REGFORMAT "%08x"
#else
# define SIGSEGV_STACK_GENERIC
# define REGFORMAT "%x"
#endif

#include "misc.h"
#include "setsignal.h"

int logmessage(FILE *fp, char *format, /*args*/ ...)
{
	char buf[1024];
	va_list ap;

	if (unlikely(NULL == fp)) {
		return -1;
	}

	va_start(ap, format); 
	vsprintf(buf, format, ap);
	va_end(ap);

	if (fprintf(fp, "%s",buf) < 0) {
		return -1;
	}

	return 0;
}

void **get_stack_info(int signum, siginfo_t *info, void *ptr,void **ip)
{
	if (unlikely(info == NULL || ptr == NULL || ip == NULL)) {
		return NULL;
	}

	ucontext_t *ucontext = (ucontext_t *) ptr;
	void **bp = 0;

# if defined(SIGSEGV_STACK_IA64)
	*ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];
	bp = (void **) ucontext->uc_mcontext.gregs[REG_RBP];
# elif defined(SIGSEGV_STACK_X86)
	*ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];
	bp = (void **) ucontext->uc_mcontext.gregs[REG_EBP];
# endif	

	return bp;
}

void print_stack_info(void **bp, void *ip)
{
	const char *symname = NULL;
	Dl_info dlinfo;
	int f = 0;
	while (bp && ip) {
		if (!dladdr(ip, &dlinfo))
			break;
		symname = dlinfo.dli_sname;
		fprintf(stderr, "% 2d: %p <%s+%u> (%s)\n", ++f, ip, symname, (unsigned)   (ip - dlinfo.dli_saddr),dlinfo.dli_fname);
		if (dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
			break;
		ip = bp[1];
		bp = (void **) bp[0];
	}

}

void log_stack_info(char *path, void **bp, void *ip)
{
	const char *symname = NULL;
	Dl_info dlinfo;
	int f = 0;
	char stack_info[128];

	struct stat sbuf;
	FILE *fp = NULL;

	if (stat(path, &sbuf) == 0) {
		if (sbuf.st_size > MAX_DIAG_FILE_SIZE) {
			if (NULL != (fp = fopen(path, "w"))) {
				fclose(fp);
				fp = NULL;
			}
		}
	}

	if ( NULL == (fp=fopen(path, "a") ) ) {
		printf("open file :%s failed,errno:%d(%s)\n",
				path,errno,strerror(errno));
		goto err;
	}
	while (bp && ip) {
		if (!dladdr(ip, &dlinfo))
			break;
		symname = dlinfo.dli_sname;
		snprintf((char *)stack_info, 127,"% 2d: %p <%s+%u> (%s)\n", ++f, ip,      symname, (unsigned) (ip - dlinfo.dli_saddr),dlinfo.dli_fname);
		logmessage(fp, "%s", stack_info);
		if (dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
			break;
		ip = bp[1];
		bp = (void **) bp[0];
	}
	fclose(fp);
err:
	return;
}

char **get_stack_trace(size_t *size)
{
#define MAX_ARRAY_LEN   72
	char **strings = NULL;
	void *array[MAX_ARRAY_LEN];
	*size = backtrace(array, MAX_ARRAY_LEN);
	strings = backtrace_symbols(array, *size);
	return strings;
}

void log_stack_trace (char *path, size_t size, char **strings)
{
	if(unlikely(NULL == path || NULL == strings)) {
		goto err;
	}
	size_t i;
	struct stat sbuf;
	FILE * fp = NULL;

	if (stat(path, &sbuf) == 0) {
		if (sbuf.st_size > MAX_DIAG_FILE_SIZE) {
			if (NULL != (fp = fopen(path, "w"))) {
				fclose(fp);
				fp = NULL;
			}
		}
	}

	if ( NULL == (fp=fopen(path, "a") ) ) {
		printf("open file :%s failed,errno:%d(%s)\n",
				path,errno,strerror(errno));
		goto err;
	}


	for (i = 0; i < size; i++) {
		logmessage(fp, "%s\n", strings[i]);
	}

	fclose(fp);
	free (strings);
err:
	return;
}
 

void print_stack_trace(size_t size, char **string)
{
	if(unlikely(NULL == string)) {
		return;
	}
	size_t i;
	for(i = 0; i < size; i++) {
		printf("%s\n", string[i]);
	}
	return;
}
