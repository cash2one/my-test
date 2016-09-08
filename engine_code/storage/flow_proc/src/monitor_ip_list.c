/*************************************************************************
	> File Name: monitor_ip_list.c
	> Author: zhangdawei
	> Mail: zhangdawei@chanct.com 
	> Created Time: Thu 20 Aug 2015 08:17:53 AM ICT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "list_ddos.h"
#include "monitor_ip_list.h"
#include "gms_flow.h"
#include "ddos_trie.h"



#define  NET_NETWORK_IPLIST_DIR		"/gms/storage/conf/iplist.conf"

struct trie *net_iplist_trie = NULL; 


unsigned int net_mnt_ip_lookup(unsigned int ip)
{
	if(likely(net_iplist_trie->trie != NULL)){
		if(!trie_table_lookup(net_iplist_trie, ip)){
		
			return IP_FOUND;
		}
	}
	return IP_NO_FOUND;
}
unsigned int net_get_ip_file_linenum(char *path)
{
	unsigned int linenum = 0;
	char one_line[ONE_LINE_LEN];
	FILE *fp;

	fp = fopen(path, "r");
	if(fp == NULL){
		mnt_debug("open %s failed.\n", path);
		return linenum;
	}
	
	if(feof(fp)){
		fclose(fp);
		return linenum;
	}

	while(!feof(fp)){
		one_line[0] = 0;
		if(!fgets(one_line, ONE_LINE_LEN, fp))
			break;
		if(one_line[0]>31 && one_line[0]!='#') /* 0-31  control character */
			linenum++;
	}

	fclose(fp);
	return linenum;
}
/***********************************************************
 *@path 配置文件路径
 *@array 存放 ip plen 的数组
 *@size	 数组大小
 *retval: 实际存入数组中的 ip plen个数
 * *******************************************************/
int net_get_ip_file_data(char *path, net_network_iplist_t *array, int size)
{
#if 0
	int i = 0;
	char one_line[ONE_LINE_LEN];
	char *pip, *pplen;
	unsigned int iip;
	int iplen;
	FILE *fp;
	char default_plen[4] = "/32";
	
	if(array == NULL){
		mnt_debug("array is null.\n");
		return -1;
	}
	fp = fopen(path, "r");
	if(fp == NULL){
		mnt_debug("open %s failed.\n", path);
		return -1;
	}
	if(feof(fp)){
		fclose(fp);
		return -1;
	}

	while(!feof(fp) && i<=size){
		if(!fgets(one_line, ONE_LINE_LEN, fp))
			break;
		if(one_line[0]>31 && one_line[0]!='#'){
			pip = one_line;
			pplen = strchr(one_line, '/');
			if(pplen == NULL){ /* only ip,  add default plen 32*/
	 	 		//continue;
		 		pplen = default_plen;
			}
			*pplen = 0;
			pplen++;
			iip = inet_addr(pip);
			iplen = atoi(pplen);
			if(iip==INADDR_NONE || !(iplen>0 && iplen<=32))
				continue;
			array[i].ip	  = iip;
			array[i].plen = iplen;
			i++;
		}
	}

	fclose(fp);
	return i;
#endif
	int i = 0;
	char one_line[ONE_LINE_LEN];
	char *str_sip, *str_eip, *str_plen;
	unsigned int sip, eip, plen;
	FILE *fp;
	char default_plen[4] = "/32";
	
	if(array == NULL){
		mnt_debug("array is null.\n");
		return -1;
	}
	fp = fopen(path, "r");
	if(fp == NULL){
		mnt_debug("file '%s' not exist.\n", path);
		return -1;
	}
	if(feof(fp)){
		fclose(fp);
		return -1;
	}

	while(!feof(fp) && i<=size){
		if(!fgets(one_line, ONE_LINE_LEN, fp))
			break;
		if(one_line[0]>31 && one_line[0]!='#'){
			//printf("line --------------------- %s \n",one_line);
			str_sip = one_line;
			str_eip = strchr(one_line, '-');
			if(str_eip != NULL){
				*str_eip = 0;
				str_eip += 1;
				array[i].type = ddos_type_ip_seg;
				sip = inet_addr(str_sip);
				eip = inet_addr(str_eip);
				if(sip==INADDR_NONE || eip==INADDR_NONE)
					continue;
				
				array[i].ip_seg.sip = sip;
				array[i].ip_seg.eip = eip;
				i++;
			}
			else{
				str_plen = strchr(one_line, '/');
				if(str_plen == NULL){ /* only ip,  add default plen 32*/
		 			str_plen = default_plen;
				}
				*str_plen = 0;
				str_plen += 1;
				array[i].type = ddos_type_ip_plen;
				sip = inet_addr(str_sip);
				plen = atoi(str_plen);
				if(sip==INADDR_NONE || !(plen>0 && plen<=32))
					continue;
				
				array[i].ip_plen.ip = sip;
				array[i].ip_plen.plen= plen;
				i++;
			}
		}
	}

	fclose(fp);
	return i;
}

