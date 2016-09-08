#include "decode/dfp/dfp_udp_decode.h"
#include "decode/decode.h"
#include "decode/port_table.h"
#include "event.h"
//#include "decode/tcp_stream.h"
#include "misc.h"
#include "vfpm/vfpm_conf.h"
#include "preproc_common.h"
#include "thread.h"

#include "thread.h"
#include "time_consume.h"
#include "decode/udp_stream.h"

//#ifdef	DEBUG_PER
#include "perform.h"
PER_DECLARE(udp)
//#endif

extern unsigned short udp_tuple_index;
extern struct REASM_CONF reasm_conf;
//#include "decode/dynamic_mapped_port.h"

#define DIR_REQUEST     0
#define DIR_ANSWER      1
#define END_CLIENT      0
#define END_SERVER      1

//ww0817
static inline int unkind_port_udp(int port)
{
	return unkind_port_table_udp[port] == UNKIND ? 1 : 0;
}


int udp_decode_init(void *private_info, char *args)
{
	struct bprivate_info *tmp_private = (struct bprivate_info *)private_info;
	tcpstr_conf_t tsconf;
	memset(&tsconf, 0, sizeof(tcpstr_conf_t));

	/* max_session_num */
	tsconf.max_session_num = (unsigned int)reasm_conf.max_session_num;
	tsconf.max_session_len = (unsigned int)reasm_conf.max_session_len;
	tsconf.mal_percent = (unsigned int)reasm_conf.mal_percent;
	tmp_private->udpstr_hdlr = (void *)udpstr_init(&tsconf, tmp_private);
	if (!tmp_private->udpstr_hdlr){
		return -1;
	}
		
	return 0;
}

void udp_decode_clean(void *private_info)
{
	return;
}

#if 0
static inline void udp_vfpm_match(struct bprivate_info * private_info, Packet_t *packet)
{
	DELARE_TIME_CONSUME();
	GET_CPUTIME(PRE_GETTIME);
	vfpm_match(packet, private_info->vfpm_result, MAX_VFPM_NUM);
	GET_CPUTIME(AFTER_GETTIME);
	COUNT_CPUTIME();
	OUTPUT_CPUTIME(udp_vfpm);
}
#endif

u_int8_t udp_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet)
{
	u_int8_t hi_proto_id = PROTO_ID_NULL;
	/* verify */
	if (packet->ip_proto != IPPROTO_UDP) {
		goto err;
	}
	/* check */
	if(packet->ip_payload_len < sizeof(struct udphdr)) {
		goto err;
	}

	PER_BEGIN(udp);
	packet->udp_hdr = (struct udphdr *)packet->ip_payload;
	
	packet->tdp_sport = ntohs(packet->udp_hdr->source);
	packet->tdp_dport = ntohs(packet->udp_hdr->dest);

	//ww0817
	if (likely(dt_conf.UnKindPort && unkind_port_udp(packet->tdp_sport) 
				&& unkind_port_udp(packet->tdp_dport))) {
		goto err;
	}
	//ww0817

	packet->udp_payload = packet->ip_payload + 8;
	packet->udp_payload_len = packet->ip_payload_len - 8;
	packet->udp_payloadlen = packet->ip_payload_len - 8;
	
	packet->udp_payload_s = packet->udp_payload;  
    packet->udp_payload_slen  = packet->udp_payload_len; 
         
    packet->udp_payload_h = packet->udp_payload;  
    packet->udp_payload_hlen  = packet->udp_payload_len; 

	
	packet->port_diffrence = packet->tdp_sport - packet->tdp_dport;

	if(udp_port_table[packet->tdp_dport]) {	
		hi_proto_id = udp_port_table[packet->tdp_dport] & (~WELLKNOWN_PORT_MASK);
		packet->dir = DIR_REQUEST;
		packet->pkt_dir = DIR_REQUEST;
		//goto ret;
	} else if(udp_port_table[packet->tdp_sport]) {
		hi_proto_id = udp_port_table[packet->tdp_sport] & (~WELLKNOWN_PORT_MASK);
		packet->dir = DIR_ANSWER;
		packet->pkt_dir = DIR_ANSWER;
	}
#ifndef VFPM_MATCH
	packet->vfpm_result[0]++;
	packet->vfpm_result[2] = udp_tuple_index;
#endif
	packet->proto_path[packet->proto_path_len++] = PROTO_ID_UDP;
	struct bprivate_info *tmp_private = (struct bprivate_info *) private_info;
	int nreasm = udpstr_reasm(packet,tmp_private);
	if (nreasm <= 0) {
		hi_proto_id = PROTO_ID_NULL;
		packet->high_proto_id = PROTO_ID_NULL;
		goto err;
	}
#if 0	
	printf("=========== UDP SUMMARY ========\n");
	printf("\tudp sport:%d\n", packet->tdp_sport);
	printf("\tudp dport:%d\n", packet->tdp_dport);          
	printf("\tudp hlength:%d\n", packet->udp_payloadlen);
	printf("============= END ==============\n");
#endif
//ret:	

	PER_END(udp);


//#ifdef VFPM_MATCH
//	vfpm_match(packet, PROTO_ID_UDP, packet->vfpm_result, MAX_VFPM_NUM);
//#endif	
	
//	SEND_EVENT(PEN_UDP_PACKET);	

	do_preproc(packet, packet->udp_payload, packet->udp_payload_len, 
			&(tmp_private->tdp_newpayload), 
				&(tmp_private->new_payload_len), PACKET_PLUGIN);

	if (packet->udp_payloadlen > 0) {
		return hi_proto_id;
	}
err:
	return PROTO_ID_NULL;
}

