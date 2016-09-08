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


#ifndef _TRACE_API_H_
#define _TRACE_API_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

/* ******** module about ***********/
/*when you use trace module,the ONLY codes to be modifited is here:
	enmu type var: 		the module ID ,also used for name index 
	module_type_name:	the sub module name used in your project
*/

/* ISwalker: USING Attention!!!!!!!!
	 the ENUM and name arry MUST keep the SAME order !!!!
	 Queue Please ,:) Aha
*/
enum _module_type_t{
	MODULE_MIN=0,	/* keep untouched */

	/* the following is your sub module definition*/
        FILE_OPERATION = 1,
	AUTH,
	NETWORK_COMMU,
	FIREWALL_LINKAGE,
	SHM_COMMU,
	ACCREDIT,
	RUN_LOG,                        // ÔËÐÐÈÕÖ¾
	AUDIT_LOG,                      // Éó¼ÆÈÕÖ¾
	STAT_LOG,                        // Í³¼ÆÈÕÖ¾
	/*other modules can be added here*/

	IPS_INIT,
	/*csign module*/
	MAIN_ENTRY,
	EVT_FILTER,
	GET_POLICY,
	MERGE_EVT,
	SECOND_EVT,
	/*csign module*/

	STORE_PROC, //add by ldb 2013-11-14
	MERGE_PROC, //add by ldb 2013-11-25
	FLOW_PROC, //add by ldb 2013-12-04

	MODULE_MAX	/* keep untouched !! */
}module_type_t;



#define MODULE_ALL     ( 1<<MODULE_MIN\
			|1<<FILE_OPERATION\
			|1<<AUTH\
			|1<<NETWORK_COMMU \
			|1<<FIREWALL_LINKAGE\
			|1<<SHM_COMMU\
			|1<<ACCREDIT \
			|1<<RUN_LOG\
			|1<<AUDIT_LOG\
			|1<<STAT_LOG\
			|1<<IPS_INIT\
			|1<<MAIN_ENTRY\
			|1<<EVT_FILTER\
			|1<<GET_POLICY\
			|1<<MERGE_EVT\
			|1<<SECOND_EVT\
			|1<<STORE_PROC\
			|1<<MERGE_PROC\
			|1<<FLOW_PROC)				

#define MODULE_NONE	0

static char * module_type_name[]={
	 "dummy_operation",
	 "file_operation",
	 "auth",
	 "network_commu",
	 "firewall_linkage",
	 "shm_commu",
	 "accredit",
	 "run_log",
	 "audit_log",
	 "stat_log",
	 "ips_init",
	 "csign_main",
	 "evt_filter",
	 "get_policy",
	 "merge_evt",
	 "second_evt",
	 "store_proc", //add by ldb 2013-11-14
	 "merge_proc", //add by ldb 2013-11-25
	 "flow_proc", //add by ldb 2013-12-04
	/*other modules name can be adde here*/
};


#define TABLE_SIZE(table) (sizeof(table)/sizeof(table[0])) 



static inline char *module_mask_2name(unsigned int mask)
{
	static char *p = "dummy";
/*because mask's type is unsigned int ,so it's not need to comparse the mask and the MODULE_MIN*/
	if(mask < MODULE_MAX) {
		return module_type_name[mask];
	}else{
		fprintf(stderr,"invalid module mask!\n");	
		return p;
	}
}

/* ***************end about module **************/

/*************** output_udp about *****************/
typedef struct trace_udp_t {
	int fd;
	int enable; /* what is it? */
	char sk_path[108];
}trace_udp_t;


#define MAX_NAME_LEN	20 
typedef struct _sensor_msg_t
{
	char progname[MAX_NAME_LEN];		
	short otype;
	short ostype;
	short level;
#define MSG_SUCCESS 0
#define MSG_FAILED  1
	short  rflag;	
	short resp;
	short space;
	int plen;
#define MAX_PARA_LEN	256
	char para[MAX_PARA_LEN];
}sensor_msg_t;

#ifndef LOG_STRUCT
#define LOG_STRUCT
/*running log recording*/
#define MAX_PARA_LEN    256
#define LOG_RUN_INFO	1
#define LOG_RUN_ERROR	0

