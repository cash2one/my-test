#if 1 /* wdb_add */
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "atomic.h"
#include "list.h"
#include "misc.h"
#include "spinlock.h"
#include "mbuf.h"
#include "ip4.h"
#include "tcp4.h"
/* #include "statistics.h" */
#include "rwlock.h"
#endif /* wdb_add */


#include "tcad.h"
#include "httpX.h"
#include "app_id.h"
#include "utaf_session.h"
#include "utaf_timer.h"

/* #include "bm.h"
#include "applib.h"
#include "tcp4.h"
#include "logp.h"
#include "logw_lib.h" */
#include "app.h"

extern volatile time_t g_utaf_time_jiffies;
extern session_table_info_t session_table;

extern uint64_t UTAF_SYS_HZ;
#if 0
extern logp * utaf_logp_alloc(int);
extern 	uint32_t utaf_cdr_http_output(logp * plogp, session_item_t * si, time_t end_time);
extern uint32_t utaf_flows_lock(uint8_t protocol, uint16_t s_port, uint16_t d_port, uint32_t s_ip, uint32_t d_ip);  //wdb_calc222
#endif
extern int browser_get(char *useragent, int *browser_id, char *version, int vmaxlen);
char * cstrstr(char *s, unsigned int slen, char *d, unsigned int dlen);


int http_code_table[600] = { 0 };

int http_is_statcode_legal(char *pcode, int *statcode)
{
	int i = 0;
	int code = 0;
	char status_code[HTTP_STATUS_CODE_SIZE_MAX] = { 0 };

	while (pcode[i] != ' ' && i < HTTP_STATUS_CODE_SIZE_MAX - 1) {
		status_code[i] = pcode[i];
		i++;
	}
	
	code = atoi(status_code);

	if (code < 0 || code > 600) {/*状态码*/
		return HTTP_UNLEGAL;
	}

    *statcode = code;
    return HTTP_LEGAL;
}

void http_get_browser(char *t, session_item_t *si)
{
    char *v, *tmp = NULL;//, *b = NULL;
    char useragent[BROWSER_STR_SIZE+1] = {0};
    
    v = strstr(t, "\r\n");

    if (NULL != v && v > t) {
#if 1
        tmp = t + HTTP_KEYWORD_USERAGENT_SIZE;
        strncpy(useragent, tmp, (v-tmp) > BROWSER_STR_SIZE ? BROWSER_STR_SIZE : (v-tmp));

        browser_get(useragent, &(si->app_info.httpinfo.browser_id), si->app_info.httpinfo.version, SAM_STR_SIZE);
#ifdef UTAF_DEBUG_HTTP
        printf("[HTTP]User-Agent: %s/%s\n", si->app_info.httpinfo.browser, si->app_info.httpinfo.version);
#endif

#else
        tmp = v;
        while (('/' != *tmp) && (tmp > t)) {
            tmp--;
        }

        b = tmp;
        while ((' ' != *b) && (b > t)) {
            b--;
        }

        b++;
        v--;
        if (')' == *v) {
            v--;
        }
        
        if (b > t) {
            strncpy(si->app_info.httpinfo.browser, b, (tmp-b) > SAM_STR_SIZE ? SAM_STR_SIZE : (tmp-b));
            strncpy(si->app_info.httpinfo.version, tmp+1, (v-tmp) > SAM_STR_SIZE ? SAM_STR_SIZE : (v-tmp));

#ifdef UTAF_DEBUG_HTTP
            printf("[HTTP]Browser: %s/%s\n", si->app_info.httpinfo.browser,
                        si->app_info.httpinfo.version);
#endif
        }
#endif
    }
}

int http_is_get(struct m_buf *mbuf, session_item_t *si)
{
	//int ret;
	char *t;

	//ret = BM((char *)mbuf->data, 16, HTTP_KEYWORD_GET, HTTP_KEYWORD_GET_SIZE);
	if (0 != strncmp((char *)mbuf->data, HTTP_KEYWORD_GET, HTTP_KEYWORD_GET_SIZE)) {
		return -1;
	}

    t = cstrstr((char *)mbuf->data, mbuf->data_len, HTTP_KEYWORD_USERAGENT, HTTP_KEYWORD_USERAGENT_SIZE);

    if (NULL != t) {
        http_get_browser(t, si);
    }
    
	return 0;
}

