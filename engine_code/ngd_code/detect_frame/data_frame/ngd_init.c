/*************************************************************************
	> File Name: ngd_init.c
	> Author: wangleizhang
	> Explain:ngd初始化 
	> Created Time: Wed 23 Oct 2013 01:25:44 AM EDT
 ************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "ngd_sys.h"
#include "ngd_conf.h"
#include "../plugin_manage/plugin.h"
#include "ngd_init.h"
#include "argv.h"

int parsecommand(int argc, char *argv[], cmd_args_t *args)
{
    int op;

    memset(args, 0, sizeof(cmd_args_t));
#if 0
    while ((op = getopt(argc, argv, "bs:d:S:D:2p:ex:vVf:n:")) != EOF) {
	switch (op) {
	    case 'b':
		godaemon();
		break;
	    case 'x':
		args->print_event = strtoul(optarg, NULL, 16);
		break;
	    case 's':
		strncpy(args->src, optarg, 16);
		break;
	    case 'd':
		strncpy(args->dst, optarg, 16);
		break;
	    case 'S':
		args->sport = atoi(optarg);
		break;
	    case 'D':
		args->dport = atoi(optarg);
		break;
	    case '2':
		args->dual = 1;
		break;
#if 0
	    case 'p':
		if (!strcmp(optarg, "tcp"))
		    //args->protocol = IPPROTO_TCP;
		else if (!strcmp(optarg, "udp"))
		    //args->protocol = IPPROTO_UDP;
		else if (!strcmp(optarg, "icmp"))
		    //args->protocol = IPPROTO_ICMP;

		break;
#endif
	    case 'V':
		args->verbose = 1;
		break;

	    case 'v':
		//version();
		exit(0);
#if 0
	    case 'n':
		SHUGUANG_STREAM_NUM = 0;
		SHUGUANG_STREAM_NUM = atoi(optarg);
		if (SHUGUANG_STREAM_NUM == 0)
		    shuguang_drvbuf_idx = 0;
		else
		    shuguang_drvbuf_idx = 2;

		break;
#endif		

	    default:
		goto err;
		break;
	}
    }
#endif
    while (argv[optind]) {
	if (args->captor_argc == CAPTOR_ARG_NUM - 1)
	    break;
	args->captor_argv[args->captor_argc] = argv[optind];
	args->captor_argc++;
	optind++;
    }
    args->captor_argv[args->captor_argc] = NULL;

    return 0;
err:
    return -1;
}

#define RUNARGS_NUM	16
int process_cmd(int argc, char *argv[])
{
    int real_argc = 0;
    char **real_argv = NULL;

    if (argc == 1) {	/*run mt as default configure */
	real_argv = calloc(RUNARGS_NUM, sizeof(char *));
	real_argv[0] = argv[0];
	real_argc = 1;
	//real_argc += argv_create(dt_conf.default_runargs, RUNARGS_NUM - 1, &real_argv[1]);
    } else {
	real_argc = argc;
	real_argv = argv;
    }

    if (parsecommand(real_argc, real_argv, &cmd_args) < 0) {	/*cmd_args defined in Detector.h */
	//usage(real_argv[0]);
	return -1 ;
    }
    if (argc == 1 && real_argv) {
	free(real_argv);
    }
    return 0;
}

int ngd_init_entry(int argc, char *argv[])
{
    //读取配置

    int ret = NGD_OK;
    if (-1 == process_cmd(argc, argv)) {
	ret = NGD_ERR;
	goto RET;
    }

    if(NGD_ERR == read_ngd_conf(DEFAULT_NGD_CONF_PATH, &ngd_conf)) {
	ret = NGD_ERR;
	goto RET;
    }
    
    if (NGD_ERR == init_plugin()) {
	ret = NGD_ERR;
	goto RET;
    }

RET:
    return ret;
}
