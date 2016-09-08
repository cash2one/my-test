#include<stdio.h>
#include<string.h>
#include "gms_psql.h"
#include"store_common.h"
#include"table.h"

int store_sysstatus(char *insert_cmd)
{
	char now_time_str[25], month[3];                                                                          
        get_systime_str(now_time_str);                                                                            
        split_time_str(now_time_str, NULL, month, NULL, NULL, NULL, NULL);                                        
        printf("now_time: %s\n", now_time_str);                                                                   
        printf("month:%d\n", atoi(month));                                                                        
                                                                                                                  
        char table_name[TABLE_NAME_SIZE]; 
	sprintf(table_name,"%s%s%d", TABLE_NAME_SYS_STATUS, TABLE_SUFFIX, atoi(month));
	printf("table_name : %s\n", table_name); 
	
	sprintf(insert_cmd, "insert into %s(devid, c_time, disk, cpu, mem, state, runtime, libversion, sysversion, month_id) values('245accec-3c12-4642-967f-e476cef558c4','%s', 15, 8, 20, 1, 100, '1.1.1.1', '2.2.2.2',%d)", table_name, now_time_str,atoi(month));
	printf("insert_cmd:%s\n", insert_cmd);
	return 0;
}
