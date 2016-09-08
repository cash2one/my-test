// config.c: Alex, 2003.03.11, read config info from file to memory variables
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rule_table_n.h"
#include "config.h"
#include "misc.h"
#include "lock_file.h"

//#define DEBUG

//#define DEBUG_CONF

#ifdef DEBUG_CONF
#define dump_conf(...) \
	do {\
		printf("(%s:%u):", __FUNCTION__, __LINE__);\
		printf(__VA_ARGS__);\
	} while(0)
#else
#define dump_conf(...) \
	do {\
	} while(0)
#endif

#ifdef DEBUG_CONF
static void cfg_print_desc(cfg_desc *desc)
{
	if (desc) {
		printf("CFG_DESC: entry_title(%s), key_name(%s), maxlen(%u), "
			"type(%u), compulsive(%u), valid(%u), "
			"initial_value(%s)\n",
			(char *)desc->entry_title, (char *)desc->key_name, desc->maxlen, 
			desc->type, desc->compulsive, desc->valid, 
			(char *)desc->initial_value);
	}

	return;
}
#endif

/*
 * function definition
 */
static int cfg_conv_value(char *val_str, void *addr, 
			size_t maxlen, int type, int error_set_value);
static int cfg_init_value(cfg_desc *desc_table);
static int __cfg_fill(rule_entry *rentry_list, cfg_desc *desc_table);

#ifndef CFG_SET_VALUE_NUM
#define CFG_SET_VALUE_NUM(TYPE, SIZE, FUNC, BASE) \
{ \
	if (val_str_0 == '\0') { \
		if (error_set_value) { \
			*(TYPE *)addr = 0; \
		} \
		break; \
	} \
\
	if (maxlen < SIZE) { \
		dump_conf("len(%u) is too short, default size is %u\n", \
				maxlen, (unsigned int)SIZE); \
		if (error_set_value) { \
			*(TYPE *)addr = 0; \
		} \
		goto err; \
	} \
\
	errno = 0; \
	TYPE set_value = (TYPE)(FUNC((const char *)val_str, NULL, BASE)); \
	if ((set_value == 0) && (!isdigit(val_str_0))) { \
		dump_conf("the number string is invalid, use the default value.string: %s\n", val_str); \
		if (error_set_value) { \
			*(TYPE *)addr = 0; \
		} \
	} else if (errno == ERANGE) { \
		dump_conf("the number string is overflow, use the default value.string: %s\n", val_str); \
		if (error_set_value) { \
			*(TYPE *)addr = 0; \
		} \
	} else { \
		*(TYPE *)addr = set_value; \
		dump_conf("the number is %#llx\n", (unsigned long long)set_value); \
	} \
}
#endif

/*
 * function realize
 * error_set_value: show whether to set value when error
 */
