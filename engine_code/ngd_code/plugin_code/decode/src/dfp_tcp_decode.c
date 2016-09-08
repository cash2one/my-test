// dfp_tcp_decode.c 
// M : Alex, 02-10-29
// M : Alex, 04-04-10, PROTO_TABLE: use proto_table, not port_table, to check if need reasm, 
// 			because some proto havent wellknown port
// 		       NEGO_TABLE: some high protocol negotiate port, so let the packet of these protocol go
//
#include "decode/dfp/dfp_tcp_decode.h"
#include "decode/decode.h"
#include "decode/tcp_stream.h"
#include "decode/port_table.h"
#include "config.h"
#include "misc.h"
#include "decode/port_table.h"
#include "decode/dynamic_mapped_port.h"

#include "trace_api.h"
#include "thread.h"
#include "thread_ctrl.h"
#include "vfpm/vfpm_conf.h"
#include "time_consume.h"
#include "event.h"

#ifdef DEBUG_PER
#include "perform.h"
PER_DECLARE(tcp)
#endif

extern unsigned short tcp_tuple_index;
struct REASM_CONF reasm_conf;

static cfg_desc reasm_cdt[] = {
	{"", "portlist", &(reasm_conf.portlist_str), \
			sizeof(reasm_conf.portlist_str), CFG_TYPE_STR, 0, 0, ""},
	{"", "max_session_num", &(reasm_conf.max_session_num), \
			sizeof(reasm_conf.max_session_num), CFG_TYPE_UINT32, 0, 0, ""},
	{"", "max_session_len", &(reasm_conf.max_session_len), \
		sizeof(reasm_conf.max_session_len), CFG_TYPE_UINT32, 0, 0, ""},
	{"", "tcp_seg_overlapped", &(reasm_conf.tcp_seg_overlapped_str), \
			sizeof(reasm_conf.tcp_seg_overlapped_str), CFG_TYPE_STR, 0, 0, ""},
	{"", "malssn_percent", &(reasm_conf.mal_percent), \
			sizeof(reasm_conf.mal_percent), CFG_TYPE_UINT32, 0, 0, ""},
	{"", "situation", &(reasm_conf.situation), \
		sizeof(reasm_conf.situation), CFG_TYPE_UINT32, 0, 0, ""},
	{"", "", NULL, 0, 0, 0, 0, ""},
};


#define SINGLE_STREAM_0 0
#define SINGLE_STREAM_1 1
#define DUAL_STREAM 2


static inline int unkind_port_tcp(int port)
{
	return unkind_port_table_tcp[port] == UNKIND ? 1 : 0;
}

void tcp_decode_clean(void *private_info)
{
	struct bprivate_info *tmp_private = (struct bprivate_info *)private_info;
	tcpstr_clean(tmp_private->tcpstr_hdlr, private_info);

#ifdef STREAM_LOG
	if (dt_conf.stream_log)
		strlog_clean();
#endif
}

#ifdef CHECKSUM_CHECK
/**/
static inline unsigned short in_chksum_tcp(  unsigned short *h, unsigned short * d, int dlen )
{
   unsigned int cksum;
   unsigned short answer=0;

   /* PseudoHeader must have 12 bytes */
   cksum  = h[0];
   cksum += h[1];
   cksum += h[2];
   cksum += h[3];
   cksum += h[4];
   cksum += h[5];

   /* TCP hdr must have 20 hdr bytes */
   cksum += d[0];
   cksum += d[1];
   cksum += d[2];
   cksum += d[3];
   cksum += d[4];
   cksum += d[5];
   cksum += d[6];
   cksum += d[7];
   cksum += d[8];
   cksum += d[9];

   dlen  -= 20; /* bytes   */
   d     += 10; /* short's */ 

   while(dlen >=32)
   {
     cksum += d[0];
     cksum += d[1];
     cksum += d[2];
     cksum += d[3];
     cksum += d[4];
     cksum += d[5];
     cksum += d[6];
     cksum += d[7];
     cksum += d[8];
     cksum += d[9];
     cksum += d[10];
     cksum += d[11];
     cksum += d[12];
     cksum += d[13];
     cksum += d[14];
     cksum += d[15];
     d     += 16;
     dlen  -= 32;
   }

   while(dlen >=8)  
   {
     cksum += d[0];
     cksum += d[1];
     cksum += d[2];
     cksum += d[3];
     d     += 4;   
     dlen  -= 8;
   }

   while(dlen > 1)
   {
     cksum += *d++;
     dlen  -= 2;
   }

   if( dlen == 1 ) 
   { 
    /* printf("new checksum odd byte-packet\n"); */
    *(unsigned char*)(&answer) = (*(unsigned char*)d);

    /* cksum += (u_int16_t) (*(u_int8_t*)d); */
     
     cksum += answer;
   }
   
   cksum  = (cksum >> 16) + (cksum & 0x0000ffff);
   cksum += (cksum >> 16);
 
   return (unsigned short)(~cksum);
}
#endif

