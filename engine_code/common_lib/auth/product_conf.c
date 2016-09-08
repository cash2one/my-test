#include "product_conf.h"
#include "misc.h"
#include "config.h"

product_path_t  product_path_cfg;

cfg_desc g_product_cfg[] = {
        {"product", "product_path", &(product_path_cfg.product_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/product_id.conf"},
        {"uuid", "uuid_path", &(product_path_cfg.uuid_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/uuid.conf"},
        { "","",NULL,0,0,0,0,""},
};

struct PRODUCT_CONF product_conf; 
static cfg_desc product_conf_info[] = {
    {"product", "product_code", product_conf.product_code, CFG_SIZE_INT4, CFG_TYPE_STR, 1, 0, ""},    
    {"product", "product_no", product_conf.product_no, CFG_SIZE_INT4, CFG_TYPE_STR, 1, 0, ""},    
    {"device", "production_year", product_conf.production_year, CFG_SIZE_INT4, CFG_TYPE_STR, 1, 0, ""},    
    {"device", "production_month", product_conf.production_month, CFG_SIZE_INT4, CFG_TYPE_STR, 1, 0, ""}, 
    { "","",NULL,0,0,0,0,""},
};
# if 1
int init_product_conf(struct PRODUCT_CONF *conf)
{
	if (!conf) {
		return -1;
	}
	
	memset(conf, 0, sizeof(struct PRODUCT_CONF));

	return 0;
}

void clean_product_conf(struct PRODUCT_CONF *conf)
{
	if (!conf)
		return;

	memset(conf, 0, sizeof(struct PRODUCT_CONF));

	return;
}

/*读取配置文件内容*/
int read_product_conf(char *conf_file_name, struct PRODUCT_CONF *conf)
{
	if(conf == NULL){	
		return -1;	
	}
	memset(conf, 0, sizeof(struct PRODUCT_CONF));
	if(common_read_conf(conf_file_name, product_conf_info) < 0){
		return -1;
	}
//	cfg_print(product_conf);
	return 0;
}
#endif









