#ifndef __UTAF_SESSION_H__
#define __UTAF_SESSION_H__

#if 0
#include "utaf.h"
#include <list.h>
#include "mbuf.h"
#include <dns.h>
#include <httpX.h>
#include "spinlock.h"
#include "rwlock.h"
#endif
#include <list.h>
#include "spinlock.h"
#include "rwlock.h"
#include <dns.h>
#include <httpX.h>
#include "cdpi_api.h"
#include <common.h>
#include "ddos.h"

#define SESSION_MAGIC     0xabcddcba


#define SESSION_VALID   0
#define SESSION_INVALID 1

#if 0 /* wdb_ppp */
#define SESSION_MAX_TIMEOUT    (60*utaf_get_timer_hz())       /* 60s */
#define SESSION_MAX_INC_LOG    (120*utaf_get_timer_hz())      /* 120s */
#else /* wdb_ppp */
#define SESSION_MAX_TIMEOUT    (60)       /* 60s */
#define SESSION_MAX_INC_LOG    (120)      /* 120s */
#endif /* wdb_ppp */

#if 0
#define UTAF_SESSION_BUCKET_NUM     0x400000    //(4*1024*1024)
#define UTAF_SESSION_BUCKET_MASK    0x3FFFFF    //(4*1024*1024-1)
#define UTAF_SESSION_ITEM_NUM       0xA00000    //(10*1024*1024)
#else
#define UTAF_SESSION_BUCKET_NUM     0x200000    //(2*1024*1024)
#define UTAF_SESSION_BUCKET_MASK    0x1FFFFF    //(2*1024*1024-1)
#define UTAF_SESSION_ITEM_NUM       0x400000    //(4*1024*1024)
#endif

#define DIRECT_FLAG_UP          0x1
#define DIRECT_FLAG_DOWN        0x2

#define UP_DIRECTION           0
#define DOWN_DIRECTION         1
#define DOUBLE_DIRECTION       2

#define ADD_FCS_TOTAL_LEN       4   // 恒为不统计FCS字段

#if 1 /* wdb_as */

#define __WDB_SCO__ 1 // wdb_sco
/* #define __WDB_SCO_2plus__ 1 // wdb_sco */

/* #define UTAF_AS2_DEBUG 1 */

#define UTAF_AGE_RING_SIZE 130

#if 0 /* wdb_sglpkt_age */
#define UTAF_AGE_RING0_SIZE 10
#endif /* wdb_sglpkt_age */

#define SI_BUNCH_SIZE 400 /* 400 is best ! */

#endif /* wdb_as */

typedef struct session_table_info_tag_s
{
	uint32_t bucket_num;
	uint32_t bucket_size;

	uint32_t item_size;
	uint32_t item_num;
	void *bucket_base_ptr;
}session_table_info_t;



typedef struct session_bucket_tag_s
{
	struct hlist_head hash;
#ifdef UTAF_LOCK_SPINLOCK
	spinlock_t lock;
#else
	rwlock_t lock;
#endif
	uint32_t link_len;
}session_bucket_t;





typedef struct session_item_header_tag_s
{
	uint32_t session_item_magic;
	uint32_t ringq_id;
}session_item_header cache_aligned;


#define TCP_SEQ_WINDOW_SIZE 10
struct tcp_seq_info
{
	uint32_t index_w;
	uint32_t seq_window[TCP_SEQ_WINDOW_SIZE];
};



struct tcp_segment_info
{
	uint64_t retrans_pkt;
	uint64_t ood;
};