#define PREPROC_TDP_PAYLOAD_LEN	4096
static int alloc_preproc_mem(bthread_private_info_t *tmp_private)
{
	tmp_private->tdp_newpayload = calloc(PREPROC_TDP_PAYLOAD_LEN, sizeof(char));
	if(tmp_private->tdp_newpayload == NULL) {
		EMSG("calloc error when allocing tdp_newpayload");
		goto err;
	}

	tmp_private->stream_newpayload = calloc(PREPROC_TDP_PAYLOAD_LEN * 3, sizeof(char));
	if(tmp_private->stream_newpayload == NULL) {
		EMSG("calloc error when allocing stream_newpayload");
		goto err;
	}
	tmp_private->new_payload_len = PREPROC_TDP_PAYLOAD_LEN;
	return 0;
err:
	return -1;
}

static void clean_preproc_mem(bthread_private_info_t *tmp_private)
{
	if(tmp_private->tdp_newpayload) {
		free(tmp_private->tdp_newpayload);
		tmp_private->tdp_newpayload = NULL;
	}
	if(tmp_private->stream_newpayload) {
		free(tmp_private->stream_newpayload);
		tmp_private->stream_newpayload = NULL;
	}
	return ;
}

int tcp_decode_init(void *private_info, char *args)
{
	thread_lock(TCP_INIT_MUTEX);
	struct bprivate_info *tmp_private = (struct bprivate_info *)private_info;
	/* init reasm conf value */
	memset(&reasm_conf, 0, sizeof(struct REASM_CONF));
	/* fill reasm conf value 
	 * 1. construct rule entry list
	 */ 
	int32_t read_conf_ret = common_read_conf((char *)dt_conf.reasm_conf_path, reasm_cdt);
	if (read_conf_ret < 0) {
		if (read_conf_ret == -1) {
			fprintf(stderr, "Warn: Can't open reasm conf(%s), use default configuration\n", 
					dt_conf.reasm_conf_path);
			goto next;
		} else {
			goto err;
		}   
	} 

	/* portlist, fill reasm_conf.port_table, 
	 * whenever packet proto id is in port_table.
	 */
	if (reasm_conf.portlist_str[0]) {
#define MAX_REASM_PORT_NAME_NUM	100
#define MAX_REASM_PORT_NAME_LEN	64
		char portname_table[MAX_REASM_PORT_NAME_NUM][MAX_REASM_PORT_NAME_LEN];
		struct split_var svt[MAX_REASM_PORT_NAME_NUM];
		int i;
		int nsplit;

		/* init */
		for (i = 0; i < PROTO_NUM; i++) {
			reasm_conf.proto_table[i] = 0;
		}
		for (i = 0; i < 65536; i++) {
			reasm_conf.port_table[i] = 0;
		}
			
		/* parse definition string */
		for (i = 0; i < MAX_REASM_PORT_NAME_NUM; i++) {
			svt[i].addr = portname_table[i];
			svt[i].maxlen = MAX_REASM_PORT_NAME_LEN;
		}
		nsplit = str_split(reasm_conf.portlist_str, ',', sf_scp, svt, MAX_REASM_PORT_NAME_NUM);
		if (nsplit > 0) {
			u_int8_t id;
			unsigned long lport;
			
			for (i = 0; i < nsplit; i++) {
				/* possible value: 
				 * 1. proto name, like http, ftp, etc.
				 * 2. port number, like 8000, 8080, etc.
				 * 3. "all", means all port will be reasmed
				 */
				if (!strcasecmp(svt[i].addr, "all")) {
					for (i = 0; i < 65536; i++) {
						reasm_conf.port_table[i] = 1;
					}
					break;
				}
				
				/* if it's proto name */
				id = proto_name2id((char *)svt[i].addr);

				if (id != PROTO_ID_NULL) {
					/* reassemble this app-proto ONLY if it can be decoded */
				//	reasm_conf.proto_table[id] = decode_protos[id].selectflag;
					reasm_conf.proto_table[id] = 1;
				} else {
					/* if it's port number */
					lport = strtoul(svt[i].addr, NULL, 0);				
					if (lport <= 65535 && lport > 0) {
						reasm_conf.port_table[lport] = 1;
					}
				}
			}
			/* 
			 * according to reasm_conf.proto_table, 
			 * set reasm_conf.port_table, so we can 
			 * decide if one port need reasm 
			 */
			for (i = 0; i < 65536; i++) {
				id = tcp_port_table[i] & (~WELLKNOWN_PORT_MASK);	
				if (id != PROTO_ID_NULL && reasm_conf.proto_table[id]) {
					reasm_conf.port_table[i] = 1;
				}
			}
			
		}
	}

	/*active protocol reassemble setting*/
//	Active_Reasm_Init();

#if 1
	tcpstr_conf_t tsconf;
	memset(&tsconf, 0, sizeof(tcpstr_conf_t));

	/* max_session_num */
	tsconf.max_session_num = (unsigned int)reasm_conf.max_session_num;
	tsconf.max_session_len = (unsigned int)reasm_conf.max_session_len;
	tsconf.mal_percent = (unsigned int)reasm_conf.mal_percent;
	/* tcp_seg_overlapped */
	if (!strcasecmp(reasm_conf.tcp_seg_overlapped_str, "new") || \
			!strcasecmp(reasm_conf.tcp_seg_overlapped_str, "unix"))
		tsconf.tcp_seg_overlapped = OD_KEEP_NEW;
	else if (!strcasecmp(reasm_conf.tcp_seg_overlapped_str, "old") || \
			!strcasecmp(reasm_conf.tcp_seg_overlapped_str, "win32"))
		tsconf.tcp_seg_overlapped = OD_KEEP_OLD;
	else 
		tsconf.tcp_seg_overlapped = OD_NULL;
#endif

next:
	tmp_private->tcpstr_hdlr = tcpstr_init(&tsconf, tmp_private);
	if (!tmp_private->tcpstr_hdlr){
		goto err;
	}
	
#ifdef STREAM_LOG
	if (dt_conf.stream_log) {
		if (strlog_init() < 0) {
			DMSG("Error: Stream Log init\n");
			goto err;
		}
	}
#endif
	if(alloc_preproc_mem(tmp_private) < 0) {
		goto err;
	}
	thread_unlock(TCP_INIT_MUTEX);
	return 0;
err:
	clean_preproc_mem(tmp_private);
	thread_unlock(TCP_INIT_MUTEX);
	return -1;
}

