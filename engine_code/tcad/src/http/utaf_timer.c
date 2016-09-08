#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include "utaf_timer.h"
#if 0
#include "utaf_mt.h"
#include <spinlock.h>
#include <atomic.h>
#include "utaf.h"
#endif

#include "utaf_cycle.h"



enum timer_source _eal_timer_source = EAL_TIMER_HPET;


uint64_t UTAF_SYS_HZ = 0;



/* The frequency of the RDTSC timer resolution */
static uint64_t eal_tsc_resolution_hz = 0;


#if 0 /* wdb_ppp */
LIST_HEAD(timer_list, timer);

struct priv_timer {
	struct timer_list pending;  /**< list of pending timers */
	struct timer_list expired;  /**< list of expired timers */
	struct timer_list done;     /**< list of done timers */
	spinlock_t list_lock;       /**< lock to protect list access */

	/** per-core variable that true if a timer was updated on this
	 *  core since last reset of the variable */
	int updated;

	unsigned prev_lcore;              /**< used for lcore round robin */

#ifdef RTE_LIBRTE_TIMER_DEBUG
	/** per-lcore statistics */
	struct rte_timer_debug_stats stats;
#endif
} __rte_cache_aligned;



/** per-lcore private info for timers */
static struct priv_timer priv_timer[MAX_LCORE];


/* when debug is enabled, store some statistics */
#ifdef RTE_LIBRTE_TIMER_DEBUG
#define __TIMER_STAT_ADD(name, n) do {				\
		unsigned __lcore_id = rte_lcore_id();		\
		priv_timer[__lcore_id].stats.name += (n);	\
	} while(0)
#else
#define __TIMER_STAT_ADD(name, n) do {} while(0)
#endif


/* this macro allow to modify var while browsing the list */
#define LIST_FOREACH_SAFE(var, var2, head, field)		       \
	for ((var) = ((head)->lh_first),			       \
		     (var2) = ((var) ? ((var)->field.le_next) : NULL); \
	     (var);						       \
	     (var) = (var2),					       \
		     (var2) = ((var) ? ((var)->field.le_next) : NULL))




/* Init the timer library. */
void utaf_timer_subsystem_init(void)
{
	unsigned lcore_id;

	for (lcore_id = 0; lcore_id < MAX_LCORE; lcore_id ++) {
		LIST_INIT(&priv_timer[lcore_id].pending);
		LIST_INIT(&priv_timer[lcore_id].expired);
		LIST_INIT(&priv_timer[lcore_id].done);
		spinlock_init(&priv_timer[lcore_id].list_lock);
		priv_timer[lcore_id].prev_lcore = lcore_id;
	}
}
#endif /* wdb_ppp */
uint64_t timer_get_tsc_hz(void)
{
	return eal_tsc_resolution_hz;
}


#if 0 /* wdb_ppp */
/* Initialize the timer handle tim for use */
void timer_init(struct timer *tim)
{
	union timer_status status;

	status.state = TIMER_STOP;
	status.owner = TIMER_NO_OWNER;
	tim->status.u32 = status.u32;
}






/*
 * if timer is pending or stopped (or running on the same core than
 * us), mark timer as configuring, and on success return the previous
 * status of the timer
 */
static int
timer_set_config_state(struct timer *tim,
		       union timer_status *ret_prev_status)
{
	union timer_status prev_status, status;
	int success = 0;
	unsigned lcore_id;

	lcore_id = utaf_lcore_id();

	/* wait that the timer is in correct status before update,
	 * and mark it as beeing configured */
	while (success == 0) {
		prev_status.u32 = tim->status.u32;

		/* timer is running on another core, exit */
		if (prev_status.state == TIMER_RUNNING &&
		    (unsigned)prev_status.owner != lcore_id)
			return -1;

		/* timer is beeing configured on another core */
		if (prev_status.state == TIMER_CONFIG)
			return -1;

		/* here, we know that timer is stopped or pending,
		 * mark it atomically as beeing configured */
		status.state = TIMER_CONFIG;
		status.owner = (int16_t)lcore_id;
		success = atomic32_cmpset(&tim->status.u32,
					      prev_status.u32,
					      status.u32);
	}

