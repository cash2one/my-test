#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <time.h>
#include <stdarg.h>

#include "trace_api.h"
#include "../include/output_action.h"

extern trace_info_t trace_info;
extern int output_sock_send(FILE *fp,char *aBuf);

/* PLEASE keep the order ...... untouched !!!*/
enum {
	LEVEL_LOW=0,
	LEVEL_MIDDLE,
	LEVEL_HIGH
};

static  char * msg_level_name[]={
	"low",	
	"middle",
	"high",
};
/* end */

static inline char * msg_level_id2name(int level)
{
	if ( level <LEVEL_LOW || level >LEVEL_HIGH) {
		goto err;
	}

	return msg_level_name[level];
err:
	return "";
}

trace_udp_t udp_sk={
	-1,
	0,
	""
};

trace_udp_t runlog_sk={
	-1,
	0,
	""
};

trace_udp_t auditlog_sk={
	-1,
	0,
	""
};

#define SENSOR_MSG_LEN(msg) (sizeof(msg)-sizeof(msg.para)+msg.plen)

char *entry = NULL;

int trace_udp_clean()
{
#ifdef VENUS_TRACE_DEBUG
	printf("in trace_udp_clean\n");
#endif
	udpsocket_close(udp_sk.fd);
	udp_sk.fd = -1;
	udp_sk.enable = 0;

	//bzero(udp_sk.sk_path, sizeof(udp_sk.sk_path));
	memset(udp_sk.sk_path,0,sizeof(udp_sk.sk_path));
	return 0;
}

/* init udp sock ,argument as:
 sock_path 	unix socket path 
 entry		the name of process entry
*/
int trace_udp_init(trace_init_t conf_init)
{
	char *sock_path = trace_info.trace_serv_path;
	char *runlog_path = trace_info.trace_runlog_path;
	char *auditlog_path = trace_info.trace_auditlog_path;

	entry = conf_init.entry_name;

#ifdef VENUS_TRACE_DEBUG
	printf("in trace_udp_init\n");
	printf("1111111sockpath:%s\n",sock_path);
	printf("222222runlogpath:%s\n", runlog_path);
	printf("3333333auditlogpath:%s\n", auditlog_path);
	printf("entry name:%s\n", entry);
#endif
	if (!sock_path || sock_path[0] == '0' || 
			!runlog_path || runlog_path[0] == '0' ||
			!auditlog_path || auditlog_path[0] == '0' || 
			!entry || entry[0] == '0' ) {
		goto err;
	}

	/*memset(&sensor_msg,0,sizeof(sensor_msg));

	strncpy(sensor_msg.entry_name,entry, sizeof(sensor_msg.entry_name));*/

	if ((udp_sk.fd=udpsocket_init(sock_path)) < 0 ) {
		fprintf(stderr,"init udp socket failed!\n");
		goto err;
	}

	if ((runlog_sk.fd=udpsocket_init(runlog_path)) < 0 ) {
		fprintf(stderr,"init udp socket failed!\n");
		goto err;
	}

	if ((auditlog_sk.fd=udpsocket_init(auditlog_path)) < 0 ) {
		fprintf(stderr,"init udp socket failed!\n");
		goto err;
	}
	strncpy(udp_sk.sk_path, sock_path, UNIX_PATH_MAX);
	strncpy(runlog_sk.sk_path, runlog_path, UNIX_PATH_MAX);
	strncpy(auditlog_sk.sk_path, auditlog_path, UNIX_PATH_MAX);
	return 0;
err:
	return -1;
}


/*
* dump trace info to UDP socket,used to cman collector, reserve it for NEW csign
*/

void message2udpsock(unsigned int mask, const char *filename, const char *funcname, 
			int line,char *fmt, ...)
{
	time_t t=0;
	struct tm * now=NULL;

	va_list args;
	static char  buf[4096];
	sensor_msg_t sensor_msg;

	if ( -1 == udp_sk.fd ) {
		goto err;
	}
	time(&t);
	now = localtime(&t);
	
	memset(buf,0,sizeof(buf));
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
 	va_end(args);


	if(-1 == trace_thread_lock()){
		printf("trace_thread lock error\n");
		return;
	}
	memset(&sensor_msg, 0, sizeof(sensor_msg));
	strncpy(sensor_msg.progname,entry, sizeof(sensor_msg.progname));
//	sensor_msg.rflag = type;
	
	memset(sensor_msg.para, 0, sizeof(sensor_msg.para));
	if(trace_info.enable_trace_verbose) {
		snprintf(sensor_msg.para, sizeof(sensor_msg.para),
				"%04d-%02d-%02d %02d:%02d:%02d <%s> -- %s +%d,%s:%s\n", 
				now -> tm_year + 1900, now -> tm_mon + 1, 
				now -> tm_mday, now -> tm_hour, now -> tm_min, now -> tm_sec, 
				module_mask_2name(mask),
				filename, line, funcname, buf);
	} else {
		snprintf(sensor_msg.para, sizeof(sensor_msg.para), "%s,(#%d):%s\n",
				filename, line, buf);
	}
	sensor_msg.plen= strlen(sensor_msg.para);
	sock_send(udp_sk.fd, udp_sk.sk_path, (char *)&sensor_msg, SENSOR_MSG_LEN(sensor_msg)); 
	if(-1 == trace_thread_unlock()){
		printf("trace_thread lock error\n");
		return;
	}

err:
	return;
}

/*dump the running log  to cman*/


void runninglog2udpsock(unsigned int mask, int flag, const char *filename, const char *funcname, 
			int line,char *fmt, ...)
{
	time_t send_time = 0;
	struct tm *now = NULL;

	va_list args;
	static char  buf[4096];
	run_info running_msg;

	if ( -1 == runlog_sk.fd ) {
		goto err;
	}
	send_time = time(NULL);
	now = localtime(&send_time);

	memset(buf,0,sizeof(buf));
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
 	va_end(args);


	if(-1 == trace_thread_lock()){
		printf("trace_thread lock error\n");
		return;
	}
	memset(&running_msg,0,sizeof(run_info));

	running_msg.flag = flag;
	running_msg.time = send_time;
	strncpy(running_msg.progname,entry, sizeof(running_msg.progname));
	
	memset(running_msg.para, 0,sizeof(running_msg.para));
	if(trace_info.enable_trace_verbose) {
		snprintf(running_msg.para, sizeof(running_msg.para),
				"%s,(#%d),%s:%s\n", filename, line, funcname, buf);
	} else {
		snprintf(running_msg.para,sizeof(running_msg.para),"%s\n", buf);
	}
	sock_send(runlog_sk.fd, runlog_sk.sk_path, (char *)&running_msg, sizeof(run_info)); 
	if(-1 == trace_thread_unlock()){
		printf("trace_thread lock error\n");
		return;
	}

err:
	return;

}

void auditlog2udpsock(unsigned int mask, void *msg)
{
	if ( -1 == udp_sk.fd ) {
		goto err;
	}
	if(-1 == trace_thread_lock()){
		printf("trace_thread lock error\n");
		return;
	}
	/*we just send it and do nothing*/
	sock_send(auditlog_sk.fd, auditlog_sk.sk_path, (char *)msg, sizeof(audit_log));

	if(-1 == trace_thread_unlock()){
		printf("trace_thread lock error\n");
		return;
	}
err:
	return;
}

