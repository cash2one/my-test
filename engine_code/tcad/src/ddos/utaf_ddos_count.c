#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/prctl.h> 
#include <errno.h>
#include <time.h>
#include "spinlock.h"
#include "list.h"
#include "ip4.h"
#include "app.h"
#include "ddos.h"
#include "utaf_session.h"
#include "utaf_ddos_count.h"
#include "ddos_make_log.h"

extern volatile int __tm_out_flag_stat;
extern volatile time_t g_ddos_time_jiffies;
extern volatile int g_ddos_iptrie_conf_reload;

extern ddos_stat_norm_t *ddos_base_line_get_result(uint32_t ip);
extern void ddos_base_line_update(total_data_list *node, time_t time);
extern int utaf_ddos_log_insert(session_item_t *si,total_data_list *node);
extern int output_full_attack(uint32_t dip);
extern uint32_t ddos_mnt_ip_lookup(unsigned int ip);
extern int ddos_log_remove_monitorip(uint32_t dip);


struct ddos_stat_empty_para *g_ddos_stat_detect_thread_para;

volatile uint32_t ddos_statistic_index_current = 0;
ddos_detect_head_t  *g_ddos_detect_list;

#define DDOS_DETECT_RATIO  (g_ddos_conf.detect_ratio)

#define DDOS_COUNT_IPTABLES_MAX 256
ddos_iplist_bucket_t iphead[DDOS_COUNT_IPTABLES_MAX];


