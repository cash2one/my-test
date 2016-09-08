#ifndef _LOGMON_H_
#define _LOGMON_H_

#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_BUF_LEN 1024
#define MAX_PROG_NUM            128
#define MAX_PROG_NAME_LEN       64   

#define PROG_NOT_RUNNING        0                                                                                 
#define PROG_RUNNING            1   

#define NAME_EQUAL      0                                                                                         
#define NAME_NOT_EQUAL  1                                                                                         

#define AUTH_MODULE	"auth"
#define LOGMON_PROC	"logmon"

#define PROC "/proc"
#define CMDLINE "cmdline"
#define PATH "/home/xiedongling/svn/GMS/trunk/engine_code/auth/logmon/src/"
//#define REG_PATH 	"/tmp/reg_dat/reg.dat"
#define TIME_FORMAT_SIZE    25
#define REG_ID_PATH	"/gms/auth/conf/reg_id"
#define MTIME_WTL_PATH  "/gms/auth/conf/m_wtl_time"
//#define COMM_MAIN_PATH		"/gms/comm/comm_main.py"
//#define COMM_CLOUD_PATH		"/gms/comm/cloud/comm_cloud.py"
//#define MTX_PATH		"/gms/mtx/mtx"

int check_proc_name(char * file_name, char * name);
int watch_proc(char *proc_name, pid_t *pid);
int restart_proc(char *proc_name);
int kill_process(char *proc_name);
int check_process_status(char *status, char *name);
int time_cmp(char *time_str);   
int check_all_process(void);
int parse_xml(char *node_num, char *node_name);
int get_reg_id(char *reg_id);
int is_wtl(int *ret);
int sys_cmd(char *cmd);
int get_flow_num(char *flow_num);
int is_file_modified(int *t);
int is_file_exist(char *path);
int time_cmp_format(char *time_str1, char *time_str2, char *time_format);
#endif
