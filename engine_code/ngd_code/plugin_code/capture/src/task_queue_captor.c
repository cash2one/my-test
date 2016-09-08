#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "captor.h"
#include "task_queue.h"
#include "task_queue_captor.h"

captor_t task_queue_captor = {
	.name = "task_queue",
	.open = task_queue_captor_open,
	.close = task_queue_captor_close,
	.capture = task_queue_captor_capture,
};

static tskque_t *task_queue = NULL;
static tskque_reader_t *task_queue_reader = NULL;
static void *packet_buffer_base = NULL; 
static int worker_id = 0;
static int worker_num = 0;

long task_queue_captor_open(void *private_info, int argc, char **argv)
{
	key_t shmkey = 0;
	captor_t *cap = NULL;

	assert(argc == 3);
	worker_num = atoi(argv[2]);
	if (worker_num <= 0) {
		fprintf(stderr, "Error: task_queue_captor_open, worker_num can't less or equal than zero\n");
		goto err;
	}

	/* open task queue in shmmem */
	shmkey = (key_t)strtol(argv[0], NULL, 0);
	if (!shmkey)
		goto err;
	if ((task_queue = tskque_open(shmkey)) == NULL) {
		fprintf(stderr, "Error: task_queue_captor open task queue error\n");
		goto err;
	}
	task_queue_reader = tskque_reader_new(task_queue);
	assert(task_queue_reader);

	/* 
	 * use current reader num % total defined worker num in task queue as worker id,
	 * so if worker_num is defined as 4, than the 1st and 5th has the same worker id,
	 */ 
	worker_id = ((task_queue->tq_reader_num-1)%worker_num) + 1;

	/* map share packet buffer and get base address */
	cap = SearchCaptor(argv[1]);
	if (cap == NULL) {
		fprintf(stderr, "Error: task_queue_captor can't find captor(%s)\n", argv[1]);
		goto err;
	}
	if (!cap->mmap) {
		fprintf(stderr, "Error: captor(%s) didn't provide mmap function\n", argv[1]);
		goto err;
	}
	packet_buffer_base = (*cap->mmap)();
	assert(packet_buffer_base);

	return 0;
err:
	if (task_queue) {
		tskque_close(task_queue);
		task_queue = NULL;
	}
	if (cap && packet_buffer_base) {
		(*cap->munmap)(packet_buffer_base);
		packet_buffer_base = NULL;
	}
	return -1;
}

int task_queue_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	task_t *task = NULL;
	long offset;
	int buf_len = 0;

	/* get task */
	task = tskque_reader_getstuff(task_queue, task_queue_reader);
	assert(task);
	while (task->worker_id != worker_id) {
		tskque_reader_putslot(task_queue, task_queue_reader);
		task = tskque_reader_getstuff(task_queue, task_queue_reader);
		assert(task);
	}
	offset = task->packet_offset;
	buf_len = task->packet_len;
	task->worker_id = 0; /* set worker_id to 0, means this task has been handled by a worker */
	tskque_reader_putslot(task_queue, task_queue_reader);

	/* get addr */
	*pkt_buf_p = packet_buffer_base + offset;
	
	return buf_len;
}

void task_queue_captor_close(void *private_info, long hdlr)
{
	return;
}
