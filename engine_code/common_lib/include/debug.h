#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
/* #include <pthread.h> */

/* ------------ print screen ---------- */
#ifdef DEBUG_DBP
        #define DBP(fmt, args...) fprintf(stderr, fmt, ## args)
#else
        #define DBP(fmt, args...) /* not debugging: nothing */
#endif


/* ---------------- trace file --------- */
#ifdef DEBUG_TRACE
#define DEBUG_TRACE_FILE	"/venus_dt/log/shm_trace.log"
#define DEBUG_TRACE_FILE_SIZE	(512 * 1024)
/* ******* print to file ******** */
#define TRACE(...)							\
	do {								\
		FILE		*fp = NULL;				\
		time_t		 time_now;				\
		char		 stime_now[128];			\
		struct stat	 stat_buf;				\
									\
		stat(DEBUG_TRACE_FILE, &stat_buf);			\
		if (stat_buf.st_size > DEBUG_TRACE_FILE_SIZE) {		\
			unlink(DEBUG_TRACE_FILE);			\
		}							\
									\
		time(&time_now);					\
		ctime_r(&time_now, stime_now);				\
		stime_now[strlen(stime_now) - 1] = 0;			\
									\
		fp = fopen(DEBUG_TRACE_FILE, "a+");			\
		if (fp) {						\
			fprintf(fp, "%s: %s:%s:%d  ", stime_now, __FILE__, __FUNCTION__, __LINE__); \
			fprintf(fp, __VA_ARGS__);			\
			fprintf(fp, "\n");				\
			fflush(fp);					\
			fclose(fp);					\
		}							\
	} while(0); 

/* *********** print binary to file *********** */
#define PRINT_HEX				\
        do {					\
        } while(0);		
#else  /* DEBUG_TRACE */
#define TRACE(...)
#define PRINT_HEX 
#endif	/* DEBUG_TRACE */


#endif /* _DEBUG_H_ */
