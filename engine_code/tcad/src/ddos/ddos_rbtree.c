/*************************************************************************
  > File Name: ddos_rbtree.c
  > Author: xuziquan
  > Mail: xuziquan@chanct.com 
  > Created Time: Sep 17 2015 09:57:33 AM ICT
 ************************************************************************/
//#include "ddos_log_out_rbtree.h"
#include "ddos_log_out.h"
#include "ddos_make_log.h"

extern int gen_random(void);


 rbtree_node_sip_outddos_data *create_sip_rbtree_node(session_item_t *count_data,total_data_list *statis_node)
{
	iplib_info_t ipinfo;

	time_t now_time = 0;
	rbtree_node_sip_outddos_data *p  = malloc(sizeof(rbtree_node_sip_outddos_data));
    memset(p,0,sizeof(rbtree_node_sip_outddos_data));

    p->sip = ddos_get_oppo_mnt_ip(count_data);
	p->dip = ddos_get_mnt_ip(count_data);
//    if (IPV4_VERSION == count_data->ip_version) {
	if (IPLIB_OK == iplib_find((p->sip), &ipinfo)) {
		p->s_country_id = ipinfo.country;
		p->s_city_id = ipinfo.city_id;
	}
	if (IPLIB_OK == iplib_find((p->dip), &ipinfo)) {
		p->d_country_id = ipinfo.country;
		p->d_city_id = ipinfo.city_id;
	}
	//printf("\n s_country_id:%d,s_city_id:%d,d_country_id:%d,d_city_id:%d\n",
	//	p->s_country_id,p->s_city_id,p->d_country_id,p->d_city_id);

    switch(count_data->protocol){
	case PROTO_TCP:
		ddos_debug(DDOS_MID_LOG, "TCP[i]:%ld %ld TCP[o]:%ld %ld SYN[i]:%ld %ld SYN[o]:%ld %ld ACK[i]:%ld %ld ACK[o]:%ld %ld\n", 
					count_data->ddos.stat.tcp.tcp[dir_in].pkts, count_data->ddos.stat.tcp.tcp[dir_in].bytes,
					count_data->ddos.stat.tcp.tcp[dir_out].pkts, count_data->ddos.stat.tcp.tcp[dir_out].bytes,
					count_data->ddos.stat.tcp.syn[dir_in].pkts, count_data->ddos.stat.tcp.syn[dir_in].bytes,
					count_data->ddos.stat.tcp.syn[dir_out].pkts, count_data->ddos.stat.tcp.syn[dir_out].bytes,
					count_data->ddos.stat.tcp.ack[dir_in].pkts, count_data->ddos.stat.tcp.ack[dir_in].bytes,
					count_data->ddos.stat.tcp.ack[dir_out].pkts, count_data->ddos.stat.tcp.ack[dir_out].bytes);
		p->sip_attack_detail.tcp_in_packets = count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.tcp_out_packets = count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.tcp_in_bytes = count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.tcp_out_bytes = count_data->ddos.stat.tcp.tcp[dir_out].bytes;

		p->sip_attack_detail.tcp_syn_in_packets  = count_data->ddos.stat.tcp.syn[dir_in].pkts;
		p->sip_attack_detail.tcp_syn_out_packets = count_data->ddos.stat.tcp.syn[dir_out].pkts;

		p->sip_attack_detail.tcp_syn_in_bytes    = count_data->ddos.stat.tcp.syn[dir_in].bytes;
		p->sip_attack_detail.tcp_syn_out_bytes   = count_data->ddos.stat.tcp.syn[dir_out].bytes;

		p->sip_attack_detail.tcp_ack_in_packets  = count_data->ddos.stat.tcp.ack[dir_in].pkts;
		p->sip_attack_detail.tcp_ack_out_packets = count_data->ddos.stat.tcp.ack[dir_out].pkts;

		p->sip_attack_detail.tcp_ack_in_bytes    = count_data->ddos.stat.tcp.ack[dir_in].bytes;
		p->sip_attack_detail.tcp_ack_out_bytes   = count_data->ddos.stat.tcp.ack[dir_out].bytes;

		p->sip_attack_detail.flow_in_packets = count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets = count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes = count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes = count_data->ddos.stat.tcp.tcp[dir_out].bytes;

		//p->sip_attack_detail.tcp_new_num_ps = statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_new_old[dir_in];
		//p->sip_attack_detail.tcp_live_num_ps = statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_close[dir_in];
		p->sip_attack_detail.tcp_new_old_first = statis_node->tcp.conn_new_old[dir_in] + statis_node->tcp.conn_new_old[dir_out];
		//p->sip_attack_detail.tcp_new_old = statis_node->tcp.conn_new_old[dir_in] + statis_node->tcp.conn_new_old[dir_out];
		p->sip_attack_detail.tcp_new = statis_node->tcp.conn_new[dir_in] + statis_node->tcp.conn_new[dir_out];
		p->sip_attack_detail.tcp_close = statis_node->tcp.conn_close[dir_in] + statis_node->tcp.conn_close[dir_out];


		switch (count_data->service_type)
		{
				case APP_CHARGEN:
					p->sip_attack_detail.chargen_in_packets = count_data->ddos.stat.tcp.tcp[dir_in].pkts;
					p->sip_attack_detail.chargen_out_packets = count_data->ddos.stat.tcp.tcp[dir_out].pkts;
					p->sip_attack_detail.chargen_in_bytes = count_data->ddos.stat.tcp.tcp[dir_in].bytes;
					p->sip_attack_detail.chargen_out_bytes = count_data->ddos.stat.tcp.tcp[dir_out].bytes;
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
					p->sip_attack_detail.dns_in_packets = count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.dns_out_packets = count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.dns_in_bytes = count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.dns_out_bytes = count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_NTP:
					p->sip_attack_detail.ntp_in_packets = count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.ntp_out_packets = count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.ntp_in_bytes = count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.ntp_out_bytes = count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_SSDP:
					p->sip_attack_detail.ssdp_in_packets = count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.ssdp_out_packets = count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.ssdp_in_bytes = count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.ssdp_out_bytes = count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_SNMP:
					p->sip_attack_detail.snmp_in_packets = count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.snmp_out_packets = count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.snmp_in_bytes = count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.snmp_out_bytes = count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_CHARGEN:
					p->sip_attack_detail.chargen_in_packets = count_data->ddos.stat.udp.udp[dir_in].pkts;
					p->sip_attack_detail.chargen_out_packets = count_data->ddos.stat.udp.udp[dir_out].pkts;
					p->sip_attack_detail.chargen_in_bytes = count_data->ddos.stat.udp.udp[dir_in].bytes;
					p->sip_attack_detail.chargen_out_bytes = count_data->ddos.stat.udp.udp[dir_out].bytes;
					break;
					/* other app */
				default:
					printf("unknown udp app protocol\n");
					break;
		}

		p->sip_attack_detail.flow_in_packets = count_data->ddos.stat.udp.udp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets = count_data->ddos.stat.udp.udp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes = count_data->ddos.stat.udp.udp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes = count_data->ddos.stat.udp.udp[dir_out].bytes;
		break;
	case PROTO_ICMP:
		ddos_debug(DDOS_MID_LOG, "ICMP[in]:%ld %ld ICMP[out]:%ld %ld\n", 
					count_data->ddos.stat.icmp.icmp[dir_in].pkts, count_data->ddos.stat.icmp.icmp[dir_in].bytes,
					count_data->ddos.stat.icmp.icmp[dir_out].pkts, count_data->ddos.stat.icmp.icmp[dir_out].bytes);
		p->sip_attack_detail.icmp_in_packets = count_data->ddos.stat.icmp.icmp[dir_in].pkts;
		p->sip_attack_detail.icmp_out_packets = count_data->ddos.stat.icmp.icmp[dir_out].pkts;
		p->sip_attack_detail.icmp_in_bytes = count_data->ddos.stat.icmp.icmp[dir_in].bytes;
		p->sip_attack_detail.icmp_out_bytes = count_data->ddos.stat.icmp.icmp[dir_out].bytes;

		p->sip_attack_detail.flow_in_packets = count_data->ddos.stat.icmp.icmp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets = count_data->ddos.stat.icmp.icmp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes = count_data->ddos.stat.icmp.icmp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes = count_data->ddos.stat.icmp.icmp[dir_out].bytes;
		break;
	default:
		printf("log unknown protocol!\n");
		break;
	}

//	p->sip_attack_detail. = count_data->ddos_count_attack_detail;
	p->attack_id = get_attack_id(statis_node->attack_type);
//	p->attack_id = statis_node->attack_type;

	time(&now_time);
	p->start_time = now_time;
	p->end_time = now_time;
//	p->start_time = count_data->start_time;
//	p->end_time = count_data->end_time;

	return p;
}

