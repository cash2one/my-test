/*************************************************************************
	> File Name: monitor_ip_list.h
	> Author: zhangdawei
	> Mail: zhangdawei@chanct.com 
	> Created Time: Thu 20 sep 2015 08:23:55 AM ICT
 ************************************************************************/

#ifndef __MONITOR_IP_LIST_H__
#define __MONITOR_IP_LIST_H__


#define MNT_DEBUG

#ifdef MNT_DEBUG
#define mnt_debug(fmt, arg...) do\
{\
	printf("%s:%d ", __func__, __LINE__);\
	printf(fmt, ##arg);\
}while (0)
#else
#define mnt_debug(fmt, arg...) do {} while (0)
	
#endif

#define ONE_LINE_LEN  1024
#define MNT_LIST_HASH_MAX 1024
enum ddos_network_iplist_type{
     ddos_type_ip_plen = 0,
     ddos_type_ip_seg, 
 };          


enum {
	IP_NO_FOUND,
	IP_FOUND

};
enum {
	flow_in = 0,
	flow_out,
	flow_num
};

typedef struct net_network_iplist{
	//unsigned int ip;
	//int plen;
	int type;
	union{
		struct{
			unsigned int ip;
			unsigned int plen;
		}ip_plen;
		struct{
			unsigned int sip;
			unsigned int eip;
		}ip_seg;
	};
}net_network_iplist_t;



void net_mnt_ip_finish(void);
int net_mnt_ip_init(void);
unsigned int net_mnt_ip_lookup(unsigned int ip);
int net_init(void);
void reload_config(void);
#endif

