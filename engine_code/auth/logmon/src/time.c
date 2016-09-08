#define _XOPEN_SOURCE
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>


int time_cmp(char *time_str)
{
	//struct tm *n_time;                                                                                    
        struct tm l_time;                                                                                    
        time_t now_time, limit_time;
	char time_format[] = "%Y%m%d%H%M%S"; 
	
	time(&now_time);
	//n_time = localtime(&now_time);                                                                      
	
        strptime(time_str, time_format, &l_time);                                                                      
                                                                                                                  
        //now_time = mktime(n_time);                                                                               
        limit_time = mktime(&l_time);                                                                               
	printf("limit_time = %ld\n", limit_time);
                                                                                                                  
        return (now_time-limit_time);                                                                                                                
}

int time_cmp_format(char *time_str1, char *time_str2, char *time_format)
{
    struct tm time1;
    struct tm time2;
    //char time_format[] = "%Y-%m-%d %X";
    time_t time_sec1, time_sec2;

    strptime(time_str1, time_format, &time1);
    strptime(time_str2, time_format, &time2);

    time_sec1 = mktime(&time1);
    time_sec2 = mktime(&time2);

    return (time_sec1-time_sec2);
}
