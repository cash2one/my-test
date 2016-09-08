/*************************************************************************
	> File Name: gms_udp_client.c
	> Created Time: Fri 11 Sep 2015 04:27:17 PM ICT
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <atomic.h>


#include <list_ddos.h> 
#include <rwlock.h>
#include <gms_udp_sock.h>
#include "gms_flow.h"


#define  	ONE_LINE_LEN	1024
#define		GMS_UDP_SERVER_CONF_PATH	"/gms/storage/conf/gms_udp_sock.conf"
struct udp_server_info{
	char ip[16];
	char port[8];
	unsigned int timeout; //sec
};

struct udp_server_info g_udp_server_info = {"192.168.0.1", "61440", 1};

atomic32_t  g_need_update_db_flag;

#define RECV_BUF_SIZE	2048
char g_recv_buf[RECV_BUF_SIZE];

#define RECV_SUCCESS	0x80000000
#define RECV_FAILED		0x8080000
#define RECV_SUB_CMD	0x00000000

typedef struct gms_ip_account_hash{
	struct	list_head head;
	rwlock_t lock;
}gms_ip_account_hash_t;
#define GMS_IP_ACCOUNT_HASH_SZ	1024
#define GMS_IP_ACCOUNT_HASH_MASK (GMS_IP_ACCOUNT_HASH_SZ-1)
gms_ip_account_hash_t g_gms_ip_account_hash[GMS_IP_ACCOUNT_HASH_SZ];
static inline unsigned int get_hash(unsigned int key)
{
	return (key%GMS_IP_ACCOUNT_HASH_MASK);
}

typedef struct gms_ip_account_info{
	struct list_head list;
	unsigned int ip;
	char accout[32];
	char time[20];
}gms_ip_account_info_t;

struct list_head g_data_list_head;
pthread_mutex_t  g_data_lock;
typedef struct data_list{
	struct list_head list;
	int recv_len;
	char *buf;
}data_list_t;

int g_thread_run;


/** read udp server ip and port from file **/
int gms_read_server_conf(char *path)
{
	char one_line[ONE_LINE_LEN];
	FILE *fp;

	fp = fopen(path, "r");
	if(fp == NULL){
		printf("file '%s' not exist.\n", path);
		return -1;
	}

	if(feof(fp)){
		fclose(fp);
		return -1;
	}

	while(!feof(fp)){
		one_line[0] = 0;
		if(!fgets(one_line, ONE_LINE_LEN, fp))
			break;
		if(one_line[0]>31 && one_line[0]!='#') {/* 0-31  control character */
			if(strncmp(one_line, "ip=", 3) == 0){
				
				strcpy(g_udp_server_info.ip, one_line+3);
			}else if(strncmp(one_line, "port=", 5) == 0){

				strcpy(g_udp_server_info.port, one_line+5);
			}else if(strncmp(one_line, "timeout=", strlen("timeout="))==0){

				g_udp_server_info.timeout = atoi(one_line+strlen("timeout="));
			}else{;}
		}
	}

	printf("serverinfo ip:%s, port:%s, timeout:%d \n", 
		g_udp_server_info.ip, g_udp_server_info.port, g_udp_server_info.timeout);
	fclose(fp);

	return 0;
}

