/*************************************************************************
	> File Name: plugin_manage.h
	> Author: wangleizhang
	> Explain: 
	> Created Time: Mon 18 Nov 2013 12:28:31 PM EST
 ************************************************************************/
#ifndef PLUGIN_MANAGE_H
#define PLUGIN_MANAGE_H

#define int8_t   char
#define u_int8_t unsigned char
#define int16_t short
#define u_int16_t unsigned short
#define int32_t int
#define u_int32_t unsigned int
#define int64_t long long
#define u_int64_t unsigned long long

/*定义报文的后续插件处理方式*/
enum {
    CONTINUE_PRO = 0,
    NO_CONTINUM_PRO
};

/*定义该报文会话的后续报文是否传给本插件*/
enum {
    CONTINUE_GIVE_ME = 0,
    NO_CONTINUM_GIVE_ME
};

#define MAX_PACKET_LEN		2048
#define DEFRAG_RETBUF_SZ	MAX_PACKET_LEN
/*承载报文信息结构体*/
typedef struct _pkt_info {
    char hook_id;	//报文需要调用的hook
    char follow_proc;	//报文的后续处理方式，继续走流程，还是不走

    int thread_id;	//当前包所属的线程ID
    void *private_data;	//报文当前处理的插件数据私有区

    void *plugin_data;	//插件相关的数据，由外边框架赋值
    void *plugin_sess_data; //插件跟tcp会话相关的私有数据地址;
			    //有调用框架赋值，业务插件可见
    u_int8_t *raw_pkt;	//原始报文
    int raw_pkt_len;	//原始报文长度
    //  uint8_t *eth_dmac;	/**/ 
    u_int8_t *eth_payload;
    u_int16_t eth_payload_len;

    u_int16_t  eth_proto;

    struct iphdr * ip_hdr;
    struct tcphdr * tcp_hdr;
    struct udphdr * udp_hdr;

    /*ip info begin*/
    u_int32_t ip_sip;
    u_int32_t ip_dip;
    u_int8_t *ip_payload;
    int16_t ip_payload_len;
    int16_t   ip_length;
    int8_t    ip_hlength;
    /*ip info end*/

    u_int8_t defrag_retbuf[DEFRAG_RETBUF_SZ];
}pkt_info_t;

enum {
    CAPTURE_HOOK = 0,
    ETHER_HOOK,
    IP_HOOK,
    ARP_HOOK,
    UDP_HOOK,
    TCP_HOOK,
    HTTP_HOOK,
    SMTP_HOOK,
    POP3_HOOK,
    DNS_HOOK,
    MAX_HOOK_NUM
};

typedef struct _hook_info {
    int16_t plugin_num;	    // 记录每一个hook上挂载的插件个数
    int16_t tcp_plugin_num; //每一个hook上跟tcp会话数相关的插件个数
}hook_info_t;

/*存放插件信息的数组*/
hook_info_t hook_info_array[MAX_HOOK_NUM];

typedef struct _plugin_data {
    u_int64_t plugin_mask;	/*挂载在会话(tcp会话或者应用会话)上执行的plugin，
				  每一位表示plugin是否执行*/ 
    long *plugin_private_data;	/*存放会话相关插件的私有数据地址*/
}plugin_data_t;

#define DECLEAR_PLUGIN_DATA \
    plugin_data_t  plugin_data;

#define SET_PLUGIN_DATA_TO_PKT(pkt, type, sess)\
    (pkt)->plugin_data = &((type) *)(sess)->plugin_data;

/*会话(tcp会话或者是应用层会话)初始化时调用*/
#define INIT_PLUGIN_SESSDATA(type, sess, hook_id) \
    ((type) *)(sess)->plugin_data.plugin_mask = 0xFFFFFFFFFFFFFFFF;\
    ((type) *)(sess)->plugin_data.plugin_private_data = \
    calloc(hook_info_array[hook_id].tcp_plugin_num, sizeof(void *));

/*插件初始化总函数*/
int init_plugin();

void free_plugin_list();

int plugin_proc_frame(void *data);

void clean_plugin(pkt_info_t *pkt);

#endif