	ret_prev_status->u32 = prev_status.u32;
	return 0;
}

/*
 * if timer is pending, mark timer as running
 */
static int
timer_set_running_state(struct timer *tim)
{
	union timer_status prev_status, status;
	unsigned lcore_id = utaf_lcore_id();
	int success = 0;

	/* wait that the timer is in correct status before update,
	 * and mark it as running */
	while (success == 0) {
		prev_status.u32 = tim->status.u32;

		/* timer is not pending anymore */
		if (prev_status.state != TIMER_PENDING)
			return -1;

		/* here, we know that timer is stopped or pending,
		 * mark it atomically as beeing configured */
		status.state = TIMER_RUNNING;
		status.owner = (int16_t)lcore_id;
		success = atomic32_cmpset(&tim->status.u32,
					      prev_status.u32,
					      status.u32);
	}

	return 0;
}

/*
 * add in list, lock if needed
 * timer must be in config state
 * timer must not be in a list
 */
static void
timer_add(struct timer *tim, unsigned tim_lcore, int local_is_locked)
{
	uint64_t cur_time = utaf_get_timer_cycles();
	unsigned lcore_id = utaf_lcore_id();
	struct timer *t, *t_prev;

	/* if timer needs to be scheduled on another core, we need to
	 * lock the list; if it is on local core, we need to lock if
	 * we are not called from rte_timer_manage() */
	if (tim_lcore != lcore_id || !local_is_locked)
		spinlock_lock(&priv_timer[tim_lcore].list_lock);

	t = LIST_FIRST(&priv_timer[tim_lcore].pending);

	/* list is empty or 'tim' will expire before 't' */
	if (t == NULL || ((int64_t)(tim->expire - cur_time) <
			  (int64_t)(t->expire - cur_time))) {
		LIST_INSERT_HEAD(&priv_timer[tim_lcore].pending, tim, next);
	}
	else {
		t_prev = t;

		/* find an element that will expire after 'tim' */
		LIST_FOREACH(t, &priv_timer[tim_lcore].pending, next) {
			if ((int64_t)(tim->expire - cur_time) <
			    (int64_t)(t->expire - cur_time)) {
				LIST_INSERT_BEFORE(t, tim, next);
				break;
			}
			t_prev = t;
		}

		/* not found, insert at the end of the list */
		if (t == NULL)
			LIST_INSERT_AFTER(t_prev, tim, next);
	}

	if (tim_lcore != lcore_id || !local_is_locked)
		spinlock_unlock(&priv_timer[tim_lcore].list_lock);
}

/*
 * del from list, lock if needed
 * timer must be in config state
 * timer must be in a list
 */
static void
timer_del(struct timer *tim, unsigned prev_owner, int local_is_locked)
{
	unsigned lcore_id = utaf_lcore_id();

	/* if timer needs is pending another core, we need to lock the
	 * list; if it is on local core, we need to lock if we are not
	 * called from rte_timer_manage() */
	if (prev_owner != lcore_id || !local_is_locked)
		spinlock_lock(&priv_timer[prev_owner].list_lock);

	LIST_REMOVE(tim, next);

	if (prev_owner != lcore_id || !local_is_locked)
		spinlock_unlock(&priv_timer[prev_owner].list_lock);
}

/* Reset and start the timer associated with the timer handle (private func) */
static int
__timer_reset(struct timer *tim, uint64_t expire,
		  uint64_t period, unsigned tim_lcore,
		  timer_cb_t fct, void *arg,
		  int local_is_locked)
{
	union timer_status prev_status, status;
	int ret;
	unsigned lcore_id = utaf_lcore_id();

	/* round robin for tim_lcore */
	if (tim_lcore == (unsigned)LCORE_ID_ANY) {
		tim_lcore = mt_get_next_lcore(priv_timer[lcore_id].prev_lcore,
					       0, 1);
		priv_timer[lcore_id].prev_lcore = tim_lcore;
	}

	/* wait that the timer is in correct status before update,
	 * and mark it as beeing configured */
	ret = timer_set_config_state(tim, &prev_status);
	if (ret < 0)
		return -1;

	__TIMER_STAT_ADD(reset, 1);
	priv_timer[lcore_id].updated = 1;

	/* remove it from list */
	if (prev_status.state == TIMER_PENDING ||
	    prev_status.state == TIMER_RUNNING) {
		timer_del(tim, prev_status.owner, local_is_locked);
		__TIMER_STAT_ADD(pending, -1);
	}

	tim->period = period;
	tim->expire = expire;
	tim->f = fct;
	tim->arg = arg;

	__TIMER_STAT_ADD(pending, 1);
	timer_add(tim, tim_lcore, local_is_locked);

	/* update state: as we are in CONFIG state, only us can modify
	 * the state so we don't need to use cmpset() here */
	m_wmb();
	status.state = TIMER_PENDING;
	status.owner = (int16_t)tim_lcore;
	tim->status.u32 = status.u32;

	return 0;
}

