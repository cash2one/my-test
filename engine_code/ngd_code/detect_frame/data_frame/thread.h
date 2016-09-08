/*
 * =====================================================================================
 *
 *       Filename:  thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/11/2009 01:56:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhang_zaifeng (zzf), @venus.com
 *        Company:  venustech.com
 *
 * =====================================================================================
 */

#ifndef __THREAD__H_
#define __THREAD__H_

#include <pthread.h>
#if 0
#include "decode/decode.h"
#include "decode/Packet.h"
#include "capture/shuguang_drv_captor.h"
#include "capture/special_drv_captor.h"
#include "capture/captor.h"
#include "cmdinfo.h"
#include "event_def.h"
#include "profile.h"
#include "decode/dfp/addr_spoof.h"
#include "decode/dfp/dfp_tcp_decode.h"
#include "decode/syn_cookie.h"
#include "hash.h"
#include "mem_pool.h"
#include "response/responsor.h"
#include "tdsvc.h"
#endif
#define MAX_THREAD_NUM 	8
#define MAX_COOP_LEN	32

enum {
	BTHREAD_MIN_STATE = 0,
	BTHREAD_STOP = 1,
	BTHREAD_RUNNING,	
	BTHREAD_PAUSE,		
	BTHREAD_INIT_ERR,	
	BTHREAD_EXIT,		
	BTHREAD_EXITED,		
	BTHREAD_CLENA_SESSION,	
	BTHREAD_CLENA_SESSION_DONE,
	BTHREAD_MAX_STATE
};

#define SIGKILLBTHREAD  51
#define SIGEXITBTHREAD  52

#define INIT_SUCCESS	1
#define INIT_FAILED	2

#define THREAD_POOL_INTERVAL	5
#define WAIT_BTHREAD_TIME	THREAD_POOL_INTERVAL - 2

typedef struct _Bthread_conf_info {
	int config_id;
}Bthread_conf_info_t;

typedef void *(*func_t)(void *);


typedef struct bprivate_info {
#if 0
	/*capture private info*/
	captor_desc_t		*captor_desc;
	int 			thread_idx;

	union {
		shuguang_slot_list	shuguang_slot_info;
		pag_slot_list		pag_slot_info;
	}un;

	ProtoHandler		*proto_handler_table;
	void                    *ipdfr_hdlr;

	/*arp private info*/
	ip_mac_table_t   	*ipmactable;
	char     		spoof_flag;
	int 			table_index;

	/*tcp private info*/
	void 			*tcpstr_hdlr;
	void 			*udpstr_hdlr;
	mempool_t 		*session_pool;
	mempool_t 		*udp_session_pool;
	mempool_t		*syncookie_pool;
	mempool_t		*stream_data_pool;
	mempool_t		*tuple_result_pool;
	mempool_t		*rule_cfg_pool;
	syncookie_hash_t 	*syn_hash_hd;
	/* add by zyl, 2010-07-20, for session_data_t */
	mempool_t		*ssn_data_pool;/* for memfile_t struct */
	/* added by zyl 2010-04-09 */
	mempool_t		*memfile_pool;/* for memfile_t struct */
	mempool_t		*fr_ftpdata_pool;/* for ftpdata_mesg struct */
	void		*portnego_ck; /* for prot mapped cookie */
	void		*ftpdata_ck;  /* for ftp private data cookie */
	mempool_t *http_boundary; /* for http */
	/* added end */
	
	//add by Samuel Xiao 2010/6/17 for email decode.  -->
	mempool_t               *smtp_pool;
	mempool_t               *smtp_einfo_pool;
	mempool_t               *pop3_pool;
	mempool_t               *pop3_einfo_pool;
	// <--
	
	/* add by zzf 2009-10-31 13:08 */
	/*2key log packet*/
	hash_t			*pkey_2_hash;
	mempool_t		*log2keypkt_pool;

	/*preproc payload info*/
	void 			*tdp_newpayload;
	void 			*stream_newpayload;
	int 			new_payload_len;

	Responsor 		*responsor_table;
	int 			responsor_num;
	Event_t			*g_event;
	void 			*report_resp_addr;

	Packet_t		*packet;
	void 			*mtc_list;

//	FILE 			*sign_event_log_fp;

	//tdsvc.
	struct tdsvccoll tdsvccoll;
#endif
}bthread_private_info_t;

typedef struct _bthread_info {
	pthread_t		tid;	/*Bthead id*/
	Bthread_conf_info_t	conf_info;	/*Bthread config info*/
	int 			init_step;
	//coop_cmdinfo_t		*coop_cmd;	/*coop queue, shared with Mthread*/

	//struct PROFILES		profile_info;	/*the profile of the Bthread*/

	func_t			thread_main;
	void 			*arg;
	int			thread_id;   /*may del */

	int 			enable_running;	/*the command from main thread*/
	unsigned int		status;	/*the state of the thread*/
	bthread_private_info_t	*tmp_private;
}bthread_shared_info_t;

extern bthread_shared_info_t *thread_pool;

int init_thread();
int clean_thread();
void thread_make();
void reg_thread_poll();
void bthread_step_cleanup(int bthread_init_step, struct bprivate_info *private_info);
#endif
