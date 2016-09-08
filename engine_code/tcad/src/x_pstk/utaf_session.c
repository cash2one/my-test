#if 1 /* wdb_add */
//#define _GNU_SOURCE
#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "atomic.h"
#include "list.h"
#include "misc.h"
#include "spinlock.h"
#include "ringq.h"
#include "mbuf.h"
#include "ip4.h"
#include "tcp4.h"
#include "statistics.h"
#include "httpX.h"
#endif /* wdb_add */
#include "rwlock.h"
#if 0
#include "module_intf.h"
#endif
#include "utaf_session.h"
#if 0
#include "utaf_timer.h"
#include "utaf_stream.h"
#endif
#include "tluhash.h"
#if 0
#include "ip4.h"
#endif
#include "spinlock.h"
#include "common.h"
#if 0
#include "ringq.h"
#include <statistics.h>
#include "utaf_flows.h"
#include "utaf_timer.h"
#include "utaf_age.h"
#endif
#include "mbuf.h"
#if 0
#include "applib.h"
#endif
#include "app.h"
#include "iplib.h"
#if 0 /* wdb */
#include <numaif.h>
#endif /* wdb */
#if 0
#include "wt_com.h"  //wdb_lfix
#endif
#include "cdpi_api.h"
#include "ddos.h"

#ifdef UTAF_DDOS
extern int ddos_init_flow(session_item_t *si, struct m_buf *mbuf);
extern void ddos_flow_statistic_d(session_item_t *si);
extern uint32_t ddos_statistic_tcp_connect(session_item_t *si, uint32_t flag);
extern uint32_t ddos_statistic(session_item_t *si);

#endif

extern int g_thread_num;
extern void output_age_record(session_item_t *ssn);
extern void output_inc_record(session_item_t *ssn);

//uint32_t session_count = 0;

uint32_t session_alloc_count = 0;
uint32_t session_free_count = 0;

uint64_t session_item_num = 0;
uint64_t session_bucket_num = 0;
uint64_t session_bucket_mask = 0;
uint64_t session_age_thread_num = 0;

uint64_t session_item_member_mask = SESSION_ITEM_MEMBER_MASK;

session_table_info_t session_table;

struct session_item_ringq_group session_item_q_group;

//struct ringq *session_item_ringq_group[MAX_LCORE] = { 0 };

//session_bucket_t session_bucket[UTAF_SESSION_TABLE_SIZE];

#if 1 /* wdb_as */
extern int g_page_sz;
//extern uint64_t g_stTaskListCksum; //wdb_ppp
//extern uint32_t utaf_cdr_session_age_output(session_item_t *si, time_t end_time); //wdb_ppp
//extern uint32_t utaf_cdr_session_inc_output(session_item_t *si); //wdb_ppp
//extern int utaf_no_task_for_ssn(session_item_t *ssn, uint64_t cksum); //wdb_ppp

volatile int utaf_age_index_current = 0; //wdb_without_thrd_crting: 'volatile' is added this time
#if 0 /* wdb_without_thrd_crting */
rwlock_t utaf_age_index_lock;
#endif

struct hlist_head g_utaf_timeout_ring[UTAF_AGE_RING_SIZE][16];
spinlock_t        g_utaf_timeout_rlck[UTAF_AGE_RING_SIZE][16];

#if 0 /* wdb_sglpkt_age */
volatile int utaf_age0_index_current = 0;

struct hlist_head g_utaf_timeout_ring0[UTAF_AGE_RING0_SIZE][16];
spinlock_t        g_utaf_timeout_r0lck[UTAF_AGE_RING0_SIZE][16];
#endif /* wdb_sglpkt_age */

#ifdef UTAF_AS2_DEBUG
int g_utaf_timeout_rcnt[UTAF_AGE_RING_SIZE][16];
int g_utaf_timeout_r0cnt[UTAF_AGE_RING0_SIZE][16];  //wdb_sglpkt_age
#endif

#if 1 /* wdb_lfix-5 */
//extern void utaf_timer_handler_cb(void); //wdb_ppp
#endif /* wdb_lfix-5 */
#endif /* wdb_as */

#if 1 /* __wdb__ */
extern volatile time_t g_utaf_time_jiffies;
#endif /* __wdb__ */

#if 1 /* wdb_core */
extern int __misc_cores;
extern int __calcu_cores[];
#endif /* wdb_core */

/* static struct timer session_age[SESSION_MAX_AGE_CORE]; //wdb_ppp */
static struct utaf_session_age_core_s session_age_core[SESSION_MAX_AGE_CORE+1] = {
    {1,  1,     0,          2796202,    NULL},
    {2,  2,     2796202,    5592404,    NULL},
    {3,  3,     0,          2796202,    NULL},
    {4,  4,     2796202,    5592404,    NULL},
    {5,  5,     0,          2796202,    NULL},
    {6,  6,     2796202,    5592404,    NULL},     
    {7,  7,     5592404,    8388606,    NULL},
    {8,  8,     8388606,    11184808,   NULL},
    {9,  9,     0,          2796202,    NULL},
    {10, 10,    2796202,    5592404,    NULL},
    {11, 11,    0,          2796202,    NULL},
    {12, 12,    2796202,    5592404,    NULL},
    {13, 13,    0,          2796202,    NULL},
    {14, 14,    2796202,    5592404,    NULL},     
    {15, 15,    5592404,    8388606,    NULL},
    {16, 16,    8388606,    11184808,   NULL}
};

extern uint64_t UTAF_SYS_HZ;

#if 1 /* __wdb__ */
static inline void rte_prefetch0(volatile void *p)
{
        asm volatile ("prefetcht0 %[p]" : [p] "+m" (*(volatile char *)p));
}
#endif /* __wdb__ */

inline void session_update_isp_info(session_item_t *si)
{
    iplib_info_t ipinfo;
#if 0
    if (0xFFFF != si->s_isp_id || 0xFFFF != si->d_isp_id) {
        return;
    }
#endif
    if (IPV4_VERSION == si->ip_version) {
        if (IPLIB_OK == iplib_find((si->ip_tuple.sip[0]), &ipinfo)) {
            si->country = ipinfo.country;
            si->s_isp_id = ipinfo.isp_id;
            si->s_province_id = ipinfo.province_id;
            si->s_city_id = ipinfo.city_id;
        } else {
            si->country = 0;
            si->s_isp_id = 0;
            si->s_province_id = 0;
            si->s_city_id = 0;
        }

        if (IPLIB_OK == iplib_find((si->ip_tuple.dip[0]), &ipinfo)) {
            si->country = ipinfo.country;
            si->d_isp_id = ipinfo.isp_id;
            si->d_province_id = ipinfo.province_id;
            si->d_city_id = ipinfo.city_id;
        } else {
            si->country = 0;
            si->d_isp_id = 0;
            si->d_province_id = 0;
            si->d_city_id = 0;
        }
    } else {
        si->country = 0;
        si->s_isp_id = 0;
        si->s_province_id = 0;
        si->s_city_id = 0;
        si->d_isp_id = 0;
        si->d_province_id = 0;
        si->d_city_id = 0;
    }

    return;
}

unsigned int sessionhashfn(uint32_t saddr, uint32_t daddr, uint16_t sport, uint16_t dport, uint8_t prot)
{
    return session_hashfn(prot, saddr, daddr, sport, dport) & session_bucket_mask;
}

static int session_match(session_item_t *si, struct m_buf *mbuf)
{
    //脮媒脧貌
    if(si->ip_tuple.sip[0] == mbuf->ip_tuple.sip[0]
        && si->ip_tuple.dip[0] == mbuf->ip_tuple.dip[0]
        && si->sport == mbuf->sport
        && si->dport == mbuf->dport
        && si->protocol == mbuf->protocol)
    {
//#ifdef UTAF_LOCK_SPINLOCK  //wdb_lfix-6
        spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//        write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

        if (TH_SYNACK == (mbuf->tcp_f & TH_SYNACK)) {
            si->tcp_flag |= 0x2;                // bit1 syn&ack
            //si->direct_flags = DOWN_DIRECTION;
        } else if (TH_SYN == (mbuf->tcp_f & TH_SYN)) {
            si->tcp_flag |= 0x1;                // bit0 syn
            //si->direct_flags = UP_DIRECTION;
        } else if (TH_ACK == (mbuf->tcp_f & TH_ACK)) {
            si->tcp_flag |= 0x4;                // bit1 ack
            //si->direct_flags = UP_DIRECTION;
        }

#ifdef UTAF_DEBUG_SESSION
        printf("tcp_flag: %u, direct_flags: %u\n", si->tcp_flag, si->direct_flags);
#endif

        //if (FLOW_CREATE_FAIL == mbuf->flow_c_stat)
        //{
            //DEBUG_SESSION_STAT_RAW_PKT_SESSION_FAIL;
            si->pkts[0]++;
            si->bytes[0] += mbuf->total_len;
        //}
        /*else if (APP_DNS == si->app_id || APP_HTTP == si->app_id)
        {
            si->pkts[0]++;
            si->bytes[0] += mbuf->total_len;
        }*/

#if 0
        if (PROTO_TCP == mbuf->protocol && IS_OTHER_IP_DEFRAG != mbuf->ipdefrag_flag)
        {
            tcp_seq_process(si, &si->tcpSi[0], mbuf->tcp_seq, &si->tcpsegi[0]);
        }
#endif

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//        write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6
#ifdef UTAF_DDOS
		mbuf->pkt_dir = (dir_in == si->flow_dir) ? dir_in : dir_out;
#endif
        return 1;
    }
    //路麓脧貌
    else if(si->ip_tuple.sip[0] == mbuf->ip_tuple.dip[0]
        && si->ip_tuple.dip[0] == mbuf->ip_tuple.sip[0]
        && si->sport == mbuf->dport
        && si->dport == mbuf->sport
        && si->protocol == mbuf->protocol)
    {
//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//        write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

        if (TH_SYNACK == (mbuf->tcp_f & TH_SYNACK)) {
            si->tcp_flag |= 0x2;                // bit1 syn&ack
            //si->direct_flags = UP_DIRECTION;
        } else if (TH_SYN == (mbuf->tcp_f & TH_SYN)) {
            si->tcp_flag |= 0x1;                // bit0 syn
            //si->direct_flags = DOWN_DIRECTION;
        } else if (TH_ACK == (mbuf->tcp_f & TH_ACK)) {
            si->tcp_flag |= 0x4;                // bit1 ack
            //si->direct_flags = DOWN_DIRECTION;
        }

#ifdef UTAF_DEBUG_SESSION
        printf("tcp_flag: %u, direct_flags: %u\n", si->tcp_flag, si->direct_flags);
#endif

        //if (FLOW_CREATE_FAIL == mbuf->flow_c_stat)
        //{
            //DEBUG_SESSION_STAT_RAW_PKT_SESSION_FAIL;
            si->pkts[1]++;
            si->bytes[1] += mbuf->total_len;
            //printf("d byte is %d\n", mbuf->total_len);
        //}
        /*else if (APP_DNS == si->app_id || APP_HTTP == si->app_id)
        {
            si->pkts[1]++;
            si->bytes[1] += mbuf->total_len;
        }*/

#if 0
        if (PROTO_TCP == mbuf->protocol && IS_OTHER_IP_DEFRAG != mbuf->ipdefrag_flag)
        {
            tcp_seq_process(si, &si->tcpSi[1], mbuf->tcp_seq, &si->tcpsegi[1]);
        }
#endif

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//        write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6
#ifdef UTAF_DDOS
		mbuf->pkt_dir = (dir_in == si->flow_dir) ? dir_out : dir_in;
#endif

        return 1;
    }
    else
    {
        return 0;
    }
}

