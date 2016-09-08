#ifndef __SHAREMEM_H__
#define __SHAREMEM_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>





#define SHAREMEM_ID_MONITOR 0x0001079b






extern uint32_t sharemem_create(uint32_t key, uint32_t size, void **shareaddr);
extern int sharemem_del(unsigned int key);




#endif
