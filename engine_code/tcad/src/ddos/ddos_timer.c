#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "misc.h"
#include "errno.h"
#include "atomic.h"  
#include "ddos.h"

volatile int __tm_out_flag_log = 0;
volatile int __tm_out_flag_stat = 0;

volatile time_t g_ddos_time_jiffies = 0;
extern void add_to_timerinit(void);

void ddos_timer_log_out_cb(void)
{
	    ddos_atomic_set((int)1, &__tm_out_flag_log);
}

void ddos_timer_statistic_cb(void)
{
	    ddos_atomic_set((int)1, &__tm_out_flag_stat);
}

void ddos_timer_handle(void)
{
	if(!g_ddos_conf.sw)
		return;

	ddos_atomic_add64(1, &g_ddos_time_jiffies);
	ddos_timer_log_out_cb();
	ddos_timer_statistic_cb();
	add_to_timerinit();
}
int gen_random(void)
{
	unsigned int tmp = (unsigned int)time(NULL);
	srand(tmp);
	return (random());
}
#if 0 
static void timer_handler(int32_t sig)
{
	ddos_atomic_add64(1, &g_ddos_time_jiffies);
	ddos_timer_log_out_cb();
	ddos_timer_statistic_cb();
	ddos_debug("timer handle\n");
}


int32_t ddos_timer_init(void)
{
	struct itimerval value, ovalue;
	struct sigaction sg_act;

	value.it_value.tv_sec = 1;
	value.it_value.tv_usec = /* 500000 */ 0;
	value.it_interval.tv_sec = 1;
	value.it_interval.tv_usec = /* 500000 */ 0;

	setitimer(ITIMER_REAL, &value, &ovalue);

	sg_act.sa_handler  = timer_handler;
	sg_act.sa_mask     = (sigset_t){{0,}};
	sg_act.sa_flags    = SA_RESTART | SA_NODEFER;
	sg_act.sa_restorer = NULL;

	if ( sigaction(SIGALRM, &sg_act, NULL) != 0 )
	{
		fprintf(stderr, "sigaction() failed. exit...\n");
		return -2;
	}

	return 0;
}
#endif
