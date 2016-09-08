#ifndef _FLOW_COMMON_H_
#define _FLOW_COMMON_H_


#define SESSION_ITEM_MEMBER_NUM   16
#define CACHE_LINE_SIZE 64                  /**< Cache line size. */
#define CACHE_LINE_MASK (CACHE_LINE_SIZE-1) /**< Cache line mask. */
#define cache_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))
#define RINGQ_MAGIC  0xAABBCCDD

#define RING_F_SP_ENQ 0x0001 /**< The default enqueue is "single-producer". */
#define RING_F_SC_DEQ 0x0002 /**< The default dequeue is "single-consumer". */
#include "ringq.h"
#include "list_ddos.h"
#include "atomic.h"
#include "monitor_ip_list.h"
#include "ddos_trie.h"

typedef struct {
		volatile int locked; /**< lock status 0 = unlocked, 1 = locked */
} spinlock_t;
typedef struct tmout_lst_hdr_s
{
	struct hlist_head timeout_lst_head;
	struct tmout_lst_hdr_s *next;
} tmout_lst_hdr_t;
/**
 ** Initialize the spinlock to an unlocked state.
 **
 ** @param sl
 **   A pointer to the spinlock.
 **/
static inline void spinlock_init(spinlock_t *sl)
{
		sl->locked = 0;
}



/**
 ** Take the spinlock.
 **
 ** @param sl
 ** A pointer to the spinlock.
 *      */
static inline void spinlock_lock(spinlock_t *sl)
{

	int lock_val = 1;
	asm volatile (
			"1:\n"
			"lock; xchg %[locked], %[lv]\n"  /* wdb_lfix */
			"test %[lv], %[lv]\n"
			"jz 3f\n"
			"2:\n"
			"pause\n"
			"cmp $0, %[locked]\n"
			"jnz 2b\n"
			"jmp 1b\n"
			"3:\n"
			: [locked] "=m" (sl->locked), [lv] "=q" (lock_val)
			: "[lv]" (lock_val)
			: "memory");

}


/**
 *  * Release the spinlock.
 *   *
 *    * @param sl
 *     *   A pointer to the spinlock.
 *      */
static inline void spinlock_unlock (spinlock_t *sl)
{

	int unlock_val = 0;
	asm volatile (
			"lock; xchg %[locked], %[ulv]\n"  /* wdb_lfix */
			: [locked] "=m" (sl->locked), [ulv] "=q" (unlock_val)
			: "[ulv]" (unlock_val)
			: "memory");

}


/**
 *  * Try to take the lock.
 *   *
 *    * @param sl
 *     *   A pointer to the spinlock.
 *      * @return
 *       *   1 if the lock is successfully taken; 0 otherwise.
 *        */
static inline int spinlock_trylock (spinlock_t *sl)
{
	int lockval = 1;

	asm volatile (
			"lock; xchg %[locked], %[lockval]"  /* wdb_lfix */
			: [locked] "=m" (sl->locked), [lockval] "=q" (lockval)
			: "[lockval]" (lockval)
			: "memory");

	return (lockval == 0);

}


/**
 *  * Test if the lock is taken.
 *   *
 *    * @param sl
 *     *   A pointer to the spinlock.
 *      * @return
 *       *   1 if the lock is currently taken; 0 otherwise.
 *        */
static inline int rte_spinlock_is_locked (spinlock_t *sl)
{
		return sl->locked;
}

/*线程的ring指针*/
struct session_item_ringq_group
{
		uint32_t global_index;
		struct hlist_head session_item_ringq[SESSION_ITEM_MEMBER_NUM];
		spinlock_t session_item_rq_lock[SESSION_ITEM_MEMBER_NUM];
};

