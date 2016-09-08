#include <stdint.h> 
#include "ddos_log_out.h"
#include "ddos_psql.h"
#include "misc.h"
#include "ddos.h"
#include "ddos_make_log.h"
/* 
 * @param struct gpq_conn_info
 *
 * @return
 * 		 	postgresql's socket
 * */
UTAF_DECLARE_PER_LCORE(int,ddos_thread_id);
#define M_SIZE (1)
extern struct ddos_log_empty_para *g_ddos_stat_log_thread_para;

 PGconn * flow_connect_db(struct gpq_conn_info g_flow_info_cfg)
{
	PGconn *ddos_conn;

	if (g_flow_info_cfg.unix_val == 1) {
		ddos_conn = gpq_connectdb_host(g_flow_info_cfg.domain,
				g_flow_info_cfg.dbname, g_flow_info_cfg.user,
				g_flow_info_cfg.pwd);    
	}   
	else {
		ddos_conn = gpq_connectdb(g_flow_info_cfg.host_ip, g_flow_info_cfg.host_port,    
				g_flow_info_cfg.dbname, g_flow_info_cfg.user,    
				g_flow_info_cfg.pwd); 

	}
	if (gpq_sql_cmd(ddos_conn, "set standard_conforming_strings=on") < 0) {
		ddos_debug(DDOS_MID_STORE,"set standard_conforming_strings=on sucess ... \n");			
	}
	return ddos_conn;
}
void time_int_to_char(time_t timeint,char *str_time)
{
	char time_format[]="%Y-%m-%d %X";
	struct tm timetmp[1];
	localtime_r(&timeint,timetmp);
	strftime(str_time, TIME_FORMAT_SIZE-1, time_format, timetmp);
}
void top_5min_tmp_store(char *store_tmp,int size,node_curr_top100 *buf , int i,int query_id)
{
	char start_time[TIME_FORMAT_SIZE];
	char end_time[TIME_FORMAT_SIZE];
	time_t time_mis;
	time_mis = buf[i].end_time - buf[i].start_time;
	time_int_to_char(buf[i].start_time,start_time);
	time_int_to_char(buf[i].end_time,end_time);
	if (time_mis <=0)
	{
		time_mis = g_ddos_conf.detect_interval ;
	}
	snprintf(store_tmp,size,"%s|%s|%s|%u|%u|%u|%0.2f|%0.2f|%u|%u|%u|%0.2f|%0.2f|%u|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%lu|%u\n",
			g_ddos_conf.devid,
			start_time,
			end_time,
			buf[i].sip,
			buf[i].s_country_id,
			buf[i].s_city_id,
			buf[i].s_latitude,
			buf[i].s_longitude,
			buf[i].dip,
			buf[i].d_country_id,
			buf[i].d_city_id,
			buf[i].d_latitude,
			buf[i].d_longitude,
			buf[i].attack_id,
			(uint64_t)(buf[i].curr_top_detail.tcp_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.tcp_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.tcp_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.tcp_out_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.tcp_syn_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.tcp_syn_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.tcp_syn_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.tcp_syn_out_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.tcp_ack_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.tcp_ack_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.tcp_ack_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.tcp_ack_out_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)((buf[i].curr_top_detail.tcp_new-buf[i].curr_top_detail.tcp_new_old_first)/time_mis),
			(uint64_t)((buf[i].curr_top_detail.tcp_new-buf[i].curr_top_detail.tcp_close)),

			(uint64_t)(buf[i].curr_top_detail.chargen_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.chargen_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.chargen_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.chargen_out_bytes*8/(time_mis*M_SIZE)),
			
			(uint64_t)(buf[i].curr_top_detail.icmp_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.icmp_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.icmp_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.icmp_out_bytes*8/(time_mis*M_SIZE)),
			
			(uint64_t)(buf[i].curr_top_detail.dns_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.dns_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.dns_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.dns_out_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.ntp_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.ntp_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.ntp_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.ntp_out_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.ssdp_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.ssdp_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.ssdp_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.ssdp_out_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.snmp_in_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.snmp_out_packets/time_mis),
			(uint64_t)(buf[i].curr_top_detail.snmp_in_bytes*8/(time_mis*M_SIZE)),
			(uint64_t)(buf[i].curr_top_detail.snmp_out_bytes*8/(time_mis*M_SIZE)),

//(uint64_t)((buf[i].curr_top_detail.flow_in_bytes+buf[i].curr_top_detail.flow_out_bytes)*8/(time_mis*M_SIZE)),
			(uint64_t)((buf[i].curr_top_detail.flow_in_bytes+buf[i].curr_top_detail.flow_out_bytes)*8/(time_mis)),
			(uint64_t)((buf[i].curr_top_detail.flow_in_packets+buf[i].curr_top_detail.flow_out_packets)/time_mis),
			query_id
				);
	return;
}
char * init_top_5min_sql(char *sql_cmd)
{
	memset(sql_cmd,0 ,DDOS_5MIN_SQL_SIZE);
	snprintf(sql_cmd,DDOS_5MIN_SQL_SIZE,"COPY %s (dev_id,start_time,end_time,sip,s_country_id,s_city_id,s_latitude,s_longitude,dip,d_country_id,d_city_id,d_latitude,d_longitude,attack_id,"
		"tcp_in_pps,tcp_out_pps,tcp_in_bps,tcp_out_bps,tcp_syn_in_pps,tcp_syn_out_pps,tcp_syn_in_bps,tcp_syn_out_bps,"
		"tcp_ack_in_pps,tcp_ack_out_pps,tcp_ack_in_bps,tcp_ack_out_bps,tcp_new_num_ps,tcp_live_num_ps,"
		"chargen_in_pps,chargen_out_pps,chargen_in_bps,chargen_out_bps,icmp_in_pps,icmp_out_pps,icmp_in_bps,icmp_out_bps,"
		"dns_in_pps,dns_out_pps,dns_in_bps,dns_out_bps,ntp_in_pps,ntp_out_pps,ntp_in_bps,ntp_out_bps,"
		"ssdp_in_pps,ssdp_out_pps,ssdp_in_bps,ssdp_out_bps,snmp_in_pps,snmp_out_pps,snmp_in_bps,snmp_out_bps,"
		"flow_bps,flow_pps,query_id) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'",DDOS_MIN_TABLE);	
	ddos_debug(DDOS_MID_STORE,"insert top_5min_sql comm:%s",sql_cmd);
	return sql_cmd;
}
char * init_top_end_sql(char *sql_cmd)
{
	memset(sql_cmd,0 ,DDOS_END_SQL_SIZE);
	snprintf(sql_cmd,DDOS_END_SQL_SIZE,"COPY %s (dev_id,start_time,end_time,sip,s_country_id,s_city_id,dip,attack_id,query_id,tcp_new_num_ps,tcp_live_num_ps,flow_max_bps,flow_pps) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'",DDOS_END_TABLE);	
	return sql_cmd;
}
char * init_ddosip_sql(char *sql_cmd)
{
	memset(sql_cmd,0 ,DDOS_IP_SQL_SIZE);
	snprintf(sql_cmd,DDOS_IP_SQL_SIZE,"COPY %s (dev_id,start_time,end_time,ddos_ip,flow_bps,flow_pps,attack_id,query_id) FROM STDIN CSV DELIMITER '|' NULL '\\N' ESCAPE '\\'",DDOS_IP_TABLE);	
	return sql_cmd;
}
/******************************************************************
 *	storage ddos information for 5min
 * @parma 	conn	postgresql连接句柄
 * 			buf		top100的数组
 *
 * @return
 * 			suc 0
 * 			fail -1
 *
 * ***************************************************************/
