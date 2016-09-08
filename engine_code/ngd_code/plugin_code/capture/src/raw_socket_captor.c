
/*
 * $Id: raw_socket_captor.c,v 1.4 2009/06/06 10:00:16 zzf Exp $
 */

#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/errno.h>

#include "misc.h"
#include "captor.h"
#include "raw_socket_captor.h"
//#include "trace_api.h"

#define RAW_SHM_SIZE 4096 * 4096
#define RAW_SHM_KEY 0x19761126

static int init_shm(void);
static void clean_shm(void);
static void *raw_socket_captor_getbase();
static int raw_socket_captor_munmap(void *address);
void *raw_socket_captor_mmap();
static long raw_socket_captor_open(void *private_info, int argc, char **argv);
static int raw_socket_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
static void raw_socket_captor_close(void *private_info, long hdlr);

captor_t raw_socket_captor = {
	name: "raw_socket",
	open: raw_socket_captor_open,
	close: raw_socket_captor_close,
	capture: raw_socket_captor_capture,
	mmap:raw_socket_captor_mmap,
	munmap:raw_socket_captor_munmap,
	getbase:raw_socket_captor_getbase,
};

static u_int8_t	raw_packet_space[RAW_PACKET_LEN];

#define MAX_NIC_NUMBER 16
int devfd[MAX_NIC_NUMBER];
int sockfd;

static char *shm_buff = NULL;
static int shm_id = -1;

long raw_socket_captor_open(void *private_info, int argc, char **argv)
{
	struct ifreq ifr;
	int i;

	if (argc == 0) {
		DMSG("Error: Raw socket arg syntax:ifname1 [ifname2...]\n");
		goto err;
	}

	sockfd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
	if (sockfd < 0) {
		DMSG("socket call");
		goto err;
	}

	for(i = 0; i < MAX_NIC_NUMBER; i++) 
		devfd[i] = -1;
	
	for(i = 0; i < argc; i++) {
		memset (&ifr, 0, sizeof (ifr));
		strncpy (ifr.ifr_name, argv[i], sizeof (ifr.ifr_name) - 1);

		/* get nic index */
		if (ioctl (sockfd, SIOCGIFINDEX, &ifr) < 0) {
			DMSG ("ioctl SIOCGIFINDEX");
			goto err;
		}
		devfd[ifr.ifr_ifindex] = sockfd; /* hash */
		DMSG("name: %s, NIC_INDEX: %d, sockfd: %d\n", 
			ifr.ifr_name, ifr.ifr_ifindex, sockfd);
		
		/* set promisc mode */
		if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
			DMSG("ioctl SIOCGIFFLAGS");
			goto err;
		}
		ifr.ifr_flags |= IFF_PROMISC;
		if (ioctl (sockfd, SIOCSIFFLAGS, &ifr) < 0) {
			DMSG ("ioctl SIOCSIFFLAGS");
			goto err;
		}
	}

	return sockfd;
err:
	return -1;
}

int raw_socket_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
	int nread = 0;
	struct sockaddr_ll from;
	socklen_t fromlen;
	static char *pfree = NULL;

	if (hdlr < 0)
		goto err;

	while(1) {
		bzero(&from, sizeof(from));
		fromlen = sizeof(from);

		nread = recvfrom(hdlr, (char *)raw_packet_space, RAW_PACKET_LEN, 
					0, (struct sockaddr *) &from, &fromlen);
		if (nread < 0) {
			if (errno == EINTR)
				continue;	
			goto err;
		}
		if (nread == 0)
			continue;

		if (devfd[from.sll_ifindex] >= 0) /* filter */
			break;
	}

	if (shm_buff == NULL) {
		*pkt_buf_p = raw_packet_space;
	} else { /* for smp */
		if (pfree == NULL)
			pfree = shm_buff;
		
		if (RAW_SHM_SIZE - (pfree - shm_buff) < nread)
			pfree = shm_buff;

		memcpy(pfree, raw_packet_space, nread);
		*pkt_buf_p = (u_int8_t *)pfree;
		pfree += nread;
	}

	return nread;
err:
	return -1;
}

void raw_socket_captor_close(void *private_info, long hdlr)
{	
#if 0
	int i;
	struct ifreq ifr;

	for(i = 0; i < MAX_NIC_NUMBER; i++) {
		if (devfd[i] < 0) 
			continue;
		ifr.ifr_ifindex = i;
		if (ioctl (sockfd, SIOCGIFNAME, &ifr) < 0) {
			perror ("ioctl SIOCGIFNAME");
		}
		/* unset promisc mode */
		if (ioctl (sockfd, SIOCGIFFLAGS, &ifr) < 0) {
			perror ("ioctl SIOCGIFFLAGS");
		}
		ifr.ifr_flags &= ~IFF_PROMISC;
		if (ioctl (sockfd, SIOCSIFFLAGS, &ifr) < 0) {
			perror ("ioctl SIOCSIFFLAGS");
		}
	}
#endif

	if (sockfd > 0)
		close(sockfd);

	clean_shm();
	return;
}

void *raw_socket_captor_getbase()
{
	if (shm_buff == NULL) {
		if (init_shm() < 0) 
			return NULL;
	}

	return (void *) shm_buff;
}

void *raw_socket_captor_mmap()
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

int raw_socket_captor_munmap(void *address)
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
