/*
 **
 ** Modified from HIDS rule_table.c in 02-08-05, to read INI-like config file
 ** 03-02-26 M Alex, Simplified rule_table interface
 **
 ** rule_table_n.c
 ** 概念：
 ** rule_table：规则表文件，定义了各条规则的文本文件如：Rules.Def
 ** rule_var：数据结构，用于记录规则文件中定义的"Name = Value"的行，
 ** 如："LogID = AUE_telnet"
 ** rule_title：字符数组，记录规则文件中[xxx]行中内容，
 ** 如：[\Root_login_failed\Define\]中字符串
 ** rule_entry：数据结构，用于记录多个rule_var以及一个rule_title的数据结构
 ** 2003-06-10, Alex, M : 1.COMMENT_LINE: comment at the end of line
 **			 2.DOS_FORMAT: can read dos format rule table
 ** 2003-07-13, Alex, M : Multiple free
 **
 ** 
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "rule_table_n.h"
#include "misc.h"

//#define DEBUG_RULE_ENTRY
#ifdef DEBUG_RULE_ENTRY
#define dump_re(...) \
	do {\
		printf("(%s:%u):", __FUNCTION__, __LINE__);\
		printf(__VA_ARGS__);\
		printf("\n");\
	} while(0)
#else
#define dump_re(...) \
	do {\
	} while(0)
#endif

typedef int (*var_cmp_func)(const char *s1, const char *s2);
typedef int (*title_cmp_func)(const char *s1, const char *s2, size_t len);

/************
 * rule_file
 ***********/
/* FILE *open_rule_table(char *path, char *read_write)
 * {
 * 	FILE *fp = NULL;
 * 
 * 	errno = 0;
 * 	if ((fp = fopen(path, read_write)) == NULL) {
 * 		goto err;
 * 	}
 * 
 * 	return fp;
 * 
 * err:
 * 	return fp;
 * }
 * 
 * void close_rule_table(FILE *fp)
 * {
 * 	if (fp) {
 * 		fclose(fp);
 * 	}
 * 
 * 	return;
 * }
 */

/************
 * rule_entry
 ***********/
rule_entry *new_rule_entry()
{
	rule_entry *rep = (rule_entry *)calloc(1, sizeof(rule_entry));
	if (!rep) {
		return NULL;
	}

	return rep;
}

void free_rule_entry(rule_entry *rep)
{
	if (rep) {
		int idx = 0;
		for (idx = 0; idx < rep->re_var_num; idx++) {
			if (rep->re_varlist[idx].rv_valuep) {
				free(rep->re_varlist[idx].rv_valuep);
				rep->re_varlist[idx].rv_valuep = NULL;
			}
		}

		free(rep);
	}

	return;
}

void zero_rule_entry(rule_entry *rep)
{
	if (rep) {
		int idx = 0;
		for (idx = 0; idx < rep->re_var_num; idx++) {
			if (rep->re_varlist[idx].rv_valuep) {
				free(rep->re_varlist[idx].rv_valuep);
			}
		}
		memset(rep, 0, sizeof(rule_entry));
	}

	return;
}

// parse string, fill rule_title
// sztitle: max string len of title
int read_rule_title(char *string, char *title, int sztitle)
{
	char *p = NULL;

	if (sztitle <= 0) {
		goto err;
	}

	// *string == '[', so skip it
	string++;

	p = strchr(string, ']');
	if (p) {
		ca_strncpy(title, sztitle, string, p - string);
		return 0;
	}

err:
	return -1;
}

// parse string, fill rule_var
// "name = value"
// <0: illegal
int read_rule_var(char *str, rule_entry *r_entry)
{
	char *opp = NULL, *str_end = NULL;
	char *var_begin = NULL, *var_end = NULL;
	char *value_begin = NULL, *value_end = NULL;

	rule_var *var_node = NULL;
	int totlen = 0;

	if (!r_entry || !str) {
		goto err;
	}

	if (r_entry->re_var_num >= MAX_RULE_VAR_NUM) {
		dump_re("re_var_num is too large\n");
		goto err;
	}

	str_end = str + strlen(str);
	if (str_end == str) {
		dump_re("length of str is zero\n");
		goto err;
	}

	opp = strchr(str, '=');
	if (!opp) {	// "xxx"  or "=xxx"
		dump_re("no found '=' operation\n");
		goto err;
	}

	var_begin = search_sense_char(str, (opp - str), FORWARD);
	if (!var_begin) {	//"   =xxx"
		dump_re("1: nothing before '=' \n");
		goto err;
	}

	var_end = search_sense_char(opp - 1, (opp - var_begin), BACKWARD);
#if 0
	// no need
	if (!var_end) {
		dump_re("2: nothing before '=' \n");
		goto err;
	}
#endif
	var_end++;

	value_begin = search_sense_char(opp + 1, (str_end - (opp + 1)), FORWARD);
	if (!value_begin) {
		dump_re("1: nothing after '=' \n");
		goto err;
	}

	value_end = search_sense_char(str_end - 1, (str_end - value_begin), BACKWARD);
#if 0
	// no need
	if (!value_end) {
		dump_re("2: nothing after '=' \n");
		goto err;
	}
#endif
	value_end++;

	var_node = r_entry->re_varlist + r_entry->re_var_num;

	totlen = MIN_T(int, value_end - value_begin + 1, MAX_VAR_VALUE_LENGTH);
	var_node->rv_valuep = malloc(totlen);
	if (!var_node->rv_valuep) {
		dump_re("error when malloc valuep\n");
		goto serious_err;
	}

	// After so many checks above, 
	// maybe the string are legal, Woo~~
	ca_strncpy(var_node->rv_name, sizeof(var_node->rv_name), 
			var_begin, var_end - var_begin);

	var_node->rv_length = ca_strncpy(var_node->rv_valuep, totlen, 
			value_begin, value_end - value_begin);
	dump_re("var_node->rv_value: (%d)%s\n", var_node->rv_length, var_node->rv_valuep);
	dump_re("var_node->rv_name: ==%s==\n", var_node->rv_name);

	r_entry->re_var_num++;

	return 0;

err:
	dump_re("DEBUG: illegal line:%s\n", str);
	return -1;

serious_err:
	dump_re("DEBUG: illegal line:%s\n", str);
	return -2;
}

