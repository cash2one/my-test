#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "authfile_info.h"
#include"logmon.h"
int split(char* str, char* delim, char* dest[SIZE], int* count)
{
        int num = 0;
        for(dest[num] = strtok(str, delim); dest[num] != NULL; dest[++num] = strtok(NULL, delim));
        *count = num;

        return 0;
}

#if 0
int read_authfile(char *info)
{
        FILE *fp;
        if((fp = fopen(AUTH_FILE_PATH,"rb")) == NULL){
                printf("cannot open file! \n");
                return -1;
        }
        fseek(fp,0,SEEK_END);
        int file_size;
        file_size = ftell(fp);
        printf("length:%d\n",file_size);
        fseek(fp,0,SEEK_SET);
        if(fread(info,file_size,sizeof(char),fp) !=1){
                if(feof(fp)){
                        fclose(fp);
                        return 0;
                }
        } else {
		fclose(fp);
	}
	info[file_size - 1] = '\0';
        printf("info:%s\n",info);
        #if 0
	int retn;
        retn = remove(AUTH_FILE_PATH);
        if(retn < 0)
                return -1;
#endif
        return 0;
}
#endif
int get_file_info(struct FILE_INFO *file_info, char *info)
{
        extern int dest_num;
        //char info[1024];
        //memset(info, 0, 1024);
        //read_authfile(info);
        char *delim = ";";
        char* dest[SIZE] = {0};
        split(info, delim, dest, &dest_num);
        //printf("dest[%d]:%s\n", dest_num, dest[18]);
        printf("-------num:%d\n", dest_num);
        strncpy(file_info->active_time, dest[0], (int)strlen(dest[0]));
        strncpy(file_info->serial_no, dest[1], (int)strlen(dest[1]));

        printf("time:%s\n", file_info->active_time);
        printf("no:%s\n", file_info->serial_no); 
        printf("**************************\n");
	char comm_name[] = {"cloudcon"};
        char comm[2] = {0};
        char r_num[2] = {0};
        char r_name[] = {"root"};
        parse_xml(comm, comm_name);
        parse_xml(r_num, r_name);
        int i,j=0;
        for(i = 2; i < dest_num; i++){                                                                          
            if(i % 2 == 0){                                                                                   
                //strncpy(file_info->srv_array[j].name, dest[i], 29);                                        
                //printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);       
			    if(strcmp(dest[i], "vds") == 0) {
				    strncpy(file_info->srv_array[j].name, "dd", 2);
				    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
				    j++;
				    strncpy(file_info->srv_array[j].name, "apc", 3);
				    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                    j++;
				    strncpy(file_info->srv_array[j].name, "dm", 2);
				    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
			    } else if(strcmp(dest[i], "apt") == 0) {
                    strncpy(file_info->srv_array[j].name, "apt", 3);
                    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                    j++;
                    strncpy(file_info->srv_array[j].name, "offline", 7);
                    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                } else if(strcmp(dest[i], "comm") == 0) {
                    //if(strcmp(r_num, "1") == 0){
                    strncpy(file_info->srv_array[j].name, "comm_main.py", 12);
					printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                    j++;
                    strncpy(file_info->srv_array[j].name, "comm_cloud.py", 13);
					printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                    j++;
                    strncpy(file_info->srv_array[j].name, "event_export.py", 15);
                    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                    /* } else {
                    strncpy(file_info->srv_array[j].name, "comm_main.py", 12);
					printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
                    }	*/

			    } else {
				    strncpy(file_info->srv_array[j].name, dest[i], 29);
                    printf("array[%d].name:%s\n",j,file_info->srv_array[j].name);
			    }
            } else {                                                                                            
                //strncpy(file_info->srv_array[j].status, dest[i], 14);                                     
                //printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                if(strcmp(dest[i - 1], "vds") == 0) {
				    j--;
                    j--;
                    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
				    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                    j++;
                    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
				    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                    j++;
                    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
				    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);

                } else if(strcmp(dest[i - 1], "apt") == 0) {
                    j--;
                    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
                    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                    j++;
                    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
                    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                } else if(strcmp(dest[i - 1], "comm") == 0) {
				    if(strcmp(r_num, "1") == 0){
                        j--;
                        j--;
					    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
					    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
					    j++;
					    strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
					    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                        j++;
                        if(strcmp(comm, "0") == 0) {
                            strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i])); 
                            printf("array[%d].status:%s\n",j,file_info->srv_array[j].status); 
                        } else {
                            strncpy(file_info->srv_array[j].status, "0", 1); 
                            printf("array[%d].status:%s\n",j,file_info->srv_array[j].status); 
                        }
				    } else {
                        j--;
                        j--;
                        strncpy(file_info->srv_array[j].status, dest[i], (int)strlen(dest[i]));
					    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
                        j++;
                        strncpy(file_info->srv_array[j].status, "0", 1);
                        printf("array[%d].status:%s\n",j,file_info->srv_array[j].status); 
                        j++;
                        strncpy(file_info->srv_array[j].status, "0", 1);                                          
                        printf("array[%d].status:%s\n",j,file_info->srv_array[j].status); 
				    }
			    } else {
				    strncpy(file_info->srv_array[j].status, dest[i], 14);                                     
                    printf("array[%d].status:%s\n",j,file_info->srv_array[j].status);
			    }                         
                j++;                                                                                      
            }                                                                                                 
                                                                                                                  
        }                                                                               
        return 0;                                                                                                 
}                   
