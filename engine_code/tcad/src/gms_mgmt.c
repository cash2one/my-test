#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>
#include <fcntl.h>
#include "tcad.h"
#include "traffic.h"
#include "common.h"
#include "cdpi_api.h"
#include "restore.h"
#include "iplib.h"
#include "statistics.h"

int udp_server_socket = 0;
struct sockaddr_in udp_enevt_log_to;
struct sockaddr_in udp_define_flow_to;
char udp_server_ip[64] = {0};
uint32_t flow_interval = 0;
uint32_t incr_interval = 0;

extern volatile time_t g_utaf_time_jiffies;

#if 1 /* wdb_core */
extern int __misc_cores;
#endif /* wdb_core */

char * ipv4_addr_format(char *buf, uint32_t ip_addr)
{
    sprintf(buf, "%d.%d.%d.%d",
                (ip_addr >> 24) & 0xFF, (ip_addr >> 16) & 0xFF,
                (ip_addr >> 8) & 0xFF, ip_addr & 0xFF);
    return buf;
}

void gms_mgmt_udp_server_to_init(void)
{
    memset((char *) &udp_define_flow_to, 0, sizeof(udp_define_flow_to));
    udp_define_flow_to.sin_family = AF_INET;
    udp_define_flow_to.sin_addr.s_addr = inet_addr(udp_server_ip);
    udp_define_flow_to.sin_port = htons(DEFINE_FLOW_PORT);

    memset((char *) &udp_enevt_log_to, 0, sizeof(udp_enevt_log_to));
    udp_enevt_log_to.sin_family = AF_INET;
    udp_enevt_log_to.sin_addr.s_addr = inet_addr(udp_server_ip);
    udp_enevt_log_to.sin_port = htons(FLOW_EVENT_PORT);

    DEBUG_GMS_UDP_SERVER(inet_addr(udp_server_ip));
    DEBUG_GMS_DEFINE_FLOW_PORT(DEFINE_FLOW_PORT);
    DEBUG_GMS_FLOW_EVENT_PORT(FLOW_EVENT_PORT);
    
    return;
}

