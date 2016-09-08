#include "misc.h"
#include "udp4.h"
#include "mbuf.h"
#include "utaf_session.h"
#include "ip4.h"
#include <statistics.h>

static void icmp_v4_input(struct m_buf *mbuf);


static const struct net_protocol icmp_protocol = {

	.handler =  icmp_v4_input,
};

void icmp_v4_input(struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_UDP
	printf("==========>icmp_v4_input\n");
#endif
	mbuf->sport = 0;
	mbuf->dport = 0;		
	mbuf->protocol = PROTO_ICMP;
	if(UTAF_OK != session_input(mbuf))
	{
		DEBUG_NETWORK_STAT_ICMP_FAIL;
	}
#ifdef UTAF_DDOS
	else 
	{
		session_item_t *si = mbuf->psession_item;
		if (likely(g_ddos_conf.sw && si!= NULL && 
					UTAF_DO_DDOS == si->ddos_flag && mbuf->pkt_dir < flow_dir))
		{
			si->ddos.stat.icmp.icmp[mbuf->pkt_dir].pkts++;
			si->ddos.stat.icmp.icmp[mbuf->pkt_dir].bytes += mbuf->total_len;
		}
	}
#endif
    DEBUG_NETWORK_STAT_ICMP_RX_OK;
	MBUF_FREE(mbuf);
}


uint32_t icmp_init(void)
{
	if(UTAF_OK != ip4_add_protocol(&icmp_protocol, PROTO_ICMP))
	{
		printf("add protocol fail %d\n", PROTO_ICMP);
		return UTAF_FAIL;
	}

	return UTAF_OK;
}

