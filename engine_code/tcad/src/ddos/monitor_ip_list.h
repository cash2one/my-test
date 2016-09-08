/*************************************************************************
	> File Name: monitor_ip_list.h
	> Author: zhangjieming
	> Mail: zhangjieming@chanct.com 
	> Created Time: Thu 20 Aug 2015 08:23:55 AM ICT
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


#define MNT_LIST_HASH_MAX 1024



typedef struct mnt_ip_node {
	struct hlist_node list;
	unsigned int ip;
	unsigned int flag;
}mnt_ip_node_t;

typedef struct mnt_ip_head {
	struct hlist_head head;
	rwlock_t lock;
}mnt_ip_head_t;

void ddos_mnt_ip_finish(void);
int ddos_mnt_ip_init(void);
unsigned int ddos_mnt_ip_lookup(unsigned int ip);
#endif

