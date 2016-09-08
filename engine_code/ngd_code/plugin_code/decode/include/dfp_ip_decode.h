#ifndef _DFP_IP_DECODE_H
#define _DFP_IP_DECODE_H

#include "decode/Packet.h"

int ip_decode_init(void *private_info,  char *args);
void ip_decode_clean(void *private_info);
u_int8_t ip_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet);
#endif // _DFP_IP_DECODE_H