/* Reset and start the timer associated with the timer handle tim */
uint32_t timer_reset(struct timer *tim, uint64_t ticks,
		enum timer_type type, unsigned tim_lcore,
		timer_cb_t fct, void *arg)
{
	uint64_t cur_time = utaf_get_timer_cycles();
	uint64_t period;

	if (unlikely((tim_lcore != (unsigned)LCORE_ID_ANY) &&
			!mt_lcore_is_enabled(tim_lcore)))
		return UTAF_FAIL;

	if (type == PERIODICAL)
		period = ticks;
	else
		period = 0;

	__timer_reset(tim,  cur_time + ticks, period, tim_lcore,
			  fct, arg, 0);

	return UTAF_OK;
}


/* loop until rte_timer_reset() succeed */
void timer_reset_sync(struct timer *tim, uint64_t ticks,
		     enum timer_type type, unsigned tim_lcore,
		     timer_cb_t fct, void *arg)
{
	while (timer_reset(tim, ticks, type, tim_lcore,
			       fct, arg) != 0);
}

/* Stop the timer associated with the timer handle tim */
int utaf_timer_stop(struct timer *tim)
{
	union timer_status prev_status, status;
	unsigned lcore_id = utaf_lcore_id();
	int ret;

	/* wait that the timer is in correct status before update,
	 * and mark it as beeing configured */
	ret = timer_set_config_state(tim, &prev_status);
	if (ret < 0)
		return -1;

	__TIMER_STAT_ADD(stop, 1);
	priv_timer[lcore_id].updated = 1;

	/* remove it from list */
	if (prev_status.state == TIMER_PENDING ||
	    prev_status.state == TIMER_RUNNING) {
		timer_del(tim, prev_status.owner, 0);
		__TIMER_STAT_ADD(pending, -1);
	}

	/* mark timer as stopped */
	m_wmb();
	status.state = TIMER_STOP;
	status.owner = TIMER_NO_OWNER;
	tim->status.u32 = status.u32;

	return 0;
}

/* loop until rte_timer_stop() succeed */
void timer_stop_sync(struct timer *tim)
{
	while (utaf_timer_stop(tim) != 0)
		pause();
}

/* Test the PENDING status of the timer handle tim */
int timer_pending(struct timer *tim)
{
	return tim->status.state == TIMER_PENDING;
}

