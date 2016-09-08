#ifndef __TCAD_H__
#define __TCAD_H__

#define BUF_SIZE                1024
#define SAM_STR_SIZE            31
#define BROWSER_STR_SIZE        255
#define TCAD_SRV_PORT           22222       // TCAD������Ϣ�˿�
#define GMS_SRV_PORT            33333       // �ؿڽ�����Ϣ�˿�
#define SERV_LOCAL              0x7f000001
#define DEFINE_FLOW_PORT        0xDCBA      // �ؿڽ���������־�Ķ˿�
#define FLOW_EVENT_PORT         0xDCBB      // �ؿڽ���������־�Ķ˿�
#define DEFAULT_INTERVAL_FLOW   60          // 1����
#define DEFAULT_INTERVAL_INCR   300        // 30����
#define PCAP_FILE_DIR           "/data/tmpdata/engine/restore/"

/* flow���ֶνṹ�� */
typedef struct flow_struct{
    uint8_t     protocol_id;        /* Э��id 1 ����ALL 2����TCP 3����UDP 4����OTHERS */
    time_t      ftime;              /* ʱ������Ϊ��λ */
    uint64_t    pps;                /* �հ��� */
    uint64_t    bps;                /* �ֽ��� */
    uint32_t    start_ip;           /* ���Զ���������,����Ϊ0 */
    uint32_t    end_ip;             /* ���Զ���������,����Ϊ0 */
}*Pflows,flows;

/* flow_event���ֶνṹ�� */
typedef struct event_struct{
	time_t          ftime_start;            /* ��ʼʱ�䣬��1970.1.1������������ */
	time_t          ftime_end;              /* ����ʱ�䣬��1970.1.1������������ */
	time_t          visit_time;             /* ����ʱ������λ�� */
	uint32_t        sip;                    /* �û�ip */
	uint32_t        dip;                    /* Ŀ��ip */
	uint16_t        usersport;              /* �û�Դ�˿ں� */
	uint16_t        userdport;              /* Ŀ�Ķ˿ں� */
	uint16_t        procotol_id;            /* Ӧ��Э��id */
	uint8_t         country;                /* ���� */
	uint8_t         pc_if;                  /* 0 Ϊpc�� ��1 Ϊ�ƶ��� */
	uint8_t         tcp_suc;                /* tcp�����Ƿ�ɹ� 0 tcp����ʧ�ܣ�1Ϊ���ӳɹ� */
	uint8_t         bank_visit;             /* ҵ������Ƿ�ɹ����磺http ����200��Ϊ�ɹ� */
	uint8_t         flag;                   /* 0Ϊ�ǳ����ӣ�1 Ϊ������ */
	uint8_t         incr;                   /* 0��ʾ������־��1��ʾ�ϻ� */
	uint16_t        s_isp_id;               /* Դ��Ӫ��ID */
	uint16_t        d_isp_id;               /* Ŀ����Ӫ��ID */
	uint32_t        s_province_id;          /* ԴʡID */
	uint32_t        d_province_id;          /* Ŀ��ʡID */
	uint32_t        s_city_id;              /* Դ����ID */
	uint32_t        d_city_id;              /* Ŀ�ĵ���ID */
	uint64_t        ip_byte;                /* �ֽ��� */
	uint8_t         direction_flag;         
	uint64_t        up_bytes;              
	uint64_t        down_bytes;             
	uint16_t        service_group_id;       
	uint32_t        service_type_id;      
	uint64_t        ip_pkt;                 /* ���ݰ� */
	time_t          cli_ser;                /* �˵��˵�ʱ��:���а���ƽ��ֵ����λ���� */
	//unsigned char   browser[SAM_STR_SIZE+1];/* ��������ͣ�����IE��chrome �� */
	int             browser_id;             /* ���������ID */
	unsigned char   version[SAM_STR_SIZE+1];/* ������汾 */
}*Pflow_event,flow_event;

