/**
 * @file flow_statistics.c
 * @brief  流量数据库查询并统计
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-11
 */

#include "gms_flow.h"


int verbose = 2;

/*
 protocol_id | protocol_en | protocol_cn 
 -------------+-------------+-------------
            1 | ALL         | ALL
		    2 | TCP         | TCP
	        3 | UDP         | UDP
	        4 | OTHERS      | OTHERS
	        5 | FTP         | 文件传输
	        6 | HTTP        | 网站访问
            7 | MAIL        | 邮件
            8 | DNS         | 域名解析
*/

/* gms自定义协议 与 RFC协议映射表 */
unsigned int g_protocol_array[PROTOCOL_TYPE_NUM + 1] = {
	0,	
	PROTOCOL_TYPE_ALL,
	PROTOCOL_TYPE_TCP,
	PROTOCOL_TYPE_UDP,
	PROTOCOL_TYPE_OTHER,
	PROTOCOL_TYPE_FTP,
	PROTOCOL_TYPE_HTTP,
	PROTOCOL_TYPE_MAIL,
	PROTOCOL_TYPE_DNS,
};


static cfg_desc g_flow_time[] = {

	/* critical time */
	{"CRITICAL_TIME", "flow_time", &(g_flow_info_cfg.flow_ctime), TIME_FORMAT_SIZE, CFG_TYPE_STR, 0, 0, DEFAULT_FLOW_TIME},

	{"", "", NULL, 0, 0, 0, 0, ""},
};

