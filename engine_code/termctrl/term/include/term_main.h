#ifndef TERM_MAIN_H_
#define TERM_MAIN_H_
#define release_version
#define _GNU_SOURCE
#include "term_config.h"
#include "term_ip.h"
#include "port.h"
#include "term_ttyrd.h"
#include "term_ftp.h"
#include "login.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>
#include <glob.h>
#include <errno.h>
#include <ctype.h>

#define ETH_CFG_FILE "/etc/sysconfig/network-scripts/ifcfg-eth\0" 
//#define ETH_CFG_FILE "/root/Desktop/ifcfg-eth\0" 
//#define TMP_FILE "/tmp/term"
#define SERIAL_NUMBER "/gms/conf/device.id"
#define FILE_UUID "/gms/conf/uuid.conf"
#define ACCREDIT "/gms/comm/conf/auth_result.cache"
#define PATHPASSWD "/cfcard/system/gms/comm/conf/.cli_key.pem"
#define FTPFILE "/gms/conf/ftp_server.conf"
#define IP_TMP "/gms/conf/reg.dat"
#define MACHINE_TYPE "/cfcard/chanct_conf/machine_type.conf"
#define KILL_PYTHON  "kill -9 `ps aux|grep -v 'grep' |grep comm_main| awk '{print $1}'`"
#define DOC_NAME "/gms/conf/gmsconfig.xml"
#define AUTH_CMD "python /gms/gapi/modules/auth/sys_auth.pyc 1>/dev/null 2>&1"

#define CHANCT_DEF_IP		"192.168.0.200\0"
#define CHANCT_DEF_NETMASK	"255.255.255.0\0"
#define CHANCT_DEF_GW		"192.168.0.10\0"
#define CHANCT_DEF_DNS		"8.8.8.8\0"
#define PAGCONF             "/gms/termctrl/conf/pag.conf"
#define MTXPAG              "/gms/mtx/pag.conf"
#define APTPAG              "/gms/tcad/pag.conf"
#define VDSPAG              "/vds/pag.conf"
#define CLEANCMD            "/gms/termctrl/conf/cleanfile.sh"    
FILE * com_handle; 
char   gb_ip_temp[16];
char   gb_netmask_temp[16];
char   gb_gateway_temp[16];
char   gb_dns[16];
int    which;
char  * ip_t;
int   total_ethernet_number;
int   manger_ethernet_number;

#define MAX_ETH		20
#define TYPE_FREE	0
#define TYPE_CAP	1
#define TYPE_COM	2

struct global_var 
{
	char pwd[8];
	int  ethNumber;
	char hostname[128];
	int  eth_have_ip[MAX_ETH]; //ethi: 的类型，1：cap；2：com
	int  Ftp_Tel_Flag;
	int  continuity;
	int  eof;
}gb_var, * pgb_var;	

char  eth_pos[MAX_ETH];//ethi: 的位置 NIC位置
#endif
