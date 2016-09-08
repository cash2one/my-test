#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include <libpag.h>

#define ETH_HDR_LEN sizeof(struct ethhdr)
#define IP_HDR_LEN sizeof(struct iphdr)
#define TCP_HDR_LEN sizeof(struct tcphdr)
#define UDP_HDR_LEN sizeof(struct udphdr)

#define MAX_THREAD_COUNT 32
#define  THREAD_COUNT MAX_THREAD_COUNT
#define  COPY_TIME 0

struct pseudo_hdr {                     /* See RFC 793 Pseudo Header */
    u_int32_t saddr, daddr;                /* source and dest address */
    u_int8_t mbz, ptcl;                   /* zero and protocol */
    u_int16_t tcpl;                       /* tcp length */
}__attribute__ ((__packed__));

static print_one_pkt(void *pkt, int len);
void *show(void *p);
void *send_thread(void *p);
void  sig_handler(int sig);
void gen_pkt(void *pkt, int send_tcp, int len);
uint64_t pkt_cal_sum(void *pkt, int len);

int stop_flag[THREAD_COUNT];
unsigned long long int recv_packet_count[THREAD_COUNT];
unsigned long long int recv_packet_bytes[THREAD_COUNT];
unsigned long long int recv_packet_sum[THREAD_COUNT];

int stop;
int thread_count=0;
/*send_tcp==0/1 : send udp/tcp packets*/
int cfg_send_tcp = 1;
int cfg_set_sum = 0;
int cfg_silent = 1;
int cfg_sendpkt_intv = 0; /*in second*/
int gen_rand_pkt=0; /*whether to gen random packet content*/
int print_len=1500;
/* cfg_cal_sum: calculate sum of each packet.
 * the sum is simply a uint64_t sum of every uint32_t in each packet.
 */
int cfg_cal_sum = 0;

char sample_udp_pkt[48] = {
	0x45, 0x00, 0x00, 0x30, 0x5e, 0xe3, 0x00, 0x00,
	0x01, 0x11, 0x97, 0x92, 0xc0, 0xa8, 0x00, 0x93,
	0xea, 0x22, 0x17, 0xea, 0x47, 0x9b, 0x83, 0x8a,
	0x00, 0x1c, 0x61, 0x81, 0x10, 0x00, 0x00, 0x00,
	0x18, 0xc2, 0x8c, 0x7c, 0xe4, 0xd6, 0x27, 0xfd,
	0x5e, 0x06, 0x1e, 0x0d, 0xae, 0xa3, 0x22, 0xfd,
};

char eth_ipv6_pkt0[48] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 0x86, 0xdd, 0x60, 0x11,
	0x12, 0x13, 0x04, 0x15, 0x6, 0x17, 0x20, 0x21,
	0x22, 0x23, 0x24, 0x24, 0x26, 0x27, 0x30, 0x31,
	0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
};

char eth_ipv6_pkt1[48] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 0x86, 0xdd, 0x10, 0x11,
	0x12, 0x13, 0x04, 0x15, 0x11, 0x17, 0x30, 0x31,
	0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x20, 0x21,
	0x22, 0x23, 0x24, 0x24, 0x26, 0x27, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
};

//offset=18
char eth_8021q_pkt0[128] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 
	
	//C-TAG
	0x81, 0x00, 0x10, 0x11, 
	//ethertype
	0x08, 0x00,
};
int hlen_8021q=18;

//offset=22
char eth_8021ad_pkt0[128] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 

	//S-TAG
	0x88, 0xa8, 0x01, 0x02,
	//C-TAG
	0x81, 0x00, 0x10, 0x11,
	//ethertype
	0x08, 0x00,
};
int hlen_8021ad = 22;

//offset=40
char eth_8021ah_pkt0[128] = {
	//B-DA and B-DA
	0x00, 0x02, 0x03, 0x03, 0x04, 0x05, 0x0a, 0x0b,
	0x0c, 0x3d, 0x03, 0x2f, 
	//B-TAG
	0x88, 0xa8, 0x01, 0x02,
	//I-TAG
	0x88, 0xe7, 0x01, 0x02, 0x03, 0x04,
	
	//C-DA and C-SA
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 

	//C-TAG
	0x81, 0x00, 0x10, 0x11,
	//ethertype
	0x08, 0x00,
};
int hlen_8021ah = 40;

