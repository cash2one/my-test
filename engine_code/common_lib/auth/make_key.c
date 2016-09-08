#include "genkey.h"
#include "product_conf.h"
#include "md5.h"



int read_cfg(char *path, char *name)
{
    FILE *fp;
    if(NULL == (fp = fopen(path, "rb"))) {
        printf("cannot open file!\n");
        return -1;
    } else {
        fseek(fp, 0, SEEK_END);
        int file_size;
        file_size = ftell(fp);
        fseek(fp,0,SEEK_SET);
        if(fread(name, file_size, sizeof(char),fp) !=1) {
            if(feof(fp)) {                                                                                        
                fclose(fp);
                return 0;
            }
    } else {
        fclose(fp);
    }
    name[file_size - 1] = '\0';
    printf("name:%s\n", name);                                                                                    
    }   
    return 0;                                                                                                     
}

int genkey(char* id, int id_len)
//int main(int argc,char* argv[])
{

	/* 读取产品配置文件 */

	char org_device_id[ORG_DEVICE_ID_LEN];
    int retn = 0;
    retn = common_read_conf(PRODUCT_CFG_PATH, g_product_cfg);
    if (retn < 0) {
        fprintf(stderr, "[-][AUTH_CONF] Read config %s ERROR!\n", PRODUCT_CFG_PATH);

    }    

	int ret;
	ret = read_product_conf(product_path_cfg.product_path, &product_conf);
 	if(ret < 0){
		printf("read conffig [%s] error!\n", product_path_cfg.product_path);
	}
	
	char cpu_id[CPU_LEN];
    char uuid[37] = {0};
	get_cpu_id(cpu_id, CPU_LEN);
	printf("cpu_id[%d]:%s\n", (int)strlen(cpu_id),cpu_id);
	//board_no[BOARD_LEN] = '\0';
	//get_board_no(board_no, BOARD_LEN);	
	//printf("board_no[%d]:%s\n",(int)strlen(board_no),board_no);

    read_cfg(product_path_cfg.uuid_path, uuid);

	memset(org_device_id, 0, ORG_DEVICE_ID_LEN);
	sprintf(org_device_id, "%s%s%s%s%s%s", product_conf.product_code,
				           product_conf.product_no,
                           product_conf.production_year, 
				           product_conf.production_month, 
				     	   uuid,
					       cpu_id   
					);
	#if 0
	strncpy(device_id, product_conf.product_code,2);
	strcat(device_id, product_conf.product_no);
	strcat(device_id, product_conf.production_year);
	strcat(device_id, product_conf.production_month);
	
	printf("product_conf:%s\n",device_id);
	
	/* 制作原始设备序列号*/

	char serial_num[57];
	make_serial_no(serial_num);
//	printf("serial_num is%s\n", serial_num);
	strcat(device_id,serial_num);
	#endif
	printf("original_device_id:%s\n",org_device_id);
	printf("len = %d\n", (int)strlen(org_device_id));
	
	/*md5*/

        unsigned char* szinput = (unsigned char*)malloc(strlen(org_device_id)+1);
	    memset(szinput, 0, strlen(org_device_id)+1);
    unsigned char szoutput[16];
        char* szbuffer = (char*)malloc(sizeof(char)*33);
	memset(szbuffer, 0, 33);
	
        strncpy((char*)szinput,org_device_id, strlen(org_device_id));
        MessageDigest(szinput,strlen((const char*)szinput),szoutput);
        int i = 0;
/*将szoutput[]中的16进制转换为字符形式输出*/
        for(i=0; i < 16; i++){
                sprintf(&szbuffer[i*2], "%02X", *(szoutput+i));

        }
	
        //printf("device_id is %s\n",szbuffer);

	strncpy(id, szbuffer, 32);
	id[32] = '\0';

	printf("id_len:%d\n", (int)strlen(id));
	printf("my id: %s\n", id);
	
	free(szbuffer);
	free(szinput);
	return 0;
}

