/*************************************************************************
  > File Name: log_out.c
  > Author: xuziquan
  > Mail: xuziquan@chanct.com 
  > Created Time: Thu 20 Aug 2015 10:56:56 AM ICT
 ************************************************************************/

#include "ddos_log_out.h"
#include "ddos_make_log.h"
//#include "ddos_log_out_rbtree.h"
#define HASHIP(ip)  ((ip)&255) 
#define HASHMAX 256
//#define LOG_OUT_INTERVAL 300
#define LOG_OUT_INTERVAL g_ddos_conf.top100_interval
#define LOG_THREAD_NUMBER g_ddos_conf.log_thread_num
#define UTAF_DEFINE_PER_LCORE(type, name)                       \
		  __thread __typeof__(type) utaf_per_lcore_##name
UTAF_DEFINE_PER_LCORE(int, ddos_thread_id);

  //DDOS_DEFAULT_T100_INTERVAL
//#define TOP_OUTPUT_INTERVAL 300 //output top sip interval 300 seconds
//#define LIST_MOVE_INTERVAL 5
//#define LOG_OUT_THREAD_NUM 5
//session_item_t out_list[256];
extern int g_ddos_time_jiffies;
extern int  __tm_out_flag_log;

extern PGconn * flow_connect_db(struct gpq_conn_info g_flow_info_cfg);
extern void gpq_disconnect(IN PGconn *conn);
extern int gen_random(void);
extern int insert_node_into_tree(node_destIP_outddos_data* node,session_item_t *count_data,total_data_list *statis_node);


struct ddos_log_empty_para *g_ddos_stat_log_thread_para;
volatile int ddos_log_index_current = 0;
log_list_head  *utaf_ddos_log_timeout_ring;

//node_destIP_outddos_data *destIP_outddos_list_head[256] = {0};
log_hash_list_head destIP_outddos_list_head[HASHMAX];

//log_list_head utaf_ddos_log_timeout_ring[LOG_OUT_INTERVAL];//[LOG_OUT_THREAD_NUM];
//spinlock_t        utaf_ddos_log_timeout_rlck[LOG_OUT_INTERVAL];//[LOG_OUT_THREAD_NUM];

void *ddos_log_wait_thread(void *data)
{
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
	register int index;
	int i;

	while (1)
	{
		m_wmb();

		if (ddos_fetch_and_dec(&__tm_out_flag_log) == 1)
		{
			//ddos_debug(DDOS_MID_LOG, "new detect cycle\n");
			index = ddos_log_index_current + 1;
			__builtin_ia32_lfence();
			index = index%LOG_OUT_INTERVAL;
			for(i = 0;i < LOG_THREAD_NUMBER;i++)
			{
				g_ddos_stat_log_thread_para[i].index = index;
			}
			while (1)
			{
				for (i=0; i<LOG_THREAD_NUMBER; i++)
				{
					if (g_ddos_stat_log_thread_para[i].index != -1)
						goto stag; /* sleep tag */
				}
				break;
stag:
				nanosleep(&sleeptime, NULL);
			}

			m_wmb();
			ddos_atomic_set(index, &ddos_log_index_current);
			m_wmb();
		}
		else
		{
			nanosleep(&sleeptime, NULL);
		}
	}

	return NULL;
}

rbtree_node_destIP_outddos_data *utaf_ddos_log_outlist_find(session_item_t* count_data, rbtree_node_destIP_outddos_data *p)
{
	rbtree_node_destIP_outddos_data *dest_node_data;
	uint32_t dip = ddos_get_mnt_ip(count_data);

	ddos_debug(DDOS_MID_LOG, "start find dip_node:%s!\n",log_ip_ntoa(dip));
	for(dest_node_data = p;NULL != dest_node_data;)
	{
		if(dest_node_data->dip == dip){
			return dest_node_data;
		}
		dest_node_data = dest_node_data->next;
	}
	return NULL;
}

rbtree_node_destIP_outddos_data *utaf_ddos_log_ip_outlist_find(uint32_t dip, rbtree_node_destIP_outddos_data *p)
{
	rbtree_node_destIP_outddos_data *dest_node_data;
	//uint32_t dip = ddos_get_mnt_ip(count_data);

	ddos_debug(DDOS_MID_LOG, "start find dip_node:%s!\n",log_ip_ntoa(dip));
	for(dest_node_data = p;NULL != dest_node_data;)
	{
		if(dest_node_data->dip == dip){
			return dest_node_data;
		}
		dest_node_data = dest_node_data->next;
	}
	return NULL;
}

log_tm_list_node *utaf_ddos_log_tm_ring_find(log_list_head tm_head, rbtree_node_destIP_outddos_data *node)
{
	log_tm_list_node *p;

	ddos_debug(DDOS_MID_LOG, "start find dip_node in tm_list:%s!\n",log_ip_ntoa(node->dip));
	for(p = tm_head.head;NULL != p;)
	{
		if(p->dip_node->dip == node->dip){
			return p;
		}
		p = p->next;
	}
	return NULL;
}

int  remove_tm_node(log_tm_list_node *tm_node,rbtree_node_destIP_outddos_data *node)
{
	if(utaf_ddos_log_timeout_ring[node->tm_index].head == tm_node){
		utaf_ddos_log_timeout_ring[node->tm_index].head = tm_node->next;
		free(tm_node);
		return SUCCE;
	}
	else if(NULL == tm_node->next){
		tm_node->prev->next = NULL;
		free(tm_node);
		return SUCCE;
	}
	else{
		tm_node->next->prev = tm_node->prev;
		tm_node->prev->next = tm_node->next;
		free(tm_node);
	}
	return SUCCE;
}

