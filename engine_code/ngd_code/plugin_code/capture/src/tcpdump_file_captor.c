/* 
 * $Id: tcpdump_file_captor.c,v 1.14 2010/07/02 06:16:54 zyl Exp $
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "misc.h"
#include "captor.h"
#include "tcpdump_file_captor.h"
//
//#include "trace_api.h"

#define RAW_SHM_SIZE 4096 * 4096
#define RAW_SHM_KEY 0x19760916

static int init_shm(void);
static void clean_shm(void);
static void *tcpdump_file_captor_getbase();
static int tcpdump_file_captor_munmap(void *address);
void *tcpdump_file_captor_mmap();
static long tcpdump_file_captor_open(void *private_info, int argc, char **argv);
static int tcpdump_file_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
static void tcpdump_file_captor_close(void *private_info, long hdlr);

/*
 * FORMAT
 *
 * Input files hava a 24 byte header starting with A1 B2 C3 D4 hex.
 * This is followed by a series of records of 76-1530 bytes as follows.
 * The last record may be incomplete.

 * Bytes   Contents (numbers are most significant byte (MSB) first)
 * 0-3     Time in seconds since 0000 Jan 1 1970 UCT
 * 4-7     Time in microseconds, 0-999999
 * 8-11    Record length - 16 (60-1514)
 * 12-15   Second copy of length, must be identical
 */

struct tcpdump_record_head {
	unsigned int time_seconds;
	unsigned int time_microsecondes;
	unsigned int len;
	unsigned int len_copy;
};
	
captor_t tcpdump_file_captor = {
	.name = "tcpdump_file",
	.open = tcpdump_file_captor_open,
	.close = tcpdump_file_captor_close,
	.capture = tcpdump_file_captor_capture,
	.mmap = tcpdump_file_captor_mmap,
	.munmap = tcpdump_file_captor_munmap,
	.getbase = tcpdump_file_captor_getbase,
};

static u_int8_t	raw_packet_space[RAW_PACKET_LEN];

static char *shm_buff = NULL;
static int shm_id = -1;

// #ifdef DEBUG_DUMP
int record_count = 0;
// #endif

long tcpdump_file_captor_open(void *private_info, int argc, char **argv)
{
	int fd = -1;
	char file_head[24];

	if (argc != 1) {
		DMSG("Error: Tcpdump file arg syntax: filename\n");
		goto err;
	}

	fd = open(argv[0], O_RDONLY);	
	if (fd == -1) {
		DMSG("wrong argument: %s", argv[0]);
		goto err;
	}

	if (read(fd, file_head, sizeof(file_head)) != sizeof(file_head)) {
		DMSG("Error: invalid tcpdump file\n");
		goto err;
	}

	if (memcmp(file_head,"\xd4\xc3\xb2\xa1", 4) != 0) { 
		DMSG("Error: Unknown tcpdump file version\n");
		goto err;
	}

// #ifdef DEBUG_DUMP
	record_count = 0;
// #endif
	return fd;
err:
	if (fd != -1)
		close(fd);
	return -1;
}

int tcpdump_file_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	static char *pfree = NULL;
	struct tcpdump_record_head record_head;

	if (hdlr == -1)
		goto err;

	if (read(hdlr, &record_head, sizeof(record_head)) != sizeof(record_head))
		goto complete;

	if (record_head.len != record_head.len_copy) {
		fprintf(stderr, "Error:  record length error\n");
		goto err;
	}
	
	if(record_head.len >= RAW_PACKET_LEN) {
#ifdef DEBUG_DUMP
		fprintf(stderr, "Error: record_head.len TOO LARGE in Packet:%d\n", ++record_count);
#endif
		goto complete;
	}
	if (read(hdlr, raw_packet_space, record_head.len) != (long)record_head.len) {
		goto complete;
	}


	if (shm_buff == NULL) {
		*pkt_buf_p = raw_packet_space;
	} else { /* for smp */
		if (pfree == NULL)
			pfree = shm_buff;
		
		if (RAW_SHM_SIZE - (pfree - shm_buff) < (int)record_head.len)
			pfree = shm_buff;

		memcpy(pfree, raw_packet_space, record_head.len);
		*pkt_buf_p = (u_int8_t *)pfree;
		pfree += record_head.len;
	}

	printf("============ NO. %d ============\n", ++record_count);
#ifdef DEBUG_DUMP
	printf("============ NO. %d ============\n", ++record_count);
	printf("record_head.len = %u\n", record_head.len);
	printf("-----------------------------------------------\n");
//	print_buf2((char *)raw_packet_space, record_head.len, 0);
	printf("\n");
#endif
	return record_head.len;
err:
	if (hdlr != -1)
		close(hdlr);
	return -1;
complete:
	if (hdlr != -1)
		close(hdlr);

/* 	char cmd[256];
 * 	sprintf(cmd, "kill %d", getpid());
 * 	system(cmd);
 */
	return 0;
}

void tcpdump_file_captor_close(void *private_info, long hdlr)
{	
	if (hdlr != -1)
		close(hdlr);

	clean_shm();
	return;
}

void *tcpdump_file_captor_getbase()
{
	if (shm_buff == NULL) {
		if (init_shm() < 0) 
			return NULL;
	}

	return (void *) shm_buff;
}

void *tcpdump_file_captor_mmap()
{
	char *addr = NULL;
	int id;

	id = shmget(RAW_SHM_KEY, RAW_SHM_SIZE, IPC_CREAT);
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

int tcpdump_file_captor_munmap(void *address)
{
	return shmdt(address);
}

int init_shm(void)
{
	shm_id = shmget(RAW_SHM_KEY, RAW_SHM_SIZE, IPC_CREAT | 0x1c0);
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