#ifdef __WDB_SCO__ /* wdb_sco */
static struct hlist_head utaf_l_si_bunch[16];
#endif /* __WDB_SCO__ */ /* wdb_sco */

session_item_t * session_item_alloc(void)  //wdb_calc222
{
#ifndef __WDB_SCO__ /* wdb_sco */
    //return (session_item_t *)malloc(sizeof(session_item_t));
    uint64_t obj;
#if 0
    int lcore_id = utaf_lcore_id();

    if(RINGQ_OK != ringq_dequeue(session_item_ringq_group[lcore_id], &obj))
    {
        return NULL;
    }
    else
    {
        atomic32_add((atomic32_t *)&session_alloc_count, 1);
        return (session_item_t*)obj;
    }
#endif

    uint32_t index = atomic32_add_return((atomic32_t *)&session_item_q_group.global_index, 1);
    index = index & session_item_member_mask;
    if(RINGQ_OK != ringq_dequeue(session_item_q_group.session_item_ringq[index], &obj))
    {
        return NULL;
    }
    else
    {
        return (session_item_t*)obj;
    }
#else /* defined(__WDB_SCO__) */ /* wdb_sco */

    session_item_t *si;

    if (utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first)
    {
        si = (session_item_t *)utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first;
        utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first = si->list.next;
    }
    else
    {
        uint32_t index = atomic32_add_return((atomic32_t *)&session_item_q_group.global_index, 1);
        index = index & session_item_member_mask;

        spinlock_lock(&session_item_q_group.session_item_rq_lock[index]);
        if (session_item_q_group.session_item_ringq[index].first)
        {
            si = container_of(session_item_q_group.session_item_ringq[index].first, session_item_t, tm_lst);
            session_item_q_group.session_item_ringq[index].first = si->tm_lst.next;
        }
        else
        {
            si = NULL;
        }
        spinlock_unlock(&session_item_q_group.session_item_rq_lock[index]);
        if (si)
        {
            utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first = si->list.next;
        }
    }

    return si;
#endif /* defined(__WDB_SCO__) */ /* wdb_sco */
}

#ifndef __WDB_SCO__ /* wdb_sco */
void session_item_free(session_item_t *si)
{
    //free(si);
    int lcore_id;

    session_item_header *sih = (session_item_header *)((uint8_t *)si - SESSION_ITEM_HEADER_SIZE);
    if(sih->session_item_magic != SESSION_MAGIC)
    {
        printf("error session item header is destroy %p\n", sih);
    }

    lcore_id = sih->ringq_id;

#if 0
    memset((void *)si, 0, sizeof(session_item_t));

#ifdef UTAF_LOCK_SPINLOCK
    spinlock_init(&(si->item_lock));
#else
    rwlock_init(&(si->item_lock));
#endif
#endif

    if(RINGQ_OK != ringq_enqueue(session_item_q_group.session_item_ringq[lcore_id], (uint64_t)si))
    {
        printf("session_item release fail %d count is %d\n", lcore_id, ringq_count(session_item_q_group.session_item_ringq[lcore_id]));
    }

    return;
}
#else /* defined(__WDB_SCO__) */ /* wdb_sco */

void session_item_free(session_item_t *si)
{
    int lthrd_id;
    session_item_header *sih = (session_item_header *)((uint8_t *)si - SESSION_ITEM_HEADER_SIZE);
    lthrd_id = sih->ringq_id;

    spinlock_lock(&session_item_q_group.session_item_rq_lock[lthrd_id]);
    si->tm_lst.next = session_item_q_group.session_item_ringq[lthrd_id].first;
    session_item_q_group.session_item_ringq[lthrd_id].first = &si->tm_lst;
    spinlock_unlock(&session_item_q_group.session_item_rq_lock[lthrd_id]);
}

#endif /* defined(__WDB_SCO__) */ /* wdb_sco */

session_item_t *session_find(session_bucket_t *base, struct m_buf *mbuf, unsigned int hash)
{
    session_item_t *si;
    struct hlist_node *n;
    struct hlist_node *t;

#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]session find\n");
#endif

    #ifdef UTAF_LOCK_SPINLOCK
    spinlock_lock(&base[hash].lock);
    #else
    read_lock(&base[hash].lock);
    #endif

    //printf("find head address is %p\n", &session_bucket[hash].hash);
    //hlist_for_each_entry(si, n, &base[hash].hash, list)
    hlist_for_each_entry_safe(si, t, n, &base[hash].hash, list)
    {
        if (session_match(si, mbuf))
        {
            #ifdef UTAF_DEBUG_SESSION
            printf("[SESSION]session match\n");
            #endif
            #ifdef UTAF_LOCK_SPINLOCK
            spinlock_unlock(&base[hash].lock);
            #else
            read_unlock(&base[hash].lock);
            #endif
            return si;
        }
    }

    #ifdef UTAF_LOCK_SPINLOCK
    spinlock_unlock(&base[hash].lock);
    #else
    read_unlock(&base[hash].lock);
    #endif

    return NULL;
}

uint32_t session_add(session_bucket_t *base, unsigned int hash, struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]session add\n");
#endif

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_item_alloc, in);
#endif

    session_item_t *si = session_item_alloc();
#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_item_alloc, out);
#endif

    if(NULL == si)
    {
        DEBUG_SESSION_STAT_SESSION_FULL;
        return UTAF_MEM_FAIL;
    }

#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]add item addr is %p\n", si);
#endif

#if 1 /* __wdb__ */
    {
        int i;
        for (i = 0; i < 7; i++)
        {
            rte_prefetch0((void *)((char *)si + 64*i));
        }
    }
#endif /* __wdb__ */

#if 0 /* __wdb__ */
    {
        //size_t off_num = (unsigned long)(&si->app_id) - (unsigned long)si;
        //memset((void *)((char *)si + off_num), 0, sizeof(session_item_t) - off_num);
    }
#endif /* __wdb__ */

    //memset((void *)si->tasks, 0, 16);
    //si->tsk_cksum = 0;
    //si->tsk_num   = 0;
    //si->tsk_none  = 0;

    /*TODO:fill info*/
    si->start_time = g_utaf_time_jiffies;
    si->cycle = g_utaf_time_jiffies; //utaf_get_timer_cycles(); //wdb_ppp
    si->last_log_cycle = si->cycle;
    //si->session_id = ((si->cycle & 0xffff) << 16 ) | hash;
    si->service_type = APP_UNABLE_TO_IDENTIFY;
    si->service_group = APP_UNABLE_GROUP_ID;
    *((uint32_t *)(&(si->age_ring))) = 0;
    si->app_id = 0;
    si->lock = 0;
    si->app_flag = 0xFF;

    if (mbuf->ip_version == IPV4_VERSION)
    {
        si->tcp_flag = 0;
        if (TH_SYNACK == (mbuf->tcp_f & TH_SYNACK)) {
            si->tcp_flag |= 0x2;                // bit1 syn&ack
            //si->direct_flags = DOWN_DIRECTION;
        } else if (TH_SYN == (mbuf->tcp_f & TH_SYN)) {
            si->tcp_flag |= 0x1;                // bit0 syn
            //si->direct_flags = UP_DIRECTION;
        } else if (TH_ACK == (mbuf->tcp_f & TH_ACK)) {
            si->tcp_flag |= 0x4;                // bit1 ack
            //si->direct_flags = UP_DIRECTION;
        } else {
            //si->direct_flags = UP_DIRECTION;
        }

#ifdef UTAF_DEBUG_SESSION
        printf("si->tcp_flag =0x%x\n", si->tcp_flag & 0x7);
        printf("si->direct_flags =0x%x\n", si->direct_flags);
#endif

        si->ip_tuple.sip[0] = mbuf->ip_tuple.sip[0];
        si->ip_tuple.dip[0] = mbuf->ip_tuple.dip[0];

#if 0
        if (UP_DIRECTION == si->direct_flags) {
            si->ip1_eth_id = mbuf->eth_id;
        } else {
            si->ip2_eth_id = mbuf->eth_id;
        }
#endif
        si->sport = mbuf->sport;
        si->dport = mbuf->dport;
        si->protocol = (uint8_t)mbuf->protocol;
        si->ip_version = (uint8_t)mbuf->ip_version;
        si->pkts[0] = 1;
        si->bytes[0] = mbuf->total_len;
        si->pkts[1] = 0;
        si->bytes[1] = 0;
#ifdef UTAF_DDOS
		if (g_ddos_conf.sw) {
			si->tcp_state = TCP_NONE;
			si->ddos_flag = 0;
			si->ddos_ssn_new = 0;
			si->ddos_ssn_close = 0;
			si->ddos.last_log_time = si->cycle;
			memset(&si->ddos.stat, 0x0, sizeof(si->ddos.stat));
			ddos_init_flow(si, mbuf);
		}
#endif
#if 0
        if (PACKET_RAW == mbuf->pkt_type)
        {
            if (FLOW_CREATE_OK == mbuf->flow_c_stat)
            {
                si->inter_flow_stat = FLOW_CREATE_OK;
            }
            else //FLOW_CREATE_FAIL
            {
                DEBUG_SESSION_STAT_RAW_PKT_SESSION_FAIL;
                si->inter_flow_stat = FLOW_CREATE_FAIL;

                #ifdef UTAF_DEBUG_SESSION
                printf("[ FLOW_CREATE_FAIL] Raw Packet:[UP_DIRECTION] si->pkts=%lu, si->bytes=%lu\n", si->pkts[0],si->bytes[0] );
                #endif
            }
        }
        else //PACKET_NETFLOW
        {
            si->inter_flow_stat = FLOW_CREATE_OK;
        }
#endif
#if 0
        if(PROTO_TCP == mbuf->protocol && IS_OTHER_IP_DEFRAG != mbuf->ipdefrag_flag)
        {
            si->tcpSi[0].seq_window[si->tcpSi[0].index_w] = mbuf->tcp_seq;
            #ifdef UTAF_DEBUG_SESSION
                printf("[SESSION] w_index is %d, seq is %d\n",si->tcpSi[0].index_w, si->tcpSi[0].seq_window[si->tcpSi[0].index_w]);
            #endif
            si->tcpSi[0].index_w++;
        }
#endif
    }