/* 动态流量统计策略数目 */
unsigned int g_f_policy_num = 0;
static cfg_desc g_flow_policy_num[] = {
	/* 动态流量统计策略数目 */
	{"num", "total", &(g_f_policy_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 1, 0, "0"},
	{"", "", NULL, 0, 0, 0, 0, ""},
};

/* 一条流量的数据 */
static char g_a_line_fdata[4096];

/* 策略统计链表 */
list_t g_f_policy_list;
pthread_mutex_t g_f_policy_list_mutex = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------*/
/**
 * @brief  打印动态读取的配置文件信息
 */
/* -------------------------------------------*/
int show_flow_custom_conf(flow_custom_conf_t *conf)
{
	unsigned int i = 0;

	for (i = 0; i < g_f_policy_num; ++i) {

		printf("\n==============conf node ==========\n");
		printf("[title] %s\n", conf[i].title);
		printf("[cus_type] %d\n", conf[i].cus_type);
		printf("[is_display] %d\n", conf[i].is_display);
		printf("[protocol_type] %d\n", conf[i].protocol_type);
		printf("[start_ip] %s\n", conf[i].start_ip);
		printf("[end_ip] %s\n", conf[i].end_ip);
		printf("[redis_tname] %s\n", conf[i].redis_tname);
	}
	
	return 0;
}

/* -------------------------------------------*/
/**
 * @brief  打印策略链表
 */
/* -------------------------------------------*/
int show_statistical_node(void *data, void *arg)
{
	f_statistical_t *node = (f_statistical_t*)data;

	printf("\n==============list node ==========\n");
	printf("[policy_type] %d\n", node->policy_type);
	printf("[protocol_type] %d\n", node->protocol_type);
	printf("[start_ip] %s\n", node->start_ip_str);
	printf("[end_ip] %s\n", node->end_ip_str);
	printf("[key] %s\n", node->redis_tname);
	printf("[is_display] %d\n", node->is_display);
	printf("[time_slot] %d\n", node->time_slot);
	printf("[packets] %ld\n", node->packets);
	printf("[bytes] %ld\n", node->bytes);
	printf("[dynamic] %d\n", node->dynamic);

	return 0;
}

/* -------------------------------------------*/
/**
 * @brief  每查询一条流浪数据 根据链表中的策略进行统计
 */
/* -------------------------------------------*/
int do_statistical_node(void *data, void *arg)
{
	f_statistical_t *st_node = (f_statistical_t*)data;	
	flowrec_t *flow_data = (flowrec_t*)arg;

	struct in_addr start_ip;
	struct in_addr end_ip;

	switch (st_node->policy_type) {

		case 1:
			/* 协议统计策略 */

			if ((flow_data->proto == g_protocol_array[st_node->protocol_type])
			|| (st_node->protocol_type == PRO_ALL_ID)) {
				st_node->packets += flow_data->packets;
				st_node->bytes += flow_data->bytes;
			}
			if (flow_data->proto != PROTOCOL_TYPE_TCP &&
				flow_data->proto != PROTOCOL_TYPE_UDP &&
				flow_data->proto != PROTOCOL_TYPE_OTHER &&
				st_node->protocol_type == PRO_OTHERS_ID) {
				
				//printf("proto: %d\n", flow_data->proto);
				st_node->packets += flow_data->packets;
				st_node->bytes += flow_data->bytes;
			}
			break;

		case 2:
			/* ip段统计策略 */

			/* 不论 源ip 或者 目的Ip 在ip范围内均累加流量 */
			inet_aton(st_node->start_ip_str, &start_ip);
			inet_aton(st_node->end_ip_str, &end_ip);


			if (
					(
					 (ip_cmp(flow_data->sip, start_ip.s_addr) >= 0) && (ip_cmp(flow_data->sip, end_ip.s_addr) <= 0)
					) 
					|| 
					(
					 (ip_cmp(flow_data->dip, start_ip.s_addr) >= 0) && (ip_cmp(flow_data->dip, end_ip.s_addr) <= 0)
					)
				) 
			{


#if 0
				printf("sip_str[%u]:"NIPQUAD_FMT"\n", flow_data->sip,NIPQUAD(flow_data->sip));
				printf("dip_str[%u]:"NIPQUAD_FMT"\n", flow_data->dip,NIPQUAD(flow_data->dip));
				printf("start_ip[%u]:"NIPQUAD_FMT"\n", start_ip.s_addr, NIPQUAD(start_ip.s_addr));
				printf("end_ip[%u]:"NIPQUAD_FMT"\n", end_ip.s_addr,NIPQUAD(end_ip.s_addr));
#endif
				st_node->packets += flow_data->packets;
				st_node->bytes += flow_data->bytes;
			}

			break;
		default:
			break;
	}

	return 0;
}


/* -------------------------------------------*/
/**
 * @brief  找到动态策略节点
 */
/* -------------------------------------------*/
int find_dynamic_node(void *data, void *arg)
{
	f_statistical_t *node = (f_statistical_t*)data;
	
	if (node->dynamic == 1) {
		return 1;
	}

	return 0;
}

/* -------------------------------------------*/
/**
 * @brief  为静态节点修正 时间段
 */
/* -------------------------------------------*/
int modify_statistical_node(void *data, void *arg)
{
	f_statistical_t *st_node = (f_statistical_t*)data;
	flow_conn_t *conn = (flow_conn_t*)arg;
	if (st_node->dynamic == 0) {
		st_node->time_slot = conn->time_slot;
		st_node->packets = 0;
		st_node->bytes = 0;
	}

	return 0;
}

/* -------------------------------------------*/
/**
 * @brief  将统计结果写入文件中
 */
/* -------------------------------------------*/
int write_statistical_flow(void *data, void *arg)
{
	flow_conn_t *conn = (flow_conn_t*)arg;
	f_statistical_t *st_node = (f_statistical_t*)data;

	const char *time_format = "%Y-%m-%d %X";	
	char flow_time[TIME_FORMAT_SIZE] = {0};
	struct tm *flow_time_p = NULL;

	char protocol_type = 0;
	double Bps, pps;
	struct in_addr start_ip;
	struct in_addr end_ip;

    struct tm this_time;


	int data_len = 0;
	int succ_write = 0;

	memset(g_a_line_fdata, 0, 4096);

	/* dev_id */

	// g_flow_info_cfg.dev_id

	/* protocol id */
	if (st_node->policy_type == 1) {
		protocol_type = st_node->protocol_type;
	}
	else {
		protocol_type = -1;
	}

	/* flow_time_str */
	flow_time_p = localtime(&(conn->flow_time));
	strftime(flow_time, TIME_FORMAT_SIZE-1, time_format, flow_time_p);	

	/* pps */
	pps = (double)(st_node->packets)/st_node->time_slot;

	/* Bps */
	Bps = (double)(st_node->bytes*10)/(st_node->time_slot*1024*1024);

	/* sip */
	/* dip */
	if (st_node->policy_type == 2) {
		inet_aton(st_node->start_ip_str, &start_ip);
		inet_aton(st_node->end_ip_str, &end_ip);
	}
	else {
		start_ip.s_addr = 0;
		end_ip.s_addr = 0;
	}

    /* get month_id */
	memset(&this_time, 0, sizeof(struct tm));
    strptime(flow_time, time_format, &this_time);

	/* query_id */
	// g_flow_info_cfg.query_id
	
	
	sprintf(g_a_line_fdata, "%s|%d|%s|%ld|%ld|%u|%u|%d|%d\n", 
			g_flow_info_cfg.dev_id, protocol_type, flow_time, (long int)pps, (long int)Bps, 
			htonl(start_ip.s_addr), htonl(end_ip.s_addr), g_flow_info_cfg.query_id, this_time.tm_mon+1);
	printf("%s|%d|%s|%.2lf|%.2lf|%u|%u|%d|%d\n", 
			g_flow_info_cfg.dev_id, protocol_type, flow_time, pps, Bps, 
			htonl(start_ip.s_addr), htonl(end_ip.s_addr), g_flow_info_cfg.query_id, this_time.tm_mon+1);

	data_len = strlen(g_a_line_fdata);

	succ_write = write(conn->flow_fd, g_a_line_fdata, data_len);
	if (succ_write == -1) {
		FLOW_ERROR(DEBUG_TYPE_MAKE, "Write %s data: %s Error!\n", conn->flow_tmp_file, g_a_line_fdata);
		CA_LOG(LOG_MODULE, LOG_PROC, "Write %s data: %s Error!\n", conn->flow_tmp_file, g_a_line_fdata);
		return -1;
	}
	
//	FLOW_DEBUG(DEBUG_TYPE_MAKE, "Write %s data[%d]: %s SUCCESS!\n", conn->flow_tmp_file, succ_write, g_a_line_fdata);

	return 0;
}


/* -------------------------------------------*/
/**
 * @brief  清除所有动态节点
 *
 * @param list
 */
/* -------------------------------------------*/
void clear_dynamic_statistics_list(list_t *list)
{
	int list_num = list->node_num;
	int i = 0;
	f_statistical_t *remove_data = NULL;

	for (i = 0; i < list_num; ++i) {
		remove_data = NULL;
		remove_data = list_iterate(list, LIST_FORWARD, find_dynamic_node, list);
		if (remove_data != NULL) {
			list_remove(list, remove_data);
			free(remove_data);
		}
	}
}


/* -------------------------------------------*/
/**
 * @brief  初始化默认统计策略 
 *
 * @param list
 */
/* -------------------------------------------*/
void init_statistics_list(list_t *list)
{
	f_statistical_t *node = NULL;
	/* 初始化策略统计链表 */
	list_init(&(g_f_policy_list), f_statistical_t);	

	/* 初始化协议统计策略 */

	/* 添加TCP/IP协议统计策略 */
	node = calloc(1, sizeof(f_statistical_t));
	node->policy_type = 1;
	node->protocol_type = PRO_TCP_ID;	
	node->is_display = 1;
	strncpy(node->redis_tname, KEY_EVENT_FLOW_TCP, KEY_NAME_SIZ-1);
	node->dynamic = 0;

	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_enqueue(list, node);	
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	/* 添加UDP协议统计策略 */
	node = calloc(1, sizeof(f_statistical_t));
	node->policy_type = 1;
	node->protocol_type = PRO_UDP_ID;	
	node->is_display = 1;
	strncpy(node->redis_tname, KEY_EVENT_FLOW_UDP, KEY_NAME_SIZ-1);
	node->dynamic = 0;

	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_enqueue(list, node);	
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	/* 添加其他协议统计策略 */
	node = calloc(1, sizeof(f_statistical_t));
	node->policy_type = 1;
	node->protocol_type = PRO_OTHERS_ID;	
	node->is_display = 1;
	strncpy(node->redis_tname, KEY_EVENT_FLOW_OTHER, KEY_NAME_SIZ-1);
	node->dynamic = 0;

	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_enqueue(list, node);	
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	/* 添加全部协议统计策略 */
	node = calloc(1, sizeof(f_statistical_t));
	node->policy_type = 1;
	node->protocol_type = PRO_ALL_ID;	
	node->is_display = 1;
	strncpy(node->redis_tname, KEY_EVENT_FLOW_ALL, KEY_NAME_SIZ-1);
	node->dynamic = 0;

	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_enqueue(list, node);	
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	/* 遍历链表信息 */
	list_iterate(list, LIST_FORWARD, show_statistical_node, NULL);
	
}

/* -------------------------------------------*/
/**
 * @brief  清除所有节点
 *
 * @param list
 */
/* -------------------------------------------*/
void clear_all_statistics_list(list_t *list)
{
	f_statistical_t *node = NULL;
	int i = 0;		
	int list_num = list->node_num;

	pthread_mutex_lock(&g_f_policy_list_mutex);
	for (i = 0; i < list_num; ++i) {
		node = list_pop(list);
		free(node);
	}
	pthread_mutex_unlock(&g_f_policy_list_mutex);
}

/* -------------------------------------------*/
/**
 * @brief  统计流量
 */
/* -------------------------------------------*/
int flow_statistics(void *user, int qid, flowrec_t *rec, int count)
{
	flow_conn_t *conn = (flow_conn_t*)user;

#if 0
	printf("==============+++=======\npackets: %d, bytes : %d\n", (unsigned int)(rec->packets), (unsigned int )(rec->bytes));
    if ((unsigned int)(rec->packets) == 0 || (unsigned int)(rec->bytes) == 0) {
	    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\npackets: %d, bytes : %d\n", (unsigned int)(rec->packets), (unsigned int )(rec->bytes));
    }
#endif

	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_iterate(conn->list, LIST_FORWARD, do_statistical_node, rec);
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	return 0;
}


/* -------------------------------------------*/
/**
 * @brief  初始化流量数据库配置
 *
 * @param conn
 *
 * @returns   
 */
/* -------------------------------------------*/
int init_flow_env(flow_conn_t *conn)
{
	int retn = 0;

	conn->flowdb = flowdb2_open(g_flow_info_cfg.flowdb_ini_path, O_RDONLY);
	if (conn->flowdb == NULL) {
		FLOW_ERROR(DEBUG_TYPE_MAKE, "OPen flowdb Error!\n");
		CA_LOG(LOG_MODULE, LOG_PROC, "OPen flowdb[%s] Error!\n", g_flow_info_cfg.flowdb_ini_path);
		retn = -1;
		goto EXIT;
	}

	conn->qopt.cube_handler = NULL;
	conn->qopt.rec_handler = flow_statistics;

	conn->qattr.nqueue = 1;
	conn->qattr.rate = 1000;
	conn->qattr.sample = 1;
	conn->qattr.fix = 0;

	memset(conn->flow_condition, 0, 64);

EXIT:
	return retn;
}

int create_flow_cfg_array(cfg_desc* desc, flow_custom_conf_t* conf, unsigned int conf_num)
{
	unsigned int i = 0;
	int j = 0;

	/* title */
	for (i = 0; i < conf_num; ++i) {
		snprintf(conf[i].title, 100,"custom%d", i+1);
	}

	for (i = 0, j = 0; i < g_f_policy_num; ++i) {
		/* cus_type */
		desc[j].entry_title = conf[i].title;
		desc[j].key_name = "cus_type";
		desc[j].addr = &(conf[i].cus_type);
		desc[j].maxlen = CFG_SIZE_UINT8;
		desc[j].type = CFG_TYPE_UINT8;
		desc[j].compulsive = 1;
		desc[j].valid = 0;
		desc[j].initial_value = "1";
		j++;

		/* is_display */
		desc[j].entry_title = conf[i].title;
		desc[j].key_name = "is_display";
		desc[j].addr = &(conf[i].is_display);
		desc[j].maxlen = CFG_SIZE_UINT8;
		desc[j].type = CFG_TYPE_UINT8;
		desc[j].compulsive = 0;
		desc[j].valid = 0;
		desc[j].initial_value = "1";
		j++;

		/* protocol_type */
		desc[j].entry_title = conf[i].title;
		desc[j].key_name = "protocol_type";
		desc[j].addr = &(conf[i].protocol_type);
		desc[j].maxlen = CFG_SIZE_UINT8;
		desc[j].type = CFG_TYPE_UINT8;
		desc[j].compulsive = 0;
		desc[j].valid = 0;
		desc[j].initial_value = "1";
		j++;

		/* start_ip */
		desc[j].entry_title = conf[i].title;
		desc[j].key_name = "start_ip";
		desc[j].addr = &(conf[i].start_ip);
		desc[j].maxlen = IP_STR_SIZE;
		desc[j].type = CFG_TYPE_STR;
		desc[j].compulsive = 0;
		desc[j].valid = 0;
		desc[j].initial_value = "0.0.0.0";
		j++;

		/* end_ip */
		desc[j].entry_title = conf[i].title;
		desc[j].key_name = "end_ip";
		desc[j].addr = &(conf[i].end_ip);
		desc[j].maxlen = IP_STR_SIZE;
		desc[j].type = CFG_TYPE_STR;
		desc[j].compulsive = 0;
		desc[j].valid = 0;
		desc[j].initial_value = "0.0.0.0";
		j++;

		/* redis_tname */
		desc[j].entry_title = conf[i].title;
		desc[j].key_name = "redis_tname";
		desc[j].addr = &(conf[i].redis_tname);
		desc[j].maxlen = KEY_NAME_SIZ;
		desc[j].type = CFG_TYPE_STR;
		desc[j].compulsive = 0;
		desc[j].valid = 0;
		desc[j].initial_value = "";
		j++;
	}

	/* NULL */
	desc[j].entry_title = "";
	desc[j].key_name = "";
	desc[j].addr = NULL;
	desc[j].maxlen = 0;
	desc[j].type = 0;
	desc[j].compulsive = 0;
	desc[j].valid = 0;
	desc[j].initial_value = "";
}

int make_flow_file(flow_conn_t *conn)
{
	const char *time_format = "%Y%m%d-%H%M%S";	
	char file_time[TIME_FORMAT_SIZE] = {0};
	struct tm *file_time_p = NULL;
	int retn = 0;

	/* 制作文件名称 */
	file_time_p = localtime(&(conn->flow_time));
	strftime(file_time, TIME_FORMAT_SIZE-1, time_format, file_time_p);	
	if (g_flow_debug_cfg.flowevent_store_disenable==1){
	strcpy(conn->flow_file, g_flow_info_cfg.dst_file_path);
	strncat(conn->flow_file, "/", 2);
	strcpy(conn->flow_tmp_file, g_flow_info_cfg.dst_file_path);
	strncat(conn->flow_tmp_file, "/", 2);

	}
	else{
	strcpy(conn->flow_file, g_flow_info_cfg.src_file_path);
	strncat(conn->flow_file, "/", 2);
	strcpy(conn->flow_tmp_file, g_flow_info_cfg.src_file_path);
	strncat(conn->flow_tmp_file, "/", 2);
	}

	strcat(conn->flow_file, file_time);
    strncat(conn->flow_file, "_", 2);
    strcat(conn->flow_file, g_flow_info_cfg.dev_id);
	strcat(conn->flow_tmp_file, file_time);
    strncat(conn->flow_tmp_file, "_", 2);
    strcat(conn->flow_tmp_file, g_flow_info_cfg.dev_id);

	strcat(conn->flow_file, ".ok");
	strcat(conn->flow_tmp_file, ".ok.writing");
    printf("\nflow tmp file :[%s], flow_file is [%s]\n", conn->flow_tmp_file, conn->flow_file);

	/* 打开文件 */
	conn->flow_fd = open(conn->flow_tmp_file, O_RDWR|O_CREAT|O_TRUNC, 
							S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);	
	if (conn->flow_fd == -1) {
		FLOW_ERROR(DEBUG_TYPE_MAKE, "*******Open and Create %s Error!", conn->flow_tmp_file);
		CA_LOG(LOG_MODULE, LOG_PROC, "*******Open and Create %s Error!", conn->flow_tmp_file);
		retn = -1;
		goto END;
	}

	/* 写文件 */
	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_iterate(conn->list, LIST_FORWARD, write_statistical_flow, conn);
	pthread_mutex_unlock(&g_f_policy_list_mutex);
	
	/* 关闭文件 */
	close(conn->flow_fd);
	/* 将文件重命名为.ok */
	rename(conn->flow_tmp_file, conn->flow_file);
END:
	return retn;		
}

int do_flow_statistics(flow_conn_t *conn)
{
	int retn = 0;
	unsigned int i = 0;

	/* 读取配置文件 */
	retn = common_read_conf(g_flow_info_cfg.custom_flow_path, g_flow_policy_num);
	if (retn < 0) {
		FLOW_ERROR(DEBUG_TYPE_MAKE, "Read config %s ERROR", g_flow_info_cfg.custom_flow_path);
		CA_LOG(LOG_MODULE, LOG_PROC, "Read config %s ERROR", g_flow_info_cfg.custom_flow_path);
		retn = -1;
		goto END;
	}

	/* I. 生成读取配置文件规则 */

	/* 其中"6" 为 flow_custom_conf 除了title 有多少成员 */
	cfg_desc * cfg_array = calloc(g_f_policy_num*6 + 1, sizeof(cfg_desc));	
	flow_custom_conf_t *f_custom_conf = calloc(g_f_policy_num, sizeof(flow_custom_conf_t));
	create_flow_cfg_array(cfg_array, f_custom_conf, g_f_policy_num);

	/* II. 读取配置文件到配置文件链表中 */
	retn = common_read_conf(g_flow_info_cfg.custom_flow_path, cfg_array);
	if (retn < 0) {
		FLOW_ERROR(DEBUG_TYPE_MAKE, "Read config %s ERROR", g_flow_info_cfg.custom_flow_path);
		CA_LOG(LOG_MODULE, LOG_PROC, "Read config %s ERROR", g_flow_info_cfg.custom_flow_path);
		retn = -1;
		goto END;
	}

	//show_flow_custom_conf(f_custom_conf);

	/* 对比配置文件链表节点 和 链表动态节点<redis入库可直接读取此节点> , 并进行整理 */
	/* I. 删除所有 动态策略节点 */
	pthread_mutex_lock(&g_f_policy_list_mutex);
	clear_dynamic_statistics_list(conn->list);
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	/* II. 为静态节点修正 时间段 和 清零流量累计 */
	pthread_mutex_lock(&g_f_policy_list_mutex);
	list_iterate(conn->list, LIST_FORWARD, modify_statistical_node, conn);
	pthread_mutex_unlock(&g_f_policy_list_mutex);

	/* III. 将配置文件链表中所有动态节点加入进来 */
	for (i = 0; i < g_f_policy_num; ++i) {
		f_statistical_t *st_node = calloc(1, sizeof(f_statistical_t));
		/* 流量策略类型 */
		st_node->policy_type = f_custom_conf[i].cus_type;			
		if (st_node->policy_type == 1) {
			/* 协议策略 */
			st_node->protocol_type = f_custom_conf[i].protocol_type;
		}
		else if (st_node->policy_type == 2) {
			/* 流量策略 */
			strncpy(st_node->start_ip_str, f_custom_conf[i].start_ip, IP_STR_SIZE);
			strncpy(st_node->end_ip_str, f_custom_conf[i].end_ip, IP_STR_SIZE);
		}
		/* 是否实时呈现 */
		st_node->is_display = f_custom_conf[i].is_display;

		/* redis 表名 */
		strncpy(st_node->redis_tname, f_custom_conf[i].redis_tname, KEY_NAME_SIZ);

		/* 统计时间段 */
		st_node->time_slot = conn->time_slot;

		/* 统计数据包总数  */
		st_node->packets = 0;
		/* 统计字节流总数 */
		st_node->bytes = 0;

		/* 是否动态添加 1 是 0 否*/
		st_node->dynamic = 1;
		
		pthread_mutex_lock(&g_f_policy_list_mutex);
		list_enqueue(conn->list, st_node);
		pthread_mutex_unlock(&g_f_policy_list_mutex);
	}
	
	/* 执行查询流量库指令，并调用回掉进行统计 */
	conn->flow_query = flowquery_commit(conn->flowdb, conn->flow_condition, 0, conn->qattr, &(conn->qopt), conn, NULL);
	flowquery_wait(conn->flow_query, NULL);

#if 0
	printf("\n----------------------------- After !!--------------------\n");
	list_iterate(conn->list, LIST_FORWARD, show_statistical_node, NULL);
	printf("\n----------------------------------------------------------\n");
#endif

	/* 遍历统计链表 得到统计结果，生成流量PostgreSQL数据文件 */
	make_flow_file(conn);	


	free(cfg_array);
	free(f_custom_conf);
END:
	return retn;
}

void *flow_make_busi(void *arg)
{
	pthread_t tid;
	tid = pthread_self();

    if (g_flow_debug_cfg.statistics_enable == 0) {
        fprintf(stderr, "STATISTICS_ENABLE = NO!\n");
        CA_LOG(LOG_MODULE, LOG_PROC, "STATISTICS_ENABLE = NO!");
	    pthread_exit((void*)tid);
    }

	int flow_time = NEED_FLOW_TIME_SEC;
	char now_time[TIME_FORMAT_SIZE];
	char select_time[TIME_FORMAT_SIZE];
	int time_x = 0;
	const char *time_format = "%Y%m%d%H%M%S";

	flow_conn_t flow_conn;
	flow_conn.list = &g_f_policy_list;

	int retn = 0;

	/* ------------- 初始化配置信息 ------------------- */
	retn = init_flow_env(&flow_conn);	
	if (retn < 0) {
		goto EXIT_PROC;
	}


	/* 开始处理业务 */
	while (1) {

        /* 读取通讯配置文件 看是否是检测节点 */
        retn = read_comm_interface_conf(g_flow_info_cfg.comm_conf_path, g_flow_info_cfg.is_comm_monitor);
        if (retn < 0) {
            FLOW_ERROR(DEBUG_TYPE_MAKE, "Parser COMM XML FILE ERROR!!!!");
            CA_LOG(LOG_MODULE, LOG_PROC, "Parser COMM XML FILE ERROR!!!!");
            continue;
        }
#if 0
        if (strncmp(g_flow_info_cfg.is_comm_monitor, "1", 2) != 0) {
            printf("dev is not monitor Do not make flow file!\n");
            sleep(1);
            continue;
        }
#endif
        memset(g_flow_info_cfg.flow_ctime, 0, TIME_FORMAT_SIZE);
		/* 读取最后读取时间 */
		retn = common_read_conf(FLOW_TIME_CONFIG_PATH, g_flow_time);
		if (retn < 0) {
			FLOW_ERROR(DEBUG_TYPE_MAKE, "Read config %s ERROR", FLOW_TIME_CONFIG_PATH);
			CA_LOG(LOG_MODULE, LOG_PROC, "Read config %s ERROR", FLOW_TIME_CONFIG_PATH);
			goto EXIT_1;
		}

        /* flow_time.conf文件为空 读出的为默认事件 */
        if (strcmp(g_flow_info_cfg.flow_ctime, DEFAULT_FLOW_TIME) == 0) {
            /* 使读到的事件改为 当前系统时间 */
            get_systime_str_format_offset(g_flow_info_cfg.flow_ctime, time_format, -60);
			//write_conf(FLOW_TIME_CONFIG_PATH, "CRITICAL_TIME", "flow_time", NULL, g_flow_info_cfg.flow_ctime);
            
            FILE *flow_time_fp = NULL;
            flow_time_fp = fopen(FLOW_TIME_CONFIG_PATH, "w");
            if (flow_time_fp == NULL) {
			    FLOW_ERROR(DEBUG_TYPE_MAKE, "Open config %s ERROR", FLOW_TIME_CONFIG_PATH);
			    CA_LOG(LOG_MODULE, LOG_PROC, "Open config %s ERROR", FLOW_TIME_CONFIG_PATH);
                goto EXIT_1;
            }

            retn = cfg_write(flow_time_fp, g_flow_time);
            if (retn < 0) {
			    FLOW_ERROR(DEBUG_TYPE_MAKE, "Write config %s ERROR", FLOW_TIME_CONFIG_PATH);
			    CA_LOG(LOG_MODULE, LOG_PROC, "Write config %s ERROR", FLOW_TIME_CONFIG_PATH);
            }

            fclose(flow_time_fp);
        }

		/* 比较当前时间和最后读取时间 */
		get_systime_str_format_offset(now_time, time_format, -60);
		time_x = time_cmp_format(now_time, g_flow_info_cfg.flow_ctime, time_format);

        if (time_x < 0) {
            /* 当前时间小于最后处理时间 更新最后处理时间为当前时间 */
			get_critical_timestr_offset(select_time, now_time, NEED_FLOW_TIME_SEC, time_format);
            write_conf(FLOW_TIME_CONFIG_PATH, "CRITICAL_TIME", "flow_time", NULL, select_time);
        }

		if (time_x >= flow_time) {
            /* 当前时间大于最后处理时间 进行流量统计 */

			/* 取分钟的整数点时间 */
			get_critical_timestr_offset(select_time, g_flow_info_cfg.flow_ctime, NEED_FLOW_TIME_SEC, time_format);

			FLOW_DEBUG(DEBUG_TYPE_MAKE, "\n====>[FLOW]\n====>begin_time:[%s], end_time:[%s]\n", g_flow_info_cfg.flow_ctime, select_time);
			CA_LOG(LOG_MODULE, LOG_PROC, "\n====>[FLOW]\n====>begin_time:[%s], end_time:[%s]\n", g_flow_info_cfg.flow_ctime, select_time);

			/* 设置流量库查询条件 */
			sprintf(flow_conn.flow_condition, "time %s-%s", g_flow_info_cfg.flow_ctime, select_time);

			/* 设置查询条件的时间段（多少秒） */
			time_x = time_cmp_format(select_time, g_flow_info_cfg.flow_ctime, time_format);
			/* 重置生成流量数据文件信息 */
			flow_conn.time_slot = time_x;
			memset(flow_conn.flow_file, 0, FILENAME_MAX);
			memset(flow_conn.flow_tmp_file, 0, FILENAME_MAX);
			flow_conn.flow_time = get_time_sec_by_str(select_time, time_format);

			/* 开始统计业务 */
			do_flow_statistics(&flow_conn);

			/* 更新最后读取时间 */
			write_conf(FLOW_TIME_CONFIG_PATH, "CRITICAL_TIME", "flow_time", NULL, select_time);

		}

		usleep(50000);
	}


EXIT_1:
	/* 关闭flowdb 链接 */
	flowdb2_close(flow_conn.flowdb);

	pthread_exit((void*)tid);

EXIT_PROC:
    fprintf(stderr, "EXIT FLOW_PROC!");
    CA_LOG(LOG_MODULE, LOG_PROC, "EXIT FLOW_PROC!");
    exit(1);
}

