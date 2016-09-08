#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "capture/captor.h"
#include "capture/dummy_captor.h"

captor_t dummy_captor = {
	name: "dummy",
	open: dummy_captor_open,
	close: dummy_captor_close,
	capture: dummy_captor_capture
};

long dummy_captor_open(void *private_info, int argc, char **argv)
{
	return 0;
}

int dummy_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	sleep(1);
	printf("Dummy captor capture\n");
	return 0;
}

void dummy_captor_close(void *private_info, long hdlr)
{
	return;
}
