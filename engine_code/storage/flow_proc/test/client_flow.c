#include "apue.h"

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
	addr.sin_port = htons(1235);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0)
	{
		perror("bind");
		goto ERR;
	}

	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0xDCBA);
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	printf("port = %d,addr = %d\n",addr.sin_port,addr.sin_addr.s_addr);

	for(i=0; ; i++)
	{
		memset(&buf, 0, sizeof(buf));
		//sprintf(buf, "hello_%d", i);
		init_test(&buf);
		sendto(sockfd, 
			   &buf, sizeof(buf), 
			   0, 
			   (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
		perror("sendto");
		sleep(1);
	}
ERR:
	close(sockfd);







}
