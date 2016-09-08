#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "lock_file.h"
#include "config.h"

#define LINE_LEN 4096

typedef enum {
	NOFIND_TITLE = 0,
	FIND_TITLE,
	NONEEDFIND_TITLE
} FIND_TYPE;

static inline char *skip_annotate(char *buf, char note_char)
{
	char *p = strchr(buf, note_char);
	if (p != NULL) {
		*p = '\0';
	}

	return p;
}

typedef int (*copy_action_t)(char *buf, char *match_str);
static inline int _copy_file(FILE *fp_r, FILE *fp_w, copy_action_t action, char *match_str)
{
	char *pbuf = NULL;
	char buf[LINE_LEN] = {0};
	int find_it = -1;
	int action_ret = 0;

	if (!action) {
		find_it = 0;
	}

	while (memset(buf, 0, LINE_LEN)
		&& (pbuf = fgets(buf, LINE_LEN, fp_r)) != NULL) {

		pbuf = search_sense_char(pbuf, strlen(pbuf), 0);
		if (pbuf == NULL) {
			fputs(buf, fp_w);
			continue;
		}

		if (action != NULL) {
			action_ret = action(pbuf, match_str);
			if (action_ret > 0) {
				find_it = 0;
			} else if (action_ret == 0) {
				find_it = 0;
				fputs(buf, fp_w);
			} else {
				fputs(buf, fp_w);
			}
		} else {
			fputs(buf, fp_w);
		}
	}

	return find_it;
}

static inline int copy_file_func(char *src, char *dst, copy_action_t action, char *match_str)
{
	FILE *psrc = NULL, *pdst = NULL;
	int find_it = -1;

	psrc = fopen_lock(src, "r");
	if (psrc == NULL) {
		goto err;
	}

	pdst = fopen_lock(dst, "w");
	if (pdst == NULL) {
		goto err;
	}

	find_it = _copy_file(psrc, pdst, action, match_str);
	fclose_unlock(psrc);
	fclose_unlock(pdst);

	return find_it;

err:
	if (psrc) {
		fclose_unlock(psrc);
	}

	if (dst) {
		fclose_unlock(pdst);
	}

	return -1;
}

/* 
 * ret:
 * 	-1: no find
 * 	 0: suc
 */
static inline int split_string(char *buffer, char split_char, 
				char **left, int *left_len, 
				char **right, int *right_len)
{
	*left = buffer;

	char *mid = strchr(buffer, '=');
	if (unlikely(mid == NULL)) {
		goto nofind;
	}

	char *t_right = mid + 1;
	mid = search_sense_char(mid - 1, mid - buffer, -1);
	*left_len = mid + 1 - *left;

	t_right = search_sense_char(t_right, strlen(t_right), 1);
	if (t_right != NULL) {
		char *end = t_right + strlen(t_right);
		end = search_sense_char(end - 1, end - t_right, -1);
		*right_len = end + 1 - t_right;
	} else {
		*right_len = 0;
	}

	*right = t_right;
	return 0;

nofind:
	return -1;
}

typedef int (*match_action_t)(char *value, int value_len, void *arg);
typedef int (*nomatch_action_t)(char *buf, void *arg);

/* 
 * no find: -1
 * find: 0
 */
static inline int _search_conf(FILE *fp, char *title, char *config_name, 
				match_action_t match_action, void *match_arg, 
				nomatch_action_t nomatch_action, void *nomatch_arg)
{
	int find_ret = -1;
	if (unlikely(fp == NULL 
		|| config_name == NULL)) { 
		goto ret;
	}

	char buf[LINE_LEN] = {0};
	char *pbuf = NULL;
	int config_namelen = strlen(config_name);
	char *left = NULL, *right = NULL;
	int left_len = 0, right_len = 0;

	int title_len = (title != NULL ? strlen(title) : 0);
	int find_title = NOFIND_TITLE;
	if (title_len == 0) {
		find_title = NONEEDFIND_TITLE;
	}

	char *p_annotate = NULL;
	int match_ret = 0;

	while (memset(buf, 0, LINE_LEN) 
		&& fgets(buf, LINE_LEN, fp) != NULL) {
		p_annotate = skip_annotate(buf, '#');

		if ((pbuf = search_sense_char(buf, strlen(buf), 0)) == NULL) {
			goto nomatch;
		}

		if (pbuf[0] == '[' && strchr(pbuf + 1, ']') != NULL) {
			if (find_title != NONEEDFIND_TITLE) {
				if (strncasecmp(pbuf + 1, title, title_len) == 0) {
					find_title = FIND_TITLE;
				} else {
					find_title = NOFIND_TITLE;
					goto nomatch;
				}
			}
		}

		if (find_title == NOFIND_TITLE) {
			goto nomatch;
		}
		
		if (strncasecmp(pbuf, config_name, config_namelen) != 0) {
			goto nomatch;
		}

		if (split_string(pbuf, '=', &left, &left_len, &right, &right_len) == -1)  {
			// wrong format, skip it
			continue;
		}

		if (left_len != config_namelen) {
			goto nomatch;
		}

		if (match_action) {
			match_ret = match_action(right, right_len, match_arg);
			if (match_ret >= 0) {
				// action ok
				find_ret = 0;
				if (match_ret == 0) {
					break;
				}

				continue;
			}
		}

nomatch:
		if (p_annotate != NULL) {
			*p_annotate = '#';
		}

		if (nomatch_action && nomatch_action(buf, nomatch_arg) < 0) {
			break;
		}
	}

ret:
	return find_ret;
}

