/* 
 * Last coding by Chen Yu
 */

#define _SPECIAL_DRV_ //added by duanjigang@2009-04-16
#include <netinet/if_ether.h>
#include "plugin_manage.h"
//#include "dfp_ether_decode.h"

//#include "thread.h"
//#include "trace_api.h"
//#include "time_consume.h"
//#include "perform.h"
//PER_DECLARE(ether)

//unsigned char special_eth_hdr[14] = {0};
void *plugin_init(int ptheadnum)
{
#if 0
    int i = 0;
    for(i = 0; i < 6; i++) {
	special_eth_hdr[i] = (10+i)*17;;
    }
    for(i = 0; i < 6; i++) {
	special_eth_hdr[6 + i] = (15-i)*17;;
    }
    special_eth_hdr[12] = 8;
    special_eth_hdr[13] = 0;
    return 0;
#endif
}

/********************************************************************
 > function:
	1.专用网卡驱动的捕包功能的插件处理函数，作为so库函数对外提供
 > param:
	1.ppkt:传入的数据报文信息
 > return:
	1.CONTITUNE_GIVE_ME:继续传送报文给本插件
	2.NO_CONTINUE_GIVE_ME:该会话的报文后续不用给本插件
 > modify:
	1.create by wlz
 ************************************************************************/
int plugin_proc(pkt_info_t *ppkt)
{
    int ret = CONTINUE_GIVE_ME;

    //ppkt->eth_length = packet->raw_pkt_len;
    struct ethhdr *eth_hdr = (struct ethhdr *)ppkt->raw_pkt;
    //ppkt->eth_smac = special_eth_hdr + 6;// eth_hdr->h_source;
    //ppkt->eth_dmac = special_eth_hdr;//eth_hdr->h_dest;
    //ppkt->eth_smaclen = ETH_ALEN;
    //ppkt->eth_dmaclen = ETH_ALEN;
    //set protocol path. 
    ppkt->eth_payload = (u_int8_t *)(eth_hdr + 1);
    ppkt->eth_payload_len = ppkt->eth_payload_len = ppkt->raw_pkt_len - sizeof(struct ethhdr);
   // ppkt->proto_path[packet->proto_path_len++] = PROTO_ID_ETHER;
    ppkt->eth_proto = 0x0800; 

    ppkt->hook_id = IP_HOOK;
    return  ret;
}

int plugin_clean(pkt_info_t *ppkt)
{
    return 0;
}

/*跟tcp相关的hook，tcp会话结束的清除函数指针*/
int plugin_tcp_clean(pkt_info_t *ppkt)
{
    return 0;
}
