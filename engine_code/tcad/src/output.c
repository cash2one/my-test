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
#include <time.h>

#include "misc.h"
#include "mbuf.h"
#include "list.h"
#include "spinlock.h"
#include "rwlock.h"
#include "httpX.h"
#include "utaf_session.h"
#include "statistics.h"
#include "tcad.h"
#include "app.h"

extern int udp_server_socket;
extern struct sockaddr_in udp_enevt_log_to;
extern struct sockaddr_in udp_define_flow_to;
extern char udp_server_ip[64];
extern uint32_t incr_interval;
extern volatile time_t g_utaf_time_jiffies;
extern int udp_send_log(char *log_info, int length);

inline int udp_send_event_log(char *log_info, int length)
{
    //send the message
    if (sendto(udp_server_socket, log_info, length, 0, (struct sockaddr *)&udp_enevt_log_to, sizeof(udp_enevt_log_to))!=length) {
        return -1;
    }

    return 0;
}

inline uint8_t output_protocol(uint8_t proto)
{
    switch (proto)
    {
        case 6:
            return 2;
        case 17:
            return 3;
        default:
            return 4;
    }

    return 4;
}

void output_age_record(session_item_t *ssn)
{
    int r;
    uint8_t tcp_success = 0;
    flow_event record;
    time_t end_time = 0;

    if ((ssn->tcp_flag & 0x2) == 0x2 || (ssn->tcp_flag & 0x4) == 0x4) {
        tcp_success = 1;
    }

    end_time = g_utaf_time_jiffies - UTAF_AGE_RING_SIZE;

    record.ftime_start   = ssn->start_time;
    record.ftime_end     = end_time;
    record.visit_time    = ((record.ftime_end > record.ftime_start) ? (record.ftime_end - record.ftime_start) : 1);
    record.sip           = htonl(ssn->ip_tuple.sip[0]);
    record.dip           = htonl(ssn->ip_tuple.dip[0]);
    record.usersport     = ssn->sport;
    record.userdport     = ssn->dport;
    record.procotol_id   = output_protocol(ssn->protocol);
    record.service_type_id   = ssn->service_type;
    record.service_group_id  = ssn->service_group;
    record.direction_flag    = ssn->direct_flags;
    record.up_bytes	     = ssn->bytes[0];
    record.down_bytes    = ssn->bytes[1];
    record.country       = ssn->country;
    record.pc_if         = ssn->pc_if;
    record.tcp_suc       = tcp_success;
    record.bank_visit    = (APP_HTTPS == ssn->service_type ? 1 : ssn->bank_visit);
    record.flag          = (record.visit_time > incr_interval ? 1 : 0);
    record.incr          = 0;
    record.s_isp_id      = ssn->s_isp_id;
    record.d_isp_id      = ssn->d_isp_id;
    record.s_province_id = ssn->s_province_id;
    record.d_province_id = ssn->d_province_id;
    record.s_city_id     = ssn->s_city_id;
    record.d_city_id     = ssn->d_city_id;
    record.ip_byte       = ssn->bytes[0] + ssn->bytes[1];
    record.ip_pkt        = ssn->pkts[0] + ssn->pkts[1];
    record.cli_ser       = ssn->app_info.httpinfo.response_delay;
    //strncpy((char *)(record.browser), ssn->app_info.httpinfo.browser, SAM_STR_SIZE);
    record.browser_id = ssn->app_info.httpinfo.browser_id;
    strncpy((char *)(record.version), ssn->app_info.httpinfo.version, SAM_STR_SIZE);

    r = udp_send_event_log((char *)&record, sizeof(flow_event));

    if (0 == r)
        DEBUG_SESSION_STAT_SESSION_EVENT_AGE_S(1);
    else
        DEBUG_SESSION_STAT_SESSION_EVENT_AGE_F(1);
}

void output_inc_record(session_item_t *ssn)
{
    int r;
    uint8_t tcp_success = 0;
    flow_event record;
    time_t end_time = 0;

    if ((ssn->tcp_flag & 0x2) == 0x2 || (ssn->tcp_flag & 0x4) == 0x4) {
        tcp_success = 1;
    }

    end_time = g_utaf_time_jiffies - UTAF_AGE_RING_SIZE;

    record.ftime_start   = ssn->start_time;
    record.ftime_end     = end_time;
    record.visit_time    = ((record.ftime_end > record.ftime_start) ? (record.ftime_end - record.ftime_start) : 1);
    record.sip           = htonl(ssn->ip_tuple.sip[0]);
    record.dip           = htonl(ssn->ip_tuple.dip[0]);
    record.usersport     = ssn->sport;
    record.userdport     = ssn->dport;
    record.procotol_id   = (uint16_t)ssn->service_type;
    record.country       = ssn->country;
    record.pc_if         = ssn->pc_if;
    record.tcp_suc       = tcp_success;
    record.bank_visit    = (APP_HTTPS == ssn->service_type ? 1 : ssn->bank_visit);
    record.flag          = (record.visit_time > incr_interval ? 1 : 0);
    record.incr          = 1;
    record.s_isp_id      = ssn->s_isp_id;
    record.d_isp_id      = ssn->d_isp_id;
    record.s_province_id = ssn->s_province_id;
    record.d_province_id = ssn->d_province_id;
    record.s_city_id     = ssn->s_city_id;
    record.d_city_id     = ssn->d_city_id;
    record.ip_byte       = ssn->bytes[0] + ssn->bytes[1];
    record.ip_pkt        = ssn->pkts[0] + ssn->pkts[1];
    record.cli_ser       = ssn->app_info.httpinfo.response_delay;
    //strncpy((char *)(record.browser), ssn->app_info.httpinfo.browser, SAM_STR_SIZE);
    record.browser_id = ssn->app_info.httpinfo.browser_id;
    strncpy((char *)(record.version), ssn->app_info.httpinfo.version, SAM_STR_SIZE);

    r = udp_send_event_log((char *)&record, sizeof(flow_event));
    if (0 == r)
        DEBUG_SESSION_STAT_SESSION_EVENT_INC_S(1);
    else
        DEBUG_SESSION_STAT_SESSION_EVENT_INC_F(1);
}

int udp_send_log(char *log_info, int length)
{
    //send the message
    if (sendto(udp_server_socket, log_info, length, 0, (struct sockaddr *)&udp_define_flow_to, sizeof(udp_define_flow_to))!=length) {
        return -1;
    }

    return 0;
}

