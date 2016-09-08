/**
 * @file store_debug.h
 * @brief  存储子模块debug调试配置
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-11
 */
#ifndef _STORE_DEBUG_H_
#define _STORE_DEBUG_H_

#define TEST_SPEND_TIME

#include "trace_api.h"

#define STORE_TRACE_FILE_PATH            "./conf/store_trace.conf"

extern unsigned int g_store_debug;        /* 存储模块debug配置 */



enum store_debug_type {
    DEBUG_TYPE_TRUNK = 0,
    DEBUG_TYPE_ATT,
    DEBUG_TYPE_3RD,
    DEBUG_TYPE_AV,
    DEBUG_TYPE_ABB,
    DEBUG_TYPE_BLACK,
    DEBUG_TYPE_DEV_STATUS,
    DEBUG_TYPE_SPENDTIME
};

typedef struct _store_debug_info {
    unsigned char debug_all;
    unsigned char debug_trunk;
    unsigned char debug_attack;
    unsigned char debug_3rd;
    unsigned char debug_av;
    unsigned char debug_abb;
    unsigned char debug_black;
    unsigned char debug_dev_status;
    unsigned char debug_spendtime;
} store_debug_info_t;

#define _STORE_TRUNK_DEBUG        0x0001
#define _STORE_ATTACK_DEBUG        0x0002
#define _STORE_3RD_DEBUG        0x0004
#define _STORE_AV_DEBUG            0x0008
#define _STORE_ABB_DEBUG        0x0010
#define _STORE_SPENDTIME_DEBUG    0x0020
#define _STORE_BLACK_DEBUG        0x0040
#define _STORE_DEV_STATUS_DEBUG   0x0080

#define IS_STORE_TRUNK_DEBUG_ON (g_store_debug & _STORE_TRUNK_DEBUG)
#define IS_STORE_ATTACK_DEBUG_ON    (g_store_debug & _STORE_ATTACK_DEBUG)
#define IS_STORE_3RD_DEBUG_ON    (g_store_debug & _STORE_3RD_DEBUG)
#define IS_STORE_AV_DEBUG_ON    (g_store_debug & _STORE_AV_DEBUG)
#define IS_STORE_ABB_DEBUG_ON    (g_store_debug & _STORE_ABB_DEBUG)
#define IS_STORE_SPENDTIME_DEBUG_ON (g_store_debug & _STORE_SPENDTIME_DEBUG)
#define IS_STORE_BLACK_DEBUG_ON    (g_store_debug & _STORE_BLACK_DEBUG)
#define IS_STORE_DEV_STATUS_DEBUG_ON (g_store_debug &_STORE_DEV_STATUS_DEBUG)


/* -------------------------------------------*/
/**
 * @brief  配置各个模块的debug开关
 *
 * @param debug_info
 *
 * @returns   
 */
/* -------------------------------------------*/
#define STORE_SET_DEBUG(debug_info)\
do { \
    if(debug_info.debug_all == 1) {\
        g_store_debug |= _STORE_TRUNK_DEBUG;\
        g_store_debug |= _STORE_ATTACK_DEBUG;\
        g_store_debug |= _STORE_3RD_DEBUG;\
        g_store_debug |= _STORE_AV_DEBUG;\
        g_store_debug |= _STORE_ABB_DEBUG;\
        g_store_debug |= _STORE_BLACK_DEBUG;\
        g_store_debug |= _STORE_DEV_STATUS_DEBUG;\
        g_store_debug |= _STORE_SPENDTIME_DEBUG;\
        break;\
    }\
    if (debug_info.debug_trunk == 1) {\
        g_store_debug |= _STORE_TRUNK_DEBUG;\
    }\
    if (debug_info.debug_attack == 1) {\
        g_store_debug |= _STORE_ATTACK_DEBUG;\
    }\
    if (debug_info.debug_3rd == 1) {\
        g_store_debug |= _STORE_3RD_DEBUG;\
    }\
    if (debug_info.debug_av == 1) {\
        g_store_debug |= _STORE_AV_DEBUG;\
    }\
    if (debug_info.debug_abb == 1) {\
        g_store_debug |= _STORE_ABB_DEBUG;\
    }\
    if (debug_info.debug_black == 1) {\
        g_store_debug |= _STORE_BLACK_DEBUG;\
    }\
    if (debug_info.debug_dev_status == 1) {\
        g_store_debug |= _STORE_DEV_STATUS_DEBUG;\
    }\
    if (debug_info.debug_spendtime == 1) {\
        g_store_debug |= _STORE_SPENDTIME_DEBUG;\
    }\
}while(0)


/* -------------------------------------------*/
/**
 * @brief  存储模块打印debug信息
 *
 * @param type        哪个模块
 * @param x...        信息
 *
 * @returns   
 */
/* -------------------------------------------*/
#define STORE_DEBUG(type, x...) \
do {\
    switch(type) {\
        case DEBUG_TYPE_TRUNK: \
            if (IS_STORE_TRUNK_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_ATT: \
            if (IS_STORE_ATTACK_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_3RD: \
            if (IS_STORE_3RD_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_AV: \
            if (IS_STORE_AV_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_ABB: \
            if (IS_STORE_ABB_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_BLACK: \
            if (IS_STORE_BLACK_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_DEV_STATUS: \
            if (IS_STORE_DEV_STATUS_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_SPENDTIME: \
            if (IS_STORE_SPENDTIME_DEBUG_ON) {\
                DEBUGMSG(STORE_PROC, ##x);\
            }\
            break;\
    }\
}while(0)

/* -------------------------------------------*/
/**
 * @brief  存储模块打印Error信息
 *
 * @param type        哪个模块
 * @param x...        信息
 *
 * @returns   
 */
/* -------------------------------------------*/
#define STORE_ERROR(type, x...) \
do {\
    switch(type) {\
        case DEBUG_TYPE_TRUNK: \
            if (IS_STORE_TRUNK_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_ATT: \
            if (IS_STORE_ATTACK_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_3RD: \
            if (IS_STORE_3RD_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_AV: \
            if (IS_STORE_AV_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_ABB: \
            if (IS_STORE_ABB_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_BLACK: \
            if (IS_STORE_BLACK_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_DEV_STATUS:\
            if (IS_STORE_DEV_STATUS_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
        case DEBUG_TYPE_SPENDTIME: \
            if (IS_STORE_SPENDTIME_DEBUG_ON) {\
                ERRMSG(STORE_PROC, ##x);\
            }\
            break;\
    }\
}while(0)



#endif
