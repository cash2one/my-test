#ifndef __PCAP_GEN_H__
#define __PCAP_GEN_H__

#define TCPDUMP_MAGIC       0xa1b2c3d4   
#ifndef PCAP_VERSION_MAJOR   
#define PCAP_VERSION_MAJOR 2   
#endif 
#ifndef PCAP_VERSION_MINOR  
#define PCAP_VERSION_MINOR 4   
#endif  

//#define LINKTYPE_NULL       DLT_NULL   
#define LINKTYPE_ETHERNET  1  // DLT_EN10MB  /* also for 100Mb and up */   
//#define LINKTYPE_EXP_ETHERNET   DLT_EN3MB   /* 3Mb experimental Ethernet */   
//#define LINKTYPE_AX25       DLT_AX25   
//#define LINKTYPE_PRONET     DLT_PRONET   
//#define LINKTYPE_CHAOS      DLT_CHAOS   
//#define LINKTYPE_TOKEN_RING DLT_IEEE802 /* DLT_IEEE802 is used for Token Ring */   
//#define LINKTYPE_ARCNET     DLT_ARCNET  /* BSD-style headers */   
//#define LINKTYPE_SLIP       DLT_SLIP   
//#define LINKTYPE_PPP        DLT_PPP   
//#define LINKTYPE_FDDI       DLT_FDDI

struct traffic_cap_file_header {
    uint32_t magic;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;     /* gmt to local correction */
    uint32_t sigfigs;    /* accuracy of timestamps */
    uint32_t snaplen;    /* max length saved portion of each pkt */
    uint32_t linktype;   /* data link type (LINKTYPE_*) */
};

struct traffic_cap_pkthdr {
    struct timeval ts;      /* time stamp */
    uint32_t caplen;     /* length of portion present */
    uint32_t len;        /* length this packet (off wire) */
};

struct traffic_cap_timeval {
    int32_t tv_sec;		/* seconds */
    int32_t tv_usec;		/* microseconds */
};

struct traffic_cap_sf_pkthdr {
    struct traffic_cap_timeval ts;	/* time stamp */
    uint32_t caplen;		/* length of portion present */
    uint32_t len;		/* length this packet (off wire) */
};

typedef struct pcap_file_header_tag
{
	int magic ;
	unsigned short major ;
	unsigned short minor ;
	int thiszone ;
	int sigfigs ;
	int snaplen ;
	int linktype ;
}PCAPHEAD,*PPCAPHEAD;

typedef struct pkt_head_tag
{
	long tm1 ;
	long tm2 ;
	uint32_t caplen ;
	uint32_t len ;
}PKTHEAD,*PPKTHEAD ;
/*
struct timeval {
        long            tv_sec;        
        suseconds_t     tv_usec;       
};

struct pcap_pkthdr {
        struct timeval ts;     
        bpf_u_int32 caplen;    
        bpf_u_int32 len;       
};
*/
int  pcap_write_header(FILE *fp, int linktype, int thiszone, int snaplen);
void pcap_write_pkt(char* buf,int len,int flag, FILE *fp);
void pkt_hexdump(void *ptr, size_t len);

int ca_pcap_write_header(FILE *fp);
void ca_pcap_write(FILE *fp, char *buf, uint32_t len);

#endif

