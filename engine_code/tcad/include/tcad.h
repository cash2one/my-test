#ifndef __TCAD_H__
#define __TCAD_H__

#define BUF_SIZE                1024
#define SAM_STR_SIZE            31
#define BROWSER_STR_SIZE        255
#define TCAD_SRV_PORT           22222       // TCAD接收消息端口
#define GMS_SRV_PORT            33333       // 关口接收消息端口
#define SERV_LOCAL              0x7f000001
#define DEFINE_FLOW_PORT        0xDCBA      // 关口接收流量日志的端口
#define FLOW_EVENT_PORT         0xDCBB      // 关口接收流量日志的端口
#define DEFAULT_INTERVAL_FLOW   60          // 1分钟
#define DEFAULT_INTERVAL_INCR   300        // 30分钟
#define PCAP_FILE_DIR           "/data/tmpdata/engine/restore/"

/* flow的字段结构体 */
typedef struct flow_struct{
    uint8_t     protocol_id;        /* 协议id 1 代表ALL 2代表TCP 3代表UDP 4代表OTHERS */
    time_t      ftime;              /* 时间以秒为单位 */
    uint64_t    pps;                /* 收包数 */
    uint64_t    bps;                /* 字节数 */
    uint32_t    start_ip;           /* 除自定义流量外,其他为0 */
    uint32_t    end_ip;             /* 除自定义流量外,其他为0 */
}*Pflows,flows;

/* flow_event的字段结构体 */
typedef struct event_struct{
	time_t          ftime_start;            /* 开始时间，自1970.1.1年以来的秒数 */
	time_t          ftime_end;              /* 结束时间，自1970.1.1年以来的秒数 */
	time_t          visit_time;             /* 访问时长，单位秒 */
	uint32_t        sip;                    /* 用户ip */
	uint32_t        dip;                    /* 目的ip */
	uint16_t        usersport;              /* 用户源端口号 */
	uint16_t        userdport;              /* 目的端口号 */
	uint16_t        procotol_id;            /* 应用协议id */
	uint8_t         country;                /* 国家 */
	uint8_t         pc_if;                  /* 0 为pc端 ；1 为移动端 */
	uint8_t         tcp_suc;                /* tcp连接是否成功 0 tcp连接失败，1为连接成功 */
	uint8_t         bank_visit;             /* 业务访问是否成功：如：http 返回200认为成功 */
	uint8_t         flag;                   /* 0为非长链接，1 为长连接 */
	uint8_t         incr;                   /* 0表示增量日志，1表示老化 */
	uint16_t        s_isp_id;               /* 源运营商ID */
	uint16_t        d_isp_id;               /* 目的运营商ID */
	uint32_t        s_province_id;          /* 源省ID */
	uint32_t        d_province_id;          /* 目的省ID */
	uint32_t        s_city_id;              /* 源地市ID */
	uint32_t        d_city_id;              /* 目的地市ID */
	uint64_t        ip_byte;                /* 字节数 */
	uint8_t         direction_flag;         
	uint64_t        up_bytes;              
	uint64_t        down_bytes;             
	uint16_t        service_group_id;       
	uint32_t        service_type_id;      
	uint64_t        ip_pkt;                 /* 数据包 */
	time_t          cli_ser;                /* 端到端的时延:所有包的平均值，单位毫秒 */
	//unsigned char   browser[SAM_STR_SIZE+1];/* 浏览器类型，例：IE、chrome 等 */
	int             browser_id;             /* 浏览器类型ID */
	unsigned char   version[SAM_STR_SIZE+1];/* 浏览器版本 */
}*Pflow_event,flow_event;

/* TCAD处理返回码 */
typedef enum { 
	TCAD_RETCODE_OK = 0,                    /* 处理成功 */
	TCAD_RETCODE_FAILED,                    /* 处理失败 */
	TCAD_RETCODE_FLOW_EXIST,                /* 自定义流量已存在 */
	TCAD_RETCODE_FLOW_NOT_EXIST,            /* 自定义流量不存在 */
	TCAD_RETCODE_IPINFO_EXIST,              /* IP地址定位信息已存在 */
	TCAD_RETCODE_IPINFO_NOT_EXIST,          /* IP地址定位信息不存在 */
} TCAD_RETCODE;

/* 动作定义 */
typedef enum { 
	TCAD_ACTION_FLOW_ADD = 1,               /* 添加自定义流量 */
	TCAD_ACTION_FLOW_DEL,                   /* 删除自定义流量 */
	TCAD_ACTION_SET_FLOW_INTERVAL,          /* 设置自定义流量日志上报时间间隔 */
	TCAD_ACTION_SET_INCR_INTERVAL,          /* 设置长链接增量日志上报时间间隔 */
	TCAD_ACTION_SET_UDP_SERVER_IP,          /* 设置接收流量日志的服务器IP地址 */
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
	uint32_t    start_ip;           /* 起始IP地址，主机字节序 */
	uint32_t    end_ip;             /* 结束IP地址，主机字节序 */
}flow_defined_t;

/* 设置自定义流量日志上报时间间隔 */
typedef struct flow_interval_struct{
	uint8_t     action;             /* 动作标识 */
	uint32_t    itime;              /* 间隔时间，单位秒 */
}flow_interval_t;

/* 设置长链接增量日志上报时间间隔 */
typedef struct incr_interval_struct{
	uint8_t     action;             /* 动作标识 */
	uint32_t    itime;              /* 间隔时间，单位秒 */
}incr_interval_t;

/* 设置接收流量日志的服务器IP地址 */
typedef struct udp_server_struct{
	uint8_t     action;             /* 动作标识 */
	uint32_t    server_ip;          /* UDP服务器IP地址，主机字节序 */
}udp_server_t;

/* 用户配置协议还原 */
typedef struct flow_restore_struct{
	uint8_t action;                    /* 动作标识 */
	char    name[SAM_STR_SIZE+1];
	char    buf[BUF_SIZE];            /* 内容格式为:<START>\nNAME:xxx\n[UDP]\n{b|0|0x00}{b|1|0x01}{b|2|0x00}{b|3|0x00}\n[TCP]\n{o|0|[0x3c,0x3d,0x29,0x39,0x3a,0x29,0x33,0x36]}{b|1|0x00}{b|2|0x00}{b|3|0x00}\n<END> */
}flow_restore_t;

/* IP地址定位信息，主机字节序 */
typedef struct ip_info_tag_s
{
	uint8_t action;
	uint8_t country;        /* 国家 */
	uint16_t isp_id;        /* 运营商ID */
	uint32_t ip_start;      /* 起始IP地址 */
	uint32_t ip_end;        /* 结束IP地址 */
	uint32_t province_id;   /* 省、直辖市、自治区ID */
	uint32_t city_id;       /* 地市ID */
}ip_info_t;

#endif
