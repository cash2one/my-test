#include "misc.h"
#include "frag.h"
#include "ip4.h"
#include "tcp4.h"
#include "utaf_session.h"
#include <statistics.h>
#include "app.h"
#include "app_id.h"
#include "utaf_timer.h"


extern void dpi_input(struct m_buf *mbuf);

static const struct net_protocol tcp_protocol = {

	.handler = tcp_v4_input,
};

static int tcp_state_retconn(session_item_t *si, struct m_buf *mbuf)
{
	if (si == NULL)
		return -1;

		/* SYN/ACK */
	if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
		/* set the state */
		si->tcp_state = TCP_SYN_RECV;
	} else if (mbuf->tcp_f & TH_SYN) {
		/* set the state */
		si->tcp_state = TCP_SYN_SENT;
	}

	return 0;

}
static int tcp_state_closing(session_item_t *si, struct m_buf *mbuf)
{
	if (si == NULL)
		return -1;

	if (mbuf->tcp_f & TH_RST) {
		si->tcp_state = TCP_CLOSED;
	} else if (mbuf->tcp_f & TH_FIN) {
		si->tcp_closing |= TCP_CLOSE_WAIT2;
	} else if ((mbuf->tcp_f &  TH_ACK) == TH_ACK) {
		if (si->tcp_closing & TCP_CLOSE_WAIT3)
			si->tcp_closing |= TCP_CLOSE_WAIT4;
		else
			si->tcp_closing |= TCP_CLOSE_WAIT3;
		/* SYN/ACK */
	} else if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
		/* set the state */
		si->tcp_state = TCP_SYN_RECV;
	} else if (mbuf->tcp_f & TH_SYN) {
		/* set the state */
		si->tcp_state = TCP_SYN_SENT;
	}

	return 0;

}
#if 0
static int tcp_state_closing(session_item_t *si, struct m_buf *mbuf)
{
	if (si == NULL)
		return -1;

	if (mbuf->tcp_f & TH_RST) {
		si->tcp_state = TCP_CLOSED;
	} else if (mbuf->tcp_f & TH_FIN) {
		si->tcp_closing |= TCP_CLOSE_WAIT2;
		printf("tcp_closing:0x%0x\n", si->tcp_closing);
	} else if ((mbuf->tcp_f &  (TH_FIN|TH_ACK)) == (TH_FIN|TH_ACK)) {
		if (si->tcp_closing & TCP_CLOSE_WAIT3)
			si->tcp_closing |= TCP_CLOSE_WAIT4;
		else
			si->tcp_closing |= TCP_CLOSE_WAIT3;
		printf("tcp_closing:0x%0x\n", si->tcp_closing);
		/* SYN/ACK */
	} else if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
	} else if (mbuf->tcp_f & TH_SYN) {
	} else if (mbuf->tcp_f & TH_ACK) {
	}

	return 0;

}
#endif 

static int tcp_state_established(session_item_t *si, struct m_buf *mbuf)
{
	if (si == NULL)
		return -1;

	if (mbuf->tcp_f & TH_RST) {
		si->tcp_state = TCP_CLOSED;
	} else if (mbuf->tcp_f & TH_FIN) {
		si->tcp_state = TCP_CLOSING;
		si->tcp_closing = TCP_CLOSE_WAIT1;
		/* SYN/ACK */
	} else if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
	} else if (mbuf->tcp_f & TH_SYN) {
	} else if (mbuf->tcp_f & TH_ACK) {
	}

	return 0;
}

static int tcp_state_syn_recv(session_item_t *si, struct m_buf *mbuf)
{
	if (si == NULL)
		return -1;

	if (mbuf->tcp_f & TH_RST) {
		si->tcp_state = TCP_CLOSED;
	} else if (mbuf->tcp_f & TH_FIN) {
		si->tcp_state = TCP_CLOSING;
		si->tcp_closing = TCP_CLOSE_WAIT1;
		/* SYN/ACK */
	} else if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
	} else if (mbuf->tcp_f & TH_SYN) {
	} else if (mbuf->tcp_f & TH_ACK) {
		si->tcp_state = TCP_ESTABLISHED;
		si->tcp_conn_dir = mbuf->pkt_dir;
		si->ddos_ssn_new = 0;
		si->ddos_ssn_close = 0;
	}

	return 0;
}

