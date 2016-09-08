#include <netinet/in.h>
#include <assert.h>

#include "decode/decode.h"
#include "dfp_ip_decode.h"
#include "decode/Packet.h"
#include "event.h"
#include "defrag.h"
#include "config.h"
#include "perform.h"

#include "misc.h"
#include "trace_api.h"
#include "time_consume.h"

#include "thread.h"
#include "thread_ctrl.h"
extern unsigned int CN_id;

#ifdef	DEBUG_PER
#include "perform.h"
PER_DECLARE(ip)
PER_DECLARE_COUNT(errip)
#endif
void *ipdfr_hdlr = NULL;
static struct DEFRAG_CONF {
	unsigned int max_defrag_num;
	char ip_frag_overlapped_str[10];
} defrag_conf;

extern unsigned short ip_tuple_index;

static cfg_desc defrag_cdt[] = {
	{"Anonymous", "max_defrag_num", &(defrag_conf.max_defrag_num),
	 sizeof(defrag_conf.max_defrag_num), CFG_TYPE_UINT32, 0, 0, ""}
	,
	{"Anonymous", "ip_frag_overlapped",
	 &(defrag_conf.ip_frag_overlapped_str),
	 sizeof(defrag_conf.ip_frag_overlapped_str), CFG_TYPE_STR, 0, 0, ""}
	,
	{"", "", NULL, 0, 0, 0, 0, ""}
};

#ifdef CHECKSUM_CHECK
/*	Comment:
 *  copy from asm/checksum.h - ip_fast_csum(), which always checksum on 4 octet boundaries.
 *
 *	Return Val:
 * 	0 : OK;
 *   !0 : BAD checksum;
 */
static inline unsigned short IP_chksum_fast(unsigned char *iph,
					    unsigned int ihl)
{
	unsigned int sum;

	__asm__ __volatile__("\
			movl (%1), %0\
			subl $4, %2\
			jbe 2f\
			addl 4(%1), %0\
			adcl 8(%1), %0\
			adcl 12(%1), %0\
			1:	    adcl 16(%1), %0\
			lea 4(%1), %1\
			decl %2\
			jne	1b\
			adcl $0, %0\
			movl %0, %2\
			shrl $16, %0\
			addw %w2, %w0\
			adcl $0, %0\
			notl %0\
			2:\
			"
			/* Since the input registers which are loaded with iph and ipl
			   are modified, we must also specify them as outputs, or gcc
			   will assume they contain their original values. */
			:"=r"(sum), "=r"(iph), "=r"(ihl)
			:"1"(iph), "2"(ihl)
	);

	return (sum);
}

/*
 * return val:
 * 	0 : OK;
 *   !0 : BAD checksum;
 */
static inline u_int16_t IP_chksum(unsigned short *w, int bytelen)
{
	unsigned int cksum;

	/* IP must be >= 20 bytes */
	cksum = w[0];
	cksum += w[1];
	cksum += w[2];
	cksum += w[3];
	cksum += w[4];
	cksum += w[5];
	cksum += w[6];
	cksum += w[7];
	cksum += w[8];
	cksum += w[9];

	bytelen -= 20;
	w += 10;

	/* IP-hdr must be an integral number of 4 byte words */
	while (bytelen) {
		cksum += w[0];
		cksum += w[1];
		w += 2;
		bytelen -= 4;
	}

	cksum = (cksum >> 16) + (cksum & 0x0000ffff);
	cksum += (cksum >> 16);

	return (unsigned short) (~cksum);
}
#endif

int ip_decode_init(void *private_info ,char *args)
{
//	thread_lock(IP_INIT_MUTEX);

	ipdfr_conf_t idconf;
	memset(&idconf, 0, sizeof(ipdfr_conf_t));

	struct bprivate_info *tmp_private = (struct bprivate_info *) private_info;
	/* init defrag conf value */
	memset(&defrag_conf, 0, sizeof(struct DEFRAG_CONF));
	int32_t read_conf_ret = common_read_conf((char *)dt_conf.reasm_conf_path, defrag_cdt);
	if (read_conf_ret < 0) {
		if (read_conf_ret == FILE_NOT_FOUND) {
			fprintf(stderr, "Warn: Can't open reasm conf(%s), use default configuration\n", 
					dt_conf.reasm_conf_path);
			goto next;
		} else {
			goto err;
		}   
	}

	idconf.max_defrag_num = (unsigned int)defrag_conf.max_defrag_num;

	/* ip_frag_overlapped */
	if (!strcasecmp(defrag_conf.ip_frag_overlapped_str, "new") || \
			!strcasecmp(defrag_conf.ip_frag_overlapped_str, "unix"))
		idconf.ip_frag_overlapped = OD_KEEP_NEW;
	else if (!strcasecmp(defrag_conf.ip_frag_overlapped_str, "old") || \
			!strcasecmp(defrag_conf.ip_frag_overlapped_str, "win32"))
		idconf.ip_frag_overlapped = OD_KEEP_OLD;
	else 
		idconf.ip_frag_overlapped = OD_NULL;

next:
	if ((idconf.max_defrag_num > 0) && ((tmp_private->ipdfr_hdlr = ipdfr_init(&idconf)) == 0)) {
		goto err;
	}
//	thread_unlock(IP_INIT_MUTEX);
	return 0;
err:
//	thread_unlock(IP_INIT_MUTEX);
	return -1;
}


