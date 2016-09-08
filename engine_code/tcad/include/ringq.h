#ifndef __RINGQ__H__
#define __RINGQ__H__

#include <stdint.h>
#include <sys/queue.h>
/* #include "mem.h" //wdb_ppp */
#include "common.h"
#include "atomic.h"

#define RTE_RING_NAMESIZE 32

#define RINGQ_MAGIC  0xAABBCCDD

#define RING_F_SP_ENQ 0x0001 /**< The default enqueue is "single-producer". */
#define RING_F_SC_DEQ 0x0002 /**< The default dequeue is "single-consumer". */

#define RING_DEBUG




#ifdef RING_DEBUG
/**
 * A structure that stores the ring statistics (per-lcore).
 */
struct ring_debug_stats {
	uint64_t enq_success_bulk; /**< Successful enqueues number. */
	uint64_t enq_success_objs; /**< Objects successfully enqueued. */
	uint64_t enq_quota_bulk;   /**< Successful enqueues above watermark. */
	uint64_t enq_quota_objs;   /**< Objects enqueued above watermark. */
	uint64_t enq_fail_bulk;    /**< Failed enqueues number. */
	uint64_t enq_fail_objs;    /**< Objects that failed to be enqueued. */
	uint64_t deq_success_bulk; /**< Successful dequeues number. */
	uint64_t deq_success_objs; /**< Objects successfully dequeued. */
	uint64_t deq_fail_bulk;    /**< Failed dequeues number. */
	uint64_t deq_fail_objs;    /**< Objects that failed to be dequeued. */
} cache_aligned;
#endif




enum
{
	RINGQ_OK = 0,
	RINGQ_NOBUF = 1,
	RINGQ_NOENT = 2,
	RINGQ_NOVALID = 3,
};


struct ringq 
{
	struct ringq *next;      /**< Next in list. */

	char name[RTE_RING_NAMESIZE];    /**< Name of the ring. */
	int magic_flag;                       /**< Flags supplied at creation. */

	int monitor_id;

	/** Ring producer status. */
	struct prod {
		volatile uint32_t bulk_default; /**< Default bulk count. */
		uint32_t watermark;      /**< Maximum items before EDQUOT. */
		uint32_t sp_enqueue;     /**< True, if single producer. */
		uint32_t size;           /**< Size of ring. */
		
		uint32_t mask;           /**< Mask (size-1) of ring. */
		volatile uint32_t head;  /**< Producer head. */
		volatile uint32_t tail;  /**< Producer tail. */
	} prod cache_aligned;

	/** Ring consumer status. */
	struct cons {
		volatile uint32_t bulk_default; /**< Default bulk count. */
		uint32_t sc_dequeue;     /**< True, if single consumer. */
		uint32_t size;           /**< Size of the ring. */
		uint32_t mask;           /**< Mask (size-1) of ring. */
		volatile uint32_t head;  /**< Consumer head. */
		volatile uint32_t tail;  /**< Consumer tail. */
	} cons cache_aligned;

#ifdef RING_DEBUG
		struct ring_debug_stats stats;
#endif

	uint64_t volatile ring[0] \
			cache_aligned; /**< Memory space of ring starts here. */
};


#ifdef RING_DEBUG
#define __RING_STAT_ADD(r, name, n) do {		\
		r->stats.name##_objs += n;	\
		r->stats.name##_bulk += 1;	\
	} while(0)
#else
#define __RING_STAT_ADD(r, name, n) do {} while(0)
#endif



struct ringq_base
{
	uint32_t count;
	struct ringq *next;
	pthread_mutex_t lock;
};



extern struct ringq *ringq_create(const char *name, unsigned count, unsigned flags);



static inline void ringq_pause (void)
{
	asm volatile ("pause");
}




static inline int ringq_check(struct ringq *rq)
{
	if(RINGQ_MAGIC != rq->magic_flag)
	{
		return -RINGQ_NOVALID;
	}

	return RINGQ_OK;
}



static inline int ringq_sp_enqueue_bulk(struct ringq *r, uint64_t obj)
{
	uint32_t prod_head, cons_tail;
	uint32_t prod_next, free_entries;
	uint32_t mask;
	
	mask = r->prod.mask;

	prod_head = r->prod.head;
	cons_tail = r->cons.tail;

	free_entries = mask + cons_tail - prod_head;

	if (unlikely(1 > free_entries)) {
		//LOG("no enough entries for you\n");
		__RING_STAT_ADD(r, enq_fail, 1);
		return -RINGQ_NOBUF;
	}

	prod_next = prod_head + 1;
	r->prod.head = prod_next;

	/* write entries in ring */
	r->ring[prod_head & mask] = obj;
	m_wmb();

	__RING_STAT_ADD(r, enq_success, 1);

	r->prod.tail = prod_next;
	
	return RINGQ_OK;
}

