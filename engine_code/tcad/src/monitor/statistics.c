#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "misc.h"
#include "sharemem.h"
#include "statistics.h"

extern void *sharemem_addr_get(int key);

void reload_conf_usage(void)
{
  //printf("               -r:  reload configurations\n");
	printf("                  %d: reload iplist\n", CONF_RELOAD_DDOS_IPLIST);
	printf("                  %d: reload debug\n", CONF_RELOAD_DDOS_DEBUG);
	printf("                  %d: reload log\n", CONF_RELOAD_DDOS_LOG);
	printf("                  %d: reload flow\n", CONF_RELOAD_DDOS_FLOW);
	printf("                  %d: reload detect\n", CONF_RELOAD_DDOS_DETECT);
	printf("                  %d: reload database\n", CONF_RELOAD_DDOS_DATABASE);
	printf("                  %d: reload baseline\n", CONF_RELOAD_DDOS_BASELINE);
	
	return ;
}
void usage(void)  //wdb_calc222
{
	printf("Usage: monitor -s/-c/-d/-m/-b/-t/-r/-f[type]\n");
	printf("               -s:  show statistics\n");
	printf("               -c:  clear statistices\n");
    printf("               -d:  show dist statistices\n");
    printf("               -m:  show merge statistices\n");
    printf("               -b:  show brief statistices\n");	
	printf("               -t:  show ddos trie ipinfo\n");
	printf("               -r:  reload configurations\n");
	reload_conf_usage();
	printf("               -f:  show ddos conf\n");
	return;
}


void clear_statistics(void)  //wdb_calc222
{
	int *addr;
	int index;
	
	struct monitor *mi;
	
	addr = sharemem_addr_get(SHAREMEM_ID_MONITOR);
	if(addr == NULL)
	{
		printf("get addr error\n");
		return;
	}

	mi = (struct monitor *)addr;

	memset(&mi->rc, 0, sizeof(struct recv_count));
	memset(&mi->rs, 0, sizeof(struct recv_speed));

	memset(&mi->lcs, 0, sizeof(struct logp_cache_stat));

	for(index = 0; index < MAX_LOGP_BASE_COUNT; index++)
	{
		if(mi->lbs[index].valid == LOGP_RINGQ_VALID)
		{
			mi->lbs[index].enqueue_no = 0;
			mi->lbs[index].enqueue_ok = 0;
		}
		else
		{
			memset(&mi->lbs[index], 0, sizeof(struct logp_base_stat));
		}
	}

	for(index = 0; index < 64; index++)
	{
		memset(&mi->mt_rc[index], 0, sizeof(struct recv_count));
		memset(&mi->mt_rs[index], 0, sizeof(struct recv_speed));
        memset(&mi->cdrspeed[index], 0, sizeof(struct cdr_speed));
		memset(&mi->cdrstat[index], 0, sizeof(struct cdr_stat));
        memset(&mi->diststat[index], 0, sizeof(struct dist_stat));
        memset(&mi->mergestat[index], 0, sizeof(struct merge_stat));

		memset(&mi->rxstat[index], 0, sizeof(struct rx_stat));
		memset(&mi->ethstat[index], 0, sizeof(struct ether_stat));
		memset(&mi->nwstat[index], 0, sizeof(struct network_stat));
		memset(&mi->sessstat[index], 0, sizeof(struct session_stat));
		memset(&mi->tpstat, 0, sizeof(struct transport_stat));

		memset(&mi->sessapp[index], 0, sizeof(struct session_app));

        memset(&mi->netflow[index], 0, sizeof(struct net_flow));
		memset(&mi->flowlock[index], 0, sizeof(struct flow_lock));

        memset(&mi->flowdefined[index], 0, sizeof(struct flow_defined));
	}
	
	printf("clear ok!\n");
	return;
}
static int reload_configurations(const char *arg)
{
	int level;
	int *addr;
	struct monitor *mi;

	if(NULL == arg)
		level = CONF_RELOAD_ALL;
	else
		level = atoi(arg);

	addr = sharemem_addr_get(SHAREMEM_ID_MONITOR);
	
	if(addr == NULL)
	{
		printf("get addr error\n");
		return -1;
	}

	mi = (struct monitor *)addr;

	mi->reconf_flag = level;

	printf("reload configurations : %d \n", level);
	
	return 0;
}
void ddos_trie_show(void)
{
	int i;
	unsigned int monitor_cnt;
	unsigned int white_cnt;
	long long int *addr;
	struct monitor *mi;
	char ip_info_str[32];
	char str_sip[16], str_eip[16];
	
	
	addr = sharemem_addr_get(SHAREMEM_ID_MONITOR);
	if(addr == NULL)
	{
		printf("get addr error\n");
		return;
	}
	mi = (struct monitor *)addr;

	monitor_cnt = mi->dt_dbg.monitor_cnt;
	if(monitor_cnt != 0){
		
		printf("Ddos monitor ip list: %u\n", monitor_cnt);
		for(i=0; i < monitor_cnt; i++){
			if(mi->dt_dbg.monitor_tip[i].type == 1){/* ip seg*/
				memset(str_sip, 0x0, 16);
				memset(str_eip, 0x0, 16);
				strcpy(str_sip, inet_ntoa(*((struct in_addr*)(&(mi->dt_dbg.monitor_tip[i].ip)))));
				strcpy(str_eip, inet_ntoa(*((struct in_addr*)(&(mi->dt_dbg.monitor_tip[i].plen)))));
				
				sprintf(ip_info_str, "%s-%s", str_sip, str_eip);
			}
			else{
				sprintf(ip_info_str, "%s/%d", inet_ntoa(*((struct in_addr*)(&(mi->dt_dbg.monitor_tip[i].ip)))), 
						mi->dt_dbg.monitor_tip[i].plen);
			}
			printf("\t%s\n", ip_info_str);
		}
		printf("\n");
	}

	white_cnt = mi->dt_dbg.white_cnt;
	if(white_cnt != 0){
		
		printf("Ddos white ip list: %u\n", white_cnt);
		for(i=0; i < white_cnt; i++){
			
			if(mi->dt_dbg.white_tip[i].type == 1){/* ip seg*/
				memset(str_sip, 0x0, 16);
				memset(str_eip, 0x0, 16);
				strcpy(str_sip, inet_ntoa(*((struct in_addr*)(&(mi->dt_dbg.white_tip[i].ip)))));
				strcpy(str_eip, inet_ntoa(*((struct in_addr*)(&(mi->dt_dbg.white_tip[i].plen)))));
				sprintf(ip_info_str, "%s-%s", str_sip, str_eip);
			}
			else{
				sprintf(ip_info_str, "%s/%d", inet_ntoa(*((struct in_addr*)(&(mi->dt_dbg.white_tip[i].ip)))), 
						mi->dt_dbg.white_tip[i].plen);
			}
			printf("\t%s\n", ip_info_str);
		}
		printf("\n");
	}

	return;
}