static int mnt_load_config_from_file_per(void)
{
	struct trie * t;
	char *file_path;
	struct trie_ip_cfg ip_cfg;
	int linenum, datanum;
	int i;
	net_network_iplist_t *array;
	unsigned int sip, eip, ip;
	int ret_code = 0;
	
	t = net_iplist_trie;
	file_path = NET_NETWORK_IPLIST_DIR;
	
	linenum  = net_get_ip_file_linenum(file_path);
	if(linenum == 0){
		mnt_debug("linenum is 0.\n");
		return 0;
	}
	
	array = (net_network_iplist_t*)malloc(linenum * sizeof(net_network_iplist_t)); 
	if(array == NULL){
		mnt_debug("array malloc error.\n");
		return -1;
	}
	
	datanum = net_get_ip_file_data(file_path, array, linenum); 
	if(datanum == 0){
		mnt_debug("datanum is 0.\n");
		ret_code = -1;
		goto OUT1;
	}
#if 0
	for(i=0; i<datanum; i++){
		if(trie_table_insert(t, (struct trie_ip_cfg *)(&(array[i])))){
			mnt_debug("trie insert failed.\n");
			ret_code = -1;
			goto OUT1;
		}
	}
#endif
	for(i=0; i<datanum; i++){
		if(array[i].type == ddos_type_ip_seg){
			sip = ntohl(array[i].ip_seg.sip);
			eip = ntohl(array[i].ip_seg.eip);
			for(ip=sip; ip<=eip; ip++){
			printf("intip = %u ",ip);
				ip_cfg.ip = htonl(ip);
				ip_cfg.plen = 32;

				if(trie_table_insert(t, &ip_cfg)){
					mnt_debug("trie insert failed.\n");
					ret_code = -1;
					goto OUT1;
				}
			}
			
		}
		else if(array[i].type == ddos_type_ip_plen){
			ip_cfg.ip   = array[i].ip_plen.ip;
			ip_cfg.plen = array[i].ip_plen.plen;
			if(trie_table_insert(t, &ip_cfg)){
				mnt_debug("trie insert failed.\n");
				ret_code = -1;
				goto OUT1;
			}
		}
	}

OUT1:
	free(array);
	array = NULL;
	
	return ret_code;
}

int net_trie_init(void)
{
	
	net_iplist_trie = (struct trie*)malloc(sizeof(struct trie));
	if(NULL == net_iplist_trie){
		mnt_debug("g_ddos_iplist_trie malloc error.\n");
		return -1;
	}
	memset(net_iplist_trie, 0x0, sizeof(struct trie));

	return 0;
}

int net_mnt_ip_init(void)
{	

	/** init trie **/	
	if(net_trie_init()){
		mnt_debug("ddos trie init failed.\n");
		return -1;
	}
	/* load configure from file */
	if( mnt_load_config_from_file_per())
	{
		mnt_debug("load config failed.\n");
		return -1;
	}

	return 0;
}

void net_mnt_ip_finish(void)
{
#if 0
	mnt_destory_htable();
#endif 
	/** distroy trie**/
	ddos_trie_fini();
	return;
}
void reload_config(void)
{
	int retn;
	/* 初始化环境 */
	/* 读取配置信息 */
	retn = common_read_conf(FLOW_CONFIG_PATH, g_flow_cfg);
	if (retn < 0) {
		fprintf(stderr, "[-][FLOW_PROC] Read config %s ERROR!\n", FLOW_CONFIG_PATH);
		CA_LOG(LOG_MODULE, LOG_PROC, "[-][FLOW_PROC] Read config %s ERROR!\n", FLOW_CONFIG_PATH);
		
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "start reload ddos_trie_fini() .....\n");
	ddos_trie_fini();
	CA_LOG(LOG_MODULE, LOG_PROC, "start reload net_mnt_ip_init() .....\n");
	net_mnt_ip_init();
}
