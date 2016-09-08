#ifndef _SHM_H_
#define _SHM_H_

#define ERR_SHMADDR	-1
#define ERR_SHMOUTRANGE -2
#define ERR_SHMEMPTY	-3

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

#ifdef __cplusplus
extern "C"{
#endif
void*	init_shm(int key, unsigned int seg_num, unsigned int seg_width);
int	close_shm(void *addr);
ssize_t get_ref_shm(void *addr, int refer_index, unsigned int *next_index, char *buff, unsigned int len);
ssize_t get_last_shm(void *addr, char *buff, unsigned int len);
int	get_last_empty_index(void *addr);
int	del_all(void *addr);
int	get_first_index(void *addr);
int	get_last_index(void *addr);
ssize_t get_shm(void *addr, unsigned int index, char *buff, unsigned int len);
ssize_t get_shm_type(void *addr, unsigned int index, char *buff, unsigned int len, unsigned short *mtype, unsigned int *user);
ssize_t put_shm(void *addr, char *buff, unsigned int len);
ssize_t put_shm_type(void *addr, char *buff, unsigned int len, unsigned short mtype, unsigned int user);
ssize_t get_recent_shm(void *addr, shm_reader_t reader, char *buff, unsigned int len);
ssize_t get_recent_shm_type(void *addr, shm_reader_t reader, char *buff, unsigned int len, unsigned short *mtype, unsigned int *user);

#ifdef __cplusplus
}
#endif

#endif /* _SHM_H_ */