void ddos_conf_show(void)
{
	int i;
	long long int *addr;
	struct monitor *mi;
	struct ddos_conf *dcfg;
	char detech_type[256];
	int temp_len;
	
	char ddos_sw[2][4] = {"Off", "On"};
	char dir_mask[4][16] = {"", "Out", "In", "Out & In"};
	char detect_unit_mask[4][16] = {"", "Pps", "Bps", "Pps & Bps"};
	char detect_type_mask[11][16] = {"Tcp", "Syn", "Ack",
							"Chargen", "Icmp", "Dns", "Ntp", "Ssdp", 
							"Snmp","Tcp new", "Tcp live"};
	char base_sw[2][4] = {"Off", "On"};
	
	
	addr = sharemem_addr_get(SHAREMEM_ID_MONITOR);
	if(addr == NULL)
	{
		printf("get addr error\n");
		return;
	}
	mi = (struct monitor *)addr;
	dcfg = &(mi->ddos_cfg);

	memset(detech_type, 0x0, sizeof(detech_type));
	temp_len = 0;
	for(i=0; i<11; i++){
		if((1<<i)&(dcfg->detect_type_mask)){
			temp_len += sprintf(detech_type+temp_len, "%s & ", detect_type_mask[i]);
		}
	}
	detech_type[temp_len-3] = 0;
	
	printf("--------------------------- ddos-conf --------------------------\n");
		printf("Ddos function sw:                 %d %s\n", dcfg->sw, ddos_sw[dcfg->sw]);
		printf("Detect thread num:                %d\n", dcfg->detect_thread_num);
		printf("Log thread num:                   %d\n", dcfg->log_thread_num);
		printf("Debug sw:                         0x%0x\n", dcfg->debug);
		printf("Statistic flow interval:          %d s\n", dcfg->interval);
		printf("Detect cycle:                     %d s\n", dcfg->detect_interval);
		printf("Detect ratio:                     %f\n", dcfg->detect_ratio);
		printf("Detect dir:                       0x%0x %s\n", dcfg->detect_dir_mask, dir_mask[dcfg->detect_dir_mask]);
		printf("Detect unit:                      0x%0x %s\n", dcfg->detect_unit_mask, detect_unit_mask[dcfg->detect_unit_mask]);
		printf("Detect type:                      0x%0x %s\n", dcfg->detect_type_mask, detech_type);
		printf("top100 log interval:              %d\n", dcfg->top100_interval);
		printf("Device id:                        %s\n", dcfg->devid);
		printf("Flow ratio:                       %f\n", dcfg->flow_ratio);
		printf("Baseline hour/day/week:           %d/%d/%d\n", dcfg->hour_num, dcfg->day_num, dcfg->week_num);
		printf("Self learn sw:                    %d %s\n", dcfg->base_sw, base_sw[dcfg->base_sw]);
		printf("Flow in/out liminal:              %lu/%lu\n", dcfg->threshold_flow_in, dcfg->threshold_flow_out);
		printf("Tcp new connect in/out liminal:   %lu/%lu\n", dcfg->threshold_new_tcp_in, dcfg->threshold_new_tcp_out);
		printf("TCP live connect in/out liminal:  %lu/%lu\n", dcfg->threshold_live_tcp_in, dcfg->threshold_live_tcp_out);

	printf("-----------------------------------------------------------------\n");

	return;
}

