/**
 * @file dirwalk.h
 * @brief  遍历文件目录函数声明
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-08
 */


#ifndef _DIRWALK_H_
#define _DIRWALK_H_

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "list.h"

//#include "gms_store.h"


typedef struct _file_data {
    char path[ABSOLUT_FILE_PATH_SIZE];            
    char file_name[FILE_NAME_SIZE];
    char year[5];
    char month[3];
    char day[3];
    declare_list_node;
} file_data_t;

typedef struct _dirwalk_data {
    char root[DIR_FILE_SIZE];
    char type[DIR_FILE_SIZE];
    char kinds[DIR_FILE_SIZE];
    char year[5];
    char month[3];
    char day[3];
    list_t *list;
} dirwalk_t;

enum deep_level {
    ROOT_LEVEL = 0,
    TYPE_LEVEL,
    KINDS_LEVEL,
    YEAR_LEVEL,
    MONTH_LEVEL,
    DAY_LEVEL,
    FILE_LEVEL,
    
    MAX_LEVEL
};




/* -------------------------------------------*/
/**
 * @brief    通过指定的文件路径 和 指定的时间临界
 *            得到对应路径下 临界时间之后的 文件名称
 *            并封装到链表中 供后续使用
 *
 * @param root                文件根目录 
 * @param type                文件类型 例如"events"为事件 
 * @param kinds                文件种类 例如“ae”为异常事件
 *                                         “ve”为病毒事件等
 * @param time_str            时间临界值 "2013-08-09 23:03:21"格式
 * @param file_list_head    得到的链表
 *
 * @returns   
 *                            0
 */
/* -------------------------------------------*/
int dirwalk_dumplist_by_time(char *root, char* type, char* kinds, char* time_str, list_t *file_list_head);

/* -------------------------------------------*/
/**
 * @brief  只遍历当前目录， 目录下所有.ok结尾的
 *            文件名加入到链表中 
 *
 * @param root
 * @param file_list_head
 *
 * @returns   
 *                SUCC  0
 *                FAIL  -1
 */
/* -------------------------------------------*/
int dirwalk_current_list(char *root, list_t *file_list_head);


/* -------------------------------------------*/
/**
 * @brief  在链表中打印文件路径
 *
 * @param data
 * @param arg
 *
 * @returns   
 */
/* -------------------------------------------*/
int show_file_data(void *data, void *arg);

/* -------------------------------------------*/
/**
 * @brief  检查这个设备状态入库文件是否是自身设备的
 *
 * @param name  设备注册文件
 * @param devid 本端devid
 *
 * @returns   
 *          0   是自身的
 *          1   非自身的
 *          -1  错误
 */
/* -------------------------------------------*/
int check_file_is_localdev(char *name, char *devid);

#endif