int ddos_log_top100_5min_store(rbtree_node_destIP_outddos_data *buf)
{
	//int index;
	int i;
	int retn;
	PGconn *conn=g_ddos_stat_log_thread_para[UTAF_PER_LCORE(ddos_thread_id)].ddos_conn;
	//int size=(sizeof(struct node_curr_top100)+DEVID_SIZE+1);
	int size=1024;
	char store_buf[TOPNUM*size];
	char store_tmp[size];
	char sql_cmd[DDOS_5MIN_SQL_SIZE];

	if (g_ddos_conf.gpq_info.postgresql_off == 0)
	{
		if(conn == NULL)
		{
			return -1;
		}
	memset(store_buf,0,TOPNUM*size);

	for ( i =0 ; i < TOPNUM && (buf->curr_top[i].dip != 0); i++){
		memset(store_tmp,0,size);
		top_5min_tmp_store(store_tmp,size,(buf->curr_top),i,buf->query_id);
		ddos_debug(DDOS_MID_STORE, "-------------------------------store[thread_id : %d  buf[%d].sip =%u ]---------------------------- \n",UTAF_PER_LCORE(ddos_thread_id),i,buf->curr_top[i].sip);
		strncat(store_buf,store_tmp,size);
	}
	if(0 == strlen(store_buf)){		
		ddos_debug(DDOS_MID_STORE,"\nstore_buf:%s,buf_len:%ld return\n",store_buf,strlen(store_buf));
		return 0;
	}
	
	init_top_5min_sql(sql_cmd);
	ddos_debug(DDOS_MID_STORE,"\nsql_cmd:%s ,store_buf:%s,buf_len:%ld\n",sql_cmd,store_buf,strlen(store_buf));
	//CA_LOG(LOG_MODULE, LOG_PROC, "\nsql_cmd:%s ,store_buf:%s,buf_len:%ld\n",sql_cmd,store_buf,strlen(store_buf));
	for(i =0 ;i<2;i++)
	{		
		pthread_mutex_lock(&min_store_lock);
		retn = gpq_copy_from_buf_sql(conn, DDOS_MIN_TABLE, sql_cmd, store_buf, strlen(store_buf));		
		pthread_mutex_unlock(&min_store_lock);
		if(retn < 0 )
		{
			ddos_debug(DDOS_MID_STORE, "top_5min_ddos_store_ERROR_fail ... \n");
			CA_LOG(LOG_MODULE, LOG_PROC, "top_5min_ddos_store_ERROR_fail ... \n");
			gpq_disconnect(conn);
			conn=flow_connect_db(g_ddos_conf.gpq_info);
			if(conn == NULL)
			{
				return -1;
			}
			g_ddos_stat_log_thread_para[UTAF_PER_LCORE(ddos_thread_id)].ddos_conn = conn;
			continue;		
		}
		else
		{
			ddos_debug(DDOS_MID_STORE, "top_5min_ddos_store_sucess ... \n");			
			CA_LOG(LOG_MODULE, LOG_PROC, "top_5min_ddos_store_sucess ... \n");
			break;

		}
	}
	if (retn <0 )
	{
		return -1;
	}
	}
	return 0;
}
void top_end_tmp_store(char *store_tmp,int size,struct node_curr_top100 *buf,int i,uint32_t uint_id)
{
	char start_time[TIME_FORMAT_SIZE];
	char end_time[TIME_FORMAT_SIZE];
	time_t time_mis;
	time_mis = buf[i].end_time - buf[i].start_time;
	time_int_to_char(buf[i].start_time,start_time);
	time_int_to_char(buf[i].end_time,end_time);
	memset(store_tmp,0,size);
	if (time_mis <= 0)
	{
		time_mis = g_ddos_conf.detect_interval;
	}
	snprintf(store_tmp,size,"%s|%s|%s|%u|%u|%u|%u|%u|%u|%lu|%lu|%lu|%lu\n",g_ddos_conf.devid ,
			start_time,
			end_time,
			buf[i].sip,
			buf[i].s_country_id,
			buf[i].s_city_id,
			buf[i].dip,
			buf[i].attack_id,
			uint_id,
			(uint64_t)((buf[i].curr_top_detail.tcp_new-buf[i].curr_top_detail.tcp_new_old_first)/time_mis),
			(uint64_t)((buf[i].curr_top_detail.tcp_new-buf[i].curr_top_detail.tcp_close)),
			(uint64_t)((buf[i].flow_bps)*8/(time_mis*(M_SIZE))),
			(uint64_t)((buf[i].flow_pps)/time_mis)

			);

}

