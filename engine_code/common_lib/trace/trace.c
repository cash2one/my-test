/*
	* trace: the debug interface to record prog running information
	* Author:jiaoyf
	* mail:mail2walker@gmail.com
	  

	History:
	* v0.1: 2006.10.1-2006.10.7
	* v0.2: 2006.11.27 simplify some codes to display __FILE__, etc
	* v0.3: 2007.11.29 add some codes to enable specified module trace,
			   and display the module name owned by trace line			


*/
#include <time.h>
#include <errno.h>
#include <stdarg.h>

#include "trace_api.h"
#include "misc.h"
#include "config.h"
#include "output_action.h"

trace_info_t trace_info;
pthread_mutex_t mutex;

#define TRACE_LEVEL	0
#define TRACE_OUTPUT	1
#define TRACE_MODULE	2

/* default trace file path*/
#define DEFAULT_DEBUG_FILE_PATH		"./log/debug.log"
#define DEFAULT_ERROR_FILE_PATH 	"./log/error.log"
#define DEFAULT_LOG_FILE_PATH		"./log/log.log"
#define DEFAULT_FATAL_FILE_PATH		"./log/fatal.log"
#define DEFAULT_WARNING_FILE_PATH	"./log/warning.log"

/*default trace log path*/
#define SERV_PATH 	"./ccom_path"
#define RUNLOG_SERV     "./runlog_serv"
#define AUDITLOG_SERV   "./auditlog_serv"

#define TRACE_CONF_PRINT
#undef  TRACE_CONF_PRINT

/* trace configure end*/

static cfg_desc traceconf_cdt[] = {
	{"Trace", "TraceLevel", (trace_info.trace_level_str), 
				STD_BUF, CFG_TYPE_STR,1, 0, "none"},
	{"Trace", "TraceOutput", (trace_info.trace_output_str), 
				STD_BUF,CFG_TYPE_STR,1, 0, "none"},
	{"Trace", "Modules2Trace", (trace_info.trace_module_str), 
				STD_BUF,CFG_TYPE_STR,1, 0, "none"},
	{"Trace", "TraceSizeCheck", (trace_info.trace_check_mode_str), 
				MAX_FILESZ_STR,CFG_TYPE_STR,1, 0, ""},
	/*{"Trace", "TraceUdpSocket", &(trace_info.enable_trace_udpsock), 
				CFG_SIZE_YN_INT32,CFG_TYPE_YN_INT32,1, 0, "yes"},*/
	{"Trace","TraceVerbose",&(trace_info.enable_trace_verbose),
				CFG_SIZE_YN_INT32,CFG_TYPE_YN_INT32,1,0,"yes"},

	{"TracePath", "DebugFilePath", (trace_info.debug_file), 
				STD_BUF,CFG_TYPE_STR,1, 0, DEFAULT_DEBUG_FILE_PATH},
	{"TracePath", "ErrorFilePath", (trace_info.error_file), 
				STD_BUF,CFG_TYPE_STR,1, 0, DEFAULT_ERROR_FILE_PATH},
	{"TracePath", "LogFilePath", (trace_info.log_file), 
				STD_BUF,CFG_TYPE_STR,1, 0, DEFAULT_LOG_FILE_PATH},
	{"TracePath", "FatalFilePath", (trace_info.fatal_file), 
				STD_BUF,CFG_TYPE_STR,1, 0, DEFAULT_FATAL_FILE_PATH},
	{"TracePath", "WarningFilePath", (trace_info.warning_file), 
				STD_BUF,CFG_TYPE_STR,1, 0, DEFAULT_WARNING_FILE_PATH},

	{"TraceSize", "MaxDebugFileSize",&trace_info.max_debug_filesz,
				/*(trace_info.debug_filesz_str),*/
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "1"},
	{"TraceSize", "MaxErrorFileSize",&trace_info.max_error_filesz,
				/*(trace_info.error_filesz_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32, 1, 0, "2"},
	{"TraceSize", "MaxFatalFileSize",&trace_info.max_fatal_filesz,
				/*(trace_info.fatal_filesz_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32 , 1, 0, "1"},
	{"TraceSize", "MaxLogFileSize",&trace_info.max_log_filesz,
				/*(trace_info.log_filesz_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32, 1, 0, "2"},
	{"TraceSize", "MaxWarningFileSize",&trace_info.max_warning_filesz,
				/*(trace_info.warning_filesz_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "2"},

	{"TraceLine", "MaxDebugFileLine",&trace_info.max_debug_fileline,
				/*(trace_info.debug_fileln_str),*/
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "2000"},
	{"TraceLine", "MaxErrorFileLine",&trace_info.max_error_fileline,
				/*(trace_info.error_fileln_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "2000"},
	{"TraceLine", "MaxFatalFileLine",&trace_info.max_fatal_fileline,
				/*(trace_info.fatal_fileln_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "1000"},
	{"TraceLine", "MaxLogFileLine",&trace_info.max_log_fileline,
				/*(trace_info.log_fileln_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "8000"},
	{"TraceLine", "MaxWarningFileLine",&trace_info.max_warning_fileline,
				/*(trace_info.warning_fileln_str), */
				CFG_SIZE_UINT32, CFG_TYPE_UINT32,1, 0, "6000"},
	{"LogPath", "CommPath", (trace_info.trace_serv_path), 
				MAX_SOCKETPATH_LEN, CFG_TYPE_STR,1, 0, SERV_PATH},
	{"LogPath", "RunlogPath", (trace_info.trace_runlog_path), 
				MAX_SOCKETPATH_LEN, CFG_TYPE_STR,1, 0, RUNLOG_SERV},
	{"LogPath", "AuditlogPath", (trace_info.trace_auditlog_path), 
				MAX_SOCKETPATH_LEN ,CFG_TYPE_STR,1, 0, AUDITLOG_SERV},
	{"", "",NULL, 0, 0, 0, 0, ""}
};

