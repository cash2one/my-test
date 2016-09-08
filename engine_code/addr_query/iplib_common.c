#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "iplib.h"
#include "socket_ser.h"

int iplib_find_init(void)
{

    if (IPLIB_OK != iplib_set_dir(IPLIB_DIR))
    {
		printf("iplib_set_dir error\n");
		return -1;
    }
   
	if (IPLIB_OK != iplib_init())
	{
		printf("iplib_init error\n");
		return -1;
	}
	return 0;
}
    
int find_ip_addr(char *ip,char *out_str)
{
	unsigned int host_ip;
	iplib_info_t ipinfo;
	memset(&ipinfo, 0, sizeof(iplib_info_t));
	//printf("input find_ip_addr .....%s \n",ip);
	//fflush(stdout);	
	host_ip = ntohl(inet_addr(ip)); 
	//snprintf(stderr,50,"ntohl(inet_addr(ip)) jou\n");
	if (IPLIB_OK == iplib_find(host_ip, &ipinfo))
	{
		printf("iplib_find success.\n");
		printf("ipinfo: country %u, province_id %u, city_id %u, isp_id %u .\n",
				ipinfo.country, ipinfo.province_id, ipinfo.city_id, ipinfo.isp_id);
	}
	else
	{
		printf("iplib_find failed.\n");
	}
	snprintf(out_str,(ssize_t)SEND_LEN,"country %u, province_id %u, city_id %u\n",ipinfo.country, ipinfo.province_id, ipinfo.city_id);
		
    return 0;
}		
int destroy_iplib(void)
{	
	iplib_destroy();

    return 0;
}
