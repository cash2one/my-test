/******************************************************************************
description : header file for zywk and e1000 applications
author : hert
date : 2007.07.10
email : liuzhh@dawning.com.cn, liuzhaohui@tsinghua.org.cn
 ******************************************************************************/


#ifndef __LIBPAG_H__
#define __LIBPAG_H__

#include <linux/types.h>


//if failed, return < 0
typedef int (*pag_open_t)();

//close device with device id
typedef void (*pag_close_t)();


//recieve ad data block from zcp device, return no matter if there are packets
typedef void *(*pag_get_t)(int sid);


//free a packet to zcp device
//return 1 if succeed, -1 if failed
typedef  int (*pag_free_t)(int sid,const void *pkt);


typedef __u64 (*pag_time_t)(const void *pkt);

#endif
