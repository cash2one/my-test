#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <ddos.h>

/* #include "ringq.h" //wdb_ppp */ #define RTE_RING_NAMESIZE 32


#define STATISTICS_MAGIC 0xabcddcba


#define LOGP_RINGQ_VALID 1
#define LOGP_RINGQ_INVALID 0

enum {
	CONF_RELOAD_DDOS_IPLIST = 1,
	CONF_RELOAD_DDOS_DEBUG,
	CONF_RELOAD_DDOS_LOG,
	CONF_RELOAD_DDOS_FLOW,
	CONF_RELOAD_DDOS_DETECT,
	CONF_RELOAD_DDOS_DATABASE,
	CONF_RELOAD_DDOS_BASELINE,
	/* conf class */
	CONF_RELOAD_OTHER,
	CONF_RELOAD_ALL = 0xFF
};

struct recv_count 
{
	uint64_t recv_packet_count;
	uint64_t recv_packet_bytes;

	uint64_t recv_packet_count_sum;
	uint64_t recv_packet_bytes_sum;
};

struct recv_speed
{
	uint64_t recv_pps;
	uint64_t recv_bps;
};


struct logp_cache_stat
{
	uint32_t used_count;
	uint32_t free_count;
	uint64_t enqueue_ok;
	uint64_t enqueue_no;
};

struct logp_buf_stat
{
	uint32_t used_count;
	uint32_t free_count;
	uint64_t enqueue_ok;
	uint64_t enqueue_no;
};

struct logp_base_stat
{
	uint32_t valid;
	char name[RTE_RING_NAMESIZE];
	uint64_t enqueue_ok;
	uint64_t enqueue_no;
};

#define MAX_LOGP_BASE_COUNT 1000

struct rx_stat
{
	uint64_t version_err;
	uint64_t no_buffer;
	uint64_t rx_ok;
};

struct ether_stat
{
    uint64_t rx_all;
	uint64_t rx_ok;
    uint64_t raw_pkt_fail;
};

struct network_stat
{
	uint64_t packet_invalid;
	uint64_t datalen_err;
	uint64_t ip_packet;
	uint64_t first_ip_frag;
	uint64_t other_ip_frag;
	uint64_t deliver_fail;
	uint64_t defrag_fail;
	uint64_t session_fail;
	uint64_t rx_ok;
};

struct icmp_stat
{
	uint64_t icmp_fail;
    uint64_t rx_ok;
};
struct tcp4_stat
{
	uint64_t datalen_err;
	uint64_t frag_fail;
	uint64_t session_fail;
	uint64_t rx_ok;
};

struct udp4_stat
{
	uint64_t datalen_err;
	uint64_t frag_fail;
	uint64_t session_fail;
	uint64_t rx_ok;
};

struct other_stat
{
	uint64_t datalen_err;
	uint64_t frag_fail;
	uint64_t session_fail;
	uint64_t rx_ok;
};

struct transport_stat
{
	struct tcp4_stat tcpstat[64];
	struct udp4_stat udpstat[64];
    struct other_stat otherstat[64];
};


struct session_stat
{
	uint64_t session_add;
	uint64_t session_del;
    uint64_t session_aged;
	uint64_t session_full;
	uint64_t session_raw_pkt_fail;
    uint64_t session_up;
    uint64_t session_down;
    uint64_t session_up_down;
    uint64_t session_eth_id_zero;
    uint64_t session_eth_id_zero_pkts;
    uint64_t session_eth_id_zero_bytes;
    uint64_t session_no_netflow;
    //uint64_t session_no_netflow_tcp;
    //uint64_t session_no_netflow_udp;
    //uint64_t session_no_netflow_icmp;
    uint64_t session_no_stat_netflow;
    //uint64_t session_no_stat_netflow_tcp;
    //uint64_t session_no_stat_netflow_udp;
    //uint64_t session_no_stat_netflow_icmp;
    uint64_t session_no_age_netflow;
    //uint64_t session_no_age_netflow_tcp;
    //uint64_t session_no_age_netflow_udp;
    //uint64_t session_no_age_netflow_icmp;
    uint64_t session_single_packet;
    uint64_t session_ddos;
    uint64_t session_tcp;
    uint64_t session_udp;
    uint64_t session_icmp;
    uint64_t session_tcp_pkts;
    uint64_t session_tcp_bytes;
    uint64_t session_udp_pkts;
    uint64_t session_udp_bytes;
    uint64_t session_http_pkts;
    uint64_t session_http_bytes;
    uint64_t session_event_age_success;
    uint64_t session_event_inc_success;
    uint64_t session_event_age_failed;
    uint64_t session_event_inc_failed;
};


