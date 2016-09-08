#ifndef _DDOS_CONFIG_H_
#define _DDOS_CONFIG_H_

enum ddos_network_iplist_type{
	ddos_type_ip_plen = 0,
	ddos_type_ip_seg,
};
typedef struct ddos_network_iplist{
	int type;
	union{
		struct{
			unsigned int ip;
			unsigned int plen;
		}ip_plen;
		struct{
			unsigned int sip;
			unsigned int eip;
		}ip_seg;
	};
}ddos_network_iplist_t;

unsigned int ddos_get_ip_file_linenum(char *path);

int ddos_get_ip_file_data(char *path, ddos_network_iplist_t *array, int size);

#endif  /* _DDOS_CONFIG_H_ */

