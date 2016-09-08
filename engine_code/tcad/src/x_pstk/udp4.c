#include "misc.h"
#include "udp4.h"
#include "mbuf.h"
#include "frag.h"
#include "utaf_session.h"
#include "ip4.h"
#include <statistics.h>
/* #include "applib.h" //wdb_calc222 */
#include "utaf_timer.h"
#include "app.h"

static const struct net_protocol udp_protocol = {

	.handler = udp_v4_input,
};





extern void udp4_deliver(struct m_buf *mbuf);
extern void dpi_input(struct m_buf *mbuf);


#if 0
void udp_v4_input(struct m_buf *mbuf)
{
	if(debug >0)
		printf("==========>udp_v4_input\n");
	struct m_buf *bufarray[FRAG_MAX_CACHE_NUM] = {0};
	uint8_t count = 0;
	uint32_t i = 0;
	struct udp_hdr *udp = udp4_hdr(mbuf);

	int hdr_len = sizeof(struct udp_hdr);


	mbuf->sport = ntohs(udp->src_port);
	mbuf->dport = ntohs(udp->dst_port);

	if(debug >0)
		printf("sport is %d, dport is %d\n", mbuf->sport, mbuf->dport);
	
	mbuf->protocol = PROTO_UDP;
	if(debug >0)
		printf("protocol is %x\n", mbuf->protocol);

	if(!mbuf_move_data(mbuf, hdr_len))
	{
		//printf("udp_v4_input: mbuf move data fail!\n");
		DEBUG_TRANSPORT_STAT_UDP_DATALEN_ERR;
		MBUF_FREE(mbuf);
		return;
	}

	if(mbuf->ipdefrag_flag == IS_FIRST_IP_DEFRAG)
	{
		if(UTAF_RX_DROP == ip_defrag(mbuf, bufarray, &count))
		{
			for(i = 0; i < count; i++)
			{
				MBUF_FREE(bufarray[i]);
			}
			DEBUG_TRANSPORT_STAT_UDP_DEFRAG_FAIL;
			return;
		}
	}
	else
	{
		bufarray[0] = mbuf;
		count = 1;
	}

	if(debug > 0)
		printf("ip defrag count is %d\n", count);

	if(count == 0)
	{
		DEBUG_TRANSPORT_STAT_UDP_RX_OK;
		return;
	}
	else if(count == 1)
	{
		mbuf = bufarray[0];
		if(UTAF_OK != session_input(mbuf))
		{
			DEBUG_TRANSPORT_STAT_UDP_SESSION_FAIL;
			MBUF_FREE(mbuf);
			return;
		}
		else
		{
			DEBUG_TRANSPORT_STAT_UDP_RX_OK;
			udp4_deliver(mbuf);	
			return;
		}
	}
	else
	{
		if(UTAF_OK != session_input_group(bufarray, count))
		{
			DEBUG_TRANSPORT_STAT_UDP_SESSION_FAIL;
			for(i = 0; i < count; i++)
			{
				MBUF_FREE(mbuf);
			}
			return;
		}
		else
		{
			DEBUG_TRANSPORT_STAT_UDP_RX_OK;
			for(i = 0; i < count; i++)
			{
				mbuf = bufarray[i];
				udp4_deliver(mbuf);	
			}
			return;
		}
	}
}
#endif

void udp_v4_input(struct m_buf *mbuf)
{

#ifdef UTAF_DEBUG_UDP
	printf("==========>udp_v4_input\n");
#endif

	//struct m_buf *bufarray[FRAG_MAX_CACHE_NUM] = {0};
	//uint8_t count = 0;
	//uint32_t i = 0;

	if(mbuf->ipdefrag_flag == IS_IP_PKT || mbuf->ipdefrag_flag == IS_FIRST_IP_DEFRAG )
	{
		struct udp_hdr *udp = udp4_hdr(mbuf);

		int hdr_len = sizeof(struct udp_hdr);

		if(!mbuf_move_data(mbuf, hdr_len))
		{	
			DEBUG_TRANSPORT_STAT_UDP_DATALEN_ERR;
			MBUF_FREE(mbuf);
			return;
		}

		mbuf->sport = ntohs(udp->src_port);
		mbuf->dport = ntohs(udp->dst_port);

	#ifdef UTAF_DEBUG_UDP
		printf("sport is %d, dport is %d\n", mbuf->sport, mbuf->dport);		
	#endif
	}
	else
	{
		mbuf->sport = 0;
		mbuf->dport = 0;
	}
		
	mbuf->protocol = PROTO_UDP;

	#ifdef UTAF_DEBUG_UDP
		printf("protocol is %x\n", mbuf->protocol);
	#endif

#if 0
	if(UTAF_OK != session_process(mbuf))
	{
		DEBUG_TRANSPORT_STAT_UDP_SESSION_FAIL;
		MBUF_FREE(mbuf);
	}
	else
	{
		DEBUG_TRANSPORT_STAT_UDP_RX_OK;
		udp4_deliver(mbuf);	
	}
#else
	if(UTAF_OK != session_input(mbuf))
	{
		DEBUG_TRANSPORT_STAT_UDP_SESSION_FAIL;
		MBUF_FREE(mbuf);
	}
	else
	{
		DEBUG_TRANSPORT_STAT_UDP_RX_OK;
#ifdef UTAF_DDOS
		session_item_t *si = mbuf->psession_item;
		if (likely(g_ddos_conf.sw && si != NULL && 
					UTAF_DO_DDOS == si->ddos_flag && mbuf->pkt_dir < flow_dir))
		{
			si->ddos.stat.udp.udp[mbuf->pkt_dir].pkts++;
			si->ddos.stat.udp.udp[mbuf->pkt_dir].bytes += mbuf->total_len;
		}
#endif
#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(udp4_deliver, in);
#endif
		udp4_deliver(mbuf);
#ifdef UTAF_DEBUG_CYCLE
        UTAF_DEBUG_CYCLE_PRINT_TIME(udp4_deliver, out);
#endif
	}
#endif

	return;
}



void udp4_deliver(struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_UDP
	printf("============>udp4_deliver\n");
#endif

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(dpi_input, in);
#endif

#ifdef UTAF_MODULE_DPI
    dpi_input(mbuf);
#endif

#ifdef UTAF_DEBUG_CYCLE
    UTAF_DEBUG_CYCLE_PRINT_TIME(dpi_input, out);
#endif

	return;
}




uint32_t udp_init(void)
{
	if(UTAF_OK != ip4_add_protocol(&udp_protocol, PROTO_UDP))
	{
		printf("add protocol fail %d\n", PROTO_UDP);
		return UTAF_FAIL;
	}

	return UTAF_OK;
}






