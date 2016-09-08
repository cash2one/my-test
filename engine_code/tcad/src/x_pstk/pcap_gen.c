#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <unistd.h>   
#include <stdint.h>   
#include <errno.h>   
#include <sys/time.h>  
#include <ctype.h>  //wdb_calc222
#include "common.h"   

#include "pcap_gen.h" 

int  pcap_write_header(FILE *fp, int linktype, int thiszone, int snaplen)  
{  
	struct pcap_file_header_tag hdr;  

    if (NULL == fp) {
        return -1;
    }
  
    hdr.magic = TCPDUMP_MAGIC;  
    hdr.major = PCAP_VERSION_MAJOR;  
    hdr.minor = PCAP_VERSION_MINOR;  
	
    hdr.thiszone = thiszone;  
    hdr.snaplen = snaplen;  
    hdr.sigfigs = 0;  
    hdr.linktype = linktype;

    if (fwrite((char *)&hdr, sizeof(hdr), 1, fp) != 1)  
        return (-1);  
      
    return (0);  
}  

void pcap_write_pkt(char* buf,int len,int flag, FILE *fp)
{
	if(NULL != fp)
	{
		PKTHEAD pkth;

		pkth.tm1 = 0 ;
		pkth.tm2 = flag ;
		pkth.caplen = len ;
		pkth.len = len ;

		fwrite(&pkth,sizeof(pkth),1,fp) ;
		fwrite(buf,len,1,fp) ;
		fflush(fp) ;
	}

}

void pkt_hexdump(void *ptr, size_t len)
{
	unsigned char *data = (unsigned char *)ptr;
	int i,j,k;

	for ( i = 0,j=0 ; i <len ; i++ )
	{
		if (i && i%16==0){
			printf("  ");
			while(j<i){
				printf("%c", isprint(data[j])?data[j]:'.');
				++j;
			}
			printf("\n");
		}
		
		printf("%02x ", data[i]);
	}

	if (len&0x0f){
		k = 0x10-(len&0x0f);
		while(k>0){
			printf("   ");
			--k;
		}
	}
		
	
	printf("  ");
	while(j<len){
		printf("%c", isprint(data[j])?data[j]:'.');
		++j;
	}
	printf("\n");
	printf("\n");
}

int ca_pcap_write_header(FILE *fp)
{
    struct traffic_cap_file_header hdr;

    hdr.magic = TCPDUMP_MAGIC;
    hdr.version_major = PCAP_VERSION_MAJOR;
    hdr.version_minor = PCAP_VERSION_MINOR;

    hdr.thiszone = 0;
    hdr.snaplen = 65535;
    hdr.sigfigs = 0;
    hdr.linktype = LINKTYPE_ETHERNET;

    (void)fwrite((char *)&hdr, sizeof(hdr), 1, fp);
    fflush(fp);

    return (0);
}

void ca_pcap_write(FILE *fp, char *buf, uint32_t len)
{
    struct traffic_cap_pkthdr h;
    struct traffic_cap_sf_pkthdr sf_hdr;

    gettimeofday(&h.ts, NULL);
	h.caplen = len;
	h.len    = len;

    sf_hdr.ts.tv_sec  = h.ts.tv_sec;
	sf_hdr.ts.tv_usec = h.ts.tv_usec;
	sf_hdr.caplen     = h.caplen;
	sf_hdr.len        = h.len;
    
	/* XXX we should check the return status */
	(void)fwrite(&sf_hdr, sizeof(sf_hdr), 1, fp);
	(void)fwrite(buf, h.caplen, 1, fp);
    
	fflush(fp);

    return;
}

