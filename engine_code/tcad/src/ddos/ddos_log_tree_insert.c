
#include "ddos_log_out.h"
#include "ddos_make_log.h"

#define TREE_DUBUG 1

#define TOP100HASHIP(ip)  ((ip)%(TOPNUM - 1)) 

node_sip_outddos_data *create_tree_node(session_item_t *count_data,total_data_list *statis_node)
{
	iplib_info_t ipinfo;

	time_t now_time = 0;
	node_sip_outddos_data *p  = malloc(sizeof(node_sip_outddos_data));
    memset(p,0,sizeof(node_sip_outddos_data));

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

		p->sip_attack_detail.tcp_syn_in_packets = count_data->ddos.stat.tcp.syn[dir_in].pkts;
		p->sip_attack_detail.tcp_syn_out_packets = count_data->ddos.stat.tcp.syn[dir_out].pkts;

		p->sip_attack_detail.tcp_syn_in_bytes = count_data->ddos.stat.tcp.syn[dir_in].bytes;
		p->sip_attack_detail.tcp_syn_out_bytes = count_data->ddos.stat.tcp.syn[dir_out].bytes;

		p->sip_attack_detail.tcp_ack_in_packets = count_data->ddos.stat.tcp.ack[dir_in].pkts;
		p->sip_attack_detail.tcp_ack_out_packets = count_data->ddos.stat.tcp.ack[dir_out].pkts;

		p->sip_attack_detail.tcp_ack_in_bytes = count_data->ddos.stat.tcp.ack[dir_in].bytes;
		p->sip_attack_detail.tcp_ack_out_bytes = count_data->ddos.stat.tcp.ack[dir_out].bytes;

		p->sip_attack_detail.flow_in_packets = count_data->ddos.stat.tcp.tcp[dir_in].pkts;
		p->sip_attack_detail.flow_out_packets = count_data->ddos.stat.tcp.tcp[dir_out].pkts;
		p->sip_attack_detail.flow_in_bytes = count_data->ddos.stat.tcp.tcp[dir_in].bytes;
		p->sip_attack_detail.flow_out_bytes = count_data->ddos.stat.tcp.tcp[dir_out].bytes;

	//	p->sip_attack_detail.tcp_new_num_ps = statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_new_old[dir_in];
	//	p->sip_attack_detail.tcp_live_num_ps = statis_node->tcp.conn_new[dir_in] - statis_node->tcp.conn_close[dir_in];

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
#if 1
int _insert_node_into_tree(node_sip_outddos_data** sip_node,node_destIP_outddos_data* node, session_item_t *count_data,total_data_list *statis_node,node_sip_outddos_data* pParent)
{
	node->sip_node_count++;
	if(NULL == *sip_node){
		
	//	printf("********_insert_node_into_tree node->sip_node_count:%d **********",node->sip_node_count);
		if(node->sip_node_count > SIP_TREE_MAX_DEEP){
			ddos_debug(DDOS_MID_LOG,"too much sip node:%d on dip:%d\n",node->sip_node_count,node->dip);
	//		printf("too much sip node:%d on dip:%d,no insert\n",node->sip_node_count,node->dip);
			return SUCCE;
		}
		*sip_node = (node_sip_outddos_data*)create_tree_node(count_data,statis_node);
		if(NULL == *sip_node){
			ddos_debug(DDOS_MID_LOG,"create_tree_node error!\n");
			return FAIL;
		}
		(*sip_node)->parent = pParent;
		return SUCCE;
	}

	if((ddos_get_oppo_mnt_ip(count_data)) < ((*sip_node)->sip))
		return _insert_node_into_tree(&(*sip_node)->left_child,node, count_data, statis_node,*sip_node);
	else if((ddos_get_oppo_mnt_ip(count_data)) > ((*sip_node)->sip))
		return _insert_node_into_tree(&(*sip_node)->right_child,node, count_data,statis_node, *sip_node);
	else {//(count_data->sip) == ((*sip_node)->sip)
			ddos_debug(DDOS_MID_LOG, "find the sip node in tree sip:%d!\n",(*sip_node)->sip);
			//printf("find the sip node!\n");
		#if 0
			if(get_sip_sum_detail(*sip_node,count_data,statis_node) == FAIL){
				ddos_debug(DDOS_MID_LOG,"get_sip_sum_detail fail!\n");
				return FAIL;
			}
#endif
		return SUCCE;

	}
	return SUCCE;
}
#endif

/*insert sip node into sip tree*/
int insert_node_into_tree(node_destIP_outddos_data* node,session_item_t *count_data,total_data_list *statis_node)
{
	if(NULL == node->sourceip_tree_root){
		node->sourceip_tree_root = create_tree_node(count_data,statis_node);
		if(NULL == node->sourceip_tree_root){
			CA_LOG(LOG_MODULE,LOG_PROC,"create root tree node error!\n");
			return FAIL;
		}
		return SUCCE;
	}
//	node->sip_node_count = 0;
	return _insert_node_into_tree(&node->sourceip_tree_root,node, count_data,statis_node,NULL);
//		assert(NULL != *ppTreeNode);	
}
/*update current top buf*/
/*int traverse_tree_putin_topbuf(node_sip_outddos_data *curr_top[],uint64_t flow_in_bytes)
{
	return SUCCE;
}*/

int traverse_tree(node_destIP_outddos_data* dip_node,
				node_sip_outddos_data* TreeNode,	
				int (*calculate_top_buf)(node_destIP_outddos_data* dip_node,node_sip_outddos_data* TreeNode))
{
        if(TreeNode)
        {
        		dip_node->sip_node_count++;
				ddos_debug(DDOS_MID_LOG, "start traverse tree!dip:%d ,node ptr:%p,curr_top_num:%d,sip_node_count:%d\n",
							dip_node->dip,dip_node,dip_node->curr_top_num,dip_node->sip_node_count);
                if(calculate_top_buf(dip_node,TreeNode)){
                        if(traverse_tree(dip_node,TreeNode->left_child,calculate_top_buf)){
                                if(traverse_tree(dip_node,TreeNode->right_child,calculate_top_buf)){
                                	return 1;
                                }
                        }
                }
				return 0;
        }else return 1;
}

int ddos_traverse_tree(node_destIP_outddos_data *node)
{
	ddos_debug(DDOS_MID_LOG, "start traverse tree!dip:%d ,node ptr:%p,curr_top_num:%d,sip_node_count:%d\n",node->dip,node,node->curr_top_num,node->sip_node_count);
	node->sip_node_count = 0;
	traverse_tree(node,node->sourceip_tree_root,calculate_top_buf);
	node->sip_node_count = 0;
	return SUCCE;
}     

int insert_curr_topbuf(int insert_loc,int num,node_destIP_outddos_data* dip_node,node_sip_outddos_data* TreeNode)
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

int calculate_top_buf(node_destIP_outddos_data* dip_node,node_sip_outddos_data* TreeNode)
{
//	traverse_tree_putin_topbuf(dip_node->curr_top,TreeNode->sip_attack_detail.flow_in_bytes);
		uint64_t curr_bps = TreeNode->sip_attack_detail.flow_in_bytes + TreeNode->sip_attack_detail.flow_out_bytes;
		
		ddos_debug(DDOS_MID_LOG, "calculate_top_buf,dip:%d ,node ptr:%p,curr_top_num:%d,sip_node_count:%d,sip:%u\n",dip_node->dip,dip_node,dip_node->curr_top_num,dip_node->sip_node_count,TreeNode->sip);
		node_sip_outddos_data *tmp = 0;
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
		insert_curr_topbuf(insert_loc,num,dip_node,TreeNode);
	return SUCCE;
}

/* 查找是否建立hash值，如果没有，建立hash值，如果已经建立，比较ip是否相等 */
int log_haship_find(node_curr_top100 *MergeArray, uint32_t *array, node_curr_top100 *node, int n)
{
	int x = 0;

	/* 后面的值比之前的大，出现错误 */
	if (n && (MergeArray[n - 1].flow_bps < node->flow_bps))
	{
		ddos_debug(DDOS_MID_LOG,"error,last bps:%lu < now bps:%lu,n:%d\n", MergeArray[n - 1].flow_bps, node->flow_bps, n);
	}

	if (!array[0])
	{
		memcpy(&MergeArray[n], node, sizeof(node_curr_top100));
		array[0] = node->sip;

		return n + 1;
	}

	while (array[x])
	{
		if (array[x] == node->sip)
		{
			return n; /* 找到相同ip，后来数据比之前肯定小，不需比较flow_bps */
		}
		x++;
	}

	/* 哈希值相等，ip不等 */
	memcpy(&MergeArray[n], node, sizeof(node_curr_top100));
	array[x] = node->sip;

	return n + 1;
}

/* 将当前top100与攻击top100比较排序,算出最新攻击top100 */
int update_attack_top(rbtree_node_destIP_outddos_data *node)
{
	int i, j, n;
	int curr_num = node->curr_top_num;
	int att_num = node->att_top_num;
	int len = 0;
	node_curr_top100 *MergeArray = NULL;
	uint32_t hasharray[TOPNUM][TOPNUM];

	/* att_top中还没有值，拷贝第一次得到的top100 */
	if (0 == node->att_top_num)
	{
		memcpy(node->att_top, node->curr_top, sizeof(node_curr_top100) * curr_num);
		node->att_top_num = curr_num;

		return SUCCE;
	}

	if (curr_num + att_num < TOPNUM) /* 获得拷贝长度 */
	{
		len = curr_num + att_num + 1;
	}
	else
	{
		len = TOPNUM + 1;
	}

	MergeArray = malloc(sizeof(node_curr_top100) * len);
	if (NULL == MergeArray)
	{
		return FAIL;

	}
	memset(MergeArray, 0, sizeof(node_curr_top100) * len);

	for (i = 0; i < TOPNUM; i++)
	{
		for (j = 0; j < TOPNUM; j++)
		{
			hasharray[i][j] = 0;
		}
	}

	i = j = n = 0;

	while (i < curr_num && j < att_num && n < TOPNUM) /* 循环一直进行到拷贝完某一个数组的元素为止 */
	{
		if (node->curr_top[i].flow_bps > node->att_top[j].flow_bps)	/* 拷贝array1的元素 */
		{
			n = log_haship_find(MergeArray, hasharray[TOP100HASHIP(node->curr_top[i].sip)], &node->curr_top[i], n);
			i++;
		}
		else if (node->curr_top[i].flow_bps < node->att_top[j].flow_bps)/*  拷贝array2的元素 */
		{
			n = log_haship_find(MergeArray, hasharray[TOP100HASHIP(node->att_top[j].sip)], &node->att_top[j], n);
			j++;
		}
		else	/* 相等的元素拷贝 */
		{
			if (node->curr_top[i].sip != node->att_top[j].sip) /* 值相等,ip不等，需要都留下来 */
			{
				n = log_haship_find(MergeArray, hasharray[TOP100HASHIP(node->curr_top[i].sip)], &node->curr_top[i], n);
				if (n < TOPNUM)
				{
					n = log_haship_find(MergeArray, hasharray[TOP100HASHIP(node->att_top[j].sip)], &node->att_top[j], n);
				}
				i++;
				j++;
			}
			else /* ip相等，不需再做ip hash值查找，两个数组中相同ip最多2个 */
			{
				memcpy(&MergeArray[n], &node->curr_top[i], sizeof(node_curr_top100));
				n++;
				i++;
				j++;
			}
		}
	}

	if (i == curr_num) 	/*  如果array1已经被拷贝完毕就拷贝array2的元素 */
	{
		while (j < att_num && n < TOPNUM)
		{
			n = log_haship_find(MergeArray, hasharray[TOP100HASHIP(node->att_top[j].sip)], &node->att_top[j], n);
		    j++;
		}
	}
	else /*  如果array2已经被拷贝完毕就拷贝array1的元素 */
	{
		while (i < curr_num && n < TOPNUM)
		{
			n = log_haship_find(MergeArray, hasharray[TOP100HASHIP(node->curr_top[i].sip)], &node->curr_top[i], n);
			i++;
	    }
	}

	n = DDOS_MIN(n, TOPNUM);
	memcpy(node->att_top, MergeArray, sizeof(node_curr_top100) * n);
	node->att_top_num = n;

	free(MergeArray);

	return SUCCE;
}

