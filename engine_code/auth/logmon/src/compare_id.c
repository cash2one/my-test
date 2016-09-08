#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include "authfile_info.h"
#include"genkey.h"
#include"logmon.h"
#include"auth_cfg.h"

int is_exist(char *name, char *str, int *len, int *retn)
{
    struct dirent *ptr;
    DIR *dir;
    dir = opendir(AUTH_CHECK_RET_PATH);
    while((ptr = readdir(dir)) !=NULL) {
        if(strcmp(".", ptr->d_name) == 0 ||strcmp("..", ptr->d_name) == 0)
        continue;
        char *p = NULL;
        int length = strlen(ptr->d_name);
        for(p = ptr->d_name; *p !='.'; ++p);
        char buf[(*len) + 1];
        memset(buf, 0, (*len) + 1);
        strncpy(buf, p, (*len));
        int ret = strcmp(buf, str);
        if(ret == 0) {
            memset(name, 0, length - 3);                                                                             
            printf("buf_ret:%d\n", ret);
            strncpy(name, ptr->d_name, length - 4);
            printf("name:%s\n", name); 
            *retn = 0;
            break;
        } else {
            *retn = -1;
        }
    }
    closedir(dir);
    
    return 0;
}

int get_authcheck_ret(int *retn, char *info)
{
        char device_id[DEVICE_ID_LEN];
	    memset(device_id, 0, DEVICE_ID_LEN);
        genkey(device_id, DEVICE_ID_LEN);
       // int ret = 0;
        printf("***********device_id***********88:%s\n",device_id);  
        //ret = is_file_exist(auth_conf_cfg.xml_path); 
        //if(ret == 1) {
        get_file_info(&file_info, info);

        printf("222:%s\n", file_info.serial_no);
	
	if(strcmp(device_id, file_info.serial_no) == 0) {
		*retn = 0;
	} else {
		*retn = -1;
	}
        printf("retn:%d\n", *retn);

        FILE *fp1, *fp2;
        char eng_path[MAX_BUF_LEN];
        char web_path[MAX_BUF_LEN];
        memset(eng_path, 0, MAX_BUF_LEN);
        memset(web_path, 0, MAX_BUF_LEN);
        snprintf(eng_path, MAX_BUF_LEN, "%s%d.eng", auth_conf_cfg.auth_check_path, *retn);
        snprintf(web_path, MAX_BUF_LEN, "%s%d.web", auth_conf_cfg.auth_check_path, *retn);
        //printf("jjjjjjpath:%s\n", path);
#if 0
	int ret;
	fd = creat(path, 0755);
	if(ret == -1) {
		printf("fail to creat!\n");
		exit(1);
	} else {
		printf("creat success!\n");
	}
#endif 
    char old_eng_path[MAX_BUF_LEN];
    char old_web_path[MAX_BUF_LEN];
    memset(old_eng_path, 0, MAX_BUF_LEN);
    memset(old_web_path, 0, MAX_BUF_LEN);    
    char str1[] = {".eng"};
    char str2[] = {".web"};
    char name1[2] = {0};
    char name2[3] = {0};
    int t_ret1 = 0;
    int t_ret2 = 0;
    int len = 4;
    is_exist(name1, str1, &len, &t_ret1);   
    is_exist(name2, str2, &len, &t_ret2);
    printf("--------------t_ret1:%d\n", t_ret1);
    printf("--------------t_ret2:%d\n", t_ret2);
    if(t_ret1 == -1) {
	    if((fp1 = fopen(eng_path, "w")) == NULL){

                printf("open [%s] error! \n", eng_path);
	//	fclose(fp1);
                return -1;
        } else {
		    fclose(fp1);
	    }
    } else if(t_ret1 == 0) {
        printf("name1:%s\n", name1);
        snprintf(old_eng_path, MAX_BUF_LEN, "%s%s.eng", auth_conf_cfg.auth_check_path, name1);
        rename(old_eng_path, eng_path);
    }
    if(t_ret2 == -1) {
        if((fp2 = fopen(web_path, "w")) == NULL) {                                                                     

                printf("open [%s] error! \n", web_path);                                                                   
      //  fclose(fp2);                                                                                              
                return -1;                                                                                        
        } else {   
            fclose(fp2);
        }
    } else if(t_ret2 == 0) {
        snprintf(old_web_path, MAX_BUF_LEN, "%s%s.web", auth_conf_cfg.auth_check_path, name2);
        rename(old_web_path, web_path);
    }
//}
        return 0;
}
