
#ifndef __CDPI_PUBLIC_FUNCTIONS_H__
#define __CDPI_PUBLIC_FUNCTIONS_H__

#include "cdpi_main.h"
#include "yara.h"
#include <pcap.h>

//#define USE_CDPI_FLOW
#define USE_CDPI_CUSTOM_FEASURES
#define USE_CDPI_MOBILE_FEASURES_AS_ID
//#define USE_CDPI_MOBILE_FEASURES
#define USE_CDPI_PAYLOAD_FEASURES
#define USE_DEFAULT_PORTS_FEASURES
#define USE_CDPI_MULTI_THREAD
//#define USE_CDPI_LOCAL_DEBUG

#define PAYLOAD_MATCH_PACKS     10
#define PARSE_PACKS_NUM			10

#define MAX_NUM_READER_THREADS 		40
#define RULE_FILE_NUM      		3
#define NUM_MOBILE_BUFFER		65536

#ifdef USE_CDPI_FLOW
/*****************************locol test**********************************/
//#define ENABLE_FREE_FLOWS
#define IDLE_SCAN_PERIOD        4000 /* 4 msec */
#define MAX_IDLE_TIME           10000 /* 10 msec */
#define IDLE_SCAN_BUDGET        4096  
#define IDLE_SCAN_RATE			0.8
#define MAX_CDPI_FLOWS			30000000//30000000//100000
#define NUM_ROOTS				1048576//2048//1048576

struct thread_stats {
  u_int64_t raw_packet_count;
  u_int64_t ip_packet_count;
  u_int64_t total_wire_bytes, total_ip_bytes, total_discarded_bytes;
  u_int64_t protocol_counter[CDPI_MAX_SUPPORTED_PROTOCOLS + 1];
  u_int64_t protocol_counter_bytes[CDPI_MAX_SUPPORTED_PROTOCOLS + 1];
  u_int32_t protocol_flows[CDPI_MAX_SUPPORTED_PROTOCOLS + 1];
  u_int32_t cdpi_flow_count;
  u_int64_t tcp_count, udp_count;
#ifdef USE_CDPI_MOBILE_FEASURES
  u_int32_t mobile_flow_count;
  u_int64_t mobile_byte_count;
  u_int64_t mobile_packet_count;
  u_int32_t ios_flow_count;
  u_int64_t ios_byte_count;
  u_int64_t ios_packet_count;
  u_int32_t android_flow_count;
  u_int64_t android_byte_count;
  u_int64_t android_packet_count;
#endif
};
struct reader_thread {
  struct cdpi_detection_module_struct *cdpi_struct;

  void *cdpi_flows_root[NUM_ROOTS];

  char _pcap_error_buffer[PCAP_ERRBUF_SIZE];
  pcap_t *_pcap_handle;
  u_int64_t last_time;
  u_int64_t last_idle_scan_time;

  pthread_t pthread;
  int _pcap_datalink_type;
  struct thread_stats stats;

  u_int16_t parsed_lines;
  u_int16_t empty_line_position;
  struct cdpi_int_one_line_struct line[CDPI_MAX_PARSE_LINES_PER_PACKET];
  struct cdpi_int_one_line_struct host_line;
  struct cdpi_int_one_line_struct referer_line;
  struct cdpi_int_one_line_struct agent_line;
  struct cdpi_int_one_line_struct server_line;

#ifdef USE_CDPI_MOBILE_FEASURES
  u_int16_t mobile_inf_count;
  u_int64_t mobile_inf_ip[NUM_MOBILE_BUFFER];
  u_int16_t mobile_inf_port[NUM_MOBILE_BUFFER];
  u_int8_t mobile_inf_type[NUM_MOBILE_BUFFER];
#endif
};
// flow tracking
typedef struct cdpi_flow {
  u_int32_t lower_ip;
  u_int32_t upper_ip;
  u_int16_t lower_port;
  u_int16_t upper_port;
  u_int8_t  protocol;

  //u_int8_t isOccupy;

#ifdef USE_CDPI_MOBILE_FEASURES
  u_int8_t  system;
#endif

  struct cdpi_flow_struct *cdpi_flow;
  u_int32_t packets, bytes;
  u_int32_t detected_protocol;
  u_int64_t last_seen;

//  u_int32_t srcBytes, dstBytes;
//  u_int32_t srcIP;
//  u_int8_t cnt, packetsDirections[10];
} cdpi_flow_t;
/*************************************************************************/
#else
// flow tracking
struct reader_thread {
  struct cdpi_detection_module_struct *cdpi_struct;