/* 统计上下行发方向流量 */
void ddos_data_count(session_item_t *si, ddos_group_counter_t *pstTotalData)
{
	switch (si->protocol) 
	{
		case PROTO_TCP:
			ddos_debug_lf(DDOS_MID_FLOW, "TCP[i]:%ld %ld TCP[o]:%ld %ld SYN[i]:%ld %ld SYN[o]:%ld %ld ACK[i]:%ld %ld ACK[o]:%ld %ld\n", 
					si->ddos.stat.tcp.tcp[dir_in].pkts, si->ddos.stat.tcp.tcp[dir_in].bytes,
					si->ddos.stat.tcp.tcp[dir_out].pkts, si->ddos.stat.tcp.tcp[dir_out].bytes,
					si->ddos.stat.tcp.syn[dir_in].pkts, si->ddos.stat.tcp.syn[dir_in].bytes,
					si->ddos.stat.tcp.syn[dir_out].pkts, si->ddos.stat.tcp.syn[dir_out].bytes,
					si->ddos.stat.tcp.ack[dir_in].pkts, si->ddos.stat.tcp.ack[dir_in].bytes,
					si->ddos.stat.tcp.ack[dir_out].pkts, si->ddos.stat.tcp.ack[dir_out].bytes);
			pstTotalData->c[e_ddos_stat_type_tcp][dir_in].pkts += si->ddos.stat.tcp.tcp[dir_in].pkts;
			pstTotalData->c[e_ddos_stat_type_tcp][dir_in].bytes += si->ddos.stat.tcp.tcp[dir_in].bytes;
			pstTotalData->c[e_ddos_stat_type_tcp][dir_out].pkts += si->ddos.stat.tcp.tcp[dir_out].pkts;
			pstTotalData->c[e_ddos_stat_type_tcp][dir_out].bytes += si->ddos.stat.tcp.tcp[dir_out].bytes;
			pstTotalData->c[e_ddos_stat_type_tcp_syn][dir_in].pkts   += si->ddos.stat.tcp.syn[dir_in].pkts;
			pstTotalData->c[e_ddos_stat_type_tcp_syn][dir_in].bytes  += si->ddos.stat.tcp.syn[dir_in].bytes;
			pstTotalData->c[e_ddos_stat_type_tcp_syn][dir_out].pkts  += si->ddos.stat.tcp.syn[dir_out].pkts;
			pstTotalData->c[e_ddos_stat_type_tcp_syn][dir_out].bytes += si->ddos.stat.tcp.syn[dir_out].bytes;
			pstTotalData->c[e_ddos_stat_type_tcp_ack][dir_in].pkts   += si->ddos.stat.tcp.ack[dir_in].pkts;
			pstTotalData->c[e_ddos_stat_type_tcp_ack][dir_in].bytes  += si->ddos.stat.tcp.ack[dir_in].bytes;
			pstTotalData->c[e_ddos_stat_type_tcp_ack][dir_out].pkts  += si->ddos.stat.tcp.ack[dir_out].pkts;
			pstTotalData->c[e_ddos_stat_type_tcp_ack][dir_out].bytes += si->ddos.stat.tcp.ack[dir_out].bytes;
			switch (si->service_type)
			{
				case APP_CHARGEN:
					pstTotalData->c[e_ddos_stat_type_chargen][dir_in].pkts   += si->ddos.stat.tcp.tcp[dir_in].pkts;
					pstTotalData->c[e_ddos_stat_type_chargen][dir_in].bytes  += si->ddos.stat.tcp.tcp[dir_in].bytes;
					pstTotalData->c[e_ddos_stat_type_chargen][dir_out].pkts  += si->ddos.stat.tcp.tcp[dir_out].pkts;
					pstTotalData->c[e_ddos_stat_type_chargen][dir_out].bytes += si->ddos.stat.tcp.tcp[dir_out].bytes;
					break;
				default:
					break;
			}
			break;
		case PROTO_UDP:
			ddos_debug_lf(DDOS_MID_FLOW, "UDP[in]:%ld %ld UDP[out]:%ld %ld\n", 
					si->ddos.stat.udp.udp[dir_in].pkts, si->ddos.stat.udp.udp[dir_in].bytes,
					si->ddos.stat.udp.udp[dir_out].pkts, si->ddos.stat.udp.udp[dir_out].bytes);
			switch(si->service_type)
			{
				case APP_DNS:
					pstTotalData->c[e_ddos_stat_type_dns][dir_in].pkts  += si->ddos.stat.udp.udp[dir_in].pkts;
					pstTotalData->c[e_ddos_stat_type_dns][dir_in].bytes += si->ddos.stat.udp.udp[dir_in].bytes;
					pstTotalData->c[e_ddos_stat_type_dns][dir_out].pkts += si->ddos.stat.udp.udp[dir_out].pkts;
					pstTotalData->c[e_ddos_stat_type_dns][dir_out].bytes += si->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_NTP:
					pstTotalData->c[e_ddos_stat_type_ntp][dir_in].pkts += si->ddos.stat.udp.udp[dir_in].pkts;
					pstTotalData->c[e_ddos_stat_type_ntp][dir_in].bytes += si->ddos.stat.udp.udp[dir_in].bytes;
					pstTotalData->c[e_ddos_stat_type_ntp][dir_out].pkts += si->ddos.stat.udp.udp[dir_out].pkts;
					pstTotalData->c[e_ddos_stat_type_ntp][dir_out].bytes += si->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_SSDP:
					pstTotalData->c[e_ddos_stat_type_ssdp][dir_in].pkts += si->ddos.stat.udp.udp[dir_in].pkts;
					pstTotalData->c[e_ddos_stat_type_ssdp][dir_in].bytes += si->ddos.stat.udp.udp[dir_in].bytes;
					pstTotalData->c[e_ddos_stat_type_ssdp][dir_out].pkts += si->ddos.stat.udp.udp[dir_out].pkts;
					pstTotalData->c[e_ddos_stat_type_ssdp][dir_out].bytes += si->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_SNMP:
					pstTotalData->c[e_ddos_stat_type_snmp][dir_in].pkts += si->ddos.stat.udp.udp[dir_in].pkts;
					pstTotalData->c[e_ddos_stat_type_snmp][dir_in].bytes += si->ddos.stat.udp.udp[dir_in].bytes;
					pstTotalData->c[e_ddos_stat_type_snmp][dir_out].pkts += si->ddos.stat.udp.udp[dir_out].pkts;
					pstTotalData->c[e_ddos_stat_type_snmp][dir_out].bytes += si->ddos.stat.udp.udp[dir_out].bytes;
					break;
				case APP_CHARGEN:
					pstTotalData->c[e_ddos_stat_type_chargen][dir_in].pkts += si->ddos.stat.udp.udp[dir_in].pkts;
					pstTotalData->c[e_ddos_stat_type_chargen][dir_in].bytes += si->ddos.stat.udp.udp[dir_in].bytes;
					pstTotalData->c[e_ddos_stat_type_chargen][dir_out].pkts += si->ddos.stat.udp.udp[dir_out].pkts;
					pstTotalData->c[e_ddos_stat_type_chargen][dir_out].bytes += si->ddos.stat.udp.udp[dir_out].bytes;
					break;
					/* other app */
				default:
					break;
			}
			break;
		case PROTO_ICMP:
			ddos_debug_lf(DDOS_MID_FLOW, "ICMP[in]:%ld %ld ICMP[out]:%ld %ld\n", 
					si->ddos.stat.icmp.icmp[dir_in].pkts, si->ddos.stat.icmp.icmp[dir_in].bytes,
					si->ddos.stat.icmp.icmp[dir_out].pkts, si->ddos.stat.icmp.icmp[dir_out].bytes);
			pstTotalData->c[e_ddos_stat_type_icmp][dir_in].pkts += si->ddos.stat.icmp.icmp[dir_in].pkts;
			pstTotalData->c[e_ddos_stat_type_icmp][dir_in].bytes += si->ddos.stat.icmp.icmp[dir_in].bytes;
			pstTotalData->c[e_ddos_stat_type_icmp][dir_out].pkts += si->ddos.stat.icmp.icmp[dir_out].pkts;
			pstTotalData->c[e_ddos_stat_type_icmp][dir_out].bytes += si->ddos.stat.icmp.icmp[dir_out].bytes;
			break;
		default:
			ddos_debug_lf(DDOS_MID_FLOW, "protocol unknown\n");
	}


	return;
}

/*
 * 功能: 为每一个检测线程指定位置信息index，
 * 通过全局变量g_ddos_stat_detect_thread_para[tid]传递
 * 给检测线程.index的范围为0到DDOS_STATISTIC_DETECT_LIST_MAX，
 * 每个检测线程内部要根据自己的线程ID去计算当前时间需要处理的
 * node list在detect list中的位置?
 */
