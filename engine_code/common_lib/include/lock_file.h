/*
 * =====================================================================================
 *       Filename:  lock_file.h
 *    Description:  lock file interface
 *
 *        Version:  1.0
 *        Created:  2008-10-21 19:16:25
 *         Author:  idp@venus.com
 *        Company:  venustech
 * =====================================================================================
 */

#ifndef _LOCK_FILE_H
#define _LOCK_FILE_H

#include <sys/file.h>
#include <stdio.h>
#include <errno.h>

#define _LOCK_FILE

/*
 * b: block
 * nb: non-block
 * i: file->int
 * p: file->FILE *
 * ret:
 *	suc: 0
 *	locked: 1
 *	fail: -1
 */
#define FILE_LOCKSUC	0
#define FILE_LOCKED	1
#define FILE_LOCK_ERR	-1
/******************
 * file: int
 *****************/
static inline int lockfile_bi(int fd)
{
#ifdef _LOCK_FILE
	int lock_ret = flock(fd, LOCK_EX);
	if (lock_ret == 0) {
		return FILE_LOCKSUC;
	} else if (errno == EWOULDBLOCK) {
		return FILE_LOCKED;
	} else {
		return FILE_LOCK_ERR;
	}
#else
	return FILE_LOCKSUC;
#endif
}

static inline int unlockfile_bi(int fd)
{
#ifdef _LOCK_FILE
	return flock(fd, LOCK_UN);
#else
	return 0;
#endif
}

static inline int lockfile_nbi(int fd)
{
#ifdef _LOCK_FILE
	int lock_ret = flock(fd, LOCK_EX | LOCK_NB);
	if (lock_ret == 0) {
		return FILE_LOCKSUC;
	} else if (errno == EWOULDBLOCK) {
		return FILE_LOCKED;
	} else {
		return FILE_LOCK_ERR;
	}
#else
	return FILE_LOCKSUC;
#endif
}

static inline int unlockfile_nbi(int fd)
{
#ifdef _LOCK_FILE
	return flock(fd, LOCK_UN | LOCK_NB);
#else
	return 0;
#endif 
}

/******************
 * file: point
 *****************/
static inline int lockfile_bp(FILE *fp)
{
#ifdef _LOCK_FILE
	return (fp != NULL ? lockfile_bi(fileno(fp)) : -1);
#else
	return 0;
#endif
}

static inline int unlockfile_bp(FILE *fp)
{
#ifdef _LOCK_FILE
	return (fp != NULL ? unlockfile_bi(fileno(fp)) : -1);
#else
	return 0;
#endif
}

static inline int lockfile_nbp(FILE *fp)
{
#ifdef _LOCK_FILE
	return (fp != NULL ? lockfile_nbi(fileno(fp)) : -1);
#else
	return 0;
#endif
}

static inline int unlockfile_nbp(FILE *fp)
{
#ifdef _LOCK_FILE
	return (fp != NULL ? unlockfile_nbi(fileno(fp)) : -1);
#else
	return 0;
#endif
}

/********************
 * open and lock file
 ********************/
static inline FILE *fopen_lock(const char *path, const char *mode)
{	
	FILE *fp = fopen(path, mode);
	if (fp) {
		if (lockfile_bp(fp) == 0) {
			return fp;
		} else {
			fclose(fp);
			return NULL;
		}
	}

	return NULL;
}

static inline int fclose_unlock(FILE *fp)
{
	if (fp != NULL) {
		unlockfile_bp(fp);
		fclose(fp);
	}

	return 0;
}

#endif

