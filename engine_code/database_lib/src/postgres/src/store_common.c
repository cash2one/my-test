#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "store_common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>



/* -------------------------------------------*/
/**
 * @brief  读取指定文件到指定buf中，要求buf足够大
 *
 * @param src_path	文件路径
 * @param dst_data	得到的文件buf
 *
 * @returns   
 *			>=0 得到的buf长度
 *			-1 失败
 */
/* -------------------------------------------*/
int read_file_to_data(char *src_path, void *dst_data)
{
	int fd;
	char *tmp_buf = (char *)dst_data;
	int succ_read_size = 0;
	int succ_read = 0;
	int file_size;

	fd = open(src_path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open %s error\n", src_path);
		return -1;
	}


	file_size = lseek(fd, 0, SEEK_END);
	if (file_size == -1) {
		printf("lseek error\n");
		return -1;
	}

	lseek(fd, 0, SEEK_SET);


	while (file_size != 0) {
		succ_read = read(fd, (void *)tmp_buf, 4096);
		if (succ_read == -1) {
			fprintf(stderr, "read error\n");
			close(fd);
			return -1;
		}
		tmp_buf += succ_read;
		file_size -= succ_read;
		succ_read_size += succ_read;
	}

	close(fd);
	return succ_read_size;
}

int read_file_to_data_m(char *src_path, void **dst_data)
{
	int fd;
	char *tmp_buf = NULL;
	int succ_read_size = 0;
	int succ_read = 0;
	int file_size;

	fd = open(src_path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open %s error\n", src_path);
		return -1;
	}

	file_size = lseek(fd, 0, SEEK_END);
	if (file_size == -1) {
		printf("lseek error\n");
		return -1;
	}
	*dst_data=(void *)malloc((file_size+1)*sizeof(char));
	memset(*dst_data,0,file_size);
	tmp_buf=(*dst_data);	
	lseek(fd, 0, SEEK_SET);

	while (file_size != 0) {
		succ_read = read(fd, (void *)tmp_buf, 4096);
		if (succ_read == -1) {
			fprintf(stderr, "read error\n");
			close(fd);
			return -1;
		}
		tmp_buf += succ_read;
		file_size -= succ_read;
		succ_read_size += succ_read;
	}

	close(fd);
	return succ_read_size;
}
/* -------------------------------------------*/
/**
 * @brief  将指定buf中数据内容写到文件中去
 *
 * @param src_data	数据源buf
 * @param data_len	数据长度
 * @param dst_path	目标文件 如果没有则创建
 *
 * @returns   
 *			>=0 写进文件的长度
 *			-1  失败
 */