typedef struct _run_info{
	char progname[MAX_NAME_LEN];           // ¸÷Ó¦ÓÃ³ÌÐòÃû
	unsigned int time;                 // ÈÕÖ¾·¢ÉúÊ±¼ä
	int flag;                     // 1-ÐÅÏ¢£»0-´íÎó
	char para[MAX_PARA_LEN]; // ÈÕÖ¾ÄÚÈÝ
}run_info;

/*audit log recording*/
#define MAX_USERNAME_LEN 256
#define MAX_OPERATION_LEN 64
#define MAX_INFO_LEN 128
#define MAX_OPINFO_LEN 	1024
 
typedef struct _audit_log{
	char progname[MAX_NAME_LEN];                    // ¸÷Ó¦ÓÃ³ÌÐòÃû
	unsigned int time;                                 // ÈÕÖ¾·¢ÉúÊ±¼ä
	unsigned int clientip;                             // ¿Í»§¶Ëip,Ö÷»ú×Ö½ÚÐò
	char username[MAX_USERNAME_LEN];  // ÓÃ»§Ãû
	int userroletype;  
	// ÓÃ»§½ÇÉ« 0: user manager 1: auditor 10:configuration admin 
	char operation[MAX_OPERATION_LEN]; // ²Ù×÷
	char info[MAX_INFO_LEN];                 // ²Ù×÷½á¹û
	char opinfo[MAX_OPINFO_LEN];                 // ²Ù×÷ÃèÊö
}audit_log;
#endif

/* *********** end output_udp about ****/

int udpsocket_init(char *path);
int sock_recv(int aFd,char *path,char *aBuf,int aNum);
int sock_send(int aFd,char *path,char *aBuf,int aNum);
int udpsocket_close(int aFd);
/******** DEBUG API :: Declaration  of DUMP/DEBUG MACRO ***************/

/******* trace.h ***********/


/* trace level */
#define TRACE_DEBUG 0x0001
#define TRACE_ERROR 0x0002
#define TRACE_LOG   0x0004
#define TRACE_FATAL 0x0008
#define TRACE_WARNING 0x0010

#define ALL_TRACE_LEVEL 0xffff
#define NONE_TRACE_LEVEL 0x0000
/*end*/

/* trace output*/
#define OUTPUT_STDOUT	0x0001
#define OUTPUT_FILE	0x0002
#define OUTPUT_SYSLOG	0x0004
#define OUTPUT_UDPSOCKET 0x0008

#define ALL_TRACE_OUTPUT 0xffff
#define NONE_TRACE_OUTPUT 0x0000
/*end*/

#define STD_BUF 512
#define MAX_FILESZ_STR 32
#define MAX_FILELNE_STR 32


typedef int (* chk_file_fn)(FILE *);



/* trace file information */
typedef struct _file_info_t {
	char fname[STD_BUF];
	FILE *fp;

	unsigned int max_size;
	unsigned int max_line;
	unsigned int current_line;
}file_info_t;



/*ATTENTION: the var of this type should be filled by caller */
typedef struct _trace_init_t {
	char *conf_file;	/*trace conf file ,like dt_trace.conf, etc*/
	char *entry_name;	/*the name of caller*/
}trace_init_t;




/* the order should untouched !!! KEEP */
enum {
	TRACE_FILE_ID_DEBUG=0,
	TRACE_FILE_ID_ERROR=1,
	TRACE_FILE_ID_LOG,
	TRACE_FILE_ID_FATAL,
	TRACE_FILE_ID_WARNING,
	/* other trace file ID added here*/
	TRACE_FILE_ID_BORDER
};

#define MAX_SOCKETPATH_LEN	108
typedef struct _trace_info_t {
	unsigned int trace_level;
	unsigned int trace_output;
	unsigned int trace_module;	

	int  enable_trace_udpsock;
	int  enable_trace_verbose;

	char trace_level_str[STD_BUF];
	char trace_output_str[STD_BUF];	
	char trace_module_str[STD_BUF];
	char trace_check_mode_str[MAX_FILESZ_STR];

	char trace_serv_path[MAX_SOCKETPATH_LEN];
	char trace_runlog_path[MAX_SOCKETPATH_LEN];
	char trace_auditlog_path[MAX_SOCKETPATH_LEN];

	chk_file_fn fn;

	file_info_t file_info[TRACE_FILE_ID_BORDER];
} trace_info_t;