/****************************************************************
 * storage ddos end total top100 information
 *
 * @parma	conn postgresql数据库的句柄
 * 		 	buf  当ddos结束时统计出的top100数组
 * 		 	uint_id	同一个ddos攻击具有相同的id，以便于查询
 *
 * @return 
 * 			suc  0
 * 			fail 1
 * **************************************************************/
int ddos_log_top100_end_store(PGconn *conn,struct node_curr_top100 *buf,uint32_t uint_id)
{
	//int index;
	int i;
	int retn;
	//int size=(sizeof(struct node_curr_top100)+DEVID_SIZE+1);
	int size= 800;
	char store_buf[TOPNUM*size];
	char store_tmp[size];
	char sql_cmd[DDOS_5MIN_SQL_SIZE];
	if (g_ddos_conf.gpq_info.postgresql_off == 0)
	{
	memset(store_buf,0,TOPNUM*size);

	for ( i =0 ; i < TOPNUM && buf[i].dip != 0; i++){
		memset(store_tmp,0,size);
		top_end_tmp_store(store_tmp,size,buf,i,uint_id);
		strncat(store_buf,store_tmp,size);
	}
	if(0 == strlen(store_buf)){
		ddos_debug(DDOS_MID_STORE, " store_buf 0,len store_buf 0... \n");
		return 0;
	}
	init_top_end_sql(sql_cmd);
	retn = gpq_copy_from_buf_sql(conn, DDOS_END_TABLE, sql_cmd, store_buf, strlen(store_buf));
	//CA_LOG(LOG_MODULE, LOG_PROC, "ddos_log_top100_end_store:sql_cmd:%s,store_buf:%s,store_buf len:%d ... \n",sql_cmd,store_buf,strlen(store_buf));
	if(retn < 0 )
	{
		ddos_debug(DDOS_MID_STORE, "top_end_ddos_store_ERROR_fail ... \n");
		CA_LOG(LOG_MODULE, LOG_PROC, "top_end_ddos_store_ERROR_fail ... \n");
		return -1;		
	}
	else
	{
		ddos_debug(DDOS_MID_STORE, "top_end_ddos_store_sucess ... \n");
		CA_LOG(LOG_MODULE, LOG_PROC, "top_end_ddos_store_sucess ... \n");		
	}
	}
	return 0;
}
void ddosip_to_buf(char *store_tmp,int size,rbtree_node_destIP_outddos_data *buf)
{
	char start_time[TIME_FORMAT_SIZE];
	char end_time[TIME_FORMAT_SIZE];
	time_t time_mis;
	time_mis = buf->end_time - buf->start_time;
	time_int_to_char(buf->start_time,start_time);
	time_int_to_char(buf->end_time,end_time);
	memset(store_tmp,0,size);
	if(time_mis <= 0)
	{
		time_mis = g_ddos_conf.detect_interval;
	}
	snprintf(store_tmp,size,"%s|%s|%s|%u|%0.2f|%lu|%u|%u\n",g_ddos_conf.devid ,
			start_time,
			end_time,
			buf->dip,
			(float)buf->flow_bps*8/(time_mis*(1024*1024)),/** flow bps**/
			(uint64_t)buf->flow_pps/time_mis,/** flow_pps**/
			buf->attack_id,
			buf->query_id
			);

}
/****************************************************************
 * storage ddos end output  ddos_ip information
 *
 * @parma	conn postgresql数据库的句柄
 * 		 	buf  当ddos结束时统计的只要ddos_ip的数组
 * 		 	uint_id	同一个ddos攻击具有相同的id，以便于查询
 *
 * @return 
 * 			suc  0
 * 			fail 1
 * **************************************************************/