/* -------------------------------------------*/
int write_data_to_file(void *src_data, unsigned int data_len, char *dst_path)
{
	int fd;
	int succ_write = 0;
	char *tmp_buf = (char *)src_data;
	unsigned int succ_write_size = 0;

	fd = open(dst_path, O_RDWR| O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if (fd == -1) {
		fprintf(stderr, "open %s error\n", dst_path);
		return -1;
	}

	while (data_len > 4096) {
		succ_write = write(fd, (void *)tmp_buf, 4096);
		if (succ_write == -1) {
			printf("%s\n",strerror(errno));
			fprintf(stderr, "write error\n");
			close(fd);
			return -1;
		}
		data_len -= succ_write;
		succ_write_size += succ_write;
		tmp_buf += succ_write;
	}

	succ_write = write(fd, tmp_buf, data_len);
	if (succ_write == -1) {
		printf("%s\n",strerror(errno));
		fprintf(stderr, "write error\n");
		close(fd);
		return -1;
	}
	succ_write_size += succ_write;

	close(fd);	
	return succ_write_size;
}

/* -------------------------------------------*/
/**
 * @brief  将时间格式“2013-11-03 06:24:59”类似
 *			时间字符串 拆分出每个单位的字符串
 *			如果不想得到的时间单位 可以传NULL
 *
 * @param time_str	原完整时间字符串
 * @param year		得到的年
 * @param month		得到的月
 * @param day		得到的天
 * @param hour		得到的小时
 * @param minute	得到的分钟
 * @param second	得到的秒
 */
/* -------------------------------------------*/
void split_time_str(char *time_str, 
					char *year, 
					char *month, 
					char *day, 
					char *hour, 
					char *minute, 
					char *second)
{
	char *p = NULL;
	char *head = NULL;

	if (time_str == NULL) {
		return;
	}

	/* year */
	for (head = time_str, p = time_str; *p != '-'; ++p);
	if (year != NULL) {
		memcpy(year, head, p - head);	
		year[4] = '\0';
	}

	++p;

	/* month */
	for (head = p; *p != '-'; ++p);
	if (month != NULL) {
		memcpy(month, head, p - head);
		month[2] = '\0';
	}

	++p;

	/* day */
	for(head = p; *p != ' '; ++p);
	if (day != NULL) {
		memcpy(day, head, p - head);
		day[2] = '\0';
	}

	++p;

	/* hour */
	for (head = p; *p != ':'; ++p);
	if (hour != NULL) {
		memcpy(hour, head, p - head);
		hour[2] = '\0';
	}

	++p;

	/* minute */
	for (head = p; *p != ':'; ++p);
	if (minute != NULL) {
		memcpy(minute, head, p - head);
		minute[2] = '\0';
	}

	++p;

	/* second */
	for (head = p; *p != '\0'; ++p);
	if (second != NULL) {
		memcpy(second, head, p - head);
		second[2] = '\0';
	}
}

/* -------------------------------------------*/
/**
 * @brief  根据当前系统时间 返回第二天零晨的
 *			日历时间
 *
 * @returns   
 *			第二天凌晨日历时间
 */
/* -------------------------------------------*/
time_t get_next_zero_time(void)
{
	struct tm* time_p = NULL;
	time_t now_timestamp, next_day_timestamp;
	int one_day_sec = 3600 * 24;

	time(&now_timestamp);
	time_p = localtime(&now_timestamp);	
	time_p->tm_sec = time_p->tm_min = time_p->tm_hour = 0;
	next_day_timestamp = mktime(time_p);	
	next_day_timestamp += one_day_sec;

	return next_day_timestamp;
}
/* ________________________________________ */
/**
 * @brief 根据当前系统时间 返回下周一的凌晨
 * 时间
 * @return 
 * 		  下周一的凌晨时间
 */
/* ----------------------------------- */
time_t get_next_week_zero_time(void)
{
	time_t now_time;
	struct tm this_time;
	int days;
	memset(&this_time, 0, sizeof(struct tm));
	time(&now_time);
	localtime_r(&now_time,&this_time);

	this_time.tm_sec = 0;
	this_time.tm_min = 0;
	this_time.tm_hour = 0;
	days = this_time.tm_wday - 1;
	if (days == -1) {
		days = 6;
	}
	return mktime(&this_time)+(7-days)*3600*24;

}



/* -------------------------------------------*/
/**
 * @brief  比较两个时间相差秒数 格式为1999-01-22 11:02:03
 *
 * @param time_str1	
 * @param time_str2
 *
 * @returns   
 *			相差的秒数，>0 time_str1 晚于 time_str2 多少秒
*						<0 time_str1 早于 time_str2 多少秒
 */
/* -------------------------------------------*/
int time_cmp(char *time_str1, char *time_str2)
{
	struct tm time1 = {0};	
	struct tm time2 = {0};	
	char time_format[] = "%Y-%m-%d %X";
	time_t time_sec1, time_sec2;

	strptime(time_str1, time_format, &time1);
	strptime(time_str2, time_format, &time2);

	time_sec1 = mktime(&time1);
	time_sec2 = mktime(&time2);

	return (time_sec1-time_sec2);
}

int time_cmp_format(char *time_str1, char *time_str2, const char *format)
{
	struct tm time1 = {0};	
	struct tm time2 = {0};	
	time_t time_sec1, time_sec2;

	strptime(time_str1, format, &time1);
	strptime(time_str2, format, &time2);

	time_sec1 = mktime(&time1);
	time_sec2 = mktime(&time2);

	return (time_sec1-time_sec2);
}

/* -------------------------------------------*/
/**
 * @brief  通过制定格式的时间字符串返回time_t时间
 *
 * @param time_str
 * @param format
 *
 * @returns   
 */
/* -------------------------------------------*/
time_t get_time_sec_by_str(char *time_str, const char *format)
{
	struct tm time = {0};
	
	strptime(time_str, format, &time);

	return mktime(&time);
}

/*获取当前时间的零点时间*/

char* get_current_zero_time(char *time_str, char *zero_time_str)
{
	time_t time_str_sec;
	char time_format[] = "%Y-%m-%d %X";
	struct tm *time_p = NULL;

	time_str_sec = get_time_sec_by_str(time_str, time_format);
	time_p = localtime(&time_str_sec);
	time_p->tm_sec = time_p->tm_min = time_p->tm_hour = 0;
	strftime(zero_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);

	return zero_time_str;


}
char* get_current_next_zero_time(char *time_str, char *next_zero_time_str)
{
	struct tm* time_p = NULL;
	char time_format[] = "%Y-%m-%d %X";
	time_t time_str_sec,tmp;

	time_str_sec = get_time_sec_by_str(time_str, time_format);
	tmp=time_str_sec+24*60*60;
	time_p = localtime(&tmp);	
	strftime(next_zero_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);
//	printf("time   **********************************%s\n",next_zero_time_str);
	return next_zero_time_str;
}
/* -------------------------------------------*/
/**
 * @brief  获取当前字符串时间 的 偏移秒数的 字符串时间
 *
 * @param out_time_str	
 * @param in_time_str
 * @param offset_sec	正数为将来 负数为过去
 * @param format
 *
 * @returns   
 *		获得的时间字符串 首地址
 */
/* -------------------------------------------*/
char *get_timestr_offset(char *out_time_str, char *in_time_str, int offset_sec, const char *format)
{
	struct tm in_time = {0};
	time_t time_sec;
	struct tm *out_time_p = NULL;

	strptime(in_time_str, format, &in_time);
	time_sec = mktime(&in_time);

	time_sec += offset_sec;
	out_time_p = localtime(&time_sec);

	strftime(out_time_str, TIME_FORMAT_SIZE-1, format, out_time_p);

	return out_time_str;
}

char *get_critical_timestr_offset(char *out_time_str, char *in_time_str, int offset_sec, const char *format)
{
	struct tm in_time = {0};
	time_t time_sec;
	struct tm *out_time_p = NULL;

	strptime(in_time_str, format, &in_time);
	time_sec = mktime(&in_time);

	time_sec += offset_sec;
	out_time_p = localtime(&time_sec);

	out_time_p->tm_sec = 0;
	
	strftime(out_time_str, TIME_FORMAT_SIZE-1, format, out_time_p);

	return out_time_str;
}

/* -------------------------------------------*/
/**
 * @brief  获取当前系统时间 格式为1999-01-22 11:02:03
 *
 * @param now_time_str
 *
 * @returns   
 *		获得的时间字符串 首地址
 */
/* -------------------------------------------*/
char* get_systime_str(char *now_time_str)
{
	time_t now_time;
	char time_format[] = "%Y-%m-%d %X";

	struct tm* now_time_p = NULL;
	struct tm  tmp_time = {0};

	time(&now_time);
	now_time_p = localtime_r(&now_time,&tmp_time);
	strftime(now_time_str, TIME_FORMAT_SIZE-1, time_format, now_time_p);

	return now_time_str;
}

char* get_systime_str_format(char *now_time_str, const char *format)
{
	time_t now_time;

	struct tm* now_time_p = NULL;
	struct tm  tmp_time = {0};

	time(&now_time);
	now_time_p = localtime_r(&now_time,&tmp_time);
	strftime(now_time_str, TIME_FORMAT_SIZE-1, format, now_time_p);

	return now_time_str;
}

char* get_systime_str_format_offset(char *out_time_str, const char *format, int offset_sec)
{
	time_t now_time;

	struct tm* now_time_p = NULL;

	time(&now_time);
    now_time += offset_sec;
	now_time_p = localtime(&now_time);
	strftime(out_time_str, TIME_FORMAT_SIZE-1, format, now_time_p);

	return out_time_str;
}


/* -------------------------------------------*/
/**
 * @brief  以‘/’为分层次级别， 得到对应级别的目录名字
 *		   例如 “/gms/events/ae/2013/11/12”
 *		   0级别为12，1级别为11， 2级别为2013
 *
 * @param src_name	原绝对路径
 * @param name		得到的对应级别的名字
 * @param level		级别
 *
 * @returns   
 *					得到的对应几倍的名字
 */
/* -------------------------------------------*/
char *name_by_level(char *src_name, char *name, char level)
{
	char *last_pos = NULL;	
	char *tok_pos = NULL;
	char *pre_tok_pos = NULL;
	char now_level = 0;

	if (level < 0 || src_name == NULL) {
		fprintf(stderr, "name by level: param error!\n");
		return NULL;
	}

	last_pos = src_name + (strlen(src_name)-1);

	if (level == 0) {
		for (tok_pos = last_pos; *tok_pos != '/'; --tok_pos);
		strncpy(name, tok_pos+1, FILE_NAME_SIZE);
		goto end;
	}
	else {
		for (tok_pos = last_pos, pre_tok_pos = last_pos; tok_pos >= src_name; --tok_pos) {
			if (*tok_pos == '/') {
				if (now_level == level) {
					memcpy(name, tok_pos+1, pre_tok_pos - (tok_pos+1));
					name[pre_tok_pos - (tok_pos+1)] = '\0';
					break;
				}
				else {
					pre_tok_pos = tok_pos;
				}
				++now_level;
			}
		}
	}

end:
	return name;
}


/* -------------------------------------------*/
/**
 * @brief  比较两个整型IP的大小
 *		   定义 192.168.1.2 > 192.168.1.1
 *		   权重自左向右递减
 *
 * @returns   
 *			0   相等
 *			1  ip1 > ip2 
 *			-1 ip1 < ip2 
 */
/* -------------------------------------------*/
int ip_cmp(unsigned int ip1, unsigned int ip2)
{
	int equal = 0;

	/* 第1位 */
	equal = ((unsigned char*)&ip1)[0] - ((unsigned char*)&ip2)[0];	
	if (equal != 0) {
		goto END;
	}

	/* 第2位 */
	equal = ((unsigned char*)&ip1)[1] - ((unsigned char*)&ip2)[1];	
	if (equal != 0) {
		goto END;
	}

	/* 第3位 */
	equal = ((unsigned char*)&ip1)[2] - ((unsigned char*)&ip2)[2];	
	if (equal != 0) {
		goto END;
	}

	/* 第4位 */
	equal = ((unsigned char*)&ip1)[3] - ((unsigned char*)&ip2)[3];	
	if (equal != 0) {
		goto END;
	}

END:
	return equal;
}

/* -------------------------------------------*/
/**
 * @brief  创建一个多层次目录
 *
 * @param path 目录
 *
 * @returns   
 *          -1 错误
 *          0  成功 
 */
/* -------------------------------------------*/
int create_dir_ex(char *path)
{
    char dir_name[ABSOLUT_FILE_PATH_SIZE]; 
    int i = 0;
    int len = 0;
    int retn = 0;

    if (path == NULL || strlen(path) > (ABSOLUT_FILE_PATH_SIZE - 1)) {
        retn = -1;
        goto END;
    }

    strncpy(dir_name, path, ABSOLUT_FILE_PATH_SIZE-1);
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


#ifdef COMMON_TEST

int main(int argc, char *argv[])
{
     int i = 0;
     List list = {0, NULL, NULL};
     char* data = "dataaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

     ProfileStats xxxstats = {0};
     
     PROFILE_VARS;
     PROFILE_START(xxxstats);

     for (i=0; i<100; i++) {
          list_insert(&list, (void*)data);
     }
    
     Node* tmp = list.head;
     while (tmp != NULL) {
          printf("data [%s]\n", (char*)tmp->data);
          tmp = tmp->next;
     }
    
     system("ping 8.8.8.8");
     PROFILE_END(xxxstats);
    
     printf("sec = %f\n", xxxstats.sec);
     printf("usec = %llu\n", xxxstats.usec);
     return 0;
}
#endif

