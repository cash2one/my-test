/*
 * =====================================================================================
 *       Filename:  send_warmboot_signal.h
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008-10-07 15:13:10
 *         Author:  idp idp@venus.com
 *        Company:  venustech
 * =====================================================================================
 */

#ifndef _SEND_WARMBOOT_SIGNAL_H
#define _SEND_WARMBOOT_SIGNAL_H
#include <sys/types.h>

#define SQL_PROTECT_CONF 	1	//ÐÞ¸ÄÁËvxid_protect_host.confÎÄ¼þ
#define VXID_PROTECT_CONF 	2	// ÐÞ¸ÄÁËsql_protect_host.confÎÄ¼þ
#define ENGINE_CONF      	3	// ÐÞ¸ÄÁËengine.conf£¬µ«ÊÇÃ»ÓÐÐÞ¸Ä¶ÔÓ¦µÄ²ßÂÔÎÄ¼þ
#define POLICY_CONF		4	// ÐÞ¸ÄÁË²ßÂÔÎÄ¼þ
#define EVTMERG_CONF		5	// ÐÞ¸ÄÁËcsign_merg.conf
#define SECEVT_CONF 		6 	//¶þ´ÎÊÂ¼þµÄ¹æÔòÎÄ¼þÐÞ¸ÄÁË
#define EVT_LIMIT_CONF 		7 	//ÉÏ±¨Æµ¶ÈÎÄ¼þÐÞ¸Ä
#define EVT_INFO_FILE_UPDATE	8	//½øÐÐÁËÊÂ¼þÐÅÏ¢¸üÐÂ
#define VENGINE_SWITCH_CONF	9

#define OTHER_CONF 		0	// ÆäËüÎÄ¼þÐÞ¸Ä


#define SEND_ENGINE_CONF  	63
#define SEND_VXID_PROTECT_CONF 	62
#define SEND_SQL_PROTECT_CONF 	61
#define SEND_POLICY_CONF 	60
#define COMM2_TIMER_SIGNO	59
#define EVT_INFO_UPDATE		58
#define SEND_VENGINE_SWITCH	57


int send_signal(int signo, char * proc_name);
int send_warmboot_signal(int filenum);
#endif