int http_is_post(struct m_buf *mbuf, session_item_t *si)
{
	//int ret;
	char *t;

	//ret = BM((char *)mbuf->data, 16, HTTP_KEYWORD_POST, HTTP_KEYWORD_POST_SIZE);
	if (0 != strncmp((char *)mbuf->data, HTTP_KEYWORD_POST, HTTP_KEYWORD_POST_SIZE)) {
		return -1;
	}

    t = cstrstr((char *)mbuf->data, mbuf->data_len, HTTP_KEYWORD_USERAGENT, HTTP_KEYWORD_USERAGENT_SIZE);

    if (NULL != t) {
        http_get_browser(t, si);
    }
    
	return 0;
}

int http_is_response(struct m_buf *mbuf, session_item_t *si, int *statcode)
{
	//int ret;
	char *pcode = NULL;
	
	//ret = BM((char *)mbuf->data, 16, HTTP_KEYWORD_VERSION_1, HTTP_KEYWORD_VERSION_1_SIZE);
	//if(-1 != ret && 0 == ret) /* HTTP/1.0 */
	if (0 == strncmp((char *)mbuf->data, HTTP_KEYWORD_VERSION_1, HTTP_KEYWORD_VERSION_1_SIZE)) {
        //strncpy(si->app_info.httpinfo.version, HTTP_KEYWORD_VERSION_1, SAM_STR_SIZE);
		pcode = (char *)mbuf->data + HTTP_KEYWORD_VERSION_1_SIZE;
		if (HTTP_LEGAL == http_is_statcode_legal(pcode, statcode)) {
			return HTTP_RESPONSE;
		} else {
			return HTTP_NONE;
		}
	}
    else if (0 == strncmp((char *)mbuf->data, HTTP_KEYWORD_VERSION_2, HTTP_KEYWORD_VERSION_2_SIZE))
	//ret = BM((char *)mbuf->data, 16, HTTP_KEYWORD_VERSION_2, HTTP_KEYWORD_VERSION_2_SIZE);
	//if(-1 != ret && 0 == ret)
	{
        //strncpy(si->app_info.httpinfo.version, HTTP_KEYWORD_VERSION_2, SAM_STR_SIZE);
		pcode = (char *)mbuf->data + HTTP_KEYWORD_VERSION_2_SIZE;
		if (HTTP_LEGAL == http_is_statcode_legal(pcode, statcode)) {
			return HTTP_RESPONSE;
		} else {
			return HTTP_NONE;
		}
	}

	return HTTP_NONE;
}

uint32_t http_state(struct m_buf *mbuf, session_item_t *si, uint8_t *cmd, int *url_off, int *statcode)
{
	int ret; //wdb_calc222
    
#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]====>http_state\ndata is: ");
    for (i = 0; i < 7; i++) {
        printf("%02x ", mbuf->data[i]);
    }
    printf("\n");
#endif

	ret = http_is_get(mbuf, si);
	if (ret == 0) {
		*url_off = HTTP_KEYWORD_GET_SIZE;
        *cmd = HTTP_SERVICE_CMD_GET;
		return HTTP_REQUEST;
	}

	ret = http_is_post(mbuf, si);
	if (ret == 0) {
		*url_off = HTTP_KEYWORD_POST_SIZE;
        *cmd = HTTP_SERVICE_CMD_POST;
		return HTTP_REQUEST;
	}

	ret = http_is_response(mbuf, si, statcode);
	if (ret == HTTP_RESPONSE) {
		return HTTP_RESPONSE;
	}

	return HTTP_NONE;
}
char* form_to_hex(char *str,int islen)
{
	char buf[65535]={0};
	int i;
	int len=0;
	char *p=NULL;
	memset(buf,0,65535);
	for(i=0;i<islen;i++)
	{
		if(str[i]!=' ')
		{
			strncat(buf,&str[i],1);
			len++;
		}
		else
		{
			strncat(buf,"\\20",4);
			len=len+3;
		}
	}
			strncat(buf,"\0",2);
	
	p=(char*)malloc(len+1);
	memset(p,0,len+1);
	strncpy(p,buf,len);
	return p;

}

