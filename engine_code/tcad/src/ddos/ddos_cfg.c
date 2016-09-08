#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ddos_cfg.h"

#define cfg_debug(fmt, arg...) do\
{\
		printf("%s:%d ", __func__, __LINE__);\
		printf(fmt, ##arg);\
}while(0)
#define  ONE_LINE_LEN	1024

/****************************************************************
 *@path 配置文件路径
 *retval: 配置文件的行数，不包含注释
 ************************************************************** */
unsigned int ddos_get_ip_file_linenum(char *path)
{
	unsigned int linenum = 0;
	char one_line[ONE_LINE_LEN];
	FILE *fp;

	fp = fopen(path, "r");
	if(fp == NULL){
		cfg_debug("file '%s' not exist.\n", path);
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
int ddos_get_ip_file_data(char *path, ddos_network_iplist_t *array, int size)
{
	int i = 0;
	char one_line[ONE_LINE_LEN];
	char *str_sip, *str_eip, *str_plen;
	unsigned int sip, eip, plen;
	FILE *fp;
	char default_plen[4] = "/32";
	
	if(array == NULL){
		cfg_debug("array is null.\n");
		return -1;
	}
	fp = fopen(path, "r");
	if(fp == NULL){
		cfg_debug("file '%s' not exist.\n", path);
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


#if 0 
int main(void)
{
	int linenum;
	ddos_network_iplist_t *array;
	int datanum;
	int i;
	char *path = "./iplist.conf";

	
	linenum	 = ddos_get_ip_file_linenum(path);
	if(linenum == 0){
		printf("linenum is 0\n");
		return 0;
	}


	array = (ddos_network_iplist_t*)malloc(linenum * sizeof(ddos_network_iplist_t));
	if(array == NULL){
		printf("malloc error\n");
		return -1;
	}


	datanum = ddos_get_ip_file_data(path, array, linenum);
	
	printf("444, %d\n", datanum);
	//print
	if(datanum == 0){
		printf("datanum is 0\n");
	}
	for(i=0; i<datanum; i++)
		printf("ip:%u, plen:%d\n", array[i].ip, array[i].plen);

	free(array);


	return 0;
}
#endif 

