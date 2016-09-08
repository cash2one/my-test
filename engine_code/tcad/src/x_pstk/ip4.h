#ifndef __IP4_H__
#define __IP4_H__

#include "mbuf.h"
#include "ether.h"

#define IPV4_VERSION 4



/* Fragment Offset * Flags. */
#define	IPV4_HDR_DF_SHIFT	14
#define	IPV4_HDR_MF_SHIFT	13
#define	IPV4_HDR_FO_SHIFT	3

#define	IPV4_HDR_DF_FLAG	(1 << IPV4_HDR_DF_SHIFT)
#define	IPV4_HDR_MF_FLAG	(1 << IPV4_HDR_MF_SHIFT)

#define	IPV4_HDR_OFFSET_MASK	((1 << IPV4_HDR_MF_SHIFT) - 1)

#define	IPV4_HDR_OFFSET_UNITS	8



enum
{
	IS_OTHER_IP_DEFRAG = 0,
	IS_IP_PKT,
	IS_FIRST_IP_DEFRAG	
};



/**
 * IPv4 Header
 */
struct ipv4_hdr {
	uint8_t  version_ihl;		/**< version and header length */
	uint8_t  type_of_service;	/**< type of service */
	uint16_t total_length;		/**< length of packet */
	uint16_t packet_id;		/**< packet ID */
	uint16_t fragment_offset;	/**< fragmentation offset */
	uint8_t  time_to_live;		/**< time to live */
	uint8_t  next_proto_id;		/**< protocol ID */
	uint16_t hdr_checksum;		/**< header checksum */
	uint32_t src_addr;		/**< source address */
	uint32_t dst_addr;		/**< destination address */
} __attribute__((__packed__));

