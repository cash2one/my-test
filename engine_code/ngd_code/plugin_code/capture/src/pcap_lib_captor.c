/* 
 * $Id: pcap_lib_captor.c,v 1.4 2009/06/06 10:00:16 zzf Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pcap.h>

#include "misc.h"
#include "captor.h"
#include "pcap_lib_captor.h"
//
//#include "trace_api.h"

#define RAW_SHM_SIZE 4096 * 4096
#define RAW_SHM_KEY 0x19761127

static int init_shm(void);
static void clean_shm(void);
static void *pcap_lib_captor_getbase();
static int pcap_lib_captor_munmap(void *address);
void *pcap_lib_captor_mmap();
static long pcap_lib_captor_open(void *private_info, int argc, char **argv);
static int pcap_lib_captor_capture(void *private_info, long hdlr, u_int8_t ** pkt_buf_p);
static void pcap_lib_captor_close(void *private_info, long hdlr);

captor_t pcap_lib_captor = {
      name:"pcap_lib",
      open:pcap_lib_captor_open,
      close:pcap_lib_captor_close,
      capture:pcap_lib_captor_capture,
      mmap:pcap_lib_captor_mmap,
      munmap:pcap_lib_captor_munmap,
      getbase:pcap_lib_captor_getbase,
};

struct pcap_pkthdr pcaphead;

static char *shm_buff = NULL;
static int shm_id = -1;

long pcap_lib_captor_open(void *private_info, int argc, char **argv)
{
	char errbuf[1024];
	pcap_t *p_cap;

	if (argc != 1) {
		DMSG("Error: pcap arg syntax: ifname\n");
		goto err;
	}

	/* pcap_t *pcap_open_live(char *device, int snaplen, int promisc, 
	 * 			int to_ms, char *ebuf)
	 * The first argument is the device that we specified in the previous 
	 * section. snaplen is an integer which defines the maximum number of 
	 * bytes to be captured by pcap.  promisc, when set to true, brings the
	 * interface into promiscuous mode (however, even if it is set to false, 
	 * it is possible under specific cases for the interface to be in 
	 * promiscuous mode, anyway).  to_ms is the read time out in milliseconds 
	 * (a value of 0 sniffs until an error occurs; -1 sniffs indefinitely).  
	 * Lastly, ebuf is a string we can store any error messages within 
	 * (as we did above with errbuf).  The function returns our session handler. 
	 */
	p_cap = pcap_open_live(argv[0], 1600, 1, 0, errbuf);
	if (!p_cap) {
		printf("err open pcap %s\n", errbuf);
		goto err;
	}

	return (long) p_cap;
err:
	return -1;
}

int pcap_lib_captor_capture(void *private_info, long hdlr, u_int8_t ** pkt_buf_p)
{
	int nread = 0;
	static char *pfree = NULL;
	char *pcap_packet;

	if (hdlr == -1)
		goto err;

	pcap_packet =
	    (char *) pcap_next((pcap_t *) hdlr, &pcaphead);
	nread = pcaphead.len;

	if (shm_buff == NULL)
		*pkt_buf_p = (u_int8_t *)pcap_packet;
	else {			/* for smp */
		if (pfree == NULL)
			pfree = shm_buff;

		if (RAW_SHM_SIZE - (pfree - shm_buff) < nread)
			pfree = shm_buff;

		memcpy(pfree, pcap_packet, nread);
		*pkt_buf_p = (u_int8_t *)pfree;
		pfree += nread;
	}

	return nread;
err:
	return -1;
}

void pcap_lib_captor_close(void *private_info, long hdlr)
{
	if (hdlr != -1)
		pcap_close((pcap_t *) hdlr);

	clean_shm();
	return;
}

void *pcap_lib_captor_getbase()
{
	if (shm_buff == NULL) {
		if (init_shm() < 0)
			return NULL;
	}

	return (void *) shm_buff;
}

void *pcap_lib_captor_mmap()
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

int pcap_lib_captor_munmap(void *address)
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
