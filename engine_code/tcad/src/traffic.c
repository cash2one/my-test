#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <time.h>

#include "misc.h"
#include "mbuf.h"
#include "tcad.h"
#include "statistics.h"
#include "rwlock.h"

time_t g_traffic_last_time = 0;
static rwlock_t traffic_rule_lock;

extern volatile time_t g_utaf_time_jiffies;
extern int udp_send_log(char *log_info, int length);

#define UTAF_DECLARE_PER_LCORE(type, name) \
        extern __thread __typeof__(type) utaf_per_lcore_##name
#define UTAF_PER_LCORE(name) (utaf_per_lcore_##name)
UTAF_DECLARE_PER_LCORE(int, lthrd_id);

typedef struct tfc_output
{
    unsigned long pkts;
    unsigned long bytes;
} tfc_output_t;

typedef struct tfc_rule
{
    struct tfc_rule *next;
    uint32_t start_ip;
    uint32_t stop_ip;
    unsigned int rl_id;
    tfc_output_t tcp[16];
    tfc_output_t udp[16];
    tfc_output_t icmp[16];
    tfc_output_t other[16];
    uint64_t last_pkts_all;
    uint64_t last_pkts_tcp;
    uint64_t last_pkts_udp;
    uint64_t last_pkts_other;
    uint64_t last_bytes_all;
    uint64_t last_bytes_tcp;
    uint64_t last_bytes_udp;
    uint64_t last_bytes_other;
} tfc_rule_t;

static tfc_rule_t tfc_all = {0};
static tfc_rule_t *tfc_rule_list = NULL;

void check_traffic_rules(struct m_buf *mbuf, struct iphdr *ip)
{
    tfc_rule_t *rl;
    uint32_t tmp1, tmp2;

    switch ( ip->protocol )
    {
        case IPPROTO_TCP:
            tfc_all.tcp[UTAF_PER_LCORE(lthrd_id)].pkts  += 1;
            tfc_all.tcp[UTAF_PER_LCORE(lthrd_id)].bytes += mbuf->total_len;
            break;
        case IPPROTO_UDP:
            tfc_all.udp[UTAF_PER_LCORE(lthrd_id)].pkts  += 1;
            tfc_all.udp[UTAF_PER_LCORE(lthrd_id)].bytes += mbuf->total_len;
            break;
        default:
            tfc_all.other[UTAF_PER_LCORE(lthrd_id)].pkts += 1;
            tfc_all.other[UTAF_PER_LCORE(lthrd_id)].bytes += mbuf->total_len;
            break;
    }

    tmp1 = ntohl(ip->saddr);
    tmp2 = ntohl(ip->daddr);

    read_lock(&traffic_rule_lock);
    for (rl = tfc_rule_list; rl; rl = rl->next)
    {
        if ( (rl->start_ip <= tmp1 && tmp1 <= rl->stop_ip) ||
             (rl->start_ip <= tmp2 && tmp2 <= rl->stop_ip) )
        {
            switch ( ip->protocol )
            {
                case IPPROTO_TCP:
                    rl->tcp[UTAF_PER_LCORE(lthrd_id)].pkts  += 1;
                    rl->tcp[UTAF_PER_LCORE(lthrd_id)].bytes += mbuf->total_len;//(ntohs(ip->tot_len) + 14);
                    break;
                case IPPROTO_UDP:
                    rl->udp[UTAF_PER_LCORE(lthrd_id)].pkts  += 1;
                    rl->udp[UTAF_PER_LCORE(lthrd_id)].bytes += mbuf->total_len;//(ntohs(ip->tot_len) + 14);
                    break;
                /*case IPPROTO_ICMP:
                    rl->icmp[UTAF_PER_LCORE(lthrd_id)].pkts += 1;
                    rl->icmp[UTAF_PER_LCORE(lthrd_id)].bytes += (ntohs(ip->tot_len) + 14);
                    break;*/
                default:
                    rl->other[UTAF_PER_LCORE(lthrd_id)].pkts += 1;
                    rl->other[UTAF_PER_LCORE(lthrd_id)].bytes += mbuf->total_len;//(ntohs(ip->tot_len) + 14);
                    break;
            }
        }
    }
    read_unlock(&traffic_rule_lock);
    
}

