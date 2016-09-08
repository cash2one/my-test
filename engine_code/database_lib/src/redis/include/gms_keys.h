/**
 * @file gms_keys.h
 * @brief  redis表 KEYS 值相关定义 
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-18
 */
#ifndef _GMS_KEYS_H_
#define _GMS_KEYS_H_


#define KEY_NAME_SIZ                (50)
#define REDIS_DILIMT                "||"
#define REDIS_DILIMT_SIZ            (3)


/*--------------------------------.
| 事件类型                        |
`--------------------------------*/
#define EVENT_TYPE_MALICIOUS        "1"
#define EVENT_TYPE_WEB                "2"
#define EVENT_TYPE_ATTACK            "3"
#define EVENT_TYPE_ELSE                "4"
#define EVENT_TYPE_ABB                "5"
#define EVENT_TYPE_AV                "6"
#define EVENT_TYPE_BDNS                "7"
#define EVENT_TYPE_BURL                "8"


/*--------------------------------.
| 今日事件统计表(HASH)            |
| Key:     T_PM_EVENTLOG_COUNT    |
| Field:   event_type             |
| Value:   event_num              |
| redis 语句                      |
|    HINCRBY key field increment  |
`--------------------------------*/
#define KEY_EVENT_COUNT                "T_PM_EVENTLOG_COUNT"


/*------------------------------------------.
| 今日被攻击TOP5表 (ZSET)                   |
| Key:     T_PM_EVENTLOG_ATTACK_TOP5        |
| Member:  attack_ip                        |
| Score:   attack_num                       |
| redis 语句                                |
|    ZINCRBY key increment member           |
`------------------------------------------*/
#define KEY_EVENT_ATTACK_TOP5        "T_PM_EVENTLOG_ATTACK_TOP5"
#define KEY_EVENT_ATTACK_WEEK_TOP10        "T_PM_EVENTLOG_ATTACK_WEEK_TOP10"


/*------------------------------------------.
| 今日事件展示表 (LIST)                     |
|攻击事件，恶意代码感染，网站事件，其他事件 |
| Key:  T_PM_EVENTLOG_ATTACK_ATTACK         |
|       T_PM_EVENTLOG_ATTACK_MALICIOUS_CODE |
|       T_PM_EVENTLOG_ATTACK_WEB            |
|       T_PM_EVENTLOG_ATTACK_ELSE           |
| Value: start_time,end_time,devid,ename,daddr,saddr,    |
|         sport,dport,risk_level            |
| redis 语句                                |
|    插入 LPUSH key value                   |
|    查询个数 LLEN key                      |
|    大于最大需要截断 LTRIM key 0 max-1     |
|    查询链表数据    LRANGE key 0 max-1     |
`------------------------------------------*/
#define KEY_EVENT_SHOW_ATTACK        "T_PM_EVENTLOG_ATTACK_ATTACK"
#define KEY_EVENT_SHOW_CODE            "T_PM_EVENTLOG_ATTACK_MALICIOUS_CODE"
#define KEY_EVENT_SHOW_WEB            "T_PM_EVENTLOG_ATTACK_WEB"
#define KEY_EVENT_SHOW_ELSE            "T_PM_EVENTLOG_ATTACK_ELSE"


/*------------------------------------------.
| 今日恶意代码传播事件展示表 (LIST)         |
| Key:    T_PM_EVENTLOG_VDS                 |
| Value: ts,vsname,sip,dip,sport,dport,pid, |
|         btype                             |
| redis 语句                                |
|    插入 LPUSH key value                   |
|    查询个数 LLEN key                      |
|    大于最大需要截断 LTRIM key 0 max-1     |
|    查询链表数据    LRANGE key 0 max-1     |
`------------------------------------------*/
#define KEY_EVENT_SHOW_AV                "T_PM_EVENTLOG_VDS"

/*------------------------------------------.
| 今日异常行为事件展示表 (LIST)             |
| Key:    T_PM_EVENTLOG_ABB                 |
| Value: ctime,sip,sport,dip,dport    TODO  |
| redis 语句                                |
|    插入 LPUSH key value                   |
|    查询个数 LLEN key                      |
|    大于最大需要截断 LTRIM key 0 max-1     |
|    查询链表数据    LRANGE key 0 max-1     |
`------------------------------------------*/
#define KEY_EVENT_SHOW_ABB                "T_PM_EVENTLOG_ABB"

/*------------------------------------------.
| 今日流量曲线表 (LIST)                     |
| 今日事件展示表 (LIST)                     |
| 全部，TCP/IP，UDP，其他                   |
| Key:    T_PM_EVENTLOG_FLOWALL             |
|        T_PM_EVENTLOG_FLOWTCP              |
|        T_PM_EVENTLOG_FLOWUDP              |
|        T_PM_EVENTLOG_FLOWOTHER            |
| Value: flow_time,pps,bps                  |
| redis 语句                                |
|    插入 LPUSH key value                   |
|    查询个数 LLEN key                      |
|    大于最大需要截断 LTRIM key 0 max-1     |
|    查询链表数据    LRANGE key 0 max-1     |
`------------------------------------------*/
#define KEY_EVENT_FLOW_ALL                "T_PM_EVENTLOG_FLOWALL"
#define KEY_EVENT_FLOW_TCP                "T_PM_EVENTLOG_FLOWTCP"
#define KEY_EVENT_FLOW_UDP                "T_PM_EVENTLOG_FLOWUDP"
#define KEY_EVENT_FLOW_OTHER            "T_PM_EVENTLOG_FLOWOTHER"

/*------------------------------------------.
| 今日黑名单 恶意DNS 展示表                 |
| Key:    T_PM_EVENTLOG_BDNS                |
| Value: devid,sip,dip,dnsip,dns,ctime      |
| redis 语句                                |
|    插入 LPUSH key value                   |
|    查询个数 LLEN key                      |
|    大于最大需要截断 LTRIM key 0 max-1     |
|    查询链表数据    LRANGE key 0 max-1     |
`------------------------------------------*/
#define KEY_EVENT_SHOW_BLACK_DNS        "T_PM_EVENTLOG_BDNS"

/*------------------------------------------.
| 今日黑名单 恶意URL 展示表                 |
| Key:    T_PM_EVENTLOG_BURL                |
| Value: devid,sip,dip,sport,dport,url,ctime|
| redis 语句                                |
|    插入 LPUSH key value                   |
|    查询个数 LLEN key                      |
|    大于最大需要截断 LTRIM key 0 max-1     |
|    查询链表数据    LRANGE key 0 max-1     |
`------------------------------------------*/
#define KEY_EVENT_SHOW_BLACK_URL        "T_PM_EVENTLOG_BURL"


/*------------------------------------------.
| 设备状态表 (HASH)                         |
| Key:       T_PM_EVENTLOG_DEVMAP           |
| Field:   devid                            |
| Value:   c_time,disk,cpu,mem,state,       |
|          runtime,libversion,sysversion    |
| redis 语句                                |
|    HSET   key field value                 |
`------------------------------------------*/
#define KEY_EVENT_DEVMAP            "T_PM_EVENTLOG_DEVMAP"




#endif

