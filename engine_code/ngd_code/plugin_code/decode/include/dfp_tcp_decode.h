#ifndef _DFP_TCP_DECODE
#define _DFP_TCP_DECODE

#include "decode/Packet.h"
#include "decode/proto.h"

#define PORTLIST_STR_LEN        1024

struct REASM_CONF {
        char            portlist_str[PORTLIST_STR_LEN];
        unsigned int   max_session_num;
        unsigned int   max_session_len;
        char                    tcp_seg_overlapped_str[10];
        char                    port_table[65536];
        char                    proto_table[PROTO_NUM]; // PROTO_TABLE;
        unsigned int   mal_percent;
	unsigned int	situation;
};

//extern struct REASM_CONF reasm_conf;

int tcp_decode_init(void *private_info, char *args);
void tcp_decode_clean(void *private_info);
u_int8_t tcp_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet);

#endif
