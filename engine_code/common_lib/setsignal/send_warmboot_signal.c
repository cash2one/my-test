/*
 * =====================================================================================
 *       Filename:  send_warmboot_signal.c
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008-10-07 11:05:08
 *         Author:  idp idp@venus.com
 *        Company:  venustech
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "setsignal.h"

#define IPS_PROC_NAME "ips"
#define CSIGN_PROC_NAME "csign"
#define EVTSEND_PROC_NAME "evt_send"
#define COMM2_NAME	"comm2"
#define SHELL_GUI	"shellgui"

/*Get the DT/IPS process ID*/
static inline int get_pid(char * proce_name)
{
	int pid = 0;
	FILE *fp = NULL;
	char buf[1024] = "";
	char grep[100] = "";

	memset(buf, 0, sizeof(buf));
	sprintf(grep, "ps -ae | grep \"\\<%s\\>\" | grep -v grep > .__tmp_cmdline__", proce_name);
	system(grep);
	fp = fopen(".__tmp_cmdline__", "r");
	if (!fp) {
		goto err;
	}
	fgets(buf, sizeof(buf), fp);
	fclose(fp);
	unlink(".__tmp_cmdline__");
	pid = strtol((const char *) buf, NULL, 10);
	return pid;

      err:
	return -1;
}

//int send_signal(int signo, u_int8_t * proc_name)
int send_signal(int signo, char * proc_name)
{
	int pid = 0;

	pid = get_pid(proc_name);
	if (-1 == pid) {
		goto err;
	}
	if (0 == pid) {
		printf("the %s process does not start\n", proc_name);
		goto pro_no_st;
	}
	if (-1 == kill(pid, signo)) {
		printf("send signal to process %d failed\n", pid);
		goto err;
	}
	return 1;

      pro_no_st:
	return 0;

      err:
	return -1;
}

int send_warmboot_signal(int filenum)
{
	int ret = 0;

	switch (filenum) {
	case SQL_PROTECT_CONF:
		ret = send_signal(SEND_SQL_PROTECT_CONF, IPS_PROC_NAME);
		ret = send_signal(SEND_SQL_PROTECT_CONF, COMM2_NAME);
		break;
	case VXID_PROTECT_CONF:
		ret = send_signal(SEND_VXID_PROTECT_CONF, IPS_PROC_NAME);
		break;
	case ENGINE_CONF:
		ret = send_signal(SEND_ENGINE_CONF, IPS_PROC_NAME);
		ret = send_signal(SIGTERM, CSIGN_PROC_NAME);
		break;
	case POLICY_CONF:
		ret = send_signal(SEND_POLICY_CONF, IPS_PROC_NAME);
		ret = send_signal(SIGTERM, CSIGN_PROC_NAME);
		break;
	case EVTMERG_CONF:
	case EVT_LIMIT_CONF:
	case SECEVT_CONF:
		ret = send_signal(SIGTERM, CSIGN_PROC_NAME);
		break;
	case EVT_INFO_FILE_UPDATE:
		ret = send_signal(EVT_INFO_UPDATE, EVTSEND_PROC_NAME);
		ret = send_signal(EVT_INFO_UPDATE, SHELL_GUI);
		break;
	case VENGINE_SWITCH_CONF:
		ret = send_signal(SEND_VENGINE_SWITCH,COMM2_NAME);
	default:
		ret = send_signal(SIGTERM, IPS_PROC_NAME);
		break;
	}

	return ret;
}

/*
int main ()
{
	send_warmboot_signal(0);
	return 0;
}*/
