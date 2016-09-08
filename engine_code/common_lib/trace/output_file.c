#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "trace_api.h"
#include "output_action.h"
#include "misc.h"

extern trace_info_t trace_info;

/*extern int thread_lock(void);
extern int thread_unlock(void);*/

/*bzero( truncate ) file*/
static void zero_file(FILE *fp)
{
	int errno_save;
	
	errno_save=errno;

	if ( !fp ) {
		goto ret;
	}
	
	if ( ftruncate(fp->_fileno,0) ) {
		static int i=0;
		i++;
		if( i < 5 ) {
			fprintf(stderr,"truncate file error,errno:%d(%s)\n",
						errno,strerror(errno));
		}
		goto ret;
	} else {
		rewind(fp);
	}
	
ret:
	errno=errno_save;
	return;
}


/*
	check file size
*/
static int check_filefp_size(FILE *fp)
{
	int max_file_size = 0;
	
	struct stat file_stat;
	
	if ( !fp ) {
		goto err;
	}

	//bzero(&file_stat, sizeof(file_stat));

	if ( trace_info.debug_fp && fp->_fileno == trace_info.debug_fp->_fileno ) {	
		max_file_size = trace_info.max_debug_filesz;
	} else if ( trace_info.error_fp && fp->_fileno == trace_info.error_fp->_fileno ) {
		max_file_size = trace_info.max_error_filesz;
	} else if ( trace_info.log_fp && fp->_fileno == trace_info.log_fp->_fileno ) {
		max_file_size = trace_info.max_log_filesz;
	} else if ( trace_info.fatal_fp && fp->_fileno == trace_info.fatal_fp->_fileno ) {
		max_file_size = trace_info.max_fatal_filesz;
	} else if ( trace_info.warning_fp && fp->_fileno == trace_info.warning_fp->_fileno ) {
		max_file_size = trace_info.max_warning_filesz;
	} else {
		// it seems impossible ??!!
		static int i = 0;
		
		if (i++ < 5 ) {
			fprintf(stderr,"trace file fp invalid !!!\n");
		}
		
		goto err;
	}


	if(!fstat(fp->_fileno, &file_stat)) {
		if ( file_stat.st_size >= max_file_size ) {
			zero_file(fp);
		}
	}

	return 0;
err:
	return -1;
}

/*
	check the number of file line
*/
static int check_file_line(FILE *fp)
{
	int level_line_idx = 0;
	int max_file_line = 0;
	
	if ( NULL == fp ) {
		goto ret;
	}

	if ( trace_info.debug_fp && fp->_fileno == trace_info.debug_fp->_fileno ) {	
		level_line_idx = TRACE_FILE_ID_DEBUG;
		max_file_line = trace_info.max_debug_fileline;
	} else if ( trace_info.error_fp && fp->_fileno == trace_info.error_fp->_fileno ) {
		level_line_idx = TRACE_FILE_ID_ERROR;
		max_file_line = trace_info.max_error_fileline;
	} else if ( trace_info.log_fp && fp->_fileno == trace_info.log_fp->_fileno ) {
		level_line_idx = TRACE_FILE_ID_LOG;
		max_file_line = trace_info.max_log_fileline;
	} else if ( trace_info.fatal_fp && fp->_fileno == trace_info.fatal_fp->_fileno ) {
		level_line_idx = TRACE_FILE_ID_FATAL;
		max_file_line = trace_info.max_fatal_fileline;
	} else if ( trace_info.warning_fp && fp->_fileno == trace_info.warning_fp->_fileno ) {
		level_line_idx = TRACE_FILE_ID_WARNING;
		max_file_line = trace_info.max_warning_fileline;
	} else {
		/* direct dump debug info,not check file size*/
		goto err;
	}


	if(trace_info.file_info[level_line_idx].current_line >trace_info.file_info[level_line_idx].max_line) {
		zero_file(fp);
		trace_info.file_info[level_line_idx].current_line=0;
	}

	trace_info.file_info[level_line_idx].current_line++;

ret:
	return 0;
err:
	return -1;
}


/* init the function pointer used to truncate */
static int check_hook_init(trace_info_t *info)
{
#define TRUNCATE_BYSIZE_STR		"byfilesizeMbytes"
#define TRUNCATE_BYLINE_STR		"byfilelinenum"
	if (!info) {
		goto err;
	}

	if ( !strcasecmp(info->trace_check_mode_str , TRUNCATE_BYSIZE_STR) ) {
		info->fn = check_filefp_size;
		int i=0;

		for(i = 0 ; i < TRACE_FILE_ID_BORDER; i++) {
			trace_info.file_info[i].max_size=
					trace_info.file_info[i].max_size*1024*1024;
		}
	} else if ( !strcasecmp(info->trace_check_mode_str, TRUNCATE_BYLINE_STR) ) {
		info->fn = check_file_line;
	} else {
		info->fn = NULL;
		fprintf(stderr, "Invalid TRACE check mode:%s\n", info->trace_check_mode_str);
		goto err;
	}

	return 0;
err:
	return -1;
}




static void log_top_path_create(void)
{
	int i = 0;

	for (i =0; i<TRACE_FILE_ID_BORDER;i++) {
		create_path(trace_info.file_info[i].fname, 0700);
	}

	return;
}


/* open trace file */
int file_open()
{	
	int i =0;
	int errno_save=0;

	errno_save=errno;
	log_top_path_create();

	for (i =0; i<TRACE_FILE_ID_BORDER;i++) {
		if ( !(trace_info.file_info[i].fp = fopen(trace_info.file_info[i].fname, "a+") ) ) {
			fprintf(stderr, "fopen file:%s failed,errno:%d(%s)\n", 
					trace_info.file_info[i].fname, errno,strerror(errno));
			goto err;
		}
	}

	if(check_hook_init(&trace_info) < 0)
		goto err;


	errno=errno_save;
	return 0;
err:
	errno=errno_save;
	return -1;
}

/*
	dump trace infomation to files
*/
int file_put(FILE *fp, char *msg)
{
	if ( !fp || !msg ) {
		goto ret;
	} 

	int result = 0;
	result = trace_thread_lock();
	if(-1 == result) {
		printf("trace_thread_lock error\n");
		goto ret;
	}
	if(NULL != trace_info.fn)
		trace_info.fn(fp);	/*check the file size and trucate it*/
	fprintf(fp, "%s", msg);
	fflush(fp);
	if(-1 == trace_thread_unlock()) {
		printf("trace_thread_unlock error\n");
		goto ret;
	}
ret:
	return 0;
}

int file_close(void)
{
	int i = 0;
	
	for ( i = 0; i < TRACE_FILE_ID_BORDER ;i ++) {
		if (trace_info.file_info[i].fp) {
			fflush(trace_info.file_info[i].fp);
			fclose(trace_info.file_info[i].fp);
			trace_info.file_info[i].fp = NULL;
			trace_info.file_info[i].fname[0] = '\0';
			//bzero(&trace_info.file_info[i].fname,sizeof(trace_info.file_info[i].fname));
		}
	}

	return 0;
}