uint32_t get_attack_id(uint32_t attack_type)
{
	uint32_t attack_id;
	switch(attack_type){
		case 0:
			//CA_LOG(LOG_MODULE,LOG_PROC,"unknown attack_type:%d!\n",statis_node->attack_type);
			break;
		case DDOS_ATTACK_TYPE_SHIFT_TCP:
			attack_id = TCP_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_TCP_SYN:
			attack_id = TCP_SYN_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_TCP_ACK:
			attack_id = TCP_ACK_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_CHARGEN:
			attack_id = CHARGEN_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_ICMP:
			attack_id = ICMP_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_DNS:
			attack_id = DNS_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_NTP:
			attack_id = NTP_FLOOD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_SSDP:
			attack_id = SSDP_FLODD;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_SNMP:
			attack_id = TCP_SNMP;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_NEW:
			attack_id = TCP_LINK;
			break;
		case DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_LIVE:
			attack_id = TCP_LINK;
			break;
		default:
			//CA_LOG(LOG_MODULE,LOG_PROC,"unknown attack_type:%d!\n",statis_node->attack_type);
			attack_id = BLENDED_ATT;
			break;
	}
	return attack_id;
}

int get_dip_sum_detail(node_destIP_outddos_data *node,session_item_t *count_data,total_data_list *statis_node)
{
	time_t now_time = 0;
	
	uint32_t tmp_attack_id = get_attack_id(statis_node->attack_type);
	if(node->attack_id !=0 && tmp_attack_id != node->attack_id){
		node->attack_id = BLENDED_ATT;	
	}

	time(&now_time);
	node->end_time = now_time; 

	switch(count_data->protocol){
		case PROTO_TCP:
			node->flow_pps += (count_data->ddos.stat.tcp.tcp[dir_in].pkts + count_data->ddos.stat.tcp.tcp[dir_out].pkts);
			node->flow_bps += (count_data->ddos.stat.tcp.tcp[dir_in].bytes + count_data->ddos.stat.tcp.tcp[dir_out].bytes);
			break;
		case PROTO_UDP:
			node->flow_pps += (count_data->ddos.stat.udp.udp[dir_in].pkts + count_data->ddos.stat.udp.udp[dir_out].pkts);
			node->flow_bps += (count_data->ddos.stat.udp.udp[dir_in].bytes + count_data->ddos.stat.udp.udp[dir_out].bytes);
			break;
		case PROTO_ICMP:
			node->flow_pps += (count_data->ddos.stat.icmp.icmp[dir_in].pkts + count_data->ddos.stat.icmp.icmp[dir_out].pkts);
			node->flow_bps += (count_data->ddos.stat.icmp.icmp[dir_in].bytes + count_data->ddos.stat.icmp.icmp[dir_out].bytes);
			break;
		default:
			printf("log in func get_dip_sum_detail unknown protocol!\n");
			break;
	}
	return SUCCE;
}
/*
int get_sip_sum_detail(node_sip_outddos_data *node,session_item_t *count_data,total_data_list *statis_node)
{
	time_t now_time = 0;
	node_sip_outddos_data *p = node;
	uint32_t tmp_attack_id;
	
	tmp_attack_id = get_attack_id(statis_node->attack_type);
	if(node->attack_id != 0 && node->attack_id != tmp_attack_id){
		node->attack_id = BLENDED_ATT;	
	}
	time(&now_time);
	node->end_time = now_time;

	switch(count_data->protocol){
	case PROTO_TCP:
		ddos_debug(DDOS_MID_LOG, "TCP[in]:%ld %ld TCP[out]:%ld %ld SYN:%ld %ld ACK:%ld %ld\n", 
					count_data->ddos.stat.tcp.tcp[dir_in].pkts, count_data->ddos.stat.tcp.tcp[dir_in].bytes,
					count_data->ddos.stat.tcp.tcp[dir_out].pkts, count_data->ddos.stat.tcp.tcp[dir_out].bytes,
					count_data->ddos.stat.tcp.syn.pkts, count_data->ddos.stat.tcp.syn.bytes,
					count_data->ddos.stat.tcp.ack.pkts, count_data->ddos.stat.tcp.ack.bytes);
		p->sip_attack_detail.tcp_in_packets += count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.tcp_out_packets += count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.tcp_in_bytes += count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.tcp_out_bytes += count_data->ddos.stat.tcp.tcp[dir_out].bytes;

		p->sip_attack_detail.tcp_syn_in_packets += count_data->ddos.stat.tcp.syn.pkts;
		p->sip_attack_detail.tcp_syn_in_bytes += count_data->ddos.stat.tcp.syn.bytes;
		p->sip_attack_detail.tcp_ack_in_packets += count_data->ddos.stat.tcp.ack.pkts;
		p->sip_attack_detail.tcp_ack_in_bytes += count_data->ddos.stat.tcp.ack.bytes;

		p->sip_attack_detail.flow_in_packets += count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets += count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes += count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes += count_data->ddos.stat.tcp.tcp[dir_out].bytes;

		p->sip_attack_detail.tcp_new_num_ps += statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_new_old[dir_in];
		p->sip_attack_detail.tcp_live_num_ps += statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_close[dir_in];
		if(p->sip_attack_detail.tcp_new_num_ps < 0){
			p->sip_attack_detail.tcp_new_num_ps = 0;
		}
		if(p->sip_attack_detail.tcp_live_num_ps < 0){
			p->sip_attack_detail.tcp_live_num_ps = 0;
		}

		switch (count_data->service_type)
		{
				case APP_CHARGEN:
					p->sip_attack_detail.chargen_in_packets += count_data->ddos.stat.tcp.tcp[dir_in].pkts;
					p->sip_attack_detail.chargen_out_packets += count_data->ddos.stat.tcp.tcp[dir_out].pkts;
					p->sip_attack_detail.chargen_in_bytes += count_data->ddos.stat.tcp.tcp[dir_in].bytes;
					p->sip_attack_detail.chargen_out_bytes += count_data->ddos.stat.tcp.tcp[dir_out].bytes;
					break;
				default:
					break;
		}
		break;
	case PROTO_UDP:
		ddos_debug(DDOS_MID_LOG, "UDP[in]:%ld %ld UDP[out]:%ld %ld\n", 
					count_data->ddos.stat.udp.udp[dir_in].pkts, count_data->ddos.stat.udp.udp[dir_in].bytes,
					count_data->ddos.stat.udp.udp[dir_out].pkts, count_data->ddos.stat.udp.udp[dir_out].bytes);
		switch(count_data->service_type)
		{
				case APP_DNS:
					p->sip_attack_detail.dns_in_packets += count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.dns_out_packets += count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.dns_in_bytes += count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.dns_out_bytes += count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_NTP:
					p->sip_attack_detail.ntp_in_packets += count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.ntp_out_packets += count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.ntp_in_bytes += count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.ntp_out_bytes += count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_SSDP:
					p->sip_attack_detail.ssdp_in_packets += count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.ssdp_out_packets += count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.ssdp_in_bytes += count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.ssdp_out_bytes += count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_SNMP:
					p->sip_attack_detail.snmp_in_packets += count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.snmp_out_packets += count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.snmp_in_bytes += count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.snmp_out_bytes += count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_CHARGEN:
					p->sip_attack_detail.chargen_in_packets += count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.chargen_out_packets += count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.chargen_in_bytes += count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.chargen_out_bytes += count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				default:
					printf("unknown udp app protocol\n");
					break;
		}

		p->sip_attack_detail.flow_in_packets += count_data->ddos.stat.udp.udp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets += count_data->ddos.stat.udp.udp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes += count_data->ddos.stat.udp.udp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes += count_data->ddos.stat.udp.udp[dir_out].bytes;
		break;
	case PROTO_ICMP:
		ddos_debug(DDOS_MID_LOG, "ICMP[in]:%ld %ld ICMP[out]:%ld %ld\n", 
					count_data->ddos.stat.icmp.icmp[dir_in].pkts, count_data->ddos.stat.icmp.icmp[dir_in].bytes,
					count_data->ddos.stat.icmp.icmp[dir_out].pkts, count_data->ddos.stat.icmp.icmp[dir_out].bytes);
		p->sip_attack_detail.icmp_in_packets += count_data->ddos.stat.icmp.icmp[dir_in].pkts;
		p->sip_attack_detail.icmp_out_packets += count_data->ddos.stat.icmp.icmp[dir_out].pkts;
		p->sip_attack_detail.icmp_in_bytes += count_data->ddos.stat.icmp.icmp[dir_in].bytes;
		p->sip_attack_detail.icmp_out_bytes += count_data->ddos.stat.icmp.icmp[dir_out].bytes;

		p->sip_attack_detail.flow_in_packets += count_data->ddos.stat.icmp.icmp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets += count_data->ddos.stat.icmp.icmp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes += count_data->ddos.stat.icmp.icmp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes += count_data->ddos.stat.icmp.icmp[dir_out].bytes;
		break;
	default:
		printf("log unknown protocol!\n");
		break;
	}//end of switch(count_data->protocol){


	return SUCCE;
}
*/
/*create dip node*/
/*
node_destIP_outddos_data *create_dip_node(session_item_t *count_data,total_data_list *statis_node)
{
	uint32_t res;
	time_t now_time=0;
	node_destIP_outddos_data *dip_node = malloc(sizeof(node_destIP_outddos_data));
	if(NULL == dip_node){
		printf("malloc error in func create_dip_node!\n");
		return NULL;
	}
	memset(dip_node,0,sizeof(node_destIP_outddos_data));
	res=pthread_rwlock_init(&(dip_node->rwlock),NULL);//init rwlock
    if (res != 0)
    {
        printf("rwlock initialization failed in func create_dip_node\n");
        return NULL;
    }
	time(&now_time);

	dip_node->start_time = now_time;
	dip_node->end_time = now_time;

	dip_node->dip = ddos_get_mnt_ip(count_data);
	//gen a unique num
	dip_node->query_id = gen_random() + dip_node->dip;l		

	switch(count_data->protocol){
	case PROTO_TCP:
		dip_node->flow_bps = count_data->ddos.stat.tcp.tcp[dir_in].pkts + count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		dip_node->flow_pps = count_data->ddos.stat.tcp.tcp[dir_in].bytes + count_data->ddos.stat.tcp.tcp[dir_out].bytes;
		break;
	case PROTO_UDP:
		dip_node->flow_bps = count_data->ddos.stat.udp.udp[dir_in].pkts + count_data->ddos.stat.udp.udp[dir_out].pkts;
		dip_node->flow_pps = count_data->ddos.stat.udp.udp[dir_in].bytes + count_data->ddos.stat.udp.udp[dir_out].bytes;
		break;
	case PROTO_ICMP:
		dip_node->flow_bps = count_data->ddos.stat.icmp.icmp[dir_in].pkts + count_data->ddos.stat.icmp.icmp[dir_out].pkts;
		dip_node->flow_pps = count_data->ddos.stat.icmp.icmp[dir_in].bytes + count_data->ddos.stat.icmp.icmp[dir_out].bytes;
		break;
	default:
		printf("log in create_dip_node protocol unknown!\n");
		break;
	}
	
	dip_node->attack_id = get_attack_id(statis_node->attack_type);
	dip_node->sourceip_tree_root = (node_sip_outddos_data*)create_tree_node(count_data,statis_node);

	return dip_node;
}
*/
/*insert dip node into hash list*/
rbtree_node_destIP_outddos_data *utaf_ddos_log_insert_node(session_item_t* count_data,total_data_list *statis_node,struct rbtree_node_destIP_outddos_data **head)
{
	rbtree_node_destIP_outddos_data *node = create_rbtree_dip_node(count_data,statis_node);
	if(NULL == node){
		printf("create dip node error!\n");
		return NULL;
	}
	if(NULL == *head){
		*head = node;
		return node;
	}
	else{
		node->next = *head;
		(*head)->prev = node;
		*head = node;
	}
	
	return node;
}

