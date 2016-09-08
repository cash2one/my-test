#ifndef __TCAD_H__
#define __TCAD_H__

#define _GNU_SOURCE
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libxml/xmlmemory.h> 
#include <libxml/parser.h> 
#define BUF_SIZE 				1024
#define SAM_STR_SIZE            31
#define TCAD_SRV_PORT           22222       // TCAD接收消息端口
#define GMS_SRV_PORT            33333       // 关口接收消息端口
#define SERV_LOCAL              0x7f000001
#define XML_FILE_PATH 			"/gms/conf/gmsconfig.xml"
#define IP_LOCATION_CONF_FILE_PATH  "/gms/conf/ip_location_list.conf"
#define IP_ADDR_LIST_BUF_LEN    400

/* TCAD处理返回码 */
typedef enum { 
    TCAD_RETCODE_OK = 0,                    /* 处理成功 */
    TCAD_RETCODE_FAILED,                    /* 处理失败 */
    TCAD_RETCODE_FLOW_EXIST,                /* 自定义流量已存在 */
    TCAD_RETCODE_FLOW_NOT_EXIST,            /* 自定义流量不存在 */
} TCAD_RETCODE;

/* 动作定义 */
typedef enum { 
    TCAD_ACTION_FLOW_ADD = 1,               /* 添加自定义流量 */
    TCAD_ACTION_FLOW_DEL,                   /* 删除自定义流量 */
    TCAD_ACTION_SET_FLOW_INTERVAL,          /* 设置自定义流量日志上报时间间隔 */
    TCAD_ACTION_SET_INCR_INTERVAL,          /* 设置长链接增量日志上报时间间隔 */
	TCAD_ACTION_SET_UDP_SERVER_IP,			/* 设置接收流量日志的服务器IP地址 */
    TCAD_ACTION_BATCH,                      /* 批量备份 */
	TCAD_ACTION_RESTORE_ADD,                /* 添加协议还原配置 */
    TCAD_ACTION_RESTORE_DEL,                /* 删除协议还原配置 */
    TCAD_ACTION_RESTORE_MOD,                /* 修改协议还原配置 */
	TCAD_ACTION_IPINFO_ADD,                 /* 添加IP地址定位信息 */
    TCAD_ACTION_IPINFO_DEL,                 /* 删除IP地址定位信息 */
} TCAD_ACTION;



/* */
typedef struct tag_TCAD_ACTION
{
    uint32_t action;
} tcad_action_t;

typedef struct tag_TCAD_RETCODE
{
    uint32_t retcode;
} tcad_retcode_t;

/* 用户自定义流量 */
typedef struct flow_defined_struct{
    uint8_t     action;             /* 动作标识 */
    uint32_t    start_ip;           /* 起始IP地址 */
    uint32_t    end_ip;             /* 结束IP地址 */
}flow_defined;

/* 设置自定义流量日志上报时间间隔 */
typedef struct flow_interval_struct{
    uint8_t     action;             /* 动作标识 */
    uint32_t    itime;              /* 间隔时间，单位秒 */
}flow_interval;

/* 设置长链接增量日志上报时间间隔 */
typedef struct incr_interval_struct{
    uint8_t     action;             /* 动作标识 */
    uint32_t    itime;              /* 间隔时间，单位秒 */
}incr_interval;
typedef struct param{
	uint8_t	flag;
    char	start_ip[16];
	char	end_ip[16];	
}param_struct;
/* 设置接收流量日志的服务器IP地址 */
typedef struct udp_server_struct{
	    uint8_t     action;             /* 动作标识 */
		uint32_t    server_ip;          /* UDP服务器IP地址，主机字节序 */
}udp_server_t;
/* 用户配置协议还原 */
typedef struct flow_restore_struct{
	uint8_t action;
	char    name[32];
	char	buf[BUF_SIZE];
}flow_restore;
typedef struct restore_struct{
	uint8_t action;
	char	name[BUF_SIZE];
	char 	udp[BUF_SIZE];
	char 	tcp[BUF_SIZE];
}restore_char;
/* IP地址定位信息 */
typedef struct ip_info_tag_s
{
    uint8_t action;         /*动作标识*/
	uint8_t country;        /* 国家 */
//	uint8_t res;            /* 保留 */
	uint16_t isp_id;        /* 运营商ID */
    uint32_t ip_start;      /* 起始IP地址 */
    uint32_t ip_end;        /* 结束IP地址 */
	uint32_t province_id;   /* 省、直辖市、自治区ID */
	uint32_t city_id;       /* 地市ID */
}ip_info_t;
extern char ret_buf[TCAD_RETCODE_FLOW_NOT_EXIST+2][20];

int gms_socket_init();
int gms_send_rcv(int sockfd, void *data, int length, char *task_buf, int task_len);
void parseDoc(char *docname,char *tagone,char *tagtwo);
int parseconf(char *path,char *act);
char *hextochs ( char* ascii );
#endif
