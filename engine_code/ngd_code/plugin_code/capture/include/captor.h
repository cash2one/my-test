#ifndef _CAPTOR_H
#define _CAPTOR_H

#define RAW_PACKET_LEN          2000
//#include "shuguang_drv_captor.h"
//#include "special_drv_captor.h"
typedef long(*captor_open_t)(void *private_info, int argc, char **argv);
typedef int(*captor_capture_t)(void *private_info, long handler, u_int8_t **pkt_buf_p);
typedef void(*captor_close_t)(void *private_info, long handler);
typedef void*(*captor_getbase_t)();
typedef void*(*captor_mmap_t)();
typedef int (*captor_munmap_t)(void *address);
typedef void (*captor_cleanctl_t)(void *private_info);

#define CAPTOR_NAME_LEN	64
typedef struct captor_struct {
	char name[CAPTOR_NAME_LEN];
	captor_open_t		open;
	captor_close_t		close;
	captor_capture_t	capture;
	captor_getbase_t	getbase;
	captor_mmap_t		mmap;
	captor_munmap_t		munmap;
	captor_cleanctl_t	cleanctl;
}captor_t;

#define MAX_SHUGUANG_MEM 16
typedef struct _shuguang_slot_list {
	int slot_num;
	int read_pos;
	int slot_list[MAX_SHUGUANG_MEM];
	int thread_idx;
	void *ego;
} shuguang_slot_list;

//由于程序在配置抓包方式为曙光驱动时，每个Dt可以读写多个内存
#define MAX_SPECIAL_MEM 16
typedef struct _pag_slot_list
{
	int slot_num;
	int read_pos;
	int slot_list[MAX_SPECIAL_MEM];
	int thread_idx;
	int stream_idx;
	void *ego;
}pag_slot_list;

typedef struct captor_desc {
    captor_t *captor;
    long handler;
    int thread_idx;

    union {
	shuguang_slot_list  shuguang_slot_info;
	pag_slot_list	    pag_slot_info;
    }un;
} captor_desc_t;

#define shuguang_slot_info un.shuguang_slot_info
#define pag_slot_info	un.pag_slot_info

/*type of captor*/
enum {
	CAPTOR_GENERAL = 0,
	CAPTOR_SHUGUANG,
	CAPTOR_SPECIAL,
};



#endif // _CAPTOR_H
