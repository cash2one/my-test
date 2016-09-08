#ifndef __HTTP_H__
#define __HTTP_H__

#include "tcad.h"

//#define HTTP_URL_LENGTH_MAX 128
//#define HTTP_HOST_LENGTH_MAX 64

#define HTTP_URL_LENGTH_MAX 65535
#define HTTP_HOST_LENGTH_MAX 128



#define HTTP_KEYWORD_GET  "GET "
#define HTTP_KEYWORD_POST "POST "
#define HTTP_KEYWORD_HEAD "HEAD "
#define HTTP_KEYWORD_HOST "Host: "

#define HTTP_KEYWORD_GET_SIZE  strlen(HTTP_KEYWORD_GET)
#define HTTP_KEYWORD_POST_SIZE strlen(HTTP_KEYWORD_POST)
#define HTTP_KEYWORD_HOST_SIZE strlen(HTTP_KEYWORD_HOST)


#define HTTP_KEYWORD_VERSION_1 "HTTP/1.0 "
#define HTTP_KEYWORD_VERSION_2 "HTTP/1.1 "
#define HTTP_KEYWORD_VERSION_1_SIZE strlen(HTTP_KEYWORD_VERSION_1)
#define HTTP_KEYWORD_VERSION_2_SIZE strlen(HTTP_KEYWORD_VERSION_2)

#define HTTP_KEYWORD_USERAGENT  "User-Agent: "
#define HTTP_KEYWORD_REFERER  "Referer: "
#define HTTP_KEYWORD_PARAM  "Param: "
#define HTTP_KEYWORD_USERAGENT_SIZE  strlen(HTTP_KEYWORD_USERAGENT)
#define HTTP_KEYWORD_REFERER_SIZE  strlen(HTTP_KEYWORD_REFERER)
#define HTTP_KEYWORD_PARAM_SIZE  strlen(HTTP_KEYWORD_PARAM)

#define HTTP_KEYWORD_SPACE_SIZE 1



#define HTTP_STATUS_CODE_SIZE_MAX 5


#define HTTP_UNLEGAL  0
#define HTTP_LEGAL    1


#define HTTP_NONE     0
#define HTTP_REQUEST  1
#define HTTP_RESPONSE 2

#define HTTP_SERVICE_CMD_GET    64
#define HTTP_SERVICE_CMD_POST   96

struct http_info
{
	uint8_t service_status;
	uint8_t service_cmd;
	uint8_t http_status;
    uint8_t res;
	int32_t response_code; 
	//char url[HTTP_URL_LENGTH_MAX];
	//char host[HTTP_HOST_LENGTH_MAX];
    //char browser[SAM_STR_SIZE+1];
    int browser_id;
    char version[SAM_STR_SIZE+1];
	uint64_t request_cycle;
	uint64_t response_cycle;
#if 0
	uint64_t end_cycle;
	time_t start_time;
	time_t end_time;
    uint64_t request_byte;
#endif
    uint32_t response_delay;
    uint32_t data_size;
#if 0
    uint32_t download_rate;
    uint32_t download_delay;
    uint64_t last_ack_cycle;
#endif
};

void http_parser(struct m_buf *mbuf);

#endif
