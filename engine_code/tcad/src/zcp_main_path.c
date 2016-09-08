#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <stdint.h>
#include <pthread.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "misc.h"
#include "common.h"
#include "iplib.h"
#include "libpag.h"
#include "gms_mgmt.h"
#include "service_group.h"
#include "cdpi_api.h"
#include "statistics.h"
#include "ddos_make_log.h"

extern void timer_source_init(void);
extern uint32_t session_table_init(void);
extern uint32_t monitor_init(void);  //wdb_calc222
extern int ip4_init(void);
extern void traffic_init(void);
extern void restore_init(void);
extern int32_t TimerListInit(void);
extern void ether_init(void);
extern int ddos_init(void);
extern void ddos_exit(void);
extern int ddos_load_conf_from_file(void);
extern int ddos_load_debug_conf_from_file(void);
extern int ddos_load_flow_conf_from_file(void);
extern int ddos_load_log_conf_from_file(void);
extern int ddos_load_detect_conf_from_file(void);
extern int ddos_load_postgresql_conf_from_file(void);
extern int ddos_load_baseline_conf_from_file(void);
extern void ddos_mnt_ip_finish(void);
extern int ddos_mnt_ip_init(void);
extern void monitor_exit(void);
void ddos_set_iplist_change(void);
extern volatile int g_ddos_iptrie_conf_reload;

#if 1 /* wdb_as */
extern int utaf_ssn_age_thread_crt(void);
#endif /* wdb_as */

extern int utaf_rx_ex(void *pkt, unsigned int len, int stream_id);

#define UTAF_DECLARE_PER_LCORE(type, name) \
        extern __thread __typeof__(type) utaf_per_lcore_##name
#define UTAF_PER_LCORE(name) (utaf_per_lcore_##name)
UTAF_DECLARE_PER_LCORE(int, lthrd_id);

#if 1 /* wdb_core */
extern int __age_cores_initialized;
extern int __calcu_cores_initialized, __misc_cores_initialized;

extern int parse_age_cores(char *optarg);
extern int parse_calc_cores(char *optarg);
extern int parse_misc_cores(char *optarg);

int g_thread_num = 4;

/* display usage */
static void l2fwd_usage(const char *prgname)
{
    printf("%s [EAL options] -- -p PORTMASK\n"
           "  -p PORTMASK: hexadecimal bitmask of ports to configure\n",
           prgname);
}
#if 0
static int parse_core_args(int argc, char **argv)
{
    int opt, ret;
    char **argvopt;
    int option_index;
    char *prgname = argv[0];
    static struct option lgopts[] = {
        {NULL, 0, 0, 0}
    };

    argvopt = argv;

    while ((opt = getopt_long(argc, argvopt, "a:j:g:",  //wdb_core
                  lgopts, &option_index)) != EOF) {

        switch (opt) {
            case 0:
                l2fwd_usage(prgname);
                return -1;
            case 'a':
                if (parse_age_cores(optarg) < 0)
                {
                    fprintf(stderr, "__BUG__: parse_age_cores a failed.\n");
                    return -1;
                }
                break;
            case 'j':
                if (parse_calc_cores(optarg) < 0)
                {
                    fprintf(stderr, "__BUG__: parse_age_cores j failed.\n");
                    return -1;
                }
                break;
            case 'g':
                if (parse_misc_cores(optarg) < 0)
                {
                    fprintf(stderr, "__BUG__: parse_age_cores g failed.\n");
                    return -1;
                }
                break;

            default:
                l2fwd_usage(prgname);
                return -1;
        }
    }

    if ( !__age_cores_initialized ||
         !__calcu_cores_initialized || !__misc_cores_initialized )
    {
        fprintf(stderr, "__BUG__: Cpu cores are not alloc'ed for all threads.\n");
        return -1;
    }

    ret = optind-1;
    optind = 0; /* reset getopt lib */
    return ret;
}
#endif
#endif /* wdb_core */

static int32_t set_global_vm(void)
{
    struct rlimit r;
    if(getrlimit(RLIMIT_RSS,&r)<0)
    {
        fprintf(stdout,"[Error]: getrlimit error\n");
        return -1;
    }
    printf("RLIMIT_AS cur:%lu\n",r.rlim_cur);
    printf("RLIMIT_AS max:%lu\n",r.rlim_max);
    
    /** set limit **/
    r.rlim_cur=GLOBAL_VM_SIZE;
    r.rlim_max=GLOBAL_VM_SIZE;
    if (setrlimit(RLIMIT_RSS,&r)<0)
    {
        fprintf(stdout,"[Error]: setrlimit error\n");
        return -1;
    }
    /** get value of set **/
    if(getrlimit(RLIMIT_RSS,&r)<0)
    {
        fprintf(stdout,"[Error]: getrlimit error\n");
        return -1;
    }
    printf("RLIMIT_AS cur:%lu\n",r.rlim_cur);
    printf("RLIMIT_AS max:%lu\n",r.rlim_max);

    r.rlim_cur=0;
    r.rlim_max=0;
    if (setrlimit(RLIMIT_CORE,&r)<0)
    {
        fprintf(stdout,"[Error]: setrlimit core error\n");
    }

    return 0;
}