int show_statistics(void)  //wdb_calc222
{
	int i;
	int *addr;
	printf("test share mem\n");
	struct monitor *mi;
	#if 1
	addr = sharemem_addr_get(SHAREMEM_ID_MONITOR);

	#else
	printf("monitor_init init\n");
	if(UTAF_OK != sharemem_create(0x0001231, sizeof(struct monitor), &addr))
	{
		printf("sharemem_init error\n");
		return UTAF_FAIL;
	}
	#endif
	
	if(addr == NULL)
	{
		printf("get addr error\n");
		return 0;
	}

	mi = (struct monitor *)addr;
	
	printf("0x%x\n", mi->magic);

    #if 0
	printf("-----------------------------------------------\n");
	printf("recv thread: \n");
	for(i = 0;i < 16; i++)
	{
		printf("thread %d: %7lu  pps,  %7lu  Mbps %7lu pkts,  %7lu Mbytes\n",i, mi->mt_rs[i].recv_pps, mi->mt_rs[i].recv_bps, mi->mt_rc[i].recv_packet_count_sum, mi->mt_rc[i].recv_packet_bytes_sum>>17);
	}
	

	printf("-----------------------------------------------\n");
	


	printf("-----------------------------------------------\n");
	printf("recv thread: \n");
	printf("       %7lu  pps,  %7lu  Mbps \n", mi->rs.recv_pps, mi->rs.recv_bps);
	printf("total: %7lu pkts,  %7lu Mbytes\n", mi->rc.recv_packet_count_sum, mi->rc.recv_packet_bytes_sum>>17);
	printf("-----------------------------------------------\n");

	printf("-----------------------------------------------\n");
	printf("logp buf stat:\n");
	printf("      total          free\n");
	printf("      %d          %d\n", LOGP_BUF_NUM - 1, mi->lbufs.used_count);
	printf("-----------------------------------------------\n");

	printf("-------------------------------------------------------\n");
	printf("logp cache stat:\n");
	printf("      used           free          enQ_ok        enQ_no\n");
	printf("       %d            %d      %10lu     %10lu\n", mi->lcs.used_count, mi->lcs.free_count, mi->lcs.enqueue_ok, mi->lcs.enqueue_no);
	printf("-------------------------------------------------------\n");

	printf("-------------------------------------------------------\n");
	printf("logp base stat(Q num: %d):\n", mi->logp_base_count);
	printf("    name       enQ_ok       enQ_no\n");
	for(i = 0; i < MAX_LOGP_BASE_COUNT; i++)
	{
		if(mi->lbs[i].valid == LOGP_RINGQ_VALID)
		{			
			printf("   %s	%10lu 	 %10lu\n",mi->lbs[i].name , mi->lbs[i].enqueue_ok, mi->lbs[i].enqueue_no);
		}
	}
	printf("-------------------------------------------------------\n");
#endif

    printf("udp server info:\n");
	printf("------------------------------------------------------\n");
	printf("    udp_server_socket       udp_server      define_flow_port   flow_event_port\n");
	printf("    %7d          0x%x       %15u           %15u\n", mi->gms_info.udp_server_socket,
            mi->gms_info.udp_server, mi->gms_info.define_flow_port, mi->gms_info.flow_event_port);
	printf("------------------------------------------------------\n");

	uint64_t version_err = 0;
	uint64_t no_buf = 0;
	uint64_t rx_ok = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		version_err += mi->rxstat[i].version_err;
		no_buf += mi->rxstat[i].no_buffer;
		rx_ok += mi->rxstat[i].rx_ok;
	}
	printf("rx stat:\n");
	printf("------------------------------------------------------\n");
	printf("    version_err       no_buf             rx_ok\n");
	printf("    %7lu          %7lu       %15lu\n", version_err, no_buf, rx_ok);
	printf("------------------------------------------------------\n");
    
	printf("\n\n");
    uint64_t ether_rxall = 0;
	uint64_t ether_rxok = 0;
    uint64_t raw_pkt_fail = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
        ether_rxall += mi->ethstat[i].rx_all;
		ether_rxok += mi->ethstat[i].rx_ok;
        raw_pkt_fail += mi->ethstat[i].raw_pkt_fail;
	}
	printf("ether stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    rx_all         rx_ok           raw_pkt_fail\n");
	printf("    %lu           %lu            %lu\n", ether_rxall, ether_rxok, raw_pkt_fail);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
    
	printf("\n\n");
	uint64_t nw_packet_invalid = 0;
	uint64_t nw_datalen_err = 0;
	uint64_t nw_ip_packet = 0;
	uint64_t nw_first_ip_frag = 0;
	uint64_t nw_other_ip_frag = 0;
	uint64_t nw_deliver_fail = 0;
	//uint64_t nw_defrag_fail = 0;
	//uint64_t nw_session_fail = 0;
	uint64_t nw_rx_ok = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		nw_packet_invalid += mi->nwstat[i].packet_invalid;
		nw_datalen_err += mi->nwstat[i].datalen_err;
		nw_ip_packet += mi->nwstat[i].ip_packet;
		nw_first_ip_frag += mi->nwstat[i].first_ip_frag;
		nw_other_ip_frag += mi->nwstat[i].other_ip_frag;
		nw_deliver_fail += mi->nwstat[i].deliver_fail;
		nw_rx_ok += mi->nwstat[i].rx_ok;
	}

	printf("network stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    packet_invalid     datalen_err        ip_packet    first_ip_frag    other_ip_frag    deliver_fail      rx_ok\n");
	printf("       %lu                   %lu                %lu              %lu                 %lu                 %lu             %lu\n", \
		nw_packet_invalid, nw_datalen_err, nw_ip_packet, nw_first_ip_frag, nw_other_ip_frag, nw_deliver_fail, nw_rx_ok);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");	

	printf("\n\n");
	uint64_t icmp4_session_fail = 0;
	uint64_t icmp4_rx_ok = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		icmp4_session_fail += mi->icmpstat[i].icmp_fail;
		icmp4_rx_ok += mi->icmpstat[i].rx_ok;
	}
	printf("icmp4 stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    session_fail        rx_ok\n");

	printf("    %15lu         %15lu\n", icmp4_session_fail, icmp4_rx_ok);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\n\n");
	uint64_t tcp4_datalen_err = 0;
	uint64_t tcp4_frag_fail = 0;
	uint64_t tcp4_session_fail = 0;
	uint64_t tcp4_rx_ok = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		tcp4_datalen_err += mi->tpstat.tcpstat[i].datalen_err;
		tcp4_frag_fail += mi->tpstat.tcpstat[i].frag_fail;
		tcp4_session_fail += mi->tpstat.tcpstat[i].session_fail;
		tcp4_rx_ok += mi->tpstat.tcpstat[i].rx_ok;
	}
	printf("tcp4 stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    datalen_err        frag_fail      session_fail                  rx_ok\n");

	printf("     %lu                   %lu        %15lu         %15lu\n", tcp4_datalen_err, tcp4_frag_fail, tcp4_session_fail, tcp4_rx_ok);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\n\n");
	uint64_t udp4_datalen_err = 0;
	uint64_t udp4_frag_fail = 0;
	uint64_t udp4_session_fail = 0;
	uint64_t udp4_rx_ok = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		udp4_datalen_err += mi->tpstat.udpstat[i].datalen_err;
		udp4_frag_fail += mi->tpstat.udpstat[i].frag_fail;
		udp4_session_fail += mi->tpstat.udpstat[i].session_fail;
		udp4_rx_ok += mi->tpstat.udpstat[i].rx_ok;
	}
	printf("udp4 stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    datalen_err        frag_fail      session_fail                  rx_ok\n");

	printf("     %lu                   %lu        %15lu         %15lu\n", udp4_datalen_err, udp4_frag_fail, udp4_session_fail, udp4_rx_ok);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

    printf("\n\n");
	uint64_t other_datalen_err = 0;
	uint64_t other_frag_fail = 0;
	uint64_t other_session_fail = 0;
	uint64_t other_rx_ok = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		other_datalen_err += mi->tpstat.otherstat[i].datalen_err;
		other_frag_fail += mi->tpstat.otherstat[i].frag_fail;
		other_session_fail += mi->tpstat.otherstat[i].session_fail;
		other_rx_ok += mi->tpstat.otherstat[i].rx_ok;
	}
	printf("others stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    datalen_err        frag_fail      session_fail                  rx_ok\n");

	printf("     %lu                   %lu        %15lu         %15lu\n", other_datalen_err, other_frag_fail, other_session_fail, other_rx_ok);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

