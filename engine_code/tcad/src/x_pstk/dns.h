#ifndef __DNS_H__
#define __DNS_H__


#include "mbuf.h"


#define MAX_REQUEST_NAME_SIZE 256


#define NOERROR		0		/* no error */
#define FORMERR		1		/* format error */
#define SERVFAIL	2		/* server failure */
#define NXDOMAIN	3		/* non existent domain */
#define NOTIMP		4		/* not implemented */
#define REFUSED		5		/* query refused */



#define DNS_NONE     0
#define DNS_REQUEST  1
#define DNS_RESPONSE 2




/*
 * Macros for subfields of flag fields.
 */
#define DNS_QR(np)	((np)->flags1 & 0x80)		/* response flag */
#define DNS_OPCODE(np)	((((np)->flags1) >> 3) & 0xF)	/* purpose of message */
#define DNS_AA(np)	((np)->flags1 & 0x04)		/* authoritative answer */
#define DNS_TC(np)	((np)->flags1 & 0x02)		/* truncated message */
#define DNS_RD(np)	((np)->flags1 & 0x01)		/* recursion desired */

#define DNS_RA(np)	((np)->flags2 & 0x80)	/* recursion available */
#define DNS_AD(np)	((np)->flags2 & 0x20)	/* authentic data from named */
#define DNS_CD(np)	((np)->flags2 & 0x10)	/* checking disabled by resolver */
#define DNS_RCODE(np)	((np)->flags2 & 0xF)	/* response code */




typedef struct
{
	uint16_t id;		/* query identification number */
	uint8_t  flags1;	/* first byte of flags */
	uint8_t  flags2;	/* second byte of flags */
	uint16_t qdcount;	/* number of question entries */
	uint16_t ancount;	/* number of answer entries */
	uint16_t nscount;	/* number of authority entries */
	uint16_t arcount;	/* number of resource entries */
}DNS_HEADER;




struct dns_info
{
	uint8_t service_status;
	uint8_t response_code;
	uint8_t dns_status;
    uint16_t request_id;
#if 0    
	char request_domain[MAX_REQUEST_NAME_SIZE];
	uint32_t response_delay;
    uint32_t request_count;
	uint64_t request_cycle;
	uint64_t response_cycle;
	time_t start_time;
    time_t end_time;
#endif
};


extern void dns_parser(struct m_buf *mbuf);





#endif