char *http_get_user_agent(struct m_buf *mbuf)
{
	char *t=NULL;
	char *pret=NULL;
    char *v, *tmp = NULL;//, *b = NULL;

    t = cstrstr((char *)mbuf->data, mbuf->data_len, HTTP_KEYWORD_USERAGENT, HTTP_KEYWORD_USERAGENT_SIZE);
	if(NULL == t)
	{
lable:
		pret=(char *)malloc(sizeof(char)*(strlen("NULL")+1));
		strncpy(pret,"NULL",strlen("NULL")+1);
		return pret;
	}
    v = strstr(t, "\r\n");
	tmp=t;
	if(v-tmp+1 > mbuf->data_len-(t- (char*)mbuf->data))
		v = (char*)mbuf->data + mbuf->data_len;

	pret=(char *)malloc(sizeof(char)*(v-tmp+1));
	if(pret == NULL)
		goto lable;

	memset(pret,0,v-tmp+1);
	strncpy(pret,tmp,v-tmp);

	tmp=form_to_hex(pret,v-tmp);
	if(pret)
	{
		free(pret);
	}

	return tmp;
}
char *http_get_referer(struct m_buf *mbuf)
{
	char *t=NULL;
	char *pret=NULL;
    char *v, *tmp = NULL;//, *b = NULL;

    t = cstrstr((char *)mbuf->data, mbuf->data_len, HTTP_KEYWORD_REFERER, HTTP_KEYWORD_REFERER_SIZE);
	if(NULL == t)
	{
refer_label:
		pret=(char *)malloc(sizeof(char)*(strlen("NULL")+1));
		strncpy(pret,"NULL",strlen("NULL")+1);
		return pret;
	}
    v = strstr(t, "\r\n");
	tmp=t ;
	if(v-tmp+1 > mbuf->data_len-(t- (char*)mbuf->data))
		v = (char*)mbuf->data + mbuf->data_len;

	pret=(char *)malloc(sizeof(char)*(v-tmp+1));
	if(pret == NULL)
		goto refer_label;
	pret=(char *)malloc(sizeof(char)*(v-tmp+1));
	memset(pret,0,v-tmp+1);
	strncpy(pret,tmp,v-tmp);
	tmp=form_to_hex(pret,v-tmp);
	if(pret)
	{
		free(pret);
	}

	return tmp;

}
char *http_get_param(char*url_str,struct m_buf *mbuf,int method)
{
	char *t=NULL;
	char *pret=NULL;
    //char *v, *tmp = NULL;//, *b = NULL;
	if (method == 0)
	{
		pret=(char *)malloc(sizeof(char)*(strlen("NULL")+1));
		memset(pret,0,strlen("NULL")+1);
		strncpy(pret,"NULL",strlen("NULL"));
		return pret;
	}

    t = strstr((char *)url_str, "?");
	if(NULL == t)
	{
param_label:
		pret=(char *)malloc(sizeof(char)*(strlen("NULL")+1));
		memset(pret,0,strlen("NULL")+1);
		strncpy(pret,"NULL",strlen("NULL"));
		return pret;
	}
		pret=(char *)malloc(sizeof(char)*(strlen(t)+1));
		if (NULL==pret)
			goto param_label;
		memset(pret,0,strlen(t)+1);
		strncpy(pret,t,strlen(t));

	return pret;
}

uint32_t http_gethost(struct m_buf *mbuf, int url_off, char *url, char *host)
{	
	int i = 0;
	char *url_start = (char *)(mbuf->data) + url_off;
	char *host_start = NULL;
    
#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]====>http_gethost\n");
#endif

	while (url_start[i] != ' ' && i < HTTP_URL_LENGTH_MAX) {
		url[i] = url_start[i];
		i++;
	}
    url[i] = '\0';
	
	//ret = BM((char *)mbuf->data, mbuf->data_len, HTTP_KEYWORD_HOST, HTTP_KEYWORD_HOST_SIZE);
	//if(ret < 0)
	host_start = strstr((char *)mbuf->data, HTTP_KEYWORD_HOST);
	if (NULL == host_start) {
		return UTAF_OK;
	}

	host_start += HTTP_KEYWORD_HOST_SIZE;

	i = 0;
	while (host_start[i] != '\r' && i < HTTP_HOST_LENGTH_MAX) {
		host[i] = host_start[i];
		i++;
	}
    host[i] = '\0';

#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]http host: %s %s\n", host, url);
#endif
	return UTAF_OK;
}


