#ifndef __FRAG_H__
#define __FRAG_H__

#include "mbuf.h"
#include "list.h"
#include "spinlock.h"


#define INETFRAGS_HASHSZ		64


#define FRAG_MAX_CACHE_NUM    8


#define FRAG_MAX_CACHE_Q_NUM  65536


#define FRAG_MAX_TIMEOUT    20*utaf_get_timer_hz()       /* 5s */



struct inet_frag_queue {
	struct hlist_node	list;
	//struct netns_frags	*net;
	//struct list_head	lru_list;   /* lru list member */
	spinlock_t		lock;
	//atomic_t		refcnt;
	//struct timer_list	timer;      /* when will this queue expire? */
	struct m_buf    *fragments; /* list of received fragments */
	uint8_t        count;
	//ktime_t			stamp;
	//int			len;        /* total length of orig datagram */
	//int			meat;
	uint8_t			last_in;    /* first/last segment arrived? */

	uint64_t       cycle;

#define INET_FRAG_COMPLETE	4
#define INET_FRAG_FIRST_IN	2
#define INET_FRAG_LAST_IN	1
};




struct frags_bucket
{
	struct hlist_head hash;
	spinlock_t bkt_lock;
};


struct inet_frags {
	//struct hlist_head	hash[INETFRAGS_HASHSZ];
	struct frags_bucket bucket[INETFRAGS_HASHSZ];
	//rwlock_t  lock;

	int			(*match)(struct inet_frag_queue *q,	void *arg);
	unsigned int		(*hashfn)(struct inet_frag_queue *);
};




extern uint32_t ip_defrag(struct m_buf *mbuf, struct m_buf **buf, uint8_t *count);
extern uint32_t ipfrag_init(void);  //wdb_calc222




#endif