void ip_decode_clean(void *private_info)
{
	struct bprivate_info *tmp_private = (struct bprivate_info *) private_info;

	if (tmp_private->ipdfr_hdlr){
		ipdfr_clean(tmp_private->ipdfr_hdlr);
		tmp_private->ipdfr_hdlr = NULL;
	}
}

#if 0
static inline void icmp_vfpm_match(struct bprivate_info * private_info, Packet_t *packet)
{
	DELARE_TIME_CONSUME();
	GET_CPUTIME(PRE_GETTIME);
	vfpm_match(packet, (private_info)->vfpm_result, MAX_VFPM_NUM);
	GET_CPUTIME(AFTER_GETTIME);
	COUNT_CPUTIME();
	OUTPUT_CPUTIME(icmp_vfpm);
}

static inline void igmp_vfpm_match(struct bprivate_info * private_info, Packet_t *packet)
{
	DELARE_TIME_CONSUME();
	GET_CPUTIME(PRE_GETTIME);
	vfpm_match(packet, (private_info)->vfpm_result, MAX_VFPM_NUM);
	GET_CPUTIME(AFTER_GETTIME);
	COUNT_CPUTIME();
	OUTPUT_CPUTIME(igmp_vfpm);
}

static inline void egp_vfpm_match(struct bprivate_info * private_info, Packet_t *packet)
{
	DELARE_TIME_CONSUME();
	GET_CPUTIME(PRE_GETTIME);
	vfpm_match(packet, (private_info)->vfpm_result, MAX_VFPM_NUM);
	GET_CPUTIME(AFTER_GETTIME);
	COUNT_CPUTIME();
	OUTPUT_CPUTIME(egp_vfpm);
}
#endif

