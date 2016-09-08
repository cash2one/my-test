#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/ether.h>
#include <netinet/ip.h>

#include <libpag.h>

void  sig_handler(int sig);
void *show(void *p);


unsigned long long int recv_packet_count[10];
unsigned long long int recv_packet_bytes[10];

int stop;

int main()
{
	int i;
	int retv;
	double sec;
	unsigned long long int  all_recv_packet_count = 0;
	unsigned long long int  all_recv_packet_bytes = 0;

	struct sigaction sig_act;

	pthread_t pid;

	uint32_t  nic_id;
	uint32_t  packet_len;

	struct timeval tv_begin, tv_end;

	void *desc_p;
	struct iphdr *ip_hdr;

	for(i=0; i<10; i++){
		recv_packet_count[i] = 0;
		recv_packet_bytes[i] = 0;
	}

	stop = 0;

	pthread_create(&pid, NULL, show, NULL);
	pthread_detach(pid);

	memset((void *)(&sig_act), 0, sizeof(struct sigaction));
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = sig_handler;
	sigaction(SIGINT, &sig_act, NULL);

	gettimeofday(&tv_begin, NULL);

	{/*check stream_num*/
		int stream_num;
		if (pag_getStreamNum(&stream_num)!=0) {
			printf("Error: faied to get stream_num !\n");
			return 0;
		}
		if (stream_num != 1) {
			printf("Error: got stream_num=%d, must be 1 ! \n", stream_num);
			return 0;
		}
	}

	retv = pag_open();
	if(retv != 1){
		printf("ppf_open() failed.\n");
		return 0;
	}

	/* ppf_setStreamNum(1); */


	while(!stop){
		//desc_p = ppf_getDataBlock_no_Lock(0);
		//desc_p = ppf_recvDataBlock(0);
		desc_p = pag_get(0);
		if(!desc_p){
			usleep(1);
			continue;
		}
		
		ip_hdr = (struct iphdr*)desc_p;
		packet_len = ntohs(ip_hdr->tot_len);
		recv_packet_count[0]++;
		recv_packet_bytes[0] += packet_len;

		pag_free(0, desc_p);
	}

	pag_close();
	
	gettimeofday(&tv_end, NULL);

	for(i=0; i<10; i++){
		all_recv_packet_count += recv_packet_count[i];
		all_recv_packet_bytes += recv_packet_bytes[i];
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

void *show(void *p)
{
	int i;
	int j;
	double sec;
	struct timeval  tv_prev, tv_curr;
	unsigned long long int prev_recv_packet_count[10];
	unsigned long long int prev_recv_packet_bytes[10];
	
	for(i=0; i<10; i++){
		prev_recv_packet_count[i] = 0;
		prev_recv_packet_bytes[i] = 0;
	}
	
	gettimeofday(&tv_prev, NULL);

	while(!stop){
		gettimeofday(&tv_curr, NULL);
		
		sec = (double)(tv_curr.tv_sec - tv_prev.tv_sec) +
				((double)(tv_curr.tv_usec - tv_prev.tv_usec))/1000000;
		
		j = 0;
		for(i=0; i<10; i++){
			if(recv_packet_count[i]){
				j = 1;
				printf("eth%d: %7llu pps, %4lluMbps\n", i,
						(unsigned long long int)((recv_packet_count[i]-prev_recv_packet_count[i])/sec),
						(unsigned long long int)(((recv_packet_bytes[i]-prev_recv_packet_bytes[i]) >> 17)/sec));
				prev_recv_packet_count[i] = recv_packet_count[i];
				prev_recv_packet_bytes[i] = recv_packet_bytes[i];
			}
		}

		if(j)
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
