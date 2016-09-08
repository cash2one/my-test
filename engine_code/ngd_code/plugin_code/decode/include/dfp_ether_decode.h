#ifndef _DFP_ETHER_DECODE_H
#define _DFP_ETHER_DECODE_H
#include "pkt_info.h"
#include "plugin_manage.h"

struct _VlanHdr
{
    u_int16_t vh_pri_cfi_vlan;
    u_int16_t vh_proto;  /* protocol field... */
};
extern unsigned char special_eth_hdr[14];

#if 0
int ether_decode_init(void *private_info, char *args);
int special_ether_decode_init(void *private_info, char *args);
void ether_decode_clean(void *private_info);
u_int8_t ether_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet);
u_int8_t special_ether_decode(void *private_info, u_int8_t *raw_packet, Packet_t *packet);
u_int8_t MPLS_decode(u_int8_t *raw_packet, Packet_t *packet);
#endif
#endif
