#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <limits.h>

#include "com_mem.h"
#define SHM_SO	"./libshm.so"

static void *(*init_shm)(int key, unsigned int seg_num, unsigned int seg_width)	   = NULL;
static ssize_t (*put_shm)(void *addr, char *buff, unsigned int len)			   = NULL;
static ssize_t (*get_shm)(void *addr, unsigned int index, char *buff, unsigned int len)	   = NULL;
static int (*get_first_index)(void *addr)							   = NULL;
static int (*get_last_index)(void *addr)							   = NULL;
static int (*del_all)(void *addr)								   = NULL;
static ssize_t (*get_recent_shm)(void *addr, shm_reader_t reader, char *buff, unsigned int len) = NULL;
static void *(*init_shm)(int key, unsigned int seg_num, unsigned int seg_width);

void *init_mem(int aMode,int aKey,int aLength,int aWidth)
{
	void	*hd    = NULL;
	char	*error = NULL;
	void	*addr  = NULL;
	printf("uuuuuuuuuuuuuuuuuuuu");

	hd = dlopen(SHM_SO, RTLD_LAZY);
	if (!hd) {
		fprintf(stderr, "%s\n", dlerror());
		printf("ddddddddddd\n");
		return NULL;
	}

	dlerror();
	init_shm = dlsym(hd, "init_shm");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "init_shm %s\n", dlerror());
		return NULL;
	}

	dlerror();
	init_shm = dlsym(hd, "init_shm");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "init_shm %s\n", dlerror());
		return NULL;
	}

	dlerror();
	del_all = dlsym(hd, "del_all");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "del_all %s\n", dlerror());
		return NULL;
	}

	dlerror();
	get_first_index = dlsym(hd, "get_first_index");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "get_first_index %s\n", dlerror());
		return NULL;
	}

	dlerror();
	get_last_index = dlsym(hd, "get_last_index");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "get_last_index %s\n", dlerror());
		return NULL;
	}

	dlerror();
	get_shm = dlsym(hd, "get_shm");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "get_shm %s\n", dlerror());
		return NULL;
	}

	dlerror();
	put_shm = dlsym(hd, "put_shm");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "put_shm %s\n", dlerror());
		return NULL;
	}

	dlerror();
	get_recent_shm = dlsym(hd, "get_recent_shm");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "get_recent_shm %s\n", dlerror());
		return NULL;
	}

	addr = init_shm(aKey, aLength, aWidth);
	if (addr == NULL) {
		fprintf(stderr, "init_shm %s\n", dlerror());
		return NULL;
	}

	return addr;	
}


ssize_t so_get_recent_shm(void *addr, shm_reader_t reader, char *buff, unsigned int len)
{
	ssize_t ret;
	ret =  get_recent_shm(addr, reader, buff, len);
	/* printf("ret-----------: %d \n", ret); */
	return ret;
}

int ComPutMem(char *aMem, char *aBuf, unsigned short mlen, unsigned short mtype, unsigned int user)
{
	return put_shm(aMem, aBuf, mlen);
}

int ComGetMem(char *aMem, char *aBuf, unsigned short *mlen, unsigned short *mtype, unsigned int *user)
{

	/* 
         * ret   = get_shm(aMem, aBuf, 6000);
	 * *mlen = ret;
         */

	return 0;
}

ssize_t get_mem(void *addr, unsigned int index, char *buff, unsigned int len)
{
	return get_shm(addr, index, buff, len);
}

int mem_first_index(void *addr)
{
	return get_first_index(addr);
}

int mem_last_index(void *addr)
{
	return 0;
}

int mem_del_data(void *addr)
{
	return del_all(addr);
}
