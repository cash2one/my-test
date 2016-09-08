#ifndef __OUTPUT_SYSLOG_H
#define __OUTPUT_SYSLOG_H

int stdout_open();
int stdout_put(FILE *,char *);
int stdout_close();
#endif /*output_syslog.h*/
