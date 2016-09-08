#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sched.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <fcntl.h>

#include <dlfcn.h>

#include "misc.h"
#include "ngd_conf.h"
#include "captor.h"
#include "shuguang_drv_captor.h"
//#include "trace_api.h"
//#include "process_init.h"

//#include "thread.h"
//#include "thread_ctrl.h"
//#include "common/dt_misc.h"

#ifdef RECORD_BAD_PACKET
#include "cap_record.h"
#endif

//#include "perform_record.h"

static long 	shuguang_drv_open(void *private_info, int argc, char **argv);
static int 	shuguang_drv_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
static void 	shuguang_drv_close(void *private_info, long hdlr);
static void 	shuguang_drv_free(void *private_info);
static int 	init_shuguang_drv_slot_list(captor_desc_t *private_info);

captor_t shuguang_drv_captor = {
	.name = "shuguang_drv",
	.open = shuguang_drv_open,
	.capture = shuguang_drv_capture,
	.cleanctl = shuguang_drv_free,
	.close = shuguang_drv_close,
};

int SHUGUANG_STREAM_NUM;
int shuguang_drvbuf_idx;

ppf_open_t 			ppf_open = NULL;
ppf_close_t 			ppf_close = NULL;
ppf_setStreamNum_t		ppf_setStreamNum = NULL;
ppf_getDataBlock_no_Lock_t	ppf_getDataBlock_no_Lock = NULL;
ppf_freeDataBlock_t		ppf_freeDataBlock = NULL;
ppf_getPktLength_t		ppf_getPktLength = NULL;
void 				*ppf_handle = NULL;

#define DLSYM(func_name) \
	dlerror();\
	func_name = dlsym(ppf_handle, #func_name);\
	if((error = dlerror()) != NULL) {\
		EMSG("dlsym erro when:%s, msg:%s", #func_name, dlerror());\
		return (-1);\
	}

int open_ppf_so(char *path)
{
	char *error = NULL;
	if((ppf_handle = dlopen(path, RTLD_LAZY)) == NULL){
		EMSG("dlopen error :%s,msg:%s", path, dlerror());
		return -1;
	}

	DLSYM(ppf_open);
	DLSYM(ppf_close);
	DLSYM(ppf_setStreamNum);
	DLSYM(ppf_getDataBlock_no_Lock);
	DLSYM(ppf_freeDataBlock);
	DLSYM(ppf_getPktLength);
	DLSYM(ppf_freeDataBlock);

	return 0;
}

void close_ppf_so()
{
	if (ppf_handle){
		dlclose(ppf_handle);
		ppf_handle = NULL;
	}
}

int open_ppf_drv(char *path)
{
	if(path == NULL){
		EMSG("ppf so path is invalid");
		return -1;
	}
	if(open_ppf_so(path) != 0){
		EMSG("open ppf so error");
		return -1;
	}

    	if(ppf_open() == -1) {
      		 printf("### Error : can not open shuguang device! ###\n");
		 goto err;
    	}
	if(ppf_setStreamNum(ngd_conf.flow_num) < 0){
		ppf_close();
		goto err;
	}
	return 0;
err:
	return -1;
}

void close_ppf_drv()
{
	LMSG("before ppf_close");
	ppf_close();
	LMSG("after ppf_close");
	close_ppf_so();
	LMSG("after close_ppf_so");
	return;
}

long shuguang_drv_open(void *private_info, int argc, char **argv)
{
	 captor_desc_t *tmp_private = (captor_desc_t *)private_info;
	if(init_shuguang_drv_slot_list(tmp_private) != 0 ) { 
		DMSG("init shuguang mem list error");
		return -1;	
	} 
	
       	return 0;
}

//return: 0 no packet 
//        > 0  packet length.
//        < 0  capture nothing, error occured.
int shuguang_drv_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	int len = 0;	
	shuguang_slot_list	*tmp_slot_info = NULL;
	register int slot_idx = 0;
	int rest_packet_num = 0;
	int zero_packet_limit = ngd_conf.zero_packet_limit;
	captor_desc_t *tmp_private = (captor_desc_t *)private_info;
	tmp_slot_info = &(tmp_private->shuguang_slot_info);

	while (1){
		if(tmp_slot_info->slot_list[tmp_slot_info->read_pos] == 0) {
			tmp_slot_info->read_pos = 0;
		}
		slot_idx = tmp_slot_info->slot_list[tmp_slot_info->read_pos] - 1;
		*pkt_buf_p = ppf_getDataBlock_no_Lock(slot_idx);

		if (!(*pkt_buf_p)) { 
			tmp_slot_info->read_pos++;
			if(++rest_packet_num >= zero_packet_limit) 
			{
				rest();
				rest_packet_num = 0;
			}
			continue ;		
		}

		tmp_slot_info->ego = *pkt_buf_p;		
		len = ppf_getPktLength(tmp_slot_info->ego);
		(*pkt_buf_p)+=32;
		
		//ppf_getPktLength: return 0: no packet, 
		//                        >0: packet's length.
		if ( len == 0) {
			tmp_slot_info->read_pos++;
			// if(++rest_packet_num >= zero_packet_limit)
			// {
			//        rest();
			//         rest_packet_num = 0;
			//  }
			ppf_freeDataBlock(tmp_slot_info->ego);
			continue ;
		}

	//	DMSG("\n====captured by thread:%d, capture ego:%p, length:%d, \n", 
	//			tmp_private->thread_idx, tmp_slot_info->ego, len);
		tmp_slot_info->read_pos++;
		return len;
	}	
}

void shuguang_drv_free(void *private_info)
{
	captor_desc_t *tmp_private = (captor_desc_t *)private_info;
	shuguang_slot_list      *tmp_slot_info = &(tmp_private->shuguang_slot_info);

//	DMSG("==== freed by thread:%d, free ego:%p\n",tmp_private->thread_idx, tmp_slot_info->ego);
	if (tmp_slot_info->ego) {
		ppf_freeDataBlock(tmp_slot_info->ego);
		tmp_slot_info->ego = NULL;
	}
	return;
}

void shuguang_drv_close(void *private_info, long hdlr)
{	
//	ppf_close();
	return;
}

/* *************************
 * return 	0: success
 * 	   othrer: failed
 * ************************/
int init_shuguang_drv_slot_list(captor_desc_t *tmp_private)
{
    int i = 0;
    int global_thread_num = ngd_conf.thread_num;
    int slot_idx = tmp_private->thread_idx;

    shuguang_slot_list  *tmp_slot_info = NULL;
    tmp_slot_info = &(tmp_private->shuguang_slot_info);

    if(slot_idx < 0 || slot_idx > global_thread_num){
	DMSG("thread_idx outof range");
	return -1;
    }

    tmp_slot_info->slot_num = ngd_conf.flow_num;
    tmp_slot_info->read_pos = 0;
    tmp_slot_info->thread_idx = slot_idx;
    memset(tmp_slot_info->slot_list, 0, MAX_SHUGUANG_MEM);

    while(slot_idx < tmp_slot_info->slot_num) {
	tmp_slot_info->slot_list[i++] = slot_idx + 1;
	slot_idx += global_thread_num;
    }

    /* 	for (i = 0; i < 4; i++) {
     * 		printf("thread:%d-----stream id:%d\n", tmp_private->thread_idx, tmp_slot_info->slot_list[i]);
     * 	}
     */
    return 0;
}
