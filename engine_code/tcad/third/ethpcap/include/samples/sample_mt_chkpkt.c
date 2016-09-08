/*
 * sample_mt_chkpkt.c
 * -- use multiple threads get and check pkts, including:
 *    ip_layer_len, udp_layer_len, not_tcp_and_udp pkt.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>


#include <libpag.h>

#define MAX_THREAD_COUNT 20
#define  THREAD_COUNT MAX_THREAD_COUNT
#define  COPY_TIME 0

void *show(void *p);
void *recv_thread(void *p);
void  sig_handler(int sig);
void process_pkt(void *pkt);

int stop_flag[THREAD_COUNT];
unsigned long long int recv_packet_count[THREAD_COUNT];
unsigned long long int recv_packet_bytes[THREAD_COUNT];

int stop;
int thread_count=0;

int main()
{
	int i;
	int retv;
	double sec;
	unsigned long long int  all_recv_packet_count = 0;
	unsigned long long int  all_recv_packet_bytes = 0;
	
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
	
	for(i=0; i<thread_count; i++){
		stop_flag[i] = 0;
		recv_packet_count[i] = 0;
		recv_packet_bytes[i] = 0;
		pthread_create(pid_recv+i, NULL, recv_thread, (void*)(&i));
		usleep(300);
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
		printf("thread%d: %15llu pkts\n", i, recv_packet_count[i]);
	}
	
	sec = (double)(tv_end.tv_sec - tv_begin.tv_sec) + 
			((double)(tv_end.tv_usec - tv_begin.tv_usec))/1000000;

	printf("Total:   %llu pkts,  %llu MB\n", all_recv_packet_count,
			all_recv_packet_bytes>>20);
	printf("Speed:   %llu pps,   %llu Mbps\n\n", 
			(unsigned long long int)(all_recv_packet_count/sec),
			(unsigned long long int)((all_recv_packet_bytes >> 17)/sec));
	
	return 0;
}

void *recv_thread(void *p)
{
	int i;
	int packet_len;
	void *desc_p;

	i = *((int *)p);

	while(!stop_flag[i]){	
		desc_p = pag_get(i);
		//desc_p = ppf_recvDataBlock(i);
		if(!desc_p){
			usleep(300);
			continue;
		}
		process_pkt(desc_p);
		//packet_len = ntohs(((struct iphdr*)desc_p)->tot_len);
		packet_len = ip_pkt_len(desc_p);
		recv_packet_count[i]++;
		recv_packet_bytes[i] += packet_len;

		pag_free(i, desc_p);
	}
}

/*
 * @num, the number of bytes to print.
 */
void print_pkt(unsigned char *pkt, int num)
{
	unsigned char ch;
	int i;
	printf("--pkt-begin--\n");
	for (i=0; i<num; i++) {
		ch = pkt[i];
		printf("%2x ", ch);
		if ((i+1)%16==0) {
			printf("\n");
		}
	}
	printf("--pkt-end--\n");
}

void process_pkt(void *pkt)
{
	int i, j, len;
	char *p;
	struct iphdr *iph = (struct iphdr *)pkt;

	p = (char *)pkt;
	//len = ntohs(((struct iphdr*)pkt)->tot_len);
	len = ip_pkt_len(pkt);
	if (len > 1500) {
		printf("ip len=%d\n", len);
		print_pkt(pkt-14, 48);
	}
	if (ip_pkt_version(pkt) == 6) {
		printf("ipv6\n");
		return;
	}
	if (iph->protocol == IPPROTO_UDP) {
		struct udphdr *udph = (struct udphdr *)(pkt+(iph->ihl<<2));
		int udplen = ntohs(udph->len);
		if (udplen > 1500) {
			printf("udp len=%d\n", udplen);
			print_pkt(pkt-14, 48);
		}
	} else if (iph->protocol != IPPROTO_TCP) {
		printf("what L4 protocol ? %u\n", iph->protocol);
		print_pkt(pkt-14, 48);
	}
	
	/*
	for(i=0; i<COPY_TIME; i++)
		for(j=0; j<len; j++)
			p[j] = p[j];
	*/
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
			
				printf("thread%d: %7llu pps, %4lluMbps, %15llu pkts\n", i,
						(unsigned long long int)((recv_packet_count[i]-prev_recv_packet_count[i])/sec),
						(unsigned long long int)(((recv_packet_bytes[i]-prev_recv_packet_bytes[i]) >> 17)/sec),
						recv_packet_count[i]);
				prev_recv_packet_count[i] = recv_packet_count[i];
				prev_recv_packet_bytes[i] = recv_packet_bytes[i];
		
		}

		printf("\n\n");

		gettimeofday(&tv_prev, NULL);
		sleep(60);
	}
	
}

void sig_handler(int sig)
{
	//printf("recv signal.\n");
	stop = 1;
}
