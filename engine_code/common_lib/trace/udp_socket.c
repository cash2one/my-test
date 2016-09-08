#include <sys/socket.h>
#include <fcntl.h>
#include <linux/un.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>

#include "trace_api.h"

extern trace_udp_t udp_sk;
extern int msg_len;

#define CLI_PATH "/tmp/.tmp"
char client_path[12];
/********************************************************
* create a udp socket, which used to send message to collector
* input: path name
* return:
*	>=0	success, the fd of udp socket
*	-1	init failed
*********************************************************/
int udpsocket_init(char *path)
{
	struct sockaddr_un cli_addr;
    	int val;
    	int sock_fd;
	int len;

	memset(&cli_addr, 0, sizeof(cli_addr));

	cli_addr.sun_family = AF_UNIX;
	memset(client_path, 0, sizeof(client_path));
	sprintf(client_path, "%s%05d", CLI_PATH, getpid());
	strncpy(cli_addr.sun_path, client_path, strlen(client_path));
#ifdef VENUS_TRACE_DEBUG
	printf("in udpsocket_init the client_path is:%s\n", client_path);
#endif
//	strcpy(cli_addr.sun_path, tmpnam(NULL));

	len = offsetof(struct sockaddr_un,sun_path) + strlen(cli_addr.sun_path);

	unlink(cli_addr.sun_path) ; /* remove the socket if it already exists. 
			  You will get an EINVAL error if the file is already there*/
	
#ifdef VENUS_TRACE_DEBUG
	printf("in udpsocket_init\n");
#endif
    	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if ( -1 == sock_fd ) {
		fprintf(stderr,"init udp socket failed,errno:%d(%s)\n",
				errno, strerror(errno));
        	goto err;
    	}

    	if ( -1 == (val = fcntl(sock_fd,F_GETFL,0)) ) {
		fprintf(stderr,"fcntl F_GETFL failed,errno:%d(%s)\n",
				errno, strerror(errno));
		goto err;
	}

    	val |= O_NONBLOCK;
    	if ( -1 == (val = fcntl(sock_fd, F_SETFL, val)) ) {
        	fprintf(stderr,"fcntl F_SETFL, O_NONBLOCK,failed,errno:%d(%s)\n",
				errno, strerror(errno));
        	goto err;
    	}

    	/*memset((char*)&addr,0, sizeof(addr));
    	addr.sun_family      = AF_UNIX;
	if ( path ) {
    		strncpy(addr.sun_path, path, UNIX_PATH_MAX);
	}*/

	/* we bind the sender's addr to its socket */
	if (-1 == bind(sock_fd, (struct sockaddr *)&cli_addr, len) ) {
      		fprintf(stderr,"bind socket fd failed,errno:%d(%s)\n",
				errno, strerror(errno));
        	goto err;
    	}

#ifdef VENUS_TRACE_DEBUG
	printf("after udpsocket_init\n");
#endif
	return sock_fd;

err:
	return -1;
}
/************************************************************
* receive message from a UDP socket
* input:
* return:
*	>=0 success
*	<0  failed
*************************************************************/	
int sock_recv(int aFd,char *path, char *aBuf,int aNum)
{
	struct sockaddr_un addr;
    	int    addr_len,n;

    	memset((char *)&addr,0,sizeof(struct sockaddr_un));
    	addr.sun_family      = AF_UNIX;
    	addr_len = sizeof(addr);

    	n = recvfrom(aFd,aBuf,aNum,0,(struct sockaddr*)&addr,(socklen_t *)&addr_len);
    
	if  ( -1 == n ) {
        	if ( errno != EWOULDBLOCK ) {
			static int i=0;
	
			i++;

			if ( i < 5 ) {
            			fprintf(stderr,"recvfrom failed,errno:%d(%s)\n",
						errno, strerror(errno));
			}
		}
    	}


    return n;
}

/*********************************************************
* send a message to a UDP socket fd
* Input:
* Return:
*
**********************************************************/
int sock_send(int aFd,char *path,char *aBuf,int aNum)
{
	struct sockaddr_un addr;
    	int    n,addr_len = sizeof(addr);

	if ( '\0' == path[0] ) {
    		fprintf(stderr, "sendto path null\n");
		goto err;
	}

#ifdef VENUS_TRACE_DEBUG
	printf("\nin sock_send socket:%d,path:%s,buf:%s,anum:%d\n",aFd,path,aBuf,aNum);
#endif
	addr_len = sizeof(addr);
    	memset((char *)&addr,0,sizeof(struct sockaddr_un));
    	addr.sun_family      = AF_UNIX;
    	strncpy(addr.sun_path, path, UNIX_PATH_MAX);
	
    	if ( -1 == (n = sendto(aFd,aBuf,aNum,0,
			(struct sockaddr*)&addr,sizeof(addr))) ) {
		if ( errno!=EWOULDBLOCK ) {
			static int i=0;
			i++;
			if ( i < 5 ) {
    	//    			fprintf(stderr,"sendto failed====:%d(%s)\n",
	//				errno, strerror(errno));
			}
		} 
	}

	return n;
err:
	return 0;
}

/**************************************
* close a socket fd
* input: aFd a udp socket fd
* Return:
*	0	always
****************************************/
int udpsocket_close(int aFd)
{
	if ( aFd != -1) {
		close(aFd);
	}
#ifdef VENUS_TRACE_DEBUG
	printf(" in udpsocket_close the client_path:%s\n", client_path);
#endif
 	unlink(client_path);  
	return 0;
}
 