/*
	1. 如果已出现过request，此时该流的上一个request过程结束；进行输出；
	2. 如果未出现过request，此为该流的第一个request；进行记录
*/
uint32_t http_request_handle(struct m_buf *mbuf, session_item_t *si, char *url,
                                char *host, uint64_t cycle, uint8_t cmd)
{
    struct http_info *info = NULL;

#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]====>http_request_handle\n");
#endif

    info = &(si->app_info.httpinfo);

#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]app_id: %u, http_status: %u\n", si->app_id, info->http_status);
#endif

	if (APP_ID_UNKNOW == si->app_id)
	{
//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//        write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6
		si->app_id = APP_HTTP;
		info->http_status = HTTP_REQUEST;
#if 0   // 暂时不用，先不作任何解析
		memcpy(info->url, url, HTTP_URL_LENGTH_MAX);
		memcpy(info->host, host, HTTP_HOST_LENGTH_MAX);
#endif
		info->request_cycle = cycle;
        // 记录会话开始时间，即GET/POST发出时间
        //info->start_time = g_utaf_time_jiffies; //info->start_time = time(NULL);
        info->service_cmd = cmd;
        
//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//        write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

#ifdef UTAF_DEBUG_HTTP2
	printf("[HTTP]request_cycle: %ld, start_time: %ld\n", info->request_cycle, info->start_time);
#endif
        return UTAF_OK;
	}
	else if (APP_HTTP == si->app_id && HTTP_RESPONSE == info->http_status) 
	{
#if 0 /* wdb_ppp */
        logp *plogp = utaf_logp_alloc(utaf_lthrd_id());
		if (NULL == plogp)
		{
			return UTAF_FAIL;
		}
#endif /* wdb_ppp */
		uint32_t t = 0;

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//        write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

        if (info->response_cycle >= info->request_cycle) {
            t = (info->response_cycle - info->request_cycle)/TIMER_CYCLE_PER_MS;
            info->response_delay = (0 == t ? 1 : t);
        }
#if 0 /* wdb_ppp */      
        info->data_size = (uint32_t)(si->bytes[1] - info->request_byte);
        t = (cycle - info->response_cycle)/TIMER_CYCLE_PER_MS;
        info->download_delay = (0 == t ? 1 : t);
        info->download_rate = (info->data_size*1000)/(info->download_delay);
		
		ewx_report_session_info_t *psessinfo = (ewx_report_session_info_t *)(&plogp->buf);

		psessinfo->service_cmd = info->service_cmd;
		psessinfo->service_status = info->service_status;

        #if 0   // 暂时不用，先不作任何解析
		if (0 == strncmp(info->url, "http://", 7)) 
		{
        	memcpy(psessinfo->service_info, info->url, (SERVICE_INFO_LEN -1));
		}
		else 
		{
	    	int i = strlen(info->host);
        	int j = SERVICE_INFO_LEN - 1 - 7 - i;
        	memcpy(psessinfo->service_info, "http://", 7);
        	memcpy(psessinfo->service_info + 7, info->host, i);
        	memcpy(psessinfo->service_info + 7 + i, info->url, j);
    	}
		
		memcpy(psessinfo->service_host, info->host, (SERVICE_HOST_LEN -1));
        #else
        psessinfo->service_host[0] = 0;
        psessinfo->service_info[0] = 0;
		#endif
        
		psessinfo->start_time = info->start_time;
		psessinfo->end_time = info->end_time;
        psessinfo->total_delay = 0;

		if (info->last_ack_cycle > info->request_cycle) 
		{
        	uint32_t t = (info->last_ack_cycle - info->request_cycle)/TIMER_CYCLE_PER_MS;
	    	psessinfo->transcation_delay = (0 == t ? 1 : t);
    	} 
		else 
    	{
        	psessinfo->transcation_delay = 0;
    	}
		
		psessinfo->response_delay = info->response_delay;
    	psessinfo->data_size = info->data_size;
		psessinfo->download_rate = info->download_rate;
    	psessinfo->download_delay = info->download_delay;
		//psessinfo->ip_info.ip_byte = si->bytes[1];
		//si->bytes[1] = 0;
		//psessinfo->ip_info.ip_pkt = si->pkts[1];
		//si->pkts[1] = 0;
        psessinfo->ip_info.ip_byte = 0;
        psessinfo->ip_info.ip_pkt = 0;
        psessinfo->ip_info.ip_fragment_num = 0;

		memset(info, 0, sizeof(struct http_info));
#endif /* wdb_ppp */

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//        write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

		//info->end_time = time(NULL);
		time_t end_time = g_utaf_time_jiffies; //time_t end_time = time(NULL);
#if 0 /* wdb_ppp */
		/*HTTP LOG OUTPUT*/
		utaf_cdr_http_output(plogp, si, end_time);
#endif /* wdb_ppp */
		//info->end_time = end_time;
		
#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]end_time: %d\n", (int)info->end_time);
#endif

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//        write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

        //si->app_id = APP_HTTP;
		info->http_status = HTTP_REQUEST;
		info->request_cycle = cycle;
        #if 0   // 暂时不用，先不作任何解析
		memcpy(info->url, url, HTTP_URL_LENGTH_MAX);
		memcpy(info->host, host, HTTP_HOST_LENGTH_MAX);
        #endif
        // 记录会话开始时间，即GET/POST发出时间
        //info->start_time = info->end_time;
        info->service_cmd = cmd;
//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//        write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6
        
        return UTAF_OK;
	} 

    return UTAF_FAIL;
}