static int cfg_conv_value(char *val_str, void *addr, 
				size_t maxlen, int type, 
				int error_set_value)
{
	if (unlikely(val_str == NULL 
			|| addr == NULL 
			|| maxlen == 0)) {
		goto err;
	}

	char val_str_0 = val_str[0]; 
	switch (type) {
	case CFG_TYPE_INT8:
		CFG_SET_VALUE_NUM(char, CFG_SIZE_INT8, strtol, 0);
		break;

	case CFG_TYPE_UINT8:
		CFG_SET_VALUE_NUM(unsigned char, CFG_SIZE_UINT8, strtoul, 0);
		break;

	case CFG_TYPE_INT16:
		CFG_SET_VALUE_NUM(short, CFG_SIZE_INT16, strtol, 0);
		break;

	case CFG_TYPE_UINT16:
		CFG_SET_VALUE_NUM(unsigned short, CFG_SIZE_UINT16, strtoul, 0);
		break;

	case CFG_TYPE_INT32:
		CFG_SET_VALUE_NUM(int, CFG_SIZE_INT32, strtol, 0);
		break;

	case CFG_TYPE_UINT32:
		CFG_SET_VALUE_NUM(unsigned int, CFG_SIZE_UINT32, strtoul, 0);
		break;

	case CFG_TYPE_INT32_16:
		CFG_SET_VALUE_NUM(int, CFG_SIZE_INT32, strtol, 16);
		break;

	case CFG_TYPE_UINT32_16:
		CFG_SET_VALUE_NUM(unsigned int, CFG_SIZE_UINT32, strtoul, 16);
		break;

	case CFG_TYPE_LONG:
		CFG_SET_VALUE_NUM(long, CFG_SIZE_LONG, strtol, 0);
		break;

	case CFG_TYPE_ULONG:
		CFG_SET_VALUE_NUM(unsigned long, CFG_SIZE_ULONG, strtoul, 0);
		break;

	case CFG_TYPE_LONG_16:
		CFG_SET_VALUE_NUM(long, CFG_SIZE_LONG, strtol, 16);
		break;

	case CFG_TYPE_ULONG_16:
		CFG_SET_VALUE_NUM(unsigned long, CFG_SIZE_ULONG, strtoul, 16);
		break;

	case CFG_TYPE_STR:
		if (val_str_0 == '\0') {
			if (error_set_value) {
				dump_conf("set null string\n");
				((char *)addr)[0] = '\0';
			}
		} else {
			ca_strcpy(addr, maxlen, val_str);
		}
		break;

	case CFG_TYPE_YN_UINT8:
		if (val_str_0 == '\0') {
			if (error_set_value) {
				*(unsigned char *)addr = 0;
			}
			break;
		}

		if (maxlen < CFG_SIZE_YN_UINT8) {
			dump_conf("len(%u) is too short, default size is %u\n", \
					maxlen, (unsigned int)CFG_SIZE_YN_INT8);
			if (error_set_value) {
				*(unsigned char *)addr = 0;
			}
			goto err;
		}

		if (!strncasecmp(val_str, "yes", 3)) {
			*(unsigned char *)addr = 1;
		} else if (!strncasecmp(val_str, "no", 2)) {
			*(unsigned char *)addr = 0;
		} else {
			dump_conf("the type of yesno string is wrong, string is %s\n", val_str);
			if (error_set_value) {
				*(unsigned char *)addr = 0;
			}
		}
		break;

	case CFG_TYPE_YN_INT32:
		if (val_str_0 == '\0') {
			if (error_set_value) {
				*(int *)addr = 0;
			}
			break;
		}

		if (maxlen < CFG_SIZE_YN_INT32) {
			dump_conf("len(%u) is too short, default size is %u\n", \
					maxlen, (unsigned int)CFG_SIZE_YN_INT32);
			if (error_set_value) {
				*(int *)addr = 0;
			}
			goto err;
		}

		if (!strncasecmp(val_str, "yes", 3)) {
			*(int *)addr = 1;
		} else if (!strncasecmp(val_str, "no", 2)) {
			*(int *)addr = 0;
		} else {
			dump_conf("the type of yesno string is wrong, string is %s\n", val_str);
			if (error_set_value) {
				*(int *)addr = 0;
			}
		}
		break;

	case CFG_TYPE_INT64:
		CFG_SET_VALUE_NUM(long long, CFG_SIZE_INT64, strtoull, 0);
		break;

	case CFG_TYPE_UINT64:
		CFG_SET_VALUE_NUM(unsigned long long, CFG_SIZE_UINT64, strtoull, 0);
		break;

	case CFG_TYPE_IPV4:
		if (val_str_0 == '\0') {
			if (error_set_value) {
				*(unsigned int *)addr = 0;
			}
			break;
		}

		if (maxlen < CFG_SIZE_IPV4) {
			dump_conf("len(%u) is too short, default size is %u\n",
					maxlen, (unsigned int)CFG_SIZE_IPV4);
			if (error_set_value) {
				*(unsigned int *)addr = 0;
			}
			goto err;
		}

		struct in_addr ip_addr;
		if (inet_aton(val_str, &ip_addr) == 0) {
			dump_conf("error when convert str to ip, string: %s\n", val_str);
			if (error_set_value) {
				*(unsigned int *)addr = 0;
			}
		} else {
			*(unsigned int *)addr = ip_addr.s_addr;
		}
		break;

	default:
		goto err;
	}

	return 0;

err:
	return -1;
}