int add_ptr_to_time_list(rbtree_node_destIP_outddos_data *node)
{
	//int i;
	unsigned int random_tid = gen_random()%LOG_THREAD_NUMBER;
	int ring_index = ddos_log_index_current + random_tid*LOG_OUT_INTERVAL;
//	printf("\n---------random_tid:%u,ring_index:%d---------------------\n",random_tid,ring_index);
//	log_list_head utaf_ddos_log_timeout_ring[__tm_out_flag_log];
	log_tm_list_node *tm_node = (log_tm_list_node *)malloc(sizeof(log_tm_list_node));
	if(NULL == tm_node){
		printf("malloc error in func add_ptr_to_time_list!\n");
		return FAIL;
	}
	memset(tm_node,0,sizeof(log_tm_list_node));
	tm_node->dip_node = node;
	/*lock   lock*/
	spinlock_lock(&utaf_ddos_log_timeout_ring[ring_index].lock);
	node->tm_index = ring_index;

	if(NULL == utaf_ddos_log_timeout_ring[ring_index].head){
//		tm_node->prev = &utaf_ddos_log_timeout_ring[ring_index];
		utaf_ddos_log_timeout_ring[ring_index].head = tm_node;
	}
	else{
		utaf_ddos_log_timeout_ring[ring_index].head->prev = tm_node;
		tm_node->next = utaf_ddos_log_timeout_ring[ring_index].head;
		utaf_ddos_log_timeout_ring[ring_index].head = tm_node;

	}
	ddos_debug(DDOS_MID_LOG, "add to time list utaf_ddos_log_timeout_ring[%d]:head:%d\n",ring_index,utaf_ddos_log_timeout_ring[ring_index].head->dip_node->dip);
	spinlock_unlock(&utaf_ddos_log_timeout_ring[ring_index].lock);
	return SUCCE;
	
}

