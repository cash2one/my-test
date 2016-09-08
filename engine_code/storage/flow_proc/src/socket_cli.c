//s_unix.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <stdlib.h>
#include <unistd.h>
#define UNIX_DOMAIN "/tmp/FLOW.domain"
int main(void)
{
	//socklen_t clt_addr_len;
	int listen_fd;
	int ret;
	int num;
	struct sockaddr_un srv_addr;
	listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(listen_fd<0)
	{
		perror("cannot create communication socket");
		return -1;
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
	ret=1;
	write(listen_fd,(void*)&ret,sizeof(int));
	num=read(listen_fd,(void*)&ret,sizeof(int));
	printf("Message from client (%d)) :%d\n",num,ret);  
	close(listen_fd);
	return 0;
}

