#ifndef __DFP_UDP_DECODE_H
#define __DFP_UDP_DECODE_H

#include "decode/Packet.h"
int udp_decode_init(void *private_info, char *args);
void udp_decode_clean(void*private_info);
u_int8_t udp_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet);
#endif