void *ddos_statistic_wait_thread(void *data)
{
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
	register int index = 0;
	int i;

	prctl(PR_SET_NAME,(unsigned long)"ddos_stat_wait_thread");

	while (1)
	{
		m_wmb();

		if (ddos_fetch_and_dec(&__tm_out_flag_stat) == 1)
		{
			//ddos_debug(DDOS_MID_STAT, "new detect cycle\n");
			index++;
			__builtin_ia32_lfence();
			index = index%DDOS_STATISTIC_DETECT_LIST_MAX;

			for (i=0; i<DDOS_DETECT_THREAD_NUM; i++)
			{
				g_ddos_stat_detect_thread_para[i].index = index;
			}

			while (1)
			{
				for (i=0; i<DDOS_DETECT_THREAD_NUM; i++)
				{
					if (g_ddos_stat_detect_thread_para[i].index != -1)
						goto stag; /* sleep tag */
				}
				break;
stag:
				nanosleep(&sleeptime, NULL);
			}
		}
		else
		{
			nanosleep(&sleeptime, NULL);
		}
	}

	return NULL;
}
#if 0
void *ddos_statistic_wait_thread(void *data)
{
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
	register int index;
	int i;

	while (1)
	{
		m_wmb();

		if (ddos_fetch_and_dec(&__tm_out_flag_stat) == 1)
		{
			//ddos_debug(DDOS_MID_STAT, "new detect cycle\n");
			index = ddos_statistic_index_current + 1;
			__builtin_ia32_lfence();
			index = index%DDOS_STATISTIC_DETECT_LIST_MAX;

			for (i=0; i<DDOS_DETECT_THREAD_NUM; i++)
			{
				g_ddos_stat_detect_thread_para[i].index = index;
			}

			while (1)
			{
				for (i=0; i<DDOS_DETECT_THREAD_NUM; i++)
				{
					if (g_ddos_stat_detect_thread_para[i].index != -1)
						goto stag; /* sleep tag */
				}
				break;
stag:
				nanosleep(&sleeptime, NULL);
			}

			m_wmb();
			ddos_atomic_set(index, &ddos_statistic_index_current);
			m_wmb();
		}
		else
		{
			nanosleep(&sleeptime, NULL);
		}
	}

	return NULL;
}
#endif
//ddos_stat_norm_t bline_test = {0};
#if 0
ddos_stat_norm_t *ddos_base_line_get_result(uint32_t ip)
{
	return &bline_test; /* only for test */ 
}

void ddos_base_line_update(total_data_list *node, uint32_t time, uint32_t detect_result)
{
	ddos_debug(DDOS_MID_STAT, "update base line.\n");
	return;
}
#endif
void ddos_detect_info_d(total_data_list *node, ddos_stat_norm_t *result)
{
	int i;
	
	if(likely(!(g_ddos_conf.debug & DDOS_MID_DETECT)))
		return ;
	
	ddos_debug_lock();
	ddos_debug_f(DDOS_MID_DETECT, "node info(0x%0x):Tcp connection IN[new live]:%lu %lu\n", 
			node->ip,
			(node->tcp.conn_new[dir_in] - node->tcp.conn_new_old[dir_in]),
			(node->tcp.conn_new[dir_in] - node->tcp.conn_close[dir_in]));
	for (i=0; i<e_ddos_stat_type_max; i++)
	{
		ddos_debug_f(DDOS_MID_DETECT, "type[%d] IN[pkts bytes]:%lu %lu OUT[pkts bytes]:%lu %lu\n",i, 
				node->stTotalData.c[i][dir_in].pkts,
				node->stTotalData.c[i][dir_in].bytes,
				node->stTotalData.c[i][dir_out].pkts,
				node->stTotalData.c[i][dir_out].bytes);
	}

	ddos_debug_f(DDOS_MID_DETECT, "Tcp connetction IN[new live]:%lu %lu\n", 
			result->tcp_new_conn_s[dir_in], result->tcp_live_conn_s[dir_in]);
	for (i=0; i<e_ddos_stat_type_max; i++)
	{
		ddos_debug_f(DDOS_MID_DETECT, "type[%d] IN[pps bps]:%lu %lu OUT[pps bps]:%lu %lu\n",i, 
				result->rate[i][dir_in].pps,
				result->rate[i][dir_in].bps,
				result->rate[i][dir_out].pps,
				result->rate[i][dir_out].bps);
	}
	
	ddos_debug_unlock();
}

