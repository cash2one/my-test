#include <errno.h>
#include <stdint.h>
#include "mbuf.h"
#include "utaf_session.h"
#include "misc.h"
#include "common.h"
#include "ddos_log_out.h"
#include "monitor_ip_list.h"
#include "ddos.h"
#include "utaf_ddos_count.h"
#include "base_line.h"
#include "ddos_make_log.h"

#define ONE_HOUR_TIME   3600
#define DETECT_VALUSE	2

static volatile int _day_flag = 0;
extern volatile time_t g_ddos_time_jiffies;

#define utaf_fetch_and_dec(x) __sync_fetch_and_sub ((volatile int *)(x), (int)1)

#define  DDOS_BASELINE_IPLIST_DIR		DDOS_CONFIG_DIR"/baseline.conf"

static int last_week_index ;
static int cur_week_index;

static int cur_day_index;
static int last_day_index;

static int last_hour_index;
static int cur_hour_index;
static int tmp_hour_index;

int val_if_null(ddos_stat_norm_t *ret_base);
mnt_ip_head_t base_hash_buf[MNT_LIST_HASH_MAX];
static struct hlist_head ddos_base_timeout;
rwlock_t ddos_base_lock;

static __inline__ void utaf_atomic_set(int i, volatile int *target)
{
	__asm__ __volatile__(
			"lock; xchgl %1,%0"
			:"=m" (*target)
			:"ir" (i), "m" (*target));
}

void add_to_timerinit(void)
{
	time_t now_time;
	struct tm this_time;
	memset(&this_time, 0, sizeof(struct tm));
	time(&now_time);
	localtime_r(&now_time,&this_time);
	//int ret = g_ddos_time_jiffies%ONE_HOUR_TIME;
	cur_hour_index = this_time.tm_hour;
	if (cur_hour_index != tmp_hour_index)
	//if( ret == 0 )
	{
		last_hour_index=tmp_hour_index;
		last_day_index  = cur_day_index;
		last_week_index = cur_week_index;
		tmp_hour_index=cur_hour_index;
		//cur_hour_index = (cur_hour_index + 1)%g_ddos_conf.hour_num;

		//cur_hour_index = this_time.tm_hour;

#if 0
		//cur_day_index = ( cur_day_index + 1 )%g_ddos_conf.day_num;
		ret = this_time.tm_wday - 1;
		if (ret == -1)
		{
			cur_day_index = 6;
		}
		else{
			cur_day_index = this_time.tm_wday;
		}
#endif
		cur_day_index = this_time.tm_wday;
		if ( cur_day_index == 0 )
		{
			cur_week_index = ( cur_week_index +1 )%g_ddos_conf.week_num;
		}
		utaf_atomic_set(1,&_day_flag);
			
	}	

}

/* 读取一行基线数据，存入结构体 */
int ddos_gbaseline_data(char **base, base_line_hour *basehour, int *len)
{
	char *ptr = NULL;
	char *pcData = *base;
	int i = 0;
	int j = 0;

	if (NULL == *base || len <= 0)
	{
		return -1;
	}

	basehour->tcp_new_conn_s[dir_out] = strtoul(pcData, &ptr, 10);
	pcData = ptr + 1;
	basehour->tcp_new_conn_s[dir_in] = strtoul(pcData, &ptr, 10);
	pcData = ptr + 1;
	basehour->tcp_live_conn_s[dir_out] = strtoul(pcData, &ptr, 10);
	pcData = ptr + 1;
	basehour->tcp_live_conn_s[dir_in] = strtoul(pcData, &ptr, 10);
	pcData = ptr + 1;

	for ( i = 0; i < e_ddos_stat_type_max; i++)
	{
		for (j = 0; j < flow_dir ; j++)
		{
			basehour->rate[i][j].pps = strtoul(pcData, &ptr, 10);
			pcData = ptr + 1;
			basehour->rate[i][j].bps = strtoul(pcData, &ptr, 10);
			pcData = ptr + 1;
		}
	}
	spinlock_init(&(basehour->base_lock));
	basehour->time = 0;
	basehour->enable_flag = 1;

	*len -= (int)(pcData - *base);
	*base = pcData;

	return 0;
}

