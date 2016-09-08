#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"
#include "misc.h"
#include "service_group.h"



uint16_t service_group_table[SERVICE_GROUP_MAX_NUM];
uint8_t service_mobile_table[SERVICE_GROUP_MAX_NUM];

void service_group_table_print(void)
{
	int loop= 0;
	
	for(loop = 0; loop < SERVICE_GROUP_MAX_NUM; loop++)		
	{
		if(0 != service_group_table[loop])
			printf("[server_group_table]  server_group: %d , service_type: %d , mobile: %d\n",
			service_group_table[loop], loop, service_mobile_table[loop]);	
	}
	printf("\n");
}


uint16_t service_group_find(uint16_t appid)
{
	return service_group_table[appid];
}

uint8_t service_mobile_find(uint16_t appid)
{
	return service_mobile_table[appid];
}

uint32_t service_group_table_create()
{
	FILE *fp = NULL;
	uint32_t service_type = 0; 
	uint32_t service_group = 0;
	uint32_t service_mobile = 0;
    char file[100] = {0};

    sprintf(file, "%s/service_group.list", PROGRAM_DIR);
	fp = fopen(file,"r");
	if(fp == NULL)
	{
		printf("fopen service_group fail\n");
		return UTAF_FAIL;
	}
	
	printf("fopen service_group success!\n");	
		

	int cnt = 0;
	while (3 == fscanf(fp, "%d%d%d\n", &service_group, &service_type, &service_mobile) && cnt < SERVICE_GROUP_MAX_NUM)
	{
        if (service_type >= SERVICE_GROUP_MAX_NUM) {
            continue;
        }

		cnt++;
		service_group_table[service_type] = (uint16_t)service_group;
		service_mobile_table[service_type] = (uint8_t)service_mobile;
	}
	
	//service_group_table_print();
	printf("service_type lib total entry %d\n", cnt);
	fclose(fp);
	return UTAF_OK;	
}


uint32_t service_group_init()
{
    int loop = 0;
	
	for (loop = 0; loop < SERVICE_GROUP_MAX_NUM; loop++) {
		service_group_table[loop] = 20;
        service_mobile_table[loop] = 0;
	}
    
	service_group_table_create();
	
	return UTAF_OK;
}