void ddos_detect_judge(ddos_detect_node_t *dnode, 
	uint8_t cflag /* configuration change flag */)
{
	ddos_stat_norm_t result, *rbase;
	total_data_list *node, tmp_node;
	uint32_t t;
	uint32_t detect_result = 0;
	int i, dir;
	
	if(unlikely(!dnode))
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "dnode is null\n");
		return ;
	}

	node = dnode->stat_node;
	//ddos_debug(DDOS_MID_STAT, "entry IP:0x%0x\n", node->ip);

	tmp_node = *node;
	node->tcp.conn_new_old[dir_in] = node->tcp.conn_new[dir_in];
	node->tcp.conn_new_old[dir_out] = node->tcp.conn_new[dir_out];
	memset(&node->stTotalData, 0x0, sizeof(node->stTotalData));

	t = g_ddos_time_jiffies - dnode->last_detect_time;	
	dnode->last_detect_time = g_ddos_time_jiffies;

	if (unlikely(0 == t)) /* 没有超过1s，不进行检测 */
		return ;
	
	for (i=0; i<e_ddos_stat_type_max; i++)
	{
		result.rate[i][dir_in].pps  = tmp_node.stTotalData.c[i][dir_in].pkts/t;
		result.rate[i][dir_in].bps  = (tmp_node.stTotalData.c[i][dir_in].bytes*8)/t;
		result.rate[i][dir_out].pps = tmp_node.stTotalData.c[i][dir_out].pkts/t;
		result.rate[i][dir_out].bps = (tmp_node.stTotalData.c[i][dir_out].bytes*8)/t;
	}
	if (tmp_node.tcp.conn_new[dir_in] < tmp_node.tcp.conn_close[dir_in])
	{
		tmp_node.tcp.conn_close[dir_in]=tmp_node.tcp.conn_new[dir_in];
	}
	if (tmp_node.tcp.conn_new[dir_out] < tmp_node.tcp.conn_close[dir_out])
	{
		tmp_node.tcp.conn_close[dir_out]=tmp_node.tcp.conn_new[dir_out];
	}

	result.tcp_new_conn_s[dir_in]   = (tmp_node.tcp.conn_new[dir_in]  - tmp_node.tcp.conn_new_old[dir_in])/t;
	result.tcp_new_conn_s[dir_out]  = (tmp_node.tcp.conn_new[dir_out] - tmp_node.tcp.conn_new_old[dir_out])/t;
	result.tcp_live_conn_s[dir_in]  = (tmp_node.tcp.conn_new[dir_in]  - tmp_node.tcp.conn_close[dir_in])/t;
	result.tcp_live_conn_s[dir_out] = (tmp_node.tcp.conn_new[dir_out] - tmp_node.tcp.conn_close[dir_out])/t;
	if(result.tcp_new_conn_s[dir_in] < 0)
	{
		result.tcp_new_conn_s[dir_in] = 0;
	}
	if(result.tcp_new_conn_s[dir_out] < 0)
	{
		result.tcp_new_conn_s[dir_out] = 0;
	}
	if(result.tcp_live_conn_s[dir_in] < 0)
	{
		result.tcp_live_conn_s[dir_in] = 0;
	}
	if(result.tcp_live_conn_s[dir_out] < 0)
	{
		result.tcp_live_conn_s[dir_out] = 0;
	}
	ddos_detect_info_d(&tmp_node, &result);

	/* get base line result from base line module */
	ddos_base_line_update(&tmp_node, t);
	ddos_debug(DDOS_MID_DETECT, "base line update **************************\n");

	rbase = ddos_base_line_get_result(tmp_node.ip);
	//rbase = &bline_test; /* only for test */ 
	if (!rbase)
	{
		//ddos_base_line_update(&tmp_node, time);
		ddos_debug(DDOS_MID_DETECT, "base line is null\n");
		return ;
	}
	ddos_debug(DDOS_MID_DETECT, "base line get_result [ tcp_new_conn_s ]: %lu %lu**************************\n",
			rbase->tcp_new_conn_s[dir_in], rbase->tcp_new_conn_s[dir_out]);

	for (dir=0; dir<flow_dir; dir++)
	{
		/* 根据配置判断需要检测哪个方向的流量 0:流出 1:流入 */
		if (g_ddos_conf.detect_dir_mask & (1<<dir))
		{
			/* 根据配置选择需要哪些指标来检测是否受到ddos攻击 */
			/* tcp */
			if ((g_ddos_conf.detect_type_mask & (1<<e_ddos_stat_type_tcp)) && 
					(((g_ddos_conf.detect_unit_mask & DDOS_DET_MASK_BPS) &&
					  (result.rate[e_ddos_stat_type_tcp][dir].bps > 
					   rbase->rate[e_ddos_stat_type_tcp][dir].bps*DDOS_DETECT_RATIO)) || 
					 ((g_ddos_conf.detect_unit_mask & DDOS_DET_MASK_PPS) && 
					  (result.rate[e_ddos_stat_type_tcp][dir].pps > 
					   rbase->rate[e_ddos_stat_type_tcp][dir].pps*DDOS_DETECT_RATIO))))
			{
				detect_result |= (1<<e_ddos_stat_type_tcp);
				ddos_debug_lf(DDOS_MID_DETECT,"[type dir]:[%u %u] res<pps bps>:"\
						"<%lu %lu> rbase:<%lu %lu>\n", e_ddos_stat_type_tcp, dir,
						result.rate[e_ddos_stat_type_tcp][dir].pps, result.rate[e_ddos_stat_type_tcp][dir].bps,
						(uint64_t)(rbase->rate[e_ddos_stat_type_tcp][dir].pps*DDOS_DETECT_RATIO), 
						(uint64_t)(rbase->rate[e_ddos_stat_type_tcp][dir].bps*DDOS_DETECT_RATIO));
			}
			/* syn */
			if ((g_ddos_conf.detect_type_mask & (1<<e_ddos_stat_type_tcp_syn)) && 
					(((result.rate[e_ddos_stat_type_tcp_syn][dir].pps > 
					   rbase->rate[e_ddos_stat_type_tcp_syn][dir].pps*DDOS_DETECT_RATIO))))
			{
				detect_result |= (1<<e_ddos_stat_type_tcp_syn);
				ddos_debug_lf(DDOS_MID_DETECT,"[type dir]:[%u %u] res<pps>:"\
						"<%lu> rbase:<%lu>\n", e_ddos_stat_type_tcp_syn, dir,
						result.rate[e_ddos_stat_type_tcp_syn][dir].pps,
						(uint64_t)(rbase->rate[e_ddos_stat_type_tcp_syn][dir].pps*DDOS_DETECT_RATIO));
			}
			/* ack */
			if ((g_ddos_conf.detect_type_mask & (1<<e_ddos_stat_type_tcp_ack)) && 
					(((result.rate[e_ddos_stat_type_tcp_ack][dir].pps > 
					   rbase->rate[e_ddos_stat_type_tcp_ack][dir].pps*DDOS_DETECT_RATIO))))
			{
				detect_result |= (1<<e_ddos_stat_type_tcp_ack);
				ddos_debug_lf(DDOS_MID_DETECT,"[type dir]:[%u %u] res<pps>:"\
						"<%lu> rbase:<%lu>\n", e_ddos_stat_type_tcp_ack, dir,
						result.rate[e_ddos_stat_type_tcp_ack][dir].pps,
						(uint64_t)(rbase->rate[e_ddos_stat_type_tcp_ack][dir].pps*DDOS_DETECT_RATIO));
			}
			/* >ack */
			for (i=e_ddos_stat_type_chargen; i<e_ddos_stat_type_max; i++)
			{
				/* 根据配置选择需要哪些指标来检测是否受到ddos攻击 */
				if ((g_ddos_conf.detect_type_mask & (1<<i)) && 
						(((g_ddos_conf.detect_unit_mask & DDOS_DET_MASK_BPS)&&
						 (result.rate[i][dir].bps > rbase->rate[i][dir].bps*DDOS_DETECT_RATIO)) || 
						 ((g_ddos_conf.detect_unit_mask & DDOS_DET_MASK_PPS) && 
						  (result.rate[i][dir].pps > rbase->rate[i][dir].pps*DDOS_DETECT_RATIO))))
				{
					detect_result |= (1<<i);
					ddos_debug_lf(DDOS_MID_DETECT,"[type dir]:[%u %u] res<pps bps>:"\
						"<%lu %lu> rbase:<%lu %lu>\n", i, dir,
						result.rate[i][dir].pps, result.rate[i][dir].bps,
						(uint64_t)(rbase->rate[i][dir].pps*DDOS_DETECT_RATIO), 
						(uint64_t)(rbase->rate[i][dir].bps*DDOS_DETECT_RATIO));
				}
			}

			/* 根据配置判断是否将新建连接数作为ddos攻击的指标 */
			if ((g_ddos_conf.detect_type_mask & DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_NEW) &&
					(result.tcp_new_conn_s[dir] > rbase->tcp_new_conn_s[dir]*DDOS_DETECT_RATIO))
			{
				detect_result |= DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_NEW;
			}

			/* 根据配置判断是否将并发连接数作为ddos攻击的指标 */
			if ((g_ddos_conf.detect_type_mask & DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_LIVE) &&
					(result.tcp_live_conn_s[dir] > rbase->tcp_live_conn_s[dir]*DDOS_DETECT_RATIO))
			{
				detect_result |= DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_LIVE;
			}
		}
	}

	if (detect_result)
	{
		node->flag |= DDOS_STATE_ATTACK;
		node->attack_type |= detect_result;
		node->finish_times = 0;
		ddos_debug_lf(DDOS_MID_DETECT, "this user(0x%0x) is being attacked now, attack "
				"type code is 0x%0x\n", node->ip, detect_result);
	}
	else if(node->flag & DDOS_STATE_ATTACK)
	{
		node->finish_times++;
		if (node->finish_times >= 3)  /* 连续三次没有检测到被攻击时，断定攻击已经结束 */
		{
			ddos_debug_lf(DDOS_MID_DETECT, "user(0x%0x), attack over\n", node->ip);
			node->flag &= (~DDOS_STATE_ATTACK);
			node->finish_times = 0;
			node->attack_type = 0;
			/* tell log module that this attack is already end */
			output_full_attack(node->ip);
		}
	}
	if (rbase)
	{
		free(rbase);
	}
	//ddos_base_line_update(&tmp_node, time);
		
	return;
}