uint32_t traffic_rule_check(uint32_t start_ip, uint32_t end_ip)
{
    tfc_rule_t *rl;

    read_lock(&traffic_rule_lock);
    for (rl = tfc_rule_list; rl; rl = rl->next) {
        if (rl->start_ip == start_ip && rl->stop_ip == end_ip) {
            read_unlock(&traffic_rule_lock);
            return 1;
        }
    }
    read_unlock(&traffic_rule_lock);
    
    return 0;
}

uint32_t traffic_rule_add(uint32_t start_ip, uint32_t end_ip)
{
    tfc_rule_t *rl;
    int i;

    if (0 != traffic_rule_check(start_ip, end_ip)) {
        return TCAD_RETCODE_FLOW_EXIST;
    }

    rl = malloc(sizeof(tfc_rule_t));

    if (NULL == rl) {
        return TCAD_RETCODE_FAILED;
    }

    memset(rl, 0, sizeof(tfc_rule_t));

    rl->start_ip = start_ip; //inet_network("192.168.1.200"); /* host byte order */
    rl->stop_ip  = end_ip;   //inet_network("192.168.1.254"); /* host byte order */

    for (i = 0; i < 16; i++)
    {
        rl->tcp[i].pkts    = 0;
        rl->tcp[i].bytes   = 0;
        rl->udp[i].pkts    = 0;
        rl->udp[i].bytes   = 0;
        rl->icmp[i].pkts   = 0;
        rl->icmp[i].bytes  = 0;
        rl->other[i].pkts  = 0;
        rl->other[i].bytes = 0;
    }

    write_lock(&traffic_rule_lock);
    rl->next = tfc_rule_list;
    tfc_rule_list = rl;
    write_unlock(&traffic_rule_lock);

    return TCAD_RETCODE_OK;
}

uint32_t traffic_rule_del(uint32_t start_ip, uint32_t end_ip)
{
    tfc_rule_t *rl, *tmp = NULL;

    write_lock(&traffic_rule_lock);
    for (rl = tfc_rule_list; rl; rl = rl->next) {
        if (rl->start_ip == start_ip && rl->stop_ip == end_ip) {
            if (NULL == tmp) {
                tfc_rule_list = rl->next;
            } else {
                tmp->next = rl->next;
            }

            free(rl);
            write_unlock(&traffic_rule_lock);
            return TCAD_RETCODE_OK;
        }

        tmp = rl;
    }
    write_unlock(&traffic_rule_lock);
    
    return TCAD_RETCODE_FLOW_NOT_EXIST;
}

void traffic_dump_udp_send_log(uint8_t protocol_id, time_t time, uint64_t pkts, 
                            uint64_t bytes, uint32_t start_ip, uint32_t end_ip)
{
    int r;
    flows record;

    memset(&record, 0, sizeof(flows));
    
    record.protocol_id = protocol_id;
    record.ftime = time;
    record.pps = pkts;
    record.bps = bytes;
    record.start_ip = start_ip;
    record.end_ip = end_ip;

    r = udp_send_log((char *)&record, sizeof(flows));

    if (0 == r)
        DEBUG_FLOWDEFINED_SUCCEED;
    else
        DEBUG_FLOWDEFINED_FAILED;

    return;
}

