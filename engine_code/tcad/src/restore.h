#ifndef __RESTORE_H__
#define __RESTORE_H__

#include "mbuf.h"
#include "tcad.h"

typedef struct restore
{
    uint16_t rid;
    char name[SAM_STR_SIZE+1];
    char buf[BUF_SIZE];
    char filename[128];
    FILE *fp;
    struct restore *next;
} restore_t;

uint32_t restore_add(uint16_t rid, char *name, char *buf);
uint32_t restore_del(uint16_t rid, char *buf);
uint32_t restore_mod(uint16_t rid, char *buf);
void check_restores(struct m_buf *mbuf);

#endif
