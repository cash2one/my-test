/**
 * @file flow_debug.h
 * @brief  流量存储入库debug调试配置
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-12-01
 */

#ifndef _FLOW_DEBUG_H_
#define _FLOW_DEBUG_H_

#include "trace_api.h"

#define FLOW_TRACE_FILE_PATH "./conf/flow_trace.conf"

enum flow_debug_type {
	DEBUG_TYPE_TRUNK = 0,
	DEBUG_TYPE_STORE,
	DEBUG_TYPE_MAKE,
	DEBUG_TYPE_SPENDTIME,
};

typedef struct _flow_debug_info {
	unsigned char debug_all;
	unsigned char debug_trunk;
	unsigned char debug_store;
	unsigned char debug_make;
	unsigned char debug_spendtime;
    unsigned char store_enable;
    unsigned char statistics_enable;
    unsigned char flowevent_store_disenable;
} flow_debug_info_t;


#define _FLOW_TRUNK_DEBUG		0x0001
#define _FLOW_STORE_DEBUG		0x0002
#define _FLOW_MAKE_DEBUG		0x0004
#define _FLOW_SPENDTIME_DEBUG	0x0008

#define IS_FLOW_TRUNK_DEBUG_ON (g_flow_debug & _FLOW_TRUNK_DEBUG)
#define IS_FLOW_STORE_DEBUG_ON (g_flow_debug & _FLOW_STORE_DEBUG)
#define IS_FLOW_MAKE_DEBUG_ON (g_flow_debug & _FLOW_MAKE_DEBUG)
#define IS_FLOW_SPENDTIME_DEBUG_ON (g_flow_debug & _FLOW_SPENDTIME_DEBUG)


/* -------------------------------------------*/
/**
 * @brief  配置各个模块的debug开关
 *
 * @param debug_info
 *
 * @returns   
 */
/* -------------------------------------------*/
#define FLOW_SET_DEBUG(debug_info)\
do { \
	if(debug_info.debug_all == 1) {\
		g_flow_debug |= _FLOW_TRUNK_DEBUG;\
		g_flow_debug |= _FLOW_STORE_DEBUG;\
		g_flow_debug |= _FLOW_MAKE_DEBUG;\
		g_flow_debug |= _FLOW_SPENDTIME_DEBUG;\
		break;\
	}\
	if (debug_info.debug_trunk == 1) {\
		g_flow_debug |= _FLOW_TRUNK_DEBUG;\
	}\
	if (debug_info.debug_store == 1) {\
		g_flow_debug |= _FLOW_STORE_DEBUG;\
	}\
	if (debug_info.debug_store == 1) {\
		g_flow_debug |= _FLOW_MAKE_DEBUG;\
	}\
	if (debug_info.debug_spendtime == 1) {\
		g_flow_debug |= _FLOW_STORE_DEBUG;\
	}\
}while(0)

#define FLOW_DEBUG(type, x...) \
do {\
	switch(type) {\
		case DEBUG_TYPE_TRUNK: \
			if (IS_FLOW_TRUNK_DEBUG_ON) {\
				DEBUGMSG(FLOW_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_STORE: \
			if (IS_FLOW_STORE_DEBUG_ON) {\
				DEBUGMSG(FLOW_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_MAKE: \
			if (IS_FLOW_MAKE_DEBUG_ON) {\
				DEBUGMSG(FLOW_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_SPENDTIME: \
			if (IS_FLOW_SPENDTIME_DEBUG_ON) {\
				DEBUGMSG(FLOW_PROC, ##x);\
			}\
			break;\
	}\
}while(0)

#define FLOW_ERROR(type, x...) \
do {\
	switch(type) {\
		case DEBUG_TYPE_TRUNK: \
			if (IS_FLOW_TRUNK_DEBUG_ON) {\
				ERRMSG(FLOW_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_STORE: \
			if (IS_FLOW_STORE_DEBUG_ON) {\
				ERRMSG(FLOW_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_MAKE: \
			if (IS_FLOW_MAKE_DEBUG_ON) {\
				ERRMSG(FLOW_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_SPENDTIME: \
			if (IS_FLOW_SPENDTIME_DEBUG_ON) {\
				ERRMSG(FLOW_PROC, ##x);\
			}\
			break;\
	}\
}while(0)

extern unsigned int			g_flow_debug;		/* 合并莫debug配置 */
extern trace_init_t			g_flow_trace;				/* trace 全局 */
extern flow_debug_info_t	g_flow_debug_cfg;			


#endif

