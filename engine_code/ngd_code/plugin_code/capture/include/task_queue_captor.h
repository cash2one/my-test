#ifndef _TASK_QUEUE_CAPTOR_H
#define _TASK_QUEUE_CAPTOR_H

#include <sys/types.h>
#include "captor.h"

long task_queue_captor_open(void *private_info, int argc, char **argv);
int task_queue_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
void task_queue_captor_close(void *private_info, long hdlr);

extern captor_t task_queue_captor;

#endif // _TASK_QUEUE_CAPTOR_H
