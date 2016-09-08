#ifndef _SPECIAL_DRV_CAPTOR_H
#define _SPECIAL_DRV_CAPTOR_H
#include <sys/types.h>
#include "captor.h"
#include "libpag.h"

#define MAX_DT_NUM 16

#define SPECIAL_CONFIG_FILE	 "./conf/shuguang_drv.conf"


int pag_drvbuf_idx ;
extern captor_t special_drv_captor;
extern pag_slot_list special_slot;

extern pag_open_t      pag_open;
extern pag_close_t     pag_close;
extern pag_get_t       pag_get;
extern pag_free_t      pag_free;
extern pag_time_t      pag_time;
extern void            *specila_handle;

int open_special_drv(char *path);
void close_special_drv();
#endif 
