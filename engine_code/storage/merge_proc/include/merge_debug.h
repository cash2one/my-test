/**
 * @file merge_debug.h
 * @brief  合并自摸库debug调试配置
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-15
 */

#ifndef _MERGE_DEBUG_H_
#define _MERGE_DEBUG_H_

#include "trace_api.h"

#define MERGE_TRACE_FILE_PATH		"./conf/merge_trace.conf"


enum merge_debug_type {
	DEBUG_TYPE_TRUNK = 0,
	DEBUG_TYPE_FLOW,
	DEBUG_TYPE_ATTACK,
};

typedef struct _merge_debug_info {
	unsigned char debug_all;
	unsigned char debug_trunk;
	unsigned char debug_flow;
	unsigned char debug_attack;
} merge_debug_info_t;


#define _MERGE_TRUNK_DEBUG		0x0001
#define _MERGE_FLOW_DEBUG		0x0002
#define _MERGE_ATTACK_DEBUG		0x0004

#define IS_MERGE_TRUNK_DEBUG_ON (g_merge_debug & _MERGE_TRUNK_DEBUG)
#define IS_MERGE_FLOW_DEBUG_ON (g_merge_debug & _MERGE_FLOW_DEBUG)
#define IS_MERGE_ATTACK_DEBUG_ON (g_merge_debug & _MERGE_ATTACK_DEBUG)


/* -------------------------------------------*/
/**
 * @brief  配置各个模块的debug开关
 *
 * @param debug_info
 *
 * @returns   
 */
/* -------------------------------------------*/
#define MERGE_SET_DEBUG(debug_info)\
do { \
	if(debug_info.debug_all == 1) {\
		g_merge_debug |= _MERGE_TRUNK_DEBUG;\
		g_merge_debug |= _MERGE_FLOW_DEBUG;\
		g_merge_debug |= _MERGE_ATTACK_DEBUG;\
		break;\
	}\
	if (debug_info.debug_trunk == 1) {\
		g_merge_debug |= _MERGE_TRUNK_DEBUG;\
	}\
	if (debug_info.debug_flow == 1) {\
		g_merge_debug |= _MERGE_FLOW_DEBUG;\
	}\
	if (debug_info.debug_attack == 1) {\
		g_merge_debug |= _MERGE_ATTACK_DEBUG;\
	}\
}while(0)


/* -------------------------------------------*/
/**
 * @brief  合并模块打印debug信息
 *
 * @param type		哪个模块
 * @param x...		信息
 *
 * @returns   
 */
/* -------------------------------------------*/
#define MERGE_DEBUG(type, x...) \
do {\
	switch(type) {\
		case DEBUG_TYPE_TRUNK: \
			if (IS_MERGE_TRUNK_DEBUG_ON) {\
				DEBUGMSG(MERGE_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_FLOW: \
			if (IS_MERGE_FLOW_DEBUG_ON) {\
				DEBUGMSG(MERGE_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_ATTACK: \
			if (IS_MERGE_ATTACK_DEBUG_ON) {\
				DEBUGMSG(MERGE_PROC, ##x);\
			}\
			break;\
	}\
}while(0)


/* -------------------------------------------*/
/**
 * @brief  合并模块打印ERROR信息
 *
 * @param type		哪个模块
 * @param x...		信息
 *
 * @returns   
 */
/* -------------------------------------------*/
#define MERGE_ERROR(type, x...) \
do {\
	switch(type) {\
		case DEBUG_TYPE_TRUNK: \
			if (IS_MERGE_TRUNK_DEBUG_ON) {\
				ERRMSG(MERGE_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_FLOW: \
			if (IS_MERGE_FLOW_DEBUG_ON) {\
				ERRMSG(MERGE_PROC, ##x);\
			}\
			break;\
		case DEBUG_TYPE_ATTACK: \
			if (IS_MERGE_ATTACK_DEBUG_ON) {\
				ERRMSG(MERGE_PROC, ##x);\
			}\
			break;\
	}\
}while(0)

extern unsigned int			g_merge_debug;		/* 合并莫debug配置 */
extern trace_init_t			g_merge_trace;				/* trace 全局 */
extern merge_debug_info_t	g_merge_debug_cfg;			

#endif