#if 0 /* wdb_as */
    si->inc_next = NULL;
#endif /* wdb_as */

    //si->last_log_pkts[0] = 0;
    //si->last_log_pkts[1] = 0;
    //si->last_log_bytes[0] = 0;
    //si->last_log_bytes[1] = 0;

    //session_update_isp_info(si);

#if 1 /* wdb_as & __wdb__ & wdb_sglpkt_age */
    si->hash = (int)hash;
    //si->age_ring = 0;
#endif /* wdb_as & __wdb__ & wdb_sglpkt_age */

#if 1 /* wdb_Jan14 */
#if 1 /* wdb_as */
    {
        register int current_age_index;

#if 1 /* wdb_sglpkt_age */
        //read_lock(&utaf_age_index_lock); wdb_without_thrd_crting
        current_age_index = utaf_age_index_current;  //wdb_lfix
        __builtin_ia32_lfence();  //wdb_lfix
//current_age_index = atomic_load(&utaf_age_index_current);  //wdb_lfix
        //read_unlock(&utaf_age_index_lock); wdb_without_thrd_crting

#if 1 /* wdb_without_thrd_crting */
        spinlock_lock(&si->tmrng_lck);
#endif
        si->ring_idx = current_age_index;
        si->thrd_idx = UTAF_PER_LCORE(lthrd_id);

        spinlock_lock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
        hlist_add_head(&si->tm_lst, &g_utaf_timeout_ring[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
#ifdef UTAF_AS2_DEBUG
        (g_utaf_timeout_rcnt[current_age_index][UTAF_PER_LCORE(lthrd_id)])++;
#endif
        spinlock_unlock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);

#if 1 /* wdb_without_thrd_crting */
        spinlock_unlock(&si->tmrng_lck);
#endif
#else /* wdb_sglpkt_age */
        current_age_index = utaf_age0_index_current;
        __builtin_ia32_lfence();

        spinlock_lock(&si->tmrng_lck);

        si->ring_idx = current_age_index;
        si->thrd_idx = UTAF_PER_LCORE(lthrd_id);

        spinlock_lock(&g_utaf_timeout_r0lck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
        hlist_add_head(&si->tm_lst, &g_utaf_timeout_ring0[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
        spinlock_unlock(&g_utaf_timeout_r0lck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
#ifdef UTAF_AS2_DEBUG
        (g_utaf_timeout_r0cnt[current_age_index][UTAF_PER_LCORE(lthrd_id)])++;
#endif
        spinlock_unlock(&si->tmrng_lck);
#endif /* wdb_sglpkt_age */
    }
#endif /* wdb_as */
#endif /* wdb_Jan14 */

#ifdef UTAF_LOCK_SPINLOCK
    spinlock_lock(&base[hash].lock);
#else
    write_lock(&base[hash].lock);
#endif

    hlist_add_head(&si->list, &base[hash].hash);
    base[hash].link_len++;

#ifdef UTAF_LOCK_SPINLOCK
    spinlock_unlock(&base[hash].lock);
#else
    write_unlock(&base[hash].lock);
#endif

    mbuf->psession_item = (void *)si;

    DEBUG_SESSION_STAT_SESSION_ADD;

    return UTAF_OK;
}

uint32_t session_update(session_item_t *si, struct m_buf *mbuf)
{
    /*TODO update info*/
    mbuf->psession_item = (void *)si;
    si->cycle = g_utaf_time_jiffies; //utaf_get_timer_cycles(); //wdb_ppp

    return UTAF_OK;
}


uint32_t session_input(struct m_buf *mbuf)
{
    unsigned int hash;
    session_item_t *si;
    uint32_t  result = 0;
/*    rwlock_t *rwlock;  //wdb_calc222 */
    session_bucket_t *base = NULL;

    base = (session_bucket_t *)session_table.bucket_base_ptr;

#ifdef UTAF_DEBUG_CYCLE
    //printf("#####session_input in %lu\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_input, in);
#endif

#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]============>session_input\n");
#endif
    hash = sessionhashfn(mbuf->ip_tuple.sip[0], mbuf->ip_tuple.dip[0], mbuf->sport, mbuf->dport, mbuf->protocol);

#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]hash code is %u\n", hash);
#endif

    mbuf->session_hash = hash;

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_find, in);
#endif

    si = session_find(base, mbuf, hash);

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_find, out);
#endif

    if (si)
    {
#if 1 /* wdb_as */
        register int current_age_index;  //wdb_lfix
        int moved_to_current;  //wdb_lfix

        moved_to_current = 0;

        //read_lock(&utaf_age_index_lock); wdb_without_thrd_crting
        current_age_index = utaf_age_index_current;  //wdb_lfix
        __builtin_ia32_lfence();  //wdb_lfix
        //current_age_index = atomic_load(&utaf_age_index_current);  //wdb_lfix
        //read_unlock(&utaf_age_index_lock); wdb_without_thrd_crting
#endif /* wdb_as */

#ifdef UTAF_DEBUG_SESSION
        printf("[SESSION]session item is found\n");
#endif

#if 1 /* wdb_as */
#if 1 /* wdb_without_thrd_crting */
        spinlock_lock(&si->tmrng_lck);
#endif

#if 0 /* wdb_sglpkt_age */
        if (si->age_ring == 0)
        {
            register int current_age0_index;

            current_age0_index = utaf_age0_index_current + 1;
            __builtin_ia32_lfence();
            current_age0_index = current_age0_index%UTAF_AGE_RING0_SIZE;

            if ( si->ring_idx != current_age0_index )
            {
                spinlock_lock(&g_utaf_timeout_r0lck[si->ring_idx][si->thrd_idx]);
                if ( si->hash != -1 )
                {
                    __hlist_del(&si->tm_lst);
                    si->age_ring = 1;
#ifdef UTAF_AS2_DEBUG
                    (g_utaf_timeout_r0cnt[si->ring_idx][si->thrd_idx])--;
#endif
                    moved_to_current = 1;
                }
                spinlock_unlock(&g_utaf_timeout_r0lck[si->ring_idx][si->thrd_idx]);
            }
        }
        else
        {
#endif /* wdb_sglpkt_age */
            spinlock_lock(&g_utaf_timeout_rlck[si->ring_idx][si->thrd_idx]);
            if ( si->ring_idx !=  current_age_index &&
                si->ring_idx != (current_age_index + 1)%UTAF_AGE_RING_SIZE && si->hash != -1 )
            {
                __hlist_del(&si->tm_lst);
                moved_to_current = 1;
#ifdef UTAF_AS2_DEBUG
                (g_utaf_timeout_rcnt[si->ring_idx][si->thrd_idx])--;
#endif
            }
            spinlock_unlock(&g_utaf_timeout_rlck[si->ring_idx][si->thrd_idx]);
#if 0 /* wdb_sglpkt_age */
        }
#endif /* wdb_sglpkt_age */

        if (moved_to_current)
        {
            spinlock_lock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
            si->ring_idx = current_age_index;
            si->thrd_idx = UTAF_PER_LCORE(lthrd_id);
            hlist_add_head(&si->tm_lst, &g_utaf_timeout_ring[si->ring_idx][si->thrd_idx]);
#ifdef UTAF_AS2_DEBUG
            (g_utaf_timeout_rcnt[si->ring_idx][si->thrd_idx])++;
#endif
            spinlock_unlock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
        }
#if 1 /* wdb_without_thrd_crting */
        spinlock_unlock(&si->tmrng_lck);
#endif
#endif /* wdb_as */

#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(session_update, in);
#endif
        result = session_update(si, mbuf);

#if 0 /* wdb_as */
        extern uint32_t incr_interval;
        if ( (si->cycle - si->last_log_cycle) > incr_interval )
        {
         /* if (0 == utaf_no_task_for_ssn(si, g_stTaskListCksum))
            {
                utaf_cdr_session_inc_output(si);
            } //wdb_ppp */
            output_inc_record(si); //wdb_ppp
            si->last_log_cycle = si->cycle;
        }
#endif /* wdb_as */

#ifdef UTAF_DDOS
		if (g_ddos_conf.sw && UTAF_DO_DDOS == si->ddos_flag && 
				((si->cycle - si->ddos.last_log_time) > g_ddos_conf.interval)) {
			/*  send statistic log to ddos_statistic_mod */
			ddos_statistic(si);
			ddos_flow_statistic_d(si);
			memset(&si->ddos.stat, 0x0, sizeof(si->ddos.stat));
			si->ddos.last_log_time = si->cycle;
		}
#endif 

#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(session_update, out);
#endif
    }

    if(NULL == si)
    {
#ifdef UTAF_DEBUG_SESSION
        printf("[SESSION]session item is NULL\n");
#endif

#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(session_add, in);
#endif

        result = session_add(base, hash, mbuf);

#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(session_add, out);
#endif
    }

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_input, out);
#endif


    return result;
}

uint32_t session_input_group(struct m_buf **group, uint32_t num)
{
    return session_input(group[0]);
}


FILE *eth_fd = NULL;
char * _cap_ip_addr_format(char *buf, int ip_addr)
{
    sprintf(buf, "%d.%d.%d.%d",
                (ip_addr >> 24) & 0xFF, (ip_addr >> 16) & 0xFF,
                (ip_addr >> 8) & 0xFF, ip_addr & 0xFF);
    return buf;
}

//uint32_t max_session_count = 0;

uint32_t session_item_init(void)  //wdb_calc222
{
    int i,j;
    char name[64];
    session_item_t *si;
#if 1 /* wdb */
#if 1 /* wdb_pfmns */
    //unsigned long nodemask = 0x1;
#else /* wdb_pfmns */
    unsigned long nodemask = 0x2;
#endif /* wdb_pfmns */
    //void *hp;
    size_t m_size;
#endif /* wdb */

#ifdef __WDB_SCO__ /* wdb_sco */
    struct hlist_head bunch_hdr;
    int bunch_cnt;
#endif /* wdb_sco */

    int x = session_item_num/g_thread_num;
    printf("session item Q num: %d\n", g_thread_num);
    
    int y =    get_power(x);
    printf("session item Q size: %d\n", y);
    printf("session item num per Q: %d\n", x);

    printf("session item size: %lu\n", sizeof(session_item_t));
    printf("session item cdpi_flow size: %lu\n", sizeof(struct cdpi_flow));

    for(i = 0; i < g_thread_num; i++)
    {
        memset(name, 0 ,sizeof(name));
        sprintf(name, "session q %d", i);
        printf("%s init\n", name);

#ifndef __WDB_SCO__ /* wdb_sco */

        struct ringq *rq = ringq_create(name, y, 0);
        session_item_q_group.session_item_ringq[i] = rq;

#else /* defined(__WDB_SCO__) */ /* wdb_sco */

        session_item_q_group.session_item_ringq[i].first = NULL;
        spinlock_init(&(session_item_q_group.session_item_rq_lock[i]));

#endif /* defined(__WDB_SCO__) */ /* wdb_sco */

        m_size = (x * (SESSION_ITEM_HEADER_SIZE + SESSION_ITEM_SIZE));
#if 1 /* wzh_camalloc */  //wdb_ppp
        session_item_header *sih = (session_item_header *)malloc(m_size);
#else
        session_item_header *sih = NULL;
        CA_MALLOC(sih, session_item_header, m_size, "session");
#endif
        if(NULL == sih)
        {
            return UTAF_FAIL;
        }

#if 0 /* wdb */
        hp = (void *)(((unsigned long)sih/g_page_sz)*g_page_sz);
        if ( mbind(hp, ((m_size + 2*g_page_sz)/g_page_sz)*g_page_sz, MPOL_BIND,
                   &nodemask, 3, MPOL_MF_MOVE | MPOL_MF_STRICT) != 0 )
        {
            fprintf(stderr, "mbind() failed: %s\n", strerror(errno));
        }
#endif /* wdb */

        memset(sih, 0, m_size);

#ifdef __WDB_SCO__ /* wdb_sco */
        bunch_cnt = 0;
        bunch_hdr.first = NULL;
#endif /* __WDB_SCO__ */ /* wdb_sco */

        for (j = 0; j < x; j++)
        {
            sih->session_item_magic = SESSION_MAGIC;
            sih->ringq_id = i;

            si = (session_item_t *)((uint8_t *)sih + SESSION_ITEM_HEADER_SIZE);

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
            spinlock_init(&(si->item_lock));
//#else //wdb_lfix-6
//            rwlock_init(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

#if 1 /* wdb_without_thrd_crting */
            spinlock_init(&(si->tmrng_lck));
#endif

#ifndef __WDB_SCO__ /* wdb_sco */
            if(RINGQ_OK != ringq_enqueue(rq, (uint64_t)si))
            {
                printf("session item init enqueue fail\n");
            }
#else /* defined(__WDB_SCO__) */ /* wdb_sco */

            si->list.next = bunch_hdr.first;
            bunch_hdr.first = &si->list;
            bunch_cnt++;
            if (bunch_cnt >= SI_BUNCH_SIZE)
            {
                si->tm_lst.next = session_item_q_group.session_item_ringq[i].first;
                session_item_q_group.session_item_ringq[i].first = &si->tm_lst;
                bunch_cnt = 0;
                bunch_hdr.first = NULL;
            }

#endif /* defined(__WDB_SCO__) */ /* wdb_sco */

            sih = (session_item_header *)((char *)sih + (SESSION_ITEM_HEADER_SIZE + SESSION_ITEM_SIZE));
        }

#ifdef __WDB_SCO__ /* wdb_sco */
        if (bunch_hdr.first)
        {
            si = (session_item_t *)bunch_hdr.first;
            si->tm_lst.next = session_item_q_group.session_item_ringq[i].first;
            session_item_q_group.session_item_ringq[i].first = &si->tm_lst;
        }
#endif /* __WDB_SCO__ */ /* wdb_sco */
    }

#ifdef __WDB_SCO__ /* wdb_sco */
    for (i = 0; i < 16; i++)
    {
        utaf_l_si_bunch[i].first = NULL;
    }
#endif /* __WDB_SCO__ */ /* wdb_sco */

    return UTAF_OK;
}

uint32_t session_table_init()
{
    int i = 0;

    session_bucket_t *base = NULL;
#if 1 /* wdb */
#if 1 /* wdb_pfmns */
    //unsigned long nodemask = 0x1;
#else /* wdb_pfmns */
    unsigned long nodemask = 0x2;
#endif /* wdb_pfmns */
    //void *hp;
    size_t m_size;
#endif /* wdb */

    char tcad_conf[256] = {0};
    char key_item[64] = {0};
    char key_bucket[64] = {0};

    sprintf(tcad_conf, "%s/tcad.conf", PROGRAM_DIR);

    // 从配置文件中读取
    tcad_conf_get_profile_string(tcad_conf, "session", "session_item_num", key_item);
    tcad_conf_get_profile_string(tcad_conf, "session", "session_bucket_num", key_bucket);

    if ('\0' != key_item[0]) {
        session_item_num = (uint64_t)atoi(key_item);
    }

    if ('\0' != key_bucket[0]) {
        session_bucket_num = (uint64_t)atoi(key_bucket);
        if (!POWEROF2(session_bucket_num))
        {
            printf("session_bucket_num is not a power of 2.\n");
            return UTAF_FAIL;
        }
        session_bucket_mask = session_bucket_num-1;
    }

    if (0 == session_item_num) {
        session_item_num = UTAF_SESSION_ITEM_NUM;
    }

    if (0 == session_bucket_num) {
        session_bucket_num = UTAF_SESSION_BUCKET_NUM;
        session_bucket_mask = UTAF_SESSION_BUCKET_MASK;
    }

    printf("session item total num: %lu\n", session_item_num);
    printf("session bucket total num: %lu\n", session_bucket_num);
    printf("session bucket mask: %lu\n", session_bucket_mask);

    session_table.bucket_num = session_bucket_num;
    session_table.bucket_size = sizeof(session_bucket_t);

    session_table.item_num = session_item_num;
    session_table.item_size = sizeof(session_item_t);

    m_size = session_table.bucket_num * session_table.bucket_size;

#if 1 /* wzh_camalloc */ //wdb_ppp: shit!!!
    session_table.bucket_base_ptr = (void *)malloc(m_size);
#else
    CA_MALLOC(session_table.bucket_base_ptr, void, m_size, "session");
#endif
    if(session_table.bucket_base_ptr == NULL)
    {
        printf("no memory\n");
        return UTAF_FAIL;
    }
#if 0 /* wdb */
    hp = (void *)(((unsigned long)(session_table.bucket_base_ptr)/g_page_sz)*g_page_sz);

    if ( mbind(hp, ((m_size + 2*g_page_sz)/g_page_sz)*g_page_sz, MPOL_BIND, &nodemask, 3, MPOL_MF_MOVE | MPOL_MF_STRICT) != 0 )
    {
        fprintf(stderr, "mbind() failed: %s\n", strerror(errno));
    }
#endif /* wdb */

    printf("session_item_init\n");
    if(UTAF_OK != session_item_init())
    {
        return UTAF_FAIL;
    }

    base = (session_bucket_t *)session_table.bucket_base_ptr;

    for(i = 0; i < session_bucket_num; i++)
    {
        INIT_HLIST_HEAD(&base[i].hash);
#ifdef UTAF_LOCK_SPINLOCK
        spinlock_init(&base[i].lock);
#else
        rwlock_init(&base[i].lock);
#endif
        base[i].link_len = 0;
    }

    //eth_fd = fopen("/tmp/cap_eth_sip_dip", "wb");

    return UTAF_OK;
}

uint32_t nf_session_add(session_bucket_t *base, unsigned int hash, struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]session add\n");
#endif

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_item_alloc, in);
#endif
    session_item_t *si = session_item_alloc();

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(session_item_alloc, out);
#endif

    if (NULL == si)
    {
        DEBUG_SESSION_STAT_SESSION_FULL;
        return UTAF_MEM_FAIL;
    }