uint32_t http_response_handle(struct m_buf *mbuf, session_item_t *si, int statcode)
{
	uint64_t cycle;

#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]====>http_response_handle\n");
	printf("[HTTP]app_id: %u, http_status: %u\n", si->app_id, si->app_info.httpinfo.http_status);
#endif

	if (APP_HTTP == si->app_id && HTTP_REQUEST == si->app_info.httpinfo.http_status) {
		cycle = utaf_get_timer_cycles();

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//        write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

        si->app_info.httpinfo.http_status = HTTP_RESPONSE;
		si->app_info.httpinfo.service_status = statcode;
		si->app_info.httpinfo.response_cycle = cycle;
        //si->app_info.httpinfo.request_byte = (si->bytes[1] - mbuf->total_len);

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
        spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//        write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

#ifdef UTAF_DEBUG_HTTP2
	printf("[HTTP]response_cycle: %ld\n", si->app_info.httpinfo.response_cycle);
	printf("[HTTP]HTTP STATUS: %d,  service_status: %d\n", si->app_info.httpinfo.http_status, si->app_info.httpinfo.service_status);
#endif

	}
	else 
	{
		return UTAF_FAIL;
	}
    
	return UTAF_OK;
}
char* get_systime_str(time_t now_time,char *now_time_str)
{    
	char time_format[] = "%Y-%m-%d %X";

	//struct tm* now_time_p = NULL;
	struct tm  tmp_time = {0};

	localtime_r(&now_time,&tmp_time);
	strftime(now_time_str, TIME_FORMAT_SIZE-1, time_format, &tmp_time);

	return now_time_str;
}