/* IPv4 protocols */
#define PROTO_IP         0  /**< dummy for IP */
#define PROTO_HOPOPTS    0  /**< IP6 hop-by-hop options */
#define PROTO_ICMP       1  /**< control message protocol */
#define PROTO_IGMP       2  /**< group mgmt protocol */
#define PROTO_GGP        3  /**< gateway^2 (deprecated) */
#define PROTO_IPV4       4  /**< IPv4 encapsulation */
#define PROTO_TCP        6  /**< tcp */
#define PROTO_ST         7  /**< Stream protocol II */
#define PROTO_EGP        8  /**< exterior gateway protocol */
#define PROTO_PIGP       9  /**< private interior gateway */
#define PROTO_RCCMON    10  /**< BBN RCC Monitoring */
#define PROTO_NVPII     11  /**< network voice protocol*/
#define PROTO_PUP       12  /**< pup */
#define PROTO_ARGUS     13  /**< Argus */
#define PROTO_EMCON     14  /**< EMCON */
#define PROTO_XNET      15  /**< Cross Net Debugger */
#define PROTO_CHAOS     16  /**< Chaos*/
#define PROTO_UDP       17  /**< user datagram protocol */
#define PROTO_MUX       18  /**< Multiplexing */
#define PROTO_MEAS      19  /**< DCN Measurement Subsystems */
#define PROTO_HMP       20  /**< Host Monitoring */
#define PROTO_PRM       21  /**< Packet Radio Measurement */
#define PROTO_IDP       22  /**< xns idp */
#define PROTO_TRUNK1    23  /**< Trunk-1 */
#define PROTO_TRUNK2    24  /**< Trunk-2 */
#define PROTO_LEAF1     25  /**< Leaf-1 */
#define PROTO_LEAF2     26  /**< Leaf-2 */
#define PROTO_RDP       27  /**< Reliable Data */
#define PROTO_IRTP      28  /**< Reliable Transaction */
#define PROTO_TP        29  /**< tp-4 w/ class negotiation */
#define PROTO_BLT       30  /**< Bulk Data Transfer */
#define PROTO_NSP       31  /**< Network Services */
#define PROTO_INP       32  /**< Merit Internodal */
#define PROTO_SEP       33  /**< Sequential Exchange */
#define PROTO_3PC       34  /**< Third Party Connect */
#define PROTO_IDPR      35  /**< InterDomain Policy Routing */
#define PROTO_XTP       36  /**< XTP */
#define PROTO_DDP       37  /**< Datagram Delivery */
#define PROTO_CMTP      38  /**< Control Message Transport */
#define PROTO_TPXX      39  /**< TP++ Transport */
#define PROTO_IL        40  /**< IL transport protocol */
#define PROTO_IPV6      41  /**< IP6 header */
#define PROTO_SDRP      42  /**< Source Demand Routing */
#define PROTO_ROUTING   43  /**< IP6 routing header */
#define PROTO_FRAGMENT  44  /**< IP6 fragmentation header */
#define PROTO_IDRP      45  /**< InterDomain Routing*/
#define PROTO_RSVP      46  /**< resource reservation */
#define PROTO_GRE       47  /**< General Routing Encap. */
#define PROTO_MHRP      48  /**< Mobile Host Routing */
#define PROTO_BHA       49  /**< BHA */
#define PROTO_ESP       50  /**< IP6 Encap Sec. Payload */
#define PROTO_AH        51  /**< IP6 Auth Header */
#define PROTO_INLSP     52  /**< Integ. Net Layer Security */
#define PROTO_SWIPE     53  /**< IP with encryption */
#define PROTO_NHRP      54  /**< Next Hop Resolution */
/* 55-57: Unassigned */
#define PROTO_ICMPV6    58  /**< ICMP6 */
#define PROTO_NONE      59  /**< IP6 no next header */
#define PROTO_DSTOPTS   60  /**< IP6 destination option */
#define PROTO_AHIP      61  /**< any host internal protocol */
#define PROTO_CFTP      62  /**< CFTP */
#define PROTO_HELLO     63  /**< "hello" routing protocol */
#define PROTO_SATEXPAK  64  /**< SATNET/Backroom EXPAK */
#define PROTO_KRYPTOLAN 65  /**< Kryptolan */
#define PROTO_RVD       66  /**< Remote Virtual Disk */
#define PROTO_IPPC      67  /**< Pluribus Packet Core */
#define PROTO_ADFS      68  /**< Any distributed FS */
#define PROTO_SATMON    69  /**< Satnet Monitoring */
#define PROTO_VISA      70  /**< VISA Protocol */
#define PROTO_IPCV      71  /**< Packet Core Utility */
#define PROTO_CPNX      72  /**< Comp. Prot. Net. Executive */
#define PROTO_CPHB      73  /**< Comp. Prot. HeartBeat */
#define PROTO_WSN       74  /**< Wang Span Network */
#define PROTO_PVP       75  /**< Packet Video Protocol */
#define PROTO_BRSATMON  76  /**< BackRoom SATNET Monitoring */
#define PROTO_ND        77  /**< Sun net disk proto (temp.) */
#define PROTO_WBMON     78  /**< WIDEBAND Monitoring */
#define PROTO_WBEXPAK   79  /**< WIDEBAND EXPAK */
#define PROTO_EON       80  /**< ISO cnlp */
#define PROTO_VMTP      81  /**< VMTP */
#define PROTO_SVMTP     82  /**< Secure VMTP */
#define PROTO_VINES     83  /**< Banyon VINES */
#define PROTO_TTP       84  /**< TTP */
#define PROTO_IGP       85  /**< NSFNET-IGP */
#define PROTO_DGP       86  /**< dissimilar gateway prot. */
#define PROTO_TCF       87  /**< TCF */
#define PROTO_IGRP      88  /**< Cisco/GXS IGRP */
#define PROTO_OSPFIGP   89  /**< OSPFIGP */
#define PROTO_SRPC      90  /**< Strite RPC protocol */
#define PROTO_LARP      91  /**< Locus Address Resoloution */
#define PROTO_MTP       92  /**< Multicast Transport */
#define PROTO_AX25      93  /**< AX.25 Frames */
#define PROTO_IPEIP     94  /**< IP encapsulated in IP */
#define PROTO_MICP      95  /**< Mobile Int.ing control */
#define PROTO_SCCSP     96  /**< Semaphore Comm. security */
#define PROTO_ETHERIP   97  /**< Ethernet IP encapsulation */
#define PROTO_ENCAP     98  /**< encapsulation header */
#define PROTO_APES      99  /**< any private encr. scheme */
#define PROTO_GMTP     100  /**< GMTP */
#define PROTO_IPCOMP   108  /**< payload compression (IPComp) */
/* 101-254: Partly Unassigned */
#define PROTO_PIM      103  /**< Protocol Independent Mcast */
#define PROTO_PGM      113  /**< PGM */
#define PROTO_SCTP     132  /**< Stream Control Transport Protocol */
/* 255: Reserved */
/* BSD Private, local use, namespace incursion */
#define PROTO_DIVERT   254  /**< divert pseudo-protocol */
#define PROTO_RAW      255  /**< raw IP packet */
#define PROTO_MAX      256  /**< maximum protocol number */




struct net_protocol
{
	void (*handler)(struct m_buf *);
};


static inline struct ipv4_hdr *ip_hdr(const struct m_buf *mbuf)
{
	return (struct ipv4_hdr *)mbuf_network_header(mbuf);
}

extern void ip4_input(struct m_buf *mbuf);
extern uint32_t ip4_add_protocol(const struct net_protocol *prot, unsigned char protocol);



#endif