#if 1 /* wdb */
extern int g_page_sz;
#endif /* wdb */

#if 0 /* wdb_core */
extern int parse_hex_cores(char *argv[]);
#endif /* wdb_core */

int dpdk_get_core_id(void)
{
    return UTAF_PER_LCORE(lthrd_id);
}

static int __xxx_lthrd[16];

static void *zcp_process_pkt(void *data)
{
    void *pkt;
    unsigned int len = 0;
    int stream_id = 0;
    //cpu_set_t mask;

    stream_id = *((int *)data);

#if 0
    CPU_ZERO(&mask);
    CPU_SET(stream_id, &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "set thread affinity failed\n");
    }
#endif

    UTAF_PER_LCORE(_lcore_id) = stream_id;
    UTAF_PER_LCORE(lthrd_id) = stream_id;

    //pag_open();
    printf("zcp_process_pkt on %d\n", utaf_lcore_id());

    while (1)
    {
        pkt = (void *)pag_get_tcad(stream_id, &len);
        if (NULL != pkt) {
            utaf_rx_ex(pkt, len, stream_id);

            pag_free(stream_id, pkt);
        }
    }

    //pag_close();

    return NULL;
}

static void zcp_init(void)
{
    pthread_t tid;
    int i;

    for (i = 0; i < g_thread_num; i++)
    {
        __xxx_lthrd[i] = i;
        if ( pthread_create(&tid, NULL, zcp_process_pkt, &__xxx_lthrd[i]) != 0 )
        {
            fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
            return;
        }
        if ( pthread_detach(tid) != 0 )
        {
            fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid, strerror(errno));
        }
    }
}

static void reload_iptrie(void)
{
	/* 
	 *1.destory old trie
	 *2.create new trie
	 */
	g_ddos_iptrie_conf_reload = 1;
	sleep(1);
	
	ddos_mnt_ip_finish();
	
	if (ddos_mnt_ip_init() != 0)
	{
		CA_LOG(LOG_MODULE, LOG_PROC, "create ip trie error, please retry\n");
		goto out;
	}
	/* set flag of ip change */
	ddos_set_iplist_change();
out:
	g_ddos_iptrie_conf_reload = 0;

	return ;
}
static void reload_configurations(void)
{
	int flag = CONF_GET_RELOAD_FLAG();
	
	printf("need reload conf : %d\n", flag);

	switch (flag)
	{
		case CONF_RELOAD_ALL:
			ddos_load_conf_from_file();
	  		CONF_COPY_DDOS_CONF(&g_ddos_conf);
			reload_iptrie();
			/* need recreate threads */
			//
			break;
		case CONF_RELOAD_DDOS_DEBUG:
			ddos_load_debug_conf_from_file();
			break;
		case CONF_RELOAD_DDOS_LOG:
			ddos_load_log_conf_from_file();
			/* need recreate threads */
			break;
		case CONF_RELOAD_DDOS_FLOW:
			ddos_load_flow_conf_from_file();
			/* need recreate threads */
			break;
		case CONF_RELOAD_DDOS_DETECT:
			ddos_load_detect_conf_from_file();
			/* need recreate threads */
			break;
		case CONF_RELOAD_DDOS_IPLIST:
			reload_iptrie();
			break;
		case CONF_RELOAD_DDOS_DATABASE:
			ddos_load_postgresql_conf_from_file();
			break;
		case CONF_RELOAD_DDOS_BASELINE:
			ddos_load_baseline_conf_from_file();
			break;
		case CONF_RELOAD_OTHER:
			break;
		default:
			break;
	}

	CONF_SET_RELOAD_FLAG(0);
	CONF_COPY_DDOS_CONF(&g_ddos_conf);
	return;
}
void thread_num_init(void)
{
    int thread_num = 0;
    char tcad_conf[256] = {0};
    char key[64] = {0};

    sprintf(tcad_conf, "%s/tcad.conf", PROGRAM_DIR);

    // 从配置文件中读取
    tcad_conf_get_profile_string(tcad_conf, "pag", "stream_num", key);

    thread_num = atoi(key);

    printf("get thread_num from conf: %d\n", thread_num);

    if (1 == thread_num || 2 == thread_num || 4 == thread_num
        || 8 == thread_num || 16 == thread_num)
    {
        g_thread_num = thread_num;
    }

    extern uint64_t session_item_member_mask;
    session_item_member_mask = g_thread_num - 1;

    printf("libpag thread number: %d\n", g_thread_num);

    return;
}