/* must be called periodically, run all timer that expired */
void timer_manage(uint32_t cid)
{
	union timer_status status;
	struct timer *tim, *tim2;
	unsigned lcore_id = cid;
    //unsigned j;
    //cpu_set_t get;
    uint64_t cur_time;
	int ret;

    #if 0
    pthread_getaffinity_np(pthread_self(), sizeof(get), &get);
    for (j = 0; j < 32; j++) {
        if (CPU_ISSET(j, &get)) {
            lcore_id = j;
        }
    }
    #endif

	__TIMER_STAT_ADD(manage, 1);
	/* optimize for the case where per-cpu list is empty */
	if (LIST_EMPTY(&priv_timer[lcore_id].pending))
		return;
	cur_time = utaf_get_timer_cycles();

	/* browse ordered list, add expired timers in 'expired' list */
	spinlock_lock(&priv_timer[lcore_id].list_lock);

	LIST_FOREACH_SAFE(tim, tim2, &priv_timer[lcore_id].pending, next) {
		if ((int64_t)(cur_time - tim->expire) < 0)
			break;

		LIST_REMOVE(tim, next);
		LIST_INSERT_HEAD(&priv_timer[lcore_id].expired, tim, next);
	}


	/* for each timer of 'expired' list, check state and execute callback */
	while ((tim = LIST_FIRST(&priv_timer[lcore_id].expired)) != NULL) {
		ret = timer_set_running_state(tim);

		/* remove from expired list, and add it in done list */
		LIST_REMOVE(tim, next);
		LIST_INSERT_HEAD(&priv_timer[lcore_id].done, tim, next);

		/* this timer was not pending, continue */
		if (ret < 0)
			continue;

		spinlock_unlock(&priv_timer[lcore_id].list_lock);

		priv_timer[lcore_id].updated = 0;

		/* execute callback function with list unlocked */
		tim->f(tim, tim->arg);

		spinlock_lock(&priv_timer[lcore_id].list_lock);

		/* the timer was stopped or reloaded by the callback
		 * function, we have nothing to do here */
		if (priv_timer[lcore_id].updated == 1)
			continue;

		if (tim->period == 0) {
			/* remove from done list and mark timer as stopped */
			LIST_REMOVE(tim, next);
			__TIMER_STAT_ADD(pending, -1);
			status.state = TIMER_STOP;
			status.owner = TIMER_NO_OWNER;
			m_wmb();
			tim->status.u32 = status.u32;
		}
		else {
			/* keep it in done list and mark timer as pending */
			status.state = TIMER_PENDING;
			status.owner = (int16_t)lcore_id;
			m_wmb();
			tim->status.u32 = status.u32;
		}
	}

	/* finally, browse done list, some timer may have to be
	 * rescheduled automatically */
	LIST_FOREACH_SAFE(tim, tim2, &priv_timer[lcore_id].done, next) {

		/* reset may fail if timer is beeing modified, in this
		 * case the timer will remain in 'done' list until the
		 * core that is modifying it remove it */
		__timer_reset(tim, cur_time + tim->period,
				  tim->period, lcore_id, tim->f,
				  tim->arg, 1);
	}

	/* job finished, unlock the list lock */
	spinlock_unlock(&priv_timer[lcore_id].list_lock);
}

/* dump statistics about timers */
void timer_dump_stats(void)
{
#ifdef RTE_LIBRTE_TIMER_DEBUG
	struct rte_timer_debug_stats sum;
	unsigned lcore_id;

	memset(&sum, 0, sizeof(sum));
	for (lcore_id = 0; lcore_id < MAX_LCORE; lcore_id++) {
		sum.reset += priv_timer[lcore_id].stats.reset;
		sum.stop += priv_timer[lcore_id].stats.stop;
		sum.manage += priv_timer[lcore_id].stats.manage;
		sum.pending += priv_timer[lcore_id].stats.pending;
	}
	printf("Timer statistics:\n");
	printf("  reset = %"PRIu64"\n", sum.reset);
	printf("  stop = %"PRIu64"\n", sum.stop);
	printf("  manage = %"PRIu64"\n", sum.manage);
	printf("  pending = %"PRIu64"\n", sum.pending);
#else
	printf("No timer statistics, RTE_LIBRTE_TIMER_DEBUG is disabled\n");
#endif
}
#endif /* wdb_ppp */

