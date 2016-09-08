/**
 * @file flow_store.c
 * @brief  流量存储流程 
 * @author zhang_dawei <zhangdawei@chanct.com>
 * @version 1.0
 * @date 2015-02-05
 */

#include "gms_flow.h"
int Init_Message()
{
	message=(Pmessage)malloc(sizeof(Message));
	message->head = (Pflows)malloc(sizeof(flows));
	message->head->next=(message->head);
	message->head->pre=(message->head);
	(message->tail)=(message->head);
	pthread_mutex_init(&message->mutex, NULL);

	pthread_cond_init(&message->cond_noempty,NULL);
	pthread_cond_init(&message->cond_nofull,NULL);
	pthread_cond_init(&message->cond_pause,NULL);
	/*      message->cond_noempty=PTHREAD_COND_INITIALIZER;
			message->cond_nofull=PTHREAD_COND_INITIALIZER;
			message->cond_pause=PTHREAD_COND_INITIALIZER;*/
	return 0;
}
void Destroy_Message()
{
	pthread_mutex_destroy(&message->mutex);
	pthread_cond_destroy(&message->cond_nofull);
	pthread_cond_destroy(&message->cond_noempty);
	pthread_cond_destroy(&message->cond_pause);
	if(message->head)
		free(message->head);
	if(message!=NULL)
	{
		free(message);
		message=NULL;
	}
}
int write_message(flows k)
{
	flows *ptmp=NULL;
	ptmp=(flows *)malloc(sizeof(flows));
	if(ptmp==NULL)
	{
		perror("malloc()");
		return -1;
	}
	*ptmp=k;
	ptmp->pre=NULL;
	ptmp->next=NULL;
	ptmp->pre=(message->head);
	(message->head->next->pre)=ptmp;
	ptmp->next=message->head->next;
	message->head->next=ptmp;

	return 0;
}
int read_message(flows *k)
{
	flows *pdel=NULL;
	flows *p=NULL;
	if((((message->head)->pre)==NULL)||(((message->head)->pre)==(message->head)))
	{
		      //printf("stack empty\n");
		return -1;
	}
	p=message->head;
	pdel=p->pre;
	p->pre=pdel->pre;
	pdel->pre->next=p;
	pdel->next=NULL;
	pdel->pre=NULL;
	*k=*pdel;
	if(pdel != NULL)
	{
		free(pdel);
	}
	return 0;

}

void insert_message(flows k)
{
	int ret;
	pthread_mutex_lock(&message->mutex);
	while(1)
	{
		ret=write_message(k);
		if(ret==0)
		{
			break;
		}
		else
			pthread_cond_wait(&message->cond_nofull,&message->mutex);
	}
	pthread_mutex_unlock(&message->mutex);
	pthread_cond_signal(&message->cond_noempty);

}
flows delete_message(void)
{
	int ret;
	flows rd;
	pthread_mutex_lock(&message->mutex);
	while(1)
	{
		ret=read_message(&rd);
		if(ret==0)
		{
			break;
		}
		else
			pthread_cond_wait(&message->cond_noempty,&message->mutex);
	}
	pthread_mutex_unlock(&message->mutex);
	pthread_cond_signal(&message->cond_nofull);
	return rd;

}

void* flow_sock_busi(void *arg)
{
	int n;
	struct sockaddr_in addr;
	int sockfd;
	flows buf;
//	flows test;
	int i;
	int ret;
	socklen_t addr_len = sizeof(struct sockaddr_in);

    pthread_t tid;
    tid = pthread_self();
	Init_Message();
    if (g_flow_debug_cfg.store_enable == 0) {
        fprintf(stderr, "STORE_ENABLE = NO!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "STORE_ENABLE = NO!");
        goto EXIT;
    }
/*	ret=read_comm_interface_conf(g_flow_info_cfg.comm_conf_path, g_flow_info_cfg.is_comm_monitor);
	if(ret<0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "Parser COMM XML FILE ERROR!!!!");
		goto EXIT;
	}
	if (strncmp(g_flow_info_cfg.is_comm_monitor, "1", 2) != 0) {
		printf("dev is not monitor Do not revice UDP packs!\n");
		goto EXIT;
	}*/
	//socket的udp链接，入队列
	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0)
	{
		perror("socket");
		exit(1);
	}
	bzero(&addr, sizeof(struct sockaddr_in));
#if 1
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_flow_info_cfg.flow_port);
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_addr.s_addr = inet_addr(g_flow_info_cfg.socket_ip);
#endif
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0)
	{
		perror("bind");
		goto ERR;
	}
	for(i=0; ; i++)
	{
		memset(&buf, 0, sizeof(buf));
		//init_test(&buf);
		n = recvfrom(sockfd, 
			    &buf, sizeof(buf), 
			    0, 
			    (struct sockaddr *)&addr, &addr_len);
		printf("connect ip=%s port=%d\n", 
				inet_ntoa(addr.sin_addr),
				ntohs(addr.sin_port));
	insert_message(buf);
	//write_message(buf);
	//read_message(&test);

	}
#if 0
	for(i=0;i<10;i++)
	{
	test=delete_message();
		printf("siptes= %lld , diptest = %lld ,query_idtest=%d\n",test.sip,test.dip,test.query_id);
	}
#endif
ERR:
	close(sockfd);

EXIT:
	Destroy_Message();
    pthread_exit((void*)tid);
}
