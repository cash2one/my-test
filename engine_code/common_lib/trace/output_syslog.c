#include <syslog.h>
#include "trace_api.h"

int syslog_open()
{
	return 0;
}

/* dump message to syslog,fp just used padding */
int syslog_put(FILE *fp, char *msg)
{
	if ( !msg) {
		goto ret;
	}

	openlog("", 0, LOG_DAEMON);
	syslog(LOG_ERR, msg);
	closelog();

ret:
	return 0;
}

int syslog_close()
{
	return 0;
}