static inline int tcp_header(Packet_t *packet)
{
	if(packet->ip_payload_len < sizeof(struct tcphdr)) {
		goto err;
	}

	if (packet->ip_proto != IPPROTO_TCP) {
		goto err;
	}

	u_int16_t tcp_hdr_len = 0;

#ifdef  DEBUG_PER
	PER_BEGIN(tcp);
#endif
	struct tcphdr *tcp_head = (struct tcphdr *)packet->ip_payload;
	
	tcp_hdr_len = tcp_head->doff*4;

	if (unlikely(packet->ip_payload_len < tcp_hdr_len)) {
		goto err;
	}
	packet->tcp_hdr = tcp_head;
	
	packet->tdp_sport = ntohs(tcp_head->source);
	packet->tdp_dport = ntohs(tcp_head->dest);
	
	if (likely(dt_conf.UnKindPort 
		&& unkind_port_tcp(packet->tdp_sport) 
		&& unkind_port_tcp(packet->tdp_dport)))
		goto donothing;
	
	packet->tcp_payload = packet->ip_payload + tcp_hdr_len;
	packet->tcp_payload_len = packet->tcp_payloadlen = packet->ip_payload_len - tcp_hdr_len;

	packet->tdp_hlength = tcp_hdr_len;
	packet->tdp_length = packet->ip_payload_len;
	
#ifdef CHECKSUM_CHECK
	struct pseudoheader {     
	       u_int32_t sip, dip;   
	       u_int8_t  zero;       
	       u_int8_t  protocol;   
	       u_int16_t tcplen;     
	} ph;

	/* checksum */
	ph.sip = packet->ip_hdr->saddr;
	ph.dip = packet->ip_hdr->daddr;
	ph.zero = 0;
	ph.protocol = packet->ip_hdr->protocol;
	ph.tcplen = htons(packet->tdp_length);
	if(in_chksum_tcp((u_int16_t *)(&ph), (u_int16_t *)(packet->tcp_hdr), packet->tdp_length)) {
		printf("tcp checksum err.\n");
		goto err;
	}
#endif
	
	packet->tcp_syn = tcp_head->syn;
	packet->tcp_ack = tcp_head->ack;
	packet->tcp_rst = tcp_head->rst;
	packet->tcp_fin = tcp_head->fin;
	packet->tcp_psh = tcp_head->psh;
	packet->tcp_urg = tcp_head->urg;
	packet->tcp_res2 = tcp_head->res2 & 0x0001;
	packet->tcp_res1 = (tcp_head->res2>>1) & 0x0001;
	packet->tcp_seq = ntohl(tcp_head->seq);
	packet->tcp_ack_seq = ntohl(tcp_head->ack_seq);
	packet->tcp_window = ntohs(tcp_head->window);
	packet->tcp_flag_num = 0;

	if(packet->tcp_urg == 1)
		packet->tcp_flag_num++;
	if(packet->tcp_ack == 1)
		packet->tcp_flag_num++;
	if(packet->tcp_psh == 1)
		packet->tcp_flag_num++;
	if(packet->tcp_fin == 1) {
		packet->tcp_flag_num++;
		packet->tcp_status = 4;
	}
	if(packet->tcp_syn == 1) {
		packet->tcp_flag_num++;
		packet->tcp_status = 1;
	} 
	if(packet->tcp_rst == 1) {
		packet->tcp_flag_num++;
		packet->tcp_status = 5;
	} 
		
	packet->tcp_flag = *((char *)(packet->tcp_hdr) + 13);
#if 0	
	printf("=========== TCP SUMMARY ========\n");
	printf("\ttcp sport:%d\n", packet->tdp_sport);
	printf("\ttcp dport:%d\n", packet->tdp_dport);
	printf("\ttcp hlength:%d\n", packet->tdp_hlength);

	printf("\tpacket->tcp_syn :%d\n", packet->tcp_syn );
	printf("\tpacket->tcp_ack :%d\n", packet->tcp_ack );
	printf("\tpacket->tcp_rst :%d\n", packet->tcp_rst );
	printf("\tpacket->tcp_fin :%d\n", packet->tcp_fin );
	printf("\tpacket->tcp_psh :%d\n", packet->tcp_psh );
	printf("\tpacket->tcp_urg :%d\n", packet->tcp_urg );
	printf("\tpacket->tcp_res2:%d\n", packet->tcp_res2);
	printf("\tpacket->tcp_res1:%d\n", packet->tcp_res1);
	printf("\tpacket->tcp_seq :%d\n", packet->tcp_seq );

	printf("============= END ==============\n");
#endif
	return 0;
donothing:
	return 1;
err:
	return -1;

}