#define debug_file 		file_info[TRACE_FILE_ID_DEBUG].fname
#define error_file 		file_info[TRACE_FILE_ID_ERROR].fname
#define log_file   		file_info[TRACE_FILE_ID_LOG].fname
#define fatal_file 		file_info[TRACE_FILE_ID_FATAL].fname
#define warning_file 		file_info[TRACE_FILE_ID_WARNING].fname

#define debug_fp 		file_info[TRACE_FILE_ID_DEBUG].fp
#define error_fp 		file_info[TRACE_FILE_ID_ERROR].fp
#define log_fp   		file_info[TRACE_FILE_ID_LOG].fp
#define fatal_fp 		file_info[TRACE_FILE_ID_FATAL].fp
#define warning_fp 		file_info[TRACE_FILE_ID_WARNING].fp

#define max_debug_filesz 	file_info[TRACE_FILE_ID_DEBUG].max_size
#define max_error_filesz 	file_info[TRACE_FILE_ID_ERROR].max_size
#define max_log_filesz   	file_info[TRACE_FILE_ID_LOG].max_size
#define max_fatal_filesz 	file_info[TRACE_FILE_ID_FATAL].max_size
#define max_warning_filesz 	file_info[TRACE_FILE_ID_WARNING].max_size

#define max_debug_fileline 	file_info[TRACE_FILE_ID_DEBUG].max_line
#define max_error_fileline 	file_info[TRACE_FILE_ID_ERROR].max_line
#define max_log_fileline   	file_info[TRACE_FILE_ID_LOG].max_line
#define max_fatal_fileline 	file_info[TRACE_FILE_ID_FATAL].max_line
#define max_warning_fileline 	file_info[TRACE_FILE_ID_WARNING].max_line


extern trace_info_t trace_info;
#ifdef __cplusplus
extern "C"{
#endif 
int trace_init(trace_init_t);
int trace_udp_init(trace_init_t);
void dumpmsg2file(FILE *fp,unsigned int mask, char *tag, const char *filename, 
			const char *funcname, int line,char *fmt, ...);
void message2udpsock(unsigned int mask, const char *filename, const char *funcname, int line,char *fmt, ...);
void auditlog2udpsock(unsigned int mask, void *msg);
void runninglog2udpsock(unsigned int mask, int flag, const char *filename, const char *funcname, 
			int line,char *fmt, ...);
int trace_clean(void);
int trace_udp_clean();

#ifdef __cplusplus
}
#endif 
/****** end of trace.h******/

