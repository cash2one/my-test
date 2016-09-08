/******************************************************************************
description : header file for zywk and e1000 applications
author : hert
date : 2007.07.10
email : liuzhh@dawning.com.cn, liuzhaohui@tsinghua.org.cn
 ******************************************************************************/


#ifndef __LIBPPF_H__
#define __LIBPPF_H__

#include <linux/types.h>


//////use function definations in libppf

#define DEBUG_PPF 0

#define DUM_CONF_FILE "dum.conf"
#define PAG_CONF_FILE "pag.conf"


//open devices listed in file "dum.conf"
//if failed, return < 0
typedef int (*ppf_open_t)();

//close device with device id
typedef void (*ppf_close_t)();

//set stream number 
//if failed, return < 0
typedef int (*ppf_setStreamNum_t)(int sn);
typedef int (*ppf_getStreamNum_t)();

//recieve ad data block from zcp device, return no matter if there are packets
typedef void *(*ppf_getDataBlock_no_Lock_t)(int sid);

//return if there are packets, block otherwise
typedef void *(*ppf_recvDataBlock_t)(int sid);

//free a packet to zcp device
//return 1 if succeed, -1 if failed
typedef int (*ppf_freeDataBlock_t)(const void *pkt);

//retrieve the ip packet
typedef void* (*ppf_getIPPacket_t)(const void *pkt);

//retrieve the interface id of the ip
typedef __u32 (*ppf_getIfId_t)(const void *pkt);

typedef __u64 (*ppf_getTimeStamp_t)(const void *pkt);


typedef __u32 (*ppf_getPktLength_t)(const void *pkt);	

#endif
