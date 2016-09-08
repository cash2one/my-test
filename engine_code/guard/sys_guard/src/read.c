#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "report_stat.h"
int split(char* str, char* delim, char* dest[SIZE])
{
        int num = 0;
        for(dest[num] = strtok(str, delim); dest[num] != NULL; dest[++num] = strtok(NULL, delim));

        return 0;
}
int read_sys_file(char *info)
{
//	struct SYS_INFO sys_info[3]; 
        FILE *fp;
        if((fp = fopen( SYS_STATUS_PATH,"rb")) == NULL) {
                printf("cannot open file! \n");
		        fclose(fp);
                return -1;
        }
	int i = 0;
        while(fgets(info, 20, fp) !=NULL){
                printf("info:%s", info);
		char *delim = ":";
		char* dest[SIZE];
		split(info, delim, dest);
		//printf(">%s\n>%s\n",dest[0],dest[1]);
		strncpy(sys_info[i].name, dest[0], (int)strlen(dest[0]));
		sys_info[i].percent = atof(dest[1]);
		printf("sys_info[%d]:%s\n",i,sys_info[i].name);
		printf("sys_info[%d]:%f\n",i,sys_info[i].percent);
		i++;
                       
        }
	fclose(fp);
/*
        int retn;
        retn = remove(AUTH_FILE_PATH);
        if(retn < 0)
                return -1;*/
        return 0;
}
#if 0
int main(void)
{
	char info[10];
	read_sys_file(info);

	return 0;
}
#endif