/*此结构不动， 仍然是双向流结构 - - - wangfan@20140529*/
typedef struct session_item_tag_s
{
	struct hlist_node	list;
//#ifdef UTAF_LOCK_SPINLOCK  //wdb_lfix-6
	spinlock_t item_lock;
//#else  //wdb_lfix-6
//	rwlock_t item_lock;  //wdb_lfix-6
//#endif  //wdb_lfix-6
#if 0 /* wdb_as */
    struct session_item_tag_s *inc_next;
#else /* wdb_as */

#if 1 /* wdb_sglpkt_age */
    uint8_t age_ring;
#endif /* wdb_sglpkt_age */
    uint8_t  pc_if;
    uint8_t  tcp_suc;
    uint8_t  bank_visit;

    struct hlist_node tm_lst;
#if 1 /* wdb_without_thrd_crting */
    spinlock_t tmrng_lck;
#endif
    int ring_idx;
    int thrd_idx;
    int hash;
#endif /* wdb_as */
	uint64_t cycle;
    uint64_t last_log_cycle;
	//uint64_t session_id;
	
	ip_tuple_t ip_tuple;
	
	uint16_t  sport;
   	uint16_t  dport;
	uint8_t  protocol;
    uint8_t  ip_version;
	uint16_t  app_id;
    
    uint64_t  start_time;
	//uint64_t  end_time;

	uint64_t pkts[2];   // 0表示正向, 1表示反向
	uint64_t bytes[2];  // 0表示正向, 1表示反向
	//uint64_t netflow_pkts[2];   // 0表示正向, 1表示反向
	//uint64_t netflow_bytes[2];  // 0表示正向, 1表示反向
    //uint64_t last_log_pkts[2];   // 0表示正向, 1表示反向
	//uint64_t last_log_bytes[2];  // 0表示正向, 1表示反向
	
	uint8_t  tcp_flag;	// 如syn（bit0）、syn-ack（bit1）和ack（bit2）。
	uint8_t  direct_flags;	// 如up(bit0）、down （bit1）
	uint8_t  lock;
	uint8_t  inter_flow_stat;
	uint16_t session_status;
	uint16_t service_group;
    
	uint32_t service_type;
	//uint32_t ip1_eth_id;
    
	//uint32_t ip2_eth_id;

#if 0
	uint16_t Tcp_Reply_Delay;
	uint16_t Tcp_Ack_Delay;
    
	uint16_t Tcp_Conn_Delay;
	uint16_t Tcp_Sve_Delay;
	uint16_t Tcp_Win_Ip1;
	uint16_t Tcp_Win_Ip2;
    
	uint8_t  Tcp_Win_Ip1_Ext;
	uint8_t  Tcp_Win_Ip2_Ext;
	uint16_t Tcp_SYN_Number;
	uint8_t  Tcp_Success;
	uint8_t  Tcp_End;
	uint16_t Tcp_Mss;
    
	uint16_t Tcp_Disorder_Num1;
	uint16_t Tcp_Disorder_Num2;
	uint16_t Tcp_Resend_Num1;
	uint16_t Tcp_Resend_Num2;
#endif
    //uint16_t stat_netflow_num;
    //uint16_t age_netflow_num;
    uint8_t app_flag;
    uint8_t send_log_flag;
    uint8_t bgp_version;
    uint8_t country;
#if 0
    unsigned char tasks[16];
    uint64_t tsk_cksum;
    uint16_t tsk_num;
    uint16_t tsk_none;
#endif
    uint16_t s_isp_id;
	uint16_t d_isp_id;
	uint32_t s_province_id;
	uint32_t d_province_id;
	uint32_t s_city_id;
	uint32_t d_city_id;

    struct cdpi_flow fflow;
    
#if 0
	struct tcp_segment_info tcpsegi[2];
	struct tcp_seq_info tcpSi[2];
#endif
    
	union 
	{
		struct dns_info dnsinfo;  //wdb_ppp
		struct http_info httpinfo;
	}app_info; 
	
#ifdef UTAF_DDOS
	uint8_t flow_dir:4,
			tcp_conn_dir:4;       /* flow direction to_server:0 to_client:1 */
	uint8_t tcp_state;       /* tcp state */
#define TCP_CLOSE_WAIT1 0x01	 /* first th_fin */
#define TCP_CLOSE_WAIT2 0x02   /* sec th_fin */
#define TCP_CLOSE_WAIT3 0x04   /* first th_finack */
#define TCP_CLOSE_WAIT4 0x08   /* sec th_finack */
	uint8_t tcp_closing;
	uint8_t ddos_flag:1,      /* record if need to ddos */
			ddos_ssn_new:1,
			ddos_ssn_close:1,
			reserving:5;
	struct ddos ddos;
#endif
}session_item_t cache_aligned;


#define SESSION_ITEM_MEMBER_NUM   16
#define SESSION_ITEM_MEMBER_MASK (SESSION_ITEM_MEMBER_NUM - 1)
struct session_item_ringq_group
{
	uint32_t global_index;
#ifndef __WDB_SCO__ /* wdb_sco */
	struct ringq *session_item_ringq[SESSION_ITEM_MEMBER_NUM];
#else /* defined(__WDB_SCO__) */ /* wdb_sco */
	struct hlist_head session_item_ringq[SESSION_ITEM_MEMBER_NUM];
	spinlock_t session_item_rq_lock[SESSION_ITEM_MEMBER_NUM];
#endif /* defined(__WDB_SCO__) */ /* wdb_sco */
};


struct utaf_session_age_core_s {
	int tid;
	uint32_t cid;
	uint32_t start;
    uint32_t end;
    struct timer *timer;
};


#define SESSION_ITEM_HEADER_SIZE sizeof(session_item_header)
#define SESSION_ITEM_SIZE sizeof(session_item_t)



extern uint32_t session_input(struct m_buf *mbuf);
extern uint32_t session_table_init(void);  //wdb_calc222
session_item_t *session_find(session_bucket_t *base, struct m_buf *mbuf, unsigned int hash);
void session_table_age_init(struct utaf_session_age_core_s *ac);
void utaf_session_age_thread_init(void);
inline void session_update_isp_info(session_item_t *si);

#endif
