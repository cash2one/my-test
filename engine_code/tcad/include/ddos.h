#ifndef __DDOS_H__
#define __DDOS_H__
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DDOS_DEFAULT_LOG_INTERVAL 10 /* 10s */
#define DDOS_DEFAULT_DET_INTERVAL 60 /* default detect cycle 10s */
#define DDOS_DEFAULT_DET_RATIO 1.5 /* radio  */
#define DDOS_DEFAULT_DET_THREAT 2  /* detect thread num */
/** postgresql information**/
#define DDOS_POSTGRES_OFF  		0
#define DDOS_POSTGRES_IP  		"127.0.0.1"
#define DDOS_POSTGRES_PORT		"5432"
#define DDOS_POSTGRES_DB		"gms_db"
#define DDOS_POSTGRES_USER		"gms_user"
#define DDOS_POSTGRES_pwd		"123456"
#define DDOS_POSTGRES_unix		0
#define DDOS_POSTGRES_domain	"/tmp"
/** base conifg **/
#define DDOS_BASE_LEARN			1
#define DDOS_BASE_FLOW_IN			(100*1024*1024)
#define DDOS_BASE_FLOW_OUT			(1000*1024*1024)
#define DDOS_BASE_NEW_TCP_IN		(1000)
#define DDOS_BASE_NEW_TCP_OUT	    (1000)
#define DDOS_BASE_LIVE_TCP_IN	    (100000)
#define DDOS_BASE_LIVE_TCP_OUT		(100000)
#define DDOS_BASE_HOUR			24
#define DDOS_BASE_DAY			7
#define DDOS_BASE_WEEK			7
#define DDOS_FLOW_RATIO			0.9


/* 检测指标掩码 */
#define DDOS_DEFAULT_DET_TYPE_MASK 510
#define DDOS_DEFAULT_DET_DIR_MASK 2  /* detect dir mask */
#define DDOS_DEFAULT_DET_UNIT_MASK 3  /* detect unit mask */
#define DDOS_DEFAULT_T100_INTERVAL 300 /* top100 cycle */
#define DDOS_DEFAULT_LOG_THREAD_NUM 1

#define IP_STR_SIZE                 16  /* IP地址字符串长度 */
#define DEVID_SIZE                  16  /* IP地址字符串长度 */
#define PORT_STR_SIZE               10  /* PORT字符串长度 */
#define DBNAME_SIZE                 50  /* 数据库名长度 */
#define USER_NAME_SIZE              50  /* 用户名长度 */
#define PASSWORD_SIZE               100 /* 密码长度 */
#define DOMAIN_SOCK					100 
#define TIME_FORMAT_SIZE    		25

#define SUCCE 1
#define FAIL -1

#define DDOS_MID_FLOW       0X01  /* flow manage module id */
#define DDOS_MID_STAT       0X02  /* statistic module id */
#define DDOS_MID_LOG        0x04  /* log out module id */
#define DDOS_MID_BASE_LINE  0X08  /* base line module id */
#define DDOS_MID_IPTIRE     0X10  /* ip trie */
#define DDOS_MID_STORE      0X20  /* log store */
#define DDOS_MID_DETECT     0X40  /* detect */


#define DDOS_DEBUG
#define DDOS_DBG_F_PATH_LEN	128

#ifdef DDOS_DEBUG
extern pthread_mutex_t g_ddos_debug_lock;
char g_ddos_debug_f_path[DDOS_DBG_F_PATH_LEN];