#ifdef UTAF_DEBUG_SESSION
    printf("[SESSION]add item addr is %p\n", si);
#endif

    {
        int i;
        for (i = 0; i < 10; i++)
        {
            rte_prefetch0((void *)((char *)si + 64*i));
        }
    }

    {
        size_t off_num = (unsigned long)(&si->app_id) - (unsigned long)si;
        memset((void *)((char *)si + off_num), 0, sizeof(session_item_t) - off_num);
    }

        //memset((void *)si->tasks, 0, 16);
        //si->tsk_cksum = 0;
        //si->tsk_num   = 0;
        //si->tsk_none  = 0;

        /*TODO:fill info*/
        si->start_time = g_utaf_time_jiffies;
        si->cycle = g_utaf_time_jiffies; //utaf_get_timer_cycles(); //wdb_ppp
        si->last_log_cycle = si->cycle;

        //si->session_id = ((si->cycle & 0xffff) << 16 ) | hash;
        si->service_type = APP_UNABLE_TO_IDENTIFY;
        si->service_group = APP_UNABLE_GROUP_ID;

        if(mbuf->ip_version == IPV4_VERSION)
        {
            if (TH_SYNACK == (mbuf->tcp_f & TH_SYNACK)) {
                si->tcp_flag |= 0x2;                // bit1 syn&ack
                si->direct_flags = DOWN_DIRECTION;
            } else if (TH_SYN == (mbuf->tcp_f & TH_SYN)) {
                        si->tcp_flag |= 0x1;                // bit0 syn
                        si->direct_flags = UP_DIRECTION;
                } else if (TH_ACK == (mbuf->tcp_f & TH_ACK)) {
                        si->tcp_flag |= 0x4;                // bit1 ack
                        si->direct_flags = UP_DIRECTION;
                } else {
                        si->direct_flags = UP_DIRECTION;
                }

#ifdef UTAF_DEBUG_SESSION
                printf("si->tcp_flag =0x%x\n", si->tcp_flag & 0x7);
                printf("si->direct_flags =0x%x\n", si->direct_flags);
#endif

#if IPV4_VERSION
                si->ip_tuple.sip[0] = mbuf->ip_tuple.sip[0];
                si->ip_tuple.dip[0] = mbuf->ip_tuple.dip[0];
#endif
#if 0
                if (UP_DIRECTION == si->direct_flags) {
                    si->ip1_eth_id = mbuf->eth_id;
                } else {
                    si->ip2_eth_id = mbuf->eth_id;
                }
#endif
                si->sport = mbuf->sport;
                si->dport = mbuf->dport;
                si->protocol = (uint8_t)mbuf->protocol;
                si->ip_version = (uint8_t)mbuf->ip_version;

                if (PACKET_RAW == mbuf->pkt_type)
                {
                        if(FLOW_CREATE_OK == mbuf->flow_c_stat)
                        {
                                si->inter_flow_stat = FLOW_CREATE_OK;
                        }
                        else //FLOW_CREATE_FAIL
                        {
                                DEBUG_SESSION_STAT_RAW_PKT_SESSION_FAIL;
                                si->inter_flow_stat = FLOW_CREATE_FAIL;

                                si->pkts[0]++;
                                si->bytes[0] += mbuf->total_len;
#ifdef UTAF_DEBUG_SESSION
                                printf("[ FLOW_CREATE_FAIL] Raw Packet:[UP_DIRECTION] si->pkts=%lu, si->bytes=%lu\n", si->pkts[0],si->bytes[0] );
#endif
                        }
                }
                else //PACKET_NETFLOW
                {
                        si->inter_flow_stat = FLOW_CREATE_OK;
                }

                #if 0
                if(PROTO_TCP == mbuf->protocol && IS_OTHER_IP_DEFRAG != mbuf->ipdefrag_flag)
                {
                        si->tcpSi[0].seq_window[si->tcpSi[0].index_w] = mbuf->tcp_seq;
                        #ifdef UTAF_DEBUG_SESSION
                        printf("[SESSION] w_index is %d, seq is %d\n",si->tcpSi[0].index_w, si->tcpSi[0].seq_window[si->tcpSi[0].index_w]);
                        #endif
                        si->tcpSi[0].index_w++;
                }
                #endif
        }

        //si->last_log_pkts[0] = 0;
        //si->last_log_pkts[1] = 0;
        //si->last_log_bytes[0] = 0;
        //si->last_log_bytes[1] = 0;

        si->s_isp_id = 0xFFFF;
        si->d_isp_id = 0xFFFF;
