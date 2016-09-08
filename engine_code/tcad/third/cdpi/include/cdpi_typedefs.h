
#ifndef __CDPI_TYPEDEFS_FILE__
#define __CDPI_TYPEDEFS_FILE__


typedef struct node_t {
  char	  *key;
  struct node_t *left, *right;
} cdpi_node;

typedef u_int32_t cdpi_cdpi_mask;

typedef struct cdpi_protocol_bitmask_struct {
  cdpi_cdpi_mask  fds_bits[CDPI_NUM_FDS_BITS];
} cdpi_protocol_bitmask_struct_t;

#ifdef CDPI_DETECTION_SUPPORT_IPV6
struct cdpi_ip6_addr {
  union {
    u_int8_t u6_addr8[16];
    u_int16_t u6_addr16[8];
    u_int32_t u6_addr32[4];
    u_int64_t u6_addr64[2];
  } cdpi_v6_u;

#define cdpi_v6_addr		cdpi_v6_u.u6_addr8
#define cdpi_v6_addr16		cdpi_v6_u.u6_addr16
#define cdpi_v6_addr32		cdpi_v6_u.u6_addr32
#define cdpi_v6_addr64		cdpi_v6_u.u6_addr64
};

struct cdpi_ipv6hdr {
  /* use userspace and kernelspace compatible compile parameters */
#if defined(__LITTLE_ENDIAN__)
  u_int8_t priority:4, version:4;
#elif defined(__BIG_ENDIAN__)
  u_int8_t version:4, priority:4;
#else
# error "Byte order must be defined"
#endif

  u_int8_t flow_lbl[3];

  u_int16_t payload_len;
  u_int8_t nexthdr;
  u_int8_t hop_limit;

  struct cdpi_ip6_addr saddr;
  struct cdpi_ip6_addr daddr;
};
#endif							/* CDPI_DETECTION_SUPPORT_IPV6 */

typedef union {
  u_int32_t ipv4;
  u_int8_t ipv4_u_int8_t[4];
#ifdef CDPI_DETECTION_SUPPORT_IPV6
  struct cdpi_ip6_addr ipv6;
#endif
} cdpi_ip_addr_t;


/* ************************************************** */

struct cdpi_flow_tcp_struct {
  u_char prev_zmq_pkt[10];			//16bytes
  u_int16_t smtp_command_bitmask;
  u_int16_t pop_command_bitmask;
  u_int8_t tds_login_version;
  u_int8_t irc_stage;

  u_int8_t skype_packet_id;			//5bytes
  u_int8_t citrix_packet_id;
  u_int8_t lotus_notes_packet_id;
  u_int8_t teamviewer_stage;
  u_int8_t prev_zmq_pkt_len;

  u_int8_t irc_3a_counter:3;
  u_int8_t irc_stage2:5;

  u_int8_t irc_direction:2;
  u_int8_t winmx_stage:1;			// 0-1
  u_int8_t soulseek_stage:2;
   u_int8_t tds_stage:3;

  u_int8_t filetopia_stage:2;
  u_int8_t usenet_stage:2;
  u_int8_t imesh_stage:4;

  u_int8_t mms_stage:2;
  u_int8_t msn_stage:3;
  u_int8_t ssh_stage:3;

  u_int8_t http_stage:2;
  u_int8_t gnutella_stage:2;		//0-2
  u_int8_t vnc_stage:2;			// 0 - 3
  u_int8_t telnet_stage:2;			// 0 - 2

  u_int8_t ssl_stage:2/*, ssl_seen_client_cert:1, ssl_seen_server_cert:1*/; // 0 - 5
  u_int8_t postgres_stage:3;
  u_int8_t seen_syn:1;
  u_int8_t seen_syn_ack:1;
  u_int8_t seen_ack:1;

  u_int8_t icecast_stage:1;
  u_int8_t dofus_stage:1;
  u_int8_t fiesta_stage:2;
  u_int8_t wow_stage:2;
  u_int8_t veoh_tv_stage:2;

