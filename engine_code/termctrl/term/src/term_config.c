#include "term_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

int get_ethernet_number()
{
        int sockt;
        struct ifreq ifr;
      //  unsigned char *p;
        int i,ret;

        bzero(&ifr, sizeof(ifr));
        sockt = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockt < 0)
                return -1;
        for(i = 0; i < 20; i++)
        {
                sprintf(ifr.ifr_ifrn.ifrn_name, "eth%d", i);
                if((ret = ioctl(sockt, SIOCGIFHWADDR, &ifr)) < 0)
                {
                                break;
                }else{  //try next device
						if ( !i )
                        //	memcpy(eth0_mac,ifr.ifr_ifru.ifru_hwaddr.sa_data,6);
                        ioctl(sockt,SIOCGIFADDR,&ifr);
                     //   p = ifr.ifr_ifru.ifru_hwaddr.sa_data;
                }
        }
        close(sockt);

        return i;
}
int read_system_msg()
{
#if 1
      int i;	
	for(i = 0; i < total_ethernet_number; i++) {
		if(pgb_var->eth_have_ip[i] == TYPE_COM) {
			set_ethernet_msg(i, CHANCT_DEF_IP, CHANCT_DEF_NETMASK,CHANCT_DEF_GW,CHANCT_DEF_DNS);
			
		}else{
			set_ethernet_null(i);
		}
	}
#endif
			
	return 0;		
	
}
int save_system_msg()
{
	int iret;
	iret = 0;
		if (IsValidIpaddr(gb_ip_temp)&&IsValidIpaddr(gb_netmask_temp))
		{
			iret=set_ethernet_msg(which,
						gb_ip_temp,
						gb_netmask_temp,gb_gateway_temp,gb_dns);
			if(iret<=0)
				iret = 0;
		}	
		else
		{
			iret = 0;
		}	
	return iret;	
}
