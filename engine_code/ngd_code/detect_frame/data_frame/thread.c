/*
 * =====================================================================================
 *
 *       Filename:  thread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/11/2009 02:51:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhang_zaifeng (zzf), @venus.com
 *        Company:  venustech.com
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "thread.h"
#include "../plugin_manage/plugin.h"
#include "ngd_conf.h"
#define DMSG printf
#define EMSG printf
bthread_shared_info_t	*thread_pool = NULL;
static pthread_attr_t thread_attr;
#if 0
static void* Bthread_main (void* arg);

static int process_init_captor(struct bprivate_info *private_info)
{
//	thread_lock(CAPTOR_TABLE_MUTEX);
	if(InitCaptor(private_info) < 0) {
		DMSG("InitCaptor error");
		return -1;
	}

	if(dt_conf.multiple_task){
		/* if we use multiple task, get packet from dispatcher using task_queue,
		 * 3 args:
		 * 	task queue buffer key
		 * 	captor name which really catch packet
		 * 	worker num defined in configuration
		 */
		int tq_argc = 0;
		char *tq_argv[5];
		static char worker_num_str[3];  /* can not beyond 99 worker */
		snprintf(worker_num_str, sizeof(worker_num_str), "%u", dt_conf.multiple_task);
		tq_argv[0] = dt_conf.tskque_buffer_key_str;
		tq_argv[1] = dt_conf.captor_name;
		tq_argv[2] = worker_num_str;
		tq_argc = 3;
		if ((OpenCaptor(private_info, "task_queue", tq_argc, tq_argv)) < 0) {
			DMSG("Detector error: open captor %s", "task_queue");
			goto err;
		}
	}else {
		if((OpenCaptor(private_info, dt_conf.captor_name, 
							cmd_args.captor_argc, cmd_args.captor_argv)) < 0) {
			DMSG("OpenCaptor error");
			goto err;
		}
	}
//	thread_unlock(CAPTOR_TABLE_MUTEX);
	return 0;
err:
//	thread_unlock(CAPTOR_TABLE_MUTEX);
	return -1;
}

static int process_init_decoder(struct bprivate_info *private_info, Packet_t *packet)
{
	if(init_port_table(dt_conf.proto_conf_path) < 0) {
		DMSG("init_port_table error");
		goto err;
	}
	
	if (RegisterDecoderPlugins(private_info) < 0) {
		DMSG("RegisterDecoderPlugins");
		goto err;
	}

	if(InitDecoder(private_info) < 0 ){
		DMSG("InitDecoder error");
		goto err;
	}

	return 0;
err:
	return -1;
}

static int process_init_checker(struct bprivate_info *private_info)
{
	/*place holder, this function from check files*/
	InitChecker(private_info);
	return 0;
}

static int process_init_responsor(struct bprivate_info *private_info)
{
	if(InitResponsor(private_info) < 0){
		DMSG("InitResponsor error");
		goto err;
	}
	return 0;
err:
	return -1;
}

