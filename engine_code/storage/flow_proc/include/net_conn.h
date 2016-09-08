#ifndef _NET_CONN_H_
#define _NET_CONN_H_

int  init_net_store(void);
int net_store_per_one(char *account, unsigned int ip,char *timeval);
#define NET_TABLE 					"t_net_id"
#define NET_CMD_SIZE				200

#endif
