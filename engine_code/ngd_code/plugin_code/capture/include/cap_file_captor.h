#ifndef _CAP_FILE_CAPTOR_H
#define _CAP_FILE_CAPTOR_H

#include <sys/types.h>
#include "captor.h"

long cap_file_captor_open(void *private_info, int argc, char **argv);
int cap_file_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
void cap_file_captor_close(void *private_info, long hdlr);

extern captor_t cap_file_captor;

#endif