/* 程序启动后从文件中读取基线值 */
int ddos_read_baseline(char *one_line)
{
	base_sev_week *pstbase = NULL;
	char *ptr = NULL;
	char *pcData = NULL;
	unsigned int hash = 0;
	int len = 0;
	int ret = 0;
	int i = 0;
	int j = 0;
	int z = 0;

	pstbase = (base_sev_week*)malloc(sizeof(base_sev_week));
	if(pstbase == NULL)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "baseline malloc error.\n");
		return -1;
	}

	len = strlen(one_line);

	pstbase->ip = strtol(one_line, &ptr, 10);
	hash = pstbase->ip % MNT_LIST_HASH_MAX;
	pcData = ptr + 1;
	len -= (int)(pcData - one_line);

	for (i = 0; i < g_ddos_conf.week_num; i++)
	{
		for (j = 0; j < g_ddos_conf.day_num; j++)
		{
			for (z = 0; z < g_ddos_conf.hour_num; z++)
			{
				ret = ddos_gbaseline_data(&pcData, &pstbase->w[i].d[j].h[z], &len);
				if (-1 == ret)
				{
					CA_LOG(LOG_MODULE, LOG_PROC, "baseline get hour data error.\n");
					free(pstbase);
					return -1;
				}
			}
		}
	}

	if (*(pcData - 1) != '\n')
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "baseline get file data error.\n");
		free(pstbase);
		return -1;
	}

	write_lock(&base_hash_buf[hash].lock);
	hlist_add_head(&pstbase->list, &base_hash_buf[hash].head);
	write_unlock(&base_hash_buf[hash].lock);
	
	write_lock(&ddos_base_lock);
	hlist_add_head(&pstbase->base_list,&ddos_base_timeout); 
	write_unlock(&ddos_base_lock);	

	return 0;
}

/* 程序启动后从文件中读取基线值 */
int ddos_get_baseline_from_file(void)
{
	char *oneline = NULL;
	int onelen = 0;
	char *file_path;
	FILE *pFile;

	file_path = DDOS_BASELINE_IPLIST_DIR;
	
	pFile = fopen(file_path, "r");
	if(pFile == NULL)
	{
		return 0;
	}

	if(feof(pFile))
	{
		fclose(pFile);
		return 0;
	}
	
	printf("begin to load ddos baseline file data.\n");

	onelen = sizeof(base_sev_week) * 2;

	oneline = malloc(onelen);
	if (NULL == oneline)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "baseline malloc error.\n");
		fclose(pFile);
		return -1;
	}

	while(!feof(pFile))
	{
		if(!fgets(oneline, onelen, pFile))
		{
			break;
		}
		
		if (-1 == ddos_read_baseline(oneline))
		{
			fclose(pFile);
			free(oneline);
			return -1;
		}
	}

	free(oneline);
	fclose(pFile);

	printf("load ddos baseline file data ok.\n");
	
	return 0;
}

base_sev_week *find_ip_base(unsigned int ip)
{
	//int hash = ip%MNT_LIST_HASH_MAX;
	base_sev_week *node;
	struct hlist_node *pos, *n;
	unsigned int hash = ip%MNT_LIST_HASH_MAX;
	mnt_ip_head_t *head = &base_hash_buf[hash];

	read_lock(&head->lock);
	hlist_for_each_entry_safe(node, pos, n, &head->head, list){
		if (node->ip == ip) {
			read_unlock(&head->lock);
			return node;
		}
	}
	read_unlock(&head->lock);
	return NULL;
}