rbtree_node_destIP_outddos_data *create_rbtree_dip_node(session_item_t *count_data,total_data_list *statis_node)
{
	uint32_t res;
	time_t now_time=0;
	rbtree_node_destIP_outddos_data *dip_node = malloc(sizeof(rbtree_node_destIP_outddos_data));
	if(NULL == dip_node){
		printf("malloc error in func rbtree_create_dip_node!\n");
		return NULL;
	}
	memset(dip_node,0,sizeof(rbtree_node_destIP_outddos_data));
	res=pthread_rwlock_init(&(dip_node->rwlock),NULL);//init rwlock
    if (res != 0)
    {
        printf("rwlock initialization failed in func rbtree_create_dip_node\n");
        return NULL;
    }

//	uint32_t unit_id;
	time(&now_time);

	dip_node->start_time = now_time;
	dip_node->end_time = now_time;
	//Uint32 sip;

	dip_node->dip = ddos_get_mnt_ip(count_data);
//	dip_node-> sip_node_count = 0;
	//gen a unique num
	dip_node->query_id = gen_random() + dip_node->dip;
	
	//dip_node->dip_attack_detail = count_data->ddos_count_attack_detail		

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
//	query_id
#if 0
	dip_node->sip_tree_root = (rbtree_node_sip_outddos_data*)create_sip_rbtree_node(count_data,statis_node);
	if(NULL == dip_node->sip_tree_root){
		ddos_debug(DDOS_MID_LOG,"create_sip_rbtree_node error!\n");
		return NULL;
	}

	struct rb_root root;
	root.rb_node = &dip_node->sip_tree_root->rbtree_node;

	//rbtree_node_sip_outddos_data *sip_node;
	//uint32_t sip = ddos_get_oppo_mnt_ip(count_data);

    struct rb_node **tmp = &(root.rb_node), *parent = NULL;
	
    rb_link_node(&dip_node->sip_tree_root->rbtree_node, parent, tmp);
    rb_insert_color(&dip_node->sip_tree_root->rbtree_node, &root);
#endif
	rbtree_insert_node_into_tree(dip_node,count_data,statis_node);


/*	dip_node->curr_top_ptr[TOPNUM]; 
	dip_node->curr_top[TOPNUM];
	dip_node->att_top[TOPNUM];

	dip_node->prev;
	dip_node->next;*/

	return dip_node;
}

