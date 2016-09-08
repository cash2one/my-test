#include "trace_api.h"
#include "output_action.h"

extern int thread_lock(void);
extern int thread_unlock(void);

int stdout_open()
{
	return 0;
}

/* just use fprintf to debug */
int stdout_put(FILE *fp, char *msg)
{	

	if ( !msg) {
		goto ret;
	}
	int ret1=0;
	ret1=trace_thread_lock();
	if(-1 == ret1) {
		printf("thread_lock error\n");
		goto ret;
	}
	fprintf(stdout,"%s", msg);
	ret1=trace_thread_unlock();
	if(-1 == ret1) {
		printf("thread_unlock error\n");
		goto ret;
	}
ret:
	return 0;
}

int stdout_close()
{

	return 0;
}
