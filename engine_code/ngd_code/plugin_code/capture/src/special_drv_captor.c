#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sched.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dlfcn.h>

#include "ngd_conf.h"
#include "misc.h"
#include "captor.h"
#include "special_drv_captor.h"
//#include "common/dt_misc.h"
//#include "thread.h"
//#include "process_init.h"

//#include "trace_api.h"

#ifdef RECORD_BAD_PACKET
#include "cap_record.h"
#endif

static long 	special_drv_open(void *private_info, int argc, char **argv);
static int 	special_drv_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
static void 	special_drv_close(void *private_info, long hdlr);
static void 	special_drv_free(void *private_info);
static int	init_special_drv_slot_list(captor_desc_t *tmp_private);
captor_t special_drv_captor = {
	name: "special_drv",
	open: special_drv_open,
	capture: special_drv_capture,
	cleanctl: special_drv_free,
	close: special_drv_close,
};

int special_drvbuf_idx = 0;
pag_slot_list special_slot;

pag_open_t	pag_open = NULL;
pag_close_t	pag_close = NULL;
pag_get_t	pag_get = NULL;
pag_free_t	pag_free = NULL;
pag_time_t	pag_time = NULL;
void 		*special_handle = NULL;

#define DLSYM(func_name) \
	dlerror();\
	func_name = dlsym(special_handle, #func_name);\
	if((error = dlerror()) != NULL) {\
		EMSG("dlsym erro when:%s, msg:%s", #func_name, dlerror());\
		return (-1);\
	}

static int open_special_so(char *path)
{
	char *error = NULL;
	if((special_handle = dlopen(path, RTLD_LAZY)) == NULL){
		EMSG("dlopen error :%s,msg:%s", path, dlerror());
		return -1; 
	}

	DLSYM(pag_open);
	DLSYM(pag_close);
	DLSYM(pag_get);
	DLSYM(pag_free);
	DLSYM(pag_time);
	 
	return 0;
}

static void close_special_so()
{
	if(special_handle){
		dlclose(special_handle);
		special_handle = NULL;
	}
	return;
}

void close_special_drv()
{
	LMSG("before pag_close");
	pag_close();
	LMSG("after pag_close");
	close_special_so();
	LMSG("after close_special_so");

	return;
}

int open_special_drv(char *path)
{
	if(path == NULL){
		EMSG("special drv so path is invalid");
		goto err;
	}

	if(open_special_so(path) != 0){
		EMSG("open special drv so error");
		goto err;
	}
	/* add by zzf for special_drv in Project307 */
	DMSG("before open special captor");
    	if(pag_open() < 0)
    	{
		EMSG("### Error : can not open special dirver device! ###\n");
		goto err;
    	}
	DMSG("open special driver  captor success\n");

	return 0;
err:
	return -1;
}

long special_drv_open(void *private_info, int argc, char **argv)
{
	captor_desc_t *tmp_private = (captor_desc_t *)private_info;

	if(init_special_drv_slot_list(tmp_private) != 0 ) { 
		DMSG("init special mem list error");
		return -1;	
	} 
	
       	return 0;
}

int special_drv_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	uint16_t len = 0;	
	pag_slot_list	*tmp_slot_info = NULL;
	register int slot_idx = 0;
	int rest_packet_num = 0;
	int zero_packet_limit = ngd_conf.zero_packet_limit;
	struct iphdr *ip_hdr = NULL;	

	captor_desc_t *tmp_private = (captor_desc_t *)private_info;
	tmp_slot_info = &(tmp_private->pag_slot_info);
	while (1){
		if(tmp_slot_info->slot_list[tmp_slot_info->read_pos] == 0) {
			tmp_slot_info->read_pos = 0;
		}
		slot_idx = tmp_slot_info->slot_list[tmp_slot_info->read_pos] - 1;
		*pkt_buf_p = pag_get(tmp_slot_info->thread_idx);
		if (!(*pkt_buf_p)) { 
			tmp_slot_info->read_pos++;
			if(++rest_packet_num >= zero_packet_limit) {
				rest();
			}
			continue ;		
		}
		/*FIXME: what's the relationship between stream and thread in special drv*/
		tmp_slot_info->ego = *pkt_buf_p;	
		tmp_slot_info->stream_idx = slot_idx;

		/* FIXME: How to deal with VLAN? */
	//	len = ntohs(*((uint16_t*)pkt_buf_p) + 1) + 14;
		ip_hdr = (struct iphdr*)(*pkt_buf_p);
		len = ntohs(ip_hdr->tot_len);
		len += 14;
		(*pkt_buf_p) -= 14;

		if ( !len ) {
			len = -1;
		}
		DMSG("\n====captured by thread:%d, capture ego:%p, length:%d, \n", 
				tmp_private->thread_idx, tmp_slot_info->ego, len);
		tmp_slot_info->read_pos++;
		return len;
	}
}

void special_drv_free(void *private_info)
{
	captor_desc_t *tmp_private = (captor_desc_t *)private_info;
	pag_slot_list      *tmp_slot_info = &(tmp_private->pag_slot_info);
	if (tmp_slot_info->ego){
		pag_free(tmp_private->thread_idx, tmp_slot_info->ego);
		tmp_slot_info->ego = NULL;
	}

	return;
}

void special_drv_close(void *private_info, long hdlr)
{
	/*Mthread call pag_close in close_special_drv when *MT* exit*/
	return;
}
static int init_special_drv_slot_list(captor_desc_t *tmp_private)
{
	int i = 0;
	int global_thread_num = ngd_conf.thread_num; 
	int slot_idx = tmp_private->thread_idx;
	pag_slot_list      *tmp_slot_info = NULL;
	tmp_slot_info = &(tmp_private->pag_slot_info);

	if(slot_idx < 0 || slot_idx > global_thread_num){
		DMSG("thread_idx outof range");
		return -1;
	}

	tmp_slot_info->slot_num = ngd_conf.flow_num;
	tmp_slot_info->read_pos = 0;
	tmp_slot_info->thread_idx = slot_idx;
	memset(tmp_slot_info->slot_list, 0, MAX_SHUGUANG_MEM);

//	tmp_slot_info->slot_list[0] = slot_idx + 1;
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
