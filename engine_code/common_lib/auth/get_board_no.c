#include "genkey.h"
#include"errno.h"

int get_board_no(char* board_no, int no_len)
{
	int ret1 ;
	FILE *fp;
	ret1 = system("dmidecode -t 1 | grep 'UUID'|awk -F \": | [ ]+\" '{print $2}' > /gms/auth/conf/tmp_id");
	if(ret1 < 0) {
		printf("error:%s\n", strerror(errno));
	} else if(WIFSIGNALED(ret1)) {
		printf("abnormal termination, signal number = %d\n", WTERMSIG(ret1));
	} else if(WIFSTOPPED(ret1)) {
		printf("process stopped, signal number = %d\n", WSTOPSIG(ret1));
	} else if(WIFEXITED(ret1)) {
		printf("normal termination, exit status = %d\n", WEXITSTATUS(ret1));
		if(NULL == (fp = fopen(TMP_ID_PATH,"rb"))){
			printf("cannot open file! \n");
	  		return -1;
		}	
		fseek(fp,0,SEEK_END);
        	int file_size;
        	file_size = ftell(fp);
        	printf("length:%d\n",file_size);
        	fseek(fp,0,SEEK_SET);
	 	if (fread(board_no, file_size, sizeof(char),fp) != 1) {
          		if (feof(fp)) {
                  		fclose(fp);
                  		return 0;
                	}
		} else {
			fclose(fp);
		}
		
		board_no[file_size - 1] = '\0';
		printf("board_len:%d\n", (int)strlen(board_no));
	//	printf("wwwwwwwwwwww:%s\n", board_no);
#if 0
        	int ret2;
        	ret2 = remove(TMP_ID_PATH);
        	if(ret2 < 0) {
			return -1;
		}
#endif
	}
	return 0 ;
}

#if 0
int main(void)
{
	char board_no[28];
	get_board_no(board_no,28);
	printf("board_no:%s\n",board_no);
}
#endif
