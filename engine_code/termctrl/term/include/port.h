#ifndef PORT_H_
#define PORT_H_

//#define NETPORT "/gms/termctrl/conf/netport.conf"
//#define TMP_NETPORT "tmp"
typedef struct
{
	int file_port;
	int command_port;
}Nport;
Nport get_port();
int set_port();
#endif
