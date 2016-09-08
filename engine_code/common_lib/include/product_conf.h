#ifndef _PRODUCT_CONF_H
#define _PRODUCT_CONF_H

#include"config.h"
//#define DEFAULT_PRODUCT_CONF_PATH     "/gms/auth/conf/product_id.conf"
#define CFG_SIZE_INT4 4
#define FILE_PATH_SIZE   1024
#define PRODUCT_CFG_PATH   "/gms/auth/conf/auth.conf"

struct PRODUCT_CONF {
	char product_code[CFG_SIZE_INT4];
	char product_no[CFG_SIZE_INT4];
	char production_year[CFG_SIZE_INT4];
	char production_month[CFG_SIZE_INT4];
};

typedef struct _product_path {
    char product_path[FILE_PATH_SIZE];
    char uuid_path[FILE_PATH_SIZE];

} product_path_t;

typedef struct _device_id_path {
    char device_id_path[FILE_PATH_SIZE];

} device_id_path_t;

extern product_path_t product_path_cfg;
extern device_id_path_t device_id_path_cfg;
extern cfg_desc     g_product_cfg[];
extern cfg_desc     g_device_id_cfg[];

extern struct PRODUCT_CONF product_conf;
int init_product_conf(struct PRODUCT_CONF *conf);
void clean_product_conf(struct PRODUCT_CONF *conf);
int read_product_conf(char *conf_file_name, struct PRODUCT_CONF *conf);
//int read_product_conf(char *conf_file_name, struct PRODUCT_CONF *conf);
#endif

