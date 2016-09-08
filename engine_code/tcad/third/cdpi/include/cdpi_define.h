
#ifndef __CDPI_DEFINE_INCLUDE_FILE__
#define __CDPI_DEFINE_INCLUDE_FILE__

/*
  gcc -E -dM - < /dev/null |grep ENDIAN
*/

#if 0
#ifndef CDPI_ENABLE_DEBUG_MESSAGES
#define CDPI_ENABLE_DEBUG_MESSAGES
#endif
#endif

#ifdef WIN32
#define __LITTLE_ENDIAN__ 1
#endif

#if !(defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__))
/* Kernel modules */
#if defined(__LITTLE_ENDIAN)
#define __LITTLE_ENDIAN__
#endif
#if defined(__BIG_ENDIAN)
#define __BIG_ENDIAN__
#endif
/* Everything else */
#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__))
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#else
#define __BIG_ENDIAN__
#endif
#endif

#endif

#define CDPI_USE_ASYMMETRIC_DETECTION             0
#define CDPI_SELECTION_BITMASK_PROTOCOL_SIZE			u_int32_t

#define CDPI_SELECTION_BITMASK_PROTOCOL_IP			(1<<0)
#define CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP			(1<<1)
#define CDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP			(1<<2)
#define CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP		(1<<3)
#define CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD		(1<<4)
#define CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION	(1<<5)
#define CDPI_SELECTION_BITMASK_PROTOCOL_IPV6			(1<<6)
#define CDPI_SELECTION_BITMASK_PROTOCOL_IPV4_OR_IPV6		(1<<7)
#define CDPI_SELECTION_BITMASK_PROTOCOL_COMPLETE_TRAFFIC	(1<<8)
/* now combined detections */

/* v4 */
#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP (CDPI_SELECTION_BITMASK_PROTOCOL_IP | CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP)
#define CDPI_SELECTION_BITMASK_PROTOCOL_UDP (CDPI_SELECTION_BITMASK_PROTOCOL_IP | CDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP)
#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP (CDPI_SELECTION_BITMASK_PROTOCOL_IP | CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP)