/*update attack top buf*/
int update_curr_top(rbtree_node_destIP_outddos_data *node)//compare current top with attack top
{
	int i;
	int end = node->curr_top_num - 1;
	if(end > TOPNUM)
	{
		end = TOPNUM;
	}
	for(i = 0;i <= end && (NULL != node->curr_top_ptr[i]);i++)
	{
/*		node->curr_top[i]
		node->curr_top_ptr[i]*/

		node->curr_top[i].start_time = node->curr_top_ptr[i]->start_time;
		node->curr_top[i].end_time = node->curr_top_ptr[i]->end_time;

		node->curr_top[i].sip = node->curr_top_ptr[i]->sip;
		node->curr_top[i].s_country_id = node->curr_top_ptr[i]->s_country_id;
		node->curr_top[i].s_city_id = node->curr_top_ptr[i]->s_city_id;
	//	node->curr_top[i].s_latitude;
	//	node->curr_top[i].s_longitude;
		node->curr_top[i].dip = node->curr_top_ptr[i]->dip;
		node->curr_top[i].d_country_id = node->curr_top_ptr[i]->d_country_id;
		node->curr_top[i].d_city_id = node->curr_top_ptr[i]->d_city_id;
	//node->curr_top[i].d_latitude;
	//node->curr_top[i].d_longitude;

		node->curr_top[i].attack_id = node->curr_top_ptr[i]->attack_id;

		node->curr_top[i].curr_top_detail = node->curr_top_ptr[i]->sip_attack_detail;

		node->curr_top[i].flow_bps = (node->curr_top_ptr[i]->sip_attack_detail.flow_in_bytes)
									 + (node->curr_top_ptr[i]->sip_attack_detail.flow_out_bytes);
		node->curr_top[i].flow_pps = (node->curr_top_ptr[i]->sip_attack_detail.flow_in_packets)
									 + (node->curr_top_ptr[i]->sip_attack_detail.flow_out_packets);
		ddos_debug(DDOS_MID_LOG,"curr_top :start_time = %lu,end_time = %lu,sip = %x,dip = %x,attack_id = %d flow_bps = %lu,flow_pps = %lu\n",
			node->curr_top[i].start_time,node->curr_top[i].end_time,node->curr_top[i].sip,node->curr_top[i].dip,
			node->curr_top[i].attack_id,node->curr_top[i].flow_bps,node->curr_top[i].flow_pps);
	}
	
	memset(node->curr_top_ptr,0,sizeof(rbtree_node_sip_outddos_data *)*TOPNUM);
	return SUCCE;
}

