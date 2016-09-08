#ifndef _GENKEY_H_
#define _GENKEY_H_

#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>
#define DEVICE_ID_CFG_PATH   "/gms/auth/conf/auth.conf"
#define ID_PATH "/gms/auth/conf/device.id"
#define TMP_ID_PATH "/gms/auth/conf/tmp_id"
#define CPU_LEN 28
#define BOARD_LEN 39
#define ORG_DEVICE_ID_LEN 100
#define DEVICE_ID_LEN 32

void cpu(unsigned int veax1);
void LM(unsigned int var, uint32_t *vx);
int get_cpu_id(char* cpu_id, int id_len);
int get_board_no(char* board_id, int id_len);
int make_serial_no(char* serial_num);
int genkey(char* id, int id_len);
char *trim(char *src);

#endif