#if 1 /* wdb_as & __wdb__ */
        si->hash = (int)hash;
#endif /* wdb_as & __wdb__ */

#if 0 /* wdb_sglpkt_age */
        si->age_ring = 1;
#endif /* wdb_sglpkt_age */

#if 1 /* wdb_Jan14 */
        {
                register int current_age_index;

                current_age_index = utaf_age_index_current;
                __builtin_ia32_lfence();

#if 1 /* wdb_without_thrd_crting */
                spinlock_lock(&si->tmrng_lck);
#endif
                si->ring_idx = current_age_index;
                si->thrd_idx = UTAF_PER_LCORE(lthrd_id);

                spinlock_lock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
                hlist_add_head(&si->tm_lst, &g_utaf_timeout_ring[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
#ifdef UTAF_AS2_DEBUG
                (g_utaf_timeout_rcnt[current_age_index][UTAF_PER_LCORE(lthrd_id)])++;
#endif
                spinlock_unlock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);

#if 1 /* wdb_without_thrd_crting */
                spinlock_unlock(&si->tmrng_lck);
#endif
        }
#endif /* wdb_Jan14 */

#ifdef UTAF_LOCK_SPINLOCK
        spinlock_lock(&base[hash].lock);
#else
        write_lock(&base[hash].lock);
#endif

        hlist_add_head(&si->list, &base[hash].hash);
        base[hash].link_len++;


#ifdef UTAF_LOCK_SPINLOCK
        spinlock_unlock(&base[hash].lock);
#else
        write_unlock(&base[hash].lock);
#endif
        mbuf->psession_item = (void *)si;

        DEBUG_SESSION_STAT_SESSION_ADD;

        return UTAF_OK;
}

uint32_t nf_age_session_add(session_bucket_t *base, unsigned int hash, struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_SESSION
        printf("[SESSION]session add\n");
#endif

#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(session_item_alloc, in);
#endif
        session_item_t *si = session_item_alloc();

#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(session_item_alloc, out);
#endif

        if(NULL == si)
        {
                DEBUG_SESSION_STAT_SESSION_FULL;
                return UTAF_MEM_FAIL;
        }

#ifdef UTAF_DEBUG_SESSION
        printf("[SESSION]add item addr is %p\n", si);
#endif

        {
                int i;
                for (i = 0; i < 10; i++)
                {
                        rte_prefetch0((void *)((char *)si + 64*i));
                }
        }

    {
        size_t off_num = (unsigned long)(&si->app_id) - (unsigned long)si;

        memset((void *)((char *)si + off_num), 0, sizeof(session_item_t) - off_num);
    }

    si->start_time = g_utaf_time_jiffies;

    si->cycle = g_utaf_time_jiffies; //utaf_get_timer_cycles(); //wdb_ppp
    si->last_log_cycle = si->cycle;
    //si->session_id = ((si->cycle & 0xffff) << 16 ) | hash;

    if(mbuf->ip_version == IPV4_VERSION)
    {
        if (TH_SYNACK == (mbuf->tcp_f & TH_SYNACK)) {
            si->tcp_flag |= 0x2;                // bit1 syn&ack
            si->direct_flags = DOWN_DIRECTION;
        } else if (TH_SYN == (mbuf->tcp_f & TH_SYN)) {
            si->tcp_flag |= 0x1;                // bit0 syn
            si->direct_flags = UP_DIRECTION;
        } else if (TH_ACK == (mbuf->tcp_f & TH_ACK)) {
            si->tcp_flag |= 0x4;                // bit1 ack
            si->direct_flags = UP_DIRECTION;
        } else {
            si->direct_flags = UP_DIRECTION;
        }

#ifdef UTAF_DEBUG_SESSION
        printf("si->tcp_flag =0x%x\n", si->tcp_flag & 0x7);
        printf("si->direct_flags =0x%x\n", si->direct_flags);
#endif
#if IPV4_VERSION
        si->ip_tuple.sip[0] = mbuf->ip_tuple.sip[0];
        si->ip_tuple.dip[0] = mbuf->ip_tuple.dip[0];
#endif
#if 0
        if (UP_DIRECTION == si->direct_flags) {
            si->ip1_eth_id = mbuf->eth_id;
        } else {
            si->ip2_eth_id = mbuf->eth_id;
        }
#endif
        si->sport = mbuf->sport;
        si->dport = mbuf->dport;
        si->protocol = (uint8_t)mbuf->protocol;
        si->ip_version = (uint8_t)mbuf->ip_version;

        if (PACKET_RAW == mbuf->pkt_type)
        {
            if(FLOW_CREATE_OK == mbuf->flow_c_stat)
            {
                si->inter_flow_stat = FLOW_CREATE_OK;
            }
            else //FLOW_CREATE_FAIL
            {
                DEBUG_SESSION_STAT_RAW_PKT_SESSION_FAIL;
                si->inter_flow_stat = FLOW_CREATE_FAIL;

                si->pkts[0]++;
                si->bytes[0] += mbuf->total_len;
#ifdef UTAF_DEBUG_SESSION
                printf("[ FLOW_CREATE_FAIL] Raw Packet:[UP_DIRECTION] si->pkts=%lu, si->bytes=%lu\n", si->pkts[0],si->bytes[0] );
#endif
            }
        }
        else //PACKET_NETFLOW
        {
            si->inter_flow_stat = FLOW_CREATE_OK;
        }

#if 0
        if(PROTO_TCP == mbuf->protocol && IS_OTHER_IP_DEFRAG != mbuf->ipdefrag_flag)
        {
            si->tcpSi[0].seq_window[si->tcpSi[0].index_w] = mbuf->tcp_seq;
#ifdef UTAF_DEBUG_SESSION
            printf("[SESSION] w_index is %d, seq is %d\n",si->tcpSi[0].index_w, si->tcpSi[0].seq_window[si->tcpSi[0].index_w]);
#endif
            si->tcpSi[0].index_w++;
        }
#endif
    }

    si->s_isp_id = 0xFFFF;
    si->d_isp_id = 0xFFFF;

    si->hash = -1;

    mbuf->psession_item = (void *)si;

    DEBUG_SESSION_STAT_SESSION_ADD;

    return UTAF_OK;
}

typedef struct tmout_lst_hdr_s
{
    struct hlist_head timeout_lst_head;
    struct tmout_lst_hdr_s *next;
} tmout_lst_hdr_t;

struct hlist_head utaf_nf_aged_list[16];
spinlock_t utaf_nf_aged_lock[16];
#ifdef UTAF_AS2_DEBUG
unsigned int utaf_nf_aged_lcnt[16];
#endif

static struct ringq *utaf_ssn_age_queue[SESSION_MAX_AGE_CORE];
static spinlock_t    utaf_ssn_age_qlock[SESSION_MAX_AGE_CORE];

static tmout_lst_hdr_t *utaf_tmout_lst_hdr_alloc(void);
static void utaf_tmout_lst_hdr_free(tmout_lst_hdr_t *tl);

#ifdef __WDB_SCO_2plus__
static struct hlist_head lf_hdr[SESSION_MAX_AGE_CORE][SESSION_ITEM_MEMBER_NUM];
static struct hlist_node **lf_tl[SESSION_MAX_AGE_CORE][SESSION_ITEM_MEMBER_NUM];
static int si_cnt[SESSION_MAX_AGE_CORE][SESSION_ITEM_MEMBER_NUM];
#endif /* __WDB_SCO_2plus__ */

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
static void empty_thrd_crt(int f_index);
#if 1 /* wdb_lfix-4 */
static void f_index_put_thrd_crt(void);
#endif /* wdb_lfix-4 */
#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */

