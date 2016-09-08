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
#include "misc.h"
#include "dns.h"
#include "udp4.h"
#include "utaf_timer.h"
#include "utaf_session.h"
#include "app_id.h"
#include "app.h"

extern session_table_info_t session_table;
//extern logp * utaf_logp_alloc(int);

uint32_t dns_request_handle(struct m_buf *mbuf, char *request_name, uint64_t cycle, uint16_t rq_id)
{
	session_item_t *si = NULL;

	si = (session_item_t *)(mbuf->psession_item);
	if(NULL == si)
	{
		return UTAF_FAIL;
	}

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
    spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//    write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6
	
	si->app_id = APP_DNS;
	if(si->app_info.dnsinfo.dns_status == DNS_NONE || si->app_info.dnsinfo.dns_status == DNS_REQUEST)
	{
#if 0
		si->app_info.dnsinfo.request_count++;
		memcpy(si->app_info.dnsinfo.request_domain, request_name, MAX_REQUEST_NAME_SIZE);
		si->app_info.dnsinfo.request_cycle = cycle;
		si->app_info.dnsinfo.dns_status = DNS_REQUEST;
		si->app_info.dnsinfo.request_id = rq_id;
		//si->app_info.dnsinfo.start_time = time(NULL);
#endif
	}
	
//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
    spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//    write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6
	
	return UTAF_OK;
}


uint32_t dns_response_handle(struct m_buf *mbuf, uint8_t status, uint64_t cycle, uint16_t rq_id)
{
	session_item_t *si;

	si = mbuf->psession_item;
	if(NULL == si)
	{
		return UTAF_FAIL;
	}

	#ifdef UTAF_DEBUG_DNS
		printf("[DNS Parser]dns status is %d\n", si->app_info.dnsinfo.dns_status);
	#endif

	if (si->app_info.dnsinfo.dns_status == DNS_NONE)
	{
		return UTAF_FAIL;
	}
	else
	{
		if (si->app_info.dnsinfo.request_id == rq_id)
		{
		/*	uint64_t start_cycle = 0; //wdb_calc222 */
		/*	uint64_t end_cycle = cycle; //wdb_calc222 */
		/*	ewx_report_dns_info_t *pdnsinfo = NULL; //wdb_calc222 */
            
			#ifdef UTAF_DEBUG_DNS
				printf("[DNS Parser]request id match\n");
			#endif
#if 0
            logp *plogp = utaf_logp_alloc(utaf_lthrd_id());

			if (NULL == plogp)
			{
				return UTAF_FAIL;
			}

			pdnsinfo = (ewx_report_dns_info_t *)(&plogp->buf);

            spinlock_lock(&si->item_lock);
            start_cycle = si->app_info.dnsinfo.request_cycle;
            pdnsinfo->request_count = si->app_info.dnsinfo.request_count;
			pdnsinfo->service_status = status;
			
			memcpy((void *)pdnsinfo->service_host, (void *)si->app_info.dnsinfo.request_domain, MAX_REQUEST_NAME_SIZE);
			memset((void *)(&si->app_info.dnsinfo), 0, sizeof(struct dns_info));
			spinlock_unlock(&si->item_lock);
			
			utaf_cdr_dns_output(plogp, si, start_cycle, end_cycle);
#endif
			return UTAF_OK;
		}
		else
		{
			return UTAF_FAIL;
		}
	}

	return UTAF_OK;
}



void dns_parser(struct m_buf *mbuf)
{
	DNS_HEADER *dh = NULL;
	
	uint16_t qdcount;
	uint64_t request_cycle;
	uint64_t response_cycle;
	uint8_t service_status;

	uint8_t *cp = NULL;

	char request_name[MAX_REQUEST_NAME_SIZE] = { 0 };
	uint16_t request_id = 0;

#ifdef UTAF_DEBUG_DNS
	printf("[DNS Parser]========>dns parser\n");
#endif
	
	dh = (DNS_HEADER *)mbuf->data;

	if(!DNS_QR(dh)) /*request*/
	{
	#ifdef UTAF_DEBUG_DNS
		printf("[DNS Parser]===============>request\n");
	#endif

		qdcount = ntohs(dh->qdcount);
		if(qdcount == 0)
		{
			return;
		}
		
	#ifdef UTAF_DEBUG_DNS
		printf("[DNS Parser]qdcount is %d\n", qdcount);
	#endif
	
		request_cycle = utaf_get_timer_cycles();
	
		/*目前只处理问题数为1的情况，该情况为通常情况*/
		cp = (uint8_t *)(dh + 1);
		int i = 0, j = 0, k = 0, start = 0;

		int len = cp[i];

		while(len != 0 && j < MAX_REQUEST_NAME_SIZE - 1)
		{
			if(start == 0)
			{
				start = 1;
			}
			else
			{
				request_name[j] = '.';
				j++;
			}
			i++;
			
			for(k = 0; k < len && j <= (MAX_REQUEST_NAME_SIZE - 1); k++)  //wdb_calc222
			{
				request_name[j++] = cp[i + k];
			}

			//j = j + len;
			i = i + len;
			len = cp[i];
		}
		request_name[j] = '\0';

		request_id = ntohs(dh->id);
		
	#ifdef UTAF_DEBUG_DNS
		printf("[DNS Parser]domain is %s\n", request_name);
		printf("[DNS Parser]id is %d\n", request_id);
	#endif
		(void)dns_request_handle(mbuf, request_name, request_cycle, request_id);

	}
	else       /*response*/
	{
	#ifdef UTAF_DEBUG_DNS
		printf("[DNS Parser]===============>response\n");
	#endif
		response_cycle = utaf_get_timer_cycles();
		service_status = DNS_RCODE(dh);
		request_id = ntohs(dh->id);
		

	#ifdef UTAF_DEBUG_DNS
		printf("[DNS Parser]service_status is %d, id is %d\n", service_status, request_id);
	#endif
		
		(void)dns_response_handle(mbuf, service_status, response_cycle, request_id);
	}
	
	return;
}





uint32_t dns_init(void)  //wdb_calc222
{

	return UTAF_OK;
}




