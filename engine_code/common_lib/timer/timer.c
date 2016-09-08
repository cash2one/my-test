
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "setsignal.h"
#include "timer.h"
#include "misc.h"

#define DEFAULT_TIME_CYCLE 1
#define MAX_HDLR_TBL_NUM 16

typedef struct {
	t_hdlr_entry_t tbl[MAX_HDLR_TBL_NUM];
	int entry_num;
} hdlr_t;

static hdlr_t plugins = {
	.entry_num = 0,
};

long tick = 0;
static long g_interval = 0;

// temp varible
static int t_time_idx = 0;

static inline void clean_timer_entry(t_hdlr_entry_t *arg)
{
	if (arg->clean) {
		arg->clean();
	}

	return;
}

static inline int init_timer_entry(t_hdlr_entry_t *arg)
{
	if (arg->init && arg->init() < 0) {
		clean_timer_entry(arg);
		return -1;
	}

	arg->time_expire = tick + arg->time_cycle;
	return 0;
}

/*
 * for other modules to register their funcs and data
 */
int reg_timer_hdlr(t_hdlr_entry_t *arg)
{
	if (unlikely(plugins.entry_num >= MAX_HDLR_TBL_NUM)) {
		return -1;
	}

	if (unlikely(arg->proc == NULL)) {
		// nothing to do? invalid
		return -1;
	}

	plugins.tbl[plugins.entry_num] = *arg;

	if (arg->time_cycle <= 0) {
		// invalid cycle, activate the function at every alarm time
		if (g_interval != 0) {
			plugins.tbl[plugins.entry_num].time_cycle = g_interval;
		} else {
			// cycle == 0? error
			return -1;
		}
	}

	// if the timer has been inited, then init it immediately
	if (tick != 0) {
		if (init_timer_entry(plugins.tbl + plugins.entry_num) < 0) {
			return -1;
		}
	}

	// prepared, now insert into the plugin
	plugins.entry_num ++;

	return (plugins.entry_num - 1);
}

int change_plugin_cycle(int handle, int new_cycle)
{
	if (unlikely(handle >= plugins.entry_num || new_cycle <= 0)) {
		return -1;
	}

	plugins.tbl[handle].time_cycle = new_cycle;
	plugins.tbl[handle].time_expire = tick + new_cycle;

	return 0;
}

static void unreg_timer_hdlr()
{
	for (t_time_idx = 0; t_time_idx < plugins.entry_num; t_time_idx ++) {
		clean_timer_entry(plugins.tbl + t_time_idx);
	}

	plugins.entry_num = 0;
}

// signal handle
static void alarm_hdlr(int signum,  siginfo_t *sig, void *data)
{
	/* increment tick every interval */
	tick += g_interval;

	for (t_time_idx = 0; t_time_idx < plugins.entry_num; t_time_idx ++) {
		if (tick >= plugins.tbl[t_time_idx].time_expire) {
			// ×¢²áÊ±·ÀÖ¹ÁËproc²»¿ÉÄÜÎªNULL
			plugins.tbl[t_time_idx].proc(plugins.tbl[t_time_idx].private_data);
			plugins.tbl[t_time_idx].time_expire = 
				tick + plugins.tbl[t_time_idx].time_cycle;
		}
	}
}

void set_time(long tick_in)
{
	tick = tick_in;
}

void sync_time()
{
	 tick = time(NULL);
}	

static int _sync_time(void *arg)
{
	tick = time(NULL);
	return 0;
}

/************************************************
 *         Name: InitTimer
 *  Description: ¶¨Ê±Æ÷³õÊ¼»¯
 *  		 ¾­¹ý²âÊÔ£¬¸Ã¶¨Ê±Æ÷¿ÉÒÔ¾«È·µ½10ms£¬
 *  		 µ«Ä¿Ç°µÄÓÃÍ¾Ö»ÄÜÈ·¶¨µ½1s,
 *  		 Òò´Ë°ÑusecÈ¥µôÁË
 *     Argument: interval_sec: ÖÜÆÚ; 
 *     		 sys_sync_cycle: ÏµÍ³Í¬²½ÖÜÆÚ
 *       Return: 0
 ************************************************/
//int InitTimer(long interval_sec, long interval_usec)
int InitTimer(long interval_sec, long sys_sync_cycle)
{
	interval_sec = (interval_sec > 0 ? interval_sec : DEFAULT_TIME_CYCLE);

	if (sys_sync_cycle > 0) {
		t_hdlr_entry_t sys_sync;
		memset(&sys_sync, 0, sizeof(t_hdlr_entry_t));
		sys_sync.proc = _sync_time;
		sys_sync.time_cycle = sys_sync_cycle / interval_sec * interval_sec;

		reg_timer_hdlr(&sys_sync);
	}

	tick = time(NULL);	// ·ÀÖ¹ÔÚinitº¯ÊýÖÐº¬ÓÐget_timeµÄ²Ù×÷
	// init timer plugins and remove the initial failed node
	int failed_num = 0;
	for (t_time_idx = 0; t_time_idx < plugins.entry_num; t_time_idx ++) {
		if (init_timer_entry(plugins.tbl + t_time_idx) < 0) {
			failed_num++;
		} else {
			if (failed_num > 0) {
				plugins.tbl[t_time_idx - failed_num] = plugins.tbl[t_time_idx];
			}
		}
	}
	plugins.entry_num -= failed_num;

	setsignal(SIGALRM, alarm_hdlr);

	long interval_usec = 0;
	struct itimerval itime;
	itime.it_interval.tv_sec = interval_sec; 
	itime.it_interval.tv_usec = interval_usec;
	itime.it_value.tv_sec = interval_sec;
	itime.it_value.tv_usec = interval_usec;
	tick = time(NULL);
	setitimer(ITIMER_REAL, &itime, NULL);

	g_interval = interval_sec;

	return 0;
}

/*
 * ¸Ãº¯Êý¿ÉÒÔºöÂÔ
 */
int CleanTimer()
{
	unreg_timer_hdlr();

	struct itimerval itime;

//	itime.it_interval.tv_sec = 0; 
//	itime.it_interval.tv_usec = 0;
//	itime.it_value.tv_sec = 0;
//	itime.it_value.tv_usec = 0;
//	replace with the sentence below: 
	memset(&itime, 0, sizeof(struct itimerval));
	setitimer(ITIMER_REAL, &itime, NULL);

	tick = 0;

	return 0;
}