static inline int ringq_mp_enqueue_bulk(struct ringq *r, uint64_t obj)
{
	uint32_t prod_head, prod_next;
	uint32_t cons_tail, free_entries;
	int success;
	uint32_t mask = r->prod.mask;

	/* move prod.head atomically */
	do {
		prod_head = r->prod.head;
		cons_tail = r->cons.tail;
		/* The subtraction is done between two unsigned 32bits value
		 * (the result is always modulo 32 bits even if we have
		 * prod_head > cons_tail). So 'free_entries' is always between 0
		 * and size(ring)-1. */
		free_entries = (mask + cons_tail - prod_head);

		/* check that we have enough room in ring */
		if (unlikely(1 > free_entries)) {
			__RING_STAT_ADD(r, enq_fail, 1);
			return -RINGQ_NOBUF;
		}

		prod_next = prod_head + 1;
		success = atomic32_cmpset(&r->prod.head, prod_head,
						  prod_next);
	} while (unlikely(success == 0));

	/* write entries in ring */
	r->ring[prod_head & mask] = obj;
	m_wmb();

	__RING_STAT_ADD(r, enq_success, 1);

	/*
	 * If there are other enqueues in progress that preceeded us,
	 * we need to wait for them to complete
	 */
	while (unlikely(r->prod.tail != prod_head))
		ringq_pause();

	r->prod.tail = prod_next;
	
	return RINGQ_OK;
}


static inline unsigned ringq_count(const struct ringq *r)
{
	uint32_t prod_tail = r->prod.tail;
	uint32_t cons_tail = r->cons.tail;
	return ((prod_tail - cons_tail) & r->prod.mask);
}


static inline unsigned ringq_free_count(const struct ringq *r)
{
	uint32_t prod_tail = r->prod.tail;
	uint32_t cons_tail = r->cons.tail;
	return ((cons_tail - prod_tail - 1) & r->prod.mask);
}

static inline int ringq_full(const struct ringq *r)
{
	uint32_t prod_tail = r->prod.tail;
	uint32_t cons_tail = r->cons.tail;
	return (((cons_tail - prod_tail - 1) & r->prod.mask) == 0);
}

static inline int ringq_empty(const struct ringq *r)
{
	uint32_t prod_tail = r->prod.tail;
	uint32_t cons_tail = r->cons.tail;
	return !!(cons_tail == prod_tail);
}


static inline int ringq_sp_dequeue_bulk(struct ringq *r, uint64_t *obj_table)
{
	uint32_t cons_head, prod_tail;
	uint32_t cons_next, entries;
	uint32_t mask = r->prod.mask;

	cons_head = r->cons.head;
	prod_tail = r->prod.tail;

	entries = prod_tail - cons_head;

	/* check that we have enough entries in ring */
	if (unlikely(1 > entries)) {
		__RING_STAT_ADD(r, deq_fail, 1);
		return -RINGQ_NOENT;
	}

	cons_next = cons_head + 1;
	r->cons.head = cons_next;

	m_rmb();
	*obj_table = r->ring[cons_head & mask];

	__RING_STAT_ADD(r, deq_success, 1);
	
	r->cons.tail = cons_next;
	
	return RINGQ_OK;
}

static inline int ringq_mp_dequeue_bulk(struct ringq *r, uint64_t *obj_table)
{
	uint32_t cons_head, prod_tail;
	uint32_t cons_next, entries;
	int success;
	uint32_t mask = r->prod.mask;

	/* move cons.head atomically */
	do {
		cons_head = r->cons.head;
		prod_tail = r->prod.tail;
		/* The subtraction is done between two unsigned 32bits value
		 * (the result is always modulo 32 bits even if we have
		 * cons_head > prod_tail). So 'entries' is always between 0
		 * and size(ring)-1. */
		entries = (prod_tail - cons_head);

		/* check that we have enough entries in ring */
		if (unlikely(1 > entries)) {
			__RING_STAT_ADD(r, deq_fail, 1);
			return -RINGQ_NOENT;
		}

		cons_next = cons_head + 1;
		success = atomic32_cmpset(&r->cons.head, cons_head, cons_next);
	} while (unlikely(success == 0));

	/* copy in table */
	m_rmb();
	*obj_table = r->ring[cons_head & mask];
	
	/*
	 * If there are other dequeues in progress that preceeded us,
	 * we need to wait for them to complete
	 */
	while (unlikely(r->cons.tail != cons_head))
		ringq_pause();

	__RING_STAT_ADD(r, deq_success, 1);
	r->cons.tail = cons_next;
	return 0;
}



static inline int ringq_dequeue(struct ringq *rq, uint64_t *obj)
{
	if(rq->cons.sc_dequeue)		
		return ringq_sp_dequeue_bulk(rq, obj);
	else
		return ringq_mp_dequeue_bulk(rq, obj);
}


static inline int ringq_enqueue(struct ringq *rq, uint64_t obj)
{ 
	if(rq->prod.sp_enqueue)
		return ringq_sp_enqueue_bulk(rq, obj);
	else
		return ringq_mp_enqueue_bulk(rq, obj);
}







#endif