int destroyTree(node_sip_outddos_data **node)
{
	if(NULL == *node)
		return SUCCE;
	if(NULL != (*node)->left_child)
		destroyTree(&(*node)->left_child);
	if(NULL != (*node)->right_child)
		destroyTree(&(*node)->right_child);
	free(*node);
	*node = NULL;
	return SUCCE;
//	node == NULL;
}

int destroyRbtree(struct rb_node **tree_node,struct rbtree_node_sip_outddos_data *sip_node_tmp)
{
	if(NULL == *tree_node){
		ddos_debug(DDOS_MID_LOG,"rbtree is empty,noneed destory!\n");
		return SUCCE;
	}
	//struct rb_node *tree_node = root.rb_node;
	//struct rbtree_node_sip_outddos_data *sip_node_this;	
	//struct rbtree_node_sip_outddos_data *sip_node_tmp;
	//sip_node_this = container_of(tree_node, struct rbtree_node_sip_outddos_data, rbtree_node);
	
	if(NULL != (*tree_node)->rb_left){
		sip_node_tmp = container_of((*tree_node)->rb_left, struct rbtree_node_sip_outddos_data, rbtree_node);
		destroyRbtree(&((*tree_node)->rb_left),sip_node_tmp);
	}
	if(NULL != (*tree_node)->rb_right){
		sip_node_tmp = container_of((*tree_node)->rb_right, struct rbtree_node_sip_outddos_data, rbtree_node);
		destroyRbtree(&((*tree_node)->rb_right),sip_node_tmp);
	}
	sip_node_tmp = container_of(*tree_node, struct rbtree_node_sip_outddos_data, rbtree_node);
	//printf("----------traverse distory tree  sip:%s-------\n",log_ip_ntoa(sip_node_tmp->sip));
	free(sip_node_tmp);
	//memset(sip_node_tmp,0,sizeof(rbtree_node_sip_outddos_data));
	sip_node_tmp = NULL;
	*tree_node = NULL;
	
	return SUCCE;
//	struct rb_root root;
//	root.rb_node = &(*node)->rbtree_node;

//	struct rb_node *tmp_node;
//	struct rbtree_node_sip_outddos_data *sip_node_this;
//	for (node = rb_first(&dip_node->rbtree); node; node = rb_next(node))
/*
	for (tmp_node = rb_first(&root); tmp_node;)
	{
		sip_node_this = container_of(tmp_node, struct rbtree_node_sip_outddos_data, rbtree_node);
		printf("----------traverse distory tree  sip:%s-------\n",log_ip_ntoa(sip_node_this->sip));
		tmp_node = rb_next(tmp_node);
		if(NULL == sip_node_this){
			printf("no need free\n");
			continue;
		}
		memset(sip_node_this,0,sizeof(rbtree_node_sip_outddos_data));
		free(sip_node_this);
		sip_node_this = NULL;
		//printf("%d ", rb_entry(node, struct mytype, my_node)->num);
	} */

//	ddos_debug(DDOS_MID_LOG,"distory_rbtree over!\n");
//	return SUCCE;
	
//	node == NULL;
}


int destroyTmlist(log_tm_list_node *tm_node_head)
{
	log_tm_list_node *node = NULL;
	log_tm_list_node *tmp = NULL;
	for(node = tm_node_head;node != NULL;){
		tmp = node->next;
		free(node);
		node = tmp;
	}
	return SUCCE;
}

int delete_dip_node(rbtree_node_destIP_outddos_data *node,unsigned int hash)
{
	/*lock lock*/
	if(NULL == node){
		CA_LOG(LOG_MODULE,LOG_PROC,"find no dip in destIP_outddos_list_head,cause node = null\n");
		return FAIL;
	}
	if(node == destIP_outddos_list_head[hash].head){
		printf("---------dip_node is head!\n---------");
		destIP_outddos_list_head[hash].head->prev = NULL;
		destIP_outddos_list_head[hash].head = destIP_outddos_list_head[hash].head->next;
		free(node);
	}
	else if(NULL == node->next){
		node->prev->next = NULL;
		free(node);
	}
	else{
		node->prev->next = node->next;
		node->next->prev = node->prev;
		free(node);
	}
	return SUCCE;
}

