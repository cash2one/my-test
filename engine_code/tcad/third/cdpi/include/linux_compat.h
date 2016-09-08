
#ifndef __CDPI_LINUX_COMPAT_H__
#define __CDPI_LINUX_COMPAT_H__

#include "cdpi_define.h"

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <machine/endian.h>

#if _BYTE_ORDER == _LITTLE_ENDIAN
#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__ 1
#endif
#else
#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__ 1
#endif
#endif
#endif

#pragma pack(push, 1)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

#pragma pack(pop)   /* restore original alignment from stack */

struct cdpi_ethhdr {
  u_char h_dest[6];       /* destination eth addr */
  u_char h_source[6];     /* source ether addr    */
  u_int16_t h_proto;      /* packet type ID field */
};

struct cdpi_80211q {
  u_int16_t vlanId;
  u_int16_t protoType;
};

struct cdpi_iphdr {
#if defined(__LITTLE_ENDIAN__) 
  u_int8_t ihl:4, version:4;
#elif defined(__BIG_ENDIAN__)
  u_int8_t version:4, ihl:4;                     //version, header length
#else
# error "Byte order must be defined"
#endif
  u_int8_t tos;                                  //type of service
  u_int16_t tot_len;                             // total length
  u_int16_t id;                                  //identifier
  u_int16_t frag_off;                            //fragment flag and fragment offset
  u_int8_t ttl;                                  // time to live
  u_int8_t protocol;
  u_int16_t check;
  u_int32_t saddr;
  u_int32_t daddr;
};


#ifdef WIN32

typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   uint;
typedef unsigned long  u_long;
typedef u_char  u_int8_t;
typedef u_short u_int16_t;
typedef uint   u_int32_t;

#define _WS2TCPIP_H_ /* Avoid compilation problems */
#define HAVE_SIN6_LEN


/* IPv6 address */
/* Already defined in WS2tcpip.h */
struct cdpi_win_in6_addr
{
  union
  {
    u_int8_t u6_addr8[16];
    u_int16_t u6_addr16[8];
    u_int32_t u6_addr32[4];
  } in6_u;
#ifdef s6_addr
#undef s6_addr
#endif

#ifdef s6_addr16
#undef s6_addr16
#endif

#ifdef s6_addr32
#undef s6_addr32
#endif

#define s6_addr                 in6_u.u6_addr8
  // #define s6_addr16               in6_u.u6_addr16
  // #define s6_addr32               in6_u.u6_addr32

};

#define in6_addr win_in6_addr

/* Generic extension header.  */
struct cdpi_ip6_ext
{
  u_int8_t  ip6e_nxt;		/* next header.  */
  u_int8_t  ip6e_len;		/* length in units of 8 octets.  */
};

#define s6_addr16		__u6_addr.__u6_addr16
#define s6_addr32		__u6_addr.__u6_addr32
#else
#ifndef __KERNEL__
#include <arpa/inet.h>
#endif
#endif



struct cdpi_in6_addr {
  union {
    u_int8_t   __u6_addr8[16];
    u_int16_t  __u6_addr16[8];
    u_int32_t  __u6_addr32[4];
  } __u6_addr;                    /* 128-bit IP6 address */
};

struct cdpi_ip6_hdr {
  union {
    struct cdpi_ip6_hdrctl {
      u_int32_t ip6_un1_flow;
      u_int16_t ip6_un1_plen;
      u_int8_t ip6_un1_nxt;
      u_int8_t ip6_un1_hlim;
    } ip6_un1;
    u_int8_t ip6_un2_vfc;
  } ip6_ctlun;
  struct cdpi_in6_addr ip6_src;
  struct cdpi_in6_addr ip6_dst;
};

struct cdpi_tcphdr {
  u_int16_t source;
  u_int16_t dest;
  u_int32_t seq;
  u_int32_t ack_seq;
#if defined(__LITTLE_ENDIAN__)
  u_int16_t res1:4, doff:4, fin:1, syn:1, rst:1, psh:1, ack:1, urg:1, ece:1, cwr:1;
#elif defined(__BIG_ENDIAN__)
  u_int16_t doff:4, res1:4, cwr:1, ece:1, urg:1, ack:1, psh:1, rst:1, syn:1, fin:1;
#else
# error "Byte order must be defined"
#endif  
  u_int16_t window;
  u_int16_t check;
  u_int16_t urg_ptr;
};

struct cdpi_udphdr {
  u_int16_t source;
  u_int16_t dest;
  u_int16_t len;
  u_int16_t check;
};

#endif
