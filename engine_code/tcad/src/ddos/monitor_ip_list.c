/*************************************************************************
	> File Name: monitor_ip_list.c
	> Author: zhangjieming
	> Mail: zhangjieming@chanct.com 
	> Created Time: Thu 20 Aug 2015 08:17:53 AM ICT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "list.h"
#include "rwlock.h"
#include "ddos.h"
#include "monitor_ip_list.h"
#include "common.h"

#include "ddos_trie.h"
#include "ddos_cfg.h"
#include "../monitor/statistics.h"



#define  DDOS_NETWORK_IPLIST_DIR		DDOS_CONFIG_DIR"/iplist.conf"
#define  DDOS_NETWORK_WHITELIST_DIR		DDOS_CONFIG_DIR"/whitelist.conf"

/* monitor ip hash table */
mnt_ip_head_t g_mnt_ip_htable[MNT_LIST_HASH_MAX];

static inline unsigned int mnt_get_ip_hash(unsigned int ip)
{

	return ip%(MNT_LIST_HASH_MAX-1);
}

#if 0 
static int mnt_add_ip(unsigned int ip, unsigned int flag)
{
	mnt_ip_node_t *node;
	struct hlist_node *pos, *n;
	unsigned int hash = mnt_get_ip_hash(ip);
	mnt_ip_head_t *head = &g_mnt_ip_htable[hash];
	
	read_lock(&head->lock);
	hlist_for_each_entry_safe(node, pos, n, &head->head, list){
		if (node->ip == ip) {
			read_unlock(&head->lock);
			return 0;
		}
	}
	read_unlock(&head->lock);

	node = malloc(sizeof(*node));
	if (!node) {
		mnt_debug("malloc no memory\n");
		return -1;
	}
	
	node->ip = ip;
	node->flag = flag;

	write_lock(&head->lock);
	hlist_add_head(&node->list, &head->head);
	write_unlock(&head->lock);

	return 0;
}
#endif

#if 0
static int mnt_del_ip(unsigned int ip)
{
	mnt_ip_node_t *node;
	struct hlist_node *pos, *n;
	unsigned int hash = mnt_get_ip_hash(ip);
	mnt_ip_head_t *head = &g_mnt_ip_htable[hash];
	
	write_lock(&head->lock);
	hlist_for_each_entry_safe(node, pos, n, &head->head, list){
		if (node->ip == ip) {
			hlist_del(&node->list);
			free(node);
			write_unlock(&head->lock);
			return 0;
		}
	}
	write_unlock(&head->lock);

	return 0;
}

static int mnt_modify_ip(unsigned int ip, unsigned int flag)
{
	mnt_ip_node_t *node;
	struct hlist_node *pos, *n;
	unsigned int hash = mnt_get_ip_hash(ip);
	mnt_ip_head_t *head = &g_mnt_ip_htable[hash];
	
	write_lock(&head->lock);
	hlist_for_each_entry_safe(node, pos, n, &head->head, list){
		if (node->ip == ip) {
			node->flag = flag;
			write_unlock(&head->lock);
			return 0;
		}
	}
	write_unlock(&head->lock);

	return 0;
}
#endif

#if 0
static void mnt_destory_htable(void)
{
	int i;
	mnt_ip_node_t *node;
	struct hlist_node *pos, *n;
	mnt_ip_head_t *head;

	for(i=0; i<MNT_LIST_HASH_MAX; i++)
	{
		head = &g_mnt_ip_htable[i];
		write_lock(&head->lock);
		hlist_for_each_entry_safe(node, pos, n, &head->head, list){
			hlist_del(&node->list);
			free(node);
		}
		write_unlock(&head->lock);
	}

	return ;
}
#endif 


unsigned int ddos_mnt_ip_lookup(unsigned int ip)
{

	/** first lookup white list, white list priority high**/
	if(likely(g_ddos_whitelist_trie->trie != NULL)){
		if(!trie_table_lookup(g_ddos_whitelist_trie, ip))
			return e_ddos_ip_type_white;
	}

	if(likely(g_ddos_iplist_trie->trie != NULL)){
		if(!trie_table_lookup(g_ddos_iplist_trie, ip))
			return e_ddos_ip_type_monitor;
	}

#if 0
	mnt_ip_node_t *node;
	struct hlist_node *pos, *n;
	unsigned int hash = mnt_get_ip_hash(ip);
	mnt_ip_head_t *head = &g_mnt_ip_htable[hash];
	
	read_lock(&head->lock);
	hlist_for_each_entry_safe(node, pos, n, &head->head, list){
		if (node->ip == ip) {
			read_unlock(&head->lock);
			return node->flag;
		}
	}
	read_unlock(&head->lock);
#endif 

	return e_ddos_ip_type_not_found;
}