void http_parser(struct m_buf *mbuf)
{
    uint8_t cmd = 0;
	uint32_t ret;
	uint64_t cycle;
	int statcode;
	int url_offset;
	session_item_t *si = NULL;
	
	char url[HTTP_URL_LENGTH_MAX] = { 0 };
	char host[HTTP_HOST_LENGTH_MAX] = { 0 };

#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]====>http_parser\n");
#endif

	si = (session_item_t *)(mbuf->psession_item);
	if (NULL == si) {
		return ;
	}

#if 0 /* wdb_ppp */
    // 双向流时下发流锁定
    if (0 == si->lock && 0 != si->pkts[1]) {
		utaf_flows_lock(si->protocol, si->sport, si->dport, si->ip_tuple.sip[0], si->ip_tuple.dip[0]);
		si->lock = 1;
	}
#endif /* wdb_ppp */
	ret = http_state(mbuf, si, &cmd, &url_offset, &statcode);

#ifdef UTAF_DEBUG_HTTP
	printf("[HTTP]http state is %d\n", ret);
#endif
	cycle = utaf_get_timer_cycles();
	
	if (HTTP_REQUEST == ret) 
	{
        #if 0   // 暂时不用，先不作任何解析
		if (UTAF_OK != http_gethost(mbuf, url_offset, url, host))
		{
			return;
		}
        #endif

		if (UTAF_OK != http_request_handle(mbuf, si, url, host, cycle, cmd)) 
		{
			return;
		}
#ifdef UTAF_URL
#define FLOW_ONE_HANG_SIZE 65535
#define TIME_FORMAT_SIZE 25
	char method[20]={0};
	char sip[16]={0};
	char dip[16]={0};
	char flow_http[FLOW_ONE_HANG_SIZE]={0};
	char *pAgent=NULL;
	char *param=NULL;
	char *pReper=NULL;
	time_t now_time;
	char sys_str_time[TIME_FORMAT_SIZE]={0};
	time(&now_time);
	struct in_addr saddr;
	struct in_addr daddr;
	extern int write_url_file(char *buf);
	//sip縟ip縮port縟port縰rl縨ethod縣ost縰ser_agent縫aram縃TTP靠靠靠靠靠referer縃TTP靠靠referer靠靠timenow縮ip_str縟ip_str縯ime_str
		if (UTAF_OK != http_gethost(mbuf, url_offset, url, host))
		{
			return;
		}
		if(url_offset == HTTP_KEYWORD_GET_SIZE)
		{
			strncpy(method,"GET",4);
			param  = http_get_param(url,mbuf,0);
		}
		else
		{
			strncpy(method,"POST",4);
			param  = http_get_param(url,mbuf,1);
		}
		pAgent = http_get_user_agent(mbuf);
		pReper = http_get_referer(mbuf);
		saddr.s_addr = htonl(mbuf->ip_tuple.sip[0]);
		strncpy(sip,inet_ntoa(saddr),16);
		daddr.s_addr = htonl(mbuf->ip_tuple.dip[0]);
		strncpy(dip,inet_ntoa(daddr),16);
snprintf(flow_http,FLOW_ONE_HANG_SIZE,"%u\t%u\t%u\t%u\t%s\t%s\tHost:\\20%s\t%s\t%s\t%s\t%lu\t%s\t%s\t%s\n",
							saddr.s_addr,			\
							daddr.s_addr,			\
							mbuf->sport,				\
							mbuf->dport,				\
							url,					\
							method,					\
							host,					\
							pAgent,					\
							param,					\
							pReper,					\
							now_time,				\
							sip,					\
							dip,					\
							get_systime_str(now_time,sys_str_time)
							);

		if(pAgent)
		{
			free(pAgent);
		}
		if(param)
		{
			free(param);
		}
		if(pReper)
		{
			free(pReper);
		}
		//write_to_file
		write_url_file(flow_http);
	
#endif
	}
	else if (HTTP_RESPONSE == ret) 
	{
        if (200 == statcode) {
            si->bank_visit = 1;
        }
        
		if (UTAF_OK != http_response_handle(mbuf, si, statcode)) 
		{
			return;
		}
	} 
	else 
	{
        struct tcp_hdr *tcp = tcp4_hdr(mbuf);

        // RST or FIN
        if ( ((tcp->tcp_flags & 0x04)||( tcp->tcp_flags & 0x01)) && (HTTP_RESPONSE == si->app_info.httpinfo.http_status)) 
        {
	 	
            uint32_t t = 0;
            struct http_info *info = NULL;
#if 0 /* wdb_ppp */
            logp *plogp = utaf_logp_alloc(utaf_lthrd_id());
			if (NULL == plogp)
			{
				return;
			}

			ewx_report_session_info_t *psessinfo = (ewx_report_session_info_t *)(&plogp->buf);
#endif /* wdb_ppp */
            info = &(si->app_info.httpinfo);

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
            spinlock_lock(&si->item_lock);
//#else //wdb_lfix-6
//            write_lock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

            t = (info->response_cycle - info->request_cycle)/TIMER_CYCLE_PER_MS;
            info->response_delay = (0 == t ? 1 : t);
#if 0
            info->data_size = (uint32_t)(si->bytes[1] - info->request_byte);
            t = (cycle - info->response_cycle)/TIMER_CYCLE_PER_MS;
            info->download_delay = (0 == t ? 1 : t);
            info->download_rate = (info->data_size*1000)/(info->download_delay);
#endif
#if 0 /* wdb_ppp */
			psessinfo->service_cmd = info->service_cmd;
			psessinfo->service_status = info->service_status;

            #if 0   // 暂时不用，先不作任何解析
			if (0 == strncmp(info->url, "http://", 7)) 
			{
        		memcpy(psessinfo->service_info, info->url, (SERVICE_INFO_LEN -1));
			}
			else 
			{
	   	 		int i = strlen(info->host);
        		int j = SERVICE_INFO_LEN - 1 - 7 - i;
        		memcpy(psessinfo->service_info, "http://", 7);
        		memcpy(psessinfo->service_info + 7, info->host, i);
        		memcpy(psessinfo->service_info + 7 + i, info->url, j);
    		}
		
			memcpy(psessinfo->service_host, info->host, (SERVICE_HOST_LEN -1));
            #else
            psessinfo->service_host[0] = 0;
            psessinfo->service_info[0] = 0;
		    #endif
			psessinfo->start_time = info->start_time;
			psessinfo->end_time = info->end_time;
            psessinfo->total_delay = 0;

			if (info->last_ack_cycle > info->request_cycle) 
			{
        		uint32_t t = (info->last_ack_cycle - info->request_cycle)/TIMER_CYCLE_PER_MS;
	    		psessinfo->transcation_delay = (0 == t ? 1 : t);
    		} 
			else 
    		{
        		psessinfo->transcation_delay = 0;
    		}
		
			psessinfo->response_delay = info->response_delay;
    		psessinfo->data_size = info->data_size;
			psessinfo->download_rate = info->download_rate;
    		psessinfo->download_delay = info->download_delay;
			//psessinfo->ip_info.ip_byte = si->bytes[1];
			//si->bytes[1] = 0;
			//psessinfo->ip_info.ip_pkt = si->pkts[1];
			//si->pkts[1] = 0;
			psessinfo->ip_info.ip_byte = 0;
            psessinfo->ip_info.ip_pkt = 0;
            psessinfo->ip_info.ip_fragment_num = 0;
#endif /* wdb_ppp */

//#ifdef UTAF_LOCK_SPINLOCK //wdb_lfix-6
            spinlock_unlock(&si->item_lock);
//#else //wdb_lfix-6
//            write_unlock(&(si->item_lock)); //wdb_lfix-6
//#endif //wdb_lfix-6

			memset(info, 0, sizeof(struct http_info));
#if 0 /* wdb_ppp */
			time_t end_time = g_utaf_time_jiffies; //time_t end_time = time(NULL);

#ifdef UTAF_DEBUG_HTTP
			printf("end_time: %d\n", info->end_time);
#endif
            /*HTTP LOG OUTPUT*/
		    utaf_cdr_http_output(plogp, si, end_time);
#endif /* wdb_ppp */
        }

        // get the last ACK time
        tcp = tcp4_hdr(mbuf);
        if (tcp->tcp_flags & 0x10) 
		{
            //si->app_info.httpinfo.last_ack_cycle = utaf_get_timer_cycles();
#ifdef UTAF_DEBUG_HTTP
			printf("last_ack_cycle: %ld\n", si->app_info.httpinfo.last_ack_cycle);
#endif
        }
    }

	return;
}

char * cstrstr(char *s, unsigned int slen, char *d, unsigned int dlen)
{	
	unsigned int rlen;
	char *r;
	
	if(!s || !d || dlen > slen)
		return NULL;
	
	rlen = slen;
	r = s; 
	while(rlen >= dlen)
	{
		if(!strncmp(r, d, dlen))
		{
			return r;
		}
		r++;
		rlen--;
	}
	
	return NULL;
}