static int cfg_init_value(cfg_desc *desc_table)
{
	int nret = 0;

	if (unlikely(!desc_table)) {
		goto err;
	}

	cfg_desc *desc_point = desc_table;
	while (((char *)(desc_point->key_name))[0] != '\0') {
		if (desc_point->addr != NULL 
			&& cfg_conv_value(desc_point->initial_value, 
				desc_point->addr, 
				desc_point->maxlen, 
				desc_point->type,
				1) < 0) {
			dump_conf("Error: convert value error\n");
#ifdef DEBUG_CONF
			cfg_print_desc(desc_point);
#endif
		}

		desc_point++;
	}

	return nret;
err:
	return -1;
}

/*
 * _cfg_fill
 * 两种使用方式:
 * 1.如果rentry_list是一个rule_entry的链表指针，desc_table中cfg_desc各项
 * 的entry_title字段必须有值，以使得可以在链表中搜索有该title的rule_entry的出现
 * 2.如果rentry_list只是一个rule_entry的指针，那么desc_table中的cfg_desc各项
 * 中entry_list字段必须为空，表示处理的只是当前指向的rule_entry结构。
 * 这两种方式可以分别应用于配置文件的读取和事件自定义文件的读取中
 */
/* ret:
 * 	suc: return the successfully filled var num
 * 	failed: return -1
 */
static int __cfg_fill(rule_entry *rentry_list, cfg_desc *desc_table)
{
	rule_entry *entry = NULL;
	rule_var *var = NULL;
	cfg_desc *desc_point = NULL;
	
	int valid_num = 0;

	desc_point = desc_table;
	while (((char *)desc_point->key_name)[0] != '\0') {
		// 如果entry_title为空，表示将仅在rentry_list指向的rule_entry结构里查找
		if (((char *)(desc_point->entry_title))[0] != '\0') {
			entry = search_nocase_rule_entry_list(&rentry_list, 
					desc_point->entry_title, strlen(desc_point->entry_title));
			if (!entry) {
				desc_point->valid = 0;
				goto nextwhile;
			}
		} else {
			entry = rentry_list;
		}

		var = search_var_nocase_in_rule_entry(entry, desc_point->key_name);
		if (!var) {
			desc_point->valid = 0;
			goto nextwhile;
		}

		if (cfg_conv_value(var->rv_valuep, desc_point->addr, desc_point->maxlen, 
				desc_point->type, 0) < 0) {

			desc_point->valid = 0;

			if (desc_point->compulsive) {
				dump_conf("Error: 'compulsive' cfg_desc haven't been right value\n");
#ifdef DEBUG_CONF
				cfg_print_desc(desc_point);
#endif
				goto err;
			} else {
				dump_conf("Warning: convert value error\n");
#ifdef DEBUG_CONF
				cfg_print_desc(desc_point);
#endif
				goto nextwhile;
			}
		}

		desc_point->valid = 1;
		valid_num++;

nextwhile:
		desc_point++;
	}

	return valid_num;
err:
	return -1;
}