static int set_thread_info(int idx, struct bprivate_info **p_private_info, 
				bthread_shared_info_t **p_shared_info, Packet_t *packet)
{
	*p_private_info = (struct bprivate_info *)calloc(1, sizeof(struct bprivate_info));
	if(*p_private_info == NULL) {
		DMSG("thread:%ld start failed, cause calloc private_info failed.", idx);
		goto err;
	}
	packet->tmp_private = *p_private_info;
	(*p_private_info)->packet = packet;
	(*p_private_info)->thread_idx = idx;

//	thread_pool[idx].status = BTHREAD_RUNNING;
//	thread_pool[idx].enable_running = BTHREAD_RUNNING;
	(*p_shared_info) = &thread_pool[idx];
	(*p_shared_info)->tmp_private = *p_private_info;
	(*p_shared_info)->coop_cmd = (coop_cmdinfo_t *)calloc(dt_conf.coopqueue_len, sizeof(coop_cmdinfo_t));
	if((*p_shared_info)->coop_cmd == NULL) {
		EMSG("thread:%ld start failed, cause calloc coop_cmd failed.", idx);
		goto err;
	}
#ifdef HEART_BEAT
	(*p_shared_info)->profile_info.static_posed_packet = (unsigned long long ) (-1);
#endif
	return 0;
err:
	if(p_private_info) {
		free(p_private_info);
		p_private_info = NULL;
	}

	if((*p_shared_info)->coop_cmd) {
		free((*p_shared_info)->coop_cmd);
		(*p_shared_info)->coop_cmd = NULL;
	}
	return -1;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bthread_step_cleanup
 *  Description:  We don't check the status of bthread. Just clean it.
 * =====================================================================================
 */
void bthread_step_cleanup(int bthread_init_step, struct bprivate_info *private_info)
{
	printf("\tBThread Detector Cleanup  ... begin,total step:%d\n", bthread_init_step);
	if(private_info == NULL){
		goto ret;
	}

	switch(bthread_init_step){
#ifndef DT_QUICK_CLEAN
	case 6:
		CleanEvent(private_info->g_event);
	case 5:
		CleanPacket(private_info->packet);
	case 4:
		CleanResponsor(private_info);
	case 3:
		CleanChecker(private_info);
	case 2:
		CleanDecoder(private_info);
	case 1:
		CaptorCleanCtl(private_info);
		CloseCaptor(private_info);
		CleanCaptor(private_info);
	case 0:
		free(private_info);
		private_info = NULL;
		break;
#else
	case 6:
	case 5:
	case 4:
	case 3:
	case 2:
	case 1:
		CleanDecoder(private_info);
		CaptorCleanCtl(private_info);
		CloseCaptor(private_info);
		CleanCaptor(private_info);
	case 0:
		free(private_info);
		private_info = NULL;
		break;
#endif
	default:
		break;
		
	}
ret:
	printf("\tBThread Detector Cleanup  ... done\n");
	return;
}

static void reset_shared_info(bthread_shared_info_t *shared_info, int idx)
{
	shared_info->init_step = 0;		//used for mthread_step_cleanup
	if(shared_info->coop_cmd) { 				//clean old coop
		free(shared_info->coop_cmd);
		shared_info->coop_cmd = NULL;
	}
	memset(&shared_info->profile_info, 0, sizeof(shared_info->profile_info));
	shared_info->thread_main = Bthread_main;
	shared_info->arg = (void *)(long)idx;
	shared_info->tid = 0;
	shared_info->tmp_private = NULL;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  exit_bthread
 *  Description:  works for reboot bthread or exit bthread.
 *  		  we should make a difference between SIGKILLBTHREAD and SIGEXITBTHREAD.
 *  		  为了记录堆栈，我们有两种方式一种是extract_stack_info，另一种是record_sleep_info
 *  		  前者在这个函数中会导致coredump.主要是因为记录堆栈信息时会越界。
 *  		  pthread_kill也不是安全的。
 *		  record_sleep_info是安全的，但是不能记录正确的堆栈信息。此时记录下的堆栈
 *		  信息全部是该函数本身的。所以这里的堆栈有点鸡肋。
 * =====================================================================================
 */
void exit_bthread(int signo, siginfo_t * info, void *ptr)
{
	int i = 0;
	pthread_t self = pthread_self();
	for(i = 0; i < global_thread_num; i++) {
		if(pthread_equal(self, thread_pool[i].tid)) {
			break;
		}
	}
	char tmp_buf[256] = {0};
	time_t now = get_time();
	char tmp_time[32] = {0};
	ctime_r(&now, tmp_time);
	snprintf(tmp_buf, sizeof(tmp_buf) - 1, "\tIN %s, exit thread:%d at %s", 
						__FUNCTION__, i, /* ctime(&now)*/tmp_time);
	write_hb_file(tmp_buf, strlen(tmp_buf), LOG_HB_FILE, WR_FILE_MODE_APPEND);

	if(signo == SIGKILLBTHREAD) {	
		printf("I(%d) recv signo SIGKILLBTHREAD, and set the status\n", i);
	//	extract_stack_info(signo, info, ptr);
		record_sleep_info();
		bthread_step_cleanup(thread_pool[i].init_step, thread_pool[i].tmp_private);
		reset_shared_info(&thread_pool[i], i);
		thread_pool[i].status = BTHREAD_STOP;	//used for reboot bthread
	}else if (signo == SIGEXITBTHREAD){
		bthread_step_cleanup(thread_pool[i].init_step, thread_pool[i].tmp_private);
		reset_shared_info(&thread_pool[i], i);
		printf("I(%d) recv signo SIGEXITBTHREAD, and set the status\n", i);
		thread_pool[i].status = BTHREAD_EXITED;
	}
	pthread_exit(NULL);
}

static void bthread_set_signal()
{
	sigset_t	mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGABRT);

	pthread_sigmask(SIG_SETMASK, &mask, NULL);
	
	setsignal(SIGKILLBTHREAD, exit_bthread);
	setsignal(SIGEXITBTHREAD, exit_bthread);
}

static void log_thread_init_info(bthread_shared_info_t *shared_info, int flag)
{
	char tmp_buf[256] = {0};
	
	char tmp_time[32] = {0};
	ctime_r(&shared_info->profile_info.init_end_time, tmp_time);
	snprintf(tmp_buf, sizeof(tmp_buf) - 1, "\tthread:%ld, tid:%ld, init:%s, started at:%s", 
			(long)shared_info->arg, shared_info->tid, flag == INIT_SUCCESS?"success":"failed",
			/*ctime(&shared_info->profile_info.init_end_time)*/tmp_time);
	write_hb_file(tmp_buf, strlen(tmp_buf), LOG_HB_FILE, WR_FILE_MODE_APPEND);
}

static int bthread_init(Packet_t *packet, int idx, struct bprivate_info **p_private_info, 
						bthread_shared_info_t ** p_shared_info)
{
	int bthread_init_step = -1;
	struct bprivate_info *private_info = NULL;
	bthread_shared_info_t *shared_info = NULL;

	if(set_thread_info(idx, p_private_info, p_shared_info, packet) != 0) {
		DMSG("set_thread_info error");
		goto err;
	}
	private_info = *p_private_info;
	shared_info = *p_shared_info;
	
	bthread_init_step = 0; 

	/*set the mask of signal*/
	bthread_set_signal();

	shared_info->profile_info.init_begin_time = get_time();
	if(process_init_captor(private_info) < 0) {
		DMSG("process_init_captor error");
		goto err;
	}
	
	bthread_init_step = 1;
	
 	if(process_init_decoder(private_info, packet) < 0) {
  		DMSG("process_init_decoder error");
  		goto err;
  	}
  
  	bthread_init_step = 2;

	if(process_init_checker(private_info) < 0) {
		DMSG("process_init_checker error");
		goto err;
	}

	bthread_init_step = 3;

	if(process_init_responsor(private_info) < 0) {
		DMSG("process_init_responsor error");
		goto err;
	}

	bthread_init_step = 4;

	if(InitPacket(packet) < 0) {
		DMSG("InitPacket error");
		goto err;
	}

	bthread_init_step = 5;
	
	if((private_info->g_event = InitEvent()) == NULL) {
		DMSG("InitEvent error");
		goto err;
	}

	bthread_init_step = 6;
	
	usleep(10000);
	shared_info->init_step = bthread_init_step;
	shared_info->profile_info.init_end_time = get_time();
	shared_info->status = BTHREAD_RUNNING;
	shared_info->enable_running = BTHREAD_RUNNING;
	log_thread_init_info(shared_info, INIT_SUCCESS);

	return bthread_init_step;
err:
	if(shared_info != NULL){
		shared_info->init_step = bthread_init_step;
		shared_info->profile_info.init_end_time = get_time();
		log_thread_init_info(shared_info, INIT_FAILED);
	}
	return -1;
}

static inline void heartbeat_update(struct PROFILES *profiles)
{
#ifdef HEART_BEAT
	profiles->prev_capture_packet = profiles->capture_packet;
	profiles->prev_decode_packet = profiles->decode_packet;
	profiles->prev_check_packet = profiles->check_packet;
	profiles->prev_posed_packet = profiles->posed_packet;
#endif
}

static inline void heartbeat_inc_posed(struct PROFILES *profiles)
{
#ifdef HEART_BEAT
	profiles->posed_packet++;
#endif
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  capture
 *  Description:  
 * =====================================================================================
 */
#define CAPTURE_SUCCESS	0
#define CAPTURE_STOP	(-1)
#define CAPTURE_ERR	(-2)

static inline int capture(struct bprivate_info *private_info, bthread_shared_info_t *shared_info, Packet_t *packet)
{
	int raw_packet_len = 0;
	unsigned char *raw_packet = NULL;
	static int continuous_error = 0;

	while ((raw_packet_len = CaptorCapture(private_info, &raw_packet)) <= 0) 
	{
		if(raw_packet_len < 0 ) 
		{
			shared_info->profile_info.capture_error++;
			continuous_error++;
			PER_COUNT(per_captureerror);
			if(continuous_error > MAX_CAPTURE_CONTINUOUS_ERROR) {
				EMSG("Detector error: captor capture too many errors");
				goto capture_err;
			}
		}
		else if(raw_packet_len == 0)
		{
			//capture nothing, please retry it.
			goto capture_stop;
		}
	}

	//get valid packet now.
	continuous_error = 0;
	shared_info->profile_info.capture_packet++;
	shared_info->profile_info.capture_packet_tseg++;
	shared_info->profile_info.capture_bits += raw_packet_len<<3;
	PER_COUNT(per_capture);
	packet->raw_pkt = raw_packet;
	packet->raw_pkt_len = raw_packet_len;
	packet->time_stamp = get_time();

	return CAPTURE_SUCCESS;
capture_stop:
	return CAPTURE_STOP;
capture_err:
	return CAPTURE_ERR;
}

static inline int decode(struct bprivate_info *private_info, bthread_shared_info_t *shared_info, Packet_t *packet)
{
	if(DecoderDecode(private_info, packet) < 0) {
		shared_info->profile_info.decode_error++;
		PER_COUNT(per_decodeerror);
	}else{
		shared_info->profile_info.decode_packet++;
	}
	PER_COUNT(per_decode);
/* 	int i = 0;
 * 	while (i < 20) {
 * 		sleep(1);
 * 		i++;
 * 	}
 */
	return 0;
}


static inline void take_easy(struct PROFILES *profiles)
{
	if ((profiles->capture_packet & 0x1FFF) == 0){
		rest();
	}
}
static void* Bthread_main (void* arg)
{	
	bthread_shared_info_t	*shared_info = NULL;
	struct bprivate_info *private_info = NULL;
	Packet_t        packet;
	long idx = (long)arg;
	int result = 0;

	fprintf(stdout, "Detector Initialization  ...begin\n");
	if((bthread_init(&packet, idx, &private_info, &shared_info)) < 0){
		goto err2;
	}
	fprintf(stdout, "Detector Initialization  ...done\n");

	fprintf(stdout, "Detector Detection ...begin\n");
	shared_info->profile_info.detect_begin_time = get_time();

	while(1) {
		//add by xiaochengmin 2010/10/22 -->
		//use to decrease mt's cpu.
		
		static unsigned int packetidx = 0; 
		if (dt_conf.sleep_each_packet > 0 &&  
				++packetidx >= dt_conf.sleep_each_packet)
		{
		   rest(); 
		   packetidx = 0;  //set to zero.
		}
		
		//<--	

		heartbeat_update(&shared_info->profile_info);
		
		//check coop cmd.
		check_coop_cmd(shared_info);
		
		//
		ZeroPacket(&packet);
	//	printf("===-----------thread:%ld--line:%d\n", idx, __LINE__);
	//	printf("\n+=+=+=+=+=+=+=+=+=+=+=+= BEGIN +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\n");
		result = capture(private_info, shared_info, &packet);
	//	printf("===-----------thread:%ld--line:%d\n", idx, __LINE__);
		
		if(result < 0)
		{
			goto err;
		}
		
		//
		decode(private_info, shared_info, &packet);
			
	//	printf("packet.drop_flag :%d\n", packet.drop_flag );
		if (packet.drop_flag == 0) 
		{
			CheckerCheck(&packet);
			shared_info->profile_info.check_packet++;
		}

		CaptorCleanCtl(private_info);
		heartbeat_inc_posed(&shared_info->profile_info);

		//check configure cmd.
		check_config_cmd(shared_info);
		
		//
		take_easy(&(shared_info->profile_info));
		//	printf("\n+=+=+=+=+=+=+=+=+=+=+=+=+ END =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\n");

	}
	
err:
	shared_info->profile_info.detect_end_time = get_time();
	bthread_step_cleanup(shared_info->init_step, private_info);
	if(captor_type == CAPTOR_GENERAL) {
		print_profiles();
		wait_exit_mthread();
		exit(0);
	}
	thread_pool[idx].status = BTHREAD_EXIT;
	pthread_exit(NULL);
	exit(-1);
err2:
	bthread_step_cleanup(shared_info->init_step, private_info);
	if(captor_type == CAPTOR_GENERAL) {	/*one thread*/
		print_profiles();
		wait_exit_mthread();
		exit(0);
	}
	thread_pool[idx].status = BTHREAD_INIT_ERR;
	thread_pool[idx].enable_running = BTHREAD_INIT_ERR;
	pthread_exit(NULL);
}		/* -----  end of static function thread_main  ----- */

int clean_thread()
{
	if(thread_pool == NULL){
		return 0;
	}
	
	if(captor_type == CAPTOR_SHUGUANG) {
		close_ppf_drv();	/*close capture drv*/
	}else if(captor_type == CAPTOR_SPECIAL) {
		close_special_drv();
	}
	int i = 0;
	for (i = 0; i < global_thread_num; i++){
		thread_pool[i].thread_main = NULL;
		thread_pool[i].arg = NULL;
	}
	free(thread_pool);
	thread_pool = NULL;

	return 0;
}
#endif
int init_thread()
{
	int result = 0;
	int i = 0;

	result = pthread_attr_init(&thread_attr);
	if(result != 0){
		DMSG("pthread_attr_init error");
		return -1;
	}
	
	result = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(result != 0){
		DMSG("pthread_attr_setdetachstate error");
		return -1;
	}

	thread_pool = (bthread_shared_info_t*)calloc(ngd_conf.thread_num, sizeof(bthread_shared_info_t));
	if(thread_pool == NULL){
		DMSG("calloc error");
		return -1;
	}

	for (i = 0; i < ngd_conf.thread_num; i++) {
		thread_pool[i].thread_main = &plugin_proc_frame;
		thread_pool[i].thread_id = i;
	}
	
//	init_thread_mutex();	// init the init_step_mutex 
	return 0;
}

void thread_make ()
{
	int rst = 0;
	int i = 0;
	for(i = 0; i < ngd_conf.thread_num; i++) {
	    rst = pthread_create(&thread_pool[i].tid, &thread_attr, (void*)(thread_pool[i].thread_main), 
		    &thread_pool[i].thread_id);
	    if(rst != 0) {
		EMSG("pthread_create error when create %d thread", i);
	    }
	    pthread_join(thread_pool[i].tid, NULL);
	}
	return ;
}		/* -----  end of function thread_make  ----- */
#if 0
static int check_bthread_init_stat()
{
	int i = 0;
	for(i = 0; i < global_thread_num; i++) {
		if(thread_pool[i].status == BTHREAD_INIT_ERR){
			continue;
		}else{
			break;
		}
	}

	if(i == global_thread_num){
		return -1;
	}
	
	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  thread_pool
 *  Description:  check the state of bthread.
 *  		  if all of them are BTHREAD_INIT_ERR, exit the process.
 *  		  if some one trap into BTHREAD_STOP, restart it.
 * =====================================================================================
 */
int thread_poll (void *data)
{
	int i = 0;
	
	if(check_bthread_init_stat() == -1){
		wait_exit_mthread();
		exit(0);
	}
	for(i = 0; i < global_thread_num; i++) {
		if(thread_pool[i].status == BTHREAD_STOP){
			thread_make(i);
		}
	}
	return 0;
}		/* -----  end of function thread_poll  ----- */

void reg_thread_poll()
{
	t_hdlr_entry_t timer;
	memset(&timer, 0, sizeof(timer));

	timer.init = NULL;
	timer.clean = NULL;
	timer.proc = thread_poll;
	timer.time_cycle = THREAD_POOL_INTERVAL;
	timer.private_data = NULL;

	reg_timer_hdlr(&timer);
}
#endif