void print_decodecontent(char *payload, u_int32_t len)
{
#if 1
	DMSG("\t========= decrypt content =============");
//	print_buf2(payload, len, 0);
	DMSG("\t======== end decrypt content===========");
	typedef struct
	{
		u_int32_t time;
		u_int32_t sdev;
		u_int32_t ddev;
		u_int16_t com_type;     
		u_int16_t mod_type;     
		u_int16_t sub_type1;   
		u_int16_t sub_type2;   
		int32_t para_len;    
	}COM;
	typedef struct
	{
		u_int8_t   Tag;                 
		u_int8_t   Version;             
		u_int8_t   Send_type;     
		u_int8_t   Recv_type;     
		u_int16_t   serial;              
		u_int8_t   reserve[2];          
	}GEV;
	typedef struct Vip_Head
	{
		GEV ginfo;   
		COM uinfo;
	}VIPHEAD;

	VIPHEAD *test_vip = (VIPHEAD *)payload;
	if(test_vip == NULL) {
		DMSG("new_payload == NULL\n");
		return;
	}
	DMSG("time:%u", test_vip->uinfo.time);
	DMSG("sdev:%u", test_vip->uinfo.sdev);
	DMSG("ddev:%u", test_vip->uinfo.ddev);
	DMSG("com_type:%u", test_vip->uinfo.com_type);
	DMSG("mod_type:%u", test_vip->uinfo.mod_type);
	DMSG("sub_type1:%u", test_vip->uinfo.sub_type1);
	DMSG("sub_type2:%u", test_vip->uinfo.sub_type2);
	DMSG("para_len:%u", test_vip->uinfo.para_len);
	DMSG("tag:%d", test_vip->ginfo.Tag);
	DMSG("version:%d", test_vip->ginfo.Version);
	DMSG("send_type:%d", test_vip->ginfo.Send_type);
	DMSG("recv_type:%d", test_vip->ginfo.Recv_type);
	DMSG("serial:%d", test_vip->ginfo.serial);
	DMSG("reserve0:%d", test_vip->ginfo.reserve[0]);
	DMSG("reserve1:%d", test_vip->ginfo.reserve[1]);
#endif
	return;
}