#if 0
    printf("\n\n");
    uint64_t pkt_num = 0;
	uint64_t netflow_stat = 0;
	uint64_t netflow_age = 0;
    uint64_t netflow_other = 0;
	uint64_t netflow_error = 0;
    uint64_t netflow_error_ipver = 0;
    uint64_t netflow_age_pkts = 0;
    uint64_t netflow_age_bytes = 0;
    uint64_t netflow_stat_pkts = 0;
    uint64_t netflow_stat_bytes = 0;
    uint64_t no_find_sess_age_pkts = 0;
    uint64_t no_find_sess_age_bytes = 0;
    uint64_t no_find_sess_stat_bytes = 0;
    uint64_t no_find_session = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
        pkt_num += mi->netflow[i].pkt_num;
		netflow_stat += mi->netflow[i].session_stat;
		netflow_age += mi->netflow[i].session_age;
        netflow_other += mi->netflow[i].session_other;
		netflow_error += mi->netflow[i].error;
        netflow_error_ipver += mi->netflow[i].error_ipver;
        netflow_age_pkts += mi->netflow[i].age_pkt_num;
        netflow_age_bytes += mi->netflow[i].age_bytes;
        netflow_stat_pkts += mi->netflow[i].stat_pkt_num;
        netflow_stat_bytes += mi->netflow[i].stat_bytes;
        no_find_sess_age_pkts += mi->netflow[i].no_find_session_age;
        no_find_sess_age_bytes += mi->netflow[i].no_find_sess_age_bytes;
        no_find_sess_stat_bytes += mi->netflow[i].no_find_sess_stat_bytes;
        no_find_session += mi->netflow[i].no_find_session_stat;
	}
	printf("netflow stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("       pkt_num     session_stat       session_age    session_other            error     error_ipver\n");
	printf("    %10lu       %10lu        %10lu       %10lu       %10lu      %10lu \n",
            pkt_num,netflow_stat, netflow_age, netflow_other, netflow_error, netflow_error_ipver);
	printf("\n           age_pkts          age_bytes          stat_pkts         stat_bytes\n");
	printf("    %15lu    %15lu    %15lu    %15lu \n",
            netflow_age_pkts,netflow_age_bytes,netflow_stat_pkts,netflow_stat_bytes);
	printf("\n  no_find_sess_stat      no_find_sess_age     no_find_sess_stat_bytes      no_find_sess_age_bytes\n");
	printf("    %15lu       %15lu        %20lu        %20lu    \n",
            no_find_session,no_find_sess_age_pkts,no_find_sess_stat_bytes,no_find_sess_age_bytes);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
#endif
    printf("\n\n");
	uint64_t flow_defined_succeed = 0;
    uint64_t flow_defined_failed = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
		flow_defined_succeed += mi->flowdefined[i].succeed;
        flow_defined_failed += mi->flowdefined[i].failed;
	}
	printf("flow defined:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    succeed        failed\n");
	printf("    %10lu         %10lu\n",flow_defined_succeed,flow_defined_failed);
    printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\n\n");
	uint64_t session_add = 0;
	uint64_t session_del = 0;
	uint64_t session_full = 0;
	uint64_t session_cur = 0;
	uint64_t session_raw_pkt_fail = 0;
    uint64_t session_no_netflow = 0;
