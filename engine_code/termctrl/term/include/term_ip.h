#ifndef TERM_IP_H_
#define TERM_IP_H_

#define itochar(i)   (char)((int)('0')+(i%10))

#define DIR_BEFORE	1
#define DIR_AFTER	2
#include <stdio.h>
#define NETPORT "/gms/conf/comm_interface_conf.xml"
#define TMP_NETPORT "tmp"
int set_ethernet_msg(int eth,const char * ip, const char * netmask,const char *gateway,const char *dns);
int IsValidIpaddr(const char * ip);
int set_ethernet_null(int eth);

int get_record(char * buf ,char * property,
			char  seperator,char bracket,char * value);
int set_record(char * buf, const char * property,
			char  seperator,char bracket,const char * value);

int is_record(const char * buf , const char * property,char seperator);
int trim_str(char * buf);

int line_input(FILE * fd,char * linebuf);
int get_ethernet_msg(int eth,char * ip,char * netmask);
#endif