static void
check_tsc_flags(void)
{
	char line[512];
	FILE *stream;

	stream = fopen("/proc/cpuinfo", "r");
	if (!stream) {
		//RTE_LOG(WARNING, EAL, "WARNING: Unable to open /proc/cpuinfo\n");
		printf("WARNING: Unable to open /proc/cpuinfo\n");
		return;
	}

	while (fgets(line, sizeof line, stream)) {
		char *constant_tsc;
		char *nonstop_tsc;

		if (strncmp(line, "flags", 5) != 0)
			continue;

		constant_tsc = strstr(line, "constant_tsc");
		nonstop_tsc = strstr(line, "nonstop_tsc");
		if (!constant_tsc || !nonstop_tsc)
			/*
			RTE_LOG(WARNING, EAL,
				"WARNING: cpu flags "
				"constant_tsc=%s "
				"nonstop_tsc=%s "
				"-> using unreliable clock cycles !\n",
				constant_tsc ? "yes":"no",
				nonstop_tsc ? "yes":"no");
				*/
			printf("WARNING: cpu flags "
				"constant_tsc=%s "
				"nonstop_tsc=%s "
				"-> using unreliable clock cycles !\n",
				constant_tsc ? "yes":"no",
				nonstop_tsc ? "yes":"no");
		break;
	}

	fclose(stream);
}
static int
set_tsc_freq_from_cpuinfo(void)
{
	char line[256];
	FILE *stream;
	double dmhz;

	stream = fopen("/proc/cpuinfo", "r");
	if (!stream) {
		//RTE_LOG(WARNING, EAL, "WARNING: Unable to open /proc/cpuinfo\n");
		printf("WARNING: Unable to open /proc/cpuinfo\n");
		return -1;
	}

	while (fgets(line, sizeof line, stream)) {
		if (sscanf(line, "cpu MHz\t: %lf", &dmhz) == 1) {
			eal_tsc_resolution_hz = (uint64_t)(dmhz * 1000000UL);
			break;
		}
	}

	fclose(stream);

	if (!eal_tsc_resolution_hz) {
		//RTE_LOG(WARNING, EAL, "WARNING: Cannot read CPU clock from cpuinfo\n");
		printf("WARNING: Cannot read CPU clock from cpuinfo\n");
		return -1;
	}
	return 0;
}

static int
set_tsc_freq_from_clock(void)
{
#ifdef CLOCK_MONOTONIC_RAW
#define NS_PER_SEC 1E9

	struct timespec sleeptime = {.tv_nsec = 5E8 }; /* 1/2 second */

	struct timespec t_start, t_end;

	if (clock_gettime(CLOCK_MONOTONIC_RAW, &t_start) == 0) {
		uint64_t ns, end, start = utaf_rdtsc();
		nanosleep(&sleeptime,NULL);
		clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
		end = utaf_rdtsc();
		ns = ((t_end.tv_sec - t_start.tv_sec) * NS_PER_SEC);
		ns += (t_end.tv_nsec - t_start.tv_nsec);

		double secs = (double)ns/NS_PER_SEC;
		eal_tsc_resolution_hz = (uint64_t)((end - start)/secs);
		return 0;
	}
#endif
	return -1;
}

static void
set_tsc_freq_fallback(void)
{
	/*
	RTE_LOG(WARNING, EAL, "WARNING: clock_gettime cannot use "
			"CLOCK_MONOTONIC_RAW and HPET is not available"
			" - clock timings may be less accurate.\n");
	*/
	printf("WARNING: clock_gettime cannot use "
			"CLOCK_MONOTONIC_RAW and HPET is not available"
			" - clock timings may be less accurate.\n");
	
	/* assume that the sleep(1) will sleep for 1 second */
	uint64_t start = utaf_rdtsc();
	sleep(1);
	eal_tsc_resolution_hz = utaf_rdtsc() - start;
}

/*
 * This function measures the TSC frequency. It uses a variety of approaches.
 *
 * 1. If kernel provides CLOCK_MONOTONIC_RAW we use that to tune the TSC value
 * 2. If kernel does not provide that, and we have HPET support, tune using HPET
 * 3. Lastly, if neither of the above can be used, just sleep for 1 second and
 * tune off that, printing a warning about inaccuracy of timing
 */
static void set_tsc_freq(void)
{
	if (set_tsc_freq_from_cpuinfo() < 0 &&
	    set_tsc_freq_from_clock() < 0)
	    set_tsc_freq_fallback();

	//RTE_LOG(INFO, EAL, "TSC frequency is ~%"PRIu64" KHz\n",eal_tsc_resolution_hz/1000);
	printf("TSC frequency is ~%"PRIu64" KHz\n",eal_tsc_resolution_hz/1000);
}

void timer_source_init(void)
{

	_eal_timer_source = EAL_TIMER_TSC;

	set_tsc_freq();
	check_tsc_flags();

	UTAF_SYS_HZ = utaf_get_timer_hz();

    printf("UTAF_SYS_HZ: %lu\n", UTAF_SYS_HZ);
	
	return ;
}




