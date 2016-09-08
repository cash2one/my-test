#ifndef __TCP4_H__
#define __TCP4_H__



#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
#define TH_SYNACK 0x12
#define	TH_PSHACK 0x18





/**
 * TCP Header
 */
struct tcp_hdr {
	uint16_t src_port;  /**< TCP source port. */
	uint16_t dst_port;  /**< TCP destination port. */
	uint32_t sent_seq;  /**< TX data sequence number. */
	uint32_t recv_ack;  /**< RX data acknowledgement sequence number. */
	uint8_t  data_off;  /**< Data offset. */
	uint8_t  tcp_flags; /**< TCP flags */
	uint16_t rx_win;    /**< RX flow control window. */
	uint16_t cksum;     /**< TCP checksum. */
	uint16_t tcp_urp;   /**< TCP urgent pointer, if any. */
} __attribute__((__packed__));

typedef struct tcp_header
{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned int seq_no;
    unsigned int ack_no;
    unsigned char reserved_1:4;
    unsigned char thl:4;//¨º¡Á2?3¡è?¨¨
    unsigned char flag:6;
    unsigned char reserved_2:2;
    unsigned short wnd_size;
    unsigned short chk_sum;
    unsigned short urgt_p;
} __attribute__((__packed__)) tcp_header_t;  //wdb_calc222


static inline struct tcp_hdr *tcp4_hdr(const struct m_buf *mbuf)
{
	return (struct tcp_hdr *)mbuf_transport_header(mbuf);
}





extern void tcp_v4_input(struct m_buf *mbuf);










#endif