struct session_app
{
	uint64_t dns;
	uint64_t http;
};


struct cdr_stat
{
	uint64_t recv_packet_count;
	uint64_t recv_packet_bytes;

	uint64_t recv_packet_count_sum;
	uint64_t recv_packet_ages_sum;

    uint64_t alloc_failed;
    uint64_t alloc_succeed;
    uint64_t freed;
    
    uint64_t pkts;
    uint64_t bytes;
};

struct dist_stat
{
	uint64_t logp_recv;
    uint64_t cdr_dist_failed;
    uint64_t cdr_dist_succeed;
};

struct merge_stat
{
	uint64_t cdr_recv;
    uint64_t cdr_merged;
    uint64_t log_construct_failed;
    uint64_t lh_insert_failed;
    uint64_t task_null;
};

struct net_flow
{
    uint64_t pkt_num;
	uint64_t session_stat;
    uint64_t session_age;
    uint64_t session_other;
    uint64_t error;
    uint64_t error_ipver;
    uint64_t age_pkt_num;
    uint64_t age_bytes;
    uint64_t stat_pkt_num;
    uint64_t stat_bytes;  
    uint64_t no_find_session_stat;
    uint64_t no_find_session_age;
    uint64_t no_find_sess_age_bytes;
    uint64_t no_find_sess_stat_bytes;
};

struct flow_lock
{
    uint64_t send;
	uint64_t succeed;
};

struct cdr_speed
{
	uint64_t recv_pps;
	uint64_t recv_bps;
};

struct flow_defined
{
	uint64_t succeed;
    uint64_t failed;
};

struct gms_udp_server
{
	uint32_t udp_server;
    uint16_t define_flow_port;
    uint16_t flow_event_port;
    int udp_server_socket;
};

struct ddos_trie_ipinfo{
	int type;
	unsigned int ip;
	unsigned int plen;/*or end ip*/
};
#define DDOS_IP_RULE_SHOW_MAX_SIZE		(1024)  /* ip rule show support max 1024*/
struct ddos_trie_dbg{
	unsigned int monitor_cnt;
	unsigned int white_cnt;
	struct ddos_trie_ipinfo monitor_tip[DDOS_IP_RULE_SHOW_MAX_SIZE];
	struct ddos_trie_ipinfo white_tip[DDOS_IP_RULE_SHOW_MAX_SIZE];
};

struct monitor
{
	uint32_t magic;

	struct recv_count rc;
	struct recv_speed rs;

	struct logp_cache_stat lcs;

	struct logp_buf_stat lbufs;

	uint32_t logp_base_count;
	struct logp_base_stat lbs[MAX_LOGP_BASE_COUNT];

	struct recv_count mt_rc[64];
	struct recv_speed mt_rs[64];

	struct rx_stat rxstat[64];
	
	struct ether_stat ethstat[64];

	struct network_stat nwstat[64];

	struct icmp_stat icmpstat[64];
	
	struct transport_stat tpstat;
	
	struct session_stat sessstat[64];

	struct session_app sessapp[64];

	struct cdr_stat cdrstat[64];
    struct cdr_speed cdrspeed[64];
	struct dist_stat diststat[64];
    struct merge_stat mergestat[64];

    struct net_flow netflow[64];
    struct flow_lock flowlock[64];

    struct flow_defined flowdefined[64];

    struct gms_udp_server gms_info;

	/*ddos trie ip debug*/
	struct ddos_trie_dbg dt_dbg;
	struct ddos_conf    ddos_cfg;
	uint32_t reconf_flag;
};

extern struct monitor *monitor_info;
extern struct ringq *logp_cache_queue;

#define RECV_PC_ADD(count) do { monitor_info->rc.recv_packet_count++; monitor_info->rc.recv_packet_count_sum++; } while (0)
#define RECV_PB_ADD(bytes) do { monitor_info->rc.recv_packet_bytes += bytes; monitor_info->rc.recv_packet_bytes_sum += bytes; } while (0)


#define LOGP_CACHE_STAT do { monitor_info->lcs.used_count = ringq_count(logp_cache_queue); monitor_info->lcs.free_count = ringq_free_count(logp_cache_queue);} while(0)
#define LOGP_CACHE_ENQUEUE_OK do { monitor_info->lcs.enqueue_ok++; } while(0)
#define LOGP_CACHE_ENQUEUE_NO do { monitor_info->lcs.enqueue_no++; } while(0)