typedef int(* fpname2id)(char *output);
static int trace_level_name2id(char *level);
static int trace_output_name2id(char *output);
static int  split_var_func(char *str_var,fpname2id name2id,int module);
int trace_init(trace_init_t init);
int trace_clean(void);
void dumpmsg2file(FILE *fp,unsigned int mask, char *tag, const char *filename, 
			const char *funcname, int line,char *fmt, ...);

static inline int module_name2id(char *module)
{
	int module_mask = 0;
	int i = 0;
	int tablesize = 0;

	if ( !module || '\0' == module[0] ) {
		fprintf(stderr,"module name(%s) invalid!\n",module);
		goto ret;
	}

	tablesize = TABLE_SIZE(module_type_name);
	for ( i = 0; i < tablesize; i++){
#ifdef VENUS_TRACE_DEBUG
		printf("the module_type_name[%d]:%s\n",i,module_type_name[i]);
#endif
		if (!strcasecmp(module_type_name[i],module)) {
			module_mask =  (1<<i);
			break;
		}
	}
ret:
	return module_mask;
}

static int trace_level_name2id(char *level)
{
	int level_mask = 0;
	
	if ( NULL == level || '\0' == level[0] ) {
		goto ret;
	}

	if ( !strcasecmp(level,"debug") ) {
		level_mask = TRACE_DEBUG;
	} else if ( !strcasecmp(level, "error") ) {
		level_mask = TRACE_ERROR;
	} else if ( !strcasecmp(level, "log") ) {
		level_mask = TRACE_LOG;
	} else if ( !strcasecmp(level, "warning") ) {
		level_mask = TRACE_WARNING;
	} else if ( !strcasecmp(level, "fatal") ) {
		level_mask = TRACE_FATAL;
	} else {
		fprintf(stderr,"Invalid trace level: %s\n", level);
		goto ret;
	}

ret:
	return level_mask;
}

static int trace_output_name2id(char *output)
{
	int output_mask = 0;

	if ( NULL == output || '\0' == output[0] ) {
		goto ret;
	}

	if ( !strcasecmp(output, "stdout" ) ) {
		output_mask =OUTPUT_STDOUT;
	} else if ( !strcasecmp(output, "syslog" ) ) {
		output_mask =OUTPUT_SYSLOG;
	} else if ( !strcasecmp(output, "file" ) ) {
		output_mask =OUTPUT_FILE;
	} else if( !strcasecmp(output,"udpsocket")){
		output_mask = OUTPUT_UDPSOCKET;
	} else {
		fprintf(stderr,"Invalid trace output:%s\n",output);
		goto ret;
	}

ret:
	return output_mask;
}


static int trace_thread_init(void)
{
#ifdef _REENTRAT
	pthread_mutex_init(&mutex,NULL);
#endif
	return 0;
}