void ddos_detect(int32_t index)
{
	ddos_detect_node_t *dnode;
	struct hlist_node *pos, *n;
	struct hlist_head *head;
	uint8_t flag;
	struct hlist_head del_head;
	uint32_t ip;
	
	//ddos_debug(DDOS_MID_STAT, "detect thread working detect[%d].\n", index);

	read_lock(&g_ddos_detect_list[index].lock);
	head = &g_ddos_detect_list[index].head;
	flag = g_ddos_detect_list[index].change;
	hlist_for_each_entry_safe(dnode, pos, n, head, list)
	{
		ddos_detect_judge(dnode, flag);		
	}
	read_unlock(&g_ddos_detect_list[index].lock);

	/* 判断这个节点是否仍需要监测 */
	if (unlikely(DDOS_CONF_CHANGE_IPLIST == flag && !g_ddos_iptrie_conf_reload))
	{
		INIT_HLIST_HEAD(&del_head);
		
		write_lock(&g_ddos_detect_list[index].lock);
		head = &g_ddos_detect_list[index].head;
		hlist_for_each_entry_safe(dnode, pos, n, head, list)
		{
			/* 判断这个节点是否仍需要监测 */
			if (ddos_mnt_ip_lookup(dnode->stat_node->ip) != e_ddos_ip_type_monitor)
			{
				hlist_del(&dnode->list);
				hlist_add_head(&dnode->list, &del_head);
			}	
		}
		g_ddos_detect_list[index].change = DDOS_CONF_HANDLE_OVER;
		write_unlock(&g_ddos_detect_list[index].lock);

		hlist_for_each_entry_safe(dnode, pos, n, &del_head, list)
		{
			write_lock(&iphead[dnode->stat_node->hash].lock);
			/* free */
			ddos_debug(DDOS_MID_DETECT, " this node(0x%0x) need free\n", 
				dnode->stat_node->ip);
			ip = dnode->stat_node->ip;
			hlist_del(&dnode->stat_node->list);
			write_unlock(&iphead[dnode->stat_node->hash].lock);
			free(dnode->stat_node);
			
			/* 删除统计数 */
			ddos_log_remove_monitorip(ip);
			
			hlist_del(&dnode->list);
			free(dnode);
		}
	}
	
	return ;
}

