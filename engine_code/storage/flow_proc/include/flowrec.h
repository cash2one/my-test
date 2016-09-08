#ifndef __FLOWREC_H__
#define __FLOWREC_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

// the data cell stored in a flowdb, 52 bytes each.
// everything is in host byte order
#pragma pack(1)
typedef struct flowrec {
	uint32_t rip;		// 0-3: router IP
	uint32_t sip;		// 4-7: source IP
	uint32_t dip;		// 8-11: destination IP
	uint32_t nip;		// 12-15: next hop router IP
	uint16_t input;		// 16-17: incoming SNMP interface
	uint16_t output;	// 18-19: outgoing SNMP interface
	uint32_t packets;	// 20-23: count of packets
	uint32_t bytes;		// 24-27 count of bytes
	uint32_t first;		// 28-31: time of the first packet in the flow
	uint32_t last;		// 32-35: time of the last packet in the flow
	uint16_t sport;		// 36-37: source port
	uint16_t dport;		// 38-39: destination port
	uint8_t  res;		// 40: reserved
	uint8_t  flags;		// 41: TCP flags
	uint8_t  proto;		// 42: transport protocol
	uint8_t  tos;		// 43: type of service
	uint16_t sas;		// 44-45: source AS
	uint16_t das;		// 46-47: destination AS
#if 0
	uint32_t sample;	// 48-51: sample rate
#endif
} flowrec_t;
#pragma pack()

/* the buffers supplied to the record handler are
 * reused after the handler returns. if a record has to
 * be queued up for later processing, it must be copied
 * to a new permanent buffer. this func helps this. */
static inline flowrec_t * flowrec_dup (
				flowrec_t * rec
				)
{
	flowrec_t * dup = (flowrec_t *) malloc (sizeof (*rec));
	if (dup != NULL)
		memcpy (dup, rec, sizeof (*rec));
	return dup;
}

static inline void flowrec_free (
				flowrec_t * rec
				)
{
	free (rec);
}

#ifdef __cplusplus
}
#endif

#endif	/* __FLOWREC_H__ */