//offset=28
char eth_8021adu_pkt0[128] = {
	0x00, 0x00, 0x00, 0x00, 0x01, 0x15, 0x00, 0x18, 
	0x01, 0x02, 0x0a, 0x03, 
	
	//S-TAG
	0x88 , 0xa8 , 0x60 , 0x00
	//what tag? 0x64ff
	, 0x64 , 0xff , 0xff , 0xff , 0xff , 0xff , 0xff , 0xff  , 0xff , 0xff , 
	//ethertype
	0x08 , 0x00 , 

	0x45 , 0x60 , 0x00 , 0x4d, 0xac , 0xb5 , 0x40 , 0x00,
	0xf7 , 0x11 , 0x23 , 0x8c, 0xd3 , 0x8a , 0x12 , 0x56,
	0xca , 0x37 , 0x02 , 0xe6, 0xf4 , 0x6c , 0x00 , 0x35,
	0x00 , 0x39 , 0xff , 0x75, 0x2e , 0x43 , 0x00 , 0x00,
	0x00 , 0x01 , 0x00 , 0x00, 0x00 , 0x00 , 0x00 , 0x01,
	0x08 , 0x74 , 0x63 , 0x70, 0x63 , 0x6f , 0x6e , 0x6e,
	0x36 , 0x07 , 0x74 , 0x65, 0x6e , 0x63 , 0x65 , 0x6e,
	0x74 , 0x03 , 0x63 , 0x6f, 0x6d , 0x00 , 0x00 , 0x1c,
	0x00 , 0x01 , 0x00 , 0x00, 0x29 , 0x10 , 0x00 , 0x00,
	0x00 , 0x80 , 0x00 , 0x00, 0x00
};
int hlen_8021adu = 28;

int main()
{
	int i;
	int retv;
	double sec;
	unsigned long long int  all_recv_packet_count = 0;
	unsigned long long int  all_recv_packet_bytes = 0;
	unsigned long long int  all_packet_sum = 0;
	
	struct sigaction sig_act;
	pthread_t pid_recv[THREAD_COUNT];
	pthread_t pid;
	
	struct timeval tv_begin, tv_end;

	if (pag_getStreamNum(&thread_count) != 0) {
		printf("Error: failed to get stream_num !\n");
		return 0;
	}
	if (thread_count > THREAD_COUNT) {
		printf("Error: stream_num too large ! max of %d\n", THREAD_COUNT);
		return 0;
	}

	if (gen_rand_pkt) {
		srandom((int)time(NULL));
	}
	for(i=0; i<thread_count; i++){
		stop_flag[i] = 0;
		recv_packet_count[i] = 0;
		recv_packet_bytes[i] = 0;
		recv_packet_sum[i] = 0;
		pthread_create(pid_recv+i, NULL, send_thread, (void*)(&i));
		usleep(10000);
	}

	pthread_create(&pid, NULL, show, NULL);
	pthread_detach(pid);

	memset((void *)(&sig_act), 0, sizeof(struct sigaction));
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = sig_handler;
	sigaction(SIGINT, &sig_act, NULL);
	
	gettimeofday(&tv_begin, NULL);
	
	retv = pag_open();
	if(retv != 1){
		printf("pag_open() failed.\n");
		return -1;
	}

	/* ppf_setStreamNum(THREAD_COUNT); */

	stop = 0;
	while(!stop){
		sleep(2);
	}

	for(i=0; i<thread_count; i++){
		stop_flag[i] = 1;
		pthread_join(pid_recv[i], NULL);
	}

	pag_close();
	
	gettimeofday(&tv_end, NULL);

	for(i=0; i<thread_count; i++){
		all_recv_packet_count += recv_packet_count[i];
		all_recv_packet_bytes += recv_packet_bytes[i];
		all_packet_sum += recv_packet_sum[i];
	}
	
	sec = (double)(tv_end.tv_sec - tv_begin.tv_sec) + 
			((double)(tv_end.tv_usec - tv_begin.tv_usec))/1000000;

	printf("Total:   %llu pkts,  %llu MB, %llu chksum\n", all_recv_packet_count,
			all_recv_packet_bytes>>20, all_packet_sum);
	printf("Speed:   %llu pps,   %llu Mbps\n\n", 
			(unsigned long long int)(all_recv_packet_count/sec),
			(unsigned long long int)((all_recv_packet_bytes >> 17)/sec));
	
	return 0;
}

