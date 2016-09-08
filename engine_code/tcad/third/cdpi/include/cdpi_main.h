
#ifndef __CDPI_MAIN_INCLUDE_FILE__
#define __CDPI_MAIN_INCLUDE_FILE__

#ifndef __KERNEL__
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#endif

#ifndef WIN32
#ifndef __KERNEL__
#include <sys/time.h>
#endif

#if !defined __APPLE__ && !defined __FreeBSD__ && !defined __NetBSD__ && !defined __OpenBSD__

#ifndef __KERNEL__
#include <endian.h>
#include <byteswap.h>
#else
#include <asm/byteorder.h>
#include <linux/ctype.h>
#endif

#endif

#include <yara.h>

/* default includes */

#ifndef __KERNEL__
#include <sys/param.h>
#include <limits.h>
#endif
#endif

#include "cdpi_win32.h"
#include "cdpi_unix.h"
#include "cdpi_define.h"
#include "cdpi_protocol_ids.h"
#include "cdpi_typedefs.h"
#include "cdpi_protocols.h"

int callback(int message, void* message_data, void* user_data);
int callbackMobile(int message, void* message_data, void* user_data);
void *cdpi_tdelete(const void * __restrict, void ** __restrict,
		   int (*)(const void *, const void *));
void *cdpi_tfind(const void *, void *, int (*)(const void *, const void *));
void *cdpi_tsearch(const void *, void**, int (*)(const void *, const void *));
//void cdpi_twalk(const void *, void (*)(const void *, cdpi_VISIT, int, void*), void *user_data);
void cdpi_twalk(const void *, void (*)(const void *, int, void*), void *user_data);
void cdpi_tdestroy(void *vrootp, void (*freefct)(void *));

int CDPI_BITMASK_COMPARE(CDPI_PROTOCOL_BITMASK a, CDPI_PROTOCOL_BITMASK b);
int CDPI_BITMASK_IS_EMPTY(CDPI_PROTOCOL_BITMASK a);
void CDPI_DUMP_BITMASK(CDPI_PROTOCOL_BITMASK a);


extern u_int8_t cdpi_net_match(u_int32_t ip_to_check,
			       u_int32_t net,
			       u_int32_t num_bits);

extern u_int8_t cdpi_ips_match(u_int32_t src, u_int32_t dst,
			       u_int32_t net, u_int32_t num_bits);

extern char* cdpi_strnstr(const char *s, const char *find, size_t slen);

u_int16_t ntohs_cdpi_bytestream_to_number(const u_int8_t * str, u_int16_t max_chars_to_read, u_int16_t * bytes_read);

u_int32_t cdpi_bytestream_to_number(const u_int8_t * str, u_int16_t max_chars_to_read, u_int16_t * bytes_read);
u_int64_t cdpi_bytestream_to_number64(const u_int8_t * str, u_int16_t max_chars_to_read, u_int16_t * bytes_read);
u_int32_t cdpi_bytestream_dec_or_hex_to_number(const u_int8_t * str, u_int16_t max_chars_to_read, u_int16_t * bytes_read);
u_int64_t cdpi_bytestream_dec_or_hex_to_number64(const u_int8_t * str, u_int16_t max_chars_to_read, u_int16_t * bytes_read);
u_int32_t cdpi_bytestream_to_ipv4(const u_int8_t * str, u_int16_t max_chars_to_read, u_int16_t * bytes_read);


void cdpi_int_add_connection(struct cdpi_detection_module_struct *cdpi_struct,                             
                             struct cdpi_flow_struct *flow,
                             u_int16_t detected_protocol, cdpi_protocol_type_t protocol_type);


/* function to parse a packet which has line based information into a line based structure
 * this function will also set some well known line pointers like:
 *  - host, user agent, empty line,....
 */
extern void cdpi_parse_packet_line_info(u_int16_t thread_id, struct cdpi_flow_struct *flow);
extern void cdpi_parse_packet_line_info_unix(u_int16_t thread_id, struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow);
extern u_int16_t cdpi_check_for_email_address(struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow, u_int16_t counter);
extern void cdpi_int_change_packet_protocol(struct cdpi_detection_module_struct *cdpi_struct,
					    struct cdpi_flow_struct *flow,
					    u_int16_t detected_protocol, cdpi_protocol_type_t protocol_type);
extern void cdpi_int_change_protocol(struct cdpi_detection_module_struct *cdpi_struct,
				     struct cdpi_flow_struct *flow,
				     u_int16_t detected_protocol,
				     cdpi_protocol_type_t protocol_type);
extern void cdpi_set_proto_defaults(struct cdpi_detection_module_struct *cdpi_mod,
				    u_int16_t protoId, char *protoName,
				    u_int16_t *tcpDefPorts, u_int16_t *udpDefPorts);
extern void cdpi_int_reset_packet_protocol(struct cdpi_packet_struct *packet);
extern void cdpi_int_reset_protocol(struct cdpi_flow_struct *flow);
extern int cdpi_packet_src_ip_eql(const struct cdpi_packet_struct *packet, const cdpi_ip_addr_t * ip);
extern int cdpi_packet_dst_ip_eql(const struct cdpi_packet_struct *packet, const cdpi_ip_addr_t * ip);
extern void cdpi_packet_src_ip_get(const struct cdpi_packet_struct *packet, cdpi_ip_addr_t * ip);
extern void cdpi_packet_dst_ip_get(const struct cdpi_packet_struct *packet, cdpi_ip_addr_t * ip);
extern char *cdpi_get_ip_string(struct cdpi_detection_module_struct *cdpi_struct, const cdpi_ip_addr_t * ip);
extern char *cdpi_get_packet_src_ip_string(struct cdpi_detection_module_struct *cdpi_struct,
					   const struct cdpi_packet_struct *packet);
extern char* cdpi_get_proto_by_id(struct cdpi_detection_module_struct *cdpi_mod, u_int id);

extern u_int8_t cdpi_net_match(u_int32_t ip_to_check,
			       u_int32_t net,
			       u_int32_t num_bits);

extern u_int8_t cdpi_ips_match(u_int32_t src, u_int32_t dst,
			       u_int32_t net, u_int32_t num_bits);

extern char* cdpi_strnstr(const char *s, const char *find, size_t slen);

#ifdef CDPI_ENABLE_DEBUG_MESSAGES
  void cdpi_debug_get_last_log_function_line(struct cdpi_detection_module_struct *cdpi_struct,
					     const char **file, const char **func, u_int32_t * line);
#endif

#include "cdpi_api.h"

#endif							/* __CDPI_MAIN_INCLUDE_FILE__ */
