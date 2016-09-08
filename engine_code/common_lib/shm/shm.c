#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "shm.h"
#include "debug.h"

#define LONG_MAX     2147483647L

#define RI_START	1
#define WI_START	0

typedef struct _shm {
	unsigned int	init_flag;
	unsigned int	total;
	unsigned int	rollbak;
	unsigned int	seg_width;
	unsigned int	seg_num;
	unsigned int	wi;
	unsigned int   ri[READER_MAX];
} shm;

typedef struct _seg {
	unsigned short	mtype;
	unsigned int	user;
	unsigned int	data_len;   
	char		data[0];
} seg;

const char interp[] __attribute__((section(".interp"))) = "/lib/ld-linux.so.2";
void version(void)
{
	printf("libshm.so version 20090115\n");
	exit(0);
}

void *init_shm(int key, unsigned int seg_num, unsigned int seg_width)
{
	int		 id;
	void		*addr;
	shm		*shm_head;
	shm_reader_t	 reader;

	id = shmget(key, (seg_num + 1) * seg_width, IPC_CREAT|IPC_EXCL);
	if (id == -1) {
		id = shmget(key, (seg_num + 1) * seg_width, IPC_EXCL);
		if (id == -1) {
			return NULL;
		}
	}

	addr = shmat(id, NULL, 0);
	if (addr == (void *)-1) {
		return NULL;
	}

	shm_head = (shm *)addr;

	if (shm_head->init_flag == 0) {
		for (reader = 0; reader < READER_MAX; reader++) {
			shm_head->ri[reader] = RI_START;
		}
		shm_head->total	    = 0;
		shm_head->rollbak   = (LONG_MAX / seg_num) * seg_num;
		shm_head->seg_width = seg_width;
		shm_head->seg_num   = seg_num;
		shm_head->wi	    = WI_START;
		shm_head->init_flag = 1;
	}

	DBP("after shm init total: %lu width: %lu num: %lu wi: %lu\n",
	    shm_head->total,
	    shm_head->seg_width,
	    shm_head->seg_num,
	    shm_head->wi);
	for (reader = 0; reader < READER_MAX; reader++) {
		DBP("reader: %d ri: %lu \n",
		    reader,
		    shm_head->ri[reader]);
	}

	return addr;
}

int close_shm(void *addr)
{
	return shmdt(addr);
}

int del_all(void *addr)
{
	shm		*shm_head;
	unsigned int	 total = 0;
	shm_reader_t	 reader;

	if (addr == NULL) {
		return ERR_SHMADDR;
	}

	shm_head = (shm *)addr;

	total = shm_head->total;

	shm_head->total = 0;
	shm_head->wi	= WI_START;
	for (reader = 0; reader < READER_MAX; reader++) {
		shm_head->ri[reader] = RI_START;
	}

	return total;
}

int get_first_index(void *addr)
{
	shm *shm_head;

	if (addr == NULL) {
		return ERR_SHMADDR;
	}

	shm_head = (shm *)addr;

	if (shm_head->total > shm_head->seg_num) {
		return shm_head->total - shm_head->seg_num + 1;
	} else {
		return 1;
	}
}

int get_last_index(void *addr)
{
	shm *shm_head;

	if (addr == NULL) {
		return ERR_SHMADDR;
	}

	shm_head = (shm *)addr;
	
	if (shm_head->total == 0) {
		return 1;
	} else {
		return shm_head->total;	
	}
}

int get_last_empty_index(void *addr)
{
	shm *shm_head;

	if (addr == NULL) {
		return ERR_SHMADDR;
	}

	shm_head = (shm *)addr;

	if (shm_head->total == 0) {
		return 1;
	} else {
		return get_last_index(addr) + 1;
	}
}

