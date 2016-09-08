#ifndef _RULE_TABLE_N_H
#define _RULE_TABLE_N_H

#include <stdio.h>
#include <sys/types.h>

#define MAX_VAR_NAME_LENGTH	64
#define MAX_VAR_VALUE_LENGTH	4096
// max length of one line in file
#define RULE_TABLE_LINE_LENGTH	(MAX_VAR_VALUE_LENGTH + MAX_VAR_NAME_LENGTH + 4)

// rule_var structure, means "name = value"
typedef struct _rule_var {
	char		rv_name[MAX_VAR_NAME_LENGTH];

	char 		*rv_valuep;	// pointer, ÄÚÈÝÔÚre_vardataÖÐ
	int 	rv_length;
} rule_var;

#define MAX_TITLE_LENGTH		128	// 100 -> 128
//#define MAX_RULE_VARDATA_LENGTH		4096	// 2048 -> 4096
#define MAX_RULE_VAR_NUM		64	// "name = value" can max be 64 lines

// rule_entry structure, means one configuration section
// one title
// sevaral "name = value", equal rule_var
typedef struct _rule_entry {
	char re_title[MAX_TITLE_LENGTH];

	rule_var re_varlist[MAX_RULE_VAR_NUM];	// name = value
	int re_var_num;			// node num of varlist

//	char re_vardata[MAX_RULE_VARDATA_LENGTH];	// all data of value
//	int re_vardata_len;			// length of vardata

	struct _rule_entry *re_next;
} rule_entry;

// rule file operation
//FILE *open_rule_table(char *path, char *read_write);
//void close_rule_table(FILE *file);

// rule entry operation
rule_entry *new_rule_entry();
void free_rule_entry(rule_entry *rep);
int read_rule_entry(FILE *file, rule_entry *rentry);

void dump_rule_entry(rule_entry *r_entry);
void dump_rule_entry_list(rule_entry *rule_entry_list);

rule_var *search_var_in_rule_entry(rule_entry *entry, 
		char *var_name);
rule_var *search_var_nocase_in_rule_entry(rule_entry *entry, 
		char *var_name);

// rule entry list operation
int init_rule_entry_list(rule_entry **rule_entry_list);
void destroy_rule_entry_list(rule_entry **rule_entry_list);
int add_to_rule_entry_list(rule_entry **rule_entry_list, rule_entry *new_entry);
rule_entry *del_from_rule_entry_list(rule_entry **rule_entry_list, char *title, int title_len);
rule_entry *search_rule_entry_list(rule_entry **rule_entry_list, char *title, int title_len);
rule_entry *search_nocase_rule_entry_list(rule_entry **rule_entry_list, char *title, int title_len);
rule_entry *pop_from_rule_entry_list(rule_entry **rule_entry_list);
rule_entry *remove_from_rule_entry_list(rule_entry **rule_entry_list, rule_entry *del_entry);

void zero_rule_entry(rule_entry *rep);

#endif	//_RULE_TABLE_N_H

