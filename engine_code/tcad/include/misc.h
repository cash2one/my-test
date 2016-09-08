#ifndef __MISC_H__
#define __MISC_H__

//#define UTAF_DEBUG
//#define UTAF_DEBUG_RXTX
//#define UTAF_DEBUG_ETHER
//#define UTAF_DEBUG_IP
//#define UTAF_DEBUG_TCP
//#define UTAF_DEBUG_UDP
//#define UTAF_DEBUG_SESSION
//#define UTAF_DEBUG_DPI
//#define UTAF_DEBUG_HTTP
//#define UTAF_DEBUG_HTTP2
//#define UTAF_DEBUG_DNS
//#define UTAF_DEBUG_LOGP
//#define UTAF_DEBUG_FLOWLOCK
//#define UTAF_DEBUG_NETFLOW
//#define UTAF_DEBUG_RAWPKT
//#define UTAF_DEBUG_FLOWCAP

#define UTAF_MODULE_DPI

#define UTAF_DDOS

#define UTAF_URL

#define MAX_LCORE 64

#define __WITHOUT_EMPTYING_THRD_CRTED__ 1

extern int dpdk_get_core_id(void);

#define UTAF_RX_SUCCESS 0
#define UTAF_RX_DROP    1

enum {
UTAF_OK = 0,
UTAF_FAIL,
UTAF_MEM_FAIL
} __x3;

/* #define GLOBAL_VM_SIZE      (100ULL * 1024ULL * 1024ULL * 1024ULL) */
#define SESSION_MAX_AGE_CORE   16

#define unlikely(x)  __builtin_expect((x),0)

#define CACHE_LINE_SIZE 64                  /**< Cache line size. */
#define CACHE_LINE_MASK (CACHE_LINE_SIZE-1) /**< Cache line mask. */
#define cache_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))

/* utaf_flows.h: */ #define FLOW_CREATE_OK     0
/* utaf_flows.h: */ #define FLOW_CREATE_FAIL   1
/* utaf_flows.h: */ #define PACKET_RAW  1

#define UTAF_PER_LCORE(name) (utaf_per_lcore_##name)
#define UTAF_DECLARE_PER_LCORE(type, name)                      \
        extern __thread __typeof__(type) utaf_per_lcore_##name

UTAF_DECLARE_PER_LCORE(int, _lcore_id); /**< Per core "core id". */

/* wdb */
UTAF_DECLARE_PER_LCORE(int, lthrd_id);

static inline int utaf_lcore_id(void)
{
    //return UTAF_PER_LCORE(_lcore_id);
    return (0 == UTAF_PER_LCORE(_lcore_id) ? dpdk_get_core_id(): UTAF_PER_LCORE(_lcore_id));
}

#define APP_UNABLE_TO_IDENTIFY 0xffff

#define RTE_RING_NAMESIZE 32

/* #define utaf_get_timer_hz() 1 */

/* ringq.h: */ #define RING_F_SP_ENQ 0x0001 /**< The default enqueue is "single-producer". */
/* ringq.h: */ #define RING_F_SC_DEQ 0x0002 /**< The default dequeue is "single-consumer". */

#endif

