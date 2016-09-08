#ifndef _DUMMY_CAPTOR_H
#define _DUMMY_CAPTOR_H
#include <sys/types.h>
#include "captor.h"
long dummy_captor_open(void *private_info, int argc, char **argv);
int dummy_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
void dummy_captor_close(void *private_info, long hdlr);

extern captor_t dummy_captor;
#endif // _DUMMY_CAPTOR_H
