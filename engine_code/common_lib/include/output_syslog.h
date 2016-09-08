#ifndef __OUTPUT_SYSLOG_H
#define __OUTPUT_SYSLOG_H

int syslog_open();
int syslog_put(FILE *fp, char *msg);
int syslog_close();
#endif /*output_syslog.h*/