typedef struct {
	char *value;
	int valuelen;
} value_struct_t;
int copy_value(char *value, int value_len, void *arg)
{
	value_struct_t *value_struct = (value_struct_t *)arg;
	ca_strncpy(value_struct->value, value_struct->valuelen, value, value_len);

	return 0;
}

int read_conf(char *filename, 
		char *title, char *config_name, 
		char *value, int max_valuelen)
{
	if (unlikely(filename == NULL 
		|| config_name == NULL)) { 
		return -1;
	}

	FILE *fp = fopen_lock(filename, "r");
	if (fp == NULL) {
		return -2;
	}

	value_struct_t value_struct = {
		.value = value,
		.valuelen = max_valuelen
	};
	int find_ret = _search_conf(fp, title, config_name, 
					copy_value, &value_struct, 
					NULL, NULL);

	fclose_unlock(fp);
	return find_ret;
}

int cmp_value(char *value, int value_len, void *arg)
{
	value_struct_t *value_struct = (value_struct_t *)arg;
	return (strncasecmp(value_struct->value, value, value_len));
}

/*
 * find: 0
 * nofind: -1
 */
int search_conf(char *filename, 
		char *title, char *config_name, 
		char *value)
{
	if (unlikely(filename == NULL 
		|| config_name == NULL 
		|| value == NULL)) {
		return -1;
	}

	FILE *fp = fopen_lock(filename, "r");
	if (fp == NULL) {
		return -2;
	}

	value_struct_t value_struct = {
		.value = value,
		.valuelen = strlen(value)
	};

	int find_ret = _search_conf(fp, title, config_name, 
					cmp_value, &value_struct, 
					NULL, NULL);

	fclose_unlock(fp);
	return find_ret;
}

/* 
 * del or modify one config_node
 *
 * return :
 * 	0: success
 * 	-1: required param is null
 * 	-2: file  operation fail
 *     	-3: create tmp file fail
 *     	-4: sync file fail 
 */
typedef struct {
	FILE *file;
	char *value;
	char *content;
} match_struct_t;
int copy_match(char *value, int value_len, void *arg)
{
	match_struct_t *match_struct = (match_struct_t *)arg;
	if (match_struct->value != NULL 
		&& match_struct->value[0] != '0'
		&& strncasecmp(match_struct->value, value, value_len)) {
		return -1;
	}

	if (match_struct->content != NULL && match_struct->content[0] != '\0') {
		fputs(match_struct->content, match_struct->file);
	}

	return 1;
}
int copy_nomatch(char *buf, void *arg)
{
	FILE *file = (FILE *)arg;
	if (buf != NULL) {
		fputs(buf, file);
	}

	return 0;
}

/*
 * ret:
 * 	0: changed
 * 	<0:no changed
 */
static inline int modify_config(char *filename, 
				char *title, char *config_name, char *value,
				char *content)
{
	FILE *fp_lock = NULL;
	FILE *fp_r = NULL;
	FILE *fp_t = NULL;
	FILE *fp_w = NULL;

	fp_lock = fopen_lock(filename, "r");
	if (unlikely(fp_lock == NULL)) {
		goto err;
	}

	fp_t = tmpfile();	// open for "w+b"
	if (unlikely(fp_t == NULL)) {
		goto err;
	}

	fp_r = fopen(filename, "r");
	if (unlikely(fp_r == NULL)) {
		goto err;
	}

	match_struct_t match_struct = {
		.file = fp_t,
		.content = content
	};

	int changed = _search_conf(fp_r, title, config_name,
				copy_match, &match_struct,
				copy_nomatch, fp_t);

	fclose(fp_r);
	fp_r = NULL;

	if (changed == 0) {
		// copy back from tmpfile
		fp_w = fopen(filename, "w");
		if (unlikely(fp_w == NULL)) {
			goto err;
		}

		rewind(fp_t);
		_copy_file(fp_t, fp_w, NULL, NULL);

		fclose(fp_w);
	}

	fclose(fp_t);
	fclose_unlock(fp_lock);
	return changed;

err:
	if (fp_lock != NULL) {
		fclose_unlock(fp_lock);
	}

	if (fp_r != NULL) {
		fclose(fp_r);
	}

	if (fp_w != NULL) {
		fclose(fp_w);
	}

	if (fp_t != NULL) {
		fclose(fp_t);
	}

	return -1;
}

