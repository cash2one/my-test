#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include "vis24.h"
#include <linux/sockios.h>
#include <ctype.h>
#include <limits.h>
#define MAX_ADDR_LEN    32
#define ETHTOOL_GPERMADDR   0x00000020 /* Get permanent hardware address */
struct ethtool_perm_addr {
	unsigned int   cmd;
	unsigned int   size;
	unsigned char    data[0];
};
/*
 * get_hwaddr
 *   get mac addr of interface to hwaddr
 *
 * input:
 *   interface, char[IFNAMSIZ=16], such as "eth0"
 *   
 * output:
 *   hwaddr, unsigned char[IFHWADDRLEN=6], such as "\x00\xd0\xb7\xb6\x1a\x37", no ending '\0'
 *
 * return value:
 *   0: success
 *   -1: failure
 */
int get_hwaddr(unsigned char* hwaddr, const char *interface)
{
	int sd,i;
	struct ifreq req;
	struct ethtool_perm_addr *epaddr;

   epaddr = malloc(sizeof(struct ethtool_perm_addr) + MAX_ADDR_LEN);
     epaddr->cmd = ETHTOOL_GPERMADDR;
     epaddr->size = MAX_ADDR_LEN; 
	sd = socket(AF_INET,SOCK_DGRAM,0);
	if (sd == -1) {
		perror("socket()");
		return -1;
	}
	req.ifr_data=(void*)epaddr;
	sprintf(req.ifr_name,"%s",interface);
	//if (ioctl(sd,SIOCGIFHWADDR,&req) == -1) {
	if (ioctl(sd,SIOCETHTOOL,&req) == -1) {
		perror("ioctl():SIOCETHTOOL");
	  return -1;
	}
#if 0
	printf("Permanent address:");
	for (i = 0; i < epaddr->size; i++)
	{	printf("%c%02x", (i == 0) ? ' ' : ':',
			epaddr->data[i]);
	//sprintf(buf,"%02x",epaddr->data[i]);

	}
	printf("\n");
#endif
	memcpy(hwaddr, epaddr->data,6);
	free(epaddr);
#ifdef DEBUG
	printf("hwaddr of %s is %02x:%02x:%02x:%02x:%02x:%02x",
	       interface,
		(int)(req.ifr_hwaddr.sa_data[0] & 0xff),
		(int)(req.ifr_hwaddr.sa_data[1] & 0xff),
		(int)(req.ifr_hwaddr.sa_data[2] & 0xff),
		(int)(req.ifr_hwaddr.sa_data[3] & 0xff),
		(int)(req.ifr_hwaddr.sa_data[4] & 0xff),
		(int)(req.ifr_hwaddr.sa_data[5] & 0xff)); 
#endif

	return 0;
}
void fromt_ture(char *DevID)
{
	int i,j;
	char *tmp=DevID;
	char buf[32];
	bzero(buf,32);
	j=1;
	for(i=0;i<strlen(tmp);i++)
	{
		if(tmp[i]!='-')
		{
			buf[j-1]=tmp[i];
			j++;
			if((j%5==0) && (i != strlen(tmp)-1))
			{
				buf[j-1]='-';
				j++;
			}
		}
		
	}
	strcpy(DevID,buf);
}
int Dev_id(char *DevID)
{
        char hwaddr[IFHWADDRLEN];
	//char DevID[32];
	int r;
	//FILE * fp;
	//fp = fopen("/etc/DevID", "w+");
	get_hwaddr(hwaddr,"eth0");
	r=vis24_encode(DevID,32,hwaddr,IFHWADDRLEN);
	r=dash4(DevID,32);
	fromt_ture(DevID);
	//fprintf(fp,"DevID is %s\n", DevID);
	//fclose(fp);
	//printf("DevID is %s\n", DevID);
	return 0;
}

int main()
{
        char hwaddr[IFHWADDRLEN];
	char DevID[32];
	int r;
	//FILE * fp;
	//fp = fopen("/etc/DevID", "w+");
	get_hwaddr(hwaddr,"eth0");
	//printf("%s\n",hwaddr);
	//getchar();
	r=vis24_encode(DevID,32,hwaddr,IFHWADDRLEN);
	r=dash4(DevID,32);
	fromt_ture(DevID);
	//fprintf(fp,"DevID is %s\n", DevID);
	//fclose(fp);
	printf("%s\n", DevID);
	return 0;
}
