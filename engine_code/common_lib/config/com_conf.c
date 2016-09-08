#include <stdio.h>
#include <string.h>
#include "com_conf.h"
#include "config.h"
#include "sign_event_def.h"

struct Configs g_config;
static cfg_desc comconf_cdt[] = {
	{"general", "debug", &(g_config.debug), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "signToDB", &(g_config.signToDBFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "signToClient", &(g_config.signToClientFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "statToDB", &(g_config.statToDBFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "statToClient", &(g_config.statToClientFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "configToDB", &(g_config.configToDBFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "configToClient", &(g_config.configToClientFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "server_log", &(g_config.server_logFlag), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"general", "location", g_config.location, 64, CFG_TYPE_STR, 0, 0, "province"},


	{"udp_ice", "udpthread", (&g_config.Udp_thread), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"udp_ice", "oneway", &(g_config.Udp_oneway), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"udp_ice", "proxy", (g_config.UdpProxyStr), 128, CFG_TYPE_STR, 0, 0, "No"},
	{"udp_ice", "listen_port", &(g_config.Udp_listenPort), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},

	{"tcp_ice", "tcpthread", &(g_config.Tcp_thread), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"tcp_ice", "oneway", &(g_config.Tcp_oneway), CFG_SIZE_YN, CFG_TYPE_YN, 0, 0, "No"},
	{"tcp_ice", "proxy", (g_config.TcpProxyStr), 128, CFG_TYPE_STR, 0, 0, "No"},
	{"tcp_ice", "listen_port", &(g_config.Tcp_listenPort), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"tcp_ice", "timeout", &(g_config.Tcp_timout), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},

	{"", "", NULL, 0, 0, 0, 0, ""}
};

int read_complus_conf(char *conf_file_name, struct Configs *conf)
{
	if(conf == NULL) {
		goto err;
	}

	memset(conf, 0, sizeof(struct Configs));

	if(common_read_conf(conf_file_name, comconf_cdt) < 0) {
		//DMSG("common_read_conf error in read_dt_conf");
		goto err;
	}

	g_config.whoamI = REPORT_NULL;
	if (!strncasecmp(g_config.location, "province", 8)) {
		g_config.whoamI = REPORT_PROVINCE;
	} else if (!strncasecmp(g_config.location, "country", 7)) {
		g_config.whoamI = REPORT_COUNTRY;
	}

	if (g_config.debug) {
		cfg_print(comconf_cdt);
	}
	return 0;
err:
	return -1;
}
struct ShmConfigs g_ShmConfig;
static cfg_desc shmconf_cdt[] = {
	{"TcpWriteToDb", "owner", g_ShmConfig.tcpOwer, 128, CFG_TYPE_STR, 0, 0, ""},
	{"TcpWriteToDb", "key", &(g_ShmConfig.tcpShmKey), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"TcpWriteToDb", "width", &(g_ShmConfig.tcpShmWidth), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"TcpWriteToDb", "length", &(g_ShmConfig.tcpShmWidth), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},

	{"UdpWriteToDb", "owner", g_ShmConfig.udpOwer, 128, CFG_TYPE_STR, 0, 0, ""},
	{"UdpWriteToDb", "key", &(g_ShmConfig.udpShmKey), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"UdpWriteToDb", "width", &(g_ShmConfig.udpShmWidth), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"UdpWriteToDb", "length", &(g_ShmConfig.udpShmWidth), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},

	{"TcpWriteToClient", "owner", g_ShmConfig.tcpclientOwer, 128, CFG_TYPE_STR, 0, 0, ""},
	{"TcpWriteToClient", "key", &(g_ShmConfig.tcpclientShmKey), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"TcpWriteToClient", "width", &(g_ShmConfig.tcpclientShmWidth), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"TcpWriteToClient", "length", &(g_ShmConfig.tcpclientShmLength), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},

	{"UdpWriteToClient", "owner", g_ShmConfig.udpclientOwer, 128, CFG_TYPE_STR, 0, 0, ""},
	{"UdpWriteToClient", "key", &(g_ShmConfig.udpclientShmKey), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"UdpWriteToClient", "width", &(g_ShmConfig.udpclientShmWidth), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},
	{"UdpWriteToClient", "length", &(g_ShmConfig.udpclientShmLength), CFG_SIZE_UINT32, CFG_TYPE_UINT32, 0, 0, "0"},

	{"", "", NULL, 0, 0, 0, 0, ""}
};

int read_shm_conf(char *conf_file_name, struct ShmConfigs *conf)
{
	if(conf == NULL) {
		goto err;
	}

	memset(conf, 0, sizeof(struct Configs));

	if(common_read_conf(conf_file_name, shmconf_cdt) < 0) {
		//DMSG("common_read_conf error in read_dt_conf");
		goto err;
	}

	if (g_config.debug) {
		cfg_print(shmconf_cdt);
	}
	return 0;
err:
	return -1;
}