  u_int8_t shoutcast_stage:2;
  u_int8_t mail_pop_stage:2;
  u_int8_t mail_imap_stage:3;  
}
#if !defined(WIN32)
  __attribute__ ((__packed__));
#endif

/* ************************************************** */

struct cdpi_flow_udp_struct {
  u_int32_t snmp_msg_id;
  u_int32_t battlefield_stage:3;
  u_int32_t snmp_stage:2;
  u_int32_t ppstream_stage:3;		// 0-7
  u_int32_t halflife2_stage:2;		// 0 - 2
  u_int32_t tftp_stage:1;
  u_int32_t aimini_stage:5;
  u_int32_t xbox_stage:1;
  u_int32_t wsus_stage:1;
  u_int8_t skype_packet_id;
  u_int8_t teamviewer_stage;
}
#if !defined(WIN32)
  __attribute__ ((__packed__))
#endif
  ;

/* ************************************************** */

typedef struct cdpi_int_one_line_struct {
  const u_int8_t *ptr;
  u_int16_t len;
} cdpi_int_one_line_struct_t;

typedef struct cdpi_packet_struct {
  const struct cdpi_iphdr *iph;			//8bytes
#ifdef CDPI_DETECTION_SUPPORT_IPV6
  const struct cdpi_ipv6hdr *iphv6;		//8bytes
#endif
  const struct cdpi_tcphdr *tcp;		//8bytes
  const struct cdpi_udphdr *udp;		//8bytes
  const u_int8_t *payload;			//8bytes

  u_int16_t detected_protocol_stack[CDPI_PROTOCOL_HISTORY_SIZE];

  u_int16_t l3_packet_len;
  u_int16_t l4_packet_len;
  u_int16_t payload_packet_len;
  u_int16_t actual_payload_len;
  u_int16_t num_retried_bytes;

  u_int8_t l4_protocol;

  u_int8_t ssl_certificate_detected:4;
  u_int8_t ssl_certificate_num_checks:4;
  u_int8_t tcp_retransmission:1;
  u_int8_t empty_line_position_set:1;
  u_int8_t packet_direction:1;
} cdpi_packet_struct_t;

struct cdpi_detection_module_struct;
struct cdpi_flow_struct;

typedef struct cdpi_call_function_struct {
  CDPI_PROTOCOL_BITMASK detection_bitmask;
  CDPI_PROTOCOL_BITMASK excluded_protocol_bitmask;
  CDPI_SELECTION_BITMASK_PROTOCOL_SIZE cdpi_selection_bitmask;
  //void (*func) (struct cdpi_detection_module_struct *, struct cdpi_flow_struct *flow);
  void (*func) (u_int16_t thread_id, struct cdpi_flow_struct *flow);
  	
  u_int8_t detection_feature;
} cdpi_call_function_struct_t;

/* ntop extensions */
typedef struct cdpi_proto_defaults {
  char *protoName;
  u_int16_t protoId, protoIdx;
  void (*func) (u_int16_t thread_id, struct cdpi_flow_struct *flow);
} cdpi_proto_defaults_t;

typedef struct cdpi_default_ports_tree_node {
  cdpi_proto_defaults_t *proto;
  u_int16_t default_port;
} cdpi_default_ports_tree_node_t;