int base_ip_add(total_data_list *node_val,time_t timeval )
{
	int i;
	int j;
	unsigned int hash = (node_val->ip)%MNT_LIST_HASH_MAX;
	base_sev_week *node;
	base_line_hour *basehour = NULL;
	
	node = (base_sev_week *)malloc(sizeof(base_sev_week));
	if (!node) 
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "baseline malloc error.\n");
		return -1;
	}
	memset(node,0,sizeof(base_sev_week));
	/** 为新增节点赋值 **/
	node ->ip = node_val->ip;

	basehour = &node->w[cur_week_index].d[cur_day_index].h[cur_hour_index]; 

	basehour->enable_flag = 1;	
	basehour->time   = timeval ;
	basehour->tcp_new_conn_s[dir_in] = node_val->tcp.conn_new[dir_in] - node_val->tcp.conn_new_old[dir_in];
	basehour->tcp_live_conn_s[dir_in] = node_val->tcp.conn_new[dir_in] - node_val->tcp.conn_close[dir_in];
	basehour->tcp_new_conn_s[dir_out] = node_val->tcp.conn_new[dir_out] - node_val->tcp.conn_new_old[dir_out];
	basehour->tcp_live_conn_s[dir_out] = node_val->tcp.conn_new[dir_out] - node_val->tcp.conn_close[dir_out];

	for ( i = 0;i< e_ddos_stat_type_max ;i++ )
	{
		for (j = 0;j< flow_dir;j++)
		{
			basehour->rate[i][j].pps = node_val->stTotalData.c[i][j].pkts;
			basehour->rate[i][j].bps = node_val->stTotalData.c[i][j].bytes;
		}
	}
	
	write_lock(&base_hash_buf[hash].lock);
	hlist_add_head(&node->list, &base_hash_buf[hash].head);
	write_unlock(&base_hash_buf[hash].lock);

	/*** link to ddos_base_timeout**/
	write_lock(&ddos_base_lock);
	hlist_add_head(&node->base_list,&ddos_base_timeout);	
	write_unlock(&ddos_base_lock);

	return 0;
}

uint64_t base_total_av(uint64_t base_val,uint64_t new_base)
{
	uint64_t ret_result;
	if (base_val == 0)
	{
		ret_result = new_base;
	}
	else if(new_base == 0)
	{
		ret_result = base_val;
	}
	else if (base_val > DETECT_VALUSE*new_base )
	{
		ret_result = new_base;
	}
	else if (new_base > DETECT_VALUSE*base_val )
	{
		ret_result = base_val;
	}
	else
	{
		ret_result = (uint64_t)(base_val + new_base)/2;
	}
	return ret_result;
}

ddos_stat_norm_t *ddos_week_get_base(base_sev_week *node,ddos_stat_norm_t *ret_base)
{
	int i,j;
	int num = 0;
	int z = 0;
	base_line_hour *basehour = NULL;
//	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 };

	memset(ret_base,0,sizeof(ddos_stat_norm_t));

	for ( i = 0;i < g_ddos_conf.week_num  ;i++)
	{
		if ( i != cur_week_index )
		{
			basehour = &node->w[i].d[cur_day_index].h[cur_hour_index];

			if ( basehour->enable_flag != 0 )
			{
#if 0
				while(1)
				{
					spinlock_lock(&basehour->base_lock);
					if (basehour->time != 0 )
					{
						spinlock_unlock(&node->w[i].d[last_day_index].h[cur_hour_index].base_lock);
						nanosleep(&sleeptime, NULL);
						continue;
					}
					break;
				}
#endif
					if (basehour->time != 0 )
					{
						continue;
					}
				
					spinlock_lock(&basehour->base_lock);
				num++;
				ret_base->tcp_new_conn_s[dir_in] = 
					base_total_av( ret_base->tcp_new_conn_s[dir_in],basehour->tcp_new_conn_s[dir_in]);  
				ret_base->tcp_live_conn_s[dir_in] = 
					base_total_av( ret_base->tcp_live_conn_s[dir_in],basehour->tcp_live_conn_s[dir_in]);  
				ret_base->tcp_new_conn_s[dir_out] = 
					base_total_av( ret_base->tcp_new_conn_s[dir_out],basehour->tcp_new_conn_s[dir_out]);  
				ret_base->tcp_live_conn_s[dir_out] = 
					base_total_av( ret_base->tcp_live_conn_s[dir_out],basehour->tcp_live_conn_s[dir_out]);  

				for ( j = 0;j< e_ddos_stat_type_max;j++)
				{
					for ( z = 0;z < flow_dir;z++)
					{
						ret_base->rate[j][z].pps = base_total_av(ret_base->rate[j][z].pps, basehour->rate[j][z].pps); 
						ret_base->rate[j][z].bps = base_total_av(ret_base->rate[j][z].bps,basehour->rate[j][z].bps); 
					}
				}
				spinlock_unlock(&basehour->base_lock);
			}
		}
	}

	if (num != 0)
	{
		if(val_if_null(ret_base))
		{
		return ret_base;
			
		}
		return NULL;
	}
	else
	{
		return NULL;
	}
}

