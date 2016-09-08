/*************************************************************************
	> File Name: src/ddos/ddos.c
	> Author: zhangjieming
	> Mail: zhangjieming@chanct.com 
	> Created Time: Thu 20 Aug 2015 12:24:20 PM ICT
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ip4.h>
#include "misc.h"
#include "mbuf.h"
#include "utaf_session.h"
#include "monitor_ip_list.h"
#include "ddos.h"
#include "base_line.h"
#include "ddos_make_log.h"
#include "../monitor/statistics.h"

struct ddos_conf g_ddos_conf;
volatile int g_ddos_iptrie_conf_reload = 0;
pthread_mutex_t g_ddos_debug_lock=PTHREAD_MUTEX_INITIALIZER;

char g_ddos_debug_f_path[DDOS_DBG_F_PATH_LEN];

extern int32_t ddos_timer_init(void);
extern int32_t ddos_statistic_init(void);
extern void ddos_statistic_fini(void);
extern int ddos_log_init(void);


static int ddos_mkdirs(char *dirs)
{
	int i,len;
	char dir_path[DDOS_DBG_F_PATH_LEN];

	memset(dir_path, 0x0, DDOS_DBG_F_PATH_LEN);
	strcpy(dir_path, dirs);
	len = strlen(dir_path);
	if(dir_path[len-1] != '/')
		strcat(dir_path, "/");
	len = strlen(dir_path);

	for(i=0; i<len; i++){
		if(dir_path[i] == '/' && i!=0){
			dir_path[i] = 0;
			if(access(dir_path, F_OK)!=0){
				if(mkdir(dir_path, 0777)==-1){
					printf("mkdir fail. %s, %s\n", dir_path, strerror(errno));
					return -1;
				}
			}
			dir_path[i] = '/';
		}
	}
	return 0;
}

#define DDOS_DBG_F_ROOT	"/data/log/debug/ddos"
int  ddos_dbg_init(void)
{
	char exec_path[DDOS_DBG_F_PATH_LEN];
	pid_t pid;
	char logtime[32];

	time_t t = time(NULL);
	strftime(logtime, 32, "%Y%m%d%H%M%S", localtime(&t));

	memset(g_ddos_debug_f_path, 0x0, DDOS_DBG_F_PATH_LEN);
	memset(exec_path, 0x0, DDOS_DBG_F_PATH_LEN);

	pid = getpid();

	if(ddos_mkdirs(DDOS_DBG_F_ROOT)){
		printf("mkdirs fail .");
		return -1;
	}

	snprintf(g_ddos_debug_f_path, DDOS_DBG_F_PATH_LEN, "%s/ddos-%d-%s.debug", DDOS_DBG_F_ROOT, pid, logtime);

	return 0;
}

void ddos_flow_statistic_d(session_item_t *si)
{
	uint32_t sip, dip;

	if(likely(!(g_ddos_conf.debug & DDOS_MID_FLOW)))
		return ;
	
	ddos_debug_lock();

	sip = si->ip_tuple.sip[0];
	dip = si->ip_tuple.dip[0];

	ddos_debug_f(DDOS_MID_FLOW, "flow data >> sip:%"IPV4FMT" dip:%"IPV4FMT" sport:%d dport:%d" 
			" protocol:%d stype:%d stroup:%d\n", IPV4P(&sip), IPV4P(&dip), 
			si->sport, si->dport, si->protocol, si->service_type,
			si->service_group);

	switch (si->protocol) 
	{
		case PROTO_TCP:
			ddos_debug_f(DDOS_MID_FLOW, "TCP[in]:%ld %ld TCP[out]:%ld %ld SYN[in]:%ld %ld SYN[out]:%ld %ld ACK:%ld %ld ACK:%ld %ld\n", 
					si->ddos.stat.tcp.tcp[dir_in].pkts, si->ddos.stat.tcp.tcp[dir_in].bytes,
					si->ddos.stat.tcp.tcp[dir_out].pkts, si->ddos.stat.tcp.tcp[dir_out].bytes,
					si->ddos.stat.tcp.syn[dir_in].pkts,  si->ddos.stat.tcp.syn[dir_in].bytes,
					si->ddos.stat.tcp.syn[dir_out].pkts, si->ddos.stat.tcp.syn[dir_out].bytes,
					si->ddos.stat.tcp.ack[dir_in].pkts,  si->ddos.stat.tcp.ack[dir_in].bytes,
					si->ddos.stat.tcp.ack[dir_out].pkts, si->ddos.stat.tcp.ack[dir_out].bytes);
			break;
		case PROTO_UDP:
			ddos_debug_f(DDOS_MID_FLOW, "UDP[in]:%ld %ld UDP[out]:%ld %ld\n", 
					si->ddos.stat.udp.udp[dir_in].pkts, si->ddos.stat.udp.udp[dir_in].bytes,
					si->ddos.stat.udp.udp[dir_out].pkts, si->ddos.stat.udp.udp[dir_out].bytes);
			break;
		case PROTO_ICMP:
			ddos_debug_f(DDOS_MID_FLOW, "ICMP[in]:%ld %ld ICMP[out]:%ld %ld\n", 
					si->ddos.stat.icmp.icmp[dir_in].pkts, si->ddos.stat.icmp.icmp[dir_in].bytes,
					si->ddos.stat.icmp.icmp[dir_out].pkts, si->ddos.stat.icmp.icmp[dir_out].bytes);
			break;
		default:
			ddos_debug_f(DDOS_MID_FLOW, "protocol unknown\n");
	}
	ddos_debug_unlock();
	
	return;
}
int ddos_init_flow(session_item_t *si, struct m_buf *mbuf)
{
#if 0 
	unsigned int ret ;
	unsigned int ip;
	ip = ntohl(si->ip_tuple.sip[0]);
	ret = ddos_mnt_ip_lookup(ip);
	printf("-----------------ret:%u, ip:%s\n", ret, inet_ntoa(*((struct in_addr*)(&ip))) );
#endif 
	if (unlikely(g_ddos_iptrie_conf_reload))
		goto out;
	
	if (e_ddos_ip_type_monitor ==  ddos_mnt_ip_lookup(ntohl(si->ip_tuple.sip[0]))) {
		si->ddos_flag = UTAF_DO_DDOS;
		si->flow_dir = dir_out;
		ddos_debug_lf(DDOS_MID_FLOW, "ddos_flag:%d flow_dir:%d\n", si->ddos_flag, si->flow_dir);
	} else if (e_ddos_ip_type_monitor == ddos_mnt_ip_lookup(ntohl(si->ip_tuple.dip[0]))) {
		si->ddos_flag = UTAF_DO_DDOS;
		si->flow_dir = dir_in;
		ddos_debug_lf(DDOS_MID_FLOW, "ddos_flag:%d flow_dir:%d\n", si->ddos_flag, si->flow_dir);
	} else {
out:
		si->ddos_flag = UTAF_DO_NOT_DDOS;
	}
	mbuf->pkt_dir = si->flow_dir;

	return 0;
}
int get_dev_id(char *src,char *dst)
{
	FILE *fp;
	memset(dst,0,20);
	fp=popen("python /gms/gapi/modules/auth/genkey.pyc","r");
	fgets(dst,20,fp);
	pclose(fp);
	if (strcmp(dst,"")==0)
		exit(0);


	return 0;
}
int ddos_load_debug_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};
	uint32_t uinttmp;

	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);
	if(!tcad_conf_get_profile_string(ddos_conf, "ddos", "debug", buf))
	{
		sscanf(buf, "%x\n", &uinttmp);
		g_ddos_conf.debug = uinttmp;
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos: debug 0x%0x\n", g_ddos_conf.debug);

		if(g_ddos_conf.debug && ddos_dbg_init())
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "ddos ddos_dbg_init fail\n");
			g_ddos_conf.debug = 0;
		}

		return 0;
	}

	g_ddos_conf.debug = 0;
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: load debug flag error\n");

	return 1;
}
int ddos_load_log_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};
	uint32_t uinttmp;
	
	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);

	/* (0, 3600s] */
	if (!tcad_conf_get_profile_string(ddos_conf, "log", "log_top100_interval", buf))
	{
		uinttmp = atoi(buf);
		if (uinttmp > 0 && uinttmp <= 3600)
		{
			g_ddos_conf.top100_interval = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "log_top100_interval must be in (0, 3600s]\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: output interval of top100 log %d\n", g_ddos_conf.top100_interval);

	if (!tcad_conf_get_profile_string(ddos_conf, "log", "log_thread_num", buf))
	{
		uinttmp = atoi(buf);
	//	printf("ddos: log thread num %d\n",uinttmp);
		if (uinttmp > 0 && uinttmp <= 16)
		{
			g_ddos_conf.log_thread_num = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "log_thread_num must be in (0, 16]\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: log thread num %d\n", g_ddos_conf.log_thread_num);
	
	return 1;
}

int ddos_load_flow_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};
	uint32_t uinttmp;

	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);

		/* ddos flow */
	if(!tcad_conf_get_profile_string(ddos_conf, "ddos_flow", "flow_statistic_interval", buf))
	{
		uinttmp = atoi(buf);
		if (uinttmp > 0 && uinttmp <= 120) /* (0, 120s] */
		{
			g_ddos_conf.interval = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "flow_statistic_interval must be in (0, 120s]\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: flow statistic interval %d\n", g_ddos_conf.interval);

	return 0;
}
int ddos_load_detect_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};
	uint32_t uinttmp;
	float floattmp;

	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);
	
	/* detect */
	/* (0, 3600s] */
	if (!tcad_conf_get_profile_string(ddos_conf, "detect", "detect_interval", buf))
	{
		uinttmp = atoi(buf);
		if (uinttmp > 0 && uinttmp <= 3600)
		{
			g_ddos_conf.detect_interval = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "detect_interval must be in (0, 3600s]\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: detect cycle %d\n", g_ddos_conf.detect_interval);
	/* (0, 16] */
	if (!tcad_conf_get_profile_string(ddos_conf, "detect", "detect_thread_num", buf))
	{
		uinttmp = atoi(buf);
		if (uinttmp > 0 && uinttmp <= 16)
		{
			g_ddos_conf.detect_thread_num = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "detect_thread_num must be in (0, 16]\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: detect thread num %d\n", g_ddos_conf.detect_thread_num);
	if (!tcad_conf_get_profile_string(ddos_conf, "detect", "detect_dir_mask", buf))
	{
		//uinttmp = atoi(buf);
		sscanf(buf, "%x\n", &uinttmp);
		if (uinttmp >0 && uinttmp <= 3)
		{
			g_ddos_conf.detect_dir_mask = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "detect_dir_mask must be 1,2,3\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: detect dir mask 0x%0x\n", g_ddos_conf.detect_dir_mask);
	if (!tcad_conf_get_profile_string(ddos_conf, "detect", "detect_unit_mask", buf))
	{
		//uinttmp = atoi(buf);
		sscanf(buf, "%x\n", &uinttmp);
		if (uinttmp >0 && uinttmp <= 3)
		{
			g_ddos_conf.detect_unit_mask = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "detect_unit_mask must be 1,2,3\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: detect unit mask 0x%0x\n", g_ddos_conf.detect_unit_mask);
	if (!tcad_conf_get_profile_string(ddos_conf, "detect", "detect_type_mask", buf))
	{
		//uinttmp = atoi(buf);
		sscanf(buf, "%x\n", &uinttmp);
		if (uinttmp > 0)
		{
			g_ddos_conf.detect_type_mask = uinttmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "detect_thread_num must be >0\n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: detect type mask 0x%0x\n", g_ddos_conf.detect_type_mask);
	if(!tcad_conf_get_profile_string(ddos_conf, "detect", "detect_ratio", buf))
	{
		floattmp= atof(buf);
		if (floattmp >= 0) 
		{
			g_ddos_conf.detect_ratio = floattmp;
		}
		else
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "flow_statistic_interval must be >=0 \n");
		}
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: detect ratio %.2f\n", g_ddos_conf.detect_ratio);

	return 0;
}
int ddos_load_baseline_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};
	uint32_t uinttmp;
	float  flow_ratio;

	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);

	/** base config **/
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "flow_ratio",buf))
	{
		flow_ratio = atof(buf);
		if (flow_ratio < 1)
		{
			g_ddos_conf.flow_ratio = flow_ratio;
		}
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf flow_ratio : %0.2f \n",g_ddos_conf.flow_ratio);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "hour_num",buf))
	{
		g_ddos_conf.hour_num = atoi(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf hour_num : %d \n",g_ddos_conf.hour_num);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "day_num",buf))
	{
		g_ddos_conf.day_num = atoi(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf day_num : %d \n",g_ddos_conf.day_num);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "week_num",buf))
	{
		uinttmp = atoi(buf);
	   	if (uinttmp <1 || uinttmp > 7)
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "base week_num not cloud outweigh 7 \n");	
		}
		else
		{
			g_ddos_conf.week_num = uinttmp;
			CA_LOG(LOG_MODULE, LOG_PROC,"base_conf week_num : %d \n",g_ddos_conf.week_num);
		}
	}
	/** base switch **/
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "self_learn_switch",buf))
	{
		g_ddos_conf.base_sw = atoi(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf self_learn_switch: %d \n",g_ddos_conf.base_sw);
	}

	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "threshold_flow_in",buf))
	{
		g_ddos_conf.threshold_flow_in = atol(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf threshold_flow_in: %d \n",g_ddos_conf.threshold_flow_in);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "threshold_flow_out",buf))
	{
		g_ddos_conf.threshold_flow_out = atol(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf threshold_flow_out: %d \n",g_ddos_conf.threshold_flow_out);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "threshold_new_tcp_in",buf))
	{
		g_ddos_conf.threshold_new_tcp_in = atol(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf threshold_new_tcp_in: %d \n",g_ddos_conf.threshold_new_tcp_in);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "threshold_new_tcp_out",buf))
	{
		g_ddos_conf.threshold_new_tcp_out = atol(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf threshold_new_tcp_out: %d \n",g_ddos_conf.threshold_new_tcp_out);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "threshold_live_tcp_in",buf))
	{
		g_ddos_conf.threshold_live_tcp_in = atol(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf threshold_live_tcp_in: %d \n",g_ddos_conf.threshold_live_tcp_in);
	}
	if (!tcad_conf_get_profile_string(ddos_conf, "base_conf", "threshold_live_tcp_out",buf))
	{
		g_ddos_conf.threshold_live_tcp_out = atol(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"base_conf threshold_live_tcp_out: %d \n",g_ddos_conf.threshold_live_tcp_out);
	}

	return 0;
}
int ddos_load_postgresql_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};
	
	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);
	/* postgresql */
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "postgresql_off",buf))
	{
		g_ddos_conf.gpq_info.postgresql_off = atoi(buf);
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql enable off: %d \n",g_ddos_conf.gpq_info.postgresql_off);
	}	
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "host_ip",g_ddos_conf.gpq_info.host_ip))
	{
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql host_ip: %s \n",g_ddos_conf.gpq_info.host_ip);
	}	
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "host_port",g_ddos_conf.gpq_info.host_port))
	{
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql host_port : %s \n",g_ddos_conf.gpq_info.host_port);
	}	
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "db_name",g_ddos_conf.gpq_info.dbname))
	{
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql dn_name: %s \n",g_ddos_conf.gpq_info.dbname);
	}	
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "user",g_ddos_conf.gpq_info.user))
	{
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql user_name : %s \n",g_ddos_conf.gpq_info.user);
	}	
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "pwd",g_ddos_conf.gpq_info.pwd))
	{
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql passwd : %s \n",g_ddos_conf.gpq_info.pwd);
	}	
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "unix",buf))
	{
		g_ddos_conf.gpq_info.unix_val = atoi(buf);
	}
	if(!tcad_conf_get_profile_string(ddos_conf, "postgresql", "domain_sock",g_ddos_conf.gpq_info.domain))
	{
		CA_LOG(LOG_MODULE, LOG_PROC,"postgresql domain_sock : %s \n",g_ddos_conf.gpq_info.domain);
	}	

	return 1;
}

