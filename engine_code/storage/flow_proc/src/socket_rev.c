//s_unix.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include "gms_flow.h"
#define UNIX_DOMAIN "/tmp/FLOW.domain"
enum ret_result{
	recv_ok =0,
	recv_fail

};
void config_add(int ret)
{
	if(ret == 1)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "start reload config .....\n");
		reload_config();	
		CA_LOG(LOG_MODULE, LOG_PROC, "end reload config .....\n");
	}
}
void *gms_config_recv(void*arg)
{
    pthread_detach ( pthread_self() );
	int listen_fd;
	int com_fd;
	int ret;
	int i;
	int recval; 
	int len;
	int num;
	int plen;
	struct sockaddr_un srv_addr;
	struct sockaddr_un clt_addr;
	unlink(UNIX_DOMAIN);

	listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(listen_fd<0)
	{
		perror("cannot create communication socket");
		return;
	}  

	//set server addr_param
	srv_addr.sun_family=AF_UNIX;
	strcpy(srv_addr.sun_path,UNIX_DOMAIN);//,sizeof(srv_addr.sun_path)-1);
	//bind sockfd & addr
	int srv_len = sizeof(srv_addr);
	ret=bind(listen_fd,(struct sockaddr*)&srv_addr,srv_len);
	if(ret==-1)
	{
		perror("cannot bind server socket");
		close(listen_fd);
		unlink(UNIX_DOMAIN);
		return;
	}
	//listen sockfd 
	ret=listen(listen_fd,5);
	if(ret==-1)
	{
		perror("cannot listen the client connect request");
		close(listen_fd);
		unlink(UNIX_DOMAIN);
		return;
	}
	perror("listen() ");
	//have connect request use accept
	len=sizeof(clt_addr);
	while(1)
	{
		com_fd=accept(listen_fd,(struct sockaddr*)&srv_addr,(socklen_t *)&len);
		if(com_fd<0)
		{
		CA_LOG(LOG_MODULE, LOG_PROC, "error cannot accept client connect request\n");
			close(listen_fd);
			unlink(UNIX_DOMAIN);
			return;
		}
		plen = recv_ok;

		num=recv(com_fd,&recval,sizeof(recval),0);
		if (num < 0)
		{
			perror("recv()");
		CA_LOG(LOG_MODULE, LOG_PROC, "error recv to fail from client .....\n");
			continue;
		}
		CA_LOG(LOG_MODULE, LOG_PROC, "recv to socket need reload config .....\n");

		if ((ret = write (com_fd, (int*)&plen, sizeof(int))) == -1) {
			perror ("write()");
		}
		CA_LOG(LOG_MODULE, LOG_PROC, "socket server return sucess[%d] .....\n",ret);
		close(com_fd);
		config_add(recval);
	}
	close(listen_fd);
	unlink(UNIX_DOMAIN);
	return;
}