ddos_stat_norm_t *ddos_day_get_base(base_sev_week *node,ddos_stat_norm_t *ret_base)
{
	int i,j;
	int num = 0;
	int z = 0;
	base_line_hour *basehour = NULL;
//	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 };

	memset(ret_base,0,sizeof(ddos_stat_norm_t));

	for ( i = 0;i < g_ddos_conf.day_num  ;i++)
	{
		if ( i != cur_day_index )
		{
			basehour = &node->w[cur_week_index].d[i].h[cur_hour_index];

			if ( basehour->enable_flag != 0 )
			{
#if 0
				while(1){
					spinlock_lock(&basehour->base_lock);
					if (basehour->time != 0 )
					{
						spinlock_unlock(&basehour->base_lock);
						nanosleep(&sleeptime, NULL);
						continue;
					}
					break;
				}
#endif
					if (basehour->time != 0 )
					{
						continue;
					}
				
				num++;
					spinlock_lock(&basehour->base_lock);
				ret_base->tcp_new_conn_s[dir_in] = 
					base_total_av(ret_base->tcp_new_conn_s[dir_in],basehour->tcp_new_conn_s[dir_in]);  
				ret_base->tcp_live_conn_s[dir_in] = 
					base_total_av(ret_base->tcp_live_conn_s[dir_in],basehour->tcp_live_conn_s[dir_in]);  
				ret_base->tcp_new_conn_s[dir_out] = 
					base_total_av(ret_base->tcp_new_conn_s[dir_out],basehour->tcp_new_conn_s[dir_out]);  
				ret_base->tcp_live_conn_s[dir_out] = 
					base_total_av(ret_base->tcp_live_conn_s[dir_out],basehour->tcp_live_conn_s[dir_out]);  

				for ( j = 0;j< e_ddos_stat_type_max;j++)
				{
					for ( z = 0;z < flow_dir;z++)
					{
						ret_base->rate[j][z].pps = base_total_av(ret_base->rate[j][z].pps,basehour->rate[j][z].pps); 
						ret_base->rate[j][z].bps = base_total_av(ret_base->rate[j][z].bps,basehour->rate[j][z].bps); 
					}
				}
				spinlock_unlock(&basehour->base_lock);
			}
		}
	}

	if (num != 0)
	{
		if(val_if_null(ret_base))
		{
		return ret_base;
			
		}
		return NULL;
	}
	else
	{
		return NULL;
	}
}

