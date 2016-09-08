#ifndef _SHUGUANG_DRV_CAPTOR_H
#define _SHUGUANG_DRV_CAPTOR_H

#include <sys/types.h>
#include "captor.h"
#include "shuguang_libppf.h"

#define MAX_DT_NUM 16



extern int SHUGUANG_STREAM_NUM;
extern int shuguang_drvbuf_idx;
extern shuguang_slot_list shuguang_slot;
extern captor_t shuguang_drv_captor;
int open_ppf_drv(char *path);
void close_ppf_drv();

extern ppf_open_t                      ppf_open;
extern ppf_close_t                     ppf_close;
extern ppf_setStreamNum_t              ppf_setStreamNum;
extern ppf_getDataBlock_no_Lock_t      ppf_getDataBlock_no_Lock;
extern ppf_freeDataBlock_t             ppf_freeDataBlock;
extern ppf_getPktLength_t              ppf_getPktLength;
extern void                            *ppf_handle;
//int init_shuguang_drv_slot_list();
//int read_drv_config();

#endif // _SHUGUANG_DRV_CAPTOR_H
