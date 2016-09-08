#include "socket_ser.h"
struct sockaddr_un srv_addr;
static int listen_fd;

int init_socket_listenfd(void)
{
	int ret;
	unlink(UNIX_DOMAIN);

	listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(listen_fd<0)
	{
		perror("cannot create communication socket");
		return -1;
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
		return -1;
	}
	return 0;
}
int listen_socket(void)
{
	int com_fd;
	int ret;
	char rev_to_ip[IP_LEN]={0}; 
	int num;
	char ip_addr_info[SEND_LEN]={0};
	struct sockaddr_un clt_addr;


	ret=listen(listen_fd,5);
	if(ret==-1)
	{
		perror("cannot listen the client connect request");
		close(listen_fd);
		unlink(UNIX_DOMAIN);
		return 1;
	}
	perror("listen() ");
	//have connect request use accept
	memset(&clt_addr,0,sizeof(clt_addr));
	num=sizeof(clt_addr);
	while(1)
	{
		com_fd=accept(listen_fd,(struct sockaddr*)&clt_addr,(socklen_t *)&num);
		if(com_fd<0)
		{
			perror("cannot accept client connect request");
			close(listen_fd);
			unlink(UNIX_DOMAIN);
			return 1;
		}
		

		num=recv(com_fd,rev_to_ip,sizeof(rev_to_ip),0);
		printf("Message from client (%d)) :%s\n",num,rev_to_ip);  
		//snprintf(ip_addr_info,SEND_LEN,"sdffff:1,sfdfd:2,dsfdsf:3");
		find_ip_addr(rev_to_ip,ip_addr_info);

		if ((ret = write (com_fd, ip_addr_info, sizeof(ip_addr_info))) == -1) {
			perror ("write()");
		}
		printf("write ret num :[%d]\n",ret);
		close(com_fd);
	}
	close(listen_fd);
	unlink(UNIX_DOMAIN);
	return 0;

}
int main(void)
{
	if(iplib_find_init()<0)
	{
		printf("init_iplib fail ... \n");
	}
	init_socket_listenfd();
	listen_socket();
	destroy_iplib();
	return 0;

}