ddos_stat_norm_t *ddos_hour_get_base(base_sev_week *node,ddos_stat_norm_t *ret_base)
{
	int i,j;
	int z = 0;
	int w_n = cur_week_index;
	int d_n = cur_day_index;
	int h_n = cur_hour_index;
	base_line_hour *basehour = NULL;

	//struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 };

	memset(ret_base,0,sizeof(ddos_stat_norm_t));

	for ( i = 0;i < g_ddos_conf.day_num  ;i++)
	{
		basehour = &node->w[w_n].d[d_n].h[h_n];

		if ( basehour->enable_flag != 0 )
		{
			if ((w_n == cur_week_index) && (d_n == cur_day_index) && (h_n == cur_hour_index))
			{
				goto hour_tol;
#if 0
				spinlock_lock(&basehour->base_lock);
				if (basehour->time != 0 )
				{
					ret_base->tcp_new_conn_s[dir_in] = basehour->tcp_new_conn_s[dir_in]/basehour->time;  
					ret_base->tcp_live_conn_s[dir_in] = basehour->tcp_live_conn_s[dir_in]/basehour->time;  
					ret_base->tcp_new_conn_s[dir_out] = basehour->tcp_new_conn_s[dir_out]/basehour->time;  
					ret_base->tcp_live_conn_s[dir_out] = basehour->tcp_live_conn_s[dir_out]/basehour->time;  
					for ( j = 0;j< e_ddos_stat_type_max;j++)
					{
						for ( z = 0;z < flow_dir;z++)
						{
							ret_base->rate[j][z].pps = basehour->rate[j][z].pps/basehour->time; 
							ret_base->rate[j][z].bps = basehour->rate[j][z].bps/basehour->time; 
						}
					}
				}
				spinlock_unlock(&basehour->base_lock);

				return ret_base;
#endif
			}
			else
			{
#if 0
				while(1)
				{
					spinlock_lock(&basehour->base_lock);
					if (basehour->time != 0 )
					{
						spinlock_unlock(&basehour->base_lock);
						nanosleep(&sleeptime, NULL);
						continue;
					}
					break;
				}
#endif
					if (basehour->time != 0 )
					{
						continue;
					}
					spinlock_lock(&basehour->base_lock);
				ret_base->tcp_new_conn_s[dir_in] =  base_total_av(ret_base->tcp_new_conn_s[dir_in],basehour->tcp_new_conn_s[dir_in]);  
				ret_base->tcp_live_conn_s[dir_in] = base_total_av(ret_base->tcp_live_conn_s[dir_in],basehour->tcp_live_conn_s[dir_in]);  
				ret_base->tcp_new_conn_s[dir_out] = base_total_av(ret_base->tcp_new_conn_s[dir_out],basehour->tcp_new_conn_s[dir_out]);  
				ret_base->tcp_live_conn_s[dir_out] = base_total_av(ret_base->tcp_live_conn_s[dir_out],basehour->tcp_live_conn_s[dir_out]);  
				for ( j = 0;j< e_ddos_stat_type_max;j++)
				{
					for ( z = 0;z < flow_dir;z++)
					{
						ret_base->rate[j][z].pps = base_total_av(ret_base->rate[j][z].pps,basehour->rate[j][z].pps); 
						ret_base->rate[j][z].bps = base_total_av(ret_base->rate[j][z].bps,basehour->rate[j][z].bps); 
					}
				}
				spinlock_unlock(&basehour->base_lock);

				return ret_base;
			}
		}
hour_tol:
		h_n = (h_n - 1)%g_ddos_conf.hour_num;
		if (h_n == -1)
		{
			h_n = 23;
		d_n = (d_n - 1)%g_ddos_conf.day_num;
		if(d_n == -1)
		{
			w_n = (w_n -1 )%g_ddos_conf.week_num;
			if (w_n == -1)
			{
				w_n = 6;
			}
			d_n = 6;
		}			
		}
	}
		if(val_if_null(ret_base))
		{
		return ret_base;
			
		}
		return NULL;
	
}