void traffic_dump_output(void)
{
    tfc_rule_t *rl;
    tfc_output_t output_tcp, output_udp, output_other, output_all;
    int i;
    uint64_t tmp_pkts = 0, tmp_bytes = 0;
    time_t time = g_utaf_time_jiffies;
    
    //printf("enter %s at %lu\n", __FUNCTION__, g_utaf_time_jiffies);

    memset(&output_tcp, 0, sizeof(tfc_output_t));
    memset(&output_udp, 0, sizeof(tfc_output_t));
    memset(&output_other, 0, sizeof(tfc_output_t));
    memset(&output_all, 0, sizeof(tfc_output_t));

    // 全量日志
    for (i = 0; i < 16; i++)
    {
        output_tcp.pkts    += tfc_all.tcp[i].pkts;
        output_tcp.bytes   += tfc_all.tcp[i].bytes;
        output_udp.pkts    += tfc_all.udp[i].pkts;
        output_udp.bytes   += tfc_all.udp[i].bytes;
        output_other.pkts  += tfc_all.other[i].pkts;
        output_other.bytes += tfc_all.other[i].bytes;
    }

    output_all.pkts = output_tcp.pkts + output_udp.pkts + output_other.pkts;
    output_all.bytes = output_tcp.bytes + output_udp.bytes + output_other.bytes;

    tmp_pkts = output_all.pkts - tfc_all.last_pkts_all;
    tmp_bytes = output_all.bytes - tfc_all.last_bytes_all;
    tfc_all.last_bytes_all = output_all.bytes;
    tfc_all.last_pkts_all = output_all.pkts;

    traffic_dump_udp_send_log(1, time, tmp_pkts, tmp_bytes, 0, 0);

    tmp_pkts = output_tcp.pkts - tfc_all.last_pkts_tcp;
    tmp_bytes = output_tcp.bytes - tfc_all.last_bytes_tcp;
    tfc_all.last_bytes_tcp = output_tcp.bytes;
    tfc_all.last_pkts_tcp = output_tcp.pkts;

    traffic_dump_udp_send_log(2, time, tmp_pkts, tmp_bytes, 0, 0);

    tmp_pkts = output_udp.pkts - tfc_all.last_pkts_udp;
    tmp_bytes = output_udp.bytes - tfc_all.last_bytes_udp;
    tfc_all.last_bytes_udp = output_udp.bytes;
    tfc_all.last_pkts_udp = output_udp.pkts;

    traffic_dump_udp_send_log(3, time, tmp_pkts, tmp_bytes, 0, 0);

    tmp_pkts = output_other.pkts - tfc_all.last_pkts_other;
    tmp_bytes = output_other.bytes - tfc_all.last_bytes_other;
    tfc_all.last_bytes_other = output_other.bytes;
    tfc_all.last_pkts_other = output_other.pkts;

    traffic_dump_udp_send_log(4, time, tmp_pkts, tmp_bytes, 0, 0);

    read_lock(&traffic_rule_lock);
    // 自定义流量日志
    for (rl = tfc_rule_list; rl; rl = rl->next)
    {
        memset(&output_tcp, 0, sizeof(tfc_output_t));
        memset(&output_udp, 0, sizeof(tfc_output_t));
        memset(&output_other, 0, sizeof(tfc_output_t));
        memset(&output_all, 0, sizeof(tfc_output_t));
    
        for (i = 0; i < 16; i++)
        {
            output_tcp.pkts    += rl->tcp[i].pkts;
            output_tcp.bytes   += rl->tcp[i].bytes;
            output_udp.pkts    += rl->udp[i].pkts;
            output_udp.bytes   += rl->udp[i].bytes;
            output_other.pkts  += rl->other[i].pkts;
            output_other.bytes += rl->other[i].bytes;
        }

        output_all.pkts = output_tcp.pkts + output_udp.pkts + output_other.pkts;
        output_all.bytes = output_tcp.bytes + output_udp.bytes + output_other.bytes;

        tmp_pkts = output_all.pkts - rl->last_pkts_all;
        tmp_bytes = output_all.bytes - rl->last_bytes_all;
        rl->last_bytes_all = output_all.bytes;
        rl->last_pkts_all = output_all.pkts;

        traffic_dump_udp_send_log(0xFF, time, tmp_pkts, tmp_bytes, rl->start_ip, rl->stop_ip);
#if 0
        tmp_pkts = output_tcp.pkts - rl->last_pkts_tcp;
        tmp_bytes = output_tcp.bytes - rl->last_bytes_tcp;
        rl->last_bytes_tcp = output_tcp.bytes;
        rl->last_pkts_tcp = output_tcp.pkts;

        traffic_dump_udp_send_log(2, time, tmp_pkts, tmp_bytes, rl->start_ip, rl->stop_ip);

        tmp_pkts = output_udp.pkts - rl->last_pkts_udp;
        tmp_bytes = output_udp.bytes - rl->last_bytes_udp;
        rl->last_bytes_udp = output_udp.bytes;
        rl->last_pkts_udp = output_udp.pkts;

        traffic_dump_udp_send_log(3, time, tmp_pkts, tmp_bytes, rl->start_ip, rl->stop_ip);

        tmp_pkts = output_other.pkts - rl->last_pkts_other;
        tmp_bytes = output_other.bytes - rl->last_bytes_other;
        rl->last_bytes_other = output_other.bytes;
        rl->last_pkts_other = output_other.pkts;

        traffic_dump_udp_send_log(4, time, tmp_pkts, tmp_bytes, rl->start_ip, rl->stop_ip);
#endif
    }
    read_unlock(&traffic_rule_lock);
    
}

void traffic_init(void)
{
    rwlock_init(&traffic_rule_lock);
    memset(&tfc_all, 0, sizeof(tfc_rule_t));
}

