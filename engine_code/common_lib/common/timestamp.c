
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#define CPUINFO_PROC "/proc/cpuinfo"
static int g_cpukhz = 0;

int get_cpukhz()
{
#define LINE_LEN 1024
#define MATCH_STR "cpu MHz"
	int cpu_khz = -1;
	char strline[LINE_LEN] = {'\0'};
	char *pline = NULL;

	FILE *pfile = fopen(CPUINFO_PROC, "r");
	if (pfile == NULL) {
		return -1;
	}

	while (memset(strline, 0, LINE_LEN) != NULL
		&& (pline = fgets(strline, LINE_LEN, pfile)) != NULL) {
		if (strncasecmp(pline, MATCH_STR, strlen(MATCH_STR)) == 0) {
			pline += strlen(MATCH_STR);

			// "cpu MHz             : 1695.357"
			// 跳过tab, space, 冒号
			while ((*pline == '	') || (*pline == ' ') || (*pline == ':')) {
				++pline;
			}

			cpu_khz = (int) (atof(pline) * 1000);
			if (cpu_khz == 0) {
				cpu_khz = -1;
			}
			break;
		}
	}

	(void)fclose(pfile);
	g_cpukhz = cpu_khz;

	return cpu_khz;
}

// 输入 CPU 的嘀嗒次数差和cpu的k频率，返回对应的毫秒值
unsigned long long cputick2ms(unsigned long long diff_track)
{
	return (diff_track / g_cpukhz);
}

// 输入 CPU 的嘀嗒次数差和cpu的k频率，返回对应的纳秒值
unsigned long long cputick2ns(unsigned long long diff_track)
{
	return ((diff_track * 1000)/ g_cpukhz);
}