#define LOGP_BUF_STAT do { monitor_info->lbufs.used_count = ringq_count(logp_buf_rq); monitor_info->lbufs.free_count = ringq_free_count(logp_buf_rq);} while(0)
//#define LOGP_BUF_ENQUEUE_OK do { monitor_info->lbufs.enqueue_ok++; } while(0)
//#define LOGP_BUF_ENQUEUE_NO do { monitor_info->lbufs.enqueue_no++; } while(0)



#define LOGP_BASE_ENQUEUE_OK(ID) do { monitor_info->lbs[ID].enqueue_ok++; } while(0)
#define LOGP_BASE_ENQUEUE_NO(ID) do { monitor_info->lbs[ID].enqueue_no++; } while(0)


#define MT_RECV_PC_ADD(count) do { monitor_info->mt_rc[utaf_lcore_id()].recv_packet_count++; monitor_info->mt_rc[utaf_lcore_id()].recv_packet_count_sum++; } while (0)
#define MT_RECV_PB_ADD(bytes) do { monitor_info->mt_rc[utaf_lcore_id()].recv_packet_bytes += bytes; monitor_info->mt_rc[utaf_lcore_id()].recv_packet_bytes_sum += bytes; } while (0)


#define DEBUG_RX_STAT_VERSION_ERR do { monitor_info->rxstat[utaf_lcore_id()].version_err++; } while(0)
#define DEBUG_RX_STAT_NO_BUFFER   do { monitor_info->rxstat[utaf_lcore_id()].no_buffer++; } while(0)
#define DEBUG_RX_STAT_RX_OK       do { monitor_info->rxstat[utaf_lcore_id()].rx_ok++; } while(0)


#define DEBUG_ETHER_STAT_RX_ALL   do { monitor_info->ethstat[utaf_lcore_id()].rx_all++; } while(0)
#define DEBUG_ETHER_STAT_RX_OK    do { monitor_info->ethstat[utaf_lcore_id()].rx_ok++; } while(0)
#define DEBUG_ETHER_STAT_RAW_PKT_FAIL  do { monitor_info->ethstat[utaf_lcore_id()].raw_pkt_fail++; } while(0)


#define DEBUG_NETWORK_STAT_INVALID     do { monitor_info->nwstat[utaf_lcore_id()].packet_invalid++; } while(0)
#define DEBUG_NETWORK_STAT_DATALEN_ERR do { monitor_info->nwstat[utaf_lcore_id()].datalen_err++; } while(0)
#define DEBUG_NETWORK_STAT_IP_PKT        do { monitor_info->nwstat[utaf_lcore_id()].ip_packet++; } while(0)
#define DEBUG_NETWORK_STAT_FIRST_IP_FRAG do { monitor_info->nwstat[utaf_lcore_id()].first_ip_frag++; } while(0)
#define DEBUG_NETWORK_STAT_OTHER_IP_FRAG do { monitor_info->nwstat[utaf_lcore_id()].other_ip_frag++; } while(0)
#define DEBUG_NETWORK_DELIVER_FAIL  do { monitor_info->nwstat[utaf_lcore_id()].deliver_fail++; } while(0)
//#define DEBUG_NETWORK_DEFRAG_FAIL   do { monitor_info->nwstat[utaf_lcore_id()].defrag_fail++; } while(0)
//#define DEBUG_NETWORK_SESSION_FAIL   do { monitor_info->nwstat[utaf_lcore_id()].session_fail++; } while(0)
#define DEBUG_NETWORK_STAT_RX_OK       do { monitor_info->nwstat[utaf_lcore_id()].rx_ok++; } while(0)

#define DEBUG_NETWORK_STAT_ICMP_FAIL do { monitor_info->icmpstat[utaf_lcore_id()].icmp_fail++; } while(0)
#define DEBUG_NETWORK_STAT_ICMP_RX_OK do { monitor_info->icmpstat[utaf_lcore_id()].rx_ok++; } while(0)


