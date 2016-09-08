/************************************************
 *       Filename: file_op.c
 *    Description: file operation
 *
 *        Created: 2009-01-03 00:30:50
 *         Author: david dengwei@venus.com
 ************************************************/
#include "misc.h"
#include "hint.h"

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <time.h>
#include <fcntl.h>

/*
 * get regular file length
 */
int file_length(const char *path, int *len)
{
	struct stat stats;

	if (unlikely(NULL == path || NULL == len)) {
		merror("file_length: arg error\n");
		return -1;
	}

	memset(&stats, 0, sizeof(stats));
	if (stat(path, &stats) != 0) {
		merror("stat %s: %s", path, strerror(errno));
		return -1;
	}

	if (!S_ISREG(stats.st_mode)) {
		merror("%s isn`t regular file\n", path);
		return -1;
	}

	*len = stats.st_size;

	return 0;
}

int create_path(char *path, const int mode)
{
	if (unlikely(path == NULL || strlen(path) == 0)) {
		return -1;
	}

	int path_len = strlen(path);
	char *write_path = malloc(path_len + 1);
	if (write_path == NULL) {
		return -1;
	}

	strncpy(write_path, path, path_len +1 );

	char *cp = NULL;
	if ((cp = strrchr(write_path, '/')) == NULL) {
		goto ret;
	}

	*cp = '\0';
	if (access(write_path, F_OK) == 0) {
		*cp = '/';
		goto ret;
	}
	*cp = '/';

	for (cp = write_path; *cp == '/'; cp++) ;

	int retVal = 0;
	while ((cp = strchr(cp, '/')) != NULL) {
		*cp = '\0';
		mdump("mkdir: %s\n", write_path);

		if (access(write_path, F_OK) < 0) {
			retVal = mkdir(write_path, mode);
			if (retVal != 0 && errno != EEXIST) {
				merror("mkdir %s: %s\n", write_path, strerror(errno));
				*cp = '/';
				goto err;
			}
		}
		*cp = '/';

		do {
			cp++;
		} while (*cp == '/');
	}

ret:
	if (write_path) {
		free(write_path);
		write_path = NULL;
	}

	return 0;

err:
	if (write_path) {
		free(write_path);
		write_path = NULL;
	}

	return -1;
}


/* If PATH is an existing directory or symbolic link to a directory,
   return nonzero, else 0.  */
int isdir(const char *path)
{
	struct stat stats;

	if (unlikely(NULL == path)) {
		merror("isdir: arg error\n");
		return -1;
	}

	memset(&stats, 0, sizeof(stats));
	return (stat(path, &stats) == 0 && S_ISDIR(stats.st_mode));
}

int isfile(const char *path)
{
	struct stat stats;

	if (unlikely(NULL == path)) {
		merror("isdir: arg error\n");
		return -1;
	}

	memset(&stats, 0, sizeof(stats));
	return (stat(path, &stats) == 0 && S_ISREG(stats.st_mode));
}

/*
 * ¼ì²édirµÄÊ£Óà¿Õ¼ä
 */
int check_space(const char *dir, unsigned int *free_space)
{
	struct statfs sfs;

	if (unlikely(NULL == dir || NULL == free_space)) {
		merror("check_space: arg error\n");
		return -1;
	}

	memset(&sfs, 0, sizeof(sfs));
	if (statfs(dir, &sfs) != 0) {
		merror("statfs: %s", strerror(errno));
		return -1;
	}

	*free_space = sfs.f_bfree * sfs.f_bsize;
	return 0;
}

int make_tmpfile(const char *dir, const char *prefix, char *path, const int maxlen)
{
	if (unlikely(access(dir, W_OK) < 0)) {
		mkdir(dir, 0777);
	}

	snprintf(path, maxlen, "%s/.%stmpXXXXXX", dir, prefix);

	int fd = mkstemp(path);
	if (fd >= 0) {
		close(fd);
	} else {
		merror("mkstemp error");
		srandom(time(NULL));
		snprintf(path, maxlen, "%s/.%stmp%lX", dir, prefix, (random() & 0xFFFFFF));
		fd = open(path, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);
		if (fd < 0) {
			merror("open path %s error", path);
			goto err;
		}
	}

	return (strlen(path));

err:
	return -1;
}

int make_tmpdir(const char *dir, const char *prefix, char *path, const int maxlen)
{
	if (unlikely(access(dir, W_OK) < 0)) {
		mkdir(dir, 0700);
	}

	srandom(time(NULL));
	snprintf(path, maxlen, "%s/.%stmp%lX/", dir, prefix, (random() & 0xFFFFFF));
	if (mkdir(path, 0700) < 0) {
		return -1;
	} else {
		return (strlen(path));
	}
}

