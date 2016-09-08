/*************************************************************************
	> File Name: gms_udp_sock.h
	> Created Time: Fri 11 Sep 2015 04:25:41 PM ICT
 ************************************************************************/
#ifndef _GMS_UDP_SOCK_H_
#define _GMS_UDP_SOCK_H_


struct gms_recv_ip_account{
	unsigned int cmd_id;
	unsigned int sub_cmd_id;
	unsigned int len;
	char time[20]; /* yyyy-mm-dd hh:MM:ss */
	char accout[32];
	char name[32];
	unsigned short card_type;
	char card_num[20];
	char address[32];
	char nationality[12];
	char remarks[50];
	unsigned short charge_id;
	unsigned short bandwidth_id;
	unsigned short administration_id;
	unsigned short fst_ver;
	unsigned short sec_ver;
	unsigned int checksum;
	unsigned char event_type; /* 1:login, 2:logout */ //???????????
	unsigned int ip;
	unsigned char mac[6];
	//unsigned char reserve[3]; //?????????????
}__attribute__((__packed__));
struct gms_send_ip_account{
	int cmd_id;
	int sub_cmd_id;
}__attribute__((__packed__));

int gms_udp_sock_init(void);
int gms_udp_sock_exit(void);
char * gms_get_account(unsigned int ip);


#endif  /* _GMS_UDP_SOCK_H_ */