int ddos_load_conf_from_file(void)
{
	char ddos_conf[256] = {0};
	char buf[128] = {0};

	sprintf(ddos_conf, "%s/ddos.conf", DDOS_CONFIG_DIR);

	/* ddos globle */
	if(!tcad_conf_get_profile_string(ddos_conf, "ddos", "switch", buf))
	{
		g_ddos_conf.sw = atoi(buf);
	}
	if (!g_ddos_conf.sw)
	{
		return 1;
	}
	CA_LOG(LOG_MODULE, LOG_PROC, "ddos: switch %d\n", g_ddos_conf.sw);
	
	ddos_load_debug_conf_from_file();
	
	/* ddos flow */
	ddos_load_flow_conf_from_file();

	/* log */
	ddos_load_log_conf_from_file();

	ddos_load_postgresql_conf_from_file();

	/** dev id get**/
	get_dev_id(NULL,g_ddos_conf.devid);

	ddos_load_baseline_conf_from_file();

	ddos_load_detect_conf_from_file();

	return 0;

}

int ddos_init(void)
{
	memset(&g_ddos_conf, 0x0, sizeof(g_ddos_conf));
	g_ddos_conf.sw = 0;
	g_ddos_conf.debug = 0;
	g_ddos_conf.interval = DDOS_DEFAULT_LOG_INTERVAL;
	g_ddos_conf.detect_interval = DDOS_DEFAULT_DET_INTERVAL;
	g_ddos_conf.detect_ratio = DDOS_DEFAULT_DET_RATIO;
	g_ddos_conf.detect_thread_num = DDOS_DEFAULT_DET_THREAT;
	g_ddos_conf.detect_dir_mask = DDOS_DEFAULT_DET_DIR_MASK;
	g_ddos_conf.detect_unit_mask = DDOS_DEFAULT_DET_UNIT_MASK;
	g_ddos_conf.detect_type_mask = DDOS_DEFAULT_DET_TYPE_MASK;
	g_ddos_conf.top100_interval = DDOS_DEFAULT_T100_INTERVAL;
	g_ddos_conf.log_thread_num = DDOS_DEFAULT_LOG_THREAD_NUM;
	g_ddos_conf.gpq_info.postgresql_off = DDOS_POSTGRES_OFF;
	strcpy(g_ddos_conf.gpq_info.host_ip	  , DDOS_POSTGRES_IP);
	strcpy(g_ddos_conf.gpq_info.host_port , DDOS_POSTGRES_PORT);
	strcpy(g_ddos_conf.gpq_info.dbname	  , DDOS_POSTGRES_DB);
	strcpy(g_ddos_conf.gpq_info.user	  , DDOS_POSTGRES_USER);
	strcpy(g_ddos_conf.gpq_info.pwd		  , DDOS_POSTGRES_pwd);
	g_ddos_conf.gpq_info.unix_val		= DDOS_POSTGRES_unix;
	strcpy(g_ddos_conf.gpq_info.domain	  , DDOS_POSTGRES_domain);
	g_ddos_conf.base_sw					= DDOS_BASE_LEARN;
	g_ddos_conf.threshold_flow_in		= DDOS_BASE_FLOW_IN;
	g_ddos_conf.threshold_flow_out		= DDOS_BASE_FLOW_OUT;
	g_ddos_conf.threshold_new_tcp_in	= DDOS_BASE_NEW_TCP_IN;
	g_ddos_conf.threshold_new_tcp_out	= DDOS_BASE_NEW_TCP_OUT;
	g_ddos_conf.threshold_live_tcp_in	= DDOS_BASE_LIVE_TCP_IN;
	g_ddos_conf.threshold_live_tcp_out	= DDOS_BASE_LIVE_TCP_OUT;
   	g_ddos_conf.hour_num				= DDOS_BASE_HOUR;
	g_ddos_conf.day_num					= DDOS_BASE_DAY;
	g_ddos_conf.week_num				= DDOS_BASE_WEEK;
	g_ddos_conf.flow_ratio				= DDOS_FLOW_RATIO; 
	

	if (ddos_load_conf_from_file() != 0)
	{
		g_ddos_conf.sw = 0;

		printf("ddos off.\n");
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos load conf fail, ddos off\n");

		return 0;
	}
	CONF_COPY_DDOS_CONF(&g_ddos_conf);

#if 0
	if (ddos_timer_init() != 0)
	{
		printf("ddos init timer error\n");
		return -1;
	}
#endif
	if (ddos_mnt_ip_init() != 0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos init ip list error\n");
		return -1;
	}
	if (ddos_statistic_init() != 0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos init statistic module error\n");
		return -1;
	}
	/* thread init */
	if (ddos_log_init() != SUCCE)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos init log module error\n");
		return -1;
	}

	base_hash_init();

	if (ddos_get_baseline_from_file() != 0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos get baseline file error\n");
	}

	CA_LOG(LOG_MODULE, LOG_PROC, "ddos_init: ok.\n");
	printf("ddos_init: ok.\n");
	
	return 0;
}

void ddos_exit(void)
{
	ddos_mnt_ip_finish();
	ddos_statistic_fini();

	return ;
}
