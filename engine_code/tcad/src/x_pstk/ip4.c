#include "misc.h"
#include "ether.h"
#include "ip4.h"
#include "frag.h"
#include "udp4.h"
#include "utaf_session.h"
#include <byteorder.h>
#include <statistics.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utaf_timer.h"

const struct net_protocol *inet_protos[PROTO_MAX];


/*
static struct packet_type ip_packet_type =
{
	.type = ETH_P_IP,
	.func = ip4_input,
};
*/

extern uint32_t tcp_init(void);
extern uint32_t icmp_init(void);
extern void check_traffic_rules(struct m_buf *mbuf, struct iphdr *ip);

/*
 * 管理线程初始化
 */
int ip4_init(void)
{
/*
	if(UTAF_OK != ipfrag_init())
	{
		return UTAF_FAIL;
	}
*/
	if(UTAF_OK != tcp_init())
	{
		return UTAF_FAIL;
	}

	if(UTAF_OK != udp_init())
	{
		return UTAF_FAIL;
	}
	//icmp_init();
	if(UTAF_OK != icmp_init())
	{
		return UTAF_FAIL;
	}

	//ether_add_pack(&ip_packet_type);
	return UTAF_OK;
}



static inline int is_valid_ipv4_pkt(struct m_buf *mbuf, struct ipv4_hdr *iph)
{
	/* From http://www.rfc-editor.org/rfc/rfc1812.txt section 5.2.2 */
	/*
	 * 1. The packet length reported by the Link Layer must be large
	 * enough to hold the minimum length legal IP datagram (20 bytes).
	 */
	if(mbuf->data_len < sizeof(struct ipv4_hdr))
 	{
#ifdef UTAF_DEBUG_IP
 		printf("datalen is less than ipv4 hdr\n");
#endif
 		return 0;
 	}

	/* 2. The IP checksum must be correct. */
	/* this is checked in H/W */

	/*
	 * 3. The IP version number must be 4. If the version number is not 4
	 * then the packet may be another version of IP, such as IPng or
	 * ST-II.
	 */

	if (((iph->version_ihl) >> 4) != 4)
	{
#ifdef UTAF_DEBUG_IP
		printf("ip version is err\n");
#endif
		return 0;
	}

	/*
	 * 4. The IP header length field must be large enough to hold the
	 * minimum length legal IP datagram (20 bytes = 5 words).
	 */
	if ((iph->version_ihl & 0xf) < 5)
	{
#ifdef UTAF_DEBUG_IP
		printf("ip header length is fail\n");
#endif

		return 0;
	}

	/*
	 * 5. The IP total length field must be large enough to hold the IP
	 * datagram header, whose length is specified in the IP header length
	 * field.
	 */
	if (ntohs(iph->total_length) < sizeof(struct ipv4_hdr))
	{
#ifdef UTAF_DEBUG_IP
		printf("total length is less than ipv4 hdr\n");
#endif
		return 0;
	}

	return 1;
}


int ip4_deliver(struct m_buf *mbuf, uint8_t protocol)
{
	int hash;
	const struct net_protocol *ipprot;
	
	hash = protocol & (PROTO_MAX - 1);
	ipprot = inet_protos[hash];
	if (NULL == ipprot) {
		struct ipv4_hdr *iph; /* wdb */

		iph = ip_hdr(mbuf);
		mbuf->sport = 0;
		mbuf->dport = 0;
		mbuf->protocol = iph->next_proto_id;
        
		if (UTAF_OK == session_input(mbuf)) {
            DEBUG_TRANSPORT_STAT_OTHER_RX_OK;
			return UTAF_RX_SUCCESS;
		}

        DEBUG_TRANSPORT_STAT_OTHER_SESSION_FAIL;
		return UTAF_RX_DROP;
	}

	(void)ipprot->handler(mbuf);
	
	return UTAF_RX_SUCCESS;
}

