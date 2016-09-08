#include <string.h>  //wdb_ppp
#include "sharemem.h"






int sharemem_del(unsigned int key)
{
	return shmctl(key, IPC_RMID, NULL) ;
}
uint32_t sharemem_create(uint32_t key, uint32_t size, void **shareaddr)
{
	void *addr;
	int shmid;

	shmid = shmget(key, size, IPC_CREAT|0777);
	if(-1 == shmid)
	{
		printf("create share mem err\n");
		printf("%s\n", strerror(errno));
		return 1;
	}
	
	addr = shmat(shmid, NULL, 0);
	if((void *)-1 == addr)
	{
		printf("get share mem addr err\n");
		return 1;
	}

	*shareaddr = addr;

	return 0;
}


void *sharemem_addr_get(int key)
{
	void *addr;
	int shmid;
	
	shmid = shmget(key, 0, 0|00777); 
	if(-1 == shmid)
	{
		printf("get share mem area err\n");
		return NULL;
	}
	

	addr = shmat(shmid, 0, 0);
	if((void *)-1 == addr)
	{
		printf("get share mem error\n");
		return NULL;
	}

	return addr;	
}