  u_int16_t parsed_lines;
  u_int16_t empty_line_position;
  struct cdpi_int_one_line_struct line[CDPI_MAX_PARSE_LINES_PER_PACKET];
  struct cdpi_int_one_line_struct host_line;
  struct cdpi_int_one_line_struct referer_line;
  struct cdpi_int_one_line_struct agent_line;
  struct cdpi_int_one_line_struct server_line;

#ifdef USE_CDPI_MOBILE_FEASURES
  u_int64_t mobile_inf_ip[NUM_MOBILE_BUFFER];
  u_int16_t mobile_inf_port[NUM_MOBILE_BUFFER];
  u_int16_t mobile_inf_count;
  u_int8_t mobile_inf_type[NUM_MOBILE_BUFFER];
#endif
};
typedef struct cdpi_flow {
  struct cdpi_flow_struct cdpi_flow;
#ifdef USE_CDPI_MULTI_THREAD
  u_int8_t isOccupy;
#endif
#ifdef USE_CDPI_MOBILE_FEASURES
  u_int8_t system;
#endif
  u_int16_t detected_protocol;
} cdpi_flow_t;
#endif

YR_RULES* rules[RULE_FILE_NUM];
struct reader_thread cdpi_thread_info[MAX_NUM_READER_THREADS];


#define GTP_U_V1_PORT        2152
#ifndef ETH_P_IP
#define ETH_P_IP 0x0800
#endif