void update(data_list_t *data)
{
	int i, count = 0;
	struct gms_recv_ip_account *buf = NULL;
	unsigned int ip;
	unsigned int hash;
	unsigned char event_type;
	gms_ip_account_info_t *info, *pos, *n;
	
	
	count = (data->recv_len)/sizeof(struct gms_recv_ip_account);
	buf = (struct gms_recv_ip_account*)data->buf;

	if(count && buf)
		atomic32_set(&g_need_update_db_flag, 1);
	
	for(i=0; i<count; i++){
		//ip = ntohl(buf[i].ip);
		ip = buf[i].ip;
		event_type = buf[i].event_type;
		hash = get_hash(ip);

		//printf("-----------ip:%u, account:%s, time:%s, type:%d\n", ip, buf[i].accout, buf[i].time, event_type);
		//CA_LOG(LOG_MODULE, LOG_PROC,"-----------ip:%u, account:%s, time:%s, type:%d\n", ip, buf[i].accout, buf[i].time, event_type);
		
		if(event_type == 1){ /* login */
			info = (gms_ip_account_info_t *)malloc(sizeof(gms_ip_account_info_t));
			if(info){
				info->ip = ip;
				strcpy(info->accout, buf[i].accout);
				strcpy(info->time, buf[i].time);
				write_lock(&(g_gms_ip_account_hash[hash].lock));
				list_add(&(info->list), &(g_gms_ip_account_hash[hash].head));
				write_unlock(&(g_gms_ip_account_hash[hash].lock));
			}
		}else if(event_type == 2){ /* logout */
			write_lock(&(g_gms_ip_account_hash[hash].lock));
			list_for_each_entry_safe(pos, n, &(g_gms_ip_account_hash[hash].head), list)
			{
				if(pos->ip==ip && 
					strncmp(pos->accout, buf[i].accout, sizeof(pos->accout))==0){
					list_del(&(pos->list));
					free(pos);
					pos = NULL;
				}
			}
			write_unlock(&(g_gms_ip_account_hash[hash].lock));
		}else if(event_type == 3){ /* logout */
			write_lock(&(g_gms_ip_account_hash[hash].lock));
			list_for_each_entry_safe(pos, n, &(g_gms_ip_account_hash[hash].head), list)
			{
				if(pos->ip==ip && 
					strncmp(pos->accout, buf[i].accout, sizeof(pos->accout))==0){
					list_del(&(pos->list));
					free(pos);
					pos = NULL;
				}
			}
			write_unlock(&(g_gms_ip_account_hash[hash].lock));
			info = (gms_ip_account_info_t *)malloc(sizeof(gms_ip_account_info_t));
			if(info){
				info->ip = ip;
				strcpy(info->accout, buf[i].accout);
				strcpy(info->time, buf[i].time);
				write_lock(&(g_gms_ip_account_hash[hash].lock));
				list_add(&(info->list), &(g_gms_ip_account_hash[hash].head));
				write_unlock(&(g_gms_ip_account_hash[hash].lock));
			}
		}
	}

}
void * parse_data_handle(void *arg)
{
	data_list_t *pos, *n;

	while(g_thread_run){
		if(list_empty(&g_data_list_head)){
			sleep(1);
			//printf("parse_data_handle sleep 1s.\n");
		}else{
			pthread_mutex_lock(&g_data_lock);
			list_for_each_entry_safe(pos, n, &g_data_list_head, list){
				update(pos);
				free(pos->buf);
				pos->buf = NULL;
				list_del(&(pos->list));
				free(pos);
				pos = NULL;
			}
			pthread_mutex_unlock(&g_data_lock);
		}
	}
	
	return NULL;
}

void * recv_handle(void *arg)
{
	struct sockaddr_in addr_rcv, addr_send;	
	int fd;
	int opt;
	int recv_len, send_len;
	data_list_t *data;
	struct gms_send_ip_account send_msg;
	unsigned int addr_len = sizeof(addr_rcv);


	fd = socket(AF_INET, SOCK_DGRAM, 0);	
	if(fd < 0)	{		
		printf("socket error, %s\n", strerror(errno));		
		return NULL;
	}
	/* set recv buf size*/
	opt = RECV_BUF_SIZE;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const void *)&opt, sizeof(int));
	/* allow address reuse */ 
	opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));

	memset(&addr_rcv, 0x0, sizeof(addr_rcv));
	addr_rcv.sin_family = AF_INET;	
	addr_rcv.sin_port = htons(atoi(g_udp_server_info.port));
	//addr_rcv.sin_addr.s_addr = inet_addr(g_udp_server_info.ip);	
	addr_rcv.sin_addr.s_addr = htonl(INADDR_ANY);	
	if(bind(fd, (struct sockaddr *)&addr_rcv, sizeof(addr_rcv))){
		printf("bind error, %s\n", strerror(errno));
	}

	while(g_thread_run){	

		recv_len = recvfrom(fd, g_recv_buf, RECV_BUF_SIZE, 0, 
						(struct sockaddr *)&addr_send, &addr_len);
				
		if(recv_len <= 0){
			send_msg.cmd_id = RECV_FAILED;
		}else {
		
			data = (data_list_t*)malloc(sizeof(
data_list_t));
			if(data == NULL){
				printf("malloc error 1.\n");
				goto MALLOC_ERR;
			}else{
				data->buf = malloc(recv_len+1);
				if(data->buf == NULL){
					printf("malloc error 2.\n");
					goto MALLOC_ERR;
				}else{
					data->recv_len = recv_len;
					memcpy(data->buf, g_recv_buf, recv_len);
					data->buf[recv_len] = 0;
					send_msg.cmd_id = RECV_SUCCESS;
					pthread_mutex_lock(&g_data_lock);
					list_add(&(data->list), &g_data_list_head);
					pthread_mutex_unlock(&g_data_lock);
				}
			}
		}
MALLOC_ERR:;
#if 1	
		/*  send msg to server */
		send_msg.sub_cmd_id = RECV_SUB_CMD;
		send_len = sendto(fd, &send_msg, sizeof(send_msg), 0, 
							(struct sockaddr *)&addr_send, addr_len);
		if(send_len <=0){
			printf("snedto error, %s\n", strerror(errno));
		}	
#endif 
	}
	
	close(fd);
	return NULL;
}

