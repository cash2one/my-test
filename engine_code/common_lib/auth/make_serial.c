#include "genkey.h"

int make_serial_no(char* serial_no)
{

	char id[57], cpu_id[29], board_no[29];
	get_board_no(board_no,29);
	printf("board_no[%d]:%s\n",(int)strlen(board_no),board_no);
	get_cpu_id(cpu_id,29);
	printf("cpu_id[%d]:%s\n", (int)strlen(cpu_id),cpu_id);
	memset(id,0,57);
	strcat(id,board_no);
	strcat(id,cpu_id);
//	printf("id[%d]:%s\n",(int)strlen(id),id);
	strncpy(serial_no, id, 57);
	printf("serial_no:%s\n", serial_no);	
	return 0 ;	
}
