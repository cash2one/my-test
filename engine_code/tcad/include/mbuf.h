#ifndef __MBUF_H__
#define __MBUF_H__

#include <stdint.h>
/* #include "types.h" */
#include <atomic.h>
#include <common.h>
#include <mem.h>
#include "misc.h"

typedef struct {
	uint32_t sip[4]; 	//sip[0] Â±Ã­ÃŠÂ¾src   IPv4 ÂµÃ˜Ã–Â·
	uint32_t dip[4];	//dip[0] Â±Ã­ÃŠÂ¾dest  IPv4 ÂµÃ˜Ã–Â·
}ip_tuple_t;

/*
typedef struct {
	uint32_t sip;
	uint32_t dip;
}ipv4_tuple_t;

typedef struct {
    uint32_t sip[4];	//  uint64_t sip_high;    uint64_t sip_low;
    uint32_t dip[4];	//  uint64_t dip_high;	uint64_t dip_low;
}ipv6_tuple_t;
*/

#define MBUF_MAGIC_NUM 0xacdedeca

struct m_buf_header
{
	uint32_t magic;
	uint32_t lcore_id;
}; 

#define MBUF_HEADER_SIZE 64

#define MBUF_GROUP_SIZE 4096
/*
 * desc of packet buffer
 */
struct m_buf
{
	struct m_buf *next;
	struct m_buf *prev;
	
	void *pkt_addr;		/*start address of a packet, point to ip header*/
	
	uint8_t *data;
	uint8_t *transport_header;
	uint8_t *network_header;
	uint8_t *mac_header;
    void *psession_item;

	uint16_t pkt_type;
	uint16_t pkt_len;           /*form network header*/
	uint16_t mac_len;
	uint16_t data_len;

	unsigned int users; //atomic32_t users;
    uint32_t  session_hash;

	ip_tuple_t ip_tuple;

	uint16_t  sport;
   	uint16_t  dport;
   	uint8_t  protocol;
   	uint8_t  ip_version;
    uint8_t   tcp_f;
    uint8_t   res;
    
	uint16_t  ipdefrag_id;
	uint16_t  ipdefrag_flag;
	uint32_t  tcp_seq;

    uint16_t total_len;         /*form ether header*/
    uint8_t   flow_c_stat;   
	uint8_t   flow_c_stat_prev;   
	uint32_t  flow_id;
    
    uint32_t  eth_id;
    int streamid;
    /* ÇëÔÙ´ËÐÐÒÔºóÀ©³ä */
#ifdef UTAF_DDOS
	uint8_t   pkt_dir; /* only used by ddos */
#endif
	uint8_t   is_err_pkt; /* mark error ip-pkt*/
}cache_aligned;

#define MBUF_SIZE sizeof(struct m_buf)

extern struct m_buf *mbuf_alloc(void);  //wdb_calc222
extern void mbuf_free(struct m_buf *mbuf);


#define MBUF_ALLOC() mbuf_alloc()
//#define MBUF_FREE(_m_buf)  mbuf_free(_m_buf) 
#define MBUF_FREE(_m_buf)







/**
 * A macro that points to the start of the data in the mbuf.
 *
 * The returned pointer is cast to type t. Before using this
 * function, the user must ensure that m_headlen(m) is large enough to
 * read its data.
 *
 * @param m
 *   The packet mbuf.
 * @param t
 *   The type to cast the result into.
 */
#define pktmbuf_mtod(m, t) ((t)((m)->pkt_addr))

/**
 * A macro that returns the length of the packet.
 *
 * The value can be read or assigned.
 *
 * @param m
 *   The packet mbuf.
 */
#define pktmbuf_pkt_len(m) ((m)->pkt_len)




static inline int mbuf_move_data(struct m_buf *mbuf, uint32_t size)
{
	if(unlikely(mbuf->data_len < size))
	{
		return 0;
	}
	
	mbuf->data += size;
	mbuf->data_len -= size;

	return 1;
}



static inline void mbuf_reset_mac_header(struct m_buf *mbuf)
{
	mbuf->mac_header = mbuf->pkt_addr;
}

static inline unsigned char *mbuf_mac_header(const struct m_buf *mbuf)
{
	return mbuf->mac_header;
}


static inline unsigned char *mbuf_network_header(const struct m_buf *mbuf)
{
	return mbuf->network_header;
}


static inline void mbuf_reset_network_header(struct m_buf *mbuf)
{
	mbuf->network_header = mbuf->data;
}


static inline unsigned char *mbuf_transport_header(const struct m_buf *mbuf)
{
	return mbuf->transport_header;
}


static inline void mbuf_reset_transport_header(struct m_buf *m_buf)
{
	m_buf->transport_header = m_buf->data;
}







#endif
