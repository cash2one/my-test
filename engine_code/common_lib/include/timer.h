#ifndef _SELF_TIME
#define _SELF_TIME

#include <signal.h>

typedef int (*hdlr_init_t)();
typedef int (*hdlr_proc_t)(void *private_data);
typedef void (*hdlr_clean_t)();

typedef struct {
	// fill these
	hdlr_init_t init;
	hdlr_proc_t proc;
	hdlr_clean_t clean;

	long time_cycle;	// ÖÜÆÚ
	void *private_data;

	// the below nodes are no need to fill
	long time_expire;
} t_hdlr_entry_t;

int InitTimer(long interval_sec, long interval_usec);
int CleanTimer();

int reg_timer_hdlr(t_hdlr_entry_t *arg);
int change_plugin_cycle(int handle, int new_cycle);

void set_time(long tick_in);

extern long tick;
static inline long get_time()
{
	return tick;
}

#endif

