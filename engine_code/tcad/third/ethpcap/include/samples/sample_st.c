#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <libpag.h>


void *show(void *p);
void  sig_handler(int sig);

unsigned long long int count;
unsigned long long int cksum_ok_count;

int stop;

int main()
{
	FILE *fp;
	int i;
	int retv;
	char buf[256];
	char buf2[32];
	void *desc_p;
	pthread_t pid;
	struct iphdr *ip_hdr;
	struct sigaction sig_act;

	count = 0;
	cksum_ok_count = 0;
	stop = 0;
	
	fp = fopen("/tmp/libppf_test.out", "w");

	pthread_create(&pid, NULL, show, NULL);
	pthread_detach(pid);

	memset((void *)(&sig_act), 0, sizeof(struct sigaction));
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = sig_handler;
	sigaction(SIGINT, &sig_act, NULL);

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

	printf("recv begin.\n");
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
		/*if(0){
			sprintf(buf, "nic:%u  len:%u  ", 
				ppf_getIfId(desc_p), ppf_getPktLength(desc_p));
			fputs(buf, fp);

			sprintf(buf, "ip_dst:%s   ", inet_ntoa(ip_hdr->ip_dst));
			fputs(buf, fp);

			sprintf(buf, "ip_src:%s\n", inet_ntoa(ip_hdr->ip_src));
			fputs(buf, fp);
		}*/
		pag_free(0, desc_p);
		count++;
	}
	pag_close();
	fclose(fp);
	printf("recv end.\n");
}

void print_one_pkt(struct iphdr* iph, int max_print)
{
	uint8_t *p = (uint8_t*)iph;
	int dt, pkt_len, i;
	pkt_len = ntohs(iph->tot_len);
	printf("<pkt begen> ipl len=%d\n", pkt_len);
	for (i=0; i<pkt_len && i<max_print; i++) {
		dt = *p;
		p++;
		printf("%2x ", dt);
		if ((i+1)%16==0) {
			printf("\n");
		}
	}
	printf("<end>\n");
}

void *show(void *p)
{
	int t=0;
	unsigned long long int  prev = 0;
	unsigned long long int  prev_cksum_ok_count = 0;
	char str[15];
	
	sleep(3);
	
	while(!stop){
		printf("show info:");
		scanf("%s", str);
		if(!strcmp(str, "yes")){
			printf("total:%lld   sub:%lld \n", count, count-prev);
			prev = count;
			continue;
		}
		
		if(!strcmp(str, "quit")){
			stop = 1;
		}
		
	}
}

void sig_handler(int sig)
{
	//printf("recv signal.\n");
	stop = 1;
}
