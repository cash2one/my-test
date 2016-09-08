#ifndef _MODULE_DEBUG_H
#define _MODULE_DEBUG_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include <ctype.h>	// for isdigit
#include <sys/types.h>	// for u_int32_t
#include <unistd.h>

#include "misc.h"

#define LOG_FILE "./debug.log"

enum {
	DEBUG_LEVEL_NULL,
	DEBUG_LEVEL_LOG,
	DEBUG_LEVEL_WARN,
	DEBUG_LEVEL_ERROR,
};

//#define DEBUG_LOG
//#define DEBUG_ERR_FILE
//#define DEBUG_WARN_FILE

#define FULL_FORMAT

#define MAX_FILE_SIZE 0x200000	// 2M

static inline void print_longformat(FILE * out_file, const char *file_path, const char *funcname, int line)
{
#ifdef FULL_FORMAT
	fprintf(out_file, "%s(func %s line %d)>> ", file_path, funcname, line);
	return;
#else
	return;
#endif
}

static inline FILE *open_log_file()
{
	if (access(LOG_FILE, F_OK) == 0) {
		check_file_size(LOG_FILE, MAX_FILE_SIZE);
	}

	FILE *file = fopen(LOG_FILE, "a+");
	if (file == NULL) {
		fprintf(stderr, "open %s error\n", LOG_FILE);
		goto open_err;
	}

	return file;

open_err:
	return NULL;
}

static inline FILE *get_log_file_handle()
{
#if defined(DEBUG_LOG)
	return stdout;
#elif defined(DEBUG_LOG_FILE)
	return open_log_file();
#else
	return NULL;
#endif
}

static inline void close_log_file_handle(FILE *file)
{
#if defined(DEBUG_LOG)
	fflush(file);
#elif defined(DEBUG_LOG_FILE)
	if (file) {
		fclose(file);
	}
#endif
}

static inline FILE *get_err_file_handle()
{
#if defined(DEBUG_ERR)
	return stderr;
#elif defined(DEBUG_ERR_FILE)
	return open_log_file();
#else
	return NULL;
#endif
}

static inline void close_err_file_handle(FILE *file)
{
#if defined(DEBUG_ERR)
	fflush(file);
#elif defined(DEBUG_ERR_FILE)
	if (file) {
		fclose(file);
	}
#endif
}

static inline FILE *get_warn_file_handle()
{
#if defined(DEBUG_WARN)
	return stdout;
#elif defined(DEBUG_WARN_FILE)
	return open_log_file();
#else
	return NULL;
#endif
}

static inline void close_warn_file_handle(FILE *file)
{
#if defined(DEBUG_WARN)
	fflush(file);
#elif defined(DEBUG_WARN_FILE)
	if (file) {
		fclose(file);
	}
#endif
}
#ifdef __cplusplus
extern "C"{
#endif

#define mprint(DEBUG_LEVEL, DEBUG_LEVEL_STR, FILE_FUNC, CLOSE_FUNC, ...) \
	{ \
		FILE *out_dir = FILE_FUNC(); \
		if (out_dir != NULL) { \
			fprintf(out_dir, "%s", DEBUG_LEVEL_STR); \
			if (DEBUG_LEVEL > DEBUG_LEVEL_LOG) { \
				print_longformat(out_dir, __FILE__, __FUNCTION__, __LINE__); \
			} \
			fprintf(out_dir, __VA_ARGS__);\
			CLOSE_FUNC(out_dir); \
		} \
	}

//	mprint(DEBUG_LEVEL_LOG, "[--log--] ", get_log_file_handle, __VA_ARGS__);
// print normal message
#if defined(DEBUG_LOG) || defined(DEBUG_LOG_FILE)
#define mdump(...) \
	mprint(DEBUG_LEVEL_LOG, "", get_log_file_handle, close_log_file_handle, __VA_ARGS__);
#else
#define mdump(...)
#endif

// print warning message
#if defined(DEBUG_WARN) || defined(DEBUG_WARN_FILE)
#define mwarn(...)\
	mprint(DEBUG_LEVEL_WARN, "[**WARN**] ", get_warn_file_handle, close_warn_file_handle, __VA_ARGS__);
#else
#define mwarn(...)
#endif

// print error message
#if defined(DEBUG_ERR) || defined(DEBUG_ERR_FILE)
#define merror(...)\
	mprint(DEBUG_LEVEL_ERROR, "[**ERROR**] ", get_err_file_handle, close_err_file_handle, __VA_ARGS__);
#else
#define merror(...)
#endif

#ifndef massert
#define massert(s) \
	if (unlikely(s)) { \
		goto err; \
	}
#endif
#ifdef __cplusplus
}
#endif

#endif				/* _MODULE_DEBUG_H */