ddos_stat_norm_t *off_base_valuse(ddos_stat_norm_t *ret_base)
{
	int j;

	/* 第二版需要区分连接方向 */
	ret_base->tcp_new_conn_s[dir_in]   = (uint64_t)g_ddos_conf.threshold_new_tcp_in/
		g_ddos_conf.detect_ratio;  
	ret_base->tcp_new_conn_s[dir_out]  = (uint64_t)g_ddos_conf.threshold_new_tcp_out/
		g_ddos_conf.detect_ratio;  
	ret_base->tcp_live_conn_s[dir_in]  = (uint64_t)g_ddos_conf.threshold_live_tcp_in/
		g_ddos_conf.detect_ratio;  
	ret_base->tcp_live_conn_s[dir_out] = (uint64_t)g_ddos_conf.threshold_live_tcp_out/
		g_ddos_conf.detect_ratio;  

	/* tcp 流量 */
	ret_base->rate[e_ddos_stat_type_tcp][dir_in].pps = ~0; 
	ret_base->rate[e_ddos_stat_type_tcp][dir_out].pps = ~0; 
	ret_base->rate[e_ddos_stat_type_tcp][dir_in].bps = 
		(uint64_t)(g_ddos_conf.threshold_flow_in*g_ddos_conf.flow_ratio)/g_ddos_conf.detect_ratio; 
	ret_base->rate[e_ddos_stat_type_tcp][dir_out].bps = 
		(uint64_t)(g_ddos_conf.threshold_flow_out*g_ddos_conf.flow_ratio)/g_ddos_conf.detect_ratio; 

	/* syn */
	ret_base->rate[e_ddos_stat_type_tcp_syn][dir_in].pps = (uint64_t)g_ddos_conf.threshold_new_tcp_in/
		g_ddos_conf.detect_ratio; 
	ret_base->rate[e_ddos_stat_type_tcp_syn][dir_out].pps = (uint64_t)g_ddos_conf.threshold_new_tcp_in/
		g_ddos_conf.detect_ratio; 
	ret_base->rate[e_ddos_stat_type_tcp_syn][dir_in].bps = ~0;
	ret_base->rate[e_ddos_stat_type_tcp_syn][dir_out].bps = ~0;
	/* ack */
	ret_base->rate[e_ddos_stat_type_tcp_ack][dir_in].pps = ~0;
	ret_base->rate[e_ddos_stat_type_tcp_ack][dir_out].pps = ~0;
	ret_base->rate[e_ddos_stat_type_tcp_ack][dir_in].bps = ~0;
	ret_base->rate[e_ddos_stat_type_tcp_ack][dir_out].bps = ~0;

	for ( j = e_ddos_stat_type_chargen; j < e_ddos_stat_type_max; j++)
	{
		ret_base->rate[j][dir_in].pps = ~0; 
		ret_base->rate[j][dir_out].pps = ~0; 
		ret_base->rate[j][dir_in].bps = 
			(uint64_t)(g_ddos_conf.threshold_flow_in*g_ddos_conf.flow_ratio)/g_ddos_conf.detect_ratio; 
		ret_base->rate[j][dir_out].bps = 
			(uint64_t)(g_ddos_conf.threshold_flow_out*g_ddos_conf.flow_ratio)/g_ddos_conf.detect_ratio; 
	}

	return ret_base;
}
int val_if_null(ddos_stat_norm_t *ret_base)
{
	int i,j;
	for(i=0;i<e_ddos_stat_type_max;i++)
	{
		for(j=0;j<flow_dir;j++)
		{
			if (ret_base->rate[i][j].pps!=0)
			{
				ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base base is valuse return  .....\n");
				return 1;
			}
		}


	}
	return 0;

}
/*****
 * @pamar ip
 *
 * @return 
 * 			1)NULL
 * 			2)ddos_stat_norm_t
 * 	return valuse is malloc ,as nend free 
 * **/
ddos_stat_norm_t *ddos_base_line_get_result(uint32_t ip)
{
	//int i,j;
	base_sev_week *node = NULL;
	ddos_stat_norm_t *ret_base = (ddos_stat_norm_t *)malloc(sizeof(ddos_stat_norm_t));
	
	if ( ret_base == NULL)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "baseline malloc error.\n");
		return NULL;
	}
	memset(ret_base,0,sizeof(ddos_stat_norm_t));

	/* 如果配置为自学习，计算基线值 */
	if ( g_ddos_conf.base_sw != 0)
	{
		ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u]\n",ip);
		node = find_ip_base(ip);
		if ( node )
		{
			ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] found [week].....\n",ip);
			if(!ddos_week_get_base(node,ret_base))
			{
				ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] week no base to found day .....\n",ip);
				if(!ddos_day_get_base(node,ret_base))
				{
					ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] day no base to found hour .....\n",ip);
					if(!ddos_hour_get_base(node,ret_base))
					{
						ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] hour no base return NULL .....\n",ip);
						free(ret_base);
						ret_base = NULL;
						return NULL;
					}
				}
			}
#if 0
			//ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] found [week] end.....\n",ip);
			for(i=0;i<e_ddos_stat_type_max;i++)
			{
				for(j=0;j<flow_dir;j++)
				{
					if (ret_base->rate[i][j].pps!=0)
					{
						ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] base is valuse return  .....\n",ip);
						return ret_base;
					}
				}


			}
#endif

		}
		ddos_debug_f(DDOS_MID_BASE_LINE,"find_ip_base [%u] base is valuse return  .....\n",ip);
		return ret_base;
	}
	else /* 否则读取配置的基线值 */
	{
		return off_base_valuse(ret_base);
	}

	return NULL;
}