#if 0
    uint64_t session_no_netflow_tcp = 0;
    uint64_t session_no_netflow_udp = 0;
    uint64_t session_no_netflow_icmp = 0;
#endif
    uint64_t session_no_stat_netflow = 0;
#if 0
    uint64_t session_no_stat_netflow_tcp = 0;
    uint64_t session_no_stat_netflow_udp = 0;
    uint64_t session_no_stat_netflow_icmp = 0;
#endif
    uint64_t session_no_age_netflow = 0;
#if 0
    uint64_t session_no_age_netflow_tcp = 0;
    uint64_t session_no_age_netflow_udp = 0;
    uint64_t session_no_age_netflow_icmp = 0;
#endif
    uint64_t session_single_packet = 0;
    uint64_t session_ddos = 0;
    uint64_t session_up = 0;
    uint64_t session_down = 0;
    uint64_t session_up_down = 0;
    uint64_t session_eth_id_zero = 0;
    uint64_t session_eth_id_zero_pkts = 0;
    uint64_t session_eth_id_zero_bytes = 0;
	uint64_t dns = 0;
	uint64_t http = 0;
    uint64_t session_tcp = 0;
	uint64_t session_udp = 0;
    uint64_t session_icmp = 0;
    uint64_t session_tcp_pkts = 0;
	uint64_t session_tcp_bytes = 0;
    uint64_t session_udp_pkts = 0;
	uint64_t session_udp_bytes = 0;
    uint64_t session_http_pkts = 0;
	uint64_t session_http_bytes = 0;
    uint64_t session_event_age_s = 0;
    uint64_t session_event_inc_s = 0;
    uint64_t session_event_age_f = 0;
    uint64_t session_event_inc_f = 0;
    
	for(i = 0; i < MAX_LCORE; i++)
	{
		session_add += mi->sessstat[i].session_add;
		session_del += mi->sessstat[i].session_del;
		session_full += mi->sessstat[i].session_full;
		session_raw_pkt_fail += mi->sessstat[i].session_raw_pkt_fail;
        session_no_netflow += mi->sessstat[i].session_no_netflow;
        //session_no_netflow_tcp += mi->sessstat[i].session_no_netflow_tcp;
        //session_no_netflow_udp += mi->sessstat[i].session_no_netflow_udp;
        //session_no_netflow_icmp += mi->sessstat[i].session_no_netflow_icmp;
        session_no_stat_netflow += mi->sessstat[i].session_no_stat_netflow;
        //session_no_stat_netflow_tcp += mi->sessstat[i].session_no_stat_netflow_tcp;
        //session_no_stat_netflow_udp += mi->sessstat[i].session_no_stat_netflow_udp;
        //session_no_stat_netflow_icmp += mi->sessstat[i].session_no_stat_netflow_icmp;
        session_no_age_netflow += mi->sessstat[i].session_no_age_netflow;
        //session_no_age_netflow_tcp += mi->sessstat[i].session_no_age_netflow_tcp;
        //session_no_age_netflow_udp += mi->sessstat[i].session_no_age_netflow_udp;
        //session_no_age_netflow_icmp += mi->sessstat[i].session_no_age_netflow_icmp;
        session_single_packet += mi->sessstat[i].session_single_packet;
        session_ddos += mi->sessstat[i].session_ddos;        
        session_up += mi->sessstat[i].session_up;
        session_down += mi->sessstat[i].session_down;
        session_up_down += mi->sessstat[i].session_up_down;
        session_eth_id_zero += mi->sessstat[i].session_eth_id_zero;
        session_eth_id_zero_pkts += mi->sessstat[i].session_eth_id_zero_pkts;
        session_eth_id_zero_bytes += mi->sessstat[i].session_eth_id_zero_bytes;

        session_tcp += mi->sessstat[i].session_tcp;
        session_udp += mi->sessstat[i].session_udp;
        session_icmp += mi->sessstat[i].session_icmp;

        session_tcp_pkts += mi->sessstat[i].session_tcp_pkts;
        session_tcp_bytes += mi->sessstat[i].session_tcp_bytes;
        session_udp_pkts += mi->sessstat[i].session_udp_pkts;
        session_udp_bytes += mi->sessstat[i].session_udp_bytes;
        session_http_pkts += mi->sessstat[i].session_http_pkts;
        session_http_bytes += mi->sessstat[i].session_http_bytes;
        session_event_age_s += mi->sessstat[i].session_event_age_success;
        session_event_inc_s += mi->sessstat[i].session_event_inc_success;
        session_event_age_f += mi->sessstat[i].session_event_age_failed;
        session_event_inc_f += mi->sessstat[i].session_event_inc_failed;
        
		dns += mi->sessapp[i].dns;
		http += mi->sessapp[i].http;
	}
	session_cur = session_add - session_del;
	printf("session stat:\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("    session_add         session_del       session_cur        session_full     raw_pkt_fail\n");
	printf("     %10lu          %10lu        %10lu          %10lu           %10lu \n",
            session_add, session_del, session_cur, session_full,session_raw_pkt_fail);
    #if 1
    printf("\n     no_netflow     no_stat_netflow      no_age_netflow       single_packet                ddos\n");
	printf("     %10lu          %10lu          %10lu          %10lu          %10lu\n",
            session_no_netflow,session_no_stat_netflow,session_no_age_netflow,session_single_packet,session_ddos);
    #else
    printf("\n    no_netflow      no_netflow_tcp      no_netflow_udp     no_netflow_icmp\n");
	printf("     %10llu            %10llu           %10llu       %10llu\n",
            session_no_netflow,session_no_netflow_tcp,session_no_netflow_udp,session_no_netflow_icmp);
    printf("\n    no_stat_netflow   no_stat_netflow_tcp   no_stat_netflow_udp   no_stat_netflow_icmp\n");
	printf("     %10llu            %10llu           %10llu       %10llu\n",
            session_no_stat_netflow,session_no_stat_netflow_tcp,session_no_stat_netflow_udp,session_no_stat_netflow_icmp);
    printf("\n    no_age_netflow    no_age_netflow_tcp    no_age_netflow_udp   no_age_netflow_icmp\n");
	printf("     %10llu            %10llu           %10llu       %10llu\n",
            session_no_age_netflow,session_no_age_netflow_tcp,session_no_age_netflow_udp,session_no_age_netflow_icmp);
    #endif
    printf("\n     session_up        session_down     session_up_down      eth_id_zero    eth_id_zero_pkts   eth_id_zero_bytes\n");
	printf("     %10lu          %10lu          %10lu       %10lu       %14lu       %13lu\n",
            session_up, session_down, session_up_down, session_eth_id_zero,session_eth_id_zero_pkts,session_eth_id_zero_bytes);

	printf("---------------------------------------------------------------------\n");
	printf("           tcp             udp            icmp           dns           http\n");
	printf("    %10lu      %10lu      %10lu       %7lu        %7lu\n",
            session_tcp, session_udp, session_icmp, dns, http);
    printf("       tcp_pkts          tcp_bytes           udp_pkts           udp_bytes            http_pkts         http_bytes\n");
	printf(" %14lu     %14lu     %14lu      %14lu       %14lu     %14lu\n",
            session_tcp_pkts, session_tcp_bytes, session_udp_pkts, session_udp_bytes, session_http_pkts, session_http_bytes);
    printf("      session_event_inc_s          session_event_age_s      session_event_inc_f          session_event_age_f\n");
	printf(" %22lu     %22lu      %22lu     %22lu\n", session_event_inc_s, session_event_age_s, session_event_inc_f, session_event_age_f);
	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

#if 0
    printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("cdr dispatch stat:\n");
#if 0 /* wdb_as */
	for(i = 0; i <= NUM_OF_THRD_CRTING_LOGP; i++)
#else /* wdb_as */
	for(i = 0; i <= 39; i++)
#endif /* wdb_as */
	{
		printf("thread %02d: flow stat: %14lu pkts  %14lu bytes; cdr: %12lu pkts  %12lu ages, alloc failed %8lu pkts\n",i,
            mi->cdrstat[i].pkts, mi->cdrstat[i].bytes,
            mi->cdrstat[i].recv_packet_count_sum, mi->cdrstat[i].recv_packet_ages_sum,
            mi->cdrstat[i].alloc_failed);
	}
#endif	

	printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

	return 1;
}