/*
 * read one rule_entry from rule file
 * -1: error
 * 0 : end of file
 * 1 : get one rule_entry
 */
int read_rule_entry(FILE *fp, rule_entry *rep)
{
	char linebuf[RULE_TABLE_LINE_LENGTH] = "";
	char *pget = NULL;
	int pget_len = 0;

	int end_of_config = 1;

	char *pc = NULL;
	char *pc_end = NULL;

	if ((fp == NULL) || (rep == NULL)) {
		goto err;
	}

	memset(rep, 0, sizeof(rule_entry));

	while ((pget = fgets(linebuf, sizeof(linebuf), fp)) != NULL) {
		dump_re("pget: %s", pget);

		pget_len = strlen(pget);
		if (linebuf[0] == '$') {
			fseek(fp, 0 - pget_len, SEEK_CUR);
			break;
		}


		pc = strchr(linebuf, '#');
		if (pc != NULL) {
			*pc = '\0';
		}

		pc = search_sense_char(linebuf, strlen(linebuf), FORWARD);
		if (pc == NULL) {
			dump_re("empty line\n");
			continue;
		}

		pc_end = pc + strlen(pc);
		pc_end = search_sense_char(pc_end - 1, (pc_end - pc), BACKWARD);
		*(pc_end + 1) = '\0';
		end_of_config = 0;

		if (*pc == '[') {
			if (rep->re_title[0] == '\0') {
				if (read_rule_title(pc, rep->re_title, 
						sizeof(rep->re_title)) < 0) {
					dump_re("(ERROR: title):%s", pc);
				} else {
					dump_re("(DEBUG: title):%s", rep->re_title);
				}
			} else {
				// rewind back to the first of this line
				fseek(fp, ftell(fp) - pget_len, SEEK_SET);
				break;
			}
		} else {
			// name=value line
			if (rep->re_title[0] == '\0') {
				ca_strncpy(rep->re_title, sizeof(rep->re_title), 
						"Anonymous", 9);
			}

			if (read_rule_var(pc, rep) < 0) {
				dump_re("(ERROR: var):%s", pc);
			} else {
				dump_re("(DEBUG: var):%s", pc);
			}
		}
	} // while

	if (end_of_config) {
		goto end_of_file;
	}

/*************
 * normal ret
 ************/
	return 1;

end_of_file:
	dump_re("go to end of file\n");
	return 0;

err:
	dump_re("error when read rule entry\n");
	return -1;
}

void dump_rule_entry(rule_entry *r_entry)
{
	if (!r_entry) {
		return;
	}

	printf("===Title:%s===\n", r_entry->re_title);

	rule_var *var = NULL;
	int i = 0;
	for (i = 0; i < r_entry->re_var_num; i++) {
		var = &(r_entry->re_varlist[i]);

		printf("%s\t=\t%s\n", var->rv_name, var->rv_valuep);
	}

	printf("\n");

	return;
}

void dump_rule_entry_list(rule_entry *rule_entry_list)
{
	rule_entry *r_entry = rule_entry_list;

	printf("==========%s begin==========\n", __FUNCTION__);
	while (r_entry != NULL) {
		dump_rule_entry(r_entry);
		r_entry = r_entry->re_next;
	}
	printf("==========%s end============\n", __FUNCTION__);

	return;
}

/*
 * search var in rule entry
 * return value:
 * 	not NULL: found
 */
static inline rule_var *__search_var_in_rule_entry(rule_entry *entryp, 
		char *var_name, var_cmp_func func)
{
	if (var_name == NULL || var_name[0] == '\0') {
		return NULL;
	}

	int i = 0;
	rule_var *varp = NULL;
	for (i = 0; i < entryp->re_var_num; i++) {
		varp = &(entryp->re_varlist[i]);
		if (!func((const char *)varp->rv_name, (const char *)var_name)) {
			return varp;
		}
	}

	return NULL;
}

rule_var *search_var_in_rule_entry(rule_entry *entryp, char *var_name)
{
	return (__search_var_in_rule_entry(entryp, var_name, strcmp));
}

