#include <string.h>
#include "statistics.h"
#include "sharemem.h"
#include "misc.h" //wdb_ppp: #include "utaf.h"

struct monitor *monitor_info;

uint32_t monitor_init(void)  //wdb_calc222
{
	printf("monitor_init init size %lu\n", sizeof(struct monitor));
	if(UTAF_OK != sharemem_create(SHAREMEM_ID_MONITOR, sizeof(struct monitor), (void **)(&monitor_info)))
	{
		printf("sharemem_init error\n");
		return UTAF_FAIL;
	}

	memset((void *)monitor_info, 0, sizeof(struct monitor));

	monitor_info->magic = STATISTICS_MAGIC;

	return UTAF_OK;
}

int monitor_exit(void)
{
	/** delete share memory **/
	sharemem_del(SHAREMEM_ID_MONITOR);

	return UTAF_OK;
}