static void ssn_age_timeout_cb(tmout_lst_hdr_t *tlh)
{
    struct hlist_head *timeout_lst = &tlh->timeout_lst_head;

    session_item_t *si;
    struct hlist_node *n;
    struct hlist_node *t;
#ifdef __WDB_SCO__ /* wdb_sco */
#ifndef __WDB_SCO_2plus__
    struct hlist_head lf_hdr[SESSION_ITEM_MEMBER_NUM];
    struct hlist_node **lf_tl[SESSION_ITEM_MEMBER_NUM];
    int si_cnt[SESSION_ITEM_MEMBER_NUM];
#endif /* !__WDB_SCO_2plus__ */
    session_item_header *sih;
#ifdef __WDB_SCO_2plus__
    int my_id = (int)(UTAF_PER_LCORE(lthrd_id) - 16);
#endif /* __WDB_SCO_2plus__ */
    //int i; //wdb_without_thrd_crting => move it below
#endif /* wdb_sco */
    int i; //wdb_without_thrd_crting => move it here

#ifdef UTAF_AS2_DEBUG
    int ssn_age_cnt = 0;
    time_t tst_start, tst_stop;
    tst_start = g_utaf_time_jiffies;
/* #else
    time_t tst_start = g_utaf_time_jiffies; //wdb_ppp */
#endif

#ifdef __WDB_SCO__ /* wdb_sco */
#ifndef __WDB_SCO_2plus__
    for (i = 0; i < SESSION_ITEM_MEMBER_NUM; i++)
    {
        lf_hdr[i].first = NULL;
        lf_tl[i] = &lf_hdr[i].first;
        si_cnt[i] = 0;
    }
#endif /* !__WDB_SCO_2plus__ */
#endif /* wdb_sco */

    hlist_for_each_entry_safe(si, t, n, timeout_lst, tm_lst)
    {
        __hlist_del(&si->tm_lst);

#ifdef UTAF_AS2_DEBUG
        ssn_age_cnt++;
#endif
        for (i = 0; i < 10; i++)
        {
            rte_prefetch0((void *)((char *)si + 64*i));
        }

        DEBUG_SESSION_STAT_SESSION_DEL;
#if 0
        if (0 == si->stat_netflow_num) {
            DEBUG_SESSION_STAT_SESSION_NO_STAT_NETFLOW;
        }
        if (0 == si->age_netflow_num) {
            DEBUG_SESSION_STAT_SESSION_NO_AGE_NETFLOW;
        }
        if (0 == si->stat_netflow_num && 0 == si->age_netflow_num) {
            DEBUG_SESSION_STAT_SESSION_NO_NETFLOW;
        }
#endif
        if (PROTO_TCP == si->protocol) {
            DEBUG_SESSION_STAT_SESSION_TCP;
        } else if (PROTO_UDP == si->protocol) {
            DEBUG_SESSION_STAT_SESSION_UDP;
        } else if (PROTO_ICMP == si->protocol) {
            DEBUG_SESSION_STAT_SESSION_ICMP;
        }

        if (APP_HTTP == si->service_type) {
            DEBUG_SESSION_HTTP_ADD;
        } else if (APP_DNS == si->service_type) {
            DEBUG_SESSION_DNS_ADD;
        }

        if (1 == si->pkts[0]) {
            DEBUG_SESSION_STAT_SESSION_SINGLE_PKT;
        }

     /* if ( 0 == utaf_no_task_for_ssn(si, g_stTaskListCksum) )
        {
            utaf_cdr_session_age_output(si, tst_start);
        } //wdb_ppp */

        session_update_isp_info(si);
        output_age_record(si);
#ifdef UTAF_DDOS
		if (g_ddos_conf.sw && UTAF_DO_DDOS == si->ddos_flag) {

			/* TCP SESSION CLOSE */ 
			if (PROTO_TCP == si->protocol && 0 == si->ddos_ssn_close)
			{
				ddos_statistic_tcp_connect(si, DDOS_CONNECT_CLOSE);
				ddos_debug(DDOS_MID_FLOW,"close session sip:%"IPV4FMT" dip:%"IPV4FMT" sport:%d dport:%d"
						" protocol:%d\n", IPV4P(&si->ip_tuple.sip[0]),
						IPV4P(&si->ip_tuple.dip[0]), si->sport, si->dport,
						si->protocol);
			}
			/*	send statistic log to ddos_statistic_mod */
			ddos_statistic(si);
			ddos_flow_statistic_d(si);
		}
#endif

#ifdef UTAF_DEBUG_SESSION
        printf("[SESSION]session addr %p is age\n", si);
#endif

#ifndef __WDB_SCO__ /* wdb_sco */
        session_item_free(si);
#else /* defined(__WDB_SCO__) */ /* wdb_sco */

        sih = (session_item_header *)((uint8_t *)si - SESSION_ITEM_HEADER_SIZE);

#ifndef __WDB_SCO_2plus__
        *(lf_tl[sih->ringq_id]) = &si->list;
        lf_tl[sih->ringq_id] = &si->list.next;
        (si_cnt[sih->ringq_id])++;
        if (si_cnt[sih->ringq_id] >= SI_BUNCH_SIZE)
#else /* defined(__WDB_SCO_2plus__) */
        *(lf_tl[my_id][sih->ringq_id]) = &si->list;
        lf_tl[my_id][sih->ringq_id] = &si->list.next;
        (si_cnt[my_id][sih->ringq_id])++;
        if (si_cnt[my_id][sih->ringq_id] >= SI_BUNCH_SIZE)
#endif /* __WDB_SCO_2plus__ */
        {
#ifndef __WDB_SCO_2plus__
            *(lf_tl[sih->ringq_id]) = NULL;
            session_item_free((session_item_t *)lf_hdr[sih->ringq_id].first);

            lf_hdr[sih->ringq_id].first = NULL;
            lf_tl[sih->ringq_id] = &lf_hdr[sih->ringq_id].first;
            si_cnt[sih->ringq_id] = 0;
#else /* defined(__WDB_SCO_2plus__) */
            *(lf_tl[my_id][sih->ringq_id]) = NULL;
            session_item_free((session_item_t *)lf_hdr[my_id][sih->ringq_id].first);

            lf_hdr[my_id][sih->ringq_id].first = NULL;
            lf_tl[my_id][sih->ringq_id] = &lf_hdr[my_id][sih->ringq_id].first;
            si_cnt[my_id][sih->ringq_id] = 0;
#endif /* __WDB_SCO_2plus__ */
        }
#endif /* wdb_sco */
    }

#ifdef __WDB_SCO__ /* wdb_sco */
#ifndef __WDB_SCO_2plus__
    for (i = 0; i < SESSION_ITEM_MEMBER_NUM; i++)
    {
        if (lf_hdr[i].first)
        {
            *(lf_tl[i]) = NULL;
            session_item_free((session_item_t *)lf_hdr[i].first);
        }
    }
#endif /* !__WDB_SCO_2plus__ */
#endif /* wdb_sco */

    utaf_tmout_lst_hdr_free(tlh);

#ifdef UTAF_AS2_DEBUG
    tst_stop = g_utaf_time_jiffies;

    printf("\n%s, core %u, time = %lu, aged_cnt = %d.\n\n", __FUNCTION__, UTAF_PER_LCORE(lthrd_id), tst_stop - tst_start, ssn_age_cnt);
#endif
}

static void *utaf_ssn_age_thread(void *data)
{
    int *uaps = (int *)data;
    int q_id  = *uaps;
#if 1 /* wdb_lfix-2 */
    register unsigned int pause_cnt;
#endif /* wdb_lfix-2 */
    struct utaf_session_age_core_s *ac = &(session_age_core[q_id]);
    //cpu_set_t mask;
    uint64_t obj;
    struct timespec sleeptime = {.tv_nsec = 0, .tv_sec = 1 }; /* 1 second */

#if 0
    CPU_ZERO(&mask);
    CPU_SET(ac->cid, &mask);
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "set thread affinity failed for age thread.\n");
    }
#endif

    UTAF_PER_LCORE(_lcore_id) = ac->cid;
    UTAF_PER_LCORE(lthrd_id) = ac->tid;

#ifdef UTAF_AS2_DEBUG
    printf("wdb: I'm at %s line %d, Age thread %u is crted.\n", __FILE__, __LINE__, UTAF_PER_LCORE(lthrd_id));
#endif

    while (1)
    {
        for (q_id = 0; q_id < session_age_thread_num; q_id++)
        {
            spinlock_lock(&utaf_ssn_age_qlock[q_id]);
            if (0 != ringq_dequeue(utaf_ssn_age_queue[q_id], &obj))
            {
                spinlock_unlock(&utaf_ssn_age_qlock[q_id]);
/* #ifdef UTAF_AS2_DEBUG
                (no_obj_cnt[UTAF_PER_LCORE(lthrd_id) - 16])++;
#endif */

#if 1 /* wdb_lfix-2 */
                pause_cnt = (unsigned int)random();
                pause_cnt = pause_cnt%100 + 1;
                while (pause_cnt--) { asm volatile ("pause"); }
#endif /* wdb_lfix-2 */

                nanosleep(&sleeptime, NULL);

                continue;
            }
            spinlock_unlock(&utaf_ssn_age_qlock[q_id]);
            ssn_age_timeout_cb((tmout_lst_hdr_t *)obj);
        }
    }

    return NULL;
}

static tmout_lst_hdr_t *utaf_tmout_lst_heads = NULL;
static spinlock_t       utaf_tmout_lst_hdlck;

static void utaf_ssn_age_tmout_lst_heads_init(void)
{
    tmout_lst_hdr_t *tl;
    //unsigned long nodemask = 0x1;
    //void *hp;
    //size_t m_size;
    int i;

    spinlock_init(&utaf_tmout_lst_hdlck);
    tl = (tmout_lst_hdr_t *)malloc(session_item_num*sizeof(tmout_lst_hdr_t));
    if (!tl)
    {
        fprintf(stderr, "malloc for 'tmout_lst_heads' failed\n");
        exit(0);
    }
#if 0
    hp = (void *)(((unsigned long)tl/g_page_sz)*g_page_sz);
    m_size = session_item_num*sizeof(tmout_lst_hdr_t);
    if ( mbind(hp, ((m_size + 2*g_page_sz)/g_page_sz)*g_page_sz, MPOL_BIND, &nodemask, 3, MPOL_MF_MOVE | MPOL_MF_STRICT) != 0 )
    {
        fprintf(stderr, "mbind() failed: %s\n", strerror(errno));
    }
#endif
    for (i = 0; i < session_item_num; i++)
    {
        tl[i].next = utaf_tmout_lst_heads;
        utaf_tmout_lst_heads = &tl[i];
    }
}

static tmout_lst_hdr_t *utaf_tmout_lst_hdr_alloc(void)
{
    tmout_lst_hdr_t *tl;
    spinlock_lock(&utaf_tmout_lst_hdlck);
    tl = utaf_tmout_lst_heads;
    if (tl)
    {
        utaf_tmout_lst_heads = tl->next;
    }
    spinlock_unlock(&utaf_tmout_lst_hdlck);
    return tl;
}

