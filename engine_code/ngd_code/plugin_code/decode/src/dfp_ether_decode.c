#include <stdio.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "dfp_ether_decode.h"

//#include "thread.h"
//#include "trace_api.h"
//#include "time_consume.h"
//#include "perform.h"
//PER_DECLARE(ether)

int ether_decode_init(void *private_info, char *args)
{
	return 0;
}

void ether_decode_clean(void *private_info)
{
    return;
}
/*
 *	MPLS header format
 *i 
 *	0                     20 22 23        31  
 *	++++++++++++++++++++++++++++++++++++++++
 *	+      label         | Cos |S|  TTL    |             
 *	++++++++++++++++++++++++++++++++++++++++
 *
 * 
 */ 
static inline void do_MPLS_decode(pkt_info_t *ppkt)
{
    u_int8_t *iplayer_h = NULL;
    u_int8_t *MPLS_hdata = NULL;
    int hd_cnt = 0;

    MPLS_hdata = ppkt->eth_payload;
    do {
	hd_cnt ++;
	if(*(MPLS_hdata + 2) & 0x1)
	    break;
	MPLS_hdata += 4;
    } while(1);

    hd_cnt <<= 2; /* multiple 4 */

    ppkt->eth_payload += hd_cnt;
    ppkt->eth_payload_len -= hd_cnt;
    //ppkt->eth_payloadlen = packet->eth_payload_len;

    iplayer_h = ppkt->eth_payload;
    switch((*iplayer_h >> 4) & 0xf) {
	case 4:
	    ppkt->hook_id = IP_HOOK;
	    break;
	case 6:
	    /* IPv6, not support in mt product */
	    break;
    }
    return;
}

/********************************************************************
 > function:
	1.捕包功能的插件处理函数，作为so库函数对外提供
 > param:
	1.ppkt:传入的数据报文信息
 > return:
	1.CONTITUNE_GIVE_ME:继续传送报文给本插件
	2.NO_CONTINUE_GIVE_ME:该会话的报文后续不用给本插件
 > modify:
	1.create by wlz
 ************************************************************************/
int plugin_proc(pkt_info_t *ppkt)
{
    int ret = CONTINUE_GIVE_ME;
    u_int8_t next_hook_id = IP_HOOK;
    u_int16_t proto_tmp;
    struct _VlanHdr *vlanhdr;	/* for VLAN */
    struct ethhdr *eth_hdr = (struct ethhdr *)ppkt->raw_pkt;

    //printf("ether decode %s\n",__FUNCTION__);
    /* Check */
    if (ppkt->raw_pkt_len < sizeof(struct ethhdr)) { 
	goto RET;
    }

    //ppkt->eth_length = ppkt->raw_pkt_len;
 //  ppkt->eth_smac = eth_hdr->h_source;
   // ppkt->eth_dmac = eth_hdr->h_dest;
   // ppkt->eth_smaclen = ETH_ALEN;
   //ppkt->eth_dmaclen = ETH_ALEN;
    proto_tmp = ntohs(eth_hdr->h_proto);
    switch (proto_tmp) {
	case 0x8100:
	    ppkt->eth_payload = (u_int8_t *)(eth_hdr + 1) + sizeof(struct _VlanHdr); 
	    ppkt->eth_payload_len = ppkt->raw_pkt_len - sizeof(struct ethhdr) 
		- sizeof(struct _VlanHdr);
	    //ppkt->eth_payloadlen = packet->eth_payload_len;
	    vlanhdr = (struct _VlanHdr *)(eth_hdr + 1);
	    ppkt->eth_proto = ntohs(vlanhdr->vh_proto);
	    break;
	default:
	    ppkt->eth_payload = (u_int8_t *)(eth_hdr + 1);
	    ppkt->eth_payload_len = ppkt->raw_pkt_len - sizeof(struct ethhdr);
	    //ppkt->eth_payloadlen = packet->eth_payload_len;
	    ppkt->eth_proto = ntohs(eth_hdr->h_proto);
	    break;
    }
    switch (ppkt->eth_proto) {
	case 0x0800: /* ip */
	    ppkt->hook_id = IP_HOOK;
	    break;
	case 0x8847:	/* MPLS unicast */
	case 0x8848:	/* MPLS multicast */
	    do_MPLS_decode(ppkt);
	    break;
	case 0x0806: /* arp */
	    ppkt->hook_id = ARP_HOOK;
	    break;
	case 0x8035: /* rarp */
	    break;
    }

    //ppkt->eth_payload_s = packet->eth_payload; 
    //ppkt->eth_payload_slen  = packet->eth_payloadlen;

    //ppkt->eth_payload_h = packet->eth_payload; 
    //ppkt->eth_payload_hlen  = packet->eth_payloadlen;
    //ppkt->proto_path[packet->proto_path_len++] = PROTO_ID_ETHER;

RET:
    return ret;
}


void *plugin_init(int ptheadnum)
{
    return NULL;
}

int plugin_clean(pkt_info_t *ppkt)
{
    return 0;
}

/*跟tcp相关的hook，tcp会话结束的清除函数指针*/
int plugin_tcp_clean(pkt_info_t *ppkt)
{
    return 0;
}
