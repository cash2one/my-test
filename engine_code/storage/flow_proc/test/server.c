#include "apue.h"
int main(int argc,char **argv)
{
	int n;
	struct sockaddr_in addr;
	int sockfd;
	Test buf;
	int i;
	int ret;
	socklen_t addr_len = sizeof(struct sockaddr_in);


	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0)
	{
		perror("socket");
		exit(1);
	}
	bzero(&addr, sizeof(struct sockaddr_in));
#if 1
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10110);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//addr.sin_addr.s_addr = inet_addr("192.168.0.195");
#endif
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0)
	{
		perror("bind");
		goto ERR;
	}
	for(i=0; ; i++)
	{
		memset(&buf, 0, sizeof(buf));
		//init_test(&buf);
		n = recvfrom(sockfd, 
			    &buf, sizeof(buf), 
			    0, 
			    (struct sockaddr *)&addr, &addr_len);
		printf("connect ip=%s port=%d\n", 
				inet_ntoa(addr.sin_addr),
				ntohs(addr.sin_port));
		printf("a= %lld , b = %lld ,c=%d\n",buf.sip,buf.dip,buf.query_id);

	}

ERR:
	close(sockfd);
	exit(0);






}