void ddos_base_line_update(total_data_list * result, time_t timeint )
{
	base_sev_week *node_base = NULL;
	base_line_hour *basehour = NULL;
	int i = 0;
	int j = 0;

	node_base = find_ip_base(result->ip);
	if ( NULL == node_base ) /* 如果此ip没有基线值，新增一个基线节点 */
	{
	 ddos_debug_f(DDOS_MID_BASE_LINE, "no find ip base add ip :[%u] w:[%d] d:[%d] hour:[%d] time:[%lu]\n",result->ip,cur_week_index,cur_day_index,cur_hour_index,timeint);	
		base_ip_add(result,timeint);
	}
	else /* 如果已经有基线值，更新 */
	{
	 ddos_debug_f(DDOS_MID_BASE_LINE, "already find ip base add ip :[%u] w:[%d] d:[%d] hour:[%d] time:[%lu]\n",result->ip,cur_week_index,cur_day_index,cur_hour_index,timeint);	
		basehour = &node_base->w[cur_week_index].d[cur_day_index].h[cur_hour_index];

		if (basehour->time == 0)
		{
			basehour->tcp_new_conn_s[dir_in] = 0;	
			basehour->tcp_live_conn_s[dir_in] = 0;	
			basehour->tcp_new_conn_s[dir_out] = 0;	
			basehour->tcp_live_conn_s[dir_out] = 0;	
			for(i = 0;i < e_ddos_stat_type_max ;i++ )
			{
				for(j = 0;j < flow_dir;j++)
				{
					basehour->rate[i][j].pps = 0;	
					basehour->rate[i][j].bps = 0;	
				}
			}
		}
		basehour->enable_flag = 1;

		basehour->time += timeint;
		basehour->tcp_new_conn_s[dir_in] += (result->tcp.conn_new[dir_in] - result->tcp.conn_new_old[dir_in]);	
		basehour->tcp_live_conn_s[dir_in] += (result->tcp.conn_new[dir_in] - result->tcp.conn_close[dir_in]);	
		basehour->tcp_new_conn_s[dir_out] += (result->tcp.conn_new[dir_out] - result->tcp.conn_new_old[dir_out]);	
		basehour->tcp_live_conn_s[dir_out] += (result->tcp.conn_new[dir_out] - result->tcp.conn_close[dir_out]);	
		for(i = 0;i < e_ddos_stat_type_max ;i++ )
		{
			for(j = 0;j < flow_dir;j++)
			{
				basehour->rate[i][j].pps += result ->stTotalData.c[i][j].pkts;	
				basehour->rate[i][j].bps += (result ->stTotalData.c[i][j].bytes*8);	
			}
		}
	}
}

void ddos_update_baseline_data(FILE *fp, base_line_hour *basehour)
{
	int i = 0;
	int j = 0;

	if (basehour->time != 0)
	{
		memset(basehour, 0, sizeof(base_line_hour));
	}

	fprintf(fp,"|%lu",basehour->tcp_new_conn_s[dir_out]);
	fprintf(fp," %lu",basehour->tcp_new_conn_s[dir_in]);
	fprintf(fp," %lu",basehour->tcp_live_conn_s[dir_out]);
	fprintf(fp," %lu",basehour->tcp_live_conn_s[dir_in]);
	
	for ( i = 0;i < e_ddos_stat_type_max ;i++)
	{
		for (j = 0; j < flow_dir ; j++)
		{

			fprintf(fp," %lu",basehour->rate[i][j].pps);
			fprintf(fp," %lu",basehour->rate[i][j].bps);
		}
	}

	return;
}


/* 每个小时将每个ip基线值更新到文件中 */
int ddos_update_baseline_file(FILE *fp, base_sev_week *node)
{
	int i = 0;
	int j = 0;
	int z = 0;

	if (NULL == fp)
	{
		return -1;
	}

	fprintf(fp, "%u", node->ip);

	for (i = 0; i < g_ddos_conf.week_num; i++)
	{
		for (j = 0; j < g_ddos_conf.day_num; j++)
		{
			for (z = 0; z < g_ddos_conf.hour_num; z++)
			{
				ddos_update_baseline_data(fp, &node->w[i].d[j].h[z]);
			}
		}
	}

	fprintf(fp, "%c", '\n');

	return 0;
}

