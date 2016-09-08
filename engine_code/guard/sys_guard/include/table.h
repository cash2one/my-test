#ifndef _TABLE_H_
#define _TABLE_H_
/*表名*/
#define TABLE_NAME_SYS_STATUS        	      "t_event_sysstatus"

#define TABLE_NAME_SIZE                                         (30)                                              
#define TABLE_SUFFIX                                            "_p"                                              
#define TABLE_SUFFIX_SIZ                                        (3)     

#define CMD_LEN					1024 

int store_sysstatus(char *insert_cmd);
#endif
