#ifndef APUE_H
#define APUE_H
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <shadow.h>
#include <grp.h>
#include <dirent.h>
#include <glob.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <mqueue.h>
/*******************network****************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/epoll.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#define EXIT -2
#define TIME_FORMAT_SIZE 25
#define STR_SIZE 128
#define SAM_STR_SIZE 31


typedef struct test{
	time_t          ftime_start;            /* 开始时间，自1970.1.1年以来的秒数 */
	time_t          ftime_end;              /* 结束时间，自1970.1.1年以来的秒数 */
	time_t          visit_time;             /* 访问时长，单位秒 */
	uint32_t        sip;                    /* 用户ip */
	uint32_t        dip;                    /* 目的ip */
	uint16_t        usersport;              /* 用户源端口号 */
	uint16_t        userdport;              /* 目的端口号 */
	uint8_t         procotol_id;            /* 协议id: 2代表TCP 3代表UDP 4代表OTHERS */
	uint8_t         country;                /* 国家 */
	uint8_t         pc_if;                  /* 0 为pc端 ；1 为移动端 */
	uint8_t         tcp_suc;                /* tcp连接是否成功 0 tcp连接失败，1为连接成功 */
	uint8_t         bank_visit;             /* 业务访问是否成功：如：http 返回200认为成功 */
	uint8_t         flag;                   /* 0为非长链接，1 为长连接 */
	uint8_t         incr;                   /* 0表示增量日志，1表示老化 */
	uint8_t         res;                    /* 保留字段 */
	uint16_t        s_isp_id;               /* 源运营商ID */
	uint16_t        d_isp_id;               /* 目的运营商ID */
	uint32_t        s_province_id;          /* 源省ID */
	uint32_t        d_province_id;          /* 目的省ID */
	uint32_t        s_city_id;              /* 源地市ID */
	uint32_t        d_city_id;              /* 目的地市ID */
	uint64_t        ip_byte;                /* 字节数 */
	uint8_t         direction_flag;         /* 0 = in 1 = out*/
	uint64_t        up_bytes;               /* 上行流量*/
	uint64_t        down_bytes;             /* 下行流量*/
	uint16_t        service_group_id;       /* 应用类型组id*/
	uint32_t        service_type_id;        /* 应用类型id*/
	uint64_t        ip_pkt;                 /* 数据包 */
	time_t          cli_ser;                /* 端到端的时延:所有包的平均值，单位毫秒 */
	int			    browser;/* 浏览器类型，例：IE、chrome 等 */
	unsigned char   version[SAM_STR_SIZE+1];/* 浏览器版本 */
}Test;


static inline void myperror(const char *p1, const char *p2)
{
	fprintf(stderr, "%s(%s):%s\n", p1, p2, strerror(errno));
}


#endif
