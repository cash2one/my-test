#include "apue2.h"
unsigned int country[5]={4,7,86,20,27};
unsigned int prov[8]={130000,140000,150000,210000,230000,320000,330000,340000};
//unsigned int county[5]={110000,130100,130184,130185,130200};

unsigned int county[8][9]={{130100,130200,130300,130500,130600,130700,130800,130900,131000},
{140100,140200,140300,140500,140600,140700,140800,140900,141000},
{150100,150200,150300,150400,150500,150600,150700,150800,150900},
{210100,210200,210300,210400,210500,210600,210700,210800,210900},
{230100,230200,230300,230400,230500,230600,230700,230800,230900},
{320100,320200,320300,320400,320500,320600,320700,320800,320900},
{330100,330200,330300,330400,330500,330600,330700,330800,330900},
{340100,340200,340300,340400,340500,340600,340700,340800,341000}};

unsigned int ipbuf[5]={3524531116U,3524531117U,3524531110U,3524531120U,3524531122U};
unsigned int  ipend[5]={3524531120U,3524531125U,3524531114U,3524531119U,3524531118U};
int browser[5]={1,4,7,8,9};
char vernum[5][10]={"11","10","9","8.0","7"};
time_t  visittime[5]={50,30,20,43,56};
char flag[6]={0,1,0,1,0};
char pc[5]={1,0,0,1,0};
short s_isp[5]={1,3,2,59,95};
short d_isp[5]={20,51,150,3,2};

void init_test(Test*p)
{
	unsigned int i;
	time_t now_time = 0;
	struct tm *time_p = NULL;
	char time_format[] = "%Y-%m-%d %X";
	char start[25]={0};
	char end[25]={0};
	time(&now_time);
	p->ftime_start=now_time - 60;
	p->ftime_end =now_time;	
	i=rand()%5;
	p->visit_time=visittime[i];
	p->sip=ipbuf[i];
	p->dip=ipend[i];
	p->usersport=1234;
	p->userdport=4567;
	p->procotol_id=6;
	p->country=country[i];
	p->pc_if = pc[i];
	i=rand()%5;
	p->tcp_suc=flag[i];
	p->bank_visit=pc[i];
	p->flag=0;
	p->incr=0;
	p->res=0;
	p->s_isp_id=s_isp[i];
	p->d_isp_id=d_isp[i];

	p->ip_byte=56;
	p->ip_pkt=156;
	p->cli_ser=visittime[i];
	p->browser=browser[i];
	strcpy(p->version,vernum[i]);

	i=rand()%8;
	p->s_province_id=prov[i];
//	p->s_city_id=county[i];
	p->s_city_id=county[i][rand()%9];

	i=rand()%8;
	p->d_province_id=prov[i];
//	p->d_city_id=county[i];
	p->d_city_id=county[i][rand()%9];


}
int main(int argc,char **argv)
{
	struct sockaddr_in addr;
	int sockfd;
	Test buf;
	int i;
	int ret;

	if(argc != 2)
	{
		printf("%s ip addr.\n", argv[1]);
		exit(1);
	}

	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0)
	{
		perror("socket");
		exit(1);
	}
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2235);
	printf("port = %d\n",addr.sin_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0)
	{
		perror("bind");
		goto ERR;
	}

	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0xDCBB);
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	printf("flow_event connect ip=%s port=%d\n", 
				inet_ntoa(addr.sin_addr),
				ntohs(addr.sin_port));

	for(i=0; ; i++)
	{
		memset(&buf, 0, sizeof(buf));
		//sprintf(buf, "hello_%d", i);
		init_test(&buf);
		printf("\n country:%u,s_province_id:%u,s_city_id:%u,d_province_id:%u,d_city_id:%u",(&buf)->country,(&buf)->s_province_id,(&buf)->s_city_id,(&buf)->d_province_id,(&buf)->d_city_id);
		sendto(sockfd, 
			   &buf, sizeof(buf), 
			   0, 
			   (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
		perror("sendto");
		usleep(100000);
	}
ERR:
	close(sockfd);







}