void *ddos_statistic_detect_thread(void *data)
{
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
	uint32_t index;

	while (1)
	{
		/* 在一次检测中，wait thread 指定的表项检测完毕之后将进入等待，
		 * 直到wait thread为其指定新的表项索引 */
		if ( ((struct ddos_stat_empty_para *)data)->index == -1 )
		{
			nanosleep(&sleeptime, NULL);
			continue;
		}
		/* 计算该线程当前时间需要处理的detect_list中的表项索引 */
		index =(((struct ddos_stat_empty_para *)data)->tid * 
				DDOS_STATISTIC_DETECT_LIST_MAX) + 
			((struct ddos_stat_empty_para *)data)->index ; 
		ddos_detect(index);

		((struct ddos_stat_empty_para *)data)->index = -1;
	}

	return NULL;
}

/* 
 * 根据线程数和检测周期分配index，该index决定将node添加到detect_list的位置；
 * 该函数可以实现将index均匀分配给每个线程负责的detect_list中的位置。
 */
static inline uint32_t ddos_get_detect_index(void)
{
	int curr = ddos_fetch_and_add(&ddos_statistic_index_current) % 
		(DDOS_STATISTIC_DETECT_LIST_MAX * DDOS_DETECT_THREAD_NUM);

	return (DDOS_STATISTIC_DETECT_LIST_MAX*(curr%DDOS_DETECT_THREAD_NUM)+
			(curr/DDOS_DETECT_THREAD_NUM));
}

/* add detect_node to detect_list */
int ddos_add_node_to_detect_list(total_data_list *node)
{
	int curr;
	ddos_detect_node_t *dnode;
	
	curr = ddos_get_detect_index();

	ddos_debug(DDOS_MID_STAT, "ip:0x%0x add to detect list[%d].\n", node->ip, curr);

	dnode = malloc(sizeof(*dnode));
	if (!dnode)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "no memory.\n");
		return -1;
	}
	
	dnode->free_func = free;
	dnode->stat_node = node;
	dnode->last_detect_time = g_ddos_time_jiffies-g_ddos_conf.interval;
	
	node->flag |= DDOS_STATE_ADD_DETECT;
	
	write_lock(&g_ddos_detect_list[curr].lock);
    hlist_add_head(&dnode->list, &g_ddos_detect_list[curr].head);
	write_unlock(&g_ddos_detect_list[curr].lock);
	
	return 0;
}