int ddos_log_ddosip_store(PGconn *conn,rbtree_node_destIP_outddos_data *buf)
{
	//int index;
	int retn;
	int i;
	//int size=(sizeof(struct node_curr_top100)+DEVID_SIZE+1);
	int size= 800;
	char store_buf[size];
	char sql_cmd[DDOS_IP_SQL_SIZE];
	if (g_ddos_conf.gpq_info.postgresql_off == 0)
	{
		if(conn == NULL)
		{
			return -1;
		}
	memset(store_buf,0,size);

	ddosip_to_buf(store_buf,size,buf);
	init_ddosip_sql(sql_cmd);
	for(i=0 ; i<1 ; i++)
	{
	retn = gpq_copy_from_buf_sql(conn, DDOS_IP_TABLE, sql_cmd, store_buf, strlen(store_buf));
	//CA_LOG(LOG_MODULE, LOG_PROC, "ddos_log_ddosip_store:sql_cmd:%s,store_buf:%s,store_buf len:%d ... \n",sql_cmd,store_buf,strlen(store_buf));
	if(retn < 0 )
	{
		ddos_debug(DDOS_MID_STORE, "ddos_ip_store_ERROR_fail ... \n");	
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos_ip_store_ERROR_fail ... \n");		
#if 0
		gpq_disconnect(conn);
		conn=flow_connect_db(g_ddos_conf.gpq_info);
		if(conn == NULL)
		{
			return -1;
		}
		continue;		
#endif
	}
	else
	{
		ddos_debug(DDOS_MID_STORE, "ddos_ip_store_sucess ... \n");
		CA_LOG(LOG_MODULE, LOG_PROC, "ddos_ip_store_sucess ... \n");		
		break;

	}
	}
	if(retn < 0)
	{
		return -1;
	}
	ddos_log_top100_end_store(conn,buf->att_top,buf->query_id);
	ddos_log_top100_5min_store(buf);
	}

	return 0;
}