static int  split_var_func(char *str_var,fpname2id name2id,int module)
{
#define MAX_LEVEL_NAME_NUM	8
#define MAX_LEVEL_NAME_LEN	16
	if ('\0' != str_var[0]) {
		char tracelevel_table[MAX_LEVEL_NAME_NUM][MAX_LEVEL_NAME_LEN] = {{0},{0}};
		struct split_var svt[MAX_LEVEL_NAME_NUM];
		int i;
		int nsplit;

		/* parse definition string */
		for (i = 0; i < MAX_LEVEL_NAME_NUM; i++) {
			svt[i].addr = tracelevel_table[i];
			svt[i].maxlen = MAX_LEVEL_NAME_LEN;
		}
		nsplit = str_split(str_var,',',sf_scp, svt,MAX_LEVEL_NAME_NUM);
		if (nsplit > 0) {
			for (i = 0; i < nsplit; i++) {
				if (!strcasecmp(svt[i].addr, "all")) {
					if(module == TRACE_LEVEL){
						trace_info.trace_level = ALL_TRACE_LEVEL;
					}else if(module == TRACE_OUTPUT ) {
						trace_info.trace_output = ALL_TRACE_OUTPUT;
					}else if(module == TRACE_MODULE) {
						trace_info.trace_module = MODULE_ALL;
					}
					break;
				} else if (!strcasecmp(svt[i].addr, "none")) {
					if(module == TRACE_LEVEL){
						trace_info.trace_level = NONE_TRACE_LEVEL;
					}else if(module == TRACE_OUTPUT ) {
						trace_info.trace_output = NONE_TRACE_OUTPUT;
					}else if(module == TRACE_MODULE) {
						trace_info.trace_module = MODULE_NONE;
					}
					break;
				} else {
					if(module == TRACE_LEVEL) {
						trace_info.trace_level |= name2id((char *)svt[i].addr);
					}
					if(module == TRACE_OUTPUT) {
						trace_info.trace_output |= name2id((char *)svt[i].addr);
					}
					if(module == TRACE_MODULE) {
						trace_info.trace_module |= name2id((char *)svt[i].addr);
					}
				}
			}
		} else {
			goto err;
		}
	} else {
		fprintf(stderr,"trace conf invalid !\n");
		goto err;
	}
	return 0;
err:
	return -1;
}

/*
	parse the xxx_trace.conf files
	filename: trace configure file
	return:  0 success
		-1 failed
*/
int trace_init(trace_init_t init)
{
	char *filename = init.conf_file;

	trace_thread_init();

	if ( (NULL == filename) || ('\0' == filename[0]) ) {
		fprintf(stderr,"trace conf path invalid !\n");
		goto err;
	}

	memset(&trace_info, 0, sizeof(trace_info));

	if ( common_read_conf(filename, traceconf_cdt) < 0) {
		fprintf(stderr,"read trace conf file failed!!\n");
		goto err;
	}

	/* split trace level*/
	if(split_var_func(trace_info.trace_level_str,trace_level_name2id,TRACE_LEVEL) == -1){
		goto err;
	}
	/* split trace output*/
	if(split_var_func(trace_info.trace_output_str,trace_output_name2id,TRACE_OUTPUT) == -1){
		goto err;
	}

	/* split trace modules*/
	if(split_var_func(trace_info.trace_module_str,module_name2id,TRACE_MODULE) == -1){
		goto err;
	}

	if ( output_init(init) < 0 ) {
		goto err;
	} 

#ifdef VENUS_TRACE_DEBUG
	printf("after output_init \n");
#endif
#ifdef TRACE_CONF_PRINT
	cfg_print(traceconf_cdt);
	//print_trace_conf();
#endif
	return 0;
err:
	return -1;
}


int trace_clean(void)
{
	output_clean();	

	return 0;
}



/***************	the main ENTRY for DUMP		************************/
void dumpmsg2file(FILE *fp,unsigned int mask, char *tag, const char *filename, 
			const char *funcname, int line,char *fmt, ...)
{
	char mesg[4096]={0};
	char buf[4096]={0};
	time_t t=0;
	struct tm * now=NULL;
	va_list ap;

	
#ifdef VENUS_TRACE_DEBUG
	printf("in dumpmsg2file \n");
#endif
	if ((trace_info.trace_output & OUTPUT_FILE) && NULL == fp) {
		goto ret;
	}

	/* sanity check, ONLY care of output configued */
	if (0 == trace_info.trace_output)
		goto ret;

	time(&t);
	now = localtime(&t);
	va_start(ap, fmt);
	vsprintf(mesg, fmt, ap);
	va_end(ap);
	if(trace_info.enable_trace_verbose) {
		snprintf(buf, 4096, "%04d-%02d-%02d %02d:%02d:%02d <%s> -- %s +%d,%s,%s:%s\n", 
				now -> tm_year + 1900, now -> tm_mon + 1, 
				now -> tm_mday, now -> tm_hour, now -> tm_min, now -> tm_sec, 
				module_mask_2name(mask),
				filename, line, funcname, tag, mesg);
	} else {
		snprintf(buf, 4096, "%04d-%02d-%02d %02d:%02d:%02d,%s\n", 
				now -> tm_year + 1900, now -> tm_mon + 1,
				now -> tm_mday, now -> tm_hour, now -> tm_min, now -> tm_sec, mesg);
	}

	output_put(fp, buf);
#ifdef VENUS_TRACE_DEBUG
	printf("after dumpmsg2file \n");
#endif
ret:
	return;
}
