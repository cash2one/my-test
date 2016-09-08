//s_unix.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <stdlib.h>
#include <unistd.h>
#define UNIX_DOMAIN "/tmp/IPADDR.domain"
#define IP_LEN 16
#define RECV_LEN 100
int main(int argc,char **argv)
{
	int listen_fd;
	char send_ip[IP_LEN]={0};
	char read_val[RECV_LEN]={0};
	struct sockaddr_un srv_addr;

	if(argc <2)
	{
		printf("ERROR: please input eg:%s 123.56.34.22\n",argv[0]);
		exit(1);
	}

	snprintf(send_ip,IP_LEN,"%s",argv[1]);

	listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(listen_fd<0)
	{
		perror("cannot create communication socket");
		return 1;
	}  

	//set server addr_param
	srv_addr.sun_family=AF_UNIX;
	strncpy(srv_addr.sun_path,UNIX_DOMAIN,sizeof(srv_addr.sun_path)-1);
	int result = connect(listen_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(result < 0)
	{
		perror("connect fail:");
		exit(-1);
	}

	write(listen_fd,send_ip,sizeof(send_ip));


	memset(read_val,0,RECV_LEN);
	read(listen_fd,read_val,sizeof(read_val)+1);
	printf("%s",read_val);  
	close(listen_fd);
	return 0;
}