rule_var *search_var_nocase_in_rule_entry(rule_entry *entryp, char *var_name)
{
	return (__search_var_in_rule_entry(entryp, var_name, strcasecmp));
}

/***********************
 * entry list operation
 **********************/
int init_rule_entry_list(rule_entry **rule_entry_list)
{
	if (rule_entry_list) {
		*rule_entry_list = NULL;
	}

	return 0;
}

// remove all the node of rule_entry_list
void destroy_rule_entry_list(rule_entry **rule_entry_list)
{
	rule_entry *ent = NULL;
	
	if (*rule_entry_list == NULL) {
		dump_re("rule entry list is empty\n");
		return;
	}

	do {
		ent = pop_from_rule_entry_list(rule_entry_list);
		free_rule_entry(ent);
	} while (ent != NULL);

	return;
}

int add_to_rule_entry_list(rule_entry **rule_entry_list, rule_entry *new_entry)
{
	int nret = 0;

	if (new_entry == NULL || rule_entry_list == NULL) {
		nret = -1;
		dump_re("ERROR: the incoming point is invalid\n");
		goto err;
	}

	if (new_entry->re_title[0] == 0) {
		nret = -1;
		dump_re("ERROR: cannot add a rule without title\n");
		goto err;
	}

	if (*rule_entry_list == NULL) {
		*rule_entry_list = new_entry;
	} else {
		rule_entry *tail = *rule_entry_list;

		// search for tail
		while (tail->re_next != NULL) {
			tail = tail->re_next;
		}

		tail->re_next = new_entry;
	}
	new_entry->re_next = NULL;

	return nret;

err:
	return nret;
}

// remove_from_rule_entry_list
// only rip off, not free del_entry
// return: the removed node, equal to del_entry
rule_entry *remove_from_rule_entry_list(rule_entry **rule_entry_list, rule_entry *del_entry)
{
	rule_entry *entry = NULL, *pre = NULL;

	if (del_entry == NULL) {
		dump_re("ERROR: removed entry is NULL\n");
		goto err;
	}

	if (*rule_entry_list == NULL) {
		dump_re("ERROR: cannot del from an empty list\n");
		goto err;
	}

	if (*rule_entry_list == del_entry) {	// it's just first node
		entry = pop_from_rule_entry_list(rule_entry_list);
		goto ret;
	}

	pre = *rule_entry_list;
	entry = pre->re_next;
	while (entry) {
		if (entry == del_entry) {
			// found, and rip off it from list
			pre->re_next = entry->re_next;
			break;
		}

		pre = entry;
		entry = entry->re_next;
	}

ret:
	return entry;
err:
	return NULL;
}

// del_from_rule_entry_list
// remove one node whose title is (title)
// NULL: not found
rule_entry *del_from_rule_entry_list(rule_entry **rule_entry_list, char *title, int title_len)
{
	rule_entry *ent = NULL, *pre = NULL;

	if (title[0] == 0) {
		dump_re("ERROR: cannot del a rule node with empty title\n");
		goto ret;
	}

	// search title
	ent = *rule_entry_list;
	while (ent != NULL) {
		if (!strncmp(ent->re_title, title, title_len)) {
			break;
		}

		pre = ent;
		ent = ent->re_next;
	}

	if (ent == NULL) {
		// not found
		dump_re("DEBUG: \"%s\" is not found in rule_entry_list\n", title);
		goto ret;
	}

	// del node from list
	if (pre == NULL) {
		// first node
		*rule_entry_list = ent->re_next;
	} else {
		pre->re_next = ent->re_next;
	}
	ent->re_next = NULL;

ret:
	return ent;
}

// pop_from_rule_entry_list
rule_entry *pop_from_rule_entry_list(rule_entry **rule_entry_list)
{
	rule_entry *ent = NULL;

	if (*rule_entry_list == NULL) {
		dump_re("DEBUG: cannot pop from empty list\n");
		goto ret;
	}

	ent = *rule_entry_list;
	*rule_entry_list = ent->re_next;
	ent->re_next = NULL;

ret:
	return ent;
}

/*
 * search one rule node in list whose title is equal to (title)
 */
static inline rule_entry *__search_rule_entry_list(rule_entry **rule_entry_list, 
		char *title, int title_len, title_cmp_func func)
{
	rule_entry *ent = NULL;

	if (title[0] == 0) {
		dump_re("Error: cannot search an empty title\n");
		goto ret;
	}

	ent = *rule_entry_list;
	while (ent != NULL) {
		if (!func((const char *)ent->re_title, (const char *)title, (size_t)title_len)) {
			break;
		}

		ent = ent->re_next;
	}

ret:
	return ent;
}

rule_entry *search_rule_entry_list(rule_entry **rule_entry_list, char *title, int title_len)
{
	return (__search_rule_entry_list(rule_entry_list, title, title_len, strncmp));
}

rule_entry *search_nocase_rule_entry_list(rule_entry **rule_entry_list, char *title, int title_len)
{
	return (__search_rule_entry_list(rule_entry_list, title, title_len, strncasecmp));
}

