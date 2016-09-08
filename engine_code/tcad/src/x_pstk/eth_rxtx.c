#include <stdio.h>
#include "misc.h"
#include "libpag.h"
#include "statistics.h"
#include "mbuf.h"
#include "eth_rxtx.h"

PKT_INPUT pPktInput;

void packet_input_register(PKT_INPUT pFunc)
{
        pPktInput = pFunc;
}

static inline void mbuf_memset(struct m_buf *mbuf)
{
    uint32_t i, cnt;
    uint64_t *p = (uint64_t *)mbuf;

    cnt = sizeof(struct m_buf) / sizeof(uint64_t);

    for (i = 0; i < cnt; ++i) {
        p[i] = 0;
    }
}

int utaf_rx_ex(void *pkt,unsigned int len, int stream_id)
{
    int pkt_len;
    struct m_buf mbuf_local;
    struct m_buf *mbuf = NULL;

    pkt_len = ip_pkt_len((void *)pkt);
	if (pkt_len == 0)
	{
		//pag_free(stream_id, pkt);
		DEBUG_RX_STAT_VERSION_ERR;
		return 0;
	}

#ifdef UTAF_DEBUG_CYCLE
    //printf("#####utaf_rx_ex in %lu\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(utaf_rx_ex, in);
#endif

    DEBUG_ETHER_STAT_RX_ALL;

    mbuf = &mbuf_local;
    //memset(mbuf, 0, sizeof(struct m_buf));
	mbuf_memset(mbuf);

	mbuf->total_len = len;
    mbuf->pkt_addr = (void *)pkt;
    mbuf->data = (uint8_t *)pkt;
    mbuf->pkt_len = (uint16_t)pkt_len;
    mbuf->data_len = (uint16_t)pkt_len;
    mbuf->streamid = stream_id;
   
#ifdef UTAF_DEBUG_CYCLE
    //printf("#####pPktInput in %lu\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(pPktInput, in);
#endif
    pPktInput(mbuf);
#ifdef UTAF_DEBUG_CYCLE
    //printf("#####pPktInput out %lu\n\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(pPktInput, out);
#endif

#ifdef UTAF_DEBUG_CYCLE
    //printf("#####utaf_rx_ex out %lu\n", utaf_get_timer());
    UTAF_DEBUG_CYCLE_PRINT_TIME(utaf_rx_ex, out);
#endif

    return 0;
}

