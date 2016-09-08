//#include "genkey.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include "authfile_info.h"
#include"decrypt.h"
#include"logmon.h"
#include"genkey.h"
#include"auth_cfg.h"
#include"make_log.h"

char org_time[15];
char org_name[50];
struct FILE_INFO file_info;
int dest_num = 0;
int org_wretn = -1;
int main(int argc, char* argv[])
{
	int retnn;                                                                                                 
    retnn = common_read_conf(AUTH_CFG_PATH, g_auth_cfg);                                                     
    if (retnn < 0) {                                                                                               
        fprintf(stderr, "[-][AUTH_CONF] Read config %s ERROR!\n", AUTH_CFG_PATH);                               
    }
            
	char comm[2] = {0};
	char comm_name[] = {"cloudcon"};
	char r_num[2] = {0};
	char r_name[] = {"root"};
	parse_xml(comm, comm_name);
	printf("comm:%s\n", comm);

	parse_xml(r_num, r_name);	
	printf("r_num:%s\n", r_num);

/*	char reg_id[MAX_BUF_LEN]; 
	get_reg_id(reg_id);*/
#if 1
	char name[50] = {0};
	char authfile_path[1024] = {0};
    char plainblock[1024] = {0};
	int ret = 0, retn = 0;
    int xml_ret = 0;
    char sign_len[10] = {0};
    char sign_time[15] = {0};
	while(1) {
        xml_ret = is_file_exist(auth_conf_cfg.xml_path);
        if(xml_ret == 1) {
		memset(name, 0, 50);
        printf("----------------\n");
		get_sign_info(name, sign_len, sign_time);
        //printf("************flag:%d\n", flag);     
	/*	sprintf(sourcefile, "%s%s.gau", auth_conf_cfg.auth_check_path, name);
		printf("sourcefile:%s\n", sourcefile);
        printf("org_time:%s\n", org_time);*/
        //split_sign_name(name, sign_len, sign_time);

		//memset(sign_len, 0, 10);
		//get_sign_len(name, sign_len);
		//printf("sign_len:%s\n", sign_len);
		printf("org_name = %s, strlen(org_name) = %d\n", org_name, (int)strlen(org_name));
		if(strlen(org_name) == 0) {
		    strcpy(org_name,name);
			printf("qqqqqqqqqqqqqqqq org_name:%s\n", org_name);

		    sprintf(authfile_path, "%s%s.gau", auth_conf_cfg.auth_check_path, name);
		    printf("authfile_path:%s\n", authfile_path);
            printf("org_time:%s\n", org_time);

            int v_retn = 0;
			v_retn = verify_decrypt(plainblock, name, sign_len, authfile_path);
			//DES_Decrypt(sourcefile, KEY, OBJECT_FILE);	
			if(v_retn == 0) {
			    printf("-----------------the first!---------------\n");
                //get_file_info(&file_info, plainblock);
                get_authcheck_ret(&ret, plainblock);
                //printf("file_info->active_time:%s\n", file_info.active_time);
                
                
                //printf("===========ac_time:%d\n", ac_retn);
			    //get_file_info(&file_info); 
			        printf("++++++++++++++++ret++++++++++++++ = %d\n" , ret);	
			        if(ret == 0) {
				        CA_LOG(AUTH_MODULE, LOGMON_PROC, "authret=%d, auth suc!\n", ret);
				        printf(";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
				        //printf("file_info.srv_array[0].name: %s\n", file_info.srv_array[0].name);
				        check_all_process();
			        } else {
				        CA_LOG(AUTH_MODULE, LOGMON_PROC, "authret=%d, auth failed!\n", ret);
                } 
        }

        } else {
			retn = strcmp(org_name, name);
			if(retn != 0) {
				CA_LOG(AUTH_MODULE, LOGMON_PROC, "authfile changed! original name:%s, now name:%s\n", org_name, name);
			    
                printf("-----------changed!---------------------\n");
				memset(org_name, 0, 50);
				strncpy(org_name, name, (int)strlen(name));
                printf("org_name:%s\n", org_name);
				sprintf(authfile_path, "%s%s.gau", auth_conf_cfg.auth_check_path, name);
                printf("authfile_path:%s\n", authfile_path);
                memset(plainblock, 0, 1024); 
                verify_decrypt(plainblock, name, sign_len, authfile_path);
			    //	DES_Decrypt(sourcefile, KEY, OBJECT_FILE);
			    memset(&file_info, 0, sizeof(file_info));
                get_authcheck_ret(&ret, plainblock);
				//get_file_info(&file_info); 
				if(ret == 0) {
					printf("start check all process\n");
					check_all_process();
                } else {
                    CA_LOG(AUTH_MODULE, LOGMON_PROC, "authcheck failed!\n");
                }
			} else {
                //get_authcheck_ret(&ret);
                    if(ret == 0) {
				        printf("---------------the second time!-----------------------\n");
				        //get_file_info(&file_info, plainblock);
				        check_all_process();
                    } else {
                        CA_LOG(AUTH_MODULE, LOGMON_PROC, "authret=%d, auth failed!\n", ret);
                    }
			    }

			printf("><>>>>>>>>>>>>>>>>>>>>>>>.\n");
        }
    } else {
        CA_LOG(AUTH_MODULE, LOGMON_PROC, "%s file is not exist\n", auth_conf_cfg.xml_path);
    }	
               
		sleep(1);
	}
#endif
	return 0;
}
