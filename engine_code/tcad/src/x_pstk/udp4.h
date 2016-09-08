#ifndef __UDP4_H__
#define __UDP4_H__


#include "mbuf.h"

struct udp_hdr {
	uint16_t src_port;    /**< UDP source port. */
	uint16_t dst_port;    /**< UDP destination port. */
	uint16_t dgram_len;   /**< UDP datagram length */
	uint16_t dgram_cksum; /**< UDP datagram checksum */
} __attribute__((__packed__));


static inline struct udp_hdr *udp4_hdr(const struct m_buf *mbuf)
{
	return (struct udp_hdr *)mbuf_transport_header(mbuf);
}




struct app_port
{
	void (*handler)(struct m_buf *);
};





#define PORT_DNS   53


#define MAX_PORT_OVER_UDP 65536






extern void udp_v4_input(struct m_buf *mbuf);
extern uint32_t udp_init(void);
extern uint32_t udp4_add_appport(const struct app_port *app, unsigned int port);




#endif
