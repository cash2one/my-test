/*************************************************************************
	> File Name: plugin.h
	> Author: wangleizhang
	> Explain: 
	> Created Time: Tue 29 Oct 2013 08:37:56 PM EDT
 ************************************************************************/
#ifndef PLUGIN_H
#define PLUGIN_H
#include "list.h"
#include "plugin_manage.h"

#define PLUGIN_CONF_FILE    "./conf/plugin.conf"

enum {
    SESSION_ATTRI = 0, //会话相关
    SINGLE_PKT_ATTRI	//单包处理
};

/*hook 初始化函数指针, 返回插件线程私有的数据指针*/
typedef void *(*ngd_plugin_init)(int ptheadnum);
/*hook 执行的函数指针*/
typedef int (*ngd_plugin_proc)(pkt_info_t *ppkt);
/*hook 清除函数指针*/
typedef int (*ngd_plugin_clean)(pkt_info_t *ppkt);
/*跟tcp相关的hook，tcp会话结束的清除函数指针*/
typedef int (*ngd_plugin_tcp_clean)(pkt_info_t *ppkt);


#define PLUGIN_NAME_LEN	64
/*插件属性的结构体*/
typedef struct _ngd_plugin_ops {
    char    hookname[PLUGIN_NAME_LEN]; //hook名称
    unsigned char   hooknum;	    //处于hook链表上
    unsigned char   pri;	    //优先级
    unsigned char   plugin_idx;	    //标识该hook在链表上的第几个索引，
				    //用于在会话上查询使用状态

    unsigned char   sess_data_idx;  //会话私有数据的索引
    unsigned char    attribute;	    //标记是会话相关还是单包处理 
    ngd_plugin_init   plugin_init;   //初始化函数
    ngd_plugin_proc   plugin_proc;    //执行函数
    ngd_plugin_clean  plugin_clean;    //清除函数
    ngd_plugin_tcp_clean plugin_tcp_clean;	//tcp相关信息清除函数
    void    *private_data;	    //插件运行时的线程私有数据，根据线程个数开辟，

    declare_list_node;			//申明list管理
}ngd_plugin_ops;

/*存放所有hook 链表的数组*/
list_t	plugin_list_array[MAX_HOOK_NUM];

#endif