int utaf_ddos_log_insert(session_item_t *count_data,total_data_list *statis_node)//zhouyingying call
{
	unsigned int ddos_out_hash;
	uint32_t dip;
//	uint32_t sip;
	if(NULL == count_data || NULL == statis_node){
		CA_LOG(LOG_MODULE,LOG_PROC,"count_data || statis_node is null,return");
		return FAIL;
	}
	dip = ddos_get_mnt_ip(count_data);
//	sip = ddos_get_oppo_mnt_ip(count_data);
	/*hash  get dest ip location*/
/*	if(count_data->direct == 0)
		ddos_out_hash = HASHIP(count_data->dip);
	else  
		ddos_out_hash = HASHIP(count_data->sip);*/
	ddos_out_hash = HASHIP(dip);
	
	spinlock_lock(&destIP_outddos_list_head[ddos_out_hash].lock);
	rbtree_node_destIP_outddos_data* node = utaf_ddos_log_outlist_find(count_data,destIP_outddos_list_head[ddos_out_hash].head);
	if(node == NULL){
		ddos_debug(DDOS_MID_LOG, "cannot find dip:%s,insert node!\n",log_ip_ntoa(dip));
		node = utaf_ddos_log_insert_node(count_data,statis_node,&destIP_outddos_list_head[ddos_out_hash].head);
		ddos_debug(DDOS_MID_LOG, "destIP_outddos_list_head[%d].head:%d,insert node!\n",ddos_out_hash,destIP_outddos_list_head[ddos_out_hash].head->dip);
		spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);
		/*add index,output if timeout*/
		add_ptr_to_time_list(node);
	}
	else{
		if(SUCCE == rbtree_get_dip_sum_detail(node,count_data,statis_node)) //calculate total
		{
			spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);
			pthread_rwlock_wrlock(&(node->rwlock));
			node->sip_node_count = 0;//be used for counting sip node number on tree
			if(SUCCE == rbtree_insert_node_into_tree(node,count_data,statis_node)) //insert into sip tree
			{
				printf("insert success!\n");
			}
			pthread_rwlock_unlock(&(node->rwlock));
		}
		else{
			spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);	
		}
		//add_ptr_to_time_list(node);
	}
	return SUCCE;

} //end of utaf_ddos_log_out_ddos(total_data* count_data) 

int ddos_log_remove_monitorip(uint32_t dip)
{
	unsigned int ddos_out_hash;
	log_tm_list_node * tm_node = NULL;

//	uint32_t dip = ddos_get_mnt_ip(count_data);
	/*hash  get dest ip location*/
	ddos_out_hash = HASHIP(dip);
	spinlock_lock(&destIP_outddos_list_head[ddos_out_hash].lock);
	rbtree_node_destIP_outddos_data* node = utaf_ddos_log_ip_outlist_find(dip,destIP_outddos_list_head[ddos_out_hash].head);
	if(node == NULL){
		spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);
		CA_LOG(LOG_MODULE,LOG_PROC,"find no dip_node:%s int func ddos_log_remove_monitorip!\n",log_ip_ntoa(dip));
	}
	else{
		/*remove node from the timeout_ring*/
		//lock
		spinlock_lock(&utaf_ddos_log_timeout_ring[node->tm_index].lock);
		//find node
		tm_node = utaf_ddos_log_tm_ring_find(utaf_ddos_log_timeout_ring[node->tm_index],node);
		if(NULL == tm_node){
			CA_LOG(LOG_MODULE,LOG_PROC,"can not find node:%s in utaf_ddos_log_timeout_ring[%d]\n",log_ip_ntoa(node->dip),node->tm_index);
		}
		//remove tm_node from utaf_ddos_log_timeout_ring
		else{
			CA_LOG(LOG_MODULE,LOG_PROC,"find node:%s in utaf_ddos_log_timeout_ring[%d] and remove it",log_ip_ntoa(node->dip),node->tm_index);
			remove_tm_node(tm_node,node);
		}
		//unlock
		spinlock_unlock(&utaf_ddos_log_timeout_ring[node->tm_index].lock);

		/*remove node from the hash_list*/
		pthread_rwlock_wrlock(&(node->rwlock));
		//destroyRbtree(&node->sip_tree_root);
		printf("attack over destroyTree dip:%s\n",log_ip_ntoa(node->dip));
		pthread_rwlock_unlock(&(node->rwlock));
		pthread_rwlock_destroy(&(node->rwlock));
		delete_dip_node(node,ddos_out_hash);

		spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);
	}
	return SUCCE;
}

//print  debug info
void out_put_top(rbtree_node_destIP_outddos_data *node)
{
	int i;
	ddos_debug(DDOS_MID_LOG,"-----------------current top num = %d--------------att_top_num = %d-------------",
				node->curr_top_num,node->att_top_num);
	printf("\n******************output top*********************\n");
	for(i = 0;i < TOPNUM;i++){
		printf("curr top:sip = %u ,dip = %u ,flow_bps = %lu,flow_pps = %lu,province = %d,city = %d" 
					"attack top :sip = %u ,dip = %u ,flow_bps = %lu,flow_pps = %lu,province = %d,city = %d\n",
					node->curr_top[i].sip,node->curr_top[i].dip,node->curr_top[i].flow_bps,node->curr_top[i].flow_pps,
					node->curr_top[i].s_country_id,node->curr_top[i].s_city_id,
					node->att_top[i].sip,node->att_top[i].dip,node->att_top[i].flow_bps,node->att_top[i].flow_pps,
					node->att_top[i].d_country_id,node->att_top[i].d_city_id);
	}
}