/*** old flow event*****/
typedef struct event_struct{
	time_t          ftime_start;            /* 开始时间，自1970.1.1年以来的秒数 */
	time_t          ftime_end;              /* 结束时间，自1970.1.1年以来的秒数 */
	time_t          visit_time;             /* 访问时长，单位秒 */
	uint32_t        sip;                    /* 用户ip */
	uint32_t        dip;                    /* 目的ip */
	uint16_t        usersport;              /* 用户源端口号 */
	uint16_t        userdport;              /* 目的端口号 */
	uint16_t        procotol_id;            /* 协议id: 2代表TCP 3代表UDP 4代表OTHERS */
	uint8_t         country;                /* 国家 */
	uint8_t         pc_if;                  /* 0 为pc端 ；1 为移动端 */
	uint8_t         tcp_suc;                /* tcp连接是否成功 0 tcp连接失败，1为连接成功 */
	uint8_t         bank_visit;             /* 业务访问是否成功：如：http 返回200认为成功 */
	uint8_t         flag;                   /* 0为非长链接，1 为长连接 */
	uint8_t         incr;                   /* 0表示增量日志，1表示老化 */
	uint16_t        s_isp_id;               /* 源运营商ID */
	uint16_t        d_isp_id;               /* 目的运营商ID */
	uint32_t        s_province_id;          /* 源省ID */
	uint32_t        d_province_id;          /* 目的省ID */
	uint32_t        s_city_id;              /* 源地市ID */
	uint32_t        d_city_id;              /* 目的地市ID */
	uint64_t        ip_byte;                /* 字节数 */
	uint8_t			direction_flag;			/* 0 = in 1 = out*/
	uint64_t		up_bytes;				/* 上行流量*/
	uint64_t		down_bytes;				/* 下行流量*/
	uint16_t		service_group_id;		/* 应用类型组id*/
	uint32_t		service_type_id;		/* 应用类型id*/
	uint64_t        ip_pkt;                 /* 数据包 */
	time_t          cli_ser;                /* 端到端的时延:所有包的平均值，单位毫秒 */
	int   			browser;				/* 浏览器类型，例：IE、chrome 等 */
	unsigned char   version[SAM_STR_SIZE+1];/* 浏览器版本 */
}*Pflow_event,flow_event;

typedef struct session_tables{
	struct hlist_node list;
	flow_event session_node;
	struct hlist_node tm_lst;
	spinlock_t item_lock;
	spinlock_t tmrng_lck;
}session_table;
typedef struct session_item_header_tag_s
{
	uint32_t session_item_magic;
	uint32_t ringq_id;
}session_item_header;
enum {
	UTAF_OK = 0,
	UTAF_FAIL,
	UTAF_MEM_FAIL
} ;

struct utaf_session_age_core_s {
	int tid;
	uint32_t cid;
	uint32_t start;
	uint32_t end;
	struct timer *timer;
};

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#define POWEROF2(x) ((((x)-1) & (x)) == 0)

#define SESSION_SIZE sizeof(session_table)
#define SESSION_HEAD_SIZE sizeof(session_item_header)
#define SESSION_MAGIC     0xabcddcba
#define SI_BUNCH_SIZE 400
#define UTAF_AGE_RING_SIZE 130
#define RING_F_SP_ENQ 0x0001 /**< The default enqueue is "single-producer". */
#define RING_F_SC_DEQ 0x0002 /**< The default dequeue is "single-consumer". */
#define SESSION_MAX_AGE_CORE   16

#define UTAF_DEFINE_PER_LCORE(type, name)                       \
	        __thread __typeof__(type) utaf_per_lcore_##name

#define UTAF_DECLARE_PER_LCORE(type, name) \
	        extern __thread __typeof__(type) utaf_per_lcore_##name
#define UTAF_PER_LCORE(name) (utaf_per_lcore_##name)


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


extern struct session_item_ringq_group session_group;
int init_session_tables();
extern volatile time_t g_utaf_time_jiffies;
extern time_t g_traffic_last_time;
static inline int get_power(int x);
extern  struct hlist_head utaf_l_si_bunch[16];
extern int utaf_age_index_current;
uint32_t TimerListInit(void);
int utaf_ssn_age_thread_crt(void);
extern int create_flow_srv_thr(void);
extern struct hlist_head g_utaf_timeout_ring[UTAF_AGE_RING_SIZE][16];
extern spinlock_t        g_utaf_timeout_rlck[UTAF_AGE_RING_SIZE][16];
extern int sockfd;
#endif
