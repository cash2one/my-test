#if 0
#include <utaf.h>
#include "ether.h"
#include "ip4.h"
#include "utaf_rxtx.h"
#include <statistics.h>
#include "utaf_flows.h"
#include "utaf_timer.h"

extern void utaf_netflow_process(struct m_buf *mbuf);  //wdb_calc222
#endif

#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdint.h>
#include <stdio.h>
#include "misc.h"
#include "ether.h"
#include "eth_rxtx.h"
#include "statistics.h"
#include "restore.h"
#include "libpag.h"
#include "ip4.h"

extern void check_traffic_rules(struct iphdr *ip);
extern uint32_t session_input(struct m_buf *mbuf);
extern void http_parser(struct m_buf *mbuf);

#define UTAF_NETFLOW_MODULE 1

#if 0 /* wdb */
static void ipv4_process_packet(struct m_buf *mbuf)
{
    struct iphdr  *ip  = (struct iphdr *)mbuf->network_header;
    struct udphdr *udp = (struct udphdr *)((char *)ip + (ip->ihl << 2));

    check_traffic_rules(ip);

    mbuf_move_data(mbuf, (ip->ihl << 2));
    mbuf_reset_transport_header(mbuf);

    mbuf->ip_tuple.sip[0] = ip->saddr;
    mbuf->ip_tuple.dip[0] = ip->daddr;
    mbuf->protocol        = ip->protocol;

    switch ( ip->protocol )
    {
        case IPPROTO_TCP:
        case IPPROTO_UDP:
            mbuf->sport = udp->source;
            mbuf->dport = udp->dest;
            session_input(mbuf);
            break;

        case IPPROTO_ICMP:
            mbuf->sport = 0;
            mbuf->dport = 0;
            session_input(mbuf);
            break;

        default: break;
    }

    if (ip->protocol == IPPROTO_TCP)
    {
        if ( ntohs(mbuf->sport) == 80 || ntohs(mbuf->dport) == 80 ||
             ntohs(mbuf->sport) == 443 || ntohs(mbuf->dport) == 443 )
        {
            http_parser(mbuf);
        }
    }
}
#endif /* wdb */

#ifdef UTAF_NETFLOW_MODULE
/* utaf_flows.h: */ #define PACKET_NETFLOW 0

struct flow_mac
{
	uint8_t mac1;
	uint8_t mac2;
	uint8_t mac3;
	uint8_t mac4;
	uint8_t mac5;
	uint8_t mac6;
} __attribute__((__packed__));

struct flow_src_mac
{
	uint32_t flow_id:   31;
	uint32_t zero1:      1;
	uint16_t flow_c_stat:1;
	uint16_t device_id:  5;
	uint16_t zero2:      3;
	uint16_t pkt_type:   1;
	uint16_t zero3:      6;
} __attribute__((__packed__));
#endif

void ether_input(struct m_buf *mbuf)
{
    int ehl;
	int total_len;   
	//struct eth_hdr *ethh = (struct eth_hdr *)mbuf->pkt_addr;
    struct eth_hdr *ethh = (struct eth_hdr *)pag_get_ethhdr(mbuf->streamid, mbuf->pkt_addr, &ehl);

	mbuf->mac_header = (uint8_t *)ethh;

	//printf("ethh addr is 0x%p, iphdr addr is 0x%p\n", ethh, mbuf->pkt_addr);
#ifdef UTAF_DEBUG_ETHER
	printf("dest mac is %02x:%02x:%02x:%02x:%02x:%02x\n", ethh->h_dest[0], ethh->h_dest[1],
		ethh->h_dest[2], ethh->h_dest[3], ethh->h_dest[4], ethh->h_dest[5]);
	printf("source mac is %02x:%02x:%02x:%02x:%02x:%02x\n", ethh->h_source[0], ethh->h_source[1],
		ethh->h_source[2], ethh->h_source[3], ethh->h_source[4], ethh->h_source[5]);
#endif

	//mbuf_move_data(mbuf, sizeof(struct eth_hdr));

	mbuf_reset_network_header(mbuf);
	total_len=(uint64_t)((void *)mbuf->network_header)-(uint64_t)((void *)mbuf->mac_header)+ mbuf->pkt_len;
	if(total_len > mbuf->total_len)
		mbuf->is_err_pkt = 1;

	DEBUG_ETHER_STAT_RX_OK;

#if 1 /* wdb */
#ifdef UTAF_DEBUG_CYCLE
	//printf("#####ip4_input in %lu\n", utaf_get_timer());
	UTAF_DEBUG_CYCLE_PRINT_TIME(ip4_input, in);
#endif
	ip4_input(mbuf);

#ifdef UTAF_DEBUG_CYCLE
	//printf("#####ip4_input out %lu\n\n", utaf_get_timer());
	UTAF_DEBUG_CYCLE_PRINT_TIME(ip4_input, out);
#endif
#endif /* wdb */

    check_restores(mbuf);

	return;
}

void ether_init(void)
{
	packet_input_register(ether_input);
	printf("[MBUF] size: %lu \n", sizeof(struct m_buf));
	return;
}
