/************************************************
 *       Filename: sys_op.c
 *    Description: 一些系统信息的获取
 *
 *        Created: 2009-05-15 15:08:44
 *         Author: david dengwei@venus.com
 ************************************************/

#include <stdio.h>
#include "hint.h"

// get total mem, unit k
int get_total_mem()
{
	int total_mem = -1;

	FILE *meminfo = NULL;			/* input-file pointer */
	char *meminfo_file_name = "/proc/meminfo";	/* input-file name    */

	meminfo = fopen((const char *)meminfo_file_name, "r");
	if (meminfo == NULL) {
		merror("couldn't open file '%s'; %s\n",
				meminfo_file_name, strerror(errno));
		goto err;
	}

	char strline[1024] = {0};
	char *pline = NULL;

#define TOTAL_MEM_HEAD "MemTotal:"
	int head_len = strlen(TOTAL_MEM_HEAD);

	while (memset(strline, 0, 1024)
		&& fgets(strline, 1024, meminfo) != NULL) {
		pline = search_sense_char(strline, strlen(strline), 0);
		if (pline == NULL) {
			continue;
		}

		if (strncasecmp(pline, TOTAL_MEM_HEAD, head_len) == 0) {
			pline += head_len;
			pline = search_sense_char(pline, strlen(pline), 0);
			if (pline != NULL) {
				total_mem = atoi(pline);
				break;
			}
		}
	}

	if(fclose(meminfo) == EOF) {
		merror("couldn't close file '%s'; %s\n",
				meminfo_file_name, strerror(errno));
		goto err;
	}

	return total_mem;

err:
	return -1;
}

