/*************************************************************************
	> File Name: tcp_session.h
	> Author: wangleizhang
	> Explain: 
	> Created Time: Wed 06 Nov 2013 07:58:34 AM EST
 ************************************************************************/
#ifndef TCP_SESSION_H
#define TCP_SESSION_H
#include <sys/types.h>

typedef struct _plugin_data {
    uint64_t plugin_mask;	/*挂载在会话(tcp会话或者应用会话)上执行的plugin，每一位表示plugin是否执行*/ 
    long *plugin_private_data;	/*存放会话相关插件的私有数据地址*/
}plugin_data_t;

#define DECLEAR_PLUGIN_DATA \
    plugin_data_t  plugin_data; 

typedef struct _tcp_session {
    DECLEAR_PLUGIN_DATA
    void *high_proto_data;	/*应用层数据空间*/
}tcp_sess_t;

typedef struct _h_proto_sess_data {
    DECLEAR_PLUGIN_DATA 
    /*其他数据*/
}h_proto_sess_data_t;


#define SET_PLUGIN_DATA_TO_PKT(pkt, type, sess)\
    (pkt)->plugin_data = &((type *)sess)->plugin_data;

/*设置插件mask，tcp会话和应用会话初始化时调用*/
#define SET_PLUGIN_ALLMASK(sess) \
    sess->plugin_mask = 0xFFFFFFFFFFFFFFFF;

    
#endif