#define DEBUG_TRANSPORT_STAT_TCP_DATALEN_ERR  do { monitor_info->tpstat.tcpstat[utaf_lcore_id()].datalen_err++; } while(0)
#define DEBUG_TRANSPORT_STAT_TCP_DEFRAG_FAIL  do { monitor_info->tpstat.tcpstat[utaf_lcore_id()].frag_fail++; } while(0)
#define DEBUG_TRANSPORT_STAT_TCP_SESSION_FAIL do { monitor_info->tpstat.tcpstat[utaf_lcore_id()].session_fail++; } while(0)
#define DEBUG_TRANSPORT_STAT_TCP_RX_OK        do { monitor_info->tpstat.tcpstat[utaf_lcore_id()].rx_ok++; } while(0)


#define DEBUG_TRANSPORT_STAT_UDP_DATALEN_ERR   do { monitor_info->tpstat.udpstat[utaf_lcore_id()].datalen_err++; } while(0)
#define DEBUG_TRANSPORT_STAT_UDP_DEFRAG_FAIL   do { monitor_info->tpstat.udpstat[utaf_lcore_id()].frag_fail++; } while(0)
#define DEBUG_TRANSPORT_STAT_UDP_SESSION_FAIL  do { monitor_info->tpstat.udpstat[utaf_lcore_id()].session_fail++; } while(0)
#define DEBUG_TRANSPORT_STAT_UDP_RX_OK         do { monitor_info->tpstat.udpstat[utaf_lcore_id()].rx_ok++; } while(0)

#define DEBUG_TRANSPORT_STAT_OTHER_DATALEN_ERR   do { monitor_info->tpstat.otherstat[utaf_lcore_id()].datalen_err++; } while(0)
#define DEBUG_TRANSPORT_STAT_OTHER_DEFRAG_FAIL   do { monitor_info->tpstat.otherstat[utaf_lcore_id()].frag_fail++; } while(0)
#define DEBUG_TRANSPORT_STAT_OTHER_SESSION_FAIL  do { monitor_info->tpstat.otherstat[utaf_lcore_id()].session_fail++; } while(0)
#define DEBUG_TRANSPORT_STAT_OTHER_RX_OK         do { monitor_info->tpstat.otherstat[utaf_lcore_id()].rx_ok++; } while(0)


#define DEBUG_SESSION_STAT_SESSION_ADD   do { monitor_info->sessstat[utaf_lcore_id()].session_add++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_DEL   do { monitor_info->sessstat[utaf_lcore_id()].session_del++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_AGED(num)   do { monitor_info->sessstat[utaf_lcore_id()].session_aged += (num); } while(0)
#define DEBUG_SESSION_STAT_SESSION_FULL  do { monitor_info->sessstat[utaf_lcore_id()].session_full++; } while(0)
#define DEBUG_SESSION_STAT_RAW_PKT_SESSION_FAIL  do { monitor_info->sessstat[utaf_lcore_id()].session_raw_pkt_fail++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_NO_NETFLOW  do { monitor_info->sessstat[utaf_lcore_id()].session_no_netflow++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_NETFLOW_TCP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_netflow_tcp++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_NETFLOW_UDP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_netflow_udp++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_NETFLOW_ICMP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_netflow_icmp++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_NO_STAT_NETFLOW  do { monitor_info->sessstat[utaf_lcore_id()].session_no_stat_netflow++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_STAT_NETFLOW_TCP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_stat_netflow_tcp++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_STAT_NETFLOW_UDP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_stat_netflow_udp++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_STAT_NETFLOW_ICMP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_stat_netflow_icmp++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_NO_AGE_NETFLOW  do { monitor_info->sessstat[utaf_lcore_id()].session_no_age_netflow++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_AGE_NETFLOW_TCP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_age_netflow_tcp++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_AGE_NETFLOW_UDP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_age_netflow_udp++; } while(0)
//#define DEBUG_SESSION_STAT_SESSION_NO_AGE_NETFLOW_ICMP  do { monitor_info->sessstat[utaf_lcore_id()].session_no_age_netflow_icmp++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_SINGLE_PKT  do { monitor_info->sessstat[utaf_lcore_id()].session_single_packet++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_DDOS  do { monitor_info->sessstat[utaf_lcore_id()].session_ddos++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_UP  do { monitor_info->sessstat[utaf_lcore_id()].session_up++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_DOWN  do { monitor_info->sessstat[utaf_lcore_id()].session_down++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_UP_DOWN  do { monitor_info->sessstat[utaf_lcore_id()].session_up_down++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_ETH_ID_ZERO  do { monitor_info->sessstat[utaf_lcore_id()].session_eth_id_zero++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_ETH_ID_ZERO_PKTS(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_eth_id_zero_pkts += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_ETH_ID_ZERO_BYTES(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_eth_id_zero_bytes += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_TCP  do { monitor_info->sessstat[utaf_lcore_id()].session_tcp++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_UDP  do { monitor_info->sessstat[utaf_lcore_id()].session_udp++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_ICMP  do { monitor_info->sessstat[utaf_lcore_id()].session_icmp++; } while(0)
#define DEBUG_SESSION_STAT_SESSION_TCP_PKTS(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_tcp_pkts += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_TCP_BYTES(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_tcp_bytes += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_UDP_PKTS(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_udp_pkts += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_UDP_BYTES(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_udp_bytes += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_HTTP_PKTS(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_http_pkts += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_HTTP_BYTES(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_http_bytes += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_EVENT_AGE_S(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_event_age_success += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_EVENT_INC_S(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_event_inc_success += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_EVENT_AGE_F(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_event_age_failed += (count); } while(0)
#define DEBUG_SESSION_STAT_SESSION_EVENT_INC_F(count)  do { monitor_info->sessstat[utaf_lcore_id()].session_event_inc_failed += (count); } while(0)

