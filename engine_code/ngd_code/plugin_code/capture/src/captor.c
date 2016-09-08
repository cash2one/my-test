#include <sys/file.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>

#include "captor.h"
#include "plugin_manage.h"
#include "raw_socket_captor.h"
#include "tcpdump_file_captor.h"
#include "tcpdump_filelist_captor.h"
#include "pcap_lib_captor.h"
#include "cap_file_captor.h"
#include "shuguang_drv_captor.h"
#include "special_drv_captor.h" 
#include "task_queue_captor.h"
#include "ngd_conf.h"
#include "../../../detect_frame/data_frame/ngd_init.h"

//#include "thread.h"
//#include "trace_api.h"
//#include "thread_ctrl.h"
//#include "time_consume.h"
#include "misc.h"
#define CAPTOR_DESC_NUM	15 
#define EMSG	printf
static captor_t *captors[] = {
	&raw_socket_captor,
	&tcpdump_file_captor,
	&tcpdump_filelist_captor,
	&pcap_lib_captor,
	&cap_file_captor,
	&shuguang_drv_captor,
	&special_drv_captor,
	&task_queue_captor,
	NULL
};

#if 0
int InitCaptor(void *private_info)
{
//	thread_lock(CAPTOR_TABLE_MUTEX);
	struct bprivate_info *tmp_private = (struct bprivate_info *)private_info;
	if(tmp_private->captor_desc == NULL) {
		tmp_private->captor_desc = (captor_desc_t *)calloc(1, sizeof(captor_desc_t));
		if(tmp_private->captor_desc == NULL){
			EMSG("calloc error when allocing captor_desc");
			return -1;
		}
	}

	captor_desc_t *captor_desc = tmp_private->captor_desc;
	captor_desc->captor = NULL;
	captor_desc->handler = -1;

//	thread_unlock(CAPTOR_TABLE_MUTEX);
	return 0;
}
#endif
/* 
 * SearchCaptor
 */
captor_t *SearchCaptor(char *captor_name)
{
	captor_t **cap = NULL;

	for (cap = captors; *cap != NULL; cap++) {
		if (strcasecmp((*cap)->name, captor_name) == 0)
			break;
	}

	return *cap;
}

/*
 * OpenCaptor
 * return: 
 *     <0: error; >0: handle; 0: is used trap error
 */
int OpenCaptor(captor_desc_t *private_info, char *captor_name, int argc, char *argv[])
{
	captor_desc_t *pcaptor_desc = NULL;
	pcaptor_desc = (captor_desc_t *)private_info;
	int index = 1;		//default index = 1
	printf("%s\n",__FUNCTION__);
	if (captor_name == NULL || captor_name[0] == 0) {
		goto err;
	}

	if ((pcaptor_desc->captor = SearchCaptor(captor_name)) == NULL) {
		EMSG("Error: OpenCaptor cannot search captor\n");
		goto err;
	}

	printf("pcaptor_desc = %p pcaptor_desc->captor = %p\n", 
		pcaptor_desc, pcaptor_desc->captor);
	pcaptor_desc->handler = 
			((pcaptor_desc->captor)->open)(private_info,argc, argv);
	printf("pcaptor_desc = %p pcaptor_desc->captor = %p\n", 
		pcaptor_desc, pcaptor_desc->captor);
	if (pcaptor_desc->handler < 0 && (pcaptor_desc->handler & 0xf) != 0) {
		// 识别地址和错误情况（-1)
		// 地址有可能返回负数的long，如0xb5c00500
		EMSG("Error: OpenCaptor open captor(%s): %#x\n", captor_name, pcaptor_desc->handler);
		goto err;
	}

	return index;
err:
	return -1;
}

int CaptorCapture(void *private_info, u_int8_t **pkt_buf_p)
{
    int rst = -1;

    captor_desc_t *captor_desc = (captor_desc_t *)private_info;
    printf("%s\n",__FUNCTION__);
    printf("captor_desc->captor %p \n", captor_desc->captor);
    if (likely((captor_desc->captor)->capture)) {
	rst = (captor_desc->captor)->capture(private_info, 
		captor_desc->handler, pkt_buf_p);
    }
    return rst;
}

