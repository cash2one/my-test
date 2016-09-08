#include <stdio.h>
#include "genkey.h"
#include"config.h"
#include"product_conf.h"

device_id_path_t  device_id_path_cfg;

cfg_desc g_device_id_cfg[] = {
        {"device_id", "device_id_path", &(device_id_path_cfg.device_id_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/conf/device.id"},
        { "","",NULL,0,0,0,0,""},
};

int main(int argc, char* argv[])
{
	char device_id[DEVICE_ID_LEN];
	//memset(device_id, 0, DEVICE_ID_LEN);
	genkey(device_id, DEVICE_ID_LEN);
	printf("device_id:%s\n",device_id);
	
    int retn = 0;
    retn = common_read_conf(DEVICE_ID_CFG_PATH, g_device_id_cfg);
    if (retn < 0) {
        fprintf(stderr, "[-][AUTH_CONF] Read config %s ERROR!\n", DEVICE_ID_CFG_PATH);

    }
                                                                                                               
    FILE *fp; 
    printf("path:%s\n", device_id_path_cfg.device_id_path);   
	fp = fopen(device_id_path_cfg.device_id_path, "w");     
        if( NULL == fp){                                                                                     
        	printf("cannot open file! \n");
       		return -1;                                                                                                   
        }   

       	if(fwrite(device_id, strlen(device_id), 1, fp) != 1) {                                                                        
        	printf("file write error! \n");                                                                        
            	fclose(fp);                    
	} else {     
		fclose(fp);
	} 
	return 0;
}