void ip4_input(struct m_buf *mbuf)
{
	struct ipv4_hdr *iph;
	uint8_t ip_protocol;
	uint16_t flag_offset, ip_flag, ip_ofs;
	int hdr_len;
	//int i;
	
	iph = ip_hdr(mbuf);
#ifdef UTAF_DEBUG_IP
	printf("===========>ip4_input!\n");
#endif
		
	if(!is_valid_ipv4_pkt(mbuf, iph))
	{
		DEBUG_NETWORK_STAT_INVALID;
		MBUF_FREE(mbuf);
		return;
	}

    check_traffic_rules(mbuf, iph);
	if(mbuf->is_err_pkt)
	{
		DEBUG_NETWORK_STAT_INVALID;
		MBUF_FREE(mbuf);
		return;
	}
	hdr_len = (iph->version_ihl & 0xf) << 2;

#ifdef UTAF_DEBUG_IP
	printf("ip head len is %d, total len is %d\n", hdr_len, ntohs(iph->total_length));
#endif	

	if(!mbuf_move_data(mbuf, hdr_len))
	{
		DEBUG_NETWORK_STAT_DATALEN_ERR;
		MBUF_FREE(mbuf);
		return;
	}

	/*
	 * TODO: network layer
	*/
	mbuf->ip_tuple.sip[0] = ntohl(iph->src_addr);
	mbuf->ip_tuple.dip[0] = ntohl(iph->dst_addr);
	
#ifdef UTAF_DEBUG_IP
	struct in_addr addr;
	addr.s_addr = iph->src_addr;
	printf("src_ip = %s\n", inet_ntoa(addr));
	addr.s_addr = iph->dst_addr;
	printf("dst_ip = %s\n", inet_ntoa(addr));
#endif

	mbuf->ip_version = IPV4_VERSION;

#ifdef UTAF_DEBUG_IP
	printf("ip version = %d\n", iph->version_ihl>>4);
#endif

	mbuf->ipdefrag_id = iph->packet_id;

#ifdef UTAF_DEBUG_IP
	printf("ip frag id is %d\n", mbuf->ipdefrag_id);
#endif
	
	flag_offset = ntohs(iph->fragment_offset);
	ip_flag = (uint16_t)(flag_offset & IPV4_HDR_MF_FLAG);
	ip_ofs = (uint16_t)(flag_offset & IPV4_HDR_OFFSET_MASK);

	if(ip_flag != 0 || ip_ofs != 0)
	{
		if(ip_ofs != 0)
		{
			mbuf->ipdefrag_flag = IS_OTHER_IP_DEFRAG;
			DEBUG_NETWORK_STAT_IP_PKT;
		}
		else
		{
			mbuf->ipdefrag_flag = IS_FIRST_IP_DEFRAG;
			DEBUG_NETWORK_STAT_FIRST_IP_FRAG;
		}
	}
	else
	{
		mbuf->ipdefrag_flag = IS_IP_PKT;
		DEBUG_NETWORK_STAT_IP_PKT;
	}

#ifdef UTAF_DEBUG_IP
	printf("ipdefrag_flag is %d\n", mbuf->ipdefrag_flag);
#endif

	ip_protocol = iph->next_proto_id;

#ifdef UTAF_DEBUG_IP
	printf("ip protocol is %d\n", ip_protocol);
#endif

	if(mbuf->ipdefrag_flag == IS_IP_PKT || mbuf->ipdefrag_flag == IS_FIRST_IP_DEFRAG )
	{
		mbuf_reset_transport_header(mbuf);		
	}

#if 0
	extern uint64_t g_cap_flag;
	if (0 != g_cap_flag) {
		(void)traffic_cap_match(mbuf);
	}
#endif

    #ifdef UTAF_DEBUG_CYCLE
    //printf("#####ip4_deliver in %lu\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(ip4_deliver, in);
    #endif
	if(UTAF_RX_SUCCESS != ip4_deliver(mbuf, ip_protocol))/*上送*/
	{
		DEBUG_NETWORK_DELIVER_FAIL;
		MBUF_FREE(mbuf);
		return;
	}
    #ifdef UTAF_DEBUG_CYCLE
    //printf("#####ip4_deliver out %lu\n\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(ip4_deliver, out);
    #endif

	DEBUG_NETWORK_STAT_RX_OK;
	
	return;
#if 0	
	else if(mbuf->ipdefrag_flag == IS_OTHER_IP_DEFRAG)/*后续片报文*/
	{
		struct m_buf *bufarray[FRAG_MAX_CACHE_NUM] = { 0 };
		uint8_t count = 0;
		if(debug > 0)
			printf("other ip frag packet\n");
		
		if(UTAF_RX_SUCCESS == ip_defrag(mbuf, bufarray, &count))
		{
			for(i = 0; i < count; i++)
			{
				if(UTAF_OK != session_input(bufarray[i]))
				{
					
					MBUF_FREE(bufarray[i]);
				}
				else
				{
					/*TODO: frag packet go on*/
					DEBUG_NETWORK_STAT_RX_OK;
					MBUF_FREE(bufarray[i]);
				}
			}
		}
		else
		{
			DEBUG_NETWORK_DEFRAG_FAIL;
			for(i = 0; i < count; i++)
			{
				MBUF_FREE(bufarray[i]);
			}
		}	
	}
#endif
}





uint32_t ip4_add_protocol(const struct net_protocol *prot, unsigned char protocol)
{
	int hash, ret;

	hash = protocol & (PROTO_MAX - 1);

	if (inet_protos[hash]) 
	{
		ret = UTAF_FAIL;
	} 
	else 
	{
		inet_protos[hash] = prot;
		ret = UTAF_OK;
	}

	return ret;
}

int ip4_del_protocol(const struct net_protocol *prot, unsigned char protocol)
{
	int hash,ret;

	hash = protocol & (PROTO_MAX - 1);

	if (inet_protos[hash] == prot) 
	{
		inet_protos[hash] = NULL;
		ret = 0;
	} 
	else 
	{
		ret = -1;
	}

	return ret;
}