static int tcp_state_syn_sent(session_item_t *si, struct m_buf *mbuf)
{
	if (si == NULL)
		return -1;

	/* RST */
	if (mbuf->tcp_f & TH_RST) {
		si->tcp_state = TCP_CLOSED;
		/* FIN */
	} else if (mbuf->tcp_f & TH_FIN) {
		/* SYN/ACK */
	} else if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
		/* update state */
		si->tcp_state = TCP_SYN_RECV;
	} else if (mbuf->tcp_f & TH_SYN) {
	} else if (mbuf->tcp_f & TH_ACK) {
		si->tcp_state = TCP_ESTABLISHED;
		si->tcp_conn_dir = mbuf->pkt_dir;
		si->ddos_ssn_new = 0;
		si->ddos_ssn_close = 0;
	} 

	return 0;
}

static void tcp_set_state(session_item_t *si, struct m_buf *mbuf)
{
	switch (si->tcp_state) {
		case TCP_SYN_SENT:
			tcp_state_syn_sent(si, mbuf);
			break;
		case TCP_SYN_RECV:
			tcp_state_syn_recv(si, mbuf);
			break;
		case TCP_ESTABLISHED:
			tcp_state_established(si, mbuf);
			break;
		case TCP_FIN_WAIT1:
		case TCP_FIN_WAIT2:
		case TCP_CLOSING:
		case TCP_CLOSE_WAIT:
		case TCP_LAST_ACK:
		case TCP_TIME_WAIT:
			tcp_state_closing(si, mbuf);
			break;
		case TCP_CLOSED:
			tcp_state_retconn(si, mbuf);
			break;
		default:
			break;
	}

	return ;
}


