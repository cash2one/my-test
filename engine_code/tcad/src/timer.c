
#ifdef  __cplusplus
extern "C"
{
#endif

#if 1 /* wdb_ppp */
#define __WITHOUT_EMPTYING_THRD_CRTED__ 1

#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#endif /* wdb_ppp */

/* #include "wt_com.h"
#include "timer.h" //wdb_ppp */
#if 1 /* wdb */
#include "errno.h"
#endif /* wdb */

#if 1 /* wdb_as */
#include "atomic.h"  //wdb_without_thrd_crting
#include "misc.h"

extern void utaf_ssn_age_timer_cb(void);
//static int utaf_ssn_age_intval = 0;  //wdb_calc222
#endif /* wdb_as */
#ifdef UTAF_URL
#include "url_log.h"
#endif

#if 1 /* __wdb__ */
volatile time_t g_utaf_time_jiffies;
#endif /* __wdb__ */

/* static st_timer_header *g_pstTimerHead = NULL; //wdb_ppp */

uint32_t timer_adjust(time_t cur_time, time_t stime, uint32_t cycle)
{
    while (cur_time > (stime + cycle)) {
        stime += cycle;
    }

    return (stime + cycle);
    //return (cur_time - stime);
}

#if 0 /* wdb_calc222 */
/* 删除定时器 */
static int32_t timer_del(st_timer *timer_pre)
{
    int32_t rc = 0;
    st_timer *timer_tmp = NULL;
    /* 要删除定时器的前一个是空，说明要
       * 删除的是定时器列表的第一个
       */
    if (NULL == timer_pre)
    {
        timer_tmp = TIMER_HEAD(g_pstTimerHead);
        TIMER_HEAD(g_pstTimerHead) = timer_tmp->next;
        FREE(timer_tmp);
        TIMER_NUM(g_pstTimerHead)--;
        goto end;
    }

    /* 要删除的是空定时器 */
    if (NULL == timer_pre->next)
    {
        rc = -1;
        goto    end;
    }
    timer_tmp = timer_pre->next;
    timer_pre->next = timer_pre->next->next;
    FREE(timer_tmp);
    TIMER_NUM(g_pstTimerHead)--;

end:
    return rc;
}
#endif /* wdb_calc222 */

#if 1 /* wdb_as */
#ifndef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
static void *utaf_ssn_tmTick_thrd(void *data)
{
    utaf_ssn_age_timer_cb();

    return NULL;
}

static void utaf_ssn_tmTick_thrd_crt(void)
{
    pthread_t tid;

    if ( pthread_create(&tid, NULL, utaf_ssn_tmTick_thrd, NULL) != 0 )
    {
        fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
        return;
    }
    if ( pthread_detach(tid) != 0 )
    {
        fprintf(stderr, "pthread_detach() failed. - %s\n", strerror(errno));
    }
}
#endif /* !__WITHOUT_EMPTYING_THRD_CRTED__ */
#endif /* wdb_as */

#if 0 /* wdb_lfix-5 */
static volatile int __retry = 0; //wdb_without_thrd_crting
#endif /* wdb_lfix-5 */

#if 1 /* wdb_lfix-5 */
static __inline__ void utaf_atomic_add64(unsigned long i, volatile time_t *target)
{
        __asm__ __volatile__(
                "lock; add %1,%0"
                :"=m" (*target)
                :"ir" (i), "m" (*target));
}
#endif /* wdb_lfix-5 */

static void *traffic_dump_output_tmTick_thrd(void *data)
{
    extern time_t g_traffic_last_time;
    extern uint32_t flow_interval;

    if (g_utaf_time_jiffies > (g_traffic_last_time + flow_interval)) {
        g_traffic_last_time = g_utaf_time_jiffies;

        extern void traffic_dump_output(void);
        traffic_dump_output();
    }

    return NULL;
}

static void traffic_dump_output_tmTick_thrd_crt(void)
{
    pthread_t tid;

    if ( pthread_create(&tid, NULL, traffic_dump_output_tmTick_thrd, NULL) != 0 )
    {
        fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
        return;
    }
    if ( pthread_detach(tid) != 0 )
    {
        fprintf(stderr, "pthread_detach() failed. - %s\n", strerror(errno));
    }
}

/* 定时器处理函数 */
#if 1 /* wdb */
static void timer_handler(int32_t sig)
#else /* wdb */
static void *timer_handler_thread(void *cb)
#endif /* wdb */
{
#if 0 /* wdb_lfix-5 */
    st_timer **p_timer = NULL;
    st_timer *timer_cur = NULL;
    time_t cur_time; //time_t cur_time = time(NULL);
#if 1 /* wdb */
    time_t tst_stop;
#endif
#endif /* wdb_lfix-5 */

#if 1 /* wdb_as */

 /* g_utaf_time_jiffies++; //wdb_lfix-5 */
    utaf_atomic_add64(1, &g_utaf_time_jiffies);  //wdb_lfix-5

#if 0 /* wdb_lfix-5 */
    cur_time = g_utaf_time_jiffies;
#endif /* wdb_lfix-5 */

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
    utaf_ssn_age_timer_cb();
#else /* !__WITHOUT_EMPTYING_THRD_CRTED__ */
    utaf_ssn_tmTick_thrd_crt();
#endif /* !__WITHOUT_EMPTYING_THRD_CRTED__ */

#if 1 /* wzh_db */
    traffic_dump_output_tmTick_thrd_crt();
#endif

#endif /* wdb_as */

#if 0 /* wdb_lfix-5 */
    if ( !__retry )
    {
        __retry = 1;
        m_wmb();  //wdb_without_thrd_crting
#else /* wdb_lfix-5 */
#ifdef UTAF_DDOS
		extern void ddos_timer_handle(void);
		ddos_timer_handle();
#endif
#ifdef UTAF_URL
		add_url_timer();
#endif
}

#if 0 /* wdb_ppp */
void utaf_timer_handler_cb(void)
{
    st_timer **p_timer = NULL;
    st_timer *timer_cur = NULL;
    time_t cur_time = g_utaf_time_jiffies;
#if 1 /* wdb */
    time_t tst_stop;
#endif
#endif /* wdb_ppp */

#endif /* wdb_lfix-5 */

#if 0 /* wdb_ppp */
    p_timer = &(TIMER_HEAD(g_pstTimerHead));

    /* 遍历定时器列表 */
    for (timer_cur = *p_timer; timer_cur != NULL; timer_cur = timer_cur->next)
    {
        /* 依据当前定时器状态操作 */
        switch(timer_cur->state)
        {
            /* 定时器尚未正式启动 */
            case TIMER_S_INIT:
                /* 当前时间达到起始条件 */
                if ((0 != timer_cur->stime) && (cur_time >= timer_cur->stime))
                {
                    timer_cur->timer_cb(TIMER_TYPE_STIME, timer_cur->arg);
                    timer_cur->state = TIMER_S_START;
                }

                /* 计算下一次到达任务周期的时间 */
                timer_cur->ctime = timer_adjust(cur_time, timer_cur->stime, timer_cur->cycle);
                break;
            /* 定时器已经启动 */
            case TIMER_S_START:                
                /* 当前时间达到结束条件 */
                if ((0 != timer_cur->etime) && (cur_time >= timer_cur->etime))
                {
                    timer_cur->timer_cb(TIMER_TYPE_ETIME, timer_cur->arg);
                
                    /* 定时器终止 */
                    timer_cur->state = TIMER_S_STOP;
                }
                else
                {
                    /* 当前时间达到定时周期 */
                    //timer_cur->cur_sec ++;
                    if (0 == timer_cur->cycle)
                    {
                        break;
                    }
                    //if (timer_cur->cur_sec >= timer_cur->cycle)
                    if (cur_time >= timer_cur->ctime)
                    {
                        //timer_cur->cur_sec = 0;
                        timer_cur->ctime += timer_cur->cycle;
                        timer_cur->timer_cb(TIMER_TYPE_CYCLE, timer_cur->arg);
                    }
                }
                break;
            /* 定时器终止 */
            case TIMER_S_STOP:
                timer_cur->timer_cb(TIMER_TYPE_STOP, timer_cur->arg);
                *p_timer = timer_cur->next;
                FREE(timer_cur);
                timer_cur = *p_timer;
                TIMER_NUM(g_pstTimerHead)--;
                if ((!TIMER_NUM(g_pstTimerHead)) || (NULL == timer_cur))
                {
                    goto end;
                }
                break;
        }
        p_timer = &(timer_cur->next);
    }

end:

#if 0 /* wdb_lfix-5 */
        m_wmb();  //wdb_without_thrd_crting
        __retry = 0;
    }
#endif /* wdb_lfix-5 */

#if 1 /* wdb */
    tst_stop = g_utaf_time_jiffies; //tst_stop = time(NULL);
    if ( (tst_stop - cur_time) > 0 )
    {
        printf("wdb: __BUG__ !!! Timer handler is not completed in one second! Number of second is %lu.\n", (tst_stop - cur_time));
    }
#endif

#if 1 /* wdb */
    return;
#else /* wdb */
    return NULL;
#endif /* wdb */
}
#endif /* wdb_ppp */

#if 0 /* wdb */
static void timer_handler(int32_t sig)
{
    pthread_t tid;

    if ( pthread_create(&tid, NULL, timer_handler_thread, NULL) != 0 )
    {
        fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
        return;
    }
    if ( pthread_detach(tid) != 0 )
    {
        fprintf(stderr, "pthread_detach() failed. - %s\n", strerror(errno));
    }
}
#endif /* wdb */

#if 0 /* wdb_ppp */
st_timer *timer_new(void)
{
    st_timer *rc = NULL;

    rc = (st_timer *)MALLOC(sizeof(st_timer));
    if (NULL == rc)
    {
        return rc;
    }

    rc->state = TIMER_S_INIT;
    rc->one_c = 0;
    rc->stime = 0;
    rc->etime = 0;
    rc->ctime = 0;
    rc->cur_sec = 0;
    rc->cycle = 0;
    rc->arg = NULL;
    rc->timer_cb = NULL;
    rc->next = NULL;

    return rc;
}

int32_t timer_start(st_timer *timer)
{
    if (NULL == timer)
    {
        return -1;
    }

    /* 将定时器插入列表头 */
    timer->next = TIMER_HEAD(g_pstTimerHead);
    TIMER_HEAD(g_pstTimerHead) = timer;
    TIMER_NUM(g_pstTimerHead)++;

    return 0;
}

void timer_stop(st_timer *timer)
{
    if (NULL == timer)
    {
        DEBUG(WT_DEBUG_ERR, "%s: In null\n", __FUNCTION__);
        return;
    }

    timer->state = TIMER_S_STOP;

    return;
}

void timer_clear(st_timer *timer)
{
    time_t cur_time = g_utaf_time_jiffies; //time_t cur_time = time(NULL);
    
    if (NULL == timer)
    {
        DEBUG(WT_DEBUG_ERR, "%s: In null\n", __FUNCTION__);
        return;
    }

    timer->cur_sec = 0;
    timer->ctime = timer_adjust(cur_time, timer->stime, timer->cycle);

    return;
}

int32_t IsTimerOne(st_timer *timer)
{
    if (NULL == timer)
    {
        DEBUG(WT_DEBUG_ERR, "%s: In null\n", __FUNCTION__);
        return -1;
    }

    return timer->one_c;
}
#endif /* wdb_ppp */

int32_t TimerListInit(void) //wdb_ppp
{
    struct itimerval value, ovalue;
    struct sigaction sg_act;

#if 1 /* __wdb__ */
    g_utaf_time_jiffies = time(NULL);

    extern time_t g_traffic_last_time;
    g_traffic_last_time = g_utaf_time_jiffies;
#endif /* __wdb__ */

    value.it_value.tv_sec = 1;
    value.it_value.tv_usec = /* 500000 */ 0;
    value.it_interval.tv_sec = 1;
    value.it_interval.tv_usec = /* 500000 */ 0;
    
    /* 初始化定时器列表 */
#if 0 /* wdb_ppp */
    g_pstTimerHead = (st_timer_header *)MALLOC(sizeof(st_timer_header));
    if (NULL == g_pstTimerHead)
    {
        return -1;
    }

    g_pstTimerHead->num = 0;
    g_pstTimerHead->head = NULL;
    g_pstTimerHead->tail = NULL;
#endif /* wdb_ppp */

    setitimer(ITIMER_REAL, &value, &ovalue);
#if 1
    sg_act.sa_handler  = timer_handler;
    sg_act.sa_mask     = (sigset_t){{0,}};
    sg_act.sa_flags    = SA_RESTART | SA_NODEFER;
    sg_act.sa_restorer = NULL;

    if ( sigaction(SIGALRM, &sg_act, NULL) != 0 )
    {
        fprintf(stderr, "sigaction() failed. exit...\n");
        return -2;
    }
#else
    /* 定时器操作 */
    signal(SIGALRM, timer_handler);
#endif

#if 0 /* wzh_db */ //wdb_ppp
    extern FILE *g_netflow_max_bytes_fp;
    g_netflow_max_bytes_fp = fopen("/tmp/.tcad_wzh_netflow", "a+");
#endif
    
    return 0;
}

#if 0 /* wdb_ppp */
void TimerListDestory(void)
{
    FREE(g_pstTimerHead);
}
#endif /* wdb_ppp */

#ifdef  __cplusplus
}
#endif

