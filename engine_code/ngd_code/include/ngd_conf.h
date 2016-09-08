#ifndef _NGD_CONF_H
#define _NGD_CONF_H

//#include <sys/types.h>
#define DEFAULT_NGD_CONF_PATH	    "./conf/ngd_init.conf"
#define CAPTOR_NAME_LEN	    64
#define CONF_PATH_LEN	256

/* global config structure */
struct NGD_CONF {
	unsigned int	thread_num;
	unsigned int	flow_num;
	unsigned int	zero_packet_limit;
	unsigned int	sleep_each_packet;

	char	    captor_name[CAPTOR_NAME_LEN];
	char	    reasm_conf_path[CONF_PATH_LEN];
};

extern struct NGD_CONF ngd_conf;
int init_ngd_conf(struct NGD_CONF *conf);
void clean_ngd_conf(struct NGD_CONF *conf);
int read_ngd_conf(char *conf_file_name, struct NGD_CONF *conf);
#endif // _NGD_CONF_H