#ifdef __cplusplus
extern "C" {
#endif

int cdpi_init(const char *path, u_int16_t thread_num);
int cdpi_close(void);
#ifdef USE_CDPI_CUSTOM_FEASURES
void cdpi_print_custom_features(void);
int cdpi_custom_features_operate(int type, char* features, int len);//1: add; 0: delete. only thread 0 operate. ok return id; error return 0.
#endif
#ifdef USE_CDPI_FLOW
unsigned int cdpi_parse(u_int16_t thread_id, const u_int64_t time, const struct cdpi_iphdr *iph, struct cdpi_ip6_hdr *iph6, u_int16_t ipsize, u_int16_t rawsize, struct cdpi_flow **flow);
#else
unsigned int cdpi_parse(u_int16_t thread_id, const struct cdpi_iphdr *iph, struct cdpi_ip6_hdr *iph6, u_int16_t ipsize, struct cdpi_flow *flow);
#endif

typedef void (*cdpi_debug_function_ptr) (u_int32_t protocol,
					   void *module_struct, cdpi_log_level_t log_level, const char *format, ...);

  /**
   * This function returns the size of the flow struct
   * @return the size of the flow struct
   */
  u_int32_t cdpi_detection_get_sizeof_cdpi_flow_struct(void);

  /**
   * This function returns the size of the id struct
   * @return the size of the id struct
   */
  u_int32_t cdpi_detection_get_sizeof_cdpi_id_struct(void);


  /* Public malloc/free */
  void* cdpi_malloc(unsigned long size);
  void* cdpi_calloc(unsigned long count, unsigned long size);
  void  cdpi_free(void *ptr);
  void *cdpi_realloc(void *ptr, size_t old_size, size_t new_size);
  char *cdpi_strdup(const char *s);
 /*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
  char* cdpi_strnstr(const char *s, const char *find, size_t slen);

  /**
   * This function returns a new initialized detection module.
   * @param ticks_per_second the timestamp resolution per second (like 1000 for millisecond resolution)
   * @param cdpi_malloc function pointer to a memory allocator
   * @param cdpi_debug_printf a function pointer to a debug output function, use NULL in productive envionments
   * @return the initialized detection module
   */
  struct cdpi_detection_module_struct *cdpi_init_detection_module(u_int32_t ticks_per_second,
								  void* (*__cdpi_malloc)(unsigned long size),
								  void  (*__cdpi_free)(void *ptr),
								  cdpi_debug_function_ptr cdpi_debug_printf);

  /**
   * This function enables cache support in cDPI used for some protocol such as Skype
   * @param cache host name
   * @param cache port
   */
  void cdpi_enable_cache(struct cdpi_detection_module_struct *cdpi_mod, char* host, u_int port);

  /**
   * This function destroys the detection module
   * @param cdpi_struct the to clearing detection module
   * @param cdpi_free function pointer to a memory free function
   */
  void
  cdpi_exit_detection_module(struct cdpi_detection_module_struct
			     *cdpi_struct, void (*cdpi_free) (void *ptr));

  /**
   * This function sets the protocol bitmask2
   * @param cdpi_struct the detection module
   * @param detection_bitmask the protocol bitmask
   */
  void
  cdpi_set_protocol_detection_bitmask2(struct cdpi_detection_module_struct *cdpi_struct,
				       const CDPI_PROTOCOL_BITMASK * detection_bitmask);
  /**
   * This function will processes one packet and returns the ID of the detected protocol.
   * This is the main packet processing function.
   *
   * @param cdpi_struct the detection module
   * @param flow void pointer to the connection state machine
   * @param packet the packet as unsigned char pointer with the length of packetlen. the pointer must point to the Layer 3 (IP header)
   * @param packetlen the length of the packet
   * @param current_tick the current timestamp for the packet
   * @param src void pointer to the source subscriber state machine
   * @param dst void pointer to the destination subscriber state machine
   * @return returns the detected ID of the protocol
   */
  unsigned int
  cdpi_detection_process_packet(u_int16_t thread_id,
				struct cdpi_flow *flow,
				const unsigned char *packet,
				const unsigned short packetlen);

#define CDPI_DETECTION_ONLY_IPV4 ( 1 << 0 )
#define CDPI_DETECTION_ONLY_IPV6 ( 1 << 1 )

  /**
   * query the pointer to the layer 4 packet
   *
   * @param l3 pointer to the layer 3 data
   * @param l3_len length of the layer 3 data
   * @param l4_return filled with the pointer the layer 4 data if return value == 0, undefined otherwise
   * @param l4_len_return filled with the length of the layer 4 data if return value == 0, undefined otherwise
   * @param l4_protocol_return filled with the protocol of the layer 4 data if return value == 0, undefined otherwise
   * @param flags limit operation on ipv4 or ipv6 packets, possible values are CDPI_DETECTION_ONLY_IPV4 or CDPI_DETECTION_ONLY_IPV6; 0 means any
   * @return 0 if correct layer 4 data could be found, != 0 otherwise
   */
  u_int8_t cdpi_detection_get_l4(const u_int8_t * l3, u_int16_t l3_len, const u_int8_t ** l4_return, u_int16_t * l4_len_return,
				 u_int8_t * l4_protocol_return, u_int32_t flags);
  /**
   * returns the real protocol for the flow of the last packet given to the detection.
   * if no real protocol could be found, the unknown protocol will be returned.
   *
   * @param cdpi_struct the detection module
   * @return the protocol id of the last real protocol found in the protocol history of the flow
   */
  u_int16_t cdpi_detection_get_real_protocol_of_flow(struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow);

  /**
   * returns true if the protocol history of the flow of the last packet given to the detection
   * contains the given protocol.
   *
   * @param cdpi_struct the detection module
   * @return 1 if protocol has been found, 0 otherwise
   */
  u_int8_t cdpi_detection_flow_protocol_history_contains_protocol(struct cdpi_detection_module_struct *cdpi_struct,
								  struct cdpi_flow_struct *flow,
								  u_int16_t protocol_id);
  unsigned int cdpi_find_port_based_protocol(struct cdpi_detection_module_struct *cdpi_struct,
					     u_int8_t proto, u_int32_t shost, u_int16_t sport, u_int32_t dhost, u_int16_t dport);
  unsigned int cdpi_guess_undetected_protocol(struct cdpi_detection_module_struct *cdpi_struct,
					      u_int8_t proto, u_int32_t shost, u_int16_t sport, u_int32_t dhost, u_int16_t dport);
  int cdpi_match_string_subprotocol(struct cdpi_detection_module_struct *cdpi_struct,
				    struct cdpi_flow_struct *flow, char *string_to_match, u_int string_to_match_len);
  int cdpi_match_content_subprotocol(struct cdpi_detection_module_struct *cdpi_struct,
				     struct cdpi_flow_struct *flow,
				     char *string_to_match, u_int string_to_match_len);
  char* cdpi_get_proto_name(struct cdpi_detection_module_struct *mod, u_int16_t proto_id);
  int cdpi_get_protocol_id(struct cdpi_detection_module_struct *cdpi_mod, char *proto);
  void cdpi_dump_protocols(struct cdpi_detection_module_struct *mod);
  int matchStringProtocol(struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow,
			  char *string_to_match, u_int string_to_match_len);

  int cdpi_load_protocols_file(struct cdpi_detection_module_struct *cdpi_mod, char* path);
//  u_int cdpi_get_num_supported_protocols(struct cdpi_detection_module_struct *cdpi_mod);
  char* cdpi_revision(void);
  void cdpi_set_automa(struct cdpi_detection_module_struct *cdpi_struct, void* automa);

#define ADD_TO_DETECTION_BITMASK             1
#define NO_ADD_TO_DETECTION_BITMASK          0
#define SAVE_DETECTION_BITMASK_AS_UNKNOWN    1
#define NO_SAVE_DETECTION_BITMASK_AS_UNKNOWN 0

/**
   * This function sets a single protocol bitmask
   * @param label Protocol name
   * @param cdpi_struct the detection module
   * @param detection_bitmask the protocol bitmask
   * @param idx the index of the callback_buffer
   * @param func void function point of the protocol search
   * @param cdpi_selection_bitmask the protocol selected bitmask
   * @param b_save_bitmask_unknow set true if you want save the detection bitmask as unknow
   * @param b_add_detection_bitmask set true if you want add the protocol bitmask to the detection bitmask
   * NB: this function does not increment the index of the callback_buffer
   */
   void cdpi_set_bitmask_protocol_detection(char * label, struct cdpi_detection_module_struct *cdpi_struct,
    const CDPI_PROTOCOL_BITMASK * detection_bitmask,
    const u_int32_t idx,
    u_int16_t cdpi_protocol_id,
    void (*func) (u_int16_t thread_id, struct cdpi_flow_struct *flow),
    const CDPI_SELECTION_BITMASK_PROTOCOL_SIZE cdpi_selection_bitmask,
    u_int8_t b_save_bitmask_unknow,
    u_int8_t b_add_detection_bitmask);

#ifdef __cplusplus
}
#endif
#endif
