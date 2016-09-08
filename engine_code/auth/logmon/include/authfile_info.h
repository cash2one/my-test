#ifndef _AUTHFILE_INFO_H
#define _AUTHFILE_INFO_H
#define NUM 100
#define SIZE 100
#define AUTH_LEN 1024
#define AUTH_FILE_PATH "/gms/auth/conf/auth-32-timestamp.gau"
#define AUTH_CHECK_RET_PATH "/gms/conf/"

struct service{
        char name[30];
        char status[15];
};
struct FILE_INFO{
        char active_time[9];
        char serial_no[33];
        struct service srv_array[NUM];
};




extern struct FILE_INFO file_info;

int get_sign_name(char *name);
int get_sign_len(char *name, char *len);
int get_last_file(char *authfile_path, char *sign_len); 
int get_former_file(char *authfile_path, char *sign_len);
int rsa_verify(int *ret); 
int verify_decrypt(char *plainblock, char *sign_name, char *sign_len, char *authfile_path);
int split(char* str, char* delim, char* dest[SIZE], int* count);
int read_authfile(char *info);
char *substring(char *dst, char *src, int n, int m);
int get_file_info(struct FILE_INFO* file_info, char *info);
int isequal(char *str1,char *str2);
int get_authcheck_ret(int *ret2, char *info);
int get_sign_info(char *name, char *sign_len, char *sign_time);





#endif
     