static inline void set_direction(Packet_t *packet, u_int16_t *hi_proto_id_pair)
{
	if (unlikely((tcp_port_table[packet->tdp_dport])        //src and dst are well know ports
				&& (tcp_port_table[packet->tdp_sport]))) {
		*hi_proto_id_pair = ((tcp_port_table[packet->tdp_sport] & (~WELLKNOWN_PORT_MASK)) << 8) |
			(tcp_port_table[packet->tdp_dport] & (~WELLKNOWN_PORT_MASK));
		packet->dir = DIR_REQUEST;              /* guess */
		packet->pkt_dir = DIR_REQUEST;  /* guess */
		goto ret;
	} else if(tcp_port_table[packet->tdp_dport]) {          //dst is well-know ports
		*hi_proto_id_pair = (tcp_port_table[packet->tdp_dport] & (~WELLKNOWN_PORT_MASK));
		packet->dir = DIR_REQUEST;
		packet->pkt_dir = DIR_REQUEST;
		goto ret;
	} else if(tcp_port_table[packet->tdp_sport]) {          //src is well-know ports 
		*hi_proto_id_pair = (tcp_port_table[packet->tdp_sport] & (~WELLKNOWN_PORT_MASK)) << 8;
		packet->dir = DIR_ANSWER;
		packet->pkt_dir = DIR_ANSWER;
		goto ret;
	}
	/*no need port_nego and proto_identify*/
ret:
	return;
}

#if 0
static inline void tcp_vfpm_match(struct bprivate_info* tmp_private, Packet_t *packet)
{
	DELARE_TIME_CONSUME();
	GET_CPUTIME(PRE_GETTIME);
	vfpm_match(packet, tmp_private->vfpm_result, MAX_VFPM_NUM);
	GET_CPUTIME(AFTER_GETTIME);
	COUNT_CPUTIME();
	OUTPUT_CPUTIME(tcp_vfpm);
}
#endif

