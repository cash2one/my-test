/*
 * $Id: cap_file_captor.c,v 1.10 2009/08/12 02:12:42 dengwei Exp $
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "captor.h"
#include "cap_file_captor.h"
#include "cap_record.h"
//#include "trace_api.h
#include "misc.h"

#define MAX_CAP_FILENAME_LEN 256
#define EMSG printf
#define DMSG printf

static int init_shm(void);
static void clean_shm(void);
static void *cap_file_captor_getbase();
static int cap_file_captor_munmap(void *address);
void *cap_file_captor_mmap();

captor_t cap_file_captor = {
	name: "cap_file",
	open: cap_file_captor_open,
	close: cap_file_captor_close,
	capture: cap_file_captor_capture,
	mmap:cap_file_captor_mmap,
	munmap:cap_file_captor_munmap,
	getbase:cap_file_captor_getbase,
};

static u_int8_t raw_packet_space[RAW_PACKET_LEN];

#define CAPFILE_SHM_SIZE (4096 * 4096)
#define CAPFILE_SHM_KEY 0x19760101

static char *shm_buff = NULL;
static int shm_id = -1;

long cap_file_captor_open(void *private_info, int argc, char **argv)
{
	int rd = 0;

	if(argc != 1) {
		EMSG("Error: cap_file arg syntax: filename");
		goto err;

	}
        
	if ((rd = record_open(argv[0],  G_RECORD_PLAY, 0)) == -1) {
                EMSG("Error: record_open");
                goto err;
        }

	return rd;
err:
	return -1;
}

int cap_file_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	int nread = 0;
	static char *pfree = NULL;
	static int count = 0;

	do {
		nread = record_play(hdlr, raw_packet_space, sizeof(raw_packet_space));
		count++;
	} while (nread == -2);	// item's (len != len1)

	DMSG("nread: %d\n", nread);
	if (nread == 0) {
		return 0; /* is the same as exit */
	} else if (nread < 0) {
		return -1;
	}

	printf("\n=================== PACKET NO.%d ==================\n", count);

     	if (shm_buff == NULL) {
                *pkt_buf_p = raw_packet_space;
        } else { /* for smp */ 
                if (pfree == NULL)
                        pfree = shm_buff;
                
                if (CAPFILE_SHM_SIZE - (pfree - shm_buff) < nread)
                        pfree = shm_buff;
                
                memcpy(pfree, raw_packet_space, nread);
                *pkt_buf_p = (u_int8_t *)pfree;
                pfree += nread;
        }

	return nread;
}

void cap_file_captor_close(void *private_info, long hdlr)
{
        if (hdlr != -1)
		record_close(hdlr);

	clean_shm();
	return;
}

void *cap_file_captor_getbase()
{
	if (shm_buff == NULL) {
		if (init_shm() < 0)
			return NULL;
	}

	return (void *) shm_buff;
}

void *cap_file_captor_mmap()
{
	char *addr = NULL;
	int id;

	id = shmget(CAPFILE_SHM_KEY, CAPFILE_SHM_SIZE, IPC_CREAT);
	if (id < 0) {
		perror("shmget");
		goto err;
	}

	addr = shmat(id, 0, 0);
	if (addr == (char *) -1) {
		perror("shmat");
		goto err;
	}
	
	return (void *) addr;
err:
	return NULL;
}

int cap_file_captor_munmap(void *address)
{
	return shmdt(address);
}

int init_shm(void)
{
	shm_id = shmget(CAPFILE_SHM_KEY, CAPFILE_SHM_SIZE, IPC_CREAT | 0x1c0);
	if (shm_id < 0) {
		perror("shmget");
		goto err;
	}

	shm_buff = shmat(shm_id, 0, 0);
	if (shm_buff == (char *) -1) {
		perror("shmat");
		goto err;
	}
	
	return 0;
err:
	if (shm_buff)
		shmdt(shm_buff);
	if (shm_id >= 0)
		shmctl(shm_id, IPC_RMID, 0);
	shm_id = -1;
	shm_buff = NULL;
	return -1;
}

void clean_shm(void)
{
	if (shm_buff)
		shmdt(shm_buff);
	if (shm_id >= 0)
		shmctl(shm_id, IPC_RMID, 0);
	shm_id = -1;
	shm_buff = NULL;
	return;
}