/* DUMP by DEBUGMSG*/
#ifndef NO_DEBUGMSG
#define DEBUGMSG(mask, x...) \
	do {\
	        if ((TRACE_DEBUG & trace_info.trace_level) && ((1<<mask) & trace_info.trace_module) ) {\
			dumpmsg2file(trace_info.debug_fp,mask, "DEBUG",__FILE__,__FUNCTION__,__LINE__,##x);\
		}\
	 }while(0) 
#define DMSG(x...)\
	DEBUGMSG(1,x)
#else	
#define DEBUGMSG(mask, x...)
#define DMSG(x...)
#endif


/* DUMP by ERRMSG*/
#ifndef NO_ERRMSG
#define ERRMSG(mask,  x...) \
	do {\
	        if ((TRACE_ERROR & trace_info.trace_level) && ((1<<mask) & trace_info.trace_module) ) {\
			dumpmsg2file(trace_info.error_fp,mask, "ERROR",__FILE__,__FUNCTION__,__LINE__,##x);\
		}\
	} while(0) 

#define EMSG(x...)\
	ERRMSG(1,x)
#else
#define ERRMSG(mask,  x...)
#define EMSG(x...)
#endif


/* DUMP by LOGMSG*/
#ifndef NO_LOGMSG
#define LOGMSG(mask,  x...) \
	do {\
	        if ((TRACE_LOG & trace_info.trace_level) && ((1<<mask) & trace_info.trace_module) ) {\
			dumpmsg2file(trace_info.log_fp,mask, "LOG",__FILE__,__FUNCTION__,__LINE__,##x);\
		}\
	} while(0)

#define LMSG(x...)\
	LOGMSG(1,x)
#else 
#define LOGMSG(mask,  x...)
#define LMSG(x...)
#endif


/* DUMP by FATALMSG*/
#ifndef NO_FATALMSG
#define FATALMSG(mask, x...) \
	do {\
	        if ((TRACE_FATAL & trace_info.trace_level) && ((1<<mask) & trace_info.trace_module) ) {\
			dumpmsg2file(trace_info.fatal_fp,mask, "FATAL",__FILE__,__FUNCTION__,__LINE__,##x);\
		}\
	} while(0)

#define FMSG(x...)\
	FATALMSG(1,x)
#else
#define FATALMSG(mask,  x...)
#define FMSG(x...)
#endif

#ifndef NO_WARNMSG
#define WARNMSG(mask,  x...) \
	do {\
	        if ((TRACE_WARNING & trace_info.trace_level) && ((1<<mask) & trace_info.trace_module) ) {\
			dumpmsg2file(trace_info.warning_fp,mask, "WARN",__FILE__,__FUNCTION__,__LINE__,##x);\
		}\
	} while(0) 

#define WMSG(x...)\
	WARNMSG(1,x)
#else
#define WARNMSG(mask,  x...)
#define WMSG(x...)
#endif


/* DUMP by UDP SOCKET*/
#ifndef NO_DUMP2USOCK
#define DUMP2USOCK(mask, x...)\
	do {\
		if ( (OUTPUT_UDPSOCKET&trace_info.trace_output) && ((1<<mask) & trace_info.trace_module) ) {\
			message2udpsock(mask,__FILE__,__FUNCTION__,__LINE__,##x);\
		}\
		\
	} while(0)
#else 
#define DUMP2USOCK(module,x...)
#endif

/*dump the running log to udp socket*/
#ifndef NO_DUMP2USOCK
#define RUNNINGLOG2USOCK(mask, flag ,x...)\
	do {\
		if ( (OUTPUT_UDPSOCKET&trace_info.trace_output) && ((1<<mask) & trace_info.trace_module) ) {\
			runninglog2udpsock(mask, flag, __FILE__, __FUNCTION__, __LINE__, ##x);\
		}\
		\
	} while(0)
#else 
#define RUNNINGLOG2USOCK(module,x...)
#endif


/*dump the audit log to udp socket*/
#ifndef NO_DUMP2USOCK
#define AUDITLOG2USOCK(mask, ptr)\
	do {\
		if ( (OUTPUT_UDPSOCKET&trace_info.trace_output) && ((1<<mask) & trace_info.trace_module) ) {\
			auditlog2udpsock(mask, ptr);\
		}\
		\
	} while(0)

#else 
#define AUDITLOG2USOCK(module,x...)
#endif


#ifdef CASSERT_DEBUG
#define C_ASSERT(x)							\
do {									\
	if (!(x)){							\
		printf("V_ASSERT: file(%s),line(%d),function(%s)\n",\
		       __FILE__, __LINE__, __FUNCTION__);		\
		assert(x);						\
		exit(0); 						\
	}\
} while(0)
#else
#define C_ASSERT(x)
#endif


#define INIT_TRACE_MODULE(conf) \
{\
	trace_init(conf);\
	if (trace_info.trace_output&OUTPUT_UDPSOCKET){ \
		trace_udp_init(conf);\
	} \
}

#define CLEAN_TRACE_MODULE() \
{\
	trace_clean();	\
	if (trace_info.trace_output&OUTPUT_UDPSOCKET){\
		trace_udp_clean();\
	}\
}

#define SIMPLETRACE_INIT(program, conf) \
	        trace_init_t trace_init_value = { \
			                conf, \
			                program \
			        };      \
        INIT_TRACE_MODULE(trace_init_value);

#define SIMPLETRACE_CLEAN() \
	        CLEAN_TRACE_MODULE()

#endif	/* _TRACE_API_H_ */