typedef struct cdpi_detection_module_struct {
  CDPI_PROTOCOL_BITMASK detection_bitmask;
  CDPI_PROTOCOL_BITMASK generic_http_packet_bitmask;


#ifdef CDPI_ENABLE_DEBUG_MESSAGES
  void *user_data;
#endif

  //jj-start,
  //change static to dynamic
  u_int16_t callback_buffer_size;
  u_int16_t callback_buffer_size_tcp_no_payload;
  u_int16_t callback_buffer_size_tcp_payload;
  u_int16_t callback_buffer_size_udp;
  u_int16_t callback_buffer_size_non_tcp_udp;

  struct cdpi_call_function_struct callback_buffer[CDPI_NON_HTTP];
  u_int16_t callback_buffer_tcp_no_payload[CDPI_NON_HTTP];
  u_int16_t callback_buffer_tcp_payload[CDPI_NON_HTTP];
  u_int16_t callback_buffer_udp[CDPI_NON_HTTP];
  u_int16_t callback_buffer_non_tcp_udp[CDPI_NON_HTTP];
  //jj-end

  cdpi_default_ports_tree_node_t *tcpRoot, *udpRoot;

#ifdef CDPI_ENABLE_DEBUG_MESSAGES
  /* debug callback, only set when debug is used */
  cdpi_debug_function_ptr cdpi_debug_printf;
  const char *cdpi_debug_print_file;
  const char *cdpi_debug_print_function;
  u_int32_t cdpi_debug_print_line;
#endif
  /* misc parameters */
  u_int32_t tcp_max_retransmission_window_size;

#ifdef CDPI_ENABLE_DEBUG_MESSAGES
#define CDPI_IP_STRING_SIZE 40
  char ip_string[CDPI_IP_STRING_SIZE];
#endif
  u_int8_t ip_version_limit;
  /* ********************* */
  cdpi_proto_defaults_t proto_defaults[CDPI_MAX_SUPPORTED_PROTOCOLS];
} cdpi_detection_module_struct_t;

typedef struct cdpi_flow_struct {
  CDPI_PROTOCOL_BITMASK excluded_protocol_bitmask;		//24bytes
  
  /* internal structures to save functions calls */
  struct cdpi_packet_struct packet;				//56bytes

  /* tcp sequence number connection tracking */
  u_int32_t next_tcp_seq_nr[2];					//8bytes

  u_int16_t guessed_protocol_id;				//2bytes
  u_int16_t detected_protocol_stack[CDPI_PROTOCOL_HISTORY_SIZE];//2bytes

  u_int16_t packet_counter;					//6bytes
  u_int8_t redis_s2d_first_char;
  u_int8_t redis_d2s_first_char;
  u_int8_t bittorrent_stage;		// can be 0-255
  u_int8_t system;

  union {							//32bytes
    struct cdpi_flow_tcp_struct tcp;				//29bytes
    struct cdpi_flow_udp_struct udp;				//9bytes
  } l4;

  union {							//1bytes
    struct {
      u_int8_t num_queries;
    } dns;
  } protos;

  /* init parameter, internal used to set up timestamp,... */
  u_int8_t init_finished:1;					//7bytes
  u_int8_t setup_packet_direction:1;
  u_int8_t protocol_id_already_guessed:1;
  u_int8_t directconnect_stage:2;	// 0-1
  u_int8_t http_detected:1;
  u_int8_t rtsprdt_stage:2;
  u_int8_t zattoo_stage:3;
  u_int8_t qq_stage:3;
  u_int8_t thunder_stage:2;		// 0-3
  u_int8_t socks5_stage:2;	// 0-3
  u_int8_t socks4_stage:2;	// 0-3
  u_int8_t edonkey_stage:2;	// 0-3
//  u_int8_t edonkey_udp_stage:2;
  u_int8_t ftp_control_stage:2;
  u_int8_t ftp_data_stage:2;
  u_int8_t rtmp_stage:2;
  u_int8_t pando_stage:3;
  u_int8_t florensia_stage:1;
  u_int8_t steam_stage:3;
  u_int8_t steam_stage1:3;			// 0 - 4
  u_int8_t steam_stage2:2;			// 0 - 2
  u_int8_t steam_stage3:2;			// 0 - 2
  u_int8_t pplive_stage1:3;			// 0-6
  u_int8_t pplive_stage2:2;			// 0-2
  u_int8_t protocol_port_already_detect:1;			// 0-1
  u_int8_t pplive_stage3:2;			// 0-2

} cdpi_flow_struct_t;


typedef enum {
  CDPI_REAL_PROTOCOL = 0,
  CDPI_CORRELATED_PROTOCOL = 1
} cdpi_protocol_type_t;


typedef enum {
  CDPI_LOG_ERROR,
  CDPI_LOG_TRACE,
  CDPI_LOG_DEBUG
} cdpi_log_level_t;

#endif/* __CDPI_TYPEDEFS_FILE__ */
