#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "var_func.h"

#define MAX_FUNC_PARA_VAL_NUM	7
#define MAX_PARA_NAME_LEN	8

#define MODULE_ORDER	0
#define MODULE_SYS	1
#define MODULE_ALIGN	2
#define MODULE_STRING	3

#define BIG_END		0
#define LITTLE_END	1

#define DEC_SYS		0
#define HEX_SYS		1
#define OCT_SYS		2

#define NEED_ALIGN	1

#define STRING_TYPE	1

typedef struct _var_para {
	unsigned int mask;
	char align;
	char order;
	char system;
	char string;
} var_para;

typedef struct _var_para_key {
	int value;
	int module;
	int name_len;
	char para_name[MAX_PARA_NAME_LEN];
} var_para_key;

var_para_key var_key[MAX_FUNC_PARA_VAL_NUM] = {
	{BIG_END, MODULE_ORDER, 3, "big"}, 
	{LITTLE_END, MODULE_ORDER, 6, "little"},
	{HEX_SYS, MODULE_SYS, 3, "hex"}, 
	{DEC_SYS, MODULE_SYS, 3, "dec"},
	{OCT_SYS, MODULE_SYS, 3, "oct"}, 
	{NEED_ALIGN, MODULE_ALIGN, 5, "align"},
	{STRING_TYPE, MODULE_STRING, 6, "string"}
};

static inline int get_netorder(u_int8_t * data, int byte_num)
{
	if ((byte_num <= 0) || (byte_num > 4)) {
		fprintf(stderr, "Error: get net order byte_num=%d\n", byte_num);
		return -1;
	}

	int i = 0;
	int val = 0;

	for (i = 0; i < byte_num; i++) {
		val |= data[i] << ((byte_num - i - 1) * 8);
	}

	return val;
}

static inline int get_hostorder(u_int8_t * data, int byte_num)
{
	if ((byte_num <= 0) || (byte_num > 4)) {
		fprintf(stderr, "Error: get host order byte_num=%d\n", byte_num);
		return -1;
	}

	int i = 0;
	int val = 0;

	for (i = 0; i < byte_num; i++) {
		val |= data[i] << ((i) * 8);
	}

	return val;
}

void var_func_clean(func_var_para * func_vp)
{
	if (func_vp) {
		if (func_vp->para) {
			free(func_vp->para);
		}

		free(func_vp);
	}

	return;
}

static char *search_sense_char(char *begin, int nsearch, int step)
{
	char *p = NULL, *end = NULL;

	p = begin;
	if (step >= 0) {
		end = begin + nsearch;
		while ((p < end) 
			&& (*p == ' ' || *p == '\t' 
				|| *p == '\r' || *p == '\n')) {
			p++;
		}

		if (*p == '\0' || p == end) {
			return NULL;
		} else {
			return p;
		}
	} else {
		end = begin - nsearch;
		while ((p > end) 
			&& (*p == '\r' || *p == '\n' 
				|| *p == '\t' || *p == ' ')) {
			p--;
		}

		if (p == end) {
			return NULL;
		} else {
			return p;
		}
	}
}

static char *strim(char *strline)
{
	if (strline == NULL || strline[0] == '\0') {
		goto err;
	}

	char *pline = search_sense_char(strline, strlen(strline), 0);
	if (pline == NULL) {
		*pline = '\0';
		goto err;
	}

	char *p = search_sense_char(pline + strlen(pline) - 1, strlen(pline), -1);
	if (p != NULL) {
		*(p + 1) = '\0';
	} else {
		*pline = '\0';
		goto err;
	}

	return pline;

err:
	return NULL;
}

static var_para_key *get_var_para_byname(char *para_name)
{
	var_para_key *p = NULL;

	int i = 0;
	for (i = 0; i < MAX_FUNC_PARA_VAL_NUM; i++) {
		if (strncasecmp(para_name, var_key[i].para_name, var_key[i].name_len) == 0) {
			p = var_key + i;
			break;
		}
	}

	return p;
}

#ifdef VAR_FUNC_DEBUG
static void print_var_para(var_para *var_p)
{
	if (var_p == NULL) {
		printf("var_p is NULL\n");
		return;
	}

	printf("align(%d), mask(%d), order(%d), string(%d), system(%d)\n", 
			var_p->align, var_p->mask, var_p->order, var_p->string, var_p->system);
}
#else
	#define print_var_para(var_p)
#endif