static int mnt_load_config_from_file_per(int mon_ip_type)
{
	struct trie * t;
	char *file_path;
	int linenum, datanum;
	int i;
	ddos_network_iplist_t *array;
	struct ddos_trie_ipinfo *tip_info;
	struct trie_ip_cfg ip_cfg;
	unsigned int sip, eip, ip;

	
	int ret_code = 0;
	
	switch(mon_ip_type){
		case e_ddos_ip_type_monitor:
			t = g_ddos_iplist_trie;
			file_path = DDOS_NETWORK_IPLIST_DIR;
			break;
		case e_ddos_ip_type_white:
			t = g_ddos_whitelist_trie;
			file_path = DDOS_NETWORK_WHITELIST_DIR;
			break;

		default:
			return 0;
	}
	
	linenum  = ddos_get_ip_file_linenum(file_path);
	if(linenum == 0){
		mnt_debug("linenum is 0.\n");
		return 0;
	}
	
	array = (ddos_network_iplist_t*)malloc(linenum * sizeof(ddos_network_iplist_t)); 
	if(array == NULL){
		mnt_debug("array malloc error.\n");
		return -1;
	}
	
	datanum = ddos_get_ip_file_data(file_path, array, linenum); 
	if(datanum == 0){
		mnt_debug("datanum is 0.\n");
		ret_code = -1;
		goto OUT;
	}

	/** for debug**/
	if(mon_ip_type == e_ddos_ip_type_monitor){
		DEBUG_DDOS_TRIE_MONITOR_COUNT(datanum);
		tip_info = DEBUG_DDOS_TRIE_MONITOR_TIP;
	}
	else if(mon_ip_type == e_ddos_ip_type_white){
		DEBUG_DDOS_TRIE_WHITE_COUNT(datanum);
		tip_info = DEBUG_DDOS_TRIE_WHITE_TIP;
	}
	
	for(i=0; i<datanum; i++){
		if(array[i].type == ddos_type_ip_seg){
			sip = ntohl(array[i].ip_seg.sip);
			eip = ntohl(array[i].ip_seg.eip);
			for(ip=sip; ip<=eip; ip++){
				ip_cfg.ip = htonl(ip);
				ip_cfg.plen = 32;

				if(trie_table_insert(t, &ip_cfg)){
					mnt_debug("trie insert failed.\n");
					ret_code = -1;
					goto OUT;
				}
			}
			
			if(tip_info && i<DDOS_IP_RULE_SHOW_MAX_SIZE){
				tip_info[i].type = array[i].type;
				tip_info[i].ip   = htonl(sip);
				tip_info[i].plen = htonl(eip);
			}
		}
		else if(array[i].type == ddos_type_ip_plen){
			ip_cfg.ip   = array[i].ip_plen.ip;
			ip_cfg.plen = array[i].ip_plen.plen;
			if(trie_table_insert(t, &ip_cfg)){
				mnt_debug("trie insert failed.\n");
				ret_code = -1;
				goto OUT;
			}
			else{
				if(tip_info && i<DDOS_IP_RULE_SHOW_MAX_SIZE){
					tip_info[i].type = array[i].type;
					tip_info[i].ip   = array[i].ip_plen.ip;
					tip_info[i].plen = array[i].ip_plen.plen;
				}
			}
		}
	}

OUT:
	free(array);
	array = NULL;
	
	return ret_code;
}

static int mnt_load_config_from_file(void)
{
	/** load monitor ip list **/
	if(mnt_load_config_from_file_per(e_ddos_ip_type_monitor))
		return -1;

	/** load white ip list **/
	if(mnt_load_config_from_file_per(e_ddos_ip_type_white))
		return -1;
	
	return 0;
}

int ddos_mnt_ip_init(void)
{	
#if 0 
	int i;
	/* init hash table */
	for(i=0; i<MNT_LIST_HASH_MAX; i++)
	{
		/* init rwlock */
		rwlock_init(&g_mnt_ip_htable[i].lock);
		INIT_HLIST_HEAD(&g_mnt_ip_htable[i].head);
	}
#endif

	/** init trie **/	
	if(ddos_trie_init()){
		mnt_debug("ddos trie init failed.\n");
		return -1;
	}
	/* load configure from file */
	if(mnt_load_config_from_file())
	{
		mnt_debug("load config failed.\n");
		return -1;
	}

	return 0;
}

void ddos_mnt_ip_finish(void)
{
#if 0
	mnt_destory_htable();
#endif 
	/** distroy trie**/
	ddos_trie_fini();
	return;
}