void *send_thread(void *p)
{
	int i;
	int packet_len;
	void *desc_p;
	void *pkt;
	int pkt_len = 200;
	int iplen = pkt_len/*-18*/;
	int ret;
	long success=0;
	uint64_t sum=0;
	int hlen=14, ptype=0;
	uint32_t sip=0x01020304, dip=0x10203040;
	struct iphdr *iph;

	i = *((int *)p);
	sleep(5);
	while(!stop_flag[i]){
		pkt = pag_getsendbuf(i);
		if (!pkt) {
			usleep(1);
			continue;
		}
		/**
		gen_pkt(pkt, cfg_send_tcp, pkt_len);
		if (cfg_cal_sum) {
			sum = pkt_cal_sum(pkt, pkt_len);
		}
		**/
		if (0) {
			switch (ptype) {
				case 0:
					memcpy(pkt, sample_udp_pkt, sizeof(sample_udp_pkt));
					hlen = 0;
					break;
				case 1:
					memcpy(pkt, eth_ipv6_pkt0, sizeof(eth_ipv6_pkt0));
					hlen = 14;
					break;
				case 2:
					memcpy(pkt, eth_8021q_pkt0, sizeof(eth_8021q_pkt0));
					hlen = hlen_8021q;
					break;
				case 3:
					memcpy(pkt, eth_8021ad_pkt0, sizeof(eth_8021ad_pkt0));
					hlen = hlen_8021ad;
				case 4:
					memcpy(pkt, eth_8021ah_pkt0, sizeof(eth_8021ah_pkt0));
					hlen = hlen_8021ah;
					break;
				case 5:
					memcpy(pkt, eth_8021adu_pkt0, sizeof(eth_8021adu_pkt0));
					hlen = hlen_8021adu;
					break;
			}
			if (ptype != 1) {
				iph = (struct iphdr *)(pkt+hlen);
				iph->saddr = sip;
				iph->daddr = dip;
				iph->tot_len = htons(iplen);
			}
			//slowly
			sleep(1);
		}
		//ret = pag_send(/*pkt, */SEND_PKT_TYPE_IP, i, pkt_len);
		ret = pag_send(SEND_PKT_TYPE_ETH, i, pkt_len);
		if (!ret) {
			success++;			
			packet_len = pkt_len /*ntohs(((struct iphdr*)pkt)->tot_len)*/;
			recv_packet_count[i]++;
			recv_packet_bytes[i] += packet_len;
			recv_packet_sum[i] += sum;
		}
		/*pag_freesendbuf(i, pkt);*/
		/*debug*/
		if (cfg_sendpkt_intv) {
			sleep(cfg_sendpkt_intv);
		}
	}

}

/* This is a reference internet checksum implimentation, not very fast */
inline u_short in_cksum(u_short *addr, int len)
{
    register int nleft = len;
    register u_short *w = addr;
    register int sum = 0;
    u_short answer = 0;

     /* Our algorithm is simple, using a 32 bit accumulator (sum), we add
      * sequential 16 bit words to it, and at the end, fold back all the
      * carry bits from the top 16 bits into the lower 16 bits. */

     while (nleft > 1)  {
         sum += *w++;
         nleft -= 2;
     }

     /* mop up an odd byte, if necessary */
     if (nleft == 1) {
         *(u_char *)(&answer) = *(u_char *) w;
         sum += answer;
     }

     /* add back carry outs from top 16 bits to low 16 bits */
     sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
     sum += (sum >> 16);                /* add carry */
     answer = ~sum;                     /* truncate to 16 bits */
     return(answer);
}

void form_udp_packet(char *pkt_iph, int udp_len, int set_sum)
{
	struct iphdr *iph = (struct iphdr *)pkt_iph;
	struct udphdr *udph = (struct udphdr *)(pkt_iph + IP_HDR_LEN);
	struct pseudo_hdr *pseu_hdr = (struct pseudo_hdr *)((char*)udph - sizeof(struct pseudo_hdr));
	uint16_t udpsum=0, ipsum=0;

	uint32_t srcaddr=0xdb00a8c0;
	uint32_t destaddr=0x05060708;
	int srcport = 0xab;
	int destport = 0xcd;

	udph->source = srcport;
	udph->dest = destport;
	udph->len = htons(udp_len);
	pseu_hdr->saddr = srcaddr;
	pseu_hdr->daddr = destaddr;
	pseu_hdr->mbz = 0;
	pseu_hdr->ptcl = IPPROTO_UDP;
	pseu_hdr->tcpl = htons(udp_len);
	udph->check = 0;
	udpsum = in_cksum((u_short*)pseu_hdr, sizeof(struct pseudo_hdr)+udp_len);
	if (set_sum) {
		udph->check = udpsum;
	}

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0x08;
	iph->id = rand();
	iph->frag_off = 0;
	iph->ttl = 255;
	
	iph->protocol = IPPROTO_UDP;
	iph->tot_len = htons(IP_HDR_LEN+udp_len);
	iph->saddr = srcaddr;
	iph->daddr = destaddr;
	iph->check = 0;
	//iph->check = in_cksum((u_short*)iph, sizeof(struct iphdr));
	ipsum = in_cksum((u_short*)iph, sizeof(struct iphdr));
	if (set_sum) {
		iph->check = ipsum;
	}

	printf("udpsum=%x, ip sum=%x\n", udpsum, ipsum);
}