static inline int varify_value(void *value, size_t value_len, char *data, int type)
{
	switch (type) {
		case CFG_TYPE_INT8:
			if (strtol(data, NULL, 0) == *(char *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_UINT8:
			if (strtoul(data, NULL, 0) == *(unsigned char *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_INT16:
			if (strtol(data, NULL, 0) == *(short *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_UINT16:
			if (strtoul(data, NULL, 0) == *(unsigned short *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_INT32:
			if (strtol(data, NULL, 0) == *(int *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_UINT32:
			if (strtoul(data, NULL, 0) == *(unsigned int *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_INT32_16:
			if (strtol(data, NULL, 16) == *(int *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_UINT32_16:
			if (strtoul(data, NULL, 16) == *(unsigned int *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_LONG:
			if (strtol(data, NULL, 0) == *(long *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_ULONG:
			if (strtoul(data, NULL, 0) == *(unsigned long *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_LONG_16:
			if (strtol(data, NULL, 16) == *(long *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_ULONG_16:
			if (strtoul(data, NULL, 16) == *(unsigned long *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_INT64:
			if (strtol(data, NULL, 0) == *(long long *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_UINT64:
			if (strtoul(data, NULL, 0) == *(unsigned long long *)value) {
				goto suc;
			}
			break;
		case CFG_TYPE_IPV4:
			{
				struct in_addr ip_addr;
				if (inet_aton((const char *)data, &ip_addr) != 0
					&& *(unsigned int *)value == ip_addr.s_addr) {
					goto suc;
				}
			}
			break;
		case CFG_TYPE_STR:
			if (!strcasecmp(data, (char *)value)) {
				goto suc;
			}
			break;
		case CFG_TYPE_YN_UINT8:
			if (!strncasecmp(data, "yes", 3)
				&& *(unsigned char *)value == 1) {
				goto suc;
			} else if (!strncasecmp(data, "no", 2)
				&& *(unsigned char *)value == 0) {
				goto suc;
			}
			break;
		case CFG_TYPE_YN_INT32:
			if (!strncasecmp(data, "yes", 3)
				&& *(int *)value == 1) {
				goto suc;
			} else if (!strncasecmp(data, "no", 2)
				&& *(int *)value == 0) {
				goto suc;
			}
			break;
		default:
			break;
	}

	return -1;

suc:
	return 0;
}

static inline int __cfg_find(rule_entry *entry, cfg_desc *desc_table)
{
	rule_var *var = NULL;
	cfg_desc *desc_point = desc_table;

	while (((char *)desc_point->key_name)[0] != '\0') {
		if (((char *)(desc_point->entry_title))[0] != '\0'
			&& strncasecmp(entry->re_title, 
				(char *)(desc_point->entry_title), 
				strlen((char *)(desc_point->entry_title)))) {
			goto err;
		}

		if (desc_point->compulsive == 1) {
			if (unlikely(desc_point->addr == NULL)) {
				goto err;
			}

			dump_conf("key_name: %s, addr: %s\n", (char *)desc_point->key_name, (char *)desc_point->addr);
			var = search_var_nocase_in_rule_entry(entry, desc_point->key_name);
			if (!var) {
				goto err;
			}

			if (varify_value(desc_point->addr, desc_point->maxlen, 
					var->rv_valuep, desc_point->type) < 0) {
				goto err;
			}
		}

		desc_point++;
	}

	desc_point = desc_table;
	while (((char *)desc_point->key_name)[0] != '\0') {
		if (desc_point->compulsive == 0 && desc_point->addr != NULL) {
			dump_conf("fill key_name: %s\n", (char *)desc_point->key_name);
			var = search_var_nocase_in_rule_entry(entry, desc_point->key_name);
			if (var) {
				cfg_conv_value(var->rv_valuep, desc_point->addr, desc_point->maxlen, 
						desc_point->type, 0);
			}
		}

		desc_point++;
	}

	return 0;

err:
	return -1;
}

static inline int __cfg_find_init(cfg_desc *desc_table)
{
	cfg_desc *desc_point = desc_table;

	while (((char *)(desc_point->key_name))[0] != '\0') {
		if ((desc_point->compulsive == 0) 
			&& desc_point->addr != NULL
			&& (cfg_conv_value(desc_point->initial_value, 
					desc_point->addr, 
					desc_point->maxlen, 
					desc_point->type,
					1) < 0)) {
			dump_conf("Error: convert value error, key_name: %s\n", (char *)desc_point->key_name);
		}

		desc_point++;
	}

	return 0;
}

/**************************
 * outer interface define
 *************************/
/*
 * ret:
 * 	>=0: successful, return rule entry number
 * 	-1: normal err
 * 	-2: serious err, mean "file is not found"
 */
int common_read_conf(char *filename, cfg_desc *desc_table)
{
	FILE *fp = NULL;
	rule_entry *rep = NULL;
	rule_entry *rlist = NULL;
	int read_ret = 0;

	if (unlikely(desc_table == NULL 
			|| (filename == NULL) 
			|| (filename[0] == '\0'))) {
		goto serious_err;
	}

	fp = fopen_lock((const char *)filename, "r");
	cfg_init_value(desc_table);
	if (fp == NULL) {
		dump_conf("open file(%s) failed: %s\n", filename, strerror(errno));
		goto serious_err;
	}

	int rule_num = 0;
	while (1) {
		rep = new_rule_entry();
		if (rep == NULL) {
			goto err;
		}

		read_ret = read_rule_entry(fp, rep);
		if (read_ret > 0) {
			rule_num++;
			dump_conf("get one node from %s\n", filename);
			add_to_rule_entry_list(&rlist, rep);
		} else {
			dump_conf("Error: parse file(%s) error\n", filename);
			free_rule_entry(rep);
			rep = NULL;
			break;
		}
	}

	fclose_unlock(fp);
	fp = NULL;

	if (rule_num == 0) {
		dump_conf("cannot read anything from configuration file\n");
		goto ret;
	}

#ifdef DEBUG_CONF
	dump_rule_entry_list(rlist);
#endif

	if (__cfg_fill(rlist, desc_table) < 0) {
		dump_conf("Error: configuration error\n");
		goto err;
	}

	destroy_rule_entry_list(&rlist);

ret:
	return rule_num;

err:
	if (rep) {
		free_rule_entry(rep);
		rep = NULL;
	}

	if (fp) {
		fclose_unlock(fp);
		fp = NULL;
	}

	if (rlist) {
		destroy_rule_entry_list(&rlist);
	}

	return -1;

serious_err:
	return FILE_NOT_FOUND;
}

FILE *open_config_file(char *filename)
{
	if (unlikely(filename == NULL || filename[0] == '\0')) {
		dump_conf("null filename\n");
		return NULL;
	}

	FILE *config_file = fopen_lock((const char *)filename, "r");
	if (config_file == NULL) {
		dump_conf("open file(%s) failed: %s\n", filename, strerror(errno));
	}

	return config_file;
}

void close_config_file(FILE **conf_file)
{
	if (conf_file && *conf_file) {
		fclose_unlock(*conf_file);
		*conf_file = NULL;
	}

	return;
}

FILE *cfg_file_open(char *filename, char *mode)
{
	if (unlikely(filename == NULL || filename[0] == '\0')) {
		dump_conf("null filename\n");
		return NULL;
	}

	FILE *config_file = fopen_lock((const char *)filename, (const char *)mode);
	if (config_file == NULL) {
		dump_conf("open file(%s) for \"%s\" failed: %s\n", filename, mode, strerror(errno));
	}

	return config_file;
}

void cfg_file_close(FILE **conf_file)
{
	if (conf_file && *conf_file) {
		fclose_unlock(*conf_file);
		*conf_file = NULL;
	}

	return;
}

/*
 * end of file or error: END_OF_CONF(-1);
 * normal: NORMAL_RET(0)
 * err when cfg_fill: CFG_FILL_ERR(1)
 */
int cfg_fill(FILE *conf_file, cfg_desc *desc_table)
{
	rule_entry rule_entry_node;

	cfg_init_value(desc_table);
	if (read_rule_entry(conf_file, &rule_entry_node) <= 0) {
		return END_OF_CONF;
	}

	if (__cfg_fill(&rule_entry_node, desc_table) < 0) {
		dump_conf("error when cfg_fill\n");

		zero_rule_entry(&rule_entry_node);
		return CFG_FILL_ERR;
	}

	zero_rule_entry(&rule_entry_node);

	return NORMAL_RET;
}

void cfg_print(cfg_desc *desc_table)
{
	int index = 0;

	printf("%16s\t%16s\t%5s\t%s\n", "TITLE", "KEYNAME", "VALID", "VALUE");
	while (1) {
		if (((char *)(desc_table[index].key_name))[0] == '\0')
			break;

		switch (desc_table[index].type) {
			case CFG_TYPE_INT8:
				printf("%16s\t%16s\t%5d\t%#x\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(char *) desc_table[index].addr);
				break;
			case CFG_TYPE_UINT8:
				printf("%16s\t%16s\t%5d\t%#x\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(unsigned char *) desc_table[index].addr);
				break;
			case CFG_TYPE_INT16:
				printf("%16s\t%16s\t%5d\t%#x\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(short *) desc_table[index].addr);
				break;
			case CFG_TYPE_UINT16:
				printf("%16s\t%16s\t%5d\t%#x\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(unsigned short *) desc_table[index].addr);
				break;
			case CFG_TYPE_INT32:
			case CFG_TYPE_INT32_16:
				printf("%16s\t%16s\t%5d\t%#x\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(int *) desc_table[index].addr);
				break;
			case CFG_TYPE_UINT32:
			case CFG_TYPE_UINT32_16:
				printf("%16s\t%16s\t%5d\t%#x\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(unsigned int *) desc_table[index].addr);
				break;
			case CFG_TYPE_LONG:
			case CFG_TYPE_LONG_16:
				printf("%16s\t%16s\t%5d\t%#lx\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(long *) desc_table[index].addr);
				break;
			case CFG_TYPE_ULONG_16:
				printf("%16s\t%16s\t%5d\t%#lx\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(unsigned long *) desc_table[index].addr);
				break;
			case CFG_TYPE_INT64:
				printf("%16s\t%16s\t%5d\t%#llx\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(long long *) desc_table[index].addr);
				break;
			case CFG_TYPE_UINT64:
				printf("%16s\t%16s\t%5d\t%#llx\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(unsigned long long *) desc_table[index].addr);
				break;
			case CFG_TYPE_IPV4:
				printf("%16s\t%16s\t%5d\t%x:%s\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						*(unsigned int *) desc_table[index].addr,
						inet_ntoa(*((struct in_addr *)desc_table[index].addr)));
				break;
			case CFG_TYPE_STR:
				printf("%16s\t%16s\t%5d\t%s\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						(char *)desc_table[index].addr);
				break;
			case CFG_TYPE_YN_UINT8:
				printf("%16s\t%16s\t%5d\t%s\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						(*(unsigned char *) desc_table[index].addr ? "yes" : "no"));
				break;
			case CFG_TYPE_YN_INT32:
				printf("%16s\t%16s\t%5d\t%s\n", 
						(char *)desc_table[index].entry_title, 
						(char *)desc_table[index].key_name,
						desc_table[index].valid, 
						(*(int *) desc_table[index].addr ? "yes" : "no"));
				break;
			default:
				break;
		}

		index++;
	}

	return;
}

int cfg_write(FILE *conf_file, cfg_desc *desc_table)
{
	fseek(conf_file, 0, SEEK_END);

	cfg_desc *desc_point = desc_table;
//	char *last_title = NULL;

	int valid_node = 0;

	if (((char *)desc_point->key_name)[0] != '\0'
			&& desc_point->compulsive != 1) {
		fprintf(conf_file, "[%s]\n", (char *)desc_point->entry_title);
	}

	while (((char *)desc_point->key_name)[0] != '\0') {
//		if ((char *)(desc_point->entry_title)[0] == '\0' 
//				|| last_title == NULL 
//				|| strcasecmp(desc_point->entry_title, last_title)) {
//			fprintf(conf_file, "[%s]\n", (char *)desc_point->entry_title);
//			last_title = desc_point->entry_title;
//		}

		if (desc_point->compulsive == 1) {
			fprintf(conf_file, "[%s]\n", (char *)desc_point->entry_title);
		}

		if (unlikely(desc_point->addr == NULL)) {
			desc_point++;
			continue;
		}
#if 0
#define WRITE_CONFIG_NAME(conf_file, name) 				\
	fprintf(conf_file, "%s%s= ", 					\
			(char *)(name), 				\
			(strlen((char *)name) < 8 ? "\t\t" : "\t")	\
		);
#endif 
#define WRITE_CONFIG_NAME(conf_file, name) 				\
	fprintf(conf_file, "%s%s= ", 					\
			(char *)(name), 				\
			(strlen((const char *)name) < 8 ? "\t\t" : "\t")	\
		);

	switch (desc_point->type) {
			case CFG_TYPE_INT8:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#x\n", *(char *)(desc_point->addr));
				break;
			case CFG_TYPE_UINT8:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#x\n", *(unsigned char *)(desc_point->addr));
				break;
			case CFG_TYPE_INT16:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#x\n", *(short *)(desc_point->addr));
				break;
			case CFG_TYPE_UINT16:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#x\n", *(unsigned short *)(desc_point->addr));
				break;
			case CFG_TYPE_INT32:
			case CFG_TYPE_INT32_16:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#x\n", *(int *)(desc_point->addr));
				break;
			case CFG_TYPE_UINT32:
			case CFG_TYPE_UINT32_16:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#x\n", *(unsigned int *)(desc_point->addr));
				break;
			case CFG_TYPE_LONG:
			case CFG_TYPE_LONG_16:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#lx\n", *(long *)(desc_point->addr));
				break;
			case CFG_TYPE_ULONG:
			case CFG_TYPE_ULONG_16:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#lx\n", *(unsigned long *)(desc_point->addr));
				break;
			case CFG_TYPE_STR:
				if (unlikely(((char *)(desc_point->addr))[0] == '\0')) {
					break;
				}
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%s\n", (char *)desc_point->addr);
				break;
			case CFG_TYPE_YN_UINT8:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%s\n",(*(unsigned char *)desc_point->addr ? "yes" : "no"));
				break;
			case CFG_TYPE_YN_INT32:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%s\n",(*(int *)desc_point->addr ? "yes" : "no"));
				break;
			case CFG_TYPE_INT64:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#llx\n", *(long long *)(desc_point->addr));
				break;
			case CFG_TYPE_UINT64:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%#llx\n", *(unsigned long long *)(desc_point->addr));
				break;
			case CFG_TYPE_IPV4:
				WRITE_CONFIG_NAME(conf_file, desc_point->key_name);
				fprintf(conf_file, "%s\n", inet_ntoa(*((struct in_addr *)desc_point->addr)));
				break;
			default:
				break;
		}

		valid_node = 1;
		desc_point++;
	}

	if (valid_node == 1) {
		fprintf(conf_file, "\n");
	}

	return 0;
}

int cfg_rewrite(char *in_filename, char *out_filename, cfg_desc *desc_table)
{
	FILE *in_file = fopen_lock((const char *)in_filename, "r");
	if (in_file == NULL) {
		dump_conf("open file(%s) failed: %s\n", in_filename, strerror(errno));
		return -1;
	}

	FILE *out_file = fopen_lock((const char *)out_filename, "a");
	if (out_file == NULL) {
		dump_conf("open file(%s) for writing failed: %s\n", out_filename, strerror(errno));

		fclose(in_file);
		return -1;
	}

	int cfg_fill_ret = 0;
	while ((cfg_fill_ret = cfg_fill(in_file, desc_table)) >= 0) {
		if (cfg_fill_ret > 0) {
			dump_conf("cfg fill error\n");
			continue;
		}

		cfg_write(out_file, desc_table);
	}

	fclose_unlock(in_file);
	in_file = NULL;
	fclose_unlock(out_file);
	out_file = NULL;

	return 0;
}

/* desc_table中的值（compulsive=1）在conf_file中进行查找，找到则返回0, 并填写其余字段 */
/* 可以指定title，但title必须相同 */
/* 为了提高性能，最好把compulsive=1的key放在最前面 */
/* return: 
   	0: 未找到
	1: 找到
	-1: 失败
*/
int cfg_find(FILE *conf_file, cfg_desc *desc_table, find_action_t action, void *arg)
{
	if (unlikely(!conf_file || !desc_table)) {
		goto err;
	}

	rule_entry rule_entry_node;
	rule_entry *p_entry = &rule_entry_node;
	int find_ret = 0;

	while (read_rule_entry(conf_file, p_entry) > 0) {
		__cfg_find_init(desc_table);

		if (__cfg_find(p_entry, desc_table) == 0) {
			dump_conf("find desc_table\n");
			find_ret = 1;
			if (!action || action(arg, desc_table) < 0) {
				break;
			}
		}

		zero_rule_entry(p_entry);
	}

	return find_ret;

err:
	return -1;
}

/* -------------------------------------------*/
/**
 * @brief  Creat a config file 
 *
 * @param filename
 * @param desc_table
 *
 * @returns   
 */
/* -------------------------------------------*/
int create_cfg_file(char *filename, cfg_desc *desc_table)
{
    int retn = 0;
    FILE * fp = NULL;

    fp = cfg_file_open(filename, "a+");
    if (fp == NULL) {
		dump_conf("open file(%s) failed!\n", filename);
        retn = -1;
        goto END;
    }

    cfg_write(fp, desc_table);

    cfg_file_close(&fp);
END:
    return retn;
}
