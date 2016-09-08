#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "com_mem.h"

#define BUFF_LEN 5

static int	shm_key	  = 0x2f88;
static int	shm_num	  = 500;
static int	shm_width = 100;

static void write_shm(int num)
{
	char		*w_shm = NULL;
	char		 buf[BUFF_LEN];
	int		 ret;
	unsigned short	 type  = 9;
	int	 i;
	int		 j;

	if (num < 0) {
		printf("num must > 0!\n");
		return;
	}

	printf("start write shm num: %ld \n", num);

	w_shm = (char *)init_mem('w', shm_key, shm_num, shm_width);
	if (w_shm == NULL) {
		printf("init_mem error!!! \n");
		return;
	}

	for (i = 1; i <= num; i++) {
		for (j = 0; j < BUFF_LEN; j++) {
			buf[j] = 9;
		}
		
		ret = ComPutMem(w_shm, (char *)buf, sizeof(buf), type, 0);
		/* printf("ComPutMem ret: %d \n", ret); */
		if (ret != 0) {
			printf("num: %ld write data to shm success\n", i);
		} else {
			printf("num: %ld write data to shm fault\n", i);
			return;
		}
	}
	return;
}

static void writec_shm(int num)
{
	char		*w_shm = NULL;
	char		 buf[BUFF_LEN];
	int		 ret;
	unsigned short	 type  = 9;
	int		 i;
	/* long		 j; */
	unsigned int	 total = 0;

	if (num < 0) {
		printf("num must > 0!\n");
		return;
	}

	printf("start write shm num: %ld \n", num);

	w_shm = (char *)init_mem('w', shm_key, shm_num, shm_width);
	if (w_shm == NULL) {
		printf("init_mem error \n");
		return;
	}

	while (1) {
		for (i = 1; i <= num; i++) {
			/* 
                         * for (j = 0; j < BUFF_LEN; j++) {
			 * 	buf[j] = 9;
			 * }
                         */
		
			ret = ComPutMem(w_shm, (char *)buf, sizeof(buf), type, 0);
			/* printf("ComPutMem ret: %d \n", ret); */
			if (ret != 0) {
				total++;
				printf("num: %ld write data to shm success total: %lu \n", i, total);
			} else {
				printf("num: %ld write data to shm fault\n", i);
				return;
			}
		}
		sleep(1);
	}

	return;
}

static void readc_shm()
{
	char		*r_shm;
	unsigned short	 read_len;
	unsigned short	 type;
	char		 buf[BUFF_LEN];
	int		 ret;
	int		 i;
	int              read_times = 0;
	unsigned int	 user;

	printf("start read shared memory:\n");

	r_shm = (char *)init_mem('r', shm_key, shm_num, shm_width);
	printf("3333333ddd : %x \n", (int)r_shm);

	/* init buf */
	for (i = 0; i < BUFF_LEN; i++) {
		buf[i] = 0;
		/* printf("bf[%d]: %d  ", i, buf[i]); */
	}
	printf("\n\n");

	/* read the mem */
	while (1) {
		read_len = 0;
		for (i = 0; i < BUFF_LEN; i++) {
			buf[i] = 0;
			/* printf("bf[%d]: %d  ", i, buf[i]); */
		}

		ret = ComGetMem(r_shm, (char *)buf, &read_len, &type, &user);
		
		if (ret <= 0) {
			/* printf("no date continue ---------------------- \n"); */
			/* return; */
 			continue;
		}

		printf("read_times: %d ret: %d read_len: %d \n",
		       read_times, ret, read_len);
		read_times++;

		/* judge the data is correct */
		/* 
                 * for (i = 0; i < read_len; i++) {
		 * 	printf("bf[%d]: %d  ", i, buf[i]);
		 * }
		 * printf("\n\n");
                 */

		/* sleep(2); */
	}
}

static void reade_shm()
{
	char		*r_shm;
	ssize_t		 read_len;
	char		 buf[BUFF_LEN];
	int		 i;
	unsigned long long    read_times = 0;
	shm_reader_t	 reader	    = TEST_READER;

	printf("start read shared memory:\n");

	r_shm = (char *)init_mem('r', shm_key, shm_num, shm_width);
	printf("3333333ddd : %x \n", (int)r_shm);
	/* return; */

	/* init buf */
	for (i = 0; i < BUFF_LEN; i++) {
		buf[i] = 0;
		/* printf("bf[%d]: %d  ", i, buf[i]); */
	}
	printf("\n\n");

	/* read the mem */
	while (1) {
		read_len = 0;
		/* 
                 * for (i = 0; i < BUFF_LEN; i++) {
		 * 	buf[i] = 0;
		 * 	/\* printf("bf[%d]: %d  ", i, buf[i]); *\/
		 * }
                 */

		read_len = 0;
		read_len = so_get_recent_shm(r_shm, reader, (char *)buf, sizeof(buf));
		/* printf("read_len: %d \n", read_len); */

		if (read_len <= 0) {
			/* printf("no date continue ---------------------- \n"); */
			/* return; */
 			continue;
		}

		printf("read_times: %llu read_len: %d \n",
		       read_times, read_len);
		read_times++;

		/* judge the data is correct */
		/* 
                 * for (i = 0; i < read_len; i++) {
		 * 	printf("bf[%d]: %d  ", i, buf[i]);
		 * }
		 * printf("\n\n");
                 */

		/* sleep(2); */
	}
}

static void read_shm(unsigned int index)
{
	char		*r_shm;
	char		 buf[BUFF_LEN];
	int		 ret;
	int		 i;

	printf("read shared memory index: %ld \n", index);

	r_shm = (char *)init_mem('r', shm_key, shm_num, shm_width);
	printf("shm addr: %x \n", (int)r_shm);

	/* init buf */
	for (i = 0; i < BUFF_LEN; i++) {
		buf[i] = 0;
		/* printf("bf[%d]: %d  ", i, buf[i]); */
	}
	printf("\n\n");

	printf("mem from %ld to %ld \n", 
	       mem_first_index(r_shm), mem_last_index(r_shm));

	ret = get_mem(r_shm, index, (char *)buf, sizeof(buf));
	if (ret >= 0) {
		for (i = 0; i < BUFF_LEN; i++) {
			printf("bf[%d]: %d  ", i, buf[i]);
		}
		printf("\n");
	} else {
		printf("read error ret: %d\n", ret);
	}
}

int del_data()
{
	char	*shm;
	int	 ret;

	shm = (char *)init_mem('w', shm_key, shm_num, shm_width);
	printf("shm addr: %x \n", (int)shm);

	ret = mem_del_data(shm);
	printf("del all data! num: %ld \n", ret);

	return 0;	
}

int main(int argc, char *argv[])
{
	char c;
	char usage[] = {" -d del all data\n -kn write continue n times sleep 1\n -c read continue\n -e read continue use get_recent_shm \n -rn read index n\n -wn write number n\n"};

	while ( (c = getopt(argc, argv, "dcer:w:k:h")) != EOF ) {
		switch (c) {
		case 'c':
			readc_shm();
			break;
		case 'e':
			reade_shm();
			break;
		case 'r':
			read_shm(atol(optarg));
			break;
		case 'w':
			write_shm(atol(optarg));
			break;
		case 'k':
			writec_shm(atol(optarg));
			break;
		case 'd':
			del_data();
			break;
		case 'h':
			printf(usage); 
			break;
		default:
			printf(usage); 
			break;
		}
	}

	return 0;
}

