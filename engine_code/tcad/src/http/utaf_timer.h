#ifndef __UTAF_TIMER_H__
#define __UTAF_TIMER_H__

/* #include "utaf.h" */
#include <time.h>
#include <sys/time.h>
#include <sys/queue.h>
#include "utaf_cycle.h"


enum timer_source {
	EAL_TIMER_TSC = 0,
	EAL_TIMER_HPET,
};

#define TIMER_CORE_ID       (1)
#define TIMER_CYCLE_PER_MS  (UTAF_SYS_HZ/1000)  // cycles per millisecond 


#define TIMER_STOP    0 /**< State: timer is stopped. */
#define TIMER_PENDING 1 /**< State: timer is scheduled. */
#define TIMER_RUNNING 2 /**< State: timer function is running. */
#define TIMER_CONFIG  3 /**< State: timer is being configured. */

#define TIMER_NO_OWNER -1 /**< Timer has no owner. */


/**
 * Timer type: Periodic or single (one-shot).
 */
enum timer_type {
	SINGLE,
	PERIODICAL
};


/**
 * Timer status: A union of the state (stopped, pending, running,
 * config) and an owner (the id of the lcore that owns the timer).
 */
union timer_status {
	struct {
		uint16_t state;  /**< Stop, pending, running, config. */
		int16_t owner;   /**< The lcore that owns the timer. */
	};
	uint32_t u32;            /**< To atomic-set status + owner. */
};

struct timer;
typedef void (timer_cb_t)(struct timer *, void *);

/**
 * A structure describing a timer in RTE.
 */
struct timer
{
	LIST_ENTRY(timer) next;    /**< Next and prev in list. */
	volatile union timer_status status; /**< Status of timer. */
	uint64_t period;       /**< Period of timer (0 if not periodic). */
	uint64_t expire;       /**< Time when timer expire. */
	timer_cb_t *f;     /**< Callback function. */
	void *arg;             /**< Argument to callback function. */
};

extern uint64_t timer_get_tsc_hz(void);

extern enum timer_source _eal_timer_source;
/**
 * Get the number of cycles in one second for the default timer.
 *
 * @return
 *   The number of cycles in one second.
 */
static inline uint64_t utaf_get_timer_hz(void)
{

	switch(_eal_timer_source)
	{
		case EAL_TIMER_TSC:
			return timer_get_tsc_hz();
		case EAL_TIMER_HPET:
	#ifdef RTE_LIBEAL_USE_HPET
			return rte_get_hpet_hz();
	#endif
	//default: rte_panic("Invalid timer source specified\n");
		default: printf("Invalid timer source specified\n");	
	}

	return 1000000000;
}

/**
 * Get the number of cycles since boot from the default timer.
 *
 * @return
 *   The number of cycles
 */
static inline uint64_t utaf_get_timer_cycles(void)
{
	switch(_eal_timer_source) {
	case EAL_TIMER_TSC:
		return utaf_rdtsc();
	case EAL_TIMER_HPET:
#ifdef RTE_LIBEAL_USE_HPET
		return rte_get_hpet_cycles();
#endif
	//default: rte_panic("Invalid timer source specified\n");
	default: printf("Invalid timer source specified\n");

	}

	return 0;
}

static inline uint64_t utaf_get_timer(void)
{
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

#define UTAF_DEBUG_CYCLE_PRINT_TIME(__func__, __action__) do { \
    struct timespec ts; \
    clock_gettime(CLOCK_MONOTONIC, &ts); \
    printf(#__func__" "#__action__": %d s, %d ns.\n", ts.tv_sec, ts.tv_nsec); \
} while(0)

extern uint32_t timer_reset(struct timer *tim, uint64_t ticks,
		enum timer_type type, unsigned tim_lcore,
		timer_cb_t fct, void *arg);



extern void utaf_timer_subsystem_init(void);
extern int utaf_timer_stop(struct timer *tim);
extern void timer_init(struct timer *tim);
extern void timer_manage(uint32_t cid);












#endif