static void utaf_tmout_lst_hdr_free(tmout_lst_hdr_t *tl)
{
    spinlock_lock(&utaf_tmout_lst_hdlck);
    tl->next = utaf_tmout_lst_heads;
    utaf_tmout_lst_heads = tl;
    spinlock_unlock(&utaf_tmout_lst_hdlck);
}

static int utaf_q_id[SESSION_MAX_AGE_CORE];

int utaf_ssn_age_thread_crt(void)
{
    int i, j;
    uint64_t num = 0;
    pthread_t thr_id;
    char tcad_conf[256] = {0};
    char key[64] = {0};

    sprintf(tcad_conf, "%s/tcad.conf", PROGRAM_DIR);

    // 从配置文件中读取
    tcad_conf_get_profile_string(tcad_conf, "session", "session_age_thread_num", key);

    if ('\0' != key[0]) {
        session_age_thread_num = (uint64_t)atoi(key);
    }

    if (2 != session_age_thread_num && 4 != session_age_thread_num
        && 8 != session_age_thread_num && 16 != session_age_thread_num)
    {
        session_age_thread_num = SESSION_MAX_AGE_CORE;
    }

    printf("session age thread num: %lu\n", session_age_thread_num);

    utaf_ssn_age_tmout_lst_heads_init();

    //rwlock_init(&utaf_age_index_lock); //wdb_without_thrd_crting

    for (i = 0; i < UTAF_AGE_RING_SIZE; i++)
    {
        for (j = 0; j < 16; j++)
        {
            g_utaf_timeout_ring[i][j].first = NULL;
            spinlock_init(&g_utaf_timeout_rlck[i][j]);
#ifdef UTAF_AS2_DEBUG
            g_utaf_timeout_rcnt[i][j] = 0;
#endif
        }
    }

#if 0 /* wdb_sglpkt_age */
    for (i = 0; i < UTAF_AGE_RING0_SIZE; i++)
    {
        for (j = 0; j < 16; j++)
        {
            g_utaf_timeout_ring0[i][j].first = NULL;
            spinlock_init(&g_utaf_timeout_r0lck[i][j]);
#ifdef UTAF_AS2_DEBUG
            g_utaf_timeout_r0cnt[i][j] = 0;
#endif
        }
    }
#endif /* wdb_sglpkt_age */

    extern uint64_t session_item_num;
    num = session_item_num/session_age_thread_num;

    printf("utaf_ssn_age_queue num: %lu\n", num);

    for (i = 0; i < session_age_thread_num; i++)
    {
        utaf_ssn_age_queue[i] = ringq_create("ssn age queue", num, RING_F_SP_ENQ | RING_F_SC_DEQ);
        if (NULL == utaf_ssn_age_queue[i])
        {
            printf("\n[Error]: Create ssn age queue failed.\n");
            return -1;
        }
        spinlock_init(&utaf_ssn_age_qlock[i]);
    }

    for (i = 0; i < 16; i++)
    {
        spinlock_init(&utaf_nf_aged_lock[i]);
        utaf_nf_aged_list[i].first = NULL;
#ifdef UTAF_AS2_DEBUG
        utaf_nf_aged_lcnt[i] = 0;
#endif
    }

#ifdef __WDB_SCO_2plus__
    for (i = 0; i < SESSION_MAX_AGE_CORE; i++)
    {
        for (j = 0; j < SESSION_ITEM_MEMBER_NUM; j++)
        {
            lf_hdr[i][j].first = NULL;
            lf_tl[i][j] = &lf_hdr[i][j].first;
            si_cnt[i][j] = 0;
        }
    }
#endif /* __WDB_SCO_2plus__ */

    for (i = 0; i < session_age_thread_num; i++) {
        utaf_q_id[i] = i;
        pthread_create(&thr_id, NULL, utaf_ssn_age_thread, /* ( void * )(&(session_age_core[i])) */ &utaf_q_id[i]);
    }

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
    empty_thrd_crt(-1);
#if 1 /* wdb_lfix-4 */
    f_index_put_thrd_crt();
#endif /* wdb_lfix-4 */
#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */

    return 0;
}

static void utaf_dist_aged_ssn(int q_id, uint64_t obj)
{
    spinlock_lock(&utaf_ssn_age_qlock[q_id]);
    if (0 != ringq_enqueue(utaf_ssn_age_queue[q_id], obj))
    {
            fprintf(stderr, "__BUG__ [Error]: %s(%lu) failed\n", __FUNCTION__, obj); /* __wdb__ */
         /* break; */
    }
    spinlock_unlock(&utaf_ssn_age_qlock[q_id]);
}

#if 0 /* wdb_sglpkt_age */
static volatile int __f0_index = -1;
#endif /* wdb_sglpkt_age */

static void empty_aged_bucket(int f_index, int thrd_start, int thrd_stop, int *athrd_cur, int athrd_max)
{
    session_bucket_t *base = (session_bucket_t *)session_table.bucket_base_ptr;
    tmout_lst_hdr_t *tlh;
    session_item_t *si;
    struct hlist_node *n, *t, **tmp_tl, **tmp_tl11;  //wdb_fix-Mar5
    int i;
#if 1 /* wdb_lfix-2 */
    register          int last_hash;
    register unsigned int pause_cnt;
#endif /* wdb_lfix-2 */

#ifdef UTAF_AS2_DEBUG
    unsigned int age0_cnt = 0, age_cnt = 0, nf_age_cnt = 0;  //wdb_sglpkt_age
    time_t start_tm, stop_tm;

    start_tm = g_utaf_time_jiffies;
#endif

    for (i = thrd_start; i < thrd_stop; i++)
    {
        tlh = utaf_tmout_lst_hdr_alloc();
        if (!tlh)
        {
                printf("wdb: __BUG__ ! malloc for timeout_list failed.\n");
                return;
        }

        tmp_tl = &tlh->timeout_lst_head.first;

        spinlock_lock(&g_utaf_timeout_rlck[f_index][i]);

        tlh->timeout_lst_head.first = g_utaf_timeout_ring[f_index][i].first;
        if (tlh->timeout_lst_head.first)
        {
            tlh->timeout_lst_head.first->pprev = &tlh->timeout_lst_head.first;
        }
        g_utaf_timeout_ring[f_index][i].first = NULL;

#ifdef UTAF_AS2_DEBUG
     /* printf("wdb: g_utaf_timeout_rcnt[%d][%d] = %d\n", f_index, i, g_utaf_timeout_rcnt[f_index][i]); */
        g_utaf_timeout_rcnt[f_index][i] = 0;
#endif

#if 1 /* wdb_lfix-2 */
        last_hash = -1;
#endif /* wdb_lfix-2 */

        hlist_for_each_entry_safe(si, t, n, &tlh->timeout_lst_head, tm_lst)
        {
#if 1 /* wdb_lfix-2 */
            if (si->hash == last_hash)
            {
                pause_cnt = 30;
                while (pause_cnt--) { asm volatile ("pause"); }
            }
            last_hash = si->hash;
#endif /* wdb_lfix-2 */

#ifdef UTAF_LOCK_SPINLOCK
            spinlock_lock(&base[si->hash].lock);
#else
            write_lock(&base[si->hash].lock);
#endif
            __hlist_del(&si->list);
            (base[si->hash].link_len)--;

#ifdef UTAF_LOCK_SPINLOCK
            spinlock_unlock(&base[si->hash].lock);
#else
            write_unlock(&base[si->hash].lock);
#endif
            si->hash = -1;
            tmp_tl = &si->tm_lst.next;

#ifdef UTAF_AS2_DEBUG
            age_cnt++;
#endif
            DEBUG_SESSION_STAT_SESSION_AGED(1);
        }
        m_wmb();  //wdb_without_thrd_crting ???
        spinlock_unlock(&g_utaf_timeout_rlck[f_index][i]);

#if 0 /* wdb_sglpkt_age */

#if 1 /* wdb_fix-Mar5 */
        tmp_tl11 = tmp_tl;
#endif /* wdb_fix-Mar5 */

        spinlock_lock(&g_utaf_timeout_r0lck[__f0_index][i]);

#ifdef UTAF_AS2_DEBUG
        printf("wdb: g_utaf_timeout_r0cnt[%d][%d] = %d\n", __f0_index, i, g_utaf_timeout_r0cnt[__f0_index][i]);
        g_utaf_timeout_r0cnt[__f0_index][i] = 0;
#endif

#if 1 /* wdb_lfix-2 */
        last_hash = -1;
#endif /* wdb_lfix-2 */

        hlist_for_each_entry_safe(si, t, n, &g_utaf_timeout_ring0[__f0_index][i], tm_lst)
        {
#if 1 /* wdb_lfix-2 */
            if (si->hash == last_hash)
            {
                pause_cnt = 30;
                while (pause_cnt--) { asm volatile ("pause"); }
            }
            last_hash = si->hash;
#endif /* wdb_lfix-2 */

#ifdef UTAF_LOCK_SPINLOCK
            spinlock_lock(&base[si->hash].lock);
#else
            write_lock(&base[si->hash].lock);
#endif
            __hlist_del(&si->list);
            (base[si->hash].link_len)--;

#ifdef UTAF_LOCK_SPINLOCK
            spinlock_unlock(&base[si->hash].lock);
#else
            write_unlock(&base[si->hash].lock);
#endif
            si->hash = -1;
            tmp_tl = &si->tm_lst.next;

#ifdef UTAF_AS2_DEBUG
            age0_cnt++;
#endif
            DEBUG_SESSION_STAT_SESSION_AGED(1);
        }

        *tmp_tl11 = g_utaf_timeout_ring0[__f0_index][i].first;  //wdb_fix-Mar5
        if (g_utaf_timeout_ring0[__f0_index][i].first)
        {
            g_utaf_timeout_ring0[__f0_index][i].first->pprev = tmp_tl11;  //wdb_fix-Mar5
        }
        g_utaf_timeout_ring0[__f0_index][i].first = NULL;

        m_wmb();  //wdb_without_thrd_crting ???
        spinlock_unlock(&g_utaf_timeout_r0lck[__f0_index][i]);

#endif /* wdb_sglpkt_age */

        spinlock_lock(&utaf_nf_aged_lock[i]);

        *tmp_tl = utaf_nf_aged_list[i].first;
        if (utaf_nf_aged_list[i].first)
        {
            utaf_nf_aged_list[i].first->pprev = tmp_tl;
        }
        utaf_nf_aged_list[i].first = NULL;

#ifdef UTAF_AS2_DEBUG
        nf_age_cnt += utaf_nf_aged_lcnt[i];
        utaf_nf_aged_lcnt[i] = 0;
#endif
        spinlock_unlock(&utaf_nf_aged_lock[i]);

        if (tlh)
        {
            if (tlh->timeout_lst_head.first)
            {
             /* utaf_dist_aged_ssn(i, (uint64_t)tlh); */
                utaf_dist_aged_ssn(*athrd_cur, (uint64_t)tlh);
                (*athrd_cur)++;
                if (*athrd_cur == athrd_max)
                {
                    (*athrd_cur) -= (session_age_thread_num/2);
                }
            }
            else
            {
                utaf_tmout_lst_hdr_free(tlh);
            }
        }
    }

#ifdef UTAF_AS2_DEBUG
    stop_tm = g_utaf_time_jiffies;
    printf("wdb: about age_index_current = %d, age_cnt = %u, nf_age_cnt = %u, time = %lu\n", f_index, age_cnt, nf_age_cnt, (stop_tm - start_tm));
#endif

    return;
}

