/******************************************************************************
description : header file for pcap
******************************************************************************/

#ifndef _LIBPAG_H
#define _LIBPAG_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>


/*maximum size of one pkt to send*/
#define MAX_SEND_PKT_BUF 4000

#define SEND_PKT_TYPE_IP 0
#define SEND_PKT_TYPE_IP1 1
#define SEND_PKT_TYPE_ETH 2

/*
 * open all devices and prepare for pag.
 * if succeed, return 1; if failed, return -1
 * NOTE: configuration file: pag.conf, sendlog.conf
 */
int pag_open(void);

/*
 *close all devices used for zcopy 
 */
void pag_close(void);

/*
 * get one packet. may block.
 * @sid: stream_id
 */
void *pag_get(int sid);
void *pag_get_tcad(int sid, int *len);
void pag_free(int sid, void *pkt);

/*
 * get the time stamp of the packet
 * if failed, return 0
 */
inline __u64 pag_time(const void *pkt);

static inline int ip_pkt_version(void *iph)
{
	return ((struct iphdr*)iph)->version;
}

/* XXX: ipv6 next header? */
static inline int ip_pkt_len(void *iph)
{
	if (((struct iphdr*)iph)->version == 4) {
		return ntohs(((struct iphdr*)iph)->tot_len);
	} else if (((struct iphdr*)iph)->version == 6){
		return (sizeof(struct ip6_hdr) + ((struct ip6_hdr*)iph)->ip6_plen);
	}
	return 0;
}

/*
 * get pkt buffer for sending
 *
 * @threadnum: ID of the thread to send this packet.
 */
void *pag_getsendbuf(int threadnum);

/*
 * send one packet.
 * @pkttype:
 * @threadnum: ID of the thread to send this packet.
 */
int pag_send(/*void *pbuf, */int pkttype, int threadnum, int datalen);

/*
 * release the packet buffer.
 */
void pag_freesendbuf(int threadnum, void *pbuf);

/*well, empty implementation, params may not be right*/
int pag_refreshstaticrule(int num);

/*
 * get ethernet header length of specified ip packet
 * @param iph, ip header got by pag_get().
 * @param hlen, length of eth header.
 * @return : ethernet header.
 */
void *pag_get_ethhdr(int sid, void *iph, int *hlen);

#ifdef  __cplusplus
}
#endif

#endif



