#include "url_log.h"
#include "common.h"
#include <time.h>
pthread_mutex_t url_file_lock=PTHREAD_MUTEX_INITIALIZER;

Url_str g_url_conf;

static char file_path_top[64]; 
volatile time_t url_file_out_time;
volatile time_t out_file_time;
int url_load_conf(void)
{
	char key[64];
	memset(key,0,64);
	/****读取url输出的路径****/
	tcad_conf_get_profile_string(url_conf_file_path,"url","dst_path",file_path_top);
	if (strlen(file_path_top) < 1)
	{
		strncpy(file_path_top,"/data/tmpdata/engine/url",URL_PATH_LEN);
	}	
	/****读取多久生成一个url文件****/
	tcad_conf_get_profile_string(url_conf_file_path,"url","out_file_time",key);
	out_file_time=atoi(key);
	if(out_file_time < 300 )
	{
		out_file_time = 300;
	}
	return 0;

}
/* -------------------------------------------*/
/**
 *  * @brief  创建一个多层次目录
 *   *
 *    * @param path 目录
 *     *
 *      * @returns   
 *       *          -1 错误
 *        *          0  成功 
 *         */
/* -------------------------------------------*/
int create_dir_ex(char *path)
{
	char dir_name[URL_PATH_LEN]; 
	int i = 0;
	int len = 0;
	int retn = 0;

	if (path == NULL || strlen(path) > (URL_PATH_LEN - 1)) {
		retn = -1;
		goto END;
	}

	strncpy(dir_name, path, URL_PATH_LEN-1);
	len = strlen(dir_name);

	for (i = 1; i < len + 1; ++i) {
		if (dir_name[i] == '/' || dir_name[i] == '\0') {
			dir_name[i] = '\0';
			if (access(dir_name, 0) != 0) {
				if (mkdir(dir_name, 0644) == -1) {
					perror("mkdir dir_name");
					retn = -1;
					goto END;
				}
			}
			dir_name[i] = '/';
		}
	}

END:
	return retn;
}
int get_make_path(void)
{
	time_t now_time;
	struct tm this_time;
	char path[URL_PATH_LEN];
	memset(&this_time, 0, sizeof(struct tm));

	time(&now_time);
	localtime_r(&now_time,&this_time);
	snprintf(path, URL_PATH_LEN, "%s/%04d%02d%02d/", file_path_top, this_time.tm_year + 1900, this_time.tm_mon + 1, this_time.tm_mday);
	
	 if (create_dir_ex(path)<0)
	 {
	 
	 	printf("create dir fail ...");
		return -1;
	 }

	 snprintf(g_url_conf.file_path, URL_PATH_LEN, "%s/%04d%02d%02d%02d%02d00", path, this_time.tm_year + 1900, this_time.tm_mon + 1, this_time.tm_mday,this_time.tm_hour,this_time.tm_min);
	return 0;

}
int create_file_fp(void)
{
	FILE*fp=NULL;
	//创建url输出的目录
	if(get_make_path()<0)
	{
		return -1;
	}

	fp=fopen(g_url_conf.file_path,"w");
	if(NULL == fp)
	{
		perror("fopen()");
		return -1;
	}
	g_url_conf.fp=fp;
		return 0;

}
int url_init(void)
{
	url_load_conf();
	url_file_out_time=time(NULL);
	if (create_file_fp()<0)
	{
		return -1;
	}
	return 0;	

}
/*  
 *      函数名：getFileSizeSystemCall(char * strFileName)     
 *          功能：获取指定文件的大小  
 *              参数：  
 *                      strFileName (char *)：文件名  
 *                          返回值：  
 *                                  size (int)：文件大小  
 *                                   */  
int getFileSizeSystemCall(char * strFileName)    
{   
	struct stat temp;   
	stat(strFileName, &temp);   
	return temp.st_size;   
}   
void add_url_timer(void)
{
	time_t now_time;
	char dst_path[URL_PATH_LEN]={0};
	time(&now_time);
	if((now_time - url_file_out_time) >=out_file_time)
	{
		url_file_out_time=now_time;
		snprintf(dst_path,URL_PATH_LEN,"%s.ok",g_url_conf.file_path);

		pthread_mutex_lock(&url_file_lock);

		fclose(g_url_conf.fp);
		if(getFileSizeSystemCall(g_url_conf.file_path)==0)
		{
			unlink(g_url_conf.file_path);
		}
		else{
			rename(g_url_conf.file_path,dst_path);
		}
		if (create_file_fp()<0)
		{
			pthread_mutex_unlock(&url_file_lock);
			exit(-1);
		}

		pthread_mutex_unlock(&url_file_lock);

	}

}
int write_url_file(char *buf)
{
	pthread_mutex_lock(&url_file_lock);
	if(fwrite(buf, 1,strlen(buf),g_url_conf.fp) != strlen(buf))
	{
	pthread_mutex_unlock(&url_file_lock);
		perror("write()");
		return -1;
	}


	pthread_mutex_unlock(&url_file_lock);
	return 0;

}
void url_end(void)
{
	char dst_path[URL_PATH_LEN]={0};
	snprintf(dst_path,URL_PATH_LEN,"%s.ok",g_url_conf.file_path);

	fclose(g_url_conf.fp);
	if(getFileSizeSystemCall(g_url_conf.file_path)==0)
	{
		unlink(g_url_conf.file_path);
	}
	else{
		rename(g_url_conf.file_path,dst_path);
	}

}
