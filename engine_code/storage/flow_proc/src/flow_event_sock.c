/**
 * @file flow_store.c
 * @brief  流量存储流程 
 * @author zhang_dawei <zhangdawei@chanct.com>
 * @version 1.0
 * @date 2015-02-05
 */
#include "gms_flow.h"
int sockfd;
struct sockaddr_in addr;
static int __xxx_lthrd[16];
UTAF_DEFINE_PER_LCORE(int, lthrd_id);
UTAF_DEFINE_PER_LCORE(int, _lcore_id);


session_table * session_item_alloc(void)  //wdb_calc222
{

	session_table *si;

	if (utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first)
	{
		si = (session_table *)utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first;
		utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first = si->list.next;
	}
	else
	{
		uint32_t index = atomic32_add_return((atomic32_t *)&session_group.global_index, 1);
		index = index & (g_flow_info_cfg.thread_num - 1);

		spinlock_lock(&session_group.session_item_rq_lock[index]);
		if (session_group.session_item_ringq[index].first)
		{
			si = container_of(session_group.session_item_ringq[index].first, session_table, tm_lst);
			session_group.session_item_ringq[index].first = si->tm_lst.next;
		}
		else
		{
			si = NULL;
		}
		spinlock_unlock(&session_group.session_item_rq_lock[index]);
		if (si)
		{
			utaf_l_si_bunch[UTAF_PER_LCORE(lthrd_id)].first = si->list.next;
		}
	}

	return si;
}

int session_add(flow_event mbuf)
{
	session_table *si = session_item_alloc();
	if ( si == NULL ){
		return UTAF_MEM_FAIL;
	}
	si->session_node=mbuf;
	register int current_age_index;
	current_age_index = utaf_age_index_current;

	mnt_debug("lock ......................current_age_index :[%d]  lthrd_id:[ %d]\n",utaf_age_index_current,UTAF_PER_LCORE(lthrd_id));
	spinlock_lock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
	hlist_add_head(&si->tm_lst, &g_utaf_timeout_ring[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
	spinlock_unlock(&g_utaf_timeout_rlck[current_age_index][UTAF_PER_LCORE(lthrd_id)]);
	mnt_debug("unlock ......................\n");

	return UTAF_OK;

}
int session_input(flow_event mbuf)
{
	/* if (ip_to_conf_cmp(mbuf.sip) == 0 || ip_to_conf_cmp(mbuf.dip) == 0){
	 *	return -1;
	 * }
	 */
	struct in_addr ipval;
	ipval.s_addr = mbuf.sip;
	printf("ip_recv = %s \n", inet_ntoa(ipval));
	if (IP_FOUND == net_mnt_ip_lookup(mbuf.sip))
	{
		mbuf.direction_flag = flow_out;
	}
	else if (IP_FOUND == net_mnt_ip_lookup(mbuf.dip))
	{
		 mbuf.direction_flag = flow_in;
	}
	else
	{
		mnt_debug(" ip not found .*************************************************.\n");
		return -1;
	}
	mnt_debug(" ip storge .*************************************************.\n");
	session_add(mbuf);
}
static void *udp_srv_func(void *data)
{
	int stream_id = 0;
	//cpu_set_t mask;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	flow_event buf;
	stream_id = *((int *)data);

#if 0
	CPU_ZERO(&mask);
	CPU_SET(stream_id, &mask);

	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
		fprintf(stderr, "set thread affinity failed\n");
	}
#endif

	UTAF_PER_LCORE(_lcore_id) = stream_id;
	UTAF_PER_LCORE(lthrd_id) = stream_id;
	ssize_t n;

	while (1)
	{
#if 0
		if (g_flow_debug_cfg.flow_analyze == 0) 
		{
			sleep(60);
			continue;
		}
#endif
		mnt_debug("while  recvform udp_socket ... \n");
		memset(&buf, 0, sizeof(buf));
		n = recvfrom(sockfd, 
			    &buf, sizeof(buf), 
			    0, 
			    (struct sockaddr *)&addr, &addr_len);
    	session_input(buf);

	}


	return NULL;
}
int create_flow_srv_thr(void)
{
	pthread_t tid;
	int i;

	for (i = 0; i < g_flow_info_cfg.thread_num; i++)
	//for (i = 0; i < 1; i++)
	{
		__xxx_lthrd[i] = i;
		if ( pthread_create(&tid, NULL, udp_srv_func, &__xxx_lthrd[i]) != 0 )
		{
			fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
			return -1;
		}
		if ( pthread_detach(tid) != 0 )
		{
			fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid, strerror(errno));
			return -1;
		}
	}

	return 0;
}
int flow_event_sock_busi(void)
{
	int ret;
	socklen_t addr_len = sizeof(struct sockaddr_in);

    pthread_t tid;
    tid = pthread_self();
	//socket的udp链接，入队列
	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	bzero(&addr, sizeof(struct sockaddr_in));
#if 1
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_flow_info_cfg.event_port);
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("event_port=%d\n",g_flow_info_cfg.event_port);
	addr.sin_addr.s_addr = inet_addr(g_flow_info_cfg.socket_ip);
#endif
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0)
	{
		close(sockfd);
		perror("bind");
		return -1;
	}
#if 0
	printf("sockfd_read=sockfd= %d  ###################################\n",sockfd);
	ret=create_flow_srv_thr();
	if ( ret < 0 ){
		goto ERR;	
	}

ERR:
	close(sockfd);
#endif
	return 0;
}