int gms_mgmt_udp_server_socket_init(void)
{
    int s;

    if ( (s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return -1;
    }
    
    udp_server_socket = s;

    DEBUG_GMS_UDP_SERVER_SOCKET(udp_server_socket);

    gms_mgmt_udp_server_to_init();
    
    return 0;
}

uint32_t gms_mgmt_process(char *buf)
{
    uint8_t action = 0;
    uint32_t ret = 0;
    
    if (NULL == buf) {
        return 1;
    }

    action = *((uint8_t *)buf);
    printf("%s get action %u\n", __FUNCTION__, action);

    switch (action) {
        case TCAD_ACTION_FLOW_ADD:
        {
            flow_defined_t *t = (flow_defined_t *)buf;

            printf("%s get flow add 0x%x --> 0x%x\n", __FUNCTION__, t->start_ip, t->end_ip);
            ret = traffic_rule_add(t->start_ip, t->end_ip);
            break;
        }
        case TCAD_ACTION_FLOW_DEL:
        {
            flow_defined_t *t = (flow_defined_t *)buf;

            printf("%s get flow del 0x%x --> 0x%x\n", __FUNCTION__, t->start_ip, t->end_ip);
            ret = traffic_rule_del(t->start_ip, t->end_ip);
            break;
        }
        case TCAD_ACTION_SET_FLOW_INTERVAL:
        {
            flow_interval_t *f = (flow_interval_t *)buf;
            flow_interval = f->itime;
            printf("%s get flow interval: %u\n", __FUNCTION__, flow_interval);
            break;
        }
        case TCAD_ACTION_SET_INCR_INTERVAL:
        {
            incr_interval_t *i = (incr_interval_t *)buf;
            incr_interval = i->itime;
            printf("%s get incr interval: %u\n", __FUNCTION__, incr_interval);
            break;
        }
        case TCAD_ACTION_SET_UDP_SERVER_IP:
        {
            char tbuf[64] = {0};
            udp_server_t *t = (udp_server_t *)buf;
            
            ipv4_addr_format(tbuf, t->server_ip);

            printf("%s get udp server ip: %s\n", __FUNCTION__, tbuf);
            strncpy(udp_server_ip, tbuf, 63);
            gms_mgmt_udp_server_to_init();
            break;
        }
        case TCAD_ACTION_BATCH:
            break;
        case TCAD_ACTION_RESTORE_ADD:
        {
            uint16_t rid = 0;
            flow_restore_t *t = (flow_restore_t *)buf;
            
            printf("%s get flow restore add: %s\n", __FUNCTION__, t->buf);

            rid = cdpi_custom_features_operate(1, t->buf, strlen(t->buf));

            if (0 != rid) {
                restore_add(rid, t->name, t->buf);
            } else {
                printf("%s get flow restore add %s failed\n", __FUNCTION__, t->name);
                ret = TCAD_RETCODE_FAILED;
            }
            break;
        }
        case TCAD_ACTION_RESTORE_DEL:
        {
            uint16_t rid = 0;
            flow_restore_t *t = (flow_restore_t *)buf;
            
            printf("%s get flow restore del: %s\n", __FUNCTION__, t->buf);

            rid = cdpi_custom_features_operate(2, t->buf, strlen(t->buf));

            if (0 != rid) {
                restore_del(rid, t->buf);
            } else {
                printf("%s get flow restore del %s failed\n", __FUNCTION__, t->name);
                ret = TCAD_RETCODE_FAILED;
            }
            break;
        }
        case TCAD_ACTION_RESTORE_MOD:
        {
            uint16_t rid = 0;
            flow_restore_t *t = (flow_restore_t *)buf;
            
            printf("%s get flow restore mod: %s\n", __FUNCTION__, t->buf);
            //strncpy(udp_server_ip, tbuf, 63);

            rid = cdpi_custom_features_operate(1, t->buf, strlen(t->buf));

            if (0 != rid) {
                restore_mod(rid, t->buf);
            } else {
                printf("%s get flow restore mod %s failed\n", __FUNCTION__, t->name);
                ret = TCAD_RETCODE_FAILED;
            }
            break;
        }
        case TCAD_ACTION_IPINFO_ADD:
        {
            ipinfo_t ii;
            ip_info_t *t = (ip_info_t *)buf;

            printf("%s get ipinfo add: 0x%x -> 0x%x %u %u %u %u\n", __FUNCTION__, t->ip_start, t->ip_end,
                    t->country, t->province_id, t->city_id, t->isp_id);

            ii.ip_start = t->ip_start;
            ii.ip_end = t->ip_end;
            ii.province = t->province_id;
            ii.city = t->city_id;
            ii.service = t->isp_id;
            ii.country = t->country;
            ii.index = 0;
            
            if (IPLIB_OK != iplib_insert(&ii)) {
                printf("%s get ipinfo add failed\n", __FUNCTION__);
                ret = TCAD_RETCODE_FAILED;
            }
            break;
        }
        case TCAD_ACTION_IPINFO_DEL:
        {
            ipinfo_t ii;
            ip_info_t *t = (ip_info_t *)buf;

            printf("%s get ipinfo del: 0x%x -> 0x%x %u %u %u %u\n", __FUNCTION__, t->ip_start, t->ip_end,
                    t->country, t->province_id, t->city_id, t->isp_id);

            ii.ip_start = t->ip_start;
            ii.ip_end = t->ip_end;
            ii.province = t->province_id;
            ii.city = t->city_id;
            ii.service = t->isp_id;
            ii.country = t->country;
            ii.index = 0;
            
            if (IPLIB_OK != iplib_delete(&ii)) {
                printf("%s get ipinfo del failed\n", __FUNCTION__);
                ret = TCAD_RETCODE_FAILED;
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

int gms_mgmt_recv_data(int sockfd, void * buf, size_t len)
{
    size_t nleft;
    size_t nread;
    char *ptr;
    //tcad_action_t *a = NULL;

    ptr = (char *)buf;
    nleft = len;

    while (nleft > 0) {
        if ((nread = recv(sockfd, ptr, nleft, 0)) < 0) {
            if (errno == EINTR)//是由于信号中断造成的读失败，则继续读
                continue;
            else //否则是真正的读错误，返回失败的标志
                return -1;
        } else {
            if (0 == nread)//数据读取完毕了，到达了文件的末尾
                break;
        }

        /*if (nleft == len) {
            a = (tcad_action_t *)buf;
            if (TCAD_ACTION_ADD != a->action) {
                break;
            }
        }*/
        
        nleft -= nread;//剩余要读取的
        ptr += nread;//偏移缓冲区位置
    }

    return (len - nleft);
}

void *gms_mgmt_recv(void *arg)
{
    int sockfd = 0, new_fd = 0, opt = 1, iDataNum = 0;
    char *buffer = NULL;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    uint32_t ret = 0;
    socklen_t sin_size = 0;
    //cpu_set_t mask;
    char thread_name[32] = {0};
    tcad_retcode_t retcode = {0};

#define BUF_MAX_LEN (sizeof(flow_restore_t) + 1)

    /* 线程脱离创建者*/
    pthread_detach ( pthread_self() );

    sprintf(thread_name, "dp_task_recv");
    set_thread_title(thread_name);

#if 0
    CPU_ZERO(&mask);
#if 0 /* wdb_core */
    CPU_SET(0, &mask);
#else /* wdb_core */
    CPU_SET(__misc_cores, &mask);
#endif /* wdb_core */
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "set thread affinity failed\n");
    }
#endif

    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
        fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
        return NULL;
    }

    buffer = (char *)malloc(BUF_MAX_LEN);
    if (NULL == buffer) {
        goto err;
    }
    
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(TCAD_SRV_PORT);
    
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));
    
    if (-1 == bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))) {
        fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
        goto err;
    }

    if (listen(sockfd, 5) == -1) {
        fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
        goto err;
    }

    while (1) {
        memset(buffer, 0, BUF_MAX_LEN);
        printf("Waiting...\n");
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) == -1) {
            fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
            continue;
        }

        printf("Server get connection from %s\n", inet_ntoa(client_addr.sin_addr));

        iDataNum = gms_mgmt_recv_data(new_fd, buffer, BUF_MAX_LEN - 1);
        if (iDataNum < 0) {
            //ret = 1;
            printf("Recv data len: %d\n", iDataNum);
            retcode.retcode = TCAD_RETCODE_FAILED;
            send(new_fd, &retcode, sizeof(tcad_retcode_t), 0);
            close(new_fd);
            continue;
        }
        
        printf("Recv data len: %d\n", iDataNum);

        if (0 != iDataNum) {
            ret = gms_mgmt_process(buffer);
        } else {
            ret = TCAD_RETCODE_OK;
        }

        retcode.retcode = ret;
        printf("Send data: %d\n", ret);
        send(new_fd, &retcode, sizeof(tcad_retcode_t), 0);
        close(new_fd);
    }
    
