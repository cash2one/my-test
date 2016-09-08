#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include"errno.h"
#if 1

/***************
0 表示入参是目录
1 表示入参是文件
***************/ 
int is_dir(char * filename)
{
        struct stat buf;
        int ret = stat(filename,&buf);
        if(0 == ret) {
                if(buf.st_mode & S_IFDIR) {
                        printf("%s is folder\n",filename);
                        return 0;
                } else {
                        printf("%s is file\n",filename);
                        return 1;
                }
        }
        return -1;
}
#endif

int delete_dir(char * dirname)
{
        char chBuf[256];
        DIR * dir = NULL;
        struct dirent *ptr;
        int ret = 0, retn = 0;
        dir = opendir(dirname);
        if(NULL == dir) {
                return -1;
        }
        while((ptr = readdir(dir)) != NULL) {
                if(strcmp(ptr->d_name, "..") == 0 || strcmp(ptr->d_name, ".") == 0) {
                        continue;
                } else {
                        snprintf(chBuf, 256, "%s/%s", dirname, ptr->d_name);
                        printf("chBuf:%s\n", chBuf);
                        ret = is_dir(chBuf);                                                                      
                        if(0 == ret) {                                                                            
                                printf("%s is dir\n", chBuf);                                                     
                                retn = delete_dir(chBuf);                                                          
                                if(0 != retn) {                                                                    
                                        printf("fail delete!\n"); 
                                }
                                //rmdir(chBuf);                                                                                 
                        } else if(1 == ret) {                                                                       
                                printf("%s is file\n", chBuf);                                                    
                                retn = remove(chBuf);                                                              
                                if(0 != retn) {                                                                    
                                        return -1;                                                                
                                }                                                                                 
                        }                                                                                         
                }                                                                                                 
        }                                        
	    closedir(dir);  
#if 0                                                                                          
        ret = remove(dirname);                                                                                    
        if(0 != ret) {                                                                                            
                return -1;                                                                                        
        }  
#endif                                                                                                       
        return 0;                                                                                                 
}

int sys_cmd(char *cmd)
{
	int ret = 0;
	ret = system(cmd);
	if(ret < 0) {
                printf("error:%s\n", strerror(errno));
        } else if(WIFSIGNALED(ret)) {
                printf("abnormal termination, signal number = %d\n", WTERMSIG(ret));
        } else if(WIFSTOPPED(ret)) {
                printf("process stopped, signal number = %d\n", WSTOPSIG(ret));
        } else if(WIFEXITED(ret)) {
                printf("normal termination, exit status = %d\n", WEXITSTATUS(ret));
        }
	return 0;
}                   