void form_tcp_packet(char *pkt_iph, int tcp_len, int set_sum)
{
	struct iphdr *iph = (struct iphdr *)pkt_iph;
	struct tcphdr *tcph = (struct tcphdr *)(pkt_iph + IP_HDR_LEN);
	struct pseudo_hdr *pseu_hdr = (struct pseudo_hdr *)((char*)tcph - sizeof(struct pseudo_hdr));
	uint16_t tcpsum=0, ipsum=0;

	uint32_t srcaddr=0xdb00a8c0;
	uint32_t destaddr=0x05060708;
	int srcport = 0xab;
	int destport = 0xcd;

	tcph->source = srcport;
	tcph->dest = destport;
	tcph->seq = random();
	tcph->ack_seq = rand();
	tcph->doff = sizeof(struct tcphdr) / 4;
	tcph->window = 10000;
	
	pseu_hdr->saddr = srcaddr;
	pseu_hdr->daddr = destaddr;
	pseu_hdr->mbz = 0;
	pseu_hdr->ptcl = IPPROTO_TCP;
	pseu_hdr->tcpl = htons(tcp_len);
	tcph->check = 0;
	/* tcph->check = in_cksum((u_short*)pseu_hdr, sizeof(struct pseudo_hdr)+tcp_len); */	
	if (set_sum) {
		tcpsum = in_cksum((u_short*)pseu_hdr, sizeof(struct pseudo_hdr)+tcp_len);
		tcph->check = tcpsum;
	}

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0x08;
	iph->id = rand();
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_TCP;
	iph->tot_len = htons(IP_HDR_LEN+tcp_len);
	iph->saddr = srcaddr;
	iph->daddr = destaddr;
	iph->id = rand();
	iph->check = 0;
	/* iph->check = in_cksum((u_short*)iph, sizeof(struct iphdr)); */
	if (set_sum) {
		ipsum = in_cksum((u_short*)iph, sizeof(struct iphdr));
		iph->check = set_sum;
	}

	if (set_sum && !cfg_silent) {
		printf("tcpsum=%x, ip sum=%x\n", tcpsum, ipsum);
	}
}

static print_one_pkt(void *pkt, int len)
{
	int i, dt;
	uint8_t *p;

	printf("pkt:\n");
	printf("==============\n");
	p = pkt;
	for (i=0; i<len; i++) {
		dt = *p;
		printf("%2x ", dt);
		p++;
		if ((i+1)%16==0) {
			printf("\n");
		}
	}
	printf("\n");
}

void gen_pkt(void *pkt, int send_tcp, int len)
{
	
	if (!gen_rand_pkt) {
		if (send_tcp) {
			form_tcp_packet(pkt, len-IP_HDR_LEN, cfg_set_sum);
		} else {
			form_udp_packet(pkt, len-IP_HDR_LEN, cfg_set_sum);
		}
	} else {
		unsigned char *p = (unsigned char *)pkt;
		int i;
		for (i=0; i<len; i++) {
			p[i] = random();
		}
		p[0] = 0x45; /*ipv4*/
		((struct iphdr *)pkt)->tot_len = htons(len);
	}

	if (!cfg_silent) {
		print_one_pkt(pkt, len);
	}
}

uint64_t pkt_cal_sum(void *pkt, int len)
{
	uint64_t sum=0;
	uint32_t *p = (uint32_t *)pkt;
	int i, num;

	num = len / sizeof(uint32_t);
	for (i=0; i<num; i++) {
		sum += p[i];
	}
	
	return sum;
}

void *show(void *p)
{
	int i;
	double sec;
	struct timeval  tv_prev, tv_curr;
	unsigned long long int prev_recv_packet_count[THREAD_COUNT];
	unsigned long long int prev_recv_packet_bytes[THREAD_COUNT];
	
	for(i=0; i<thread_count; i++){
		prev_recv_packet_count[i] = 0;
		prev_recv_packet_bytes[i] = 0;
	}
	
	gettimeofday(&tv_prev, NULL);
	sleep(2);

	while(!stop){
		gettimeofday(&tv_curr, NULL);
		
		sec = (double)(tv_curr.tv_sec - tv_prev.tv_sec) +
				((double)(tv_curr.tv_usec - tv_prev.tv_usec))/1000000;
		
		for(i=0; i<thread_count; i++){
			
				printf("thread%d: %7llu pps, %4lluMbps\n", i,
						(unsigned long long int)((recv_packet_count[i]-prev_recv_packet_count[i])/sec),
						(unsigned long long int)(((recv_packet_bytes[i]-prev_recv_packet_bytes[i]) >> 17)/sec));
				prev_recv_packet_count[i] = recv_packet_count[i];
				prev_recv_packet_bytes[i] = recv_packet_bytes[i];
		
		}

		printf("\n\n");

		gettimeofday(&tv_prev, NULL);
		sleep(2);
	}
	
}

void sig_handler(int sig)
{
	//printf("recv signal.\n");
	stop = 1;
}
