struct Configs {
	int debug;
	char location[64];
	char whoamI;
	char signToDBFlag;
	char signToClientFlag;
	char statToDBFlag;
	char statToClientFlag;
	char configToDBFlag;
	char configToClientFlag;
	char server_logFlag;

	char UdpProxyStr[128];
	int Udp_listenPort;
	char Udp_oneway;
	char Udp_thread;

	char Tcp_thread;
	char TcpProxyStr[128];
	char Tcp_oneway;
	int Tcp_listenPort;
	int Tcp_timout;
};

struct ShmConfigs {
	char tcpOwer[128];
	int tcpShmKey;
	int tcpShmWidth;
	int tcpShmLength;

	char udpOwer[128];
	int udpShmKey;
	int udpShmWidth;
	int udpShmLength;

	char tcpclientOwer[128];
	int tcpclientShmKey;
	int tcpclientShmWidth;
	int tcpclientShmLength;

	char udpclientOwer[128];
	int udpclientShmKey;
	int udpclientShmWidth;
	int udpclientShmLength;
};

extern struct Configs g_config;
extern struct ShmConfigs g_ShmConfig;
int read_complus_conf(char *conf_file_name, struct Configs *conf);
int read_shm_conf(char *conf_file_name, struct ShmConfigs *conf);