void *clear_base_wait(void *arg)
{
	base_sev_week *node;
	base_line_hour *basehour = NULL;
	FILE *fp = NULL;
	char *file_name = NULL;
	struct hlist_node *pos, *n;
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; 
	time_t timeval;
	int i,j;

	m_wmb();

	while(1)
	{
		if (utaf_fetch_and_dec(&_day_flag) == 1)
		{			
			file_name = DDOS_BASELINE_IPLIST_DIR;
			
			fp = fopen(file_name, "w");
			if (NULL == fp)
			{
				CA_LOG(LOG_MODULE, LOG_PROC, "open baseline file error.\n");
			}
			
			ddos_debug(DDOS_MID_BASE_LINE, "update baseline file..\n");
			ddos_debug_f(DDOS_MID_BASE_LINE, "update baseline file create..\n");
	 ddos_debug_f(DDOS_MID_BASE_LINE, "query base write to file  last w:[%d] last d:[%d] last hour:[%d]\n",last_week_index,last_day_index,last_hour_index);	

			read_lock(&ddos_base_lock);
			
			if(ddos_base_timeout.first == NULL)
			{
				ddos_debug_f(DDOS_MID_BASE_LINE, "into mem base timeout table is [NULL]\n");
			
			}
			hlist_for_each_entry_safe(node, pos, n,&ddos_base_timeout , base_list)
			{
				basehour = &node->w[last_week_index].d[last_day_index].h[last_hour_index];
				
				ddos_debug_f(DDOS_MID_BASE_LINE, "into mem base timeout table time:[%lu]\n",basehour->time);
				timeval = basehour->time;
				if(timeval == 0 )
				{
					continue;
				}
				spinlock_lock(&basehour->base_lock);
				
				basehour->tcp_new_conn_s[dir_in] = basehour->tcp_new_conn_s[dir_in]/timeval;
				basehour->tcp_live_conn_s[dir_in] = basehour->tcp_live_conn_s[dir_in]/timeval;	
				basehour->tcp_new_conn_s[dir_out] = basehour->tcp_new_conn_s[dir_out]/timeval;
				basehour->tcp_live_conn_s[dir_out] = basehour->tcp_live_conn_s[dir_out]/timeval;	

				for ( i = 0;i < e_ddos_stat_type_max ;i++)
				{
					for (j = 0; j < flow_dir ; j++)
					{
						basehour->rate[i][j].pps = basehour->rate[i][j].pps/timeval;
						basehour->rate[i][j].bps = basehour->rate[i][j].bps/timeval;
					}
				}
				
				basehour->time = 0;

				spinlock_unlock(&basehour->base_lock);
			ddos_debug_f(DDOS_MID_BASE_LINE, "will base write file wwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");	
				/* update baseline file */
				ddos_update_baseline_file(fp, node);
			}

			read_unlock(&ddos_base_lock);
			if (fp)
			{
				fclose(fp);
			}
		}
		else
		{
			nanosleep(&sleeptime, NULL);
		}
		m_wmb();
	}
	
	return NULL;
}

void base_pthread_base(void)
{
	pthread_t tid;

	if ( pthread_create(&tid, NULL, clear_base_wait, NULL) != 0 ) 
	{   
		CA_LOG(LOG_MODULE, LOG_PROC, "pthread_create() failed. - %s\n", strerror(errno));
		return;
	}   
	if ( pthread_detach(tid) != 0 ) 
	{   
		CA_LOG(LOG_MODULE, LOG_PROC, "pthread_detach(%lu) failed. - %s\n", strerror(errno));
	}

	return;
}

void base_hash_init(void)
{
	int i;
	time_t now_time;
	struct tm this_time;
	for ( i=0 ;i< MNT_LIST_HASH_MAX ;i++)
	{
		rwlock_init(&base_hash_buf[i].lock);
		INIT_HLIST_HEAD(&base_hash_buf[i].head); 
	}

	ddos_base_timeout.first = NULL;
	rwlock_init(&ddos_base_lock);

	memset(&this_time, 0, sizeof(struct tm));
	time(&now_time);
	localtime_r(&now_time,&this_time);
	cur_hour_index=tmp_hour_index=this_time.tm_hour;
	cur_day_index=this_time.tm_wday;



	base_pthread_base();
	return;
}