typedef struct __empty_thrd_para_s
{
    int f_index;
    short thrd_start;
    short thrd_stop;
    int   athrd_cur;
    int   athrd_max;
} __empty_thrd_para_t;

static __empty_thrd_para_t __empty_thrd_para[2] =
{
    { 0, 0,  8,                        0, (SESSION_MAX_AGE_CORE/2) },
    { 0, 8, 16, (SESSION_MAX_AGE_CORE/2),  SESSION_MAX_AGE_CORE }
};

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
static spinlock_t utaf_empty_qlock;
#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */

static void *empty_thrd(void *data)
{
#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
    struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
#if 0 /* wdb_lfix-3 */
    cpu_set_t mask;

    CPU_ZERO(&mask);
    //CPU_SET(0, &mask);
    if ( ((__empty_thrd_para_t *)data)->thrd_start == 0 )
    {
     /* CPU_SET(38, &mask); //wdb_core */
        CPU_SET(__calcu_cores[0], &mask);  //wdb_core
    }
    else if ( ((__empty_thrd_para_t *)data)->thrd_start == 8 )
    {
     /* CPU_SET(39, &mask); //wdb_core */
        CPU_SET(__calcu_cores[1], &mask);  //wdb_core
    }
    else
    {
        fprintf(stderr, "%s() - 'thrd_start' out of band.\n", __FUNCTION__);
        exit(1);
    }
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "%s() - set thread affinity failed.\n", __FUNCTION__);
        exit(1);
    }
#endif /* wdb_lfix-3 */

    UTAF_PER_LCORE(_lcore_id) = ((__empty_thrd_para_t *)data)->thrd_start;

    while (1)
    {
        spinlock_lock(&utaf_empty_qlock);
        if ( ((__empty_thrd_para_t *)data)->f_index == -1 )
        {
            spinlock_unlock(&utaf_empty_qlock);
            nanosleep(&sleeptime, NULL);
            continue;
        }
        spinlock_unlock(&utaf_empty_qlock);

#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */

        empty_aged_bucket( ((__empty_thrd_para_t *)data)->f_index,
             (int)(((__empty_thrd_para_t *)data)->thrd_start), (int)(((__empty_thrd_para_t *)data)->thrd_stop),
             &(((__empty_thrd_para_t *)data)->athrd_cur), ((__empty_thrd_para_t *)data)->athrd_max);

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
        spinlock_lock(&utaf_empty_qlock);
        ((__empty_thrd_para_t *)data)->f_index = -1;
        spinlock_unlock(&utaf_empty_qlock);
    }
#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */

    return NULL;
}

static void empty_thrd_crt(int f_index)
{
    pthread_t tid0, tid1;

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
    spinlock_init(&utaf_empty_qlock);
#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */

    __empty_thrd_para[0].f_index = f_index;
    __empty_thrd_para[0].thrd_start = 0;
    __empty_thrd_para[0].thrd_stop = (1 == g_thread_num ? 1 : g_thread_num/2);
    __empty_thrd_para[0].athrd_cur = 0;
    __empty_thrd_para[0].athrd_max = session_age_thread_num/2;

    __empty_thrd_para[1].f_index = f_index;
    __empty_thrd_para[1].thrd_start = g_thread_num/2;
    __empty_thrd_para[1].thrd_stop = g_thread_num;
    __empty_thrd_para[1].athrd_cur = session_age_thread_num/2;
    __empty_thrd_para[1].athrd_max = session_age_thread_num;
    

    printf("__empty_thrd_para[0]: %d, %d -> %d, %d -> %d\n", __empty_thrd_para[0].f_index,
            __empty_thrd_para[0].thrd_start, __empty_thrd_para[0].thrd_stop,
            __empty_thrd_para[0].athrd_cur, __empty_thrd_para[0].athrd_max);
    printf("__empty_thrd_para[1]: %d, %d -> %d, %d -> %d\n", __empty_thrd_para[1].f_index,
            __empty_thrd_para[1].thrd_start, __empty_thrd_para[1].thrd_stop,
            __empty_thrd_para[1].athrd_cur, __empty_thrd_para[1].athrd_max);

    if ( pthread_create(&tid0, NULL, empty_thrd, &__empty_thrd_para[0]) != 0 )
    {
        fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
        return;
    }
    if (1 != g_thread_num) {
        if ( pthread_create(&tid1, NULL, empty_thrd, &__empty_thrd_para[1]) != 0 )
        {
            fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
            return;
        }
    }

#ifndef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
    if ( pthread_join(tid0, NULL) != 0 )
    {
        fprintf(stderr, "pthread_join(%lu) failed. - %s\n", tid0, strerror(errno));
    }
    if ( pthread_join(tid1, NULL) != 0 )
    {
        fprintf(stderr, "pthread_join(%lu) failed. - %s\n", tid1, strerror(errno));
    }
#else /* __WITHOUT_EMPTYING_THRD_CRTED__ */
    if ( pthread_detach(tid0) != 0 )
    {
        fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid0, strerror(errno));
    }
    if ( pthread_detach(tid1) != 0 )
    {
        fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid1, strerror(errno));
    }
#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */
}

#ifdef __WITHOUT_EMPTYING_THRD_CRTED__ /* wdb_without_thrd_crting */
#if 0 /* wdb_lfix-4 */
static void wait_and_put_f_index(int f_index)
{
    struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */

    spinlock_lock(&utaf_empty_qlock);
    __empty_thrd_para[0].f_index = f_index;
    __empty_thrd_para[1].f_index = f_index;
    spinlock_unlock(&utaf_empty_qlock);

    while (1)
    {
        spinlock_lock(&utaf_empty_qlock);
        if ( __empty_thrd_para[0].f_index == -1 && __empty_thrd_para[1].f_index == -1 )
        {
            spinlock_unlock(&utaf_empty_qlock);
            break;
        }
        spinlock_unlock(&utaf_empty_qlock);
        nanosleep(&sleeptime, NULL);
    }
}
#else /* wdb_lfix-4 */

static __inline__ void utaf_atomic_set(int i, volatile int *target)
{
        __asm__ __volatile__(
                "lock; xchgl %1,%0"
                :"=m" (*target)
                :"ir" (i), "m" (*target));
}

#define utaf_fetch_and_dec(x) __sync_fetch_and_sub ((volatile int *)(x), (int)1)

static volatile int __tm_out_flag = 0;

static void *put_f_index_and_wait(void *data)
{
    struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
    //cpu_set_t mask;
    register int f_index;

#if 0
    CPU_ZERO(&mask);
 /* CPU_SET(0, &mask); //wdb_core */
    CPU_SET(__misc_cores, &mask);  //wdb_core
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "%s() - set thread affinity failed.\n", __FUNCTION__);
        exit(1);
    }
#endif

    while (1)
    {
        m_wmb();

        if (utaf_fetch_and_dec(&__tm_out_flag) == 1)
        {
            f_index = utaf_age_index_current + 1;
            __builtin_ia32_lfence();
            f_index = f_index%UTAF_AGE_RING_SIZE;

#if 0 /* wdb_sglpkt_age */
            __f0_index = utaf_age0_index_current + 1;
            __builtin_ia32_lfence();
            __f0_index = __f0_index%UTAF_AGE_RING0_SIZE;
#endif /* wdb_sglpkt_age */

            spinlock_lock(&utaf_empty_qlock);
            __empty_thrd_para[0].f_index = f_index;
            __empty_thrd_para[1].f_index = f_index;
            spinlock_unlock(&utaf_empty_qlock);

#if 1 /* wdb_lfix-5 */
            //utaf_timer_handler_cb(); //wdb_ppp
#endif /* wdb_lfix-5 */

            while (1)
            {
                spinlock_lock(&utaf_empty_qlock);
                if ( __empty_thrd_para[0].f_index == -1 && __empty_thrd_para[1].f_index == -1 )
                {
                    spinlock_unlock(&utaf_empty_qlock);
                    break;
                }
                spinlock_unlock(&utaf_empty_qlock);
                nanosleep(&sleeptime, NULL);
            }

            m_wmb();
            utaf_atomic_set(f_index, &utaf_age_index_current);
#if 0 /* wdb_sglpkt_age */
            m_wmb();
            utaf_atomic_set(__f0_index, &utaf_age0_index_current);
#endif /* wdb_sglpkt_age */
            m_wmb();
        }
        else
        {
            nanosleep(&sleeptime, NULL);
        }
    }

    return NULL;
}

static void f_index_put_thrd_crt(void)
{
    pthread_t tid;

    if ( pthread_create(&tid, NULL, put_f_index_and_wait, NULL) != 0 )
    {
        fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
        return;
    }
    if ( pthread_detach(tid) != 0 )
    {
        fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid, strerror(errno));
    }
}
#endif /* wdb_lfix-4 */

#endif /* __WITHOUT_EMPTYING_THRD_CRTED__ */


void utaf_ssn_age_timer_cb(void)
{
    utaf_atomic_set((int)1, &__tm_out_flag);
}

#if 1 /* wdb_core */
extern int parse_cores_string(char *opt, int *ret, int cc);

int __age_cores_initialized = 0;

int parse_age_cores(char *optarg)
{
    int __cores[SESSION_MAX_AGE_CORE], i;

    if ( parse_cores_string(optarg, __cores, SESSION_MAX_AGE_CORE) < SESSION_MAX_AGE_CORE )
    {
        return -1;
    }

    for (i = 0; i < (int)SESSION_MAX_AGE_CORE; i++)
    {
        session_age_core[i].cid = __cores[i];
    }

    __age_cores_initialized = 1;

    return 0;
}
#endif /* wdb_core */
//#endif /* wdb_as */