#define ddos_debug(mod, fmt, arg...) do\
{\
	if(unlikely(g_ddos_conf.debug & mod))\
	{\
		printf("%u:%s:%d >>> ", (unsigned int)pthread_self(), __func__, __LINE__);\
		printf(fmt, ##arg);\
	}\
}while (0)
#define ddos_debug_f(mod, fmt, arg...) do\
{\
	if(unlikely(g_ddos_conf.debug & mod))\
	{\
		FILE *_fd = NULL;\
		char logtime[20];\
		if ((_fd = fopen(g_ddos_debug_f_path, "a+")) != NULL){\
			time_t _timer = time(NULL);\
			strftime(logtime, 20, "%Y-%m-%d %H:%M:%S", localtime(&_timer));\
			fprintf(_fd, "[%s][%u][%s] " fmt, logtime, (unsigned int)pthread_self(), __FUNCTION__, ##arg);\
			fclose(_fd);}\
	}\
}while (0)

/* with mutex lock */
#define ddos_debug_lf(mod, fmt, arg...) do\
{\
	if(unlikely(g_ddos_conf.debug & mod))\
	{\
		FILE *_fd = NULL;\
		char logtime[20];\
		if ((_fd = fopen(g_ddos_debug_f_path, "a+")) != NULL){\
			time_t _timer = time(NULL);\
			pthread_mutex_lock(&g_ddos_debug_lock);\
			strftime(logtime, 20, "%Y-%m-%d %H:%M:%S", localtime(&_timer));\
			fprintf(_fd, "[%s][%u][%s] " fmt, logtime, (unsigned int)pthread_self(), __FUNCTION__, ##arg);\
			pthread_mutex_unlock(&g_ddos_debug_lock);\
			fclose(_fd);}\
	}\
}while (0)
#define ddos_debug_lock() do {\
	pthread_mutex_lock(&g_ddos_debug_lock);\
}while(0)

#define ddos_debug_unlock() do {\
		pthread_mutex_unlock(&g_ddos_debug_lock);\
}while(0)

#else
#define ddos_debug(mod, fmt, arg...) do {} while (0)
#define ddos_debug_f(mod, fmt, arg...) do {} while (0)
#define ddos_debug_lf(mod, fmt, arg...) do {} while (0)
#define ddos_debug_lock() do {} while(0)
#define ddos_debug_unlock() do {} while(0)

#endif


#define IPV4FMT "d.%d.%d.%d"
#if defined(LITTLE_ENDIAN)
#define IPV4P(_ip) ((uint8_t *)_ip)[3],((uint8_t *)_ip)[2],\
		((uint8_t*)_ip)[1],((uint8_t *)_ip)[0]
#elif defined(BIG_ENDIAN)
#define IPV4P(_ip) ((uint8_t *)_ip)[0],((uint8_t *)_ip)[1],\
		((uint8_t*)_ip)[2],((uint8_t *)_ip)[3]
#else
# error "Byte order must be defined"
#endif  

#define DDOS_CONF_NOT_CHANGE       0x00
#define DDOS_CONF_CHANGE_IPLIST    0x01
#define DDOS_CONF_HANDLE_OVER      0XFF

#define ddos_fetch_and_dec(x) __sync_fetch_and_sub ((volatile int *)(x), (int)1)
#define ddos_fetch_and_add(x) __sync_fetch_and_add ((volatile uint32_t *)(x), (int)1)

/*  */
enum {
	UTAF_DO_NOT_DDOS,
	UTAF_DO_DDOS
};

/* monitor ip type */
enum {
	e_ddos_ip_type_not_found,
	e_ddos_ip_type_monitor, 
	e_ddos_ip_type_white,
	e_ddos_ip_type_max
};

/* 0 - dir_out, 1 - dir_in. */
enum flow_dirction
{
	dir_out,
	dir_in,
	flow_dir
};


/* ddos statistic type */
enum
{
	e_ddos_stat_type_tcp,
	e_ddos_stat_type_tcp_syn,
	e_ddos_stat_type_tcp_ack,
	e_ddos_stat_type_chargen,
	e_ddos_stat_type_icmp,
	e_ddos_stat_type_dns,
	e_ddos_stat_type_ntp,
	e_ddos_stat_type_ssdp,
	e_ddos_stat_type_snmp,
	e_ddos_stat_type_max
	/* 修改以上代码一定要对应修改ddos.conf中相关检测指标掩码
	 * 含义的更新， 同时修改默认配置， 同时检查对攻击判定
	 * 函数的修改 ddos_detect_judge() */
};

#ifndef TCP_NONE
#define TCP_NONE 0
#endif
#ifndef TCP_CLOSED
#define TCP_CLOSED (TCP_CLOSING+1)
#endif

#define DDOS_CONNECT_NEW    1
#define DDOS_CONNECT_CLOSE  2

#define DDOS_DET_MASK_PPS   0x01
#define DDOS_DET_MASK_BPS   0x02

typedef struct ddos_counter
{
	uint64_t pkts;
	uint64_t bytes;
}ddos_counter_t;

typedef struct ddos_rate
{
	uint64_t pps;
	uint64_t bps;
}ddos_rate_t;
typedef struct ddos_group_counter
{
		ddos_counter_t c[e_ddos_stat_type_max][flow_dir];
}ddos_group_counter_t;

typedef struct ddos_group_rate
{
		ddos_rate_t r[e_ddos_stat_type_max][flow_dir];
}ddos_group_rate_t;

typedef struct ddos_stat_norm
{
	int64_t tcp_new_conn_s[flow_dir];
	int64_t tcp_live_conn_s[flow_dir];
	ddos_rate_t rate[e_ddos_stat_type_max][flow_dir];
}ddos_stat_norm_t;
/* -------------------------------------------*/
/**
 * @brief  PostgreSQL base information
 */
/* -------------------------------------------*/
struct gpq_conn_info {
	uint8_t postgresql_off;
	char 	host_ip[IP_STR_SIZE];
	char 	host_port[PORT_STR_SIZE];
	char 	dbname[DBNAME_SIZE];
	char 	user[USER_NAME_SIZE];
	char 	pwd[PASSWORD_SIZE];
	uint8_t unix_val;
	char 	domain[DOMAIN_SOCK];
};


/* ddos attack type */
#define DDOS_ATTACK_TYPE_SHIFT_TCP      (1<<e_ddos_stat_type_tcp)
#define DDOS_ATTACK_TYPE_SHIFT_TCP_SYN  (1<<e_ddos_stat_type_tcp_syn)
#define DDOS_ATTACK_TYPE_SHIFT_TCP_ACK  (1<<e_ddos_stat_type_tcp_ack)
#define DDOS_ATTACK_TYPE_SHIFT_CHARGEN  (1<<e_ddos_stat_type_chargen)
#define DDOS_ATTACK_TYPE_SHIFT_ICMP     (1<<e_ddos_stat_type_icmp)
#define DDOS_ATTACK_TYPE_SHIFT_DNS      (1<<e_ddos_stat_type_dns)
#define DDOS_ATTACK_TYPE_SHIFT_NTP      (1<<e_ddos_stat_type_ntp)
#define DDOS_ATTACK_TYPE_SHIFT_SSDP     (1<<e_ddos_stat_type_ssdp)
#define DDOS_ATTACK_TYPE_SHIFT_SNMP     (1<<e_ddos_stat_type_snmp)
#define DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_NEW  (1<<(e_ddos_stat_type_snmp+1))
#define DDOS_ATTACK_TYPE_SHIFT_TCP_CONN_LIVE (1<<(e_ddos_stat_type_snmp+2))
/* 修改以上代码一定要对应修改ddos.conf中相关检测指标掩码
 * 含义的更新， 同时修改默认配置， 同时检查对攻击判定
 * 函数的修改 ddos_detect_judge() */

typedef struct ddos_statistic
{
	union {
		struct {
			union {
				ddos_counter_t tcp[flow_dir];			
				ddos_counter_t chargen[flow_dir];
			};
			ddos_counter_t syn[flow_dir];
			ddos_counter_t ack[flow_dir];
		}tcp;

		struct {
			union{
				ddos_counter_t udp[flow_dir];
				ddos_counter_t chargen[flow_dir];
				ddos_counter_t dns[flow_dir];
				ddos_counter_t ntp[flow_dir];
				ddos_counter_t ssdp[flow_dir];
				ddos_counter_t snmp[flow_dir];
			};
		}udp;

		struct {
			ddos_counter_t icmp[flow_dir];
		}icmp;
	};	
}ddos_statistic_t;

struct ddos
{
	uint64_t last_log_time; /* last time of sending log to statistic mod */
	ddos_statistic_t stat;
};

struct ddos_conf
{
	uint8_t  sw; /* switch, open:1 close:0*/
	uint8_t  detect_thread_num; /* detect thread number */
	uint8_t log_thread_num;/*log thread number*/
	uint16_t  debug; /* ddos modole debug switch */ 
	uint32_t interval;
	uint32_t detect_interval; /* detect cycle */
	float    detect_ratio;    /* ratio */
	uint16_t detect_dir_mask; /* dir mask */
	uint16_t detect_unit_mask; /* unit mask */
	uint32_t detect_type_mask; /* type mask */
	uint32_t top100_interval;
	struct 	 gpq_conn_info gpq_info;
	char  	 devid[DEVID_SIZE];
	float    flow_ratio;    /* flow ratio */
	uint8_t	 hour_num;
	uint8_t  day_num;
	uint8_t  week_num;
	uint8_t  base_sw;
	uint64_t threshold_flow_in;
	uint64_t threshold_flow_out;
	uint64_t threshold_new_tcp_in;
	uint64_t threshold_new_tcp_out;
	uint64_t threshold_live_tcp_in;
	uint64_t threshold_live_tcp_out;
};

static inline uint32_t ddos_get_ip_hash(uint32_t ip, uint32_t mask)
{
	return (ip%(mask-1));
}

static __inline__ void ddos_atomic_set(int i, volatile int *target)
{
	__asm__ __volatile__(
			"lock; xchgl %1,%0"
			:"=m" (*target)
			:"ir" (i), "m" (*target));
}

static __inline__ void ddos_atomic_add64(unsigned long i, volatile time_t *target)
{
	__asm__ __volatile__(
			"lock; add %1,%0"
			:"=m" (*target)
			:"ir" (i), "m" (*target));
}

#define ddos_get_mnt_ip(si) \
				  ((dir_in == si->flow_dir)?si->ip_tuple.dip[0]:si->ip_tuple.sip[0])
#define ddos_get_oppo_mnt_ip(si) \
					  ((dir_in == si->flow_dir)?si->ip_tuple.sip[0]:si->ip_tuple.dip[0])


extern struct ddos_conf g_ddos_conf;

#endif
