/************************************************
 *       Filename: pcre_op.c
 *    Description: pcre function
 *
 *        Created: 2009-01-04 16:57:06
 *         Author: david dengwei@venus.com
 ************************************************/

#include <pcre.h>
#include <string.h>

#include "misc.h"
#include "hint.h"

/************************************************
 *         Name: pcre_strstr
 *  Description: ÕýÔò±í´ïÊ½Æ¥Åäº¯Êý
 *  		 
 *       Return: >0 success, ·µ»Ø²ÎÊý¸öÊý, 
  		    µÚÒ»¸ö²ÎÊý¿Ï¶¨ÊÇ·µ»ØµÄ×Ü´®
 * 		 =0 no match
 *       	 <0 fail when match
 ************************************************/
int pcre_strstr(char *check_str, int check_strlen, char *pcre_str, 
			ret_value_t *ret_array, int ret_max_len)
{
#define OVECCOUNT (3 * MAX_PCRE_RET_NUM)	// ±íÊ¾×î¶à¿ÉÒÔ·µ»Ø6×éÊý¾Ý, °üÀ¨Ô­Ê¼Êý¾Ý, ¼´¿ÉÒÔÓÐ5¸öÀ¨ºÅ
	int err_code = 0;

	pcre *pcre_data = NULL;

	if (unlikely(!pcre_str || strlen(pcre_str) == 0)) {
		err_code = PCRE_RULE_ERR;
		goto ret;
	}

	if (unlikely(!check_str || check_strlen == 0)) {
		err_code = PCRE_MATCH_ERR;
		goto ret;
	}

	const char *error_str = NULL;
	int rule_erroffset = 0;

	int ovector[OVECCOUNT] = {0};
	int rc = 0;

	pcre_data = pcre_compile(pcre_str,	/* the pattern */
			0,		/* default options */
			&error_str,	/* for error message */
			&rule_erroffset,/* for error offset */
			NULL);		/* use default character tables */

	if (!pcre_data) {
		mdump("STRING: %s\nPCRE compilation failed at offset %d: %s\n", 
				pcre_str,
				rule_erroffset,
				error_str);
		err_code = PCRE_RULE_ERR;
		goto ret;
	}

	pcre_extra *extra = pcre_study(pcre_data, 0, &error_str);

	rc = pcre_exec(pcre_data,		/* the compiled pattern */
			extra,		/* extra data - the result of pcre_study */
			check_str,	/* the subject string */
			check_strlen,	/* the length of the subject */
			0,		/* start at offset 0 in the subject */
			0,		/* default options */
			ovector,	/* output vector for substring information */
			OVECCOUNT);	/* number of elements in the output vector */

	/********************************************
	 * pcre_execÔËÐÐ½á¹û²âÊÔËµÃ÷:
	 * 	1. -1: Ê§°Ü, ¼´PCRE_ERROR_NOMATCH
	 * 	2. 0:  ³É¹¦, µ«·µ»ØÄÚÈÝ³¬³öÁËOVECCOUNT
	 * 	3. >0: ³É¹¦
	 * 	4. ÆäËû¸ºÖµ: Òì³£Ê§°Ü
	 ********************************************/
	mdump("rc = %d\n", rc);

	if (rc <= 0) {
		if (rc == PCRE_ERROR_NOMATCH) {	// PCRE_ERROR_NOMATCH = -1
			mdump("no match (%s) for (%s) rc = %d\n", pcre_str, check_str, rc);
			err_code = PCRE_NO_MATCH;
			goto ret;
		} else {
			merror("Matching (%s), errcode: %d\n", pcre_str, rc);
			err_code = PCRE_MATCH_ERR;
			goto ret;
		}
	}

	mdump("return: %d\n", rc);

	if (ret_array != NULL) {
		err_code = MIN(rc, ret_max_len);
		ret_value_t *node = ret_array;

		int i = 0; 
		int ovector_i = 0;
		for (i = 0; i < err_code; i++, ovector_i += 2) {
			if (node->start == NULL) {
				node->start = check_str + ovector[ovector_i];
				node->len   = ovector[ovector_i + 1] - ovector[ovector_i];
			} else {
				ca_strncpy((node->start), node->len, 
					(check_str + ovector[ovector_i]), 
					ovector[ovector_i + 1] - ovector[ovector_i]);
			}

			mdump("%2d: %d, --%.*s--\n", i, node->len, node->len, node->start);

			node++;
		}
	} else {
		err_code = rc;
	}

ret:
	if (!pcre_data) {
		pcre_free(pcre_data);		/* Release memory used for the compiled pattern */
		pcre_data = NULL;
	}

	return err_code;
}

