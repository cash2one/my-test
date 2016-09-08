/*************************************************************************
	> File Name: plugin.c
	> Author: wangleizhang
	> Explain: 
	> Created Time: Tue 29 Oct 2013 08:25:51 PM EDT
 ************************************************************************/
#include <unistd.h>
#include "plugin.h"
#include "ngd_sys.h"
#include "misc.h"
#include "config.h"
#include "common_op.h"
#include "ngd_conf.h"
#include "plugin_manage.h"

#define PLUGIN_SOFILE_LEN   64
#define HOOK_NAME_LEN	64

#define DEBUG_READ_PLUGIN

typedef void (*CaptorCleanCtl)(pkt_info_t *ppkt);

CaptorCleanCtl captorcleanfun;
typedef struct _plugin_info {
    char name[PLUGIN_NAME_LEN];
    char sopath[PLUGIN_SOFILE_LEN];
    char hookname[HOOK_NAME_LEN];
    char shift;
    char attri;
    char pri;
}plugin_info_t;

plugin_info_t plugin_info;
cfg_desc plugin_conf_info[] = {
    {"", "shift", &(plugin_info.shift), CFG_SIZE_INT8, CFG_TYPE_INT8, 1, 0, ""},    
    {"", "name", plugin_info.name, PLUGIN_NAME_LEN, CFG_TYPE_STR, 1, 0, ""},    
    {"", "sopath", plugin_info.sopath, PLUGIN_SOFILE_LEN, CFG_TYPE_STR, 1, 0, ""},    
    {"", "hookpoint", plugin_info.hookname, HOOK_NAME_LEN, CFG_TYPE_STR, 1, 0, ""},    
    {"", "attribute", &(plugin_info.attri), CFG_SIZE_INT8, CFG_TYPE_INT8, 1, 0, ""},    
    {"", "priority", &(plugin_info.pri), CFG_SIZE_INT8, CFG_TYPE_INT8, 1, 0, ""},    
    { "","",NULL,0,0,0,0,""},
};

static inline int init_plugin_list(list_t *phooklist)
{
    int ret = NGD_OK;
    int i = 0;
    list_t *ptmplist = NULL;

    if (unlikely(NULL == phooklist)) {
	ret = NGD_ERR;
	goto RET;
    }
    ptmplist = phooklist;

    for (i = 0; i < MAX_HOOK_NUM; i++) {
	list_init(ptmplist + i, ngd_plugin_ops);
    }
RET:
    return ret;
}
#ifdef DEBUG_READ_PLUGIN
void print_plugin_info(plugin_info_t *plugin_info)
{
    printf("plugin hook location :%s\n", plugin_info->hookname);
    printf("plugin name :%s\n", plugin_info->name);
    printf("plugin so path :%s\n", plugin_info->sopath);
    printf("plugin shift:%d\n", plugin_info->shift);
    printf("plugin pri :%d\n", plugin_info->pri);
}

#define PRINT_HOOK_NAME(hookid) \
    switch (hookid) {\
	case CAPTURE_HOOK:\
	    printf("\nCAPTURE_HOOK:\n");\
	    break;\
	case ETHER_HOOK:\
	    printf("\nETHER_HOOK:\n");\
	    break;\
	case IP_HOOK:\
	    printf("\nIP_HOOK:\n");\
	    break;\
	case TCP_HOOK:\
	    printf("\nTCP_HOOK:\n");\
	    break;\
	case UDP_HOOK:\
	    printf("\nUDP_HOOK:\n");\
	    break;\
	case HTTP_HOOK:\
	    printf("\nHTTP_HOOK:\n");\
	    break;\
	default:\
	    printf("\nNO HOOK\n");\
	    break;\
    }

int print_list_node(void *data, void*arg)
{
    ngd_plugin_ops *pdata = (ngd_plugin_ops *)data;
    printf("%s [idx = %d][atti %d]->", pdata->hookname, pdata->plugin_idx, pdata->attribute);
    return 0;
}