/* TCP连接数统计 */
uint32_t ddos_statistic_tcp_connect(session_item_t *si, uint32_t flag)
{
	uint32_t iphash;
	uint32_t monitorip;
	struct hlist_head *head;
	total_data_list *node, *nnode;
	struct hlist_node *pos;
	struct hlist_node *n;
	
	monitorip = ddos_get_mnt_ip(si);
	iphash = ddos_get_ip_hash(monitorip, DDOS_COUNT_IPTABLES_MAX);

	ddos_debug_lf(DDOS_MID_FLOW, "ip:0x%0x tcp connect update :%d\n", monitorip, flag);

	read_lock(&iphead[iphash].lock);
	head = &iphead[iphash].head;
	/* 根据哈希值查找当前ip是否在表中 */
	hlist_for_each_entry_safe(node, pos, n, head, list)
	{
		if (node->ip == monitorip)
		{
			if (DDOS_CONNECT_NEW == flag)
				node->tcp.conn_new[si->tcp_conn_dir]++;
			else
				node->tcp.conn_close[si->tcp_conn_dir]++;

			read_unlock(&iphead[iphash].lock);
			return UTAF_OK;
		}
	}
	read_unlock(&iphead[iphash].lock);
	
	/* need new */

	/* 如果找不到当前ip，新建一个node */
	nnode = (total_data_list*)malloc(sizeof(total_data_list));
	if (!nnode)
	{
		return UTAF_FAIL;
	}
	memset(nnode, 0, sizeof(total_data_list));

	nnode->ip = monitorip;
	nnode->hash = iphash;
	if (DDOS_CONNECT_NEW == flag)
		nnode->tcp.conn_new[si->tcp_conn_dir] = 1;
 	else
 		nnode->tcp.conn_close[si->tcp_conn_dir] = 1;

	write_lock(&iphead[iphash].lock);
	head = &iphead[iphash].head;
	hlist_for_each_entry_safe(node, pos, n, head, list)
	{
		if (node->ip == monitorip)
		{	
			free(nnode);
			write_unlock(&iphead[iphash].lock);
			return UTAF_OK;
		}
	}
	
	/* add to detect list */
	if (unlikely(ddos_add_node_to_detect_list(nnode)))
	{
		free(nnode);
		write_unlock(&iphead[iphash].lock);
		return UTAF_FAIL;
	}
	
    hlist_add_head(&nnode->list, head);
	write_unlock(&iphead[iphash].lock);

	return UTAF_OK;
}

uint32_t ddos_statistic(session_item_t *si)
{
	uint32_t iphash;
	uint32_t monitorip;
	struct hlist_head *head;
	total_data_list *node, *nnode, tnode;
	struct hlist_node *pos;
	struct hlist_node *n;
	uint32_t need_handle = 0;

	monitorip = ddos_get_mnt_ip(si);
	iphash = ddos_get_ip_hash(monitorip, DDOS_COUNT_IPTABLES_MAX);

	ddos_debug(DDOS_MID_STAT, "ddos statistic entry, ip:0x%0x\n", monitorip);

	read_lock(&iphead[iphash].lock);
	head = &iphead[iphash].head;
	hlist_for_each_entry_safe(node, pos, n, head, list)
	{
		if (node->ip == monitorip)
		{	
			ddos_data_count(si, &node->stTotalData);	
			
			if (node->flag & DDOS_STATE_ATTACK)
			{
				tnode = *node;
				need_handle = 1;
			}
			break;
		}
	}
	read_unlock(&iphead[iphash].lock);
	
	if(need_handle)
	{
	if (tnode.tcp.conn_new[dir_in] < tnode.tcp.conn_close[dir_in])
	{
		tnode.tcp.conn_close[dir_in]= tnode.tcp.conn_new[dir_in];
	}
	if (tnode.tcp.conn_new[dir_out] < tnode.tcp.conn_close[dir_out])
	{
		tnode.tcp.conn_close[dir_out]=tnode.tcp.conn_new[dir_out];
	}
		/* add source ip information to log tree */
		if(FAIL == utaf_ddos_log_insert(si, &tnode)){
			CA_LOG(LOG_MODULE, LOG_PROC, "log insert error in func ddos_statistic!\n"); 
		}
		ddos_debug(DDOS_MID_STAT, "0x%0x attacked. record source ip.\n", tnode.ip);
		
		return UTAF_OK;
	}

	/* need new */
	
	/* 判断这个IP是否仍需要监测 */
	if (ddos_mnt_ip_lookup(ntohl(monitorip)) != e_ddos_ip_type_monitor)
	{
		si->ddos_flag = UTAF_DO_NOT_DDOS;
		ddos_debug(DDOS_MID_STAT, "The user(0x%0x) needn't detect\n", monitorip);
		return UTAF_OK;
	}
	nnode = (total_data_list*)malloc(sizeof(total_data_list));
	if (!nnode)
	{
		return UTAF_FAIL;
	}
	memset(nnode, 0, sizeof(total_data_list));

	nnode->ip = monitorip;
	nnode->hash = iphash;
	ddos_data_count(si, &nnode->stTotalData);
		
	write_lock(&iphead[iphash].lock);
	head = &iphead[iphash].head;
	hlist_for_each_entry_safe(node, pos, n, head, list)
	{
		if (node->ip == monitorip)
		{	
			free(nnode);
			write_unlock(&iphead[iphash].lock);
			return UTAF_OK;
		}
	}
	
	/* add to detect list */
	if (unlikely(ddos_add_node_to_detect_list(nnode)))
	{
		free(nnode);
		write_unlock(&iphead[iphash].lock);
		return UTAF_FAIL;
	}
	
    hlist_add_head(&nnode->list, head);
	write_unlock(&iphead[iphash].lock);
	
	return UTAF_OK;
}