err:
    if (buffer) {
        free(buffer);
    }
    
    close(sockfd);
    
    return NULL;
}

pthread_t tid_gms_mgmt_recv;
void gms_mgmt_recv_thread_start(void)
{
    int err;
    printf("gms_mgmt_recv_thread_start!\n");
    
    // 创建任务接收线程
    err = pthread_create(&tid_gms_mgmt_recv, NULL, gms_mgmt_recv, NULL);
    if (0 != err) {
        printf("can not create gms mgmt recv thread\n");
    }
}

void gms_mgmt_init(void)
{
    strncpy(udp_server_ip, "127.0.0.1", 63);
    flow_interval = DEFAULT_INTERVAL_FLOW;
    incr_interval = DEFAULT_INTERVAL_INCR;

    gms_mgmt_udp_server_socket_init();
    
    return;
}

int gms_mgmt_socket_init(void)  //wdb_calc222
{
    int sockfd;
    struct sockaddr_in server_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd == -1) {
        printf("Socket Error:%s\n", strerror(errno));
        return -1;
    }
    
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(GMS_SRV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) {
        printf("Connect error:%s\n",strerror(errno));
        return -1;
    }

    /* 设置 socket 属性为非阻塞方式 */
    if(fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        printf("fcntl error:%s\n",strerror(errno));
        return -1;
    }

    return sockfd;
}

int gms_mgmt_send_rcv(void *data, int length, char *task_buf, int task_len)
{
    int ret = 0, sockfd = 0;
    
    sockfd = gms_mgmt_socket_init();
    
    if (-1 == sockfd) {
        return 1;
    }
    
    if (-1 == send(sockfd, data, length, 0)) {
        close(sockfd);
        return 1;
    }
    
    ret = recv(sockfd, task_buf, task_len, 0);
    
    if (ret <= 0) {
        close(sockfd);
        return 1;
    }
    
    close(sockfd);
    
    return 0;
}

void gms_mgmt_get_all_config(void)
{
    int ret = 0, tmp = 0;
    tcad_action_t action = {0};
    char taskbuffer[4096] = {0};

    action.action = TCAD_ACTION_BATCH;
    
    ret = gms_mgmt_send_rcv(&action, sizeof(tcad_action_t), taskbuffer, sizeof(taskbuffer));
    if (0 == ret) {
        register int *_tmp;  //wdb_calc222
        _tmp = (int *)taskbuffer;  //wdb_calc222
        tmp = *_tmp;  //wdb_calc222

        printf("%s get task num: %d\n", __FUNCTION__, tmp);
    }
    
    return;
}


