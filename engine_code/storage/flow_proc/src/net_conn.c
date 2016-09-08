#include "gms_flow.h"
#include "net_conn.h"
/* 
 * @param struct gpq_conn_info
 *
 * @return
 * 		 	postgresql's socket
 * */

static PGconn *net_conn; 

 PGconn * net_connect_db(void)
{
	PGconn *ddos_conn;

	if (g_flow_info_cfg.psql_unix == 1) {
		ddos_conn = gpq_connectdb_host(g_flow_info_cfg.psql_domain,
				g_flow_info_cfg.psql_dbname, g_flow_info_cfg.psql_user,
				g_flow_info_cfg.psql_pwd);    
	}   
	else {
		ddos_conn = gpq_connectdb(g_flow_info_cfg.psql_ip, g_flow_info_cfg.psql_port,    
				g_flow_info_cfg.psql_dbname, g_flow_info_cfg.psql_user,    
				g_flow_info_cfg.psql_pwd); 

	}
	gpq_sql_cmd(ddos_conn, "set standard_conforming_strings=on"); 
	return ddos_conn;
}
void time_int_to_char(time_t timeint,char *str_time)
{
	char time_format[]="%Y-%m-%d %X";
	struct tm timetmp[1];
	localtime_r(&timeint,timetmp);
	strftime(str_time, TIME_FORMAT_SIZE-1, time_format, timetmp);
}
/** 
 * 上网账号init
 * 创建postgresql的句柄，suc后
 * 赋值给全局变量net_conn 
 * return 
 * 		suc 0
 * 		fail -1
 * */

int  init_net_store(void)
{

	net_conn=net_connect_db();
	if (net_conn == NULL)
	{
	 	return -1;	
	}
	return 0;
}
/**
 *
 *
 * **/
int if_found_in_table(char *id)
{
	int retval=0;
	char sql_cmd[NET_CMD_SIZE];
	memset(sql_cmd,0,NET_CMD_SIZE);
	snprintf(sql_cmd,NET_CMD_SIZE,"select user_name from %s where user_name = '%s'",NET_TABLE,id);
CONN_ROW:
	retval=gpq_get_row(net_conn,sql_cmd);
	if(retval == -1)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "gpq_get_row select user_name from t_net_id where user_name FIAL !	... \n");
		gpq_disconnect(net_conn);
		net_conn=net_connect_db();
		if(net_conn == NULL)
		{
			CA_LOG(LOG_MODULE, LOG_PROC, "row net_conn=net_connect_db try conn ERROR! ... \n");
			usleep(100000);
			goto CONN_ROW;
		}
	}
	return retval;	

}	
int update_net_table(char *account,unsigned int ip, char *timeval)
{
	int retval=0;
	char sql_cmd[NET_CMD_SIZE];
	memset(sql_cmd,0,NET_CMD_SIZE);
	snprintf(sql_cmd,NET_CMD_SIZE,"update %s set net_time='%s',user_ip=%u where user_name = '%s'",NET_TABLE,timeval,ip,account);
CONN_UPDATE:
	retval=gpq_sql_cmd(net_conn,sql_cmd);
	if(retval < 0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "update t_net_id FAIL! ... \n");
		gpq_disconnect(net_conn);
		net_conn=net_connect_db();
		if(net_conn == NULL)
		{
			usleep(100000);
			CA_LOG(LOG_MODULE, LOG_PROC, "update net_conn=net_connect_db try conn ERROR! ... \n");
			goto CONN_UPDATE;
		}
	}
	return retval;	

}
uint32_t jenkins_one_at_a_time_hash(char *key, size_t len)
{
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash%128;
}

int insert_net_table(char *account, unsigned int ip,char *timeval)
{
	int retval=0;
	int status=1;
	char sql_cmd[NET_CMD_SIZE];
	memset(sql_cmd,0,NET_CMD_SIZE);
	snprintf(sql_cmd,NET_CMD_SIZE,"insert into %s (user_name,user_id,net_time,user_ip,status,datacreatetype,createuser) values ('%s',%u,'%s',%u,%d,3,9)",NET_TABLE,account,jenkins_one_at_a_time_hash(account,4),timeval,ip,status);
CONN_INSERT:
	retval=gpq_sql_cmd(net_conn,sql_cmd);
	if(retval < 0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "insert t_net_id FAIL! ... \n");
		gpq_disconnect(net_conn);
		net_conn=net_connect_db();
		if(net_conn == NULL)
		{
			usleep(100000);
			CA_LOG(LOG_MODULE, LOG_PROC, "insert net_conn=net_connect_db try conn ERROR! ... \n");
			goto CONN_INSERT;
		}
	}
	return retval;	

}
/***
 * 上网账号和时间入库
 * @armp
 * 		account 账号
 * 		time	时间
 *
 * return 
 * 		suc 0
 * 		fail -1
 * */
int net_store_per_one(char *account, unsigned int ip,char *timeval)
{
	//ret=char_to_int("qwewer");
	//ret&0x7f
	int found_flag=0;
	//int  stu_id=atol(account)%128;
	//snprintf(table_name,NET_CMD_SIZE,"%s_%d",NET_TABLE,stu_id);
	found_flag = if_found_in_table(account);
	if (found_flag > 0 )
	{
		update_net_table(account,ip,timeval);
	}
	else
	{
		insert_net_table(account,ip,timeval);
	
	}
	return 0;

}