u_int8_t ip_decode(void *private_info, u_int8_t * raw_packet, Packet_t * packet)
{
	u_int8_t ndecode = PROTO_ID_NULL;
	u_int16_t frag_off_h;
	int ndfr = 1;
	unsigned int sip, dip;
	
	struct bprivate_info *tmp_private = (struct bprivate_info *) private_info;

	/* Check */
	if (packet->eth_payload_len <= sizeof(struct iphdr)) {
		goto err;
	}
	PER_BEGIN(ip);

	frag_off_h = ntohs(*((u_int16_t *) ((char *) packet->eth_payload + 6)));

	/*
	 * Before defragment, record the original info relating to ip-fragment.
	 * Cauze after defragment, such info will be refreshed or wiped off.
	 */
	packet->ip_distance = 0x1fff & frag_off_h;
	packet->ip_flag_num = 0;
	packet->ip_flags = (0xe000 & frag_off_h) >> 13;
	packet->ip_flag_rb = (frag_off_h & 0x8000) >> 15;
	packet->ip_flag_df = (frag_off_h & 0x4000) >> 14;
	packet->ip_flag_mf = (frag_off_h & 0x2000) >> 13;
	if (packet->ip_flag_rb == 1)
		packet->ip_flag_num++;
	if (packet->ip_flag_df == 1)
		packet->ip_flag_num++;
	if (packet->ip_flag_mf == 1)
		packet->ip_flag_num++;

	if (isfragment(frag_off_h)) {
		ndfr = ipdfr_defrag(tmp_private->ipdfr_hdlr, (packet->eth_payload),
				    packet->defrag_retbuf,
				    sizeof(packet->defrag_retbuf));
		if (ndfr > 0) {
			// reassign eth vars
			packet->eth_payload = packet->defrag_retbuf;
			packet->eth_payloadlen = packet->eth_payload_len = ndfr;
		} else if (ndfr < 0) {
			goto err;
		}
	}

	packet->ip_hdr = (struct iphdr *) packet->eth_payload;
	packet->ip_hlength = packet->ip_hdr->ihl * 4;

	if (packet->ip_hlength < 20)
		goto err;
#ifdef CHECKSUM_CHECK
	/* checksum validity verification */
#ifdef FAST_CHECK_SUM
	if (IP_chksum_fast
	    ((unsigned char *) (packet->ip_hdr), packet->ip_hdr->ihl)) {
		printf("IP, checksum error.\n");
		goto err;
	}
#else
	if (IP_chksum((u_int16_t *) (packet->ip_hdr), packet->ip_hlength)) {
		printf("IP, checksum error.\n");
		goto err;
	}
#endif
#endif

	packet->ip_length = ntohs(packet->ip_hdr->tot_len);
	packet->ip_payload = packet->eth_payload + packet->ip_hlength;
	packet->ip_payload_s = packet->ip_payload_h = packet->ip_payload;
	if (unlikely(packet->ip_length < packet->ip_hlength )){
// WH for ip defrag 
//			|| (u_int8_t *)(packet->ip_payload) > packet->raw_pkt + packet->raw_pkt_len)) {

		goto err;
	}

	packet->ip_payloadlen = MIN_T(unsigned int, packet->ip_length - packet->ip_hlength, 
					packet->raw_pkt + packet->raw_pkt_len - (u_int8_t *)packet->ip_payload);
	packet->ip_payload_len = packet->ip_payloadlen;
	packet->ip_payload_hlen = packet->ip_payload_slen = packet->ip_payloadlen;

	packet->ip_options = packet->eth_payload + sizeof(struct iphdr);
	packet->ip_options_len = packet->ip_hlength - sizeof(struct iphdr);
	packet->ip_optionslen = packet->ip_options_len;

	packet->ip_sip = ntohl(packet->ip_hdr->saddr);
	packet->ip_dip = ntohl(packet->ip_hdr->daddr);

	sip = packet->ip_sip;
	dip = packet->ip_dip;

	packet->ip_diffrence = packet->ip_sip - packet->ip_dip;
	packet->ip_proto = packet->ip_hdr->protocol;
	packet->ip_version = packet->ip_hdr->version;
	packet->ip_ident = ntohs(packet->ip_hdr->id);

	packet->ip_xttl = packet->ip_hdr->ttl;

#if 0
	struct in_addr tmp_addr;
	char tmp_dip_str[20] = {0};
	char tmp_sip_str[20] = {0};
	memset(&tmp_addr, 0, sizeof(tmp_addr));
	
	tmp_addr.s_addr = packet->ip_hdr->daddr;
	strncpy(tmp_dip_str, inet_ntoa(tmp_addr), 19);
	tmp_addr.s_addr = packet->ip_hdr->saddr;
	strncpy(tmp_sip_str, inet_ntoa(tmp_addr), 19);

	printf("============IP SUMMARY ==============\n");
	printf("ip_proto:%d\n", packet->ip_proto);
	printf("ip_version:%d\n", packet->ip_version);
	printf("sip:%s\n", tmp_sip_str);
	printf("dip:%s\n", tmp_dip_str);
	printf("================ END =================\n");

#endif
	if (packet->ip_xttl == 0)
		goto err;

	packet->ip_service = packet->ip_hdr->tos;
	if (packet->ip_hlength == 24)
		packet->ip_op_code = (unsigned long) (*((unsigned char *) (packet->ip_hdr) + 20));

	if (packet->ip_version != 4) {
		goto err;
	}
	/* recode the path */
	packet->proto_path[packet->proto_path_len++] = PROTO_ID_IP;
#ifndef VFPM_MATCH
	packet->vfpm_result[0]++;
	packet->vfpm_result[1] = ip_tuple_index;
	//printf("ip:vfpm_result[%d],vfpm_index[%d]\r\n",packet->vfpm_result[0],ip_tuple_index);
#endif

	/*
	 * NOTE:
	 * if the current packet's mf bit is set, this packet will not be handled to
	 * the uplayer(TCP or UDP) to do the subsequent decoding.
	 */
	if (ndfr == 0) {
		ndecode = PROTO_ID_NULL;
		goto ret;
	}

	switch (packet->ip_proto) {
	case IPPROTO_TCP:
		ndecode = PROTO_ID_TCP;
		break;
	case IPPROTO_ICMP:
		ndecode = PROTO_ID_ICMP;
		break;
	case IPPROTO_UDP:
		ndecode = PROTO_ID_UDP;
		thread_pool[tmp_private->thread_idx].profile_info.udp_packet++;
		//thread_pool[tmp_private->thread_idx].profile_info.session_bits+=packet->raw_pkt_len << 3;
		break;
	case IPPROTO_IGMP:
		ndecode = PROTO_ID_IGMP;
		break;
	case IPPROTO_EGP:
		ndecode = PROTO_ID_EGP;
		break;
	default:
		ndecode = PROTO_ID_NULL;
		break;
	}

#ifdef VFPM_MATCH
	vfpm_match(packet, PROTO_ID_IP, packet->vfpm_result, MAX_VFPM_NUM);
#endif
ret:
	//
	packet->ip_sip_h = packet->ip_sip;
	packet->ip_dip_h = packet->ip_dip;

	PER_END(ip);
	return ndecode;

err:
	return PROTO_ID_NULL;
}