#define DEBUG_CDR_STAT_PC_ADD_MT(count) do { monitor_info->cdrstat[utaf_lcore_id()].recv_packet_count++; monitor_info->cdrstat[utaf_lcore_id()].recv_packet_count_sum++; } while (0)
#define DEBUG_CDR_STAT_PA_ADD_MT(count) do { monitor_info->cdrstat[utaf_lcore_id()].recv_packet_bytes++; monitor_info->cdrstat[utaf_lcore_id()].recv_packet_ages_sum++; } while (0)
#define DEBUG_CDR_STAT_ALLOC_FAILED do { monitor_info->cdrstat[utaf_lcore_id()].alloc_failed++;} while (0)
#define DEBUG_CDR_STAT_ALLOC_SUCCEED do { monitor_info->cdrstat[utaf_lcore_id()].alloc_succeed++;} while (0)
#define DEBUG_CDR_STAT_FREED do { monitor_info->cdrstat[utaf_lcore_id()].freed++;} while (0)
#define DEBUG_CDR_STAT_PKTS(count)   do { monitor_info->cdrstat[utaf_lcore_id()].pkts += (count); } while(0)
#define DEBUG_CDR_STAT_BYTES(count)   do { monitor_info->cdrstat[utaf_lcore_id()].bytes += (count); } while(0)

#define DEBUG_DIST_STAT_LOGP_RECV(cid) do { monitor_info->diststat[(cid)].logp_recv++;} while (0)
#define DEBUG_DIST_STAT_CDR_DIST_S(cid) do { monitor_info->diststat[(cid)].cdr_dist_succeed++;} while(0)
#define DEBUG_DIST_STAT_CDR_DIST_F(cid) do { monitor_info->diststat[(cid)].cdr_dist_failed++;} while(0)
#define DEBUG_MERGE_STAT_CDR_RECV(cid) do { monitor_info->mergestat[(cid)].cdr_recv++;} while (0)
#define DEBUG_MERGE_STAT_CDR_MERGED(cid) do { monitor_info->mergestat[(cid)].cdr_merged++;} while (0)
#define DEBUG_MERGE_STAT_CDR_LOG_FAILED(cid) do { monitor_info->mergestat[(cid)].log_construct_failed++;} while (0)
#define DEBUG_MERGE_STAT_CDR_LH_INSERT_FAILED(cid) do { monitor_info->mergestat[(cid)].lh_insert_failed++;} while (0)
#define DEBUG_MERGE_STAT_CDR_TASK_NULL(cid) do { monitor_info->mergestat[(cid)].task_null++;} while (0)

#define DEBUG_SESSION_DNS_ADD    do { monitor_info->sessapp[utaf_lcore_id()].dns++; } while (0)
#define DEBUG_SESSION_HTTP_ADD   do { monitor_info->sessapp[utaf_lcore_id()].http++; } while (0)