void * update_db_handle(void *arg)
{
	int i;
	gms_ip_account_info_t *pos;

	
	while(g_thread_run){

		if(atomic32_read(&g_need_update_db_flag)==0){
			sleep(g_udp_server_info.timeout);
			//printf("update_db_handle  sleep %ds.\n", g_udp_server_info.timeout);
			continue;
		}else{
			atomic32_set(&g_need_update_db_flag, 0);
		}
		
		for(i=0; i<GMS_IP_ACCOUNT_HASH_SZ; i++){
			read_lock(&(g_gms_ip_account_hash[i].lock));

			if(!list_empty(&(g_gms_ip_account_hash[i].head))){
				list_for_each_entry(pos, &(g_gms_ip_account_hash[i].head), list){
					// call func update db 
					net_store_per_one(pos->accout,ntohl(pos->ip), pos->time);
					//printf("ip:%u, account:%s, time:%s \n", pos->ip, pos->accout, pos->time);
				}
			}
			
			read_unlock(&(g_gms_ip_account_hash[i].lock));
		}	
	}
		
	return NULL;
}

int gms_udp_recv_thread_start(void)
{
	pthread_t tid1, tid2, tid3;

	g_thread_run = 1;
	
	if(pthread_create(&tid1, NULL, recv_handle, NULL)){
		printf("create udp recv thread error.");
		return -1;
	}
    if (pthread_detach(tid1)){
	    printf("recv_handle pthread_detach  error.\n");
		return -1;
	}

	if(pthread_create(&tid2, NULL, parse_data_handle, NULL)){
		printf("create udp parse thread error.");
		return -1;
	}
    if (pthread_detach(tid2)){
	    printf("parse_data_handle pthread_detach  error.\n");
		return -1;
	}

	if(pthread_create(&tid3, NULL, update_db_handle, NULL)){
		printf("create update db thread error.");
		return -1;
	}
    if (pthread_detach(tid3)){
	    printf("update_db_handle pthread_detach  error.\n");
		return -1;
	}

	return 0;
}

void list_lock_init(void)
{
	int i;
	
	for(i=0; i<GMS_IP_ACCOUNT_HASH_SZ; i++){
		INIT_LIST_HEAD(&(g_gms_ip_account_hash[i].head));
		rwlock_init(&(g_gms_ip_account_hash[i].lock));
	}

	/** store udp packets**/
	INIT_LIST_HEAD(&g_data_list_head);
	pthread_mutex_init(&g_data_lock, NULL);

	atomic32_init(&g_need_update_db_flag);
	
	return;
}

int gms_udp_sock_init()
{
	if(gms_read_server_conf(GMS_UDP_SERVER_CONF_PATH)){
		printf("load server info error\n");
		return -1;
	}

	if(init_net_store())
		printf("init_net_store failed.\n");
	
	list_lock_init();

	gms_udp_recv_thread_start();

	return 0;
}

int gms_udp_sock_exit()
{
	gms_ip_account_info_t *pos, *n;
	int i;
	
	g_thread_run = 0;
	sleep(1);
	pthread_mutex_destroy(&g_data_lock);

	for(i=0; i<GMS_IP_ACCOUNT_HASH_SZ; i++){
		list_for_each_entry_safe(pos, n, &(g_gms_ip_account_hash[i].head), list){
			list_del(&(pos->list));
			free(pos);
			pos = NULL;
		}
	}

	return 0;
}

/* @ip:host ip
@return: account/null
**/
char * gms_get_account(unsigned int ip)
{
	unsigned int hash;
	char *ret = NULL;
	gms_ip_account_info_t *pos;

	hash = get_hash(ip);

	read_lock(&(g_gms_ip_account_hash[hash].lock));
	list_for_each_entry(pos, &(g_gms_ip_account_hash[hash].head), list){
		if(pos->ip == ip){
			ret = pos->accout;
			break;
		}
	}
	read_unlock(&(g_gms_ip_account_hash[hash].lock));
	return ret;
}


#if  0
int main(void)
{
	int i;
	char *account = NULL;
	gms_udp_sock_init();

	while(1){
		for(i=10; i<20; i++){
			
			account = gms_get_account(i);
			if(account)
				printf("@@@@@@@@@@@@@@@@@ %d %s\n", i, account);
			else
				printf("@@@@@@@@@@@@@@@@@ %d null \n", i);

			sleep(1);
		}
	}


	return 0;
}
#endif 

