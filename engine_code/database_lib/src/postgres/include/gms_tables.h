/**
 * @file gms_tables.h
 * @brief  数据库有关表结构定义
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-14
 */
#ifndef _GMS_TABLES_H_
#define _GMS_TABLES_H_

/* 表名 */

/* 特征攻击事件 */
#define TMP_TIME_NAME                        "end_time"
#define TABLE_NAME_ATTACK_TMP                "t_event_attacktmp"
#define TABLE_NAME_ATTACK_MIN                "t_event_attackmin"
#define TABLE_NAME_ATTACK_HOUR               "t_event_attackhour"
#define TABLE_NAME_ATTACK_DAY                "t_event_attackday"
#define TABLE_NAME_ATTACK_WEEK               "t_event_attackweek"
#define TABLE_NAME_ATTACK_MONTH              "t_event_attackmonth"
#define TABLE_NAME_ATTACK_YEAR               "t_event_attackyear"
#define TABLE_NAME_ATTACK_HIS                "t_event_attackhis"
#define TABLE_NAME_ATTACK_QUERY              "t_event_attackquery"

/* 特征其他3种事件 */
#define TABLE_NAME_3RD_TMP                  "t_event_3rdtmp"
#define TABLE_NAME_3RD_MIN                  "t_event_3rdmin"
#define TABLE_NAME_3RD_HOUR                 "t_event_3rdhour"
#define TABLE_NAME_3RD_DAY                  "t_event_3rdday"
#define TABLE_NAME_3RD_WEEK                 "t_event_3rdweek"
#define TABLE_NAME_3RD_MONTH                "t_event_3rdmonth"
#define TABLE_NAME_3RD_YEAR                 "t_event_3rdyear"
#define TABLE_NAME_3RD_HIS                  "t_event_3rdhis"
#define TABLE_NAME_3RD_QUERY                "t_event_3rdquery"

/* 恶意代码事件 */
#define TABLE_NAME_AV                       "t_event_av"

/* 异常行为事件 */
#define TABLE_NAME_ABB                      "t_event_abb"

/* 恶意URL事件 */
#define TABLE_NAME_BURL                     "t_event_burl"

/* 恶意DNS事件 */
#define TABLE_NAME_BDNS                     "t_event_bdns"

/* 设备状态事件 */
#define TABLE_NAME_DEV_STATUS               "t_event_sysstatus"

/* 流量表 */
#define TABLE_NAME_FLOW_MIN                 "t_event_flowminu"
#define TABLE_NAME_FLOW_HOUR                "t_event_flowhour"
#define TABLE_NAME_FLOW_DAY                 "t_event_flowday"


#define TABLE_NAME_SIZE                     (30)
#define TABLE_SUFFIX                        "_p"
#define TABLE_SUFFIX_MONTH                  "_m"
#define TABLE_SUFFIX_DAY                    "_d"
#define TABLE_SUFFIX_SIZ                    (3)

#endif