int show_statistics_brief(void)  //wdb_calc222
{
	int i;
	int *addr;
	struct monitor *mi;

	addr = sharemem_addr_get(SHAREMEM_ID_MONITOR);
	
	if(addr == NULL)
	{
		printf("get addr error\n");
		return 0;
	}

	mi = (struct monitor *)addr;

    uint64_t ether_rxall = 0;
	uint64_t ether_rxok = 0;
    uint64_t raw_pkt_fail = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
        ether_rxall += mi->ethstat[i].rx_all;
		ether_rxok += mi->ethstat[i].rx_ok;
        raw_pkt_fail += mi->ethstat[i].raw_pkt_fail;
	}
	printf("ether stat:\n");
	printf("------------------------------------------------------\n");
	printf("    rx_all         rx_ok           raw_pkt_fail\n");
	printf("    %lu           %lu            %lu\n", ether_rxall, ether_rxok, raw_pkt_fail);

    uint64_t pkt_num = 0;
	uint64_t netflow_stat = 0;
	uint64_t netflow_age = 0;
    uint64_t netflow_other = 0;
	uint64_t netflow_error = 0;
    uint64_t netflow_error_ipver = 0;
    uint64_t netflow_age_pkts = 0;
    uint64_t netflow_age_bytes = 0;
	for(i = 0; i < MAX_LCORE; i++)
	{
        pkt_num += mi->netflow[i].pkt_num;
		netflow_stat += mi->netflow[i].session_stat;
		netflow_age += mi->netflow[i].session_age;
        netflow_other += mi->netflow[i].session_other;
		netflow_error += mi->netflow[i].error;
        netflow_error_ipver += mi->netflow[i].error_ipver;
        netflow_age_pkts += mi->netflow[i].age_pkt_num;
        netflow_age_bytes += mi->netflow[i].age_bytes;
	}
    printf("\n");
    printf("    pkt_num       session_stat        session_age      session_other         error      error_ipver       age_pkts        age_bytes\n");
	printf("    %10lu       %10lu        %10lu       %10lu       %7lu      %10lu          %10lu       %10lu \n",
            pkt_num,netflow_stat, netflow_age, netflow_other, netflow_error, netflow_error_ipver, netflow_age_pkts,netflow_age_bytes);

	uint64_t session_add = 0;
	uint64_t session_del = 0;
    uint64_t session_aged = 0;
	uint64_t session_full = 0;
	uint64_t session_cur = 0;
    uint64_t session_aging = 0;
	uint64_t session_raw_pkt_fail = 0;
    
	for(i = 0; i < MAX_LCORE; i++)
	{
		session_add += mi->sessstat[i].session_add;
        session_aged += mi->sessstat[i].session_aged;
		session_del += mi->sessstat[i].session_del;
		session_full += mi->sessstat[i].session_full;
		session_raw_pkt_fail += mi->sessstat[i].session_raw_pkt_fail;
	}
	session_cur = session_add - session_aged;
    session_aging = session_aged - session_del;
	printf("\n");
	printf("    session_add        session_aged         session_del         session_cur    session_aging         session_full\n");
	printf("     %10lu          %10lu          %10lu          %10lu       %10lu           %10lu\n",
            session_add, session_aged,session_del, session_cur, session_aging,session_full);

    printf("------------------------------------------------------\n");
        
	return 1;
}

int main(int argc, char *argv[])
{
	int ch;
	
	while ((ch = getopt(argc, argv, "scdmbtr::f")) != -1) 
	{
		switch (ch) 
		{
			case 's':
			{
				show_statistics();
				return 1;
			}
            case 'b':
			{
				show_statistics_brief();
				return 1;
			}
			case 'c':
			{
				clear_statistics();
				return 1;
			}
            case 'd':
			{
				//show_statistics_dist();
				return 1;
			}
			case 'm':
			{
				//show_statistics_merge();
				return 1;
			}
			case 't':
			{
				ddos_trie_show();
				return 1;
			}
			case 'r':
			{
				reload_configurations(optarg);
				printf("reloading...\n");
				sleep(3);
				ddos_conf_show();
				return 1;
			}
			case 'f':
			{
				ddos_conf_show();
				return 1;
			}
			default:
			{
				usage();
				return 1;
			}
		}
	}

	usage();
	return 1;
}






