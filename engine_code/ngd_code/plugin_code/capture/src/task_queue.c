
/*
 * $Log: task_queue.c,v $
 * Revision 1.1  2009/06/18 11:35:22  zzf
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2009/01/19 09:48:05  zdb
 *
 *
 * Revision 1.2  2006/02/09 03:38:02  whp
 * del ips dir
 *
 * Revision 1.2  2005/06/24 10:32:55  xh
 * *** empty log message ***
 *
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "common/dt_misc.h"
#include "capture/task_queue.h"

static tskque_t *st_task_queue = NULL;

tskque_t *tskque_new(int nmax, key_t key)
{
	tskque_t *que = NULL;
	int size = 0;
	int shmid = 0;

	if (nmax <= 0)
		goto err;

	size = offset_of(tskque_t, tq_task_table)+nmax*sizeof(task_t);
	if (key == 0) {
		/* alloc in process memory */
		que = (tskque_t *)calloc(1, size);
		if (!que)
			goto err;
		que->tq_shmid = -1;
		st_task_queue = que; /* keep in static variables */
	} else {
		/* alloc in share memory */
		shmid = shmget(key, size, IPC_CREAT|0600); /* writeable & readable */
		if (shmid < 0) {
			assert(1);
			perror("Error: shmget in tskque_new");
			goto err;
		}

		que = (tskque_t *)shmat(shmid, NULL, 0);
		assert(que);
		memset(que, 0, size);

		que->tq_shmid = shmid;
	}

	que->tq_task_nmax = nmax;

	return que;
err:
	if (que)
		free(que);

	return NULL;
}

void tskque_free(tskque_t *que)
{
	if (que) {
		if (que->tq_shmid == -1)
			free(que);
		else {
			shmdt(que);
			shmctl(que->tq_shmid, IPC_RMID, NULL);
		}
	}

	return;
}

tskque_t *tskque_open(key_t key)
{
	tskque_t *que = NULL;
	int shmid = -1;

	if (key == 0) {
		que = st_task_queue;
	} else {
		shmid = shmget(key, 0, IPC_CREAT|0600);
		if (shmid < 0) {
			assert(1);
			perror("Error: shmget in tskque_open");
			goto err;
		}

		que = (tskque_t *)shmat(shmid, NULL, 0);
		assert(que);
	}


	return que;
err:
	return NULL;
}

void tskque_close(tskque_t *que)
{
	if (que) {
		if (que->tq_shmid >= 0) {
			shmdt(que);
		}
	}

	return;
}

/* reader */
tskque_reader_t *tskque_reader_new(tskque_t *tskque)
{
	tskque_reader_t *reader = NULL;
	
	if (!tskque)
		goto err;

	reader = (tskque_reader_t *)calloc(1, sizeof(tskque_reader_t));
	if (!reader)
		goto err;

	tskque->tq_reader_num++;

	return reader;
err:
	if (reader)
		free(reader);
	return NULL;
}

void tskque_reader_free(tskque_t *tskque, tskque_reader_t *reader)
{
	if (tskque && reader) {
		free(reader);
		tskque->tq_reader_num--;
	}

	return;
}

/* writer */
tskque_writer_t *tskque_writer_new(tskque_t *tskque)
{
	tskque_writer_t *writer = NULL;
	
	if (!tskque)
		goto err;

	writer = (tskque_writer_t *)calloc(1, sizeof(tskque_writer_t));
	if (!writer)
		goto err;

	tskque->tq_writer_num++;
		
	return writer;
err:
	if (writer)
		free(writer);
	return NULL;
}

void tskque_writer_free(tskque_t *tskque, tskque_writer_t *writer)
{
	if (tskque && writer) {
		free(writer);
		tskque->tq_writer_num--;
	}
	return;
}

inline task_t *tskque_reader_getstuff(volatile tskque_t *que, volatile tskque_reader_t *reader)
{
	task_t *stuff = NULL;

	/* loop waiting for new stuff coming */
	while (que->tq_tail == reader->tqr_index) { 
#ifdef GPACK_REST
		rest();
#endif
	}

	stuff = (task_t *) que->tq_task_table + reader->tqr_index;

	return stuff;
}

inline int tskque_reader_putslot(tskque_t *queue, tskque_reader_t *reader)
{
	INCREASE_TSKQUE_READER_INDEX(queue, reader);

	return 0;
}

inline task_t *tskque_writer_getslot(tskque_t *que, tskque_writer_t *writer)
{
	task_t *task = NULL;

	task = que->tq_task_table + que->tq_tail;

	return task;
}

inline int tskque_writer_putstuff(tskque_t *que, tskque_writer_t *writer)
{
	INCREASE_TSKQUE_WRITER_INDEX(que, writer);
	INCREASE_TSKQUE_INDEX(que, que->tq_tail);

	return 0;
}