static int tcp_set_state_none(session_item_t *si, struct m_buf *mbuf)
{
	if (mbuf->tcp_f & TH_RST) {
#ifdef UTAF_DEBUG_SESSION
		printf("RST packet received, no session setup");
#endif
		return -1;

	} else if (mbuf->tcp_f & TH_FIN) {
#ifdef UTAF_DEBUG_SESSION
		printf("FIN packet received, no session setup");
#endif
		return -1;

		/* SYN/ACK */
	} else if ((mbuf->tcp_f & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
		/* set the state */
		si->tcp_state = TCP_SYN_RECV;
#ifdef UTAF_DEBUG_SESSION
		printf("midstream picked state is now "
				"TCP_SYN_RECV");
#endif
	} else if (mbuf->tcp_f & TH_SYN) {
		/* set the state */
		si->tcp_state = TCP_SYN_SENT;
#ifdef UTAF_DEBUG_SESSION
		printf("state is now TCP_SYN_SENT");
#endif
	} else if (mbuf->tcp_f & TH_ACK) {
		/* set the state */
		si->tcp_state = TCP_ESTABLISHED;
		si->tcp_conn_dir = mbuf->pkt_dir;
	} else {
#ifdef UTAF_DEBUG_SESSION
		printf("default case");
#endif
	}

	return 0;
}

void tcp_seq_cal(struct tcp_seq_info *tseq, struct tcp_segment_info *tsegi)
{
	uint32_t i;
	uint32_t j;
	int r = 0, o = 0;
	int odd = 0;
	for(i = 1; i < tseq->index_w; i++)
	{	
		for(j = 0; j < i; j++)
		{
			if(tseq->seq_window[j] == tseq->seq_window[i])
			{
				r++;
				break;
			}
			else if(tseq->seq_window[j] > tseq->seq_window[i])
			{
				odd = 1;
			}
		}
		if(j == i)
		{
			if(1 == odd)
			{
				o++;
				odd = 0;
			}
		}
	}

	tsegi->retrans_pkt += r;
	tsegi->ood += o;
	
	return;
}


void tcp_seq_process(session_item_t *si, struct tcp_seq_info *tseq, uint32_t tcp_seq, struct tcp_segment_info *tsegi)
{

	if(si->protocol != APP_ID_UNKNOW && si->protocol != APP_HTTP)
	{
		return;
	}
	
	//int i;
	uint32_t index = tseq->index_w;
	tseq->seq_window[index] = tcp_seq;

#ifdef UTAF_DEBUG_TCP
	printf("[TCP]seq is %ul w_index is %d\n", tseq->seq_window[index], index);
#endif
	tseq->index_w++;

	if(TCP_SEQ_WINDOW_SIZE == tseq->index_w)
	{
		if(APP_HTTP == si->app_id)
		{
			tcp_seq_cal(tseq, tsegi);
		}
		
		tseq->seq_window[0] = tseq->seq_window[TCP_SEQ_WINDOW_SIZE - 1];
		tseq->index_w = 0;
	}
	
	return;
}



void tcp4_deliver(struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_TCP
	printf("[TCP]============>tcp4_deliver\n");
#endif

#if 1
	if(mbuf->tcp_f & TH_SYN)
	{
#ifdef UTAF_DEBUG_TCP
	    printf("[TCP]============>tcp4_deliver %d\n", __LINE__);
#endif
		MBUF_FREE(mbuf);
	}
	else
	{
        if (0 == mbuf->data_len) {
#ifdef UTAF_DEBUG_TCP
	        printf("[TCP]============>tcp4_deliver %d\n", __LINE__);
#endif
            MBUF_FREE(mbuf);
            return;
        }
        
        if (NULL == mbuf->psession_item) {
#ifdef UTAF_DEBUG_TCP
	        printf("[TCP]============>tcp4_deliver %d\n", __LINE__);
#endif
            MBUF_FREE(mbuf);
            return;
        }
#if 0
        if (BGP_TCP_PORT == mbuf->sport || BGP_TCP_PORT == mbuf->dport) {
            bgp_proc(mbuf);
        }
#endif        

#ifdef UTAF_DEBUG_CYCLE
        //printf("#####dpi_input in %lu\n", utaf_get_timer());
        UTAF_DEBUG_CYCLE_PRINT_TIME(dpi_input, in);
#endif

#ifdef UTAF_MODULE_DPI
        dpi_input(mbuf);
#endif

#ifdef UTAF_DEBUG_CYCLE
        //printf("#####dpi_input out %lu\n\n", utaf_get_timer());
        UTAF_DEBUG_CYCLE_PRINT_TIME(dpi_input, out);
#endif
	}
#else
    http_parser(mbuf);
	MBUF_FREE(mbuf);
#endif
	return;
}


void tcp_v4_input(struct m_buf *mbuf)
{
	//int i;
	//struct m_buf *bufarray[FRAG_MAX_CACHE_NUM] = {0};
	//uint8_t count = 0;
/*	int res = -1; //wdb_calc222 */

	if(mbuf->ipdefrag_flag == IS_IP_PKT || mbuf->ipdefrag_flag == IS_FIRST_IP_DEFRAG )
	{
		struct tcp_hdr *tcp = tcp4_hdr(mbuf);
		
	#ifdef UTAF_DEBUG_TCP
		printf("[TCP]==========>tcp_v4_input\n");
	#endif
		//printf("tcp header is %p\n",tcp);

		//int hdr_len = sizeof(struct tcp_hdr);
        int hdr_len = (((struct tcp_header *)tcp)->thl << 2);

		if(!mbuf_move_data(mbuf, hdr_len))
		{
			//printf("tcp_v4_input: mbuf move data fail!\n");
			DEBUG_TRANSPORT_STAT_TCP_DATALEN_ERR;
			MBUF_FREE(mbuf);
			return;
		}

		mbuf->sport = ntohs(tcp->src_port);
		mbuf->dport = ntohs(tcp->dst_port);

		mbuf->tcp_f = tcp->tcp_flags;

		mbuf->tcp_seq = ntohl(tcp->sent_seq);
		
	#ifdef UTAF_DEBUG_TCP
		printf("[TCP]sport is %d, dport is %d, seq is %u\n", mbuf->sport, mbuf->dport, mbuf->tcp_seq);
	#endif
	
	}
	else
	{
		mbuf->sport = 0;
		mbuf->dport = 0;
	}
	
	mbuf->protocol = PROTO_TCP;

#ifdef UTAF_DEBUG_TCP
	printf("[TCP]protocol is %x\n", mbuf->protocol);
#endif

	if(UTAF_OK != session_input(mbuf))
	{
		DEBUG_TRANSPORT_STAT_TCP_SESSION_FAIL;
		MBUF_FREE(mbuf);
	}
	else
	{
		DEBUG_TRANSPORT_STAT_TCP_RX_OK;

#ifdef UTAF_DDOS
		session_item_t *si = mbuf->psession_item;
		extern uint32_t ddos_statistic_tcp_connect(session_item_t *si, uint32_t flag);

		if (si && g_ddos_conf.sw && UTAF_DO_DDOS == si->ddos_flag) 
		{
			if ( TCP_NONE == si->tcp_state)
			{
				tcp_set_state_none(si, mbuf);
			}
			else
			{
				tcp_set_state(si, mbuf);
			}
			if (0 == si->ddos_ssn_new && TCP_ESTABLISHED == si->tcp_state)
			{
				/* NEW TCP SESSION */
				ddos_statistic_tcp_connect(si, DDOS_CONNECT_NEW);
				ddos_debug(DDOS_MID_FLOW,"new session sip:%"IPV4FMT" dip:%"IPV4FMT" sport:%d dport:%d" 
						" protocol:%d\n", IPV4P(&si->ip_tuple.sip[0]), 
						IPV4P(&si->ip_tuple.dip[0]), si->sport, si->dport,
						si->protocol);
				si->ddos_ssn_new = 1;
			}
			else if((TCP_CLOSING == si->tcp_state) &&
					((si->tcp_closing & (TCP_CLOSE_WAIT1|TCP_CLOSE_WAIT2|
						TCP_CLOSE_WAIT3|TCP_CLOSE_WAIT4)) == (TCP_CLOSE_WAIT1|
							TCP_CLOSE_WAIT2|TCP_CLOSE_WAIT3|TCP_CLOSE_WAIT4)) &&
					0 == si->ddos_ssn_close)
			{
				/* TCP SESSION CLOSE */	
				ddos_statistic_tcp_connect(si, DDOS_CONNECT_CLOSE);
				ddos_debug(DDOS_MID_FLOW,"close session sip:%"IPV4FMT" dip:%"IPV4FMT" sport:%d dport:%d" 
						" protocol:%d\n", IPV4P(&si->ip_tuple.sip[0]), 
						IPV4P(&si->ip_tuple.dip[0]), si->sport, si->dport,
						si->protocol);
				si->tcp_state = TCP_CLOSED;
				si->ddos_ssn_close = 1;
			}

			if (likely(mbuf->pkt_dir < flow_dir))
			{
				si->ddos.stat.tcp.tcp[mbuf->pkt_dir].pkts++;
				si->ddos.stat.tcp.tcp[mbuf->pkt_dir].bytes += mbuf->total_len;

				/*
				 * 只统计流入检测方向的syn的tcp报文
				 */
				if (mbuf->pkt_dir == dir_in && TH_SYN == (mbuf->tcp_f & TH_SYN) &&
						!(mbuf->tcp_f & (~TH_SYN)))
				{
					si->ddos.stat.tcp.syn[mbuf->pkt_dir].pkts++;
					si->ddos.stat.tcp.syn[mbuf->pkt_dir].bytes += mbuf->total_len;
				} 
				/* 统计所有ack包,ack flood 是大量有ack标志标志位的tcp报文，
				 * 服务器由于忙于发送回应包而造成拒绝服务
				 * http://blog.sina.com.cn/s/blog_616d95180100ftzi.html
				 * http://host.zzidc.com/zhujiwenti/510.html
				 */
				else if (mbuf->pkt_dir == dir_in && TH_ACK == (mbuf->tcp_f &
							TH_ACK))
				{
					si->ddos.stat.tcp.ack[mbuf->pkt_dir].pkts++;
					si->ddos.stat.tcp.ack[mbuf->pkt_dir].bytes += mbuf->total_len;
				}
			}
		}
#endif

        #ifdef UTAF_DEBUG_CYCLE
        //printf("#####tcp4_deliver in %lu\n", utaf_get_timer());
        UTAF_DEBUG_CYCLE_PRINT_TIME(tcp4_deliver, in);
        #endif
		tcp4_deliver(mbuf);	
        #ifdef UTAF_DEBUG_CYCLE
        //printf("#####tcp4_deliver out %lu\n\n", utaf_get_timer());
        UTAF_DEBUG_CYCLE_PRINT_TIME(tcp4_deliver, out);
        #endif
	}


	return;
}


uint32_t tcp_init(void)
{
	if(UTAF_OK != ip4_add_protocol(&tcp_protocol, PROTO_TCP))
	{
		printf("add protocol fail %d\n", PROTO_TCP);
		return UTAF_FAIL;
	}

	return UTAF_OK;
}




