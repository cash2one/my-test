/**
 * @file store_file_op.h
 * @brief  存储有关对数据文件统计操作声明
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-18
 */
#ifndef _STORE_FILE_OP_H_
#define _STORE_FILE_OP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------*/
/**
 * @brief  数据文件 获取事件数目
 *
 * @param buf    数据文件buf
 * @param buf_size    文件长度
 *
 * @returns   
 *            数目
 */
/* -------------------------------------------*/
unsigned int get_event_num(char *buf, unsigned long buf_size);


/* -------------------------------------------*/
/**
 * @brief  得到本行的制定字段数据 字段号从1开始
 *
 * @param line_head        本行首地址
 * @param col            字段号 
 * @param value            得到的数据
 * @param max_len        数据最大长度
 * @param with_quote    该字段是否有“”包括 1 是 其他否
 *
 * @returns   
 *        得到的数据
 */
/* -------------------------------------------*/
char* get_value_by_col(char *line_head,int col, char *value,int max_len, char with_quote);

#endif