int write_conf(char *filename, char *title, 
		char *config_name, char *match_value, char *set_value)
{
	char content[LINE_LEN] = {0};

	if (unlikely(filename == NULL 
			|| config_name == NULL 
			|| set_value == NULL)) {
		return -1;
	}

	snprintf(content, LINE_LEN, "%s\t= %s\n", config_name, set_value);
	return (modify_config(filename, title, config_name, match_value, content));
}

int del_conf(char *filename, char *title, char *config_name, char *match_value)
{
	if (unlikely(filename == NULL 
			|| config_name == NULL)) {
		return -1;
	}

	return (modify_config(filename, title, config_name, match_value, NULL));
}

/*param filename:config filename
    * @param match_str:to be added line
    * @param cont:insert content
    * @param pas:add before or after a line flag
    * @return:*0:success
    * -1:required param is null
    * -2:file operation fail * -3:create tmp file fail * -4:sync file fail * concrete error define ... ()
*/
int add_file_cont(char *filename, char *match_str, char *cont, int pos)
{
	FILE *fd_r = NULL, *fd_w = NULL;
	char buf[LINE_LEN] = {0};
	int ret_val = -1;
	char filetmp[128] = {0};
	int flag = 0;

	if (unlikely(filename == NULL || match_str == NULL || cont == NULL 
			|| (pos != DIR_BEFORE && pos != DIR_AFTER))) {
		ret_val = -1;
		goto ret;
	}
	fd_r = fopen_lock(filename, "r");
	if (fd_r == NULL) {
		ret_val = -2;
		goto ret;
	}

	make_tmpfile("/tmp", "service", filetmp, 128);
	fd_w = fopen(filetmp, "w");
	if (fd_w == NULL) {
		ret_val = -2;
		goto ret;
	}

	while (memset(buf, 0, LINE_LEN)
		&& fgets(buf, LINE_LEN, fd_r) != NULL) {

		if (strncasecmp(buf, match_str, strlen(match_str)) != 0) {
			fwrite(buf, 1, strlen(buf), fd_w);
		} else {
			if (pos == DIR_AFTER) {
				fwrite(buf, 1, strlen(buf), fd_w);
			}

			fwrite(cont, 1, strlen(cont), fd_w);
			fwrite("\n", 1, 1, fd_w);
			flag = 1;

			if (pos == DIR_BEFORE) {
				fwrite(buf, strlen(buf), 1, fd_w);
			}
		}
	}

	fclose(fd_w);
	fd_w = NULL;
	fclose_unlock(fd_r);
	fd_r = NULL;

	if (flag) {
		if (copy_file_func(filetmp, filename, NULL, NULL) == 0) {
			ret_val = 0;
		} else {
			ret_val = -4;
		}
	}

ret:
	if (fd_r != NULL) {
		fclose_unlock(fd_r);
	}
	if (fd_w != NULL) {
		fclose(fd_w);
	}
	return ret_val;
}

static int match_action(char *buf, char *match_str)
{
	if (strncasecmp(buf, match_str, strlen(match_str)) == 0) {
		return 1;
	} else {
		return -1;
	}
}
int del_conf_str(char *filename, char *match_str)
{
	FILE *fp_lock = fopen_lock(filename, "r");
	if (fp_lock == NULL) {
		return -1;
	}

	FILE *fp_w = NULL;
	FILE *fp_t = NULL;

	FILE *fp_r = fopen(filename, "r");
	if (unlikely(fp_r == NULL)) {
		goto err;
	}
	fp_t = tmpfile();	// open for "w+b"
	if (unlikely(fp_t == NULL)) {
		goto err;
	}

	int find_it = _copy_file(fp_r, fp_t, match_action, match_str);
	fclose(fp_r);
	fp_r = NULL;

	if (find_it == 0) {
		fp_w = fopen(filename, "w");
		if (unlikely(fp_w == NULL)) {
			goto err;
		}

		rewind(fp_t);
		_copy_file(fp_t, fp_w, NULL, NULL);

		fclose(fp_w);
	}

	fclose(fp_t);
	fclose_unlock(fp_lock);
	return find_it;

err:
	if (fp_lock != NULL) {
		fclose_unlock(fp_lock);
	}

	if (fp_r != NULL) {
		fclose(fp_r);
	}

	if (fp_w != NULL) {
		fclose(fp_w);
	}

	if (fp_t != NULL) {
		fclose(fp_t);
	}

	return -1;
}

