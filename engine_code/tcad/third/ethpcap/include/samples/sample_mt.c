#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

#include <libpag.h>

#define MAX_THREAD_COUNT 32
#define  THREAD_COUNT MAX_THREAD_COUNT
#define  COPY_TIME 0

void *show(void *p);
void *recv_thread(void *p);
void  sig_handler(int sig);
uint64_t pkt_cal_sum(void *pkt, int len);
void process_pkt(int sid, void *pkt);

int stop_flag[THREAD_COUNT];
unsigned long long int recv_packet_count[THREAD_COUNT];
unsigned long long int recv_packet_bytes[THREAD_COUNT];
unsigned long long int recv_packet_sum[THREAD_COUNT];

int stop;
int thread_count=0;
int print_pkt=0, print_len=1500;
int cfg_cal_sum = 0;

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
	
	for(i=0; i<thread_count; i++){
		stop_flag[i] = 0;
		recv_packet_count[i] = 0;
		recv_packet_bytes[i] = 0;
		recv_packet_sum[i] = 0;
		pthread_create(pid_recv+i, NULL, recv_thread, (void*)(&i));
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
		process_pkt(i, desc_p);
		//packet_len = ntohs(((struct iphdr*)desc_p)->tot_len);
		packet_len = ip_pkt_len(desc_p);
		recv_packet_count[i]++;
		recv_packet_bytes[i] += packet_len;
		if (cfg_cal_sum) {
			recv_packet_sum[i] += pkt_cal_sum(desc_p, packet_len);
		}
		pag_free(i, desc_p);
	}
}

static void print_one_pkt(int sid, struct iphdr* iph, int max_print)
{
	uint8_t *p = (uint8_t*)iph;
	int dt, pkt_len, i;
	if (iph->version == 4) {
		pkt_len = ntohs(iph->tot_len);
	} else if (iph->version==6) {
		pkt_len = sizeof(struct ip6_hdr) +( (struct ip6_hdr*)iph)->ip6_plen;
	} else {
		pkt_len = max_print;
	}
	/* print eth header*/
	/*
	if (1) {
		int hlen;
		void *hdr = pag_get_ethhdr(sid, iph, &hlen);
		if (hdr) {
			p = (uint8_t*)hdr;
			printf("<eth header> hlen=%d\n", hlen);
			for (i=0; i<hlen; i++) {
				dt = *p;
				p++;
				printf("%02x ", dt);
				if ((i+1)%16==0) {
					printf("\n");
				}
			}
			printf("\n================\n");
		}
	}
	*/
	printf("<ip pkt begen> ipl len=%d sid=%d\n", pkt_len, sid);
	printf("=====================\n");
	p = (uint8_t*)iph;
	for (i=0; i<pkt_len; i++) {
		dt = *p;
		p++;
		printf("%02x ", dt);
		if ((i+1)%16==0) {
			printf("\n");
		}
	}
	printf("\n<end>\n");
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

void process_pkt(int sid, void *pkt)
{
	int i, j, len;
	char *p;

	p = (char *)pkt;
	//len = ntohs(((struct iphdr*)pkt)->tot_len);
	len = ip_pkt_len(pkt);
	for(i=0; i<COPY_TIME; i++)
		for(j=0; j<len; j++)
			p[j] = p[j];

	if (print_pkt) {
		print_one_pkt(sid, pkt, print_len);
	}	
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