int check_file_size(char *file_path, int max_size)
{
	if (unlikely(NULL == file_path 
			|| '\0' == file_path[0] 
			|| max_size <= 0)) {
		merror("argument invalid !!\n");
		goto err;
	}

	struct stat file_stat;
	memset(&file_stat, 0, sizeof(struct stat));

	if (stat(file_path, &file_stat) < 0) {
		merror("stat file %s error,errno:%d(%s)\n", file_path, errno, strerror(errno));
		goto err;
	}

	/* file size not out of bound */
	if (file_stat.st_size < max_size) {
		goto lt;
	}

	/* truncate the file to zero */
	if (-1 == truncate(file_path, 0)) {
		merror("truncate file %s error,errno:%d(%s)\n", file_path, errno, strerror(errno));
		goto err;
	}

	// truncate file
	return 1;
lt:
	return 0;
err:
	return -1;
}

int check_file_size_fp(FILE *fp, int max_size)
{
	if (unlikely(fp == NULL 
			|| max_size <= 0)) {
		merror("argument invalid !!\n");
		goto err;
	}

	struct stat file_stat;
	memset(&file_stat, 0, sizeof(struct stat));

	int fd = fileno(fp);
	if (fstat(fd, &file_stat) < 0) {
		merror("stat file %s error,errno:%d(%s)\n", file_path, errno, strerror(errno));
		goto err;
	}

	/* file size not out of bound */
	if (file_stat.st_size < max_size) {
		goto lt;
	}

	/* truncate the file to zero */
	if (-1 == ftruncate(fd, 0)) {
		merror("truncate file %s error,errno:%d(%s)\n", file_path, errno, strerror(errno));
		goto err;
	}

	// truncate file
	return 1;
lt:
	return 0;
err:
	return -1;
}

/*
 * copy regular file
 */
int copy_file_mode(char *src, char *dst, char *mode)
{
	struct stat stats;
	FILE *rfp = NULL, *wfp = NULL;

	if (unlikely(NULL == src || NULL == dst)) {
		return -1;
	}

	if (strcmp(src, dst) == 0) {
		return 0;
	}
	memset(&stats, 0, sizeof(stats));
	if (stat(src, &stats) != 0) {
		merror("stat %s: %s", src, strerror(errno));
		goto err;
	}
	if (!S_ISREG(stats.st_mode)) {
		merror("%s isn`t regular file\n", src);
		goto err;
	}
	if (create_path(dst, 0700) != 0) {
		merror("copy_file: creat_path failed\n");
		goto err;
	}
	rfp = fopen(src, "r");
	if (NULL == rfp) {
		merror("fopen %s: %s", src, strerror(errno));
		goto err;
	}

	wfp = fopen(dst, mode);
	if (NULL == wfp) {
		merror("fopen %s: %s", src, strerror(errno));
		goto err;
	}

#define BUF_LEN 4096
	char buf[BUF_LEN] = {0};
	unsigned int readlen = 0;

	while ((readlen = fread(buf, 1, BUF_LEN, rfp)) != 0) {
		if (fwrite(buf, 1, readlen, wfp) < readlen) {
			goto err;
		}
	}

	if (!feof(rfp)) {
		merror("fread %s error\n", src);
		goto err;
	}

	fclose(rfp);
	// dst file closing must be successful.
	if (fclose(wfp) != 0) {
		merror("fclose %s error\n", dst);
		wfp = NULL;
		goto err;
	}

	if (chmod(dst, stats.st_mode) != 0) {
		merror("chmod %s: %s\n", dst, strerror(errno));
	}

	return 0;

err:
	if (rfp != NULL) {
		fclose(rfp);
	}

	if (wfp != NULL) {
		fclose(wfp);
	}

	// leave nothing if copy failed.
	unlink(dst);
	return -1;
}

/*
 * delete regular file
 */
int delete_file(const char *path)
{
	struct stat stats;

	if (unlikely(NULL == path)) {
		merror("delete_file: arg error\n");
		return -1;
	}

	memset(&stats, 0, sizeof(stats));
	if (stat(path, &stats) != 0) {
		merror("stat %s: %s", path, strerror(errno));
		return -1;
	}

	if (S_ISREG(stats.st_mode)) {
		if (unlink(path) < 0) {
			merror("unlink %s: %s", path, strerror(errno));
			return -1;
		}
	} else if (S_ISDIR(stats.st_mode)) {
		if (rmdir(path) < 0) {
			merror("rmdir %s: %s", path, strerror(errno));
			return -1;
		}
	} else {
		merror("%s isn`t regular file or directory\n", path);
	}

	return 0;
}

