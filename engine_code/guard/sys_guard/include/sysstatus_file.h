#ifndef _SYSSTATUS_FILE_H_
#define _SYSSTATUS_FILE_H_

#define CMD_SIZE  1024
//#define  DEVID_PATH     "/gms/doc/conf/uuid.cfg"
//#define  LIBVERSION_PATH     "/gms/guard/conf/libversion.cfg"
//#define  SYSVERSION_PATH     "/gms/guard/conf/sysversion.cfg"
#define  PATH       "/home/xiedongling/svn/GMS/trunk/engine_code/guard/conf/"
#define  TMP_STATUS_PATH       "/gms/guard/conf/tmp.status"
#define  SYSSTATUS_FILE_PATH       "/filedata/gms/comm/devstat/up/"

int read_cfg(char *path, char *name);
int make_sysfile(char *file_str);
int parse_xml(char *node_num, char *node_name);
int get_libversion(char *libversion);
int get_last_time(char *last_time);

#endif 
