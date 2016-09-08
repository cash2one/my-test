#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "misc.h"
#include "mbuf.h"
#include "frag.h"
#include "utaf_timer.h"

#include "ip4.h"


/* Describe an entry in the "incomplete datagrams" queue. */
struct ipq {
	struct inet_frag_queue q;

	//u32		user;
	uint32_t saddr;
	uint32_t daddr;
	uint16_t id;
	uint8_t protocol;
	uint16_t sport;
	uint16_t dport;
	//int             iif;
	//unsigned int    rid;
	//struct inet_peer *peer;
};


static struct inet_frags ip4_frags;


uint64_t FRAG_SYS_HZ = 0;


extern struct mt_config mtconfig;
extern uint64_t UTAF_SYS_HZ;


/* Add new segment to existing queue. */
static uint32_t ip_frag_queue(struct ipq *qp, struct m_buf *mbuf, struct m_buf **bufarray, uint8_t *retsize)
{
	struct m_buf *next = NULL;
	struct m_buf *cache = NULL;

	qp->q.cycle = utaf_get_timer_cycles();

	switch(mbuf->ipdefrag_flag)
	{
		case IS_FIRST_IP_DEFRAG:/*首片*/
		{
		#ifdef UTAF_DEBUG
			printf("is first ip frag\n");
		#endif
			if(qp->q.last_in & INET_FRAG_FIRST_IN)
			{	
			#ifdef UTAF_DEBUG
				printf("first ip frag has come\n");
			#endif
				bufarray[0] = mbuf;
				*retsize = 1;
				return UTAF_RX_DROP;
			}
			
			/*填充端口信息*/
			qp->sport = mbuf->sport;
			qp->dport = mbuf->dport;

			bufarray[0] = mbuf;
			*retsize = 1;
			
			/*将缓存的非首片报文填充信息后上送*/
			cache = qp->q.fragments;
			if(NULL != cache)
			{
				while(cache)
				{
					next = cache->next;
					cache->next = NULL;
					cache->sport = qp->sport;
					cache->dport = qp->dport;
				
					bufarray[*retsize] = cache;
					*retsize = *retsize + 1;
					
					cache = next;
				}
				qp->q.fragments = NULL;
				qp->q.count = 0;
			}
			qp->q.last_in = qp->q.last_in | INET_FRAG_FIRST_IN;
			return UTAF_RX_SUCCESS;
		}

		case IS_OTHER_IP_DEFRAG:
		{
			if(qp->q.last_in & INET_FRAG_FIRST_IN)/*首片已到填充信息*/
			{
			#ifdef UTAF_DEBUG
				printf("first frag has come\n");
			#endif	
				mbuf->sport = qp->sport;
				mbuf->dport = qp->dport;

				bufarray[0] = mbuf;
				*retsize = 1;
			}
			else/*缓存*/
			{
				if(qp->q.count >= FRAG_MAX_CACHE_NUM)
				{
				#ifdef UTAF_DEBUG
					printf("max cache frag num\n");
				#endif				
					bufarray[0] = mbuf;
					*retsize = 1;
					return UTAF_RX_DROP;
				}

			#ifdef UTAF_DEBUG
				printf("cache frag packet\n");
			#endif	
				next = qp->q.fragments;
				if(next == NULL)
				{
					qp->q.fragments = mbuf;
				}
				else
				{
					mbuf->next = next;
					qp->q.fragments = mbuf;
				}
				qp->q.count++;
			}
			
			return UTAF_RX_SUCCESS;
		}

		default:
		{
			bufarray[0] = mbuf;
			*retsize = 1;
			return UTAF_RX_DROP;
		}
	}
	
}


static unsigned int ipqhashfn(uint16_t id, uint32_t saddr, uint32_t daddr, uint8_t prot)
{
	return jhash_3words(id << 16 | prot,
		    			saddr, 
		    			daddr,
		    			0) & (INETFRAGS_HASHSZ - 1);
	
}

static unsigned int ip4_hashfn(struct inet_frag_queue *q)
{
	struct ipq *ipq;

	ipq = container_of(q, struct ipq, q);
	return ipqhashfn(ipq->id, ipq->saddr, ipq->daddr, ipq->protocol);
}


static struct inet_frag_queue *inet_frag_intern(struct inet_frag_queue *qp_in, struct inet_frags *f, unsigned int hash)
{
	hlist_add_head(&qp_in->list, &f->bucket[hash].hash);

	return qp_in;
}


static struct inet_frag_queue *frag_q_alloc(void)  //wdb_calc222
{
	return (struct inet_frag_queue *)malloc(sizeof(struct ipq));
}


static void frag_q_free(struct inet_frag_queue *q)
{
	return free(q);
}



static struct inet_frag_queue *inet_frag_alloc(struct inet_frags *f, struct m_buf *mbuf)
{
	struct inet_frag_queue *q;
	struct ipq *qp = NULL;

	struct ipv4_hdr *iph = ip_hdr(mbuf);
	
	q = frag_q_alloc();
	if(NULL == q)
	{
		return NULL;
	}

	memset(q, 0, sizeof(struct ipq));
	
	qp = container_of(q, struct ipq, q);

	qp->protocol = iph->next_proto_id;
	qp->id = iph->packet_id;
	qp->saddr = iph->src_addr;
	qp->daddr = iph->dst_addr;

	return q;
}



static struct inet_frag_queue *inet_frag_create(struct inet_frags *f, struct m_buf *mbuf, unsigned int hash)
{
	struct inet_frag_queue *q;