func_var_para *var_func_init(char *para)
{
	func_var_para *func_vp = NULL;
	var_para *var_p = NULL;

	char *begin = para;

	func_vp = (func_var_para *) calloc(1, sizeof(func_var_para));
	if (!func_vp) {
		fprintf(stderr, "Error:func para calloc failed.\n");
		return NULL;
	}

	var_p = (var_para *) calloc(1, sizeof(var_para));
	if (!var_p) {
		goto err;
	}

	if (isdigit(begin[0])) {
		var_p->mask = strtoul(begin, NULL, 16);
	}

	char *t_para = strdup(para);
	if (t_para == NULL) {
		goto err;
	}

	char *start = t_para;
	char *p = NULL;
	char *end_p = NULL;

	while ((p = strtok_r(start, ",", &start)) != NULL) {
		if ((p = strim(p)) == NULL) {
			continue;
		}

		if (isdigit(p[0])) {
			var_p->mask = strtoul(begin, &end_p, 0);
			if (end_p == NULL || end_p[0] != 'm') {
				var_p->mask = 0;
				continue;
			}
		} else {
			var_para_key *key_node = get_var_para_byname(p);
			if (key_node == NULL) {
				printf("---key node is NULL, unknown key arg: %s---\n", p);
				continue;
			}

			switch (key_node->module) {
				case MODULE_ORDER:
					var_p->order = key_node->value;
					break;

				case MODULE_SYS:
					var_p->system = key_node->value;
					break;

				case MODULE_ALIGN:
					var_p->align = key_node->value;
					break;

				case MODULE_STRING:
					var_p->string = key_node->value;
					break;

				default:
					break;
			}
		}
	}

	print_var_para(var_p);

	func_vp->para_len = sizeof(var_para);
	func_vp->para = (void *) var_p;

	return func_vp;

err:

	if (func_vp)
		free(func_vp);

	if (var_p)
		free(var_p);

	return NULL;
}

#define STRING_BUF_LEN 32

int byte_get(func_proto_var * var, unsigned int offset, unsigned int depth, void *para)
{
	if (!var || !para || (var->len <= offset)) {
		fprintf(stderr, "Error: byte_get para is err.\n");
		return -1;
	}

	int val = 0;
	var_para *var_p = (var_para *) para;
	u_int8_t *addr = var->addr + offset;

	if (var_p->string) {

		char buf[STRING_BUF_LEN] = { 0 };
		int tmp_len = (depth >= (STRING_BUF_LEN - 1)) ? (STRING_BUF_LEN - 1) : depth;

		strncpy(buf, (char *) addr, tmp_len);

		switch (var_p->system) {
			case HEX_SYS:
				val = strtoul(buf, NULL, 16);
				break;

			case DEC_SYS:
				val = strtoul(buf, NULL, 10);
				break;

			case OCT_SYS:
				val = strtoul(buf, NULL, 8);
				break;

			default:
				val = -1;
				break;
		}
	} else if (depth <= 4) {
		if (var_p->order) {
			val = get_hostorder(addr, depth);
		} else {
			val = get_netorder(addr, depth);
		}
	}

	if (val < 0) {
		goto ret;
	}

	if (var_p->mask) {
		val &= var_p->mask;
	}
/**** WH  2010-06-07 ***/
	if (var_p->align) {
		val = (val <= 4) ? 4 : (4 * ((int) (val / 4) + 1));
	}
ret:
	return val;
}

int byte_jump(func_proto_var * var, unsigned int offset, unsigned int depth, void *para)
{
	if (!var || !para || (var->len <= offset)) {
		fprintf(stderr, "Error: byte_jump para is err.\n");
		return -1;
	}

	int val = 0;
	var_para *var_p = (var_para *) para;
	u_int8_t *addr = var->addr + offset;

	if (var_p->string) {

		char buf[STRING_BUF_LEN] = { 0 };
		int tmp_len = (depth >= (STRING_BUF_LEN - 1)) ? (STRING_BUF_LEN - 1) : depth;

		strncpy(buf, (char *) addr, tmp_len);

		switch (var_p->system) {
			case HEX_SYS:
				val = strtoul((char *) addr, NULL, 16);
				break;

			case DEC_SYS:
				val = strtoul((char *) addr, NULL, 10);
				break;

			case OCT_SYS:
				val = strtoul((char *) addr, NULL, 8);
				break;

			default:
				val = -1;
				break;
		}
	} else if (depth <= 4) {
		if (var_p->order) {
			val = get_hostorder(addr, depth);
		} else {
			val = get_netorder(addr, depth);
		}
	}

	if (val < 0) {
		goto ret;
	}

	if (var_p->align) {
		val = (val <= 4) ? 4 : (4 * ((int) (val / 4) + 1));
	}

ret:
	return val;
}

int isdataat(func_proto_var * var, unsigned int offset, unsigned int depth, void *para)
{
	if (!var || (var->len <= offset)) {
		fprintf(stderr, "Error: isdataat para err.\n");
		return -1;
	}

	return (var->len - offset);
}

