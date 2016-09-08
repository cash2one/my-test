#ifndef __SOCKET_SER_H_
#define __SOCKET_SER_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <unistd.h>
#define UNIX_DOMAIN "/tmp/IPADDR.domain"
#define IP_LEN 16
#define SEND_LEN 100
enum ret_result{
	recv_ok =0,
	recv_fail

};
extern int iplib_find_init(void);
extern int find_ip_addr(char *ip,char *out_str);
extern int destroy_iplib(void);
#endif
