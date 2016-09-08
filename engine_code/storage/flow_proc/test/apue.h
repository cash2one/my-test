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

/*typedef struct test{
    int protocol_id ;
    char ftime[25];
    float pps;
    float bps;
    long long sip;
    long long dip;
    int query_id;
    int month_id;


}Test;*/
typedef struct flow_struct{
	uint8_t     protocol_id;        /* 协议id 1 代表ALL 2代表TCP 3代表UDP 4代表OTHERS */
	time_t      ftime;              /* 时间以秒为单位 */
	uint64_t    pps;                /* 收包数 */
	uint64_t    bps;                /* 字节数 */
	uint32_t    start_ip;           /* 除自定义流量外,其他为0 */
	uint32_t    end_ip;             /* 除自定义流量外,其他为0 */
}Test;

void init_test(Test*p)
{
	p->protocol_id = 1;
//	p->ftime=1426670973U;

	time(&p->ftime);//获取当前时间
	p->pps=30;
	p->bps=50;
	p->start_ip = 0;
	p->end_ip = 0;
}


static inline void myperror(const char *p1, const char *p2)
{
	fprintf(stderr, "%s(%s):%s\n", p1, p2, strerror(errno));
}


#endif