int32_t ddos_statistic_thread_crt(void)
{
    pthread_t tid;
	int i;

	/* create wait thread */
    if ( pthread_create(&tid, NULL, ddos_statistic_wait_thread, NULL) != 0 )
    {
        CA_LOG(LOG_MODULE, LOG_PROC, "pthread_create() failed. - %s\n", strerror(errno));
        return UTAF_FAIL;
    }
    if ( pthread_detach(tid) != 0 )
    {
        CA_LOG(LOG_MODULE, LOG_PROC, "pthread_detach(%lu) failed. - %s\n", 
				tid, strerror(errno));
		return UTAF_FAIL;
    }
	CA_LOG(LOG_MODULE, LOG_PROC,"ddos statistic wait thread create successfull\n");

	/* create detect thread */
	for (i=0; i<DDOS_DETECT_THREAD_NUM; i++)
	{
		g_ddos_stat_detect_thread_para[i].index = -1;
		g_ddos_stat_detect_thread_para[i].tid  = i;

		if ( pthread_create(&tid, NULL, ddos_statistic_detect_thread, 
					&g_ddos_stat_detect_thread_para[i]) != 0 )
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "pthread_create() failed. - %s\n", 
					strerror(errno));
			return UTAF_FAIL;
		}
		if ( pthread_detach(tid) != 0 )
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "pthread_detach(%lu) failed. - %s\n", 
					tid, strerror(errno));
			return UTAF_FAIL;
		}
		CA_LOG(LOG_MODULE, LOG_PROC,"ddos statistic detect thread[%d] create successfull\n", 
				i);
	}

	return UTAF_OK;
}

void ddos_set_iplist_change(void)
{
	uint32_t index;

	for (index = 0; index < (DDOS_STATISTIC_DETECT_LIST_MAX*
				DDOS_DETECT_THREAD_NUM); index++)
	{
		write_lock(&g_ddos_detect_list[index].lock);
		g_ddos_detect_list[index].change = DDOS_CONF_CHANGE_IPLIST;
		write_unlock(&g_ddos_detect_list[index].lock);
	}

	return ;
}

/* 初始化哈希链表 */
int32_t ddos_iplist_hash_init(void)
{
	uint32_t index;
	ddos_detect_head_t *dlist;
	struct ddos_stat_empty_para *para;

	for(index = 0; index < DDOS_COUNT_IPTABLES_MAX; index++)
	{
		INIT_HLIST_HEAD(&iphead[index].head);
		rwlock_init(&(iphead[index].lock));
	}

	/* 00 01 02 ... 58 59 60 61 62 ... 119 120 ... cycle*thread num   */
	/* | detect thread 0 | detect thread 1|...| detect thread (num-1) */

	dlist = (ddos_detect_head_t *)malloc(sizeof(ddos_detect_head_t)*
			DDOS_STATISTIC_DETECT_LIST_MAX*DDOS_DETECT_THREAD_NUM);
	if (!dlist)
	{
		return UTAF_FAIL;
	}
	g_ddos_detect_list = dlist;

	for (index = 0; index < (DDOS_STATISTIC_DETECT_LIST_MAX*
				DDOS_DETECT_THREAD_NUM); index++)
	{
		INIT_HLIST_HEAD(&g_ddos_detect_list[index].head);
		rwlock_init(&(g_ddos_detect_list[index].lock));
		g_ddos_detect_list[index].change = DDOS_CONF_NOT_CHANGE;
	}

	para = malloc(sizeof(*para)*DDOS_DETECT_THREAD_NUM);
	if(!para)
	{
		free(g_ddos_detect_list);
		g_ddos_detect_list = NULL;
		return UTAF_FAIL;
	}
	memset(para, 0x0, sizeof(*para)*DDOS_DETECT_THREAD_NUM);
	g_ddos_stat_detect_thread_para = para;

	return UTAF_OK;
}

void ddos_statistic_fini(void)
{
	if (g_ddos_stat_detect_thread_para)
	{
		free(g_ddos_stat_detect_thread_para);
		g_ddos_stat_detect_thread_para = NULL;
	}

	if (g_ddos_detect_list)
	{
		free(g_ddos_detect_list);
		g_ddos_detect_list = NULL;
	}
}

int32_t ddos_statistic_init(void)
{

	if(ddos_iplist_hash_init())
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos statistic hash init faild .\n");
		goto faild;
	}

	if (ddos_statistic_thread_crt())
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos statistic thread creat faild.\n");
		return UTAF_FAIL;
	}

	return UTAF_OK;

faild:
	ddos_statistic_fini();
	return UTAF_FAIL;
}

void ddos_statistic_uninit(void)
{
	int i;
	total_data_list *node;
	ddos_detect_node_t *dnode;
	struct hlist_node *pos, *n;

	for(i = 0; i < DDOS_COUNT_IPTABLES_MAX; i++)
	{
		write_lock(&iphead[i].lock);
		hlist_for_each_entry_safe(node, pos, n, &iphead[i].head, list){
			hlist_del(&node->list);
			free(node);
		}
		write_unlock(&iphead[i].lock);
	}

	for (i = 0; i < (DDOS_STATISTIC_DETECT_LIST_MAX*
				DDOS_DETECT_THREAD_NUM); i++)
	{
		write_lock(&g_ddos_detect_list[i].lock);
		hlist_for_each_entry_safe(dnode, pos, n, &g_ddos_detect_list[i].head, list){
			hlist_del(&dnode->list);
			free(dnode);
		}
		write_unlock(&g_ddos_detect_list[i].lock);
	}
	
	return ;
}

