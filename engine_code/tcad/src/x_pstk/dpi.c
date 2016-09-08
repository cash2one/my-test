#include "misc.h"
#include "ip4.h"  //wdb_calc222
#include "mbuf.h"
#include "utaf_session.h"
//#include "ip4.h"  //wdb_calc222
#include "statistics.h"
#include "app.h"
#include "app_id.h"
#include "service_group.h"  //wdb_calc222
#include "httpX.h"
#include "cdpi_api.h"

static inline void cdpi_flow_memset(struct cdpi_flow *buf)
{
    uint32_t i, cnt;
    uint64_t *p = (uint64_t *)buf;

    cnt = sizeof(struct cdpi_flow) / sizeof(uint64_t);

    for (i = 0; i < cnt; ++i) {
        p[i] = 0;
    }
}

void dpi_proc(struct m_buf *mbuf)
{
    uint16_t service_type = APP_ID_UNKNOW;
    uint16_t service_group= 0;
    uint8_t service_mobile = 0;
    
    int thread_id = UTAF_PER_LCORE(lthrd_id);
    
#ifdef UTAF_DEBUG_DPI
    //printf("[DPI]============>dpi_proc\n");
#endif

    session_item_t * si = ((session_item_t *)(mbuf->psession_item));

    if (NULL == si) {
        goto out;
    }

    if (APP_UNABLE_TO_IDENTIFY == si->service_type) {
        si->service_type = 0;
        cdpi_flow_memset(&(si->fflow));
    }

    service_type = si->service_type;
    
    if (APP_ID_UNKNOW == service_type)
    {
#ifdef UTAF_DEBUG_DPI
        printf("[DPI]dpi_parser %lu %lu core %u \n", si->pkts[0], si->pkts[1], utaf_lcore_id());
#endif
        service_type = (uint16_t)cdpi_parse(thread_id, mbuf->network_header, NULL, mbuf->total_len, &(si->fflow));

        if (service_type != 0)
        {
#ifdef UTAF_DEBUG_DPI
            printf("[DPI]mbuf 0x%x:%u -----> 0x%x:%u / session 0x%x:%u -----> 0x%x:%u service_type is %u\n",
                    mbuf->ip_tuple.sip[0], mbuf->sport, mbuf->ip_tuple.dip[0], mbuf->dport,
                    si->ip_tuple.sip[0], si->sport, si->ip_tuple.dip[0], si->dport, service_type);
#endif
        
            service_group = service_group_find(service_type);
            service_mobile = service_mobile_find(service_type);
            si->service_type = service_type;
            si->service_group = service_group;
            si->pc_if = service_mobile;
        
#ifdef UTAF_DEBUG_DPI
            printf("[DPI]service_group is %d\n", service_group);
            //printf("[DPI]mbuf->psession_item->service_group is %d \n",  mbuf->psession_item->service_group);
#endif
        }
    }

    if (APP_ID_UNKNOW != service_type)
    {
        switch (service_type)
        {
#if 0
            case APP_DNS:
            {
                dns_parser(mbuf);
                break;
            }
#endif
            case APP_HTTP:
            {
                if (0xFF == si->app_flag) {
                    *(uint64_t *)(&(si->app_info.httpinfo.service_status)) = 0;
                    *(uint64_t *)(&(si->app_info.httpinfo.browser_id)) = 0;
                    si->app_info.httpinfo.request_cycle = 0;
                    si->app_info.httpinfo.response_cycle = 0;
                    si->app_info.httpinfo.response_delay = 0;
                    si->app_info.httpinfo.data_size = 0;
                    
                    si->app_flag = 0;
                }
                
                http_parser(mbuf);
                break;
            }
            default:
            {
                break;
            }
        }
    }
out:
    MBUF_FREE(mbuf);
    
    return;
}




void dpi_input(struct m_buf *mbuf)
{
#ifdef UTAF_DEBUG_DPI
    printf("[DPI]============>dpi_input\n");
#endif
#if 1
    if(mbuf->ipdefrag_flag == IS_IP_PKT || mbuf->ipdefrag_flag == IS_FIRST_IP_DEFRAG)
    {    
        dpi_proc(mbuf);    
    }
    else
    {
        MBUF_FREE(mbuf);
    }
#else
    MBUF_FREE(mbuf);
#endif
    return;
}