	q = inet_frag_alloc(f, mbuf);
	if(q == NULL)
	{
		return NULL;
	}

#ifdef UTAF_DEBUG
	printf("add frag entry addr is %p\n", q);
#endif

	return inet_frag_intern(q, f, hash);
}


static int ip4_frag_match(struct inet_frag_queue *q, void *arg)
{
	struct ipq *qp = NULL;
	struct ipv4_hdr *iph = (struct ipv4_hdr *)arg;

	qp = container_of(q, struct ipq, q);

	return (qp->id == iph->packet_id &&
		qp->saddr == iph->src_addr &&
		qp->daddr == iph->dst_addr &&
		qp->protocol == iph->next_proto_id);

}


struct inet_frag_queue *inet_frag_find(struct inet_frags *f, unsigned int hash, struct ipv4_hdr *iph, struct m_buf *mbuf)
{
	struct inet_frag_queue *q;
	struct hlist_node *n;
    struct hlist_node *t;
	
	//hlist_for_each_entry(q, n, &f->bucket[hash].hash, list)
    hlist_for_each_entry_safe(q, t, n, &f->bucket[hash].hash, list)
	{
		if(f->match(q, (void *)iph))
		{
		#ifdef UTAF_DEBUG
			printf("find a ip frag entry\n");
		#endif
			return q;
		}
	}

#ifdef UTAF_DEBUG
	printf("add new ip frag entry\n");
#endif

	return inet_frag_create(f, mbuf, hash);
}

static inline struct ipq *ip_find(struct ipv4_hdr *iph, 
	                                 struct m_buf *mbuf, 
	                                 unsigned int hash)
{
	struct inet_frag_queue *q;

	q = inet_frag_find(&ip4_frags, hash, iph, mbuf);
	if(NULL == q)
	{
		return NULL;
	}

	return container_of(q, struct ipq, q);
}


uint32_t ip_defrag(struct m_buf *mbuf, struct m_buf **buf, uint8_t *count)
{
	unsigned int hash;
	int ret;

	struct ipq *qp;
	struct ipv4_hdr *iph;

#ifdef UTAF_DEBUG
	printf("===========> ip_defrag\n");
#endif
	iph = ip_hdr(mbuf);
	hash = ipqhashfn(iph->packet_id, iph->src_addr, iph->dst_addr, iph->next_proto_id);

#ifdef UTAF_DEBUG
	printf("ip defrag hash is %d\n", hash);
#endif

	spinlock_lock(&ip4_frags.bucket[hash].bkt_lock);
	
	if((qp = ip_find(iph, mbuf, hash)) != NULL)
	{
		ret = ip_frag_queue(qp, mbuf, buf, count);
	}
	else
	{
		ret = UTAF_RX_DROP;
	}
	
	spinlock_unlock(&ip4_frags.bucket[hash].bkt_lock);

	return ret;
	
}


static struct timer frag_age;

/* timer0 callback */
static void frag_age_timeout_cb(__attribute__((unused)) struct timer *tim, __attribute__((unused)) void *arg)
{
	int i = 0;
	uint64_t current_cycle;
	//unsigned lcore_id = utaf_lcore_id();
	//printf("%s() on lcore %u\n", __func__, lcore_id);

	struct inet_frag_queue *q;
	struct hlist_node *n;
	struct hlist_node *t;
	struct hlist_head timeout;
	struct m_buf *next;
	struct m_buf *temp;
	
	INIT_HLIST_HEAD(&timeout);

	current_cycle = utaf_get_timer_cycles();

	for(i = 0; i < INETFRAGS_HASHSZ; i++)
	{
		spinlock_lock(&ip4_frags.bucket[i].bkt_lock);

		//hlist_for_each_entry(q, n, &ip4_frags.bucket[i].hash, list)
		hlist_for_each_entry_safe(q, t, n, &ip4_frags.bucket[i].hash, list)	
		{
			if((current_cycle > q->cycle) && ((current_cycle - q->cycle) > FRAG_MAX_TIMEOUT))
			{
				hlist_del(&q->list);
				hlist_add_head(&q->list, &timeout);
			}
		}
		
		spinlock_unlock(&ip4_frags.bucket[i].bkt_lock);

		//hlist_for_each_entry(q, n, &timeout, list)
		hlist_for_each_entry_safe(q, t, n, &timeout, list)	
		{
		#ifdef UTAF_DEBUG
			printf("frag entry is timeout addr is %p\n", q);
		#endif	
			hlist_del(&q->list);
			next = q->fragments;
			int c = 0;
			while(next)
			{
				temp = next->next;
				MBUF_FREE(next);
				next = temp;
				c++;
			}
		#ifdef UTAF_DEBUG
			printf("age mbuf release is %d\n", c);
		#endif
			frag_q_free(q);
		}
		
	}
	
}



uint32_t ipfrag_init()
{
	int i;
	//uint64_t hz;
	
	for (i = 0; i < INETFRAGS_HASHSZ; i++)
	{
		INIT_HLIST_HEAD(&ip4_frags.bucket[i].hash);
	}

	ip4_frags.hashfn = ip4_hashfn;
	ip4_frags.match = ip4_frag_match;


	timer_init(&frag_age);
	
	/*10ms*/
	//timer_reset(&frag_age, FRAG_SYS_HZ/100000, PERIODICAL, mtconfig.age_lcore, frag_age_timeout_cb, NULL);
	return timer_reset(&frag_age, UTAF_SYS_HZ, PERIODICAL, TIMER_CORE_ID, frag_age_timeout_cb, NULL);

}





