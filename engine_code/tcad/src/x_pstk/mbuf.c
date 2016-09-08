#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include "misc.h"
#include "mbuf.h"
#include "libpag.h"

void mbuf_free(struct m_buf *mbuf)
{
    //int lcore_id;
    if(mbuf->pkt_addr != NULL)
    {
    /*
        PACKET_BUFF_RELEASE;
    */
        pag_free(mbuf->streamid, mbuf->pkt_addr);
        //atomic32_add((atomic32_t *)&packet_freecount ,1);
    }


    return;
}