ssize_t put_shm_type(void *addr, char *buff, unsigned int len, unsigned short mtype, unsigned int user)
{
	unsigned int	 copy_len;
	shm		*shm_head;
	seg		*seg_head;
	shm_reader_t	 reader;

	if (addr == NULL || buff == NULL) {
		return ERR_SHMADDR;
	}

	shm_head = (shm *)addr;

	seg_head = (seg *)(addr + shm_head->seg_width + shm_head->seg_width * shm_head->wi);
	copy_len = len <= (shm_head->seg_width - sizeof(seg)) ? len : (shm_head->seg_width - sizeof(seg));
	memcpy(seg_head->data, buff, copy_len);
	seg_head->data_len  = copy_len;

	seg_head->mtype = mtype;
	seg_head->user	= user;

	shm_head->wi++;
	if (shm_head->wi >= shm_head->seg_num) {
		shm_head->wi = WI_START;
	}

	shm_head->total++;
	if (shm_head->total > shm_head->rollbak) {
		shm_head->total = 0;

		for (reader = 0; reader < READER_MAX; reader++) {
			shm_head->ri[reader] = RI_START;
		}
	}

	return copy_len;
}

ssize_t put_shm(void *addr, char *buff, unsigned int len)
{
	return put_shm_type(addr, buff, len, 0, 0);
}

ssize_t get_shm_type(void *addr, unsigned int index, char *buff, unsigned int len, unsigned short *mtype, unsigned int *user)
{
	unsigned int	 copy_len;
	shm		*shm_head;
	seg		*seg_head;
	unsigned int	 ri;

	if (addr == NULL || buff == NULL) {
		return ERR_SHMADDR;
	}
 
	shm_head = (shm *)addr;

	if (shm_head->total == 0) {
		return ERR_SHMEMPTY;
	}

	if ((int)index < get_first_index(addr) ||
	    (int)index > get_last_index(addr)) {
		return ERR_SHMOUTRANGE;
	}

	ri = (index - 1) % shm_head->seg_num;
	seg_head = (seg *)(addr + shm_head->seg_width + shm_head->seg_width * ri);
	
	copy_len = len <= seg_head->data_len ? len : seg_head->data_len;
	memcpy(buff, seg_head->data, copy_len);

	if (mtype) {
		*mtype = seg_head->mtype;
	}
	if (user) {
		*user = seg_head->user;
	}

	return copy_len;
}

ssize_t get_shm(void *addr, unsigned int index, char *buff, unsigned int len)
{
	return get_shm_type(addr, index, buff, len, NULL, NULL);
}

ssize_t get_ref_shm(void *addr, int refer_index, unsigned int *next_index, char *buff, unsigned int len)
{
	int	index;
	int	first_index;
	ssize_t	ret;

	if (addr == NULL || buff == NULL) {
		return ERR_SHMADDR;
	}

	int last_index;
	last_index = get_last_index(addr);

	if (refer_index > get_last_index(addr)) {
		return ERR_SHMOUTRANGE;
	}

	first_index = get_first_index(addr);
	/* printf("get_ref_shm first_index: %ld \n", first_index); */
	index = refer_index < first_index ? first_index : refer_index;

	ret = get_shm(addr, index, buff, len);
	if (ret < 0) {
		/* printf("ret < 0---------\n"); */
		*next_index = refer_index;
	} else {
		/* printf("ret < 0 else ---------\n"); */
		*next_index = index + 1;
	}

	return ret;	
}

ssize_t get_last_shm(void *addr, char *buff, unsigned int len)
{
	unsigned int index = 0;
	ssize_t	ret = 0;

	if (addr == NULL || buff == NULL) {
		return ERR_SHMADDR;
	}

	index = get_last_index(addr);
	
	ret = get_shm(addr, index, buff, len);

	return ret;
}

ssize_t get_recent_shm_type(void *addr, shm_reader_t reader, char *buff, unsigned int len, unsigned short *mtype, unsigned int *user)
{
	shm	*shm_head;
	ssize_t	 ret;

	if (addr == NULL || buff == NULL) {
		return ERR_SHMADDR;
	}

	shm_head = (shm *)addr;

	if ((int)shm_head->ri[reader] < get_first_index(addr)) {
		shm_head->ri[reader] = get_first_index(addr);
	}

	ret = get_shm_type(addr, shm_head->ri[reader], buff, len, mtype, user);
	if (ret >= 0) {
		shm_head->ri[reader]++;
		if (shm_head->ri[reader] > shm_head->rollbak) {
			shm_head->ri[reader] = get_last_empty_index(addr);
		}
	}

	return ret;
}

ssize_t get_recent_shm(void *addr, shm_reader_t reader, char *buff, unsigned int len)
{
	return get_recent_shm_type(addr, reader, buff, len, NULL, NULL);
}