int rbtree_get_sip_sum_detail(rbtree_node_sip_outddos_data *node,session_item_t *count_data,total_data_list *statis_node)
{
	time_t now_time = 0;
	rbtree_node_sip_outddos_data *p = node;
	uint32_t tmp_attack_id;
	
	tmp_attack_id = get_attack_id(statis_node->attack_type);
	if(node->attack_id != 0 && node->attack_id != tmp_attack_id){
		node->attack_id = BLENDED_ATT;	
	}
//	node->end_time = MAX_T(time_t,node->end_time,count_data->end_time);
//	node->start_time = MIN_T(time_t,node->start_time,count_data->start_time);
	time(&now_time);
	node->end_time = now_time;

	switch(count_data->protocol){
	case PROTO_TCP:
		ddos_debug(DDOS_MID_LOG, "TCP[i]:%ld %ld TCP[o]:%ld %ld SYN[i]:%ld %ld SYN[o]:%ld %ld ACK[i]:%ld %ld ACK[o]:%ld %ld\n", 
					count_data->ddos.stat.tcp.tcp[dir_in].pkts, count_data->ddos.stat.tcp.tcp[dir_in].bytes,
					count_data->ddos.stat.tcp.tcp[dir_out].pkts, count_data->ddos.stat.tcp.tcp[dir_out].bytes,
					count_data->ddos.stat.tcp.syn[dir_in].pkts, count_data->ddos.stat.tcp.syn[dir_in].bytes,
					count_data->ddos.stat.tcp.syn[dir_out].pkts, count_data->ddos.stat.tcp.syn[dir_out].bytes,
					count_data->ddos.stat.tcp.ack[dir_in].pkts, count_data->ddos.stat.tcp.ack[dir_in].bytes,
					count_data->ddos.stat.tcp.ack[dir_out].pkts, count_data->ddos.stat.tcp.ack[dir_out].bytes);
		p->sip_attack_detail.tcp_in_packets += count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.tcp_out_packets += count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.tcp_in_bytes += count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.tcp_out_bytes += count_data->ddos.stat.tcp.tcp[dir_out].bytes;

		p->sip_attack_detail.tcp_syn_in_packets  = count_data->ddos.stat.tcp.syn[dir_in].pkts;
		p->sip_attack_detail.tcp_syn_out_packets = count_data->ddos.stat.tcp.syn[dir_out].pkts;

		p->sip_attack_detail.tcp_syn_in_bytes    = count_data->ddos.stat.tcp.syn[dir_in].bytes;
		p->sip_attack_detail.tcp_syn_out_bytes   = count_data->ddos.stat.tcp.syn[dir_out].bytes;

		p->sip_attack_detail.tcp_ack_in_packets  = count_data->ddos.stat.tcp.ack[dir_in].pkts;
		p->sip_attack_detail.tcp_ack_out_packets = count_data->ddos.stat.tcp.ack[dir_out].pkts;

		p->sip_attack_detail.tcp_ack_in_bytes    = count_data->ddos.stat.tcp.ack[dir_in].bytes;
		p->sip_attack_detail.tcp_ack_out_bytes   = count_data->ddos.stat.tcp.ack[dir_out].bytes;

		p->sip_attack_detail.flow_in_packets += count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets += count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes += count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes += count_data->ddos.stat.tcp.tcp[dir_out].bytes;

		//p->sip_attack_detail.tcp_new_num_ps += statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_new_old[dir_in];
		//p->sip_attack_detail.tcp_live_num_ps += statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_close[dir_in];
		//p->sip_attack_detail.tcp_new_old = statis_node->tcp.conn_new_old[dir_in] + statis_node->tcp.conn_new_old[dir_out];
		p->sip_attack_detail.tcp_new = statis_node->tcp.conn_new[dir_in] + statis_node->tcp.conn_new[dir_out];
		p->sip_attack_detail.tcp_close = statis_node->tcp.conn_close[dir_in] + statis_node->tcp.conn_close[dir_out];
		
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
					/* other app */
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

int rbtree_get_dip_sum_detail(rbtree_node_destIP_outddos_data *node,session_item_t *count_data,total_data_list *statis_node)
{
/*	switch(node->attack_id){
		case BLENDED_ATT: break;
		case */
	time_t now_time = 0;
	//uint32_t tmp_attack_id = node->attack_id;
	uint32_t tmp_attack_id = get_attack_id(statis_node->attack_type);
	//node->attack_id = get_attack_id(statis_node->attack_type);
	if(node->attack_id !=0 && tmp_attack_id != node->attack_id){
		node->attack_id = BLENDED_ATT;	
	}

//	node->end_time = MAX_T(time_t,node->end_time,count_data->end_time);
//	node->start_time = MIN_T(time_t,node->start_time,count_data->start_time);
	time(&now_time);
	node->end_time = now_time; 
	
	switch(count_data->protocol){
		case PROTO_TCP:
			node->flow_pps += (count_data->ddos.stat.tcp.tcp[dir_in].pkts + count_data->ddos.stat.tcp.tcp[dir_out].pkts);
			node->flow_bps += (count_data->ddos.stat.tcp.tcp[dir_in].bytes + count_data->ddos.stat.tcp.tcp[dir_out].bytes);
		//	node->sip_node_count++;
			break;
		case PROTO_UDP:
			node->flow_pps += (count_data->ddos.stat.udp.udp[dir_in].pkts + count_data->ddos.stat.udp.udp[dir_out].pkts);
			node->flow_bps += (count_data->ddos.stat.udp.udp[dir_in].bytes + count_data->ddos.stat.udp.udp[dir_out].bytes);
	//		node->sip_node_count++;
			break;
		case PROTO_ICMP:
			node->flow_pps += (count_data->ddos.stat.icmp.icmp[dir_in].pkts + count_data->ddos.stat.icmp.icmp[dir_out].pkts);
			node->flow_bps += (count_data->ddos.stat.icmp.icmp[dir_in].bytes + count_data->ddos.stat.icmp.icmp[dir_out].bytes);
	//		node->sip_node_count++;
			break;
		default:
			printf("log in func get_dip_sum_detail unknown protocol!\n");
			break;
	}
//	uint32_t query_id
	return SUCCE;
}

int rbtree_insert_curr_topbuf(int insert_loc,int num,rbtree_node_destIP_outddos_data* dip_node,rbtree_node_sip_outddos_data* TreeNode)
{
	int i = 0;
	if(num == TOPNUM){
		num = TOPNUM -1;
	}
	for(i = num;i > insert_loc;i--)
	{
		dip_node ->curr_top_ptr[i] = dip_node -> curr_top_ptr[i-1];
	}
	dip_node ->curr_top_ptr[insert_loc] = TreeNode;
	dip_node ->curr_top_num = num + 1;
	return SUCCE;
}

int rbtree_calculate_top_buf(rbtree_node_destIP_outddos_data* dip_node,rbtree_node_sip_outddos_data* TreeNode)
{
//	traverse_tree_putin_topbuf(dip_node->curr_top,TreeNode->sip_attack_detail.flow_in_bytes);
		uint64_t curr_bps = TreeNode->sip_attack_detail.flow_in_bytes + TreeNode->sip_attack_detail.flow_out_bytes;
		
		ddos_debug(DDOS_MID_LOG, "calculate_top_buf,dip:%d ,node ptr:%p,curr_top_num:%d,sip_node_count:%d,sip:%u\n",dip_node->dip,dip_node,dip_node->curr_top_num,dip_node->sip_node_count,TreeNode->sip);
		rbtree_node_sip_outddos_data *tmp = NULL;
		int num = (dip_node->sip_node_count) - 1;

		if(num >= TOPNUM)   num = TOPNUM;
		if(num == 0)
		{
			//dip_node->curr_top[0] = value;
			dip_node->curr_top_ptr[0] = TreeNode;
			dip_node->curr_top_num = 1;
			return SUCCE;
		}
		if(num == 1)
		{
			dip_node ->curr_top_num = 2;
			if((dip_node->curr_top_ptr[0]->sip_attack_detail.flow_in_bytes +  dip_node->curr_top_ptr[0]->sip_attack_detail.flow_out_bytes)
				> curr_bps) 
			{
				dip_node->curr_top_ptr[1] = TreeNode;
				return SUCCE;
			}
			else 
			{
				tmp = dip_node->curr_top_ptr[0];
				dip_node->curr_top_ptr[0] = TreeNode;
				dip_node->curr_top_ptr[1] = tmp;
				return SUCCE;
			}
		}
		int start = 0;  
		int end = num-1;
	    int mid = (start + end)/2;
		int insert_loc = 0;
		
		if(curr_bps 
			< (dip_node->curr_top_ptr[end]->sip_attack_detail.flow_in_bytes + dip_node->curr_top_ptr[end]->sip_attack_detail.flow_out_bytes))
		{
			if(end < TOPNUM-1){
				dip_node->curr_top_ptr[end+1] = TreeNode;
				dip_node ->curr_top_num = end+1+1;
				return SUCCE;
			}
			if(end == TOPNUM-1){
				return SUCCE;
			}
		}
		ddos_debug(DDOS_MID_LOG,"num:%d\n",num);
		while(start < end)
		{
		ddos_debug(DDOS_MID_LOG,"start:%d, end:%d,mid:%d\n",start,end,mid);
				if (curr_bps
					< (dip_node->curr_top_ptr[mid]->sip_attack_detail.flow_in_bytes + dip_node->curr_top_ptr[mid]->sip_attack_detail.flow_out_bytes))
				{
						if(curr_bps  
							>= (dip_node->curr_top_ptr[mid+1]->sip_attack_detail.flow_in_bytes +dip_node->curr_top_ptr[mid+1]->sip_attack_detail.flow_out_bytes))
						{
							insert_loc = mid+1;
							break;
						}
						start = mid + 1;
	    				mid = (start + end)/2;
						
				}   
				else if (curr_bps
					> (dip_node->curr_top_ptr[mid]->sip_attack_detail.flow_in_bytes + dip_node->curr_top_ptr[mid]->sip_attack_detail.flow_out_bytes))
				{
						if(mid == 0){  //insert location default 0
							break;
						}
						if(curr_bps
							<= (dip_node->curr_top_ptr[mid-1]->sip_attack_detail.flow_in_bytes + dip_node->curr_top_ptr[mid-1]->sip_attack_detail.flow_out_bytes))
						{
							insert_loc = mid;
							break;
						}
						end = mid - 1;
	    				mid = (start + end)/2;
				}
				else //value == buf[mid]
				{
					insert_loc = mid;
					break;
				}
		}   
		rbtree_insert_curr_topbuf(insert_loc,num,dip_node,TreeNode);
	return SUCCE;
}
#if 1
int rbtree_insert_node_into_tree(rbtree_node_destIP_outddos_data* node,session_item_t *count_data,total_data_list *statis_node)
{
//	struct rb_root *rbtree_root = node->rbtree_root_node;
//	struct rb_root root;
//	root.rb_node = &node->sip_tree_root->rbtree_node;

	rbtree_node_sip_outddos_data *sip_node;
	uint32_t sip = ddos_get_oppo_mnt_ip(count_data);
	//printf("-------insert rbtree sip:%s-----------\n",log_ip_ntoa(sip));

    struct rb_node **tmp = &(node->rbtree.rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
    	node->sip_node_count++;
		//printf("----------while sip_node_count=%d------------",node->sip_node_count);
		struct rbtree_node_sip_outddos_data *sip_node_this = container_of(*tmp, struct rbtree_node_sip_outddos_data, rbtree_node);
	//	printf("\n node:%s\n",log_ip_ntoa(sip_node_this->sip)); 
		parent = *tmp;
		if (sip < sip_node_this->sip)
	    	tmp = &((*tmp)->rb_left);
		else if (sip > sip_node_this->sip)
	    	tmp = &((*tmp)->rb_right);
		else {//sip == sip_node_this->sip
			printf("find the sip node in rbtree sip:%s!\n",log_ip_ntoa(sip));
			//printf("find the sip node!\n");
			if(rbtree_get_sip_sum_detail(sip_node_this,count_data,statis_node) == FAIL){
				ddos_debug(DDOS_MID_LOG,"get_sip_sum_detail fail!\n");
				return FAIL;
			}
	    	return SUCCE;
	    }
    }
    
    /* Add new node and rebalance tree. */
    if(node->sip_node_count > SIP_TREE_MAX_DEEP){
			ddos_debug(DDOS_MID_LOG,"too much sip node:%d on dip:%s\n",node->sip_node_count,log_ip_ntoa(node->dip));
	//		printf("too much sip node:%d on dip:%d,no insert\n",node->sip_node_count,node->dip);
			return SUCCE;
	}
	sip_node = (rbtree_node_sip_outddos_data*)create_sip_rbtree_node(count_data,statis_node);
	if(NULL == sip_node){
		ddos_debug(DDOS_MID_LOG,"create_sip_rbtree_node error!\n");
		return FAIL;
	}
    rb_link_node(&sip_node->rbtree_node, parent, tmp);
    rb_insert_color(&sip_node->rbtree_node, &node->rbtree);
#if 0
	struct rb_node *tmp_node;

	for (tmp_node = rb_first(&node->rbtree); tmp_node; tmp_node = rb_next(tmp_node))
	{
		struct rbtree_node_sip_outddos_data *sip_node_this;
		sip_node_this = container_of(tmp_node, struct rbtree_node_sip_outddos_data, rbtree_node);
		printf("--------------traverse : sip:%s-----------\n",log_ip_ntoa(sip_node_this->sip));
		}
#endif
    
    return SUCCE;
}
#endif
#if 0
int rbtree_insert_node(rbtree_node_destIP_outddos_data* node,session_item_t *count_data,total_data_list *statis_node)
{
//	struct rb_root *rbtree_root = node->rbtree_root_node;
	struct rb_root root;
	root.rb_node = &node->sip_tree_root->rbtree_node;

	rbtree_node_sip_outddos_data *sip_node;
	uint32_t sip = ddos_get_oppo_mnt_ip(count_data);
	printf("-------insert rbtree sip:%s-----------\n",log_ip_ntoa(sip));

    struct rb_node **tmp = &(root.rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
    	node->sip_node_count++;
		printf("----------while sip_node_count=%d------------",node->sip_node_count);
		struct rbtree_node_sip_outddos_data *sip_node_this = container_of(*tmp, struct rbtree_node_sip_outddos_data, rbtree_node);

		parent = *tmp;
		if (sip < sip_node_this->sip)
	    	tmp = &((*tmp)->rb_left);
		else if (sip > sip_node_this->sip)
	    	tmp = &((*tmp)->rb_right);
		else {//sip == sip_node_this->sip
			printf("find the sip node in rbtree sip:%s!\n",log_ip_ntoa(sip));
			//printf("find the sip node!\n");
			if(rbtree_get_sip_sum_detail(sip_node_this,count_data,statis_node) == FAIL){
				ddos_debug(DDOS_MID_LOG,"get_sip_sum_detail fail!\n");
				return FAIL;
			}
	    	return SUCCE;
	    }
    }
    
    /* Add new node and rebalance tree. */
    if(node->sip_node_count > SIP_TREE_MAX_DEEP){
			ddos_debug(DDOS_MID_LOG,"too much sip node:%d on dip:%s\n",node->sip_node_count,log_ip_ntoa(node->dip));
	//		printf("too much sip node:%d on dip:%d,no insert\n",node->sip_node_count,node->dip);
			return SUCCE;
	}
	sip_node = (rbtree_node_sip_outddos_data*)create_sip_rbtree_node(count_data,statis_node);
	if(NULL == sip_node){
		ddos_debug(DDOS_MID_LOG,"create_sip_rbtree_node error!\n");
		return FAIL;
	}
    rb_link_node(&sip_node->rbtree_node, parent, tmp);
    rb_insert_color(&sip_node->rbtree_node, &root);
#if 1
	struct rb_node *tmp_node;

	for (tmp_node = rb_first(&root); tmp_node; tmp_node = rb_next(tmp_node))
	{
		struct rbtree_node_sip_outddos_data *sip_node_this;
		sip_node_this = container_of(tmp_node, struct rbtree_node_sip_outddos_data, rbtree_node);
		printf("--------------traverse : sip:%s-----------\n",log_ip_ntoa(sip_node_this->sip));
		}
#endif
    
    return SUCCE;
}
#endif

int traverse_rbtree(rbtree_node_destIP_outddos_data* dip_node)
{
	if(NULL == dip_node->rbtree.rb_node)
	{
		CA_LOG(LOG_MODULE,LOG_PROC,"traverse_rbtree!sip_tree_root is NULL,dip:%s\n",log_ip_ntoa(dip_node->dip));
		return FAIL;
	}
//	struct rb_root root;
//	root.rb_node = &dip_node->sip_tree_root->rbtree_node;

	struct rb_node *node;
	struct rbtree_node_sip_outddos_data *sip_node_this;

	dip_node->sip_node_count = 0;
	for (node = rb_first(&dip_node->rbtree); node; node = rb_next(node))
	{
		dip_node->sip_node_count++;
		sip_node_this = container_of(node, struct rbtree_node_sip_outddos_data, rbtree_node);
//		printf("----------traverse_rbtree  sip:%s-------\n",log_ip_ntoa(sip_node_this->sip));
		if(FAIL == rbtree_calculate_top_buf(dip_node,sip_node_this)){
			CA_LOG(LOG_MODULE,LOG_PROC,"calculate_top_buf error\n");
			return FAIL;
		}
		//printf("%d ", rb_entry(node, struct mytype, my_node)->num);
//		ddos_debug(DDOS_MID_LOG,"traverse_rbtree sip:%s",log_ip_ntoa(sip_node_this->sip));
	}
	dip_node->sip_node_count = 0;

	ddos_debug(DDOS_MID_LOG,"traverse_rbtree over dip:%s",log_ip_ntoa(dip_node->dip));
	return SUCCE;
}