/* v6 */
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP (CDPI_SELECTION_BITMASK_PROTOCOL_IPV6 | CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_UDP (CDPI_SELECTION_BITMASK_PROTOCOL_IPV6 | CDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP (CDPI_SELECTION_BITMASK_PROTOCOL_IPV6 | CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP)

/* v4 or v6 */
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP (CDPI_SELECTION_BITMASK_PROTOCOL_IPV4_OR_IPV6 | CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_UDP (CDPI_SELECTION_BITMASK_PROTOCOL_IPV4_OR_IPV6 | CDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP (CDPI_SELECTION_BITMASK_PROTOCOL_IPV4_OR_IPV6 | CDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP)


#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)

/* does it make sense to talk about udp with payload ??? have you ever seen empty udp packets ? */
#define CDPI_SELECTION_BITMASK_PROTOCOL_UDP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_UDP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_V6_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_UDP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)

#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP_WITH_PAYLOAD		(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)

#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION)

#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION)

#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_WITH_PAYLOAD_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_WITH_PAYLOAD_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_WITH_PAYLOAD_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)

#define CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP_WITH_PAYLOAD_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP_WITH_PAYLOAD_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V6_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)
#define CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP_WITH_PAYLOAD_WITHOUT_RETRANSMISSION	(CDPI_SELECTION_BITMASK_PROTOCOL_V4_V6_TCP_OR_UDP | CDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION | CDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD)

/* safe src/dst protocol check macros... */

#define CDPI_SRC_HAS_PROTOCOL(src,protocol) ((src) != NULL && CDPI_COMPARE_PROTOCOL_TO_BITMASK((src)->detected_protocol_bitmask,(protocol)) != 0)

#define CDPI_DST_HAS_PROTOCOL(dst,protocol) ((dst) != NULL && CDPI_COMPARE_PROTOCOL_TO_BITMASK((dst)->detected_protocol_bitmask,(protocol)) != 0)

#define CDPI_SRC_OR_DST_HAS_PROTOCOL(src,dst,protocol) (CDPI_SRC_HAS_PROTOCOL(src,protocol) || CDPI_SRC_HAS_PROTOCOL(dst,protocol))

/**
 * convenience macro to check for excluded protocol
 * a protocol is excluded if the flow is known and either the protocol is not detected at all
 * or the excluded bitmask contains the protocol
 */
#define CDPI_FLOW_PROTOCOL_EXCLUDED(cdpi_struct,flow,protocol) ((flow) != NULL && \
								( CDPI_COMPARE_PROTOCOL_TO_BITMASK((cdpi_struct)->detection_bitmask, (protocol)) == 0 || \
								  CDPI_COMPARE_PROTOCOL_TO_BITMASK((flow)->excluded_protocol_bitmask, (protocol)) != 0 ) )

/* misc definitions */
#define CDPI_DEFAULT_MAX_TCP_RETRANSMISSION_WINDOW_SIZE 0x10000


/* TODO: rebuild all memory areas to have a more aligned memory block here */

/* DEFINITION OF MAX LINE NUMBERS FOR line parse algorithm */
#define CDPI_MAX_PARSE_LINES_PER_PACKET                        10

#define MAX_PACKET_COUNTER                                   65000
#define MAX_DEFAULT_PORTS                                        3

/**********************
 * detection features *
 **********************/
#define CDPI_SELECT_DETECTION_WITH_REAL_PROTOCOL ( 1 << 0 )

#define CDPI_DIRECTCONNECT_CONNECTION_IP_TICK_TIMEOUT          600
#define CDPI_IRC_CONNECTION_TIMEOUT                            120
#define CDPI_GNUTELLA_CONNECTION_TIMEOUT                       60
#define CDPI_BATTLEFIELD_CONNECTION_TIMEOUT                    60
#define CDPI_THUNDER_CONNECTION_TIMEOUT                        30
#define CDPI_RTSP_CONNECTION_TIMEOUT                           5
#define CDPI_TVANTS_CONNECTION_TIMEOUT                         5
#define CDPI_YAHOO_DETECT_HTTP_CONNECTIONS                     1
#define CDPI_YAHOO_LAN_VIDEO_TIMEOUT                           30
#define CDPI_ZATTOO_CONNECTION_TIMEOUT                         120
#define CDPI_ZATTOO_FLASH_TIMEOUT                              5
#define CDPI_JABBER_STUN_TIMEOUT                               30
#define CDPI_JABBER_FT_TIMEOUT				       5
#define CDPI_SOULSEEK_CONNECTION_IP_TICK_TIMEOUT               600

#ifdef CDPI_ENABLE_DEBUG_MESSAGES

#define CDPI_LOG(proto, mod, log_level, args...)		\
  {								\
    if(mod != NULL) {						\
      mod->cdpi_debug_print_file=__FILE__;                      \
      mod->cdpi_debug_print_function=__FUNCTION__;              \
      mod->cdpi_debug_print_line=__LINE__;                      \
      mod->cdpi_debug_printf(proto, mod, log_level, args);      \
    }								\
  }

#else							/* CDPI_ENABLE_DEBUG_MESSAGES */

#if defined(WIN32)
#define CDPI_LOG(...) {}
#else
#define CDPI_LOG(proto, mod, log_level, args...) {}
#endif

#endif							/* CDPI_ENABLE_DEBUG_MESSAGES */

/**
 * macro for getting the string len of a static string
 *
 * use it instead of strlen to avoid runtime calculations
 */
#define CDPI_STATICSTRING_LEN( s ) ( sizeof( s ) - 1 )

/** macro to compare 2 IPv6 addresses with each other to identify the "smaller" IPv6 address  */
#define CDPI_COMPARE_IPV6_ADDRESS_STRUCTS(x,y)  \
  ((((u_int64_t *)(x))[0]) < (((u_int64_t *)(y))[0]) || ( (((u_int64_t *)(x))[0]) == (((u_int64_t *)(y))[0]) && (((u_int64_t *)(x))[1]) < (((u_int64_t *)(y))[1])) )

#define CDPI_NUM_BITS              192//763 //jj, changed from 256 to 1024

#define CDPI_BITS /* 32 */ (sizeof(cdpi_cdpi_mask) * 8 /* number of bits in a byte */)        /* bits per mask */
#define howmanybits(x, y)   (((x)+((y)-1))/(y))


#define CDPI_SET(p, n)    ((p)->fds_bits[(n)/CDPI_BITS] |= (1 << (((u_int32_t)n) % CDPI_BITS)))
#define CDPI_CLR(p, n)    ((p)->fds_bits[(n)/CDPI_BITS] &= ~(1 << (((u_int32_t)n) % CDPI_BITS)))
#define CDPI_ISSET(p, n)  ((p)->fds_bits[(n)/CDPI_BITS] & (1 << (((u_int32_t)n) % CDPI_BITS)))
#define CDPI_ZERO(p)      memset((char *)(p), 0, sizeof(*(p)))
#define CDPI_ONE(p)       memset((char *)(p), 0xFF, sizeof(*(p)))

#define CDPI_NUM_FDS_BITS     howmanybits(CDPI_NUM_BITS, CDPI_BITS)

#define CDPI_PROTOCOL_BITMASK cdpi_protocol_bitmask_struct_t
  
#define CDPI_BITMASK_ADD(a,b)     CDPI_SET(&a,b)
#define CDPI_BITMASK_DEL(a,b)     CDPI_CLR(&a,b)
#define CDPI_BITMASK_RESET(a)     CDPI_ZERO(&a)
#define CDPI_BITMASK_SET_ALL(a)   CDPI_ONE(&a)
#define CDPI_BITMASK_SET(a, b)    { memcpy(&a, &b, sizeof(CDPI_PROTOCOL_BITMASK)); }

/* this is a very very tricky macro *g*,
  * the compiler will remove all shifts here if the protocol is static...
 */
#define CDPI_ADD_PROTOCOL_TO_BITMASK(bmask,value)     CDPI_SET(&bmask,value)
#define CDPI_DEL_PROTOCOL_FROM_BITMASK(bmask,value)   CDPI_CLR(&bmask,value)
#define CDPI_COMPARE_PROTOCOL_TO_BITMASK(bmask,value) CDPI_ISSET(&bmask,value)

#define CDPI_SAVE_AS_BITMASK(bmask,value)  { CDPI_ZERO(&bmask) ; CDPI_ADD_PROTOCOL_TO_BITMASK(bmask, value); }


#define cdpi_min(a,b)   ((a < b) ? a : b)
#define cdpi_max(a,b)   ((a > b) ? a : b)

//#define CDPI_PARSE_PACKET_LINE_INFO(cdpi_struct,flow,packet)
//                        if (packet->packet_lines_parsed_complete != 1) {
//			  cdpi_parse_packet_line_info(cdpi_struct,flow);
//                        }

#define CDPI_IPSEC_PROTOCOL_ESP	   50
#define CDPI_IPSEC_PROTOCOL_AH	   51
#define CDPI_GRE_PROTOCOL_TYPE	   0x2F
#define CDPI_ICMP_PROTOCOL_TYPE	   0x01
#define CDPI_IGMP_PROTOCOL_TYPE	   0x02
#define CDPI_EGP_PROTOCOL_TYPE	   0x08
#define CDPI_OSPF_PROTOCOL_TYPE	   0x59
#define CDPI_SCTP_PROTOCOL_TYPE	   132
#define CDPI_IPIP_PROTOCOL_TYPE    0x04
#define CDPI_ICMPV6_PROTOCOL_TYPE  0x3a

/* the get_uXX will return raw network packet bytes !! */
#define get_u_int8_t(X,O)  (*(u_int8_t *)(((u_int8_t *)X) + O))
#define get_u_int16_t(X,O)  (*(u_int16_t *)(((u_int8_t *)X) + O))
#define get_u_int32_t(X,O)  (*(u_int32_t *)(((u_int8_t *)X) + O))
#define get_u_int64_t(X,O)  (*(u_int64_t *)(((u_int8_t *)X) + O))

/* new definitions to get little endian from network bytes */
#define get_ul8(X,O) get_u_int8_t(X,O)


#if defined(__LITTLE_ENDIAN__)
#define get_l16(X,O)  get_u_int16_t(X,O)
#define get_l32(X,O)  get_u_int32_t(X,O)
#elif defined(__BIG_ENDIAN__)
/* convert the bytes from big to little endian */
#ifndef __KERNEL__
# define get_l16(X,O) bswap_16(get_u_int16_t(X,O))
# define get_l32(X,O) bswap_32(get_u_int32_t(X,O))
#else
# define get_l16(X,O) __cpu_to_le16(get_u_int16_t(X,O))
# define get_l32(X,O) __cpu_to_le32(get_u_int32_t(X,O))
#endif

#else

#error "__BYTE_ORDER MUST BE DEFINED !"

#endif							/* __BYTE_ORDER */

/* define memory callback function */
#define match_first_bytes(payload,st) (memcmp((payload),(st),(sizeof(st)-1))==0)

#endif /* __CDPI_DEFINE_INCLUDE_FILE__ */
