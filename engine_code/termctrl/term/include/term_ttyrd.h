#ifndef __chanct_com
#define __chanct_com
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#define sleep_time   2
#define max_wait_time 1800000
#define VENUS_PROMPT "chanct:"
#define COM1   "/dev/ttyS0"
#define COM2   "/dev/ttyS0"
#define COM3   "/dev/ttyS2"
#define COM4   "/dev/ttyS3"
int _chanct_fd;

int _ini_fd[2];

int _eFlag;
int ReceiveOneLine(char * p);
int SendOneLine(char * p);
int SendString(char * p); 
int ini_tty(char * aName,int aBaud);
int read_232(char * aBuf,int aNum);
int write_232(char * aBuf,int aNum);
void close_tty();
void setechoflag();
void clearechoflag();
void setpwd();
#endif

