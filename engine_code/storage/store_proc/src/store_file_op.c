/**
 * @file store_file_op.c
 * @brief    恶意代码解析数据文件操作
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-18
 */

//#include "gms_store.h"
#include "gms_store.h"
#include "store_file_op.h"
#include "store_common.h"

/* -------------------------------------------*/
/**
 * @brief  恶意代码 数据文件 获取事件数目
 *
 * @param buf    数据文件buf
 * @param buf_size    文件长度
 *
 * @returns   
 *            数目
 */
/* -------------------------------------------*/
unsigned int get_event_num(char *buf, unsigned long buf_size)
{
    unsigned int line    = 0;
    char *pos            = NULL;
    char *end            = NULL;

    if (buf == NULL || buf_size == 0) {
        return line;
    }

    for (pos = buf, end = buf + buf_size; pos != end; ++pos) {
        if (*pos == '\n') {
            ++line;
        }
    }

    if (line == 0 && buf_size != 0) {
        ++line;
    }

#if 0
    if (line != 0) {
        /* HEADER */
        --line;
    }
#endif

    return line;
}

unsigned int get_line_by_col(char *buf, unsigned long buf_size, unsigned char col, char *value)
{
#if 0
    unsigned int line    = 0;
    char *pos            = NULL;    
    char *end            = NULL;
    char tmp_value[VALUES_ID_SIZE];

    for (pos = buf, end = buf + buf_size; pos != end; ++pos) {
        tmp_value = get_value_by_col();    
    }
#endif
}

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
char* get_value_by_col(char *line_head, int col, char *value, int max_len, char with_quote)
{
    char *pos = NULL;        
    char *value_head = NULL;
    int tmp_col = 0;
    int value_len = 0;


    if (col < 1) {
        fprintf(stderr, "col must >= 1\n");
        goto END;
    }

    /* 预留 \0 空间 */
    --max_len;

    /* 第一个字段 */
    if (col == 1) {
        for (pos = line_head, value_head = line_head; *pos != '|' && *(pos-1) != '\\'; ++pos);
        if (with_quote == 1) {
            value_len = ((pos - line_head - 2) > max_len) ? max_len: (pos-line_head-2);
            memcpy(value, line_head+1, value_len);    
        }
        else {
            value_len = ((pos - line_head) > max_len) ? max_len: (pos-line_head);
            memcpy(value, line_head, value_len);    
        }
        value[value_len] = '\0';
        goto END;
    }

    /* 二到最后字段 */
    for (pos = line_head, value_head = line_head; ; ++pos) {
        if ((*pos == '|' && *(pos-1) != '\\') || *pos == '\n') {
            ++tmp_col;
            if (tmp_col == col) {
                if (with_quote == 1) {
                    value_len = ((pos-value_head-2)>max_len) ? max_len: (pos-value_head-2);
                    memcpy(value, value_head+1, value_len);
                }
                else {
                    value_len = ((pos-value_head)>max_len) ? max_len: (pos-value_head);
                    memcpy(value, value_head, value_len);
                }
                value[value_len] = '\0';
                goto END;
            }
            value_head = pos+1;
            if (*pos == '\n') {
                break;
            }
        }
    }

    /* col 超出字段范围 */
    fprintf(stderr, "expend colum !\n");
    value = NULL;

END:
    return value;
}

/* -------------------------------------------*/
/**
 * @brief   将文件移到错误目录下 
 *
 * @param file_dir      文件当前所在目录
 * @param wrong_dir     错误目录
 * @param file_name     文件名称
 */
/* -------------------------------------------*/
void move_file_to_wrong_path(char *file_dir, char *wrong_dir, char *file_name)
{
    char wrong_path[ABSOLUT_FILE_PATH_SIZE];
    char wrong_tmp_path[ABSOLUT_FILE_PATH_SIZE];

    memset(wrong_path, 0 , ABSOLUT_FILE_PATH_SIZE);
    memset(wrong_tmp_path, 0, ABSOLUT_FILE_PATH_SIZE);

    strncat(wrong_path, wrong_dir, ABSOLUT_FILE_PATH_SIZE);
    strncat(wrong_tmp_path, wrong_dir, ABSOLUT_FILE_PATH_SIZE);
    
    strncat(wrong_tmp_path, file_name, FILE_NAME_SIZE);  
    strcat(wrong_tmp_path, ".tmp"); 
    strncat(wrong_path, file_name, FILE_NAME_SIZE);
    rename(file_dir, wrong_tmp_path);
    rename(wrong_tmp_path, wrong_path);
    printf("move [%s] -> [%s].\n", wrong_tmp_path, wrong_path);
    CA_LOG(LOG_MODULE, LOG_PROC, "move [%s] -> [%s].\n", file_dir, wrong_path);
}