void out(int sign_no)
{
    printf("I have get sign_no %d\n", sign_no);
    pag_close();
	ddos_exit();
	monitor_exit();
#ifdef UTAF_URL
	extern void url_end(void);
	url_end();
#endif
	exit(0);
}

int main(int argc, char *argv[])
{
    int retv = 0;

#if 0
    if ( parse_core_args(argc, argv) < 0 )
    {
        fprintf(stderr, "parsing for allocation of cpu cores is failed.\n");
        exit(1);
    }
#endif

#if 1 /* wdb */
    g_page_sz = getpagesize();
    printf("g_page_sz: %d\n", g_page_sz);
#endif /* wdb */

#if 0 /* wdb_core */
    if ( parse_hex_cores(argv) < 0 )
    {
        return 111;
    }
#endif /* wdb_core */

    /* 设置虚拟内存资源限制 */
    if (0 != set_global_vm())
    {
        exit(1);
    }

    thread_num_init();

    if (UTAF_OK != monitor_init())
	{
		printf("sharemem_init error\n");
		exit(1);
	}

    if (0 != cdpi_init(CDPI_DIR, g_thread_num)) {
        printf("cdpi_init error\n");
		exit(1);
    }

#ifdef UTAF_URL
	if (0!=url_init()){
		printf("url_init error...\n");
		exit(-1);
	}	
#endif
	if (0 != ddos_init()) {
		printf("ddos init error\n");
		exit(1);
	}

    // 接收配置信息
    gms_mgmt_init();
    gms_mgmt_recv_thread_start();

    timer_source_init();

    if(UTAF_OK != session_table_init())
    {
        exit(1);
    }

    //printf("===========>ether_init\n");
    (void)ether_init();

    if (UTAF_OK != ip4_init())
	{		
		printf("ip4_init error\n");
		exit(1);
	}

    /* 设置IP地址库文件所在目录 */
    if (IPLIB_OK != iplib_set_dir(PROGRAM_DIR))
    {
		printf("iplib_set_dir error\n");
		exit(1);
    }

    /* IP地址库初始化 */
	if (IPLIB_OK != iplib_init())
	{
		printf("iplib_init error\n");
		exit(1);
	}

	if(UTAF_OK != service_group_init())
	{
		printf("service_group_init error\n");
		exit(1);
	}

    traffic_init();
    restore_init();

    utaf_ssn_age_thread_crt();

    // 初始化定时器
    if (0 != TimerListInit())
    {
        exit(1);
    }

    gms_mgmt_get_all_config();

    retv = pag_open();
    if(retv != 1){
		printf("pag_open() failed.\n");
		exit(1);
	}
    
    zcp_init();

    signal(SIGINT, out);
    signal(SIGTERM, out);

	//add by zdw,found Segmentation fault
    signal(SIGSEGV, out);

    while (1)
    {
        sleep(1);
		if(CONF_NEED_RELOAD())
			reload_configurations();
    }

    pag_close();

    return 0;
}

#if 1 /* wdb_core */
int __misc_cores_initialized = 0;
int __misc_cores;

#define WHITESPACE " \t\r\n"

int parse_cores_string(char *opt, int *ret, int cc)
{
    char *sp, *sp1;
    int core;
    int core_num = 0;

    while ( (sp = strchr(opt, ',')) )
    {
        *sp = 0;
        sp1 = strtok(opt, WHITESPACE);
        if (sp1 && *sp1)
        {
            if (core_num >= cc)
            {
                return -1;
            }
            core = atoi(sp1);
         /* printf("core = %d\n", core); */
            ret[core_num] = core;
            core_num++;
        }
        opt = sp + 1;
    }

    sp1 = strtok(opt, WHITESPACE);
    if (sp1 && *sp1)
    {
        if (core_num >= cc)
        {
            return -1;
        }
        core = atoi(sp1);
     /* printf("core = %d\n", core); */
        ret[core_num] = core;
        core_num++;
    }

    return core_num;
}


int parse_misc_cores(char *optarg)
{
    if ( parse_cores_string(optarg, &__misc_cores, 1) < 1 )
    {
        return -1;
    }

    __misc_cores_initialized = 1;

    return 0;
}
#endif /* wdb_core */
