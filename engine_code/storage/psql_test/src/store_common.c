#include <stdio.h>
#include <stdlib.h>

#include "store_common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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
		perror("123\n");
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