/* TCAD�������� */
typedef enum { 
	TCAD_RETCODE_OK = 0,                    /* ����ɹ� */
	TCAD_RETCODE_FAILED,                    /* ����ʧ�� */
	TCAD_RETCODE_FLOW_EXIST,                /* �Զ��������Ѵ��� */
	TCAD_RETCODE_FLOW_NOT_EXIST,            /* �Զ������������� */
	TCAD_RETCODE_IPINFO_EXIST,              /* IP��ַ��λ��Ϣ�Ѵ��� */
	TCAD_RETCODE_IPINFO_NOT_EXIST,          /* IP��ַ��λ��Ϣ������ */
} TCAD_RETCODE;

/* �������� */
typedef enum { 
	TCAD_ACTION_FLOW_ADD = 1,               /* ����Զ������� */
	TCAD_ACTION_FLOW_DEL,                   /* ɾ���Զ������� */
	TCAD_ACTION_SET_FLOW_INTERVAL,          /* �����Զ���������־�ϱ�ʱ���� */
	TCAD_ACTION_SET_INCR_INTERVAL,          /* ���ó�����������־�ϱ�ʱ���� */
	TCAD_ACTION_SET_UDP_SERVER_IP,          /* ���ý���������־�ķ�����IP��ַ */
	TCAD_ACTION_BATCH,                      /* �������� */
	TCAD_ACTION_RESTORE_ADD,                /* ���Э�黹ԭ���� */
	TCAD_ACTION_RESTORE_DEL,                /* ɾ��Э�黹ԭ���� */
	TCAD_ACTION_RESTORE_MOD,                /* �޸�Э�黹ԭ���� */
	TCAD_ACTION_IPINFO_ADD,                 /* ���IP��ַ��λ��Ϣ */
	TCAD_ACTION_IPINFO_DEL,                 /* ɾ��IP��ַ��λ��Ϣ */
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

/* �û��Զ������� */
typedef struct flow_defined_struct{
	uint8_t     action;             /* ������ʶ */
	uint32_t    start_ip;           /* ��ʼIP��ַ�������ֽ��� */
	uint32_t    end_ip;             /* ����IP��ַ�������ֽ��� */
}flow_defined_t;

/* �����Զ���������־�ϱ�ʱ���� */
typedef struct flow_interval_struct{
	uint8_t     action;             /* ������ʶ */
	uint32_t    itime;              /* ���ʱ�䣬��λ�� */
}flow_interval_t;

/* ���ó�����������־�ϱ�ʱ���� */
typedef struct incr_interval_struct{
	uint8_t     action;             /* ������ʶ */
	uint32_t    itime;              /* ���ʱ�䣬��λ�� */
}incr_interval_t;

/* ���ý���������־�ķ�����IP��ַ */
typedef struct udp_server_struct{
	uint8_t     action;             /* ������ʶ */
	uint32_t    server_ip;          /* UDP������IP��ַ�������ֽ��� */
}udp_server_t;

/* �û�����Э�黹ԭ */
typedef struct flow_restore_struct{
	uint8_t action;                    /* ������ʶ */
	char    name[SAM_STR_SIZE+1];
	char    buf[BUF_SIZE];            /* ���ݸ�ʽΪ:<START>\nNAME:xxx\n[UDP]\n{b|0|0x00}{b|1|0x01}{b|2|0x00}{b|3|0x00}\n[TCP]\n{o|0|[0x3c,0x3d,0x29,0x39,0x3a,0x29,0x33,0x36]}{b|1|0x00}{b|2|0x00}{b|3|0x00}\n<END> */
}flow_restore_t;

/* IP��ַ��λ��Ϣ�������ֽ��� */
typedef struct ip_info_tag_s
{
	uint8_t action;
	uint8_t country;        /* ���� */
	uint16_t isp_id;        /* ��Ӫ��ID */
	uint32_t ip_start;      /* ��ʼIP��ַ */
	uint32_t ip_end;        /* ����IP��ַ */
	uint32_t province_id;   /* ʡ��ֱϽ�С�������ID */
	uint32_t city_id;       /* ����ID */
}ip_info_t;

#endif
