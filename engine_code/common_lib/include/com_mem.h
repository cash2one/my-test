#ifndef _COM_MEM_H_
#define _COM_MEM_H_

typedef enum {
	CSIGN	    = 0,
	SHELLGUI    = 1,
	COMM2	    = 2,
	CMAN	    = 3,
	CMDEASY	    = 4,
	EVT_SEND    = 5,
	TEST_READER = 6,
	READER_MAX
} shm_reader_t;

void	*init_mem(int aMode,int aKey,int aLength,int aWidth);
int	 ComPutMem(char *aMem, char *aBuf, unsigned short mlen, unsigned short mtype, unsigned int user);
int32_t	 ComGetMem(char *aMem, char *aBuf, unsigned short *mlen, unsigned short *mtype, unsigned int *user);
ssize_t	 get_mem(void *addr, unsigned int index, char *buff, unsigned int len);
int	 mem_first_index(void *addr);
int	 mem_last_index(void *addr);
int	 mem_del_data(void *addr);
ssize_t	 so_get_recent_shm(void *addr, shm_reader_t reader, char *buff, unsigned int len);

#endif /* _COM_MEM_H_ */