//
// tcp_decode
// NTD: 1. use port table to decide the high proto
// 	2. use session to decide the high proto, to provide dynamic port proto
//	3. handle the reverse stream flushing data
u_int8_t tcp_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet)
{
	struct iphdr *ip_head = NULL;

	if(tcp_header(packet) < 0) {
		goto err;
	}

	packet->high_proto_id = PROTO_ID_NULL;

	/* recode the path */
	packet->proto_path[packet->proto_path_len++] = PROTO_ID_TCP;

	struct bprivate_info *tmp_private = (struct bprivate_info *)private_info;

	int nreasm = 0;
	u_int16_t hi_proto_id_pair = PROTO_ID_NULL;
	u_int8_t hi_proto_id = PROTO_ID_NULL; 

	set_direction(packet, &hi_proto_id_pair);
	/* added by zyl, 2010-4-12*/
	if (dt_conf.fr_flag) { // file recombile
		mapped_port_entry_t entry;
		entry.src_ip = packet->ip_sip;
		entry.dest_ip = packet->ip_dip;
		entry.src_port = packet->tdp_sport;
		entry.dest_port = packet->tdp_dport;
		entry.traslayer_proto_id = PROTO_ID_TCP;
		void *p_ck = ((bthread_private_info_t *)private_info)->portnego_ck;
		if ((!get_mapped_port(p_ck, &entry))
			&& entry.traslayer_proto_id == PROTO_ID_TCP) {
			packet->high_proto_id = entry.proto_id;
			hi_proto_id_pair = entry.proto_id;
			packet->nego_flag = 1;
		} else {
			packet->nego_flag = 0;
		}
	}
	/* added end */
//	if (reasm_conf.proto_table[(hi_proto_id_pair >> 8) & 0xff]
//			|| reasm_conf.proto_table[hi_proto_id_pair & 0xff]) {
		/* 

		 */
		switch (reasm_conf.situation) {
		case DUAL_STREAM:
		case SINGLE_STREAM_1:
			packet->high_proto_id_pair = hi_proto_id_pair;
	//		packet->tcpreasm_rtn = ERR_ERROR;
			/* support bgp protocol  dy */
			hi_proto_id = packet->high_proto_id_pair & 0xff;
			if (hi_proto_id == 0)
			       	hi_proto_id = (packet->high_proto_id_pair >> 8) & 0xff;
			if(hi_proto_id == PROTO_ID_BGP){
				DMSG(" PROTO_ID_BGP\r\n");
				packet->high_proto_id = hi_proto_id;
				goto ret;
			}	
			/*end*/

			nreasm = tcpstr_reasm(packet,tmp_private);
			if (nreasm <= 0) {
				/* 
				 * nreasm <  0 : tcpreasm error. 
				 * nreasm == 0 : REASM_EATEN, do not do high proto ana.
				 */
				hi_proto_id = PROTO_ID_NULL;
				packet->high_proto_id = PROTO_ID_NULL;
				goto ret;
			} else {
				switch (nreasm) {
				case REASM_NORMAL:// continue high proto ana.
#ifdef STREAM_LOG
					if (dt_conf.stream_log)
						strlog_log(packet);
#endif
					break;

				case REASM_SUCC_REASM:
				case REASM_SUCC_RECYCLE:
				case REASM_SUCC_FLUSH:
					ip_head = (struct iphdr *)packet->reasm_retbuf;
					packet->ip_hdr = ip_head;
					packet->ip_hlength = ip_head->ihl*4;
					packet->ip_length = ntohs(ip_head->tot_len);
					packet->ip_sip = ntohl(ip_head->saddr);
					packet->ip_dip = ntohl(ip_head->daddr);
					packet->ip_payload = (char *)ip_head + packet->ip_hlength;
					packet->ip_payload_len = packet->ip_length - packet->ip_hlength;

					struct tcphdr *tcp_head = (struct tcphdr *)packet->ip_payload;
					packet->tcp_hdr = tcp_head;
					packet->tdp_hlength = tcp_head->doff*4;
					packet->tdp_length = packet->ip_payload_len;
					packet->tcp_payload = tcp_head + tcp_head->doff*4;
					packet->tcp_payload_len = packet->tcp_payloadlen = \
							packet->ip_payload_len - tcp_head->doff*4;
					packet->tdp_sport = ntohs(tcp_head->source);
					packet->tdp_dport = ntohs(tcp_head->dest);

					break;
				default: 
					break;
				}
					
				/* 
				 * adjustment according to tcp-session this packet belongs to
				 */

				packet->dir = packet->stream_dir;
				packet->pkt_dir = packet->stream_dir;
#if 1
				/*execute the preproc_plugin*/
				if(!packet->have_preproc){

					do_preproc(packet, packet->tcp_payload, packet->tcp_payload_len, 
						&(tmp_private->tdp_newpayload), &(tmp_private->new_payload_len), PACKET_PLUGIN);
					print_decodecontent(tmp_private->tdp_newpayload, tmp_private->new_payload_len);
					if(packet->have_preproc){
						packet->tcp_payload = tmp_private->tdp_newpayload;
						packet->tcp_payloadlen = tmp_private->new_payload_len;
						packet->tcp_payload_len = tmp_private->new_payload_len;
					}		
				}
#endif
				if (packet->tcp_session) {
					/* One exception:
					 * the termination-packet(such as 2nd FIN or RST)'s high_proto_id 
					 * 	cannot be adjusted, since packet->tcp_session has been set
					 * 	NULL by tcpstr_reasm() just now.
					 *
					 * 	However, in this case, hi_proto_id is less important, because
					 * 	FIN or RST packet seldom carry tcp_payload.
					 */
					hi_proto_id = packet->tcp_session->high_proto_id;
					packet->high_proto_id = packet->tcp_session->high_proto_id;
				}
			}
			break;
		case SINGLE_STREAM_0:
			/*
			 * so-called reassembling, no signle stream reassembling actually.
			 */
			if (reasm_conf.proto_table[hi_proto_id_pair & 0xff]) {
				hi_proto_id = hi_proto_id_pair & 0xff;
				packet->stream_dir = DIR_REQUEST;
			} else {
				hi_proto_id = (hi_proto_id_pair >> 8) & 0xff;
				packet->stream_dir = DIR_ANSWER;
			}
			break;
		default:
			break;
		}

ret:
#ifdef  DEBUG_PER
	PER_END(tcp);
#endif
	if (nreasm >= 0) { 
#if 1
	/*execute the preproc_plugin*/
	if(!packet->have_preproc){

		do_preproc(packet, packet->tcp_payload, packet->tcp_payload_len, 
			&(tmp_private->tdp_newpayload), &(tmp_private->new_payload_len), PACKET_PLUGIN);
	//	print_decodecontent(tmp_private->tdp_newpayload, tmp_private->new_payload_len);
		if(packet->have_preproc){
			packet->tcp_payload = tmp_private->tdp_newpayload;
			packet->tcp_payloadlen = tmp_private->new_payload_len;
			packet->tcp_payload_len = tmp_private->new_payload_len;
		}		
	    }
#endif
	
   }

    packet->tcp_payload_s = packet->tcp_payload;  
    packet->tcp_payload_slen  = packet->tcp_payloadlen; 
         
    packet->tcp_payload_h = packet->tcp_payload;  
    packet->tcp_payload_hlen  = packet->tcp_payloadlen; 

    packet->stream_payload_s = packet->stream_payload;  
    packet->stream_payload_slen  = packet->stream_payloadlen; 
         
    packet->stream_payload_h = packet->stream_payload;  
    packet->stream_payload_hlen  = packet->stream_payloadlen; 

    return packet->high_proto_id;
err:
    return PROTO_ID_NULL;
}