#define DEBUG_NETFLOW_PKT_NUM   do { monitor_info->netflow[utaf_lcore_id()].pkt_num++; } while(0)
#define DEBUG_NETFLOW_SESSION_STAT   do { monitor_info->netflow[utaf_lcore_id()].session_stat++; } while(0)
#define DEBUG_NETFLOW_SESSION_AGE   do { monitor_info->netflow[utaf_lcore_id()].session_age++; } while(0)
#define DEBUG_NETFLOW_SESSION_OTHER   do { monitor_info->netflow[utaf_lcore_id()].session_other++; } while(0)
#define DEBUG_NETFLOW_ERROR   do { monitor_info->netflow[utaf_lcore_id()].error++; } while(0)
#define DEBUG_NETFLOW_ERROR_IPVER   do { monitor_info->netflow[utaf_lcore_id()].error_ipver++; } while(0)
#define DEBUG_NETFLOW_AGE_PKT_NUM(count)   do { monitor_info->netflow[utaf_lcore_id()].age_pkt_num += count; } while(0)
#define DEBUG_NETFLOW_AGE_BYTES(bytes)   do { monitor_info->netflow[utaf_lcore_id()].age_bytes += bytes; } while(0)
#define DEBUG_NETFLOW_STAT_PKT_NUM(count)   do { monitor_info->netflow[utaf_lcore_id()].stat_pkt_num += count; } while(0)
#define DEBUG_NETFLOW_STAT_BYTES(bytes)   do { monitor_info->netflow[utaf_lcore_id()].stat_bytes += bytes; } while(0)
#define DEBUG_NETFLOW_NO_FIND_SESSION_STAT   do { monitor_info->netflow[utaf_lcore_id()].no_find_session_stat++; } while(0)
#define DEBUG_NETFLOW_NO_FIND_SESSION_AGE   do { monitor_info->netflow[utaf_lcore_id()].no_find_session_age++; } while(0)
#define DEBUG_NETFLOW_NO_FIND_SESS_AGE_BYTES(bytes)   do { monitor_info->netflow[utaf_lcore_id()].no_find_sess_age_bytes += bytes; } while(0)
#define DEBUG_NETFLOW_NO_FIND_SESS_STAT_BYTES(bytes)   do { monitor_info->netflow[utaf_lcore_id()].no_find_sess_stat_bytes += bytes; } while(0)

#define DEBUG_FLOWLOCK_SEND   do { monitor_info->flowlock[utaf_lcore_id()].send++; } while(0)
#define DEBUG_FLOWLOCK_SUCCEED   do { monitor_info->flowlock[utaf_lcore_id()].succeed++; } while(0)

#define DEBUG_FLOWDEFINED_SUCCEED   do { monitor_info->flowdefined[utaf_lcore_id()].succeed++; } while(0)
#define DEBUG_FLOWDEFINED_FAILED   do { monitor_info->flowdefined[utaf_lcore_id()].failed++; } while(0)

#define DEBUG_GMS_UDP_SERVER(u)   do { monitor_info->gms_info.udp_server = (uint32_t)(u); } while(0)
#define DEBUG_GMS_DEFINE_FLOW_PORT(u)   do { monitor_info->gms_info.define_flow_port = (uint16_t)(u); } while(0)
#define DEBUG_GMS_FLOW_EVENT_PORT(u)   do { monitor_info->gms_info.flow_event_port = (uint16_t)(u); } while(0)
#define DEBUG_GMS_UDP_SERVER_SOCKET(u)   do { monitor_info->gms_info.udp_server_socket = (int)(u); } while(0)


/** ddos trie dbg***/
#define DEBUG_DDOS_TRIE_MONITOR_COUNT(cnt) do{ monitor_info->dt_dbg.monitor_cnt = cnt<DDOS_IP_RULE_SHOW_MAX_SIZE? cnt:DDOS_IP_RULE_SHOW_MAX_SIZE; }while(0)
#define DEBUG_DDOS_TRIE_WHITE_COUNT(cnt) do{ monitor_info->dt_dbg.white_cnt = cnt<DDOS_IP_RULE_SHOW_MAX_SIZE? cnt:DDOS_IP_RULE_SHOW_MAX_SIZE; }while(0)
#define DEBUG_DDOS_TRIE_MONITOR_TIP (monitor_info->dt_dbg.monitor_tip)
#define DEBUG_DDOS_TRIE_WHITE_TIP (monitor_info->dt_dbg.white_tip)

#define CONF_NEED_RELOAD() (monitor_info->reconf_flag != 0)
#define CONF_GET_RELOAD_FLAG() (monitor_info->reconf_flag)
#define CONF_SET_RELOAD_FLAG(v) do{monitor_info->reconf_flag = v;}while(0)
#define CONF_COPY_DDOS_CONF(v)do{ memcpy(&(monitor_info->ddos_cfg), v, sizeof(monitor_info->ddos_cfg)); }while(0)

#endif
