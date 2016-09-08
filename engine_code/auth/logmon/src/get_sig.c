#include<stdio.h>
#include<dirent.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include"auth_cfg.h"
#include"authfile_info.h"
#include"logmon.h"
/***********************************************************************
 flag = 0    说明是第一次授权或授权文件发生改变，需要进行签名验证和解密
 flag = 1    说明已经授过权了，只需进行看守
************************************************************************/
int get_sign_info(char *name, char *sign_len, char *sign_time)
{
	struct dirent *ptr;
    DIR *dir;
    dir = opendir(AUTH_CHECK_RET_PATH);
    int count = 0;
    //extern char org_time[15];                                                                             
    while((ptr = readdir(dir)) !=NULL) {
        //if(ptr->d_name[0] == '.' || ptr->d_name[0] == '..')
		if(strcmp(".", ptr->d_name) == 0 ||strcmp("..", ptr->d_name) == 0)
        continue;
        char *p = NULL;
        int len = strlen(ptr->d_name);
        for(p = ptr->d_name; *p !='.' && *p != '\0'; ++p);
        char buf[5];
        memset(buf, 0, 5);
        strncpy(buf, p, 4);
          // printf("buf:%s\n",buf);
        char str[5] = ".gau";
        int ret = strcmp(buf, str);
        if(ret == 0) {
        //  printf("ptr->name[%d]:%s\n", (int)strlen(ptr->d_name),ptr->d_name);
            memset(name, 0, len - 3);
            strncpy(name, ptr->d_name, len - 4);
            //strncpy(name, ptr->d_name, (int)strlen(name));
            printf("name[%d]:%s\n",(int)strlen(name), name);
		    
            char *head = NULL;                                                                                            
            char *q = NULL;                                                                                               
            int length = 0;                                                                                               
            for(head = name, q = name; *q != '-'; ++q);                                                                   
            ++q;
            for(head = q; *q != '-'; ++q);                                                                                
            memcpy(sign_len, head, q - head);                                                                              
            length = (int)strlen(sign_len);                                                                                
            printf("sign_length:%d\n", length);                                                                           
            sign_len[length] = '\0';                                                                                       
            ++q;
            for(head = q; *q != '\0'; ++q);                                                                               
            memcpy(sign_time, head, q - head);                                                                            
            printf("sign_time:%s\n", sign_time);     

            #if 1
            extern char org_time[15];
            char time_format[] = "%Y%m%d%H%M%S";
            int t_ret = 0;
            printf("org_time:%s\n", org_time);
            if(strlen(org_time) == 0) {
                strncpy(org_time, sign_time, (int)strlen(sign_time));
               // *flag = 0;
            } else {
                t_ret = time_cmp_format(sign_time, org_time, time_format);
                printf("t_ret:%d\n", t_ret);
                if(t_ret == 0) {
                    //*flag = 1;
                    printf("1111111111111111111111\n");
                } else if(t_ret > 0) {
                    //*flag = 0;
                    strncpy(org_time, sign_time, (int)strlen(sign_time));
                    printf("2222222222222222222222\n");
                } else if(t_ret < 0) {
                    printf("3333333333333333333333\n");
                    strncpy(sign_time, org_time, (int)strlen(org_time));
                    sprintf(name, "auth-%s-%s", sign_len, sign_time); 
                }
            } 
            //printf("************flag:%d\n", *flag);     
            #endif                                                                
            //break;
            //strncpy(org_time, sign_time, (int)strlen(sign_time));
            count ++;
		}
    }
   /*     int i = 0;
        char time_format[] = "%Y%m%d%H%M%S";                                                                  
        int t_ret = 0;
        for(i = 0; i < count; ++i) {
            get_sign_info(name, sign_len, sign_time, flag);
            t_ret = time_cmp_format(sign_time, org_time, time_format); 
            if(t_ret > 0) {
                strncpy(org_time, sign_time, (int)strlen(sign_time));
            }
        break;
        }
    printf("org_time:%s\n", org_time);
    printf("..............count:%d\n", count); */
	closedir(dir);
	return 0;
}
#if 0
int get_sign_len(char *name, char *sig_len)
{	
	char *head = NULL;
	char *p = NULL;
	int length = 0;
	//end += (int)strlen(name);
	for(head = name, p = name; *p != '-'; ++p);
	++p;
	for(head = p; *p != '-'; ++p); 
	memcpy(sig_len, head, p - head);
	length = (int)strlen(sig_len);
	printf("sign_length:%d\n", length);
	sig_len[length] = '\0';
		
	return 0;
}
#endif
int split_sign_name(char *name, char *sig_len, char *sign_time)
{	
	char *head = NULL;
	char *p = NULL;
	int length = 0;
	for(head = name, p = name; *p != '-'; ++p);
	++p;
	for(head = p; *p != '-'; ++p); 
	memcpy(sig_len, head, p - head);
	length = (int)strlen(sig_len);
	printf("sign_length:%d\n", length);
	sig_len[length] = '\0';
    ++p;
    for(head = p; *p != '\0'; ++p);
    memcpy(sign_time, head, p - head);
	printf("sign_time:%s\n", sign_time);
	return 0;
}

#if 0
int get_last_file(char *sign_len, char *sign_buffer)
{
    int fd;
    int size;
    fd = open( PATH, O_RDONLY);
    printf("fd:%d\n", fd);
    size = lseek(fd, (-atoi(sign_len)), SEEK_END);
    if(read(fd, sign_buffer, atoi(sign_len)) < 0) {
        printf("read error!\n");
        close(fd);
    } else {
        printf("sign_buffer:%s\n", sign_buffer);
        close(fd);
    }
    return 0;
}
#endif