void print_plugin_list()
{
    int i = 0;
    list_t *p_list = NULL;
    for (i = 0; i < MAX_HOOK_NUM; i++) {
	PRINT_HOOK_NAME(i);
	printf("plugin_num = %d tcp_plugin_num = %d\n", 
		hook_info_array[i].plugin_num, hook_info_array[i].tcp_plugin_num);
	p_list = (plugin_list_array + i);
	list_iterate(p_list, LIST_FORWARD, print_list_node, NULL);
    }
}
#endif

static inline int cmp_plugin_pri(void *data, void *arg)
{
    if (unlikely((NULL == data) || (NULL == arg))){
	return 1;
    }

    ngd_plugin_ops *d1 = (ngd_plugin_ops *)data;
    ngd_plugin_ops *d2 = (ngd_plugin_ops *)arg;
    
    /*pri越小优先级越高，排在链表之前*/
    if (d2->pri <= d1->pri) {
	return 1;
    }

    return 0;
}

#define HOOK_NAME_TO_ID(name, id) \
    switch (name[0]) { \
	case 'C':\
	    id = CAPTURE_HOOK;\
	    break;\
	case 'E':\
	    id = ETHER_HOOK;\
	    break;\
	case 'I':\
	    id = IP_HOOK;\
	break;\
	case 'U':\
	    id = UDP_HOOK;\
	break;\
	case 'T':\
	    id = TCP_HOOK;\
	case 'H':\
	    id = HTTP_HOOK;\
	break;\
	default:\
	    break;\
    }

/*根据配置信息，注册插件, 可以统计出每一个hook点上挂载的插件个数*/
static inline int regist_plugin(plugin_info_t *plugin_info)
{
    int ret = NGD_OK;
    void *so_handle = NULL;

    list_t *pluginlist = NULL; 
    ngd_plugin_ops *plugin_node = NULL;    
    ngd_plugin_ops *bak_node = NULL;

    plugin_node = (ngd_plugin_ops *)calloc(1, sizeof(ngd_plugin_ops));
    if (unlikely(NULL == plugin_node)) {
	ret = NGD_ERR;
	goto ERR;
    }
	
    /*赋值插件节点*/
    HOOK_NAME_TO_ID(plugin_info->hookname, plugin_node->hooknum);
    ca_strncpy(plugin_node->hookname,PLUGIN_NAME_LEN, 
	    plugin_info->name, strlen(plugin_info->name));
      
    plugin_node->pri = plugin_info->pri;
    plugin_node->attribute = plugin_info->attri;

//#if 0
    dlerror();
    /*打开so文件，并赋值函数指针*/
    so_handle = dlopen(plugin_info->sopath, RTLD_LAZY);
    if (NULL == so_handle) {
	ret = NGD_ERR;
	printf("dl err = %s\n", dlerror());
	goto ERR;
    }

    dlerror();

    /*如果是驱动的so，则打开清理数据包接口 */
    if (strstr(plugin_info->sopath, "libcaptor.so")) {
	captorcleanfun = dlsym(so_handle, "CaptorCleanCtrl"); 
    }
    plugin_node->plugin_init = dlsym(so_handle, "plugin_init");
    plugin_node->plugin_proc = dlsym(so_handle, "plugin_proc");
    plugin_node->plugin_clean = dlsym(so_handle, "plugin_clean");
    plugin_node->plugin_tcp_clean = dlsym(so_handle, "plugin_tcp_clean");
    if (dlerror() != NULL) {
	ret = NGD_ERR;
	printf("dl err = %s\n", dlerror());
	goto ERR;
    }
    /*调用初始化函数*/
    plugin_node->private_data = plugin_node->plugin_init(ngd_conf.thread_num);
//#endif
    /*根据优先级插入到hooknum链表中*/
    pluginlist = &plugin_list_array[plugin_node->hooknum];
    bak_node = list_iterate(pluginlist, LIST_FORWARD, cmp_plugin_pri, plugin_node);
    if (NULL == bak_node) {
	list_enqueue(pluginlist, plugin_node);
    } else {
	/*优先级定义冲突*/
	if (bak_node->pri == plugin_node->pri) {
	    ret = NGD_ERR;
	    goto ERR;
	}

	if (0 == list_insert_before(pluginlist, bak_node, plugin_node)) {
	    ret = NGD_ERR;
	    goto ERR;
	}
    }
    
    /*填充插件信息数组*/
    hook_info_array[plugin_node->hooknum].plugin_num++;
    if (SESSION_ATTRI ==  plugin_node->attribute) {
	hook_info_array[plugin_node->hooknum].tcp_plugin_num++;
    }

    plugin_node->plugin_idx = pluginlist->node_num - 1;
    return ret;
ERR:
    if (plugin_node != NULL) {
	free(plugin_node);
    }

    if (so_handle != NULL) {
	dlclose(so_handle);
    }
    return ret;
}