int output_timeout(log_tm_list_node *tm_node_head)
{
	log_tm_list_node *node = NULL;
	/*traverse all node,calculate every top100*/
	for(node = tm_node_head;node != NULL; node = node->next)
	{
		//ddos_debug(DDOS_MID_LOG, "output_timeout dip_node:%d",node->dip_node->dip);
		CA_LOG(LOG_MODULE, LOG_PROC, "output_timeout dip_node:%s\n",log_ip_ntoa(node->dip_node->dip));
		pthread_rwlock_rdlock(&(node->dip_node->rwlock));
		//ddos_traverse_tree(node->dip_node);
		traverse_rbtree(node->dip_node);
		pthread_rwlock_unlock(&(node->dip_node->rwlock));
		ddos_debug(DDOS_MID_LOG,"curr_top_num = %d\n",node->dip_node->curr_top_num);
		update_curr_top(node->dip_node);
		if(-1 == ddos_log_top100_5min_store(node->dip_node)){
			ddos_debug(DDOS_MID_LOG,"ddos_log_top100_5min_store fail");
			return FAIL;
		}
		update_attack_top(node->dip_node);
		//utaf_ddos_log_socket_send(node);
#ifdef DEBUG_OUT_LOG
		out_put_top(node->dip_node);
#endif
		pthread_rwlock_wrlock(&(node->dip_node->rwlock));
		//destroyTree(&(node->dip_node->sourceip_tree_root));		
		destroyRbtree(&node->dip_node->rbtree.rb_node,NULL);
		memset(&node->dip_node->curr_top_ptr,0,sizeof(node_sip_outddos_data *)*TOPNUM);
		memset(&node->dip_node->curr_top,0,sizeof(node_curr_top100)*TOPNUM);
		
		pthread_rwlock_unlock(&(node->dip_node->rwlock));
		
		printf("\ndestroyTree:dip_node:%s\n",log_ip_ntoa(node->dip_node->dip));
	}
	//destroyTmlist(tm_node_head);
	return SUCCE;
}
/*attack complete*/
int output_full_attack(uint32_t dip)
{
	unsigned int ddos_out_hash;
	log_tm_list_node * tm_node = NULL;

	PGconn *ddos_conn;
	ddos_conn = flow_connect_db(g_ddos_conf.gpq_info);//connect db
	if(NULL == ddos_conn){
		ddos_debug(DDOS_MID_LOG,"connet db fail!\n");
		return FAIL;
	}

//	uint32_t dip = ddos_get_mnt_ip(count_data);
	/*hash  get dest ip location*/
	ddos_out_hash = HASHIP(dip);
	spinlock_lock(&destIP_outddos_list_head[ddos_out_hash].lock);
	rbtree_node_destIP_outddos_data* node = utaf_ddos_log_ip_outlist_find(dip,destIP_outddos_list_head[ddos_out_hash].head);
	if(node == NULL){
		spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);
		printf("find no dip_node:%s int func output_full_attack!\n",log_ip_ntoa(dip));
		goto EXIT;
	}
	else{
		/*remove node from the timeout_ring*/
		//lock
		spinlock_lock(&utaf_ddos_log_timeout_ring[node->tm_index].lock);
		//find node
		tm_node = utaf_ddos_log_tm_ring_find(utaf_ddos_log_timeout_ring[node->tm_index],node);
		if(NULL == tm_node){
			ddos_debug(DDOS_MID_LOG,"can not find node:%s in utaf_ddos_log_timeout_ring[%d]\n",log_ip_ntoa(node->dip),node->tm_index);
		}
		//remove tm_node from utaf_ddos_log_timeout_ring
		else{
			ddos_debug(DDOS_MID_LOG,"find node:%s in utaf_ddos_log_timeout_ring[%d] and remove it",log_ip_ntoa(node->dip),node->tm_index);
			remove_tm_node(tm_node,node);
		}
		//unlock
		spinlock_unlock(&utaf_ddos_log_timeout_ring[node->tm_index].lock);

		/*update current top*/
		pthread_rwlock_rdlock(&(node->rwlock));
		//ddos_traverse_tree(node);
		if(FAIL == traverse_rbtree(node)){
			CA_LOG(LOG_MODULE,LOG_PROC,"traverse_rbtree error!\n");
		}
		pthread_rwlock_unlock(&(node->rwlock));

		update_curr_top(node);
		//update attack top
		update_attack_top(node);

		ddos_log_ddosip_store(ddos_conn,node);
		/*remove node from the hash_list*/
		pthread_rwlock_wrlock(&(node->rwlock));
		destroyRbtree(&node->rbtree.rb_node,NULL);
		printf("attack over destroyRbtree dip:%s\n",log_ip_ntoa(node->dip));
		pthread_rwlock_unlock(&(node->rwlock));
		pthread_rwlock_destroy(&(node->rwlock));
		delete_dip_node(node,ddos_out_hash);

		spinlock_unlock(&destIP_outddos_list_head[ddos_out_hash].lock);
	}
EXIT:
	gpq_disconnect(ddos_conn);
	return SUCCE;
}

int ddos_log(int32_t index)
{
	log_tm_list_node *head = NULL;
	spinlock_lock(&utaf_ddos_log_timeout_ring[index].lock);
	head = utaf_ddos_log_timeout_ring[index].head;
	if(NULL == head){
	//	printf("utaf_ddos_log_timeout_ring[%d] empty!\n",index);
	//	return SUCCE;
	}
	else{
		CA_LOG(LOG_MODULE, LOG_PROC,"timeout_ring[%d].head not null,start output_timeout",index);
		if(FAIL == output_timeout(head)){	
		CA_LOG(LOG_MODULE, LOG_PROC,"output_timeout error!\n");
			//return FAIL;
		}
	}
	spinlock_unlock(&utaf_ddos_log_timeout_ring[index].lock);
	return SUCCE;
}

