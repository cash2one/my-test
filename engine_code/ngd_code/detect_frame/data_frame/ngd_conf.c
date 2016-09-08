#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ngd_conf.h"
#include "misc.h"
#include "trace_api.h"

/* global var */

struct NGD_CONF ngd_conf; //config structure

static cfg_desc ngdconf_cfg[] = 
{
    {"Thread", "ThreadNum", &(ngd_conf.thread_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "3"},
    {"Thread", "FlowNum", &(ngd_conf.flow_num), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "4"},
    {"Captor", "CaptorName", &(ngd_conf.captor_name), CAPTOR_NAME_LEN, CFG_TYPE_STR, 0, 0, ""},
    //{"Captor", "CapLibPath", &(ngd_conf.cap_lib_path), CONF_PATH_LEN, CFG_TYPE_STR, 0, 0, ""},
    {"Captor", "ZeroPacketLimit", &(ngd_conf.zero_packet_limit), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "100"},
    {"Captor", "SleepEachPacket", &(ngd_conf.sleep_each_packet), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "100"},
    {"Decode", "ReasmConfPath", &(ngd_conf.reasm_conf_path), CONF_PATH_LEN, CFG_TYPE_STR, 0, 0, ""},
    {"", "", NULL, 0, 0, 0, 0, ""},  //do not forgot to append null 
};

int init_ngd_conf(struct NGD_CONF *conf)
{
    if (!conf) {
	return -1;
    }

    memset(conf, 0, sizeof(struct NGD_CONF));

    return 0;
}

void clean_ngd_conf(struct NGD_CONF *conf)
{
    if (!conf)
	return;

    memset(conf, 0, sizeof(struct NGD_CONF));

    return;
}
#if 0
static inline void adjust_ngd_conf(struct NGD_CONF *conf)
{
	if (ngd_conf.print_responsor == 0) {
		ngd_conf.print_stream_payload = 0;
	}

	if (ngd_conf.sign_checker == 0) {
		ngd_conf.sign_association = 0;
		ngd_conf.sign_associ_4key = 0;
	}
}
#endif
//
// read_ngd_conf
// NTD: add auto conversion
int read_ngd_conf(char *conf_file_name, struct NGD_CONF *conf)
{
	if(conf == NULL) {
		goto err;
	}

	memset(conf, 0, sizeof(struct NGD_CONF));

	if(common_read_conf(conf_file_name, ngdconf_cfg) < 0) {
		//DMSG("common_read_conf error in read_ngd_conf");
		goto err;
	}

	//adjust_ngd_conf(conf);

//	cfg_print(ngdconf_cNGD);
	return 0;
err:
	return -1;
}