/*读取插件的配置文件并进行插件注册*/
static inline int read_plugin_conf(char *file_name)
{
    int ret = NGD_OK;
    FILE *pfile = NULL;
	
    if (unlikely(NULL == file_name)) {
	ret = NGD_ERR;
	goto RET;
    }
    
    pfile = open_config_file(file_name);
    if (unlikely(NULL == pfile)){
	ret = NGD_ERR;
	goto RET;
    }

    while (cfg_fill(pfile, plugin_conf_info) >= 0) {
#ifdef DEBUG_READ_PLUGIN
	print_plugin_info(&plugin_info);
#endif
	regist_plugin(&plugin_info);
    }
#ifdef DEBUG_READ_PLUGIN
	print_plugin_list();
#endif

RET:

    if (pfile != NULL) {
	close_config_file(&pfile);
    }

    return ret;
}

/*插件清理函数*/
static inline int _plugin_clean(void *data, void *arg)
{
    ngd_plugin_ops *pplugin = (ngd_plugin_ops *)data;
    pkt_info_t *pkt = (pkt_info_t *)arg;
    if (pplugin->private_data) {
	pkt->private_data = pplugin->private_data;
	pplugin->plugin_clean(pkt);
	free(pplugin->private_data);
	pplugin->private_data = NULL;
    }

    return 0;
}

//插件调用
static inline int _plugin_proc(void *data, void *arg)
{
    pkt_info_t *pkt = (pkt_info_t *)arg;
    ngd_plugin_ops *pplugin = (ngd_plugin_ops *)data;
    int plugin_ret = CONTINUE_GIVE_ME;
    plugin_data_t *pplugin_data = NULL;
    pplugin_data = pkt->plugin_data;

#if 0
    1、取出会话中的plugin_mask，与插件的plugin_idx位进行判断
    /*判断该插件是否调用*/
    if (0 == GETBIT(pplugin_data->plugin_mask, pplugin->plugin_idx)) {
	return 0;
    }
#endif
    pkt->private_data = pplugin->private_data;

    /*判断该插件是否需要tcp会话数据*/
    if (SESSION_ATTRI == pplugin->attribute) {
	/*取出会话信息中保存的该插件索引*/
#if 0
	if (unlikely(NULL == pplugin_data->plugin_private_data)) {
	    return 1;
	}
#endif
	pkt->plugin_sess_data = pplugin_data->plugin_private_data + pplugin->sess_data_idx;
	plugin_ret = pplugin->plugin_proc(pkt);
	pplugin_data->plugin_private_data[pplugin->sess_data_idx] = pkt->plugin_sess_data;
    } else {/*单包处理的插件*/
	plugin_ret = pplugin->plugin_proc(pkt);
    }

    /*说明该会话的后续报文不需要调用该插件了*/
    /*将会话中的plugin_mask的第plugin_idx位进行清零*/
    if (NO_CONTINUM_GIVE_ME == plugin_ret) {
	CLRBIT(pplugin_data->plugin_mask, pplugin->plugin_idx);
    }

    /*经过该插件处理后，不需要走后续所有的流程*/
    if (unlikely(NO_CONTINUM_PRO == pkt->follow_proc)) {
	/*置位MAX_HOOKNUM, 以便外部跳出后续hook处理*/
	pkt->hook_id = MAX_HOOK_NUM;
	return 1;
    }

    return 0;
}

