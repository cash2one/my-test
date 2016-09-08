/*************************************************************************
	> File Name: ngd_init.h
	> Author: wangleizhang
	> Explain: 
	> Created Time: Wed 23 Oct 2013 03:15:52 AM EDT
 ************************************************************************/
#ifndef NGD_INIT_H
#define NGD_INIT_H
#include <sys/types.h>

#define CAPTOR_ARG_NUM	16
typedef struct CMD_ARG {
    char src[16];
    char dst[16];
    short sport;
    short dport;
    unsigned char protocol;
    unsigned int  print_event;
    char *captor_argv[CAPTOR_ARG_NUM];
    unsigned int captor_argc;
    char dual;
    char verbose;
} cmd_args_t;

cmd_args_t cmd_args;
int ngd_init_entry(int argc, char *argv[]);

#endif