void
CloseCaptor(void *private_info)
{
	captor_desc_t *captor_desc = (captor_desc_t *)private_info;
	if (captor_desc && captor_desc->captor && captor_desc->captor->close) {
		(captor_desc->captor->close)(private_info, captor_desc->handler);
	}else{
		EMSG("Error: captor(name:%s, have not register close routine\n", 
				(captor_desc->captor)->name);
	}
	return;
}

void
CleanCaptor(void *private_info)
{
	captor_desc_t *captor_desc = (captor_desc_t *)private_info;
	if(captor_desc) {
		captor_desc->captor = NULL;
		free(captor_desc);
		captor_desc = NULL;
	}
	return;
}

/********************************************************************
 > function:
	1. 捕包功能的插件初始化函数，作为so的库函数对外提供
	2. 分配捕包插件的线程私有数据区
 > param:
	1. pthreadnum:线程个数
 > return:
	1. 返回分配的插件线程私有区地址，存放在插件总体结构中
 > modify:
	1.create by wlz
 ************************************************************************/
void *plugin_init(int pthreadnum)
{
    captor_desc_t *pcaptor_data = NULL;
    captor_desc_t *tmpcaptor_data = NULL;
    int i = 0;

    /*申请线程私有数据*/
    pcaptor_data = calloc(pthreadnum, sizeof(captor_desc_t));
    if (unlikely(NULL == pcaptor_data)) {
	goto ERR;
    }

    /*为每一个线程私有数据赋值*/
    for (i = 0; i < pthreadnum; i++) {
	printf("i = %d pthreadnum = %d\n", i, pthreadnum);
	tmpcaptor_data = pcaptor_data + i;
	tmpcaptor_data->thread_idx = i;
	OpenCaptor(tmpcaptor_data, ngd_conf.captor_name, cmd_args.captor_argc, cmd_args.captor_argv);
    }

    return pcaptor_data;
ERR:
    if (pcaptor_data != NULL) {
	free(pcaptor_data);
    }
    return NULL;
}


/********************************************************************
 > function:
	1.捕包功能的插件处理函数，作为so库函数对外提供
 > param:
	1.ppkt:传入的数据报文信息
 > return:
	1.CONTITUNE_GIVE_ME:继续传送报文给本插件
	2.NO_CONTINUE_GIVE_ME:该会话的报文后续不用给本插件
 > modify:
	1.create by wlz
 ************************************************************************/
int plugin_proc(pkt_info_t *ppkt)
{
    int ret = CONTINUE_GIVE_ME;
    if ((NULL == ppkt) || (NULL == ppkt->private_data)) {
	goto RET;
    }

    captor_desc_t *pcaptor_data = (captor_desc_t *)(ppkt->private_data) + ppkt->thread_id;
    int pkt_len = 0;

    printf("capture hook%s\n",__FUNCTION__);
    pkt_len = CaptorCapture(pcaptor_data, &(ppkt->raw_pkt));

    ppkt->raw_pkt_len = pkt_len;
    printf("raw_pkt_len = %d\n", ppkt->raw_pkt_len);
    ppkt->hook_id = ETHER_HOOK;
RET:
    return ret;
}

/********************************************************************
 > function:
	1.捕包插件的清理函数
 > param:
	1.ppkt：数据报文信息
 > return:
	1.
 > modify:
	1.create by wlz
 ************************************************************************/
int plugin_clean(pkt_info_t *ppkt)
{
    captor_desc_t *captor_desc = (captor_desc_t *)(ppkt->private_data) + ppkt->thread_id;
   // CloseCaptor(captor_desc);
    return 0;
}

/********************************************************************
 > function:
	1.
 > param:
	1.
 > return:
	1.
 > modify:
	1.create by
 ************************************************************************/
int plugin_tcp_clean(pkt_info_t *ppkt)
{
    return 0;
}

/********************************************************************
 > function:
	1. 驱动的清理报文接口，主要用于标识报文已经处理完成，驱动可以释放
 > param:
	1. ppkt:报文信息
 > return:
	1.无
 > modify:
	1.create by wlz
 ************************************************************************/
void CaptorCleanCtl(pkt_info_t *ppkt)
{
    captor_desc_t *captor_desc = (captor_desc_t *)(ppkt->private_data) + ppkt->thread_id;
    if (likely(captor_desc && captor_desc->captor && (captor_desc->captor->cleanctl))){
		(captor_desc->captor->cleanctl)(captor_desc);
    }
}