/*单个hook上的插件的调用*/
int hook_plugin_proc(pkt_info_t *pkt)
{
    int ret = NGD_OK;
    list_t *plugin_list = NULL;

    plugin_list = &plugin_list_array[pkt->hook_id]; 
    if (unlikely(0 == list_count(plugin_list))) {
	ret = NGD_ERR;
	goto RET;
    }

    if (unlikely(NULL == plugin_list)) {
	ret = NGD_ERR;
	goto RET;
    }

    list_iterate(plugin_list, LIST_FORWARD, _plugin_proc, pkt);
RET:
    return ret;
}

#define RESET_PKT_VALUE(pkt) \
    pkt.follow_proc = CONTINUE_PRO;\
    pkt.hook_id = CAPTURE_HOOK;

/********************************************************************
 > function:
	1. 插件管理的初始化函数，通过配置文件加载插件
 > param:
	1.
 > return:
	1.NGD_OK: 初始化成功
	2.NGD_ERR:初始化失败
 > modify:
	1.creat by wlz
 ************************************************************************/
int init_plugin()
{
    int ret = NGD_OK;

    //初始化所有的hook 链表
    ret = init_plugin_list(plugin_list_array);
    if (NGD_ERR == ret) {
	goto RET;
    }

    if (NGD_ERR == read_plugin_conf(PLUGIN_CONF_FILE)) {
	ret = NGD_ERR;
	goto RET;
    }

RET:
    return ret;
}


/********************************************************************
 > function:
	1.调用插件的清除接口
 > param:
	1.
 > return:
	1.
 > modify:
	1.create by wlz
 ************************************************************************/
void clean_plugin(pkt_info_t *pkt)
{
    int i = 0;
    list_t *plugin_list = NULL;
    if (unlikely(NULL == pkt)) {
	return;
    }

    for (i = CAPTURE_HOOK; i < MAX_HOOK_NUM; i++) {
	plugin_list = &plugin_list_array[i]; 
	list_iterate(plugin_list, LIST_FORWARD, _plugin_clean, pkt);
    }
}

/********************************************************************
 > function:
	1.清除插件列表
 > param:
	1.
 > return:
	1.
 > modify:
	1.create by
 ************************************************************************/
void free_plugin_list()
{
    int i = 0;
    list_t *plugin_list = NULL;
    ngd_plugin_ops *list_node = NULL;

    for (i = CAPTURE_HOOK; i < MAX_HOOK_NUM; i++) {
	plugin_list = &plugin_list_array[i]; 
	while ((list_node = list_pop(plugin_list)) != NULL) {
	    free(list_node);
	}
    }

    return;
}

/********************************************************************
 > function:
	1. 插件调用框架函数，依次调用注册的插件
 > param:
	1. data: 存放的是线程运行参数，（目前是线程ID）
 > return:
	1.
 > modify:
	1. create by wlz
 ************************************************************************/
int plugin_proc_frame(void *data)
{
    pkt_info_t pkt_info;
    list_t *plugin_list = NULL;
    printf("%s data = %p\n", __FUNCTION__, data);
    long threadid = *(long *)data;
    //printf("thread id = %d\n", threadid);
    memset(&pkt_info, 0, sizeof(pkt_info_t));
    pkt_info.thread_id = threadid;
    
    void *captor_plugin = NULL;

    /*预先取出捕包插件，在总框架中调用*/
    plugin_list = &plugin_list_array[CAPTURE_HOOK];
    if (NULL == (captor_plugin = get_list_head(plugin_list))) {
	return 0;
    }
    
    RESET_PKT_VALUE(pkt_info)
    while (1) {
	printf("threadid = %d\n", threadid);
	/*设置每一个包的初始值*/

	while (pkt_info.hook_id < MAX_HOOK_NUM) {
	    if (NGD_ERR == hook_plugin_proc(&pkt_info)) {
		break;
	    }
	    sleep(1);
	}

	RESET_PKT_VALUE(pkt_info)
	/*调用捕包的释放数据报文接口*/
	if (captorcleanfun) {
	    /*取捕包插件的私有数据*/
	    pkt_info.private_data = ((ngd_plugin_ops *)captor_plugin)->private_data;
	    captorcleanfun(&pkt_info);
	}
    }

    /*清理插件*/
    clean_plugin(&pkt_info);
    return 0;
}