int32_t ddos_log_hash_init(void)
{
	uint32_t index;
	log_list_head *dlist;
	struct ddos_log_empty_para *para;

//log_hash_list_head destIP_outddos_list_head[HASHMAX];
	for(index = 0; index < HASHMAX; index++)
	{
		INIT_LOG_HLIST_HEAD(destIP_outddos_list_head[index].head);
		spinlock_init(&(destIP_outddos_list_head[index].lock));
	}

	dlist =(log_list_head *)malloc(sizeof(log_list_head)*LOG_OUT_INTERVAL*LOG_THREAD_NUMBER);
	if(NULL == dlist){
		printf("malloc error! in ddos_log_hash_init\n");
		return FAIL;
	}
	memset(dlist,0,sizeof(log_list_head)*LOG_OUT_INTERVAL*LOG_THREAD_NUMBER);
	utaf_ddos_log_timeout_ring = dlist;
//	struct log_list_head utaf_ddos_log_timeout_ring[LOG_OUT_INTERVAL]
	for(index = 0; index < LOG_OUT_INTERVAL*LOG_THREAD_NUMBER; index++)
	{
		INIT_LOG_HLIST_HEAD(utaf_ddos_log_timeout_ring[index].head);
		spinlock_init(&(utaf_ddos_log_timeout_ring[index].lock));
	}
	para = malloc(sizeof(ddos_log_empty_para)*LOG_THREAD_NUMBER);
	if(!para)
	{
		free(utaf_ddos_log_timeout_ring);
		utaf_ddos_log_timeout_ring = NULL;
		return FAIL;
	}
	memset(para, 0x0, sizeof(ddos_log_empty_para)*LOG_THREAD_NUMBER);
	g_ddos_stat_log_thread_para = para;

	return SUCCE;
}

void *ddos_log_thread(void *data)
{

	PGconn *ddos_conn;
	uint32_t index;
	ddos_conn = flow_connect_db(g_ddos_conf.gpq_info);//connect db
	if(NULL == ddos_conn){
		ddos_debug(DDOS_MID_LOG,"connet db fail!\n");
		return NULL;
	}
	((struct ddos_log_empty_para *)data)->ddos_conn = ddos_conn;
	
	UTAF_PER_LCORE(ddos_thread_id) = ((struct ddos_log_empty_para *)data)->tid;

	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
	
	while (1)
	{
		if ( ((struct ddos_log_empty_para *)data)->index == -1 )
		{
			nanosleep(&sleeptime, NULL);
			continue;
		}
		index = (((struct ddos_log_empty_para *)data)->tid * LOG_OUT_INTERVAL)+((struct ddos_log_empty_para *)data)->index;
		printf("\n-------------index = %d-------------\n",index);
		ddos_log(index);
		
	/*	if(FAIL == ddos_log(index,ddos_conn))
		{
			ddos_debug(DDOS_MID_LOG,"ddos_log  fail,reconnect db!\n");
			gpq_disconnect(ddos_conn);
			ddos_conn = flow_connect_db(g_ddos_conf.gpq_info);
			if(NULL == ddos_conn){
			ddos_debug(DDOS_MID_LOG,"reconnet db fail!\n");
			return NULL;
			}
		}*/
	
		((struct ddos_log_empty_para *)data)->index = -1;
	}
	
		return NULL;
}

int32_t ddos_log_thread_crt(void)
{
    pthread_t tid;
	int i;
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos_log_thread_crt!\n");


	/* create wait thread */
    if ( pthread_create(&tid, NULL, ddos_log_wait_thread, NULL) != 0 )
    {
        printf("pthread_create() ddos_log_wait_thread failed.\n");
        return FAIL;
    }
    if ( pthread_detach(tid) != 0 )
    {
        printf("pthread_detach ddos_log_wait_thread failed.\n");
		return FAIL;
    }
	printf("ddos log wait thread create successfull\n");

	/* create detect thread */
	for(i = 0;i < LOG_THREAD_NUMBER;i++)
	{
		g_ddos_stat_log_thread_para[i].index = -1;
		g_ddos_stat_log_thread_para[i].tid = i;
    	if ( pthread_create(&tid, NULL, ddos_log_thread, &g_ddos_stat_log_thread_para[i]) != 0 )
    	{
    	    printf("pthread_create() ddos_log_thread failed.\n");
    	    CA_LOG(LOG_MODULE, LOG_PROC, "pthread_create() ddos_log_thread failed.\n");
    	    return FAIL;
    	}
    	if ( pthread_detach(tid) != 0 )
    	{
    	    printf("pthread_detach ddos_log_thread failed.\n");
    	    CA_LOG(LOG_MODULE, LOG_PROC, "pthread_detach ddos_log_thread failed.\n");
			return FAIL;
    	}
		printf("ddos log thread create successfull\n");
	}

	return SUCCE;
}

int32_t ddos_log_init(void)
{
//	g_ddos_stat_log_thread_para.index = -1;
//	g_ddos_stat_log_thread_para
	pthread_mutex_init(&min_store_lock, NULL);


	if(FAIL == ddos_log_hash_init())
	{
		printf("ddos log hash init faild .\n");
		return FAIL;
	}

	if (FAIL == ddos_log_thread_crt())
	{
		printf("ddos log thread creat faild.\n");
		return FAIL;
	}
	return SUCCE;
}


/*#ifdef DEBUG_OUT_LOG


int main(void)
{
	total_data* count_data = NULL;
	log_tm_list_node *tm_node_head = NULL;
	utaf_ddos_log_insert(count_data);
	output_timeout(tm_node_head);
	return 0;
}
#endif*/
