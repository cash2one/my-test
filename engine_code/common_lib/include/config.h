#ifndef _CONFIG_H
#define _CONFIG_H

#include <sys/types.h>
#include <stdio.h>

typedef enum {
	CFG_TYPE_NULL = 0,
	CFG_TYPE_INT8,		// num, int8_t
	CFG_TYPE_UINT8,		// num, u_int8_t
	CFG_TYPE_INT16,		// num, int16_t
	CFG_TYPE_UINT16,	// num, u_int16_t
	CFG_TYPE_INT32,		// num, int32_t
	CFG_TYPE_UINT32,	// num, u_int32_t
	CFG_TYPE_LONG,		// num, signed long
	CFG_TYPE_ULONG,		// num, unsigned long
	CFG_TYPE_INT64,		// num, int64_t
	CFG_TYPE_UINT64,	// num, u_int64_t
	CFG_TYPE_IPV4,		// ip string, such as 192.168.0.1, return int
	CFG_TYPE_STR,		// string

	CFG_TYPE_YN,
	CFG_TYPE_YN_UINT8=CFG_TYPE_YN,// bool type, means yes or no, return unsigned char
	CFG_TYPE_YN_INT32,	// bool type, means yes or no, return int

	CFG_TYPE_INT32_16,	// num, int32_t, 以16进制读取
	CFG_TYPE_UINT32_16,	// num, u_int32_t, 以16进制读取
	CFG_TYPE_LONG_16,	// num, long
	CFG_TYPE_ULONG_16	// num, u_long
} CFG_TYPE_ENUM;

#define CFG_SIZE_INT8		sizeof(char)
#define CFG_SIZE_UINT8		sizeof(unsigned char)
#define CFG_SIZE_INT16		sizeof(short)
#define CFG_SIZE_UINT16		sizeof(unsigned short)
#define CFG_SIZE_INT32		sizeof(int)
#define CFG_SIZE_UINT32		sizeof(unsigned int)
#define CFG_SIZE_INT64		sizeof(long long)
#define CFG_SIZE_UINT64		sizeof(unsigned long long)

#define CFG_SIZE_LONG		sizeof(long)
#define CFG_SIZE_ULONG		sizeof(unsigned long)

#define CFG_SIZE_IPV4		sizeof(unsigned int)

#define CFG_SIZE_YN_UINT8	sizeof(unsigned char)
#define CFG_SIZE_YN		CFG_SIZE_YN_UINT8

#define CFG_SIZE_YN_INT32	sizeof(int)

typedef struct config_description {
	void	 	*entry_title;	//[MAX_TITLE_LENGTH]
	void		*key_name;	//[MAX_VAR_NAME_LENGTH]

	void		*addr;		// value, can be everything
	size_t		maxlen;		// max length of value
	int		type;		// value_type, may be LONG, INT etc
	int 		compulsive;	// the key must be here
	int		valid;		// whether the key is use now
	void 		*initial_value;	//[MAX_VAR_VALUE_LENGTH];
} cfg_desc;
#ifdef __cplusplus
extern "C" {
#endif

/*
 * ret:
 * 	0: successful
 * 	-1: normal err
 * 	-2: serious err, mean "file is not found"
 */
#define FILE_NOT_FOUND -2
int common_read_conf(char *filename, cfg_desc *desc_table);

FILE *open_config_file(char *filename);
void close_config_file(FILE **conf_file);

/*
 * end of file or error: END_OF_CONF(-1);
 * err when read_file: CFG_FILL_ERR(1)
 * normal: NORMAL_RET(0)
 */
#define END_OF_CONF	-1
#define NORMAL_RET	0
#define CFG_FILL_ERR	1
int cfg_fill(FILE *conf_file, cfg_desc *desc_table);

void cfg_print(cfg_desc *desc_table);

/*****************************************
  interface for write configuration
 *****************************************/
FILE *cfg_file_open(char *filename, char *mode);
void cfg_file_close(FILE **conf_file);

int cfg_write(FILE *conf_file, cfg_desc *desc_table);
int cfg_rewrite(char *in_filename, char *out_filename, cfg_desc *desc_table);
int create_cfg_file(char *filename, cfg_desc *desc_table);

/* desc_table中的值（compulsive=1）在conf_file中进行查找，找到则返回0, 并填写其余字段 */
/* 可以指定title，但title必须相同 */
/* 为了提高性能，最好把compulsive=1的key放在最前面 */
/* return: 
   	0: 未找到
	1: 找到
	-1: 失败
*/
typedef int (*find_action_t)(void *arg, cfg_desc *desc_table);
int cfg_find(FILE *conf_file, cfg_desc *desc_table, 
			find_action_t action, void *arg);

/*****************************************
  interface for modify_config.c
 *****************************************/
int read_conf(char *filename, 
		char *title, char *config_name, 
		char *value, int max_valuelen);

int search_conf(char *filename, 
		char *title, char *config_name, 
		char *value);

int write_conf(char *filename, char *title, 
		char *config_name, char *match_value, 
		char *set_value);

int del_conf(char *filename, char *title, char *config_name, 
		char *match_value);

#define DIR_BEFORE	0
#define DIR_AFTER	1
int add_file_cont(char *filename, char *match_str, char *cont, int pos);

int del_conf_str(char *filename, char *match_str);

static inline void set_config_addr(cfg_desc *desc_table, int idx, void *addr)
{
	desc_table[idx].addr = addr;
}

static inline void *get_config_addr(cfg_desc *desc_table, int idx)
{
	return (desc_table[idx].addr);
}

#ifdef __cplusplus
}
#endif

#endif // _CONFIG_H


