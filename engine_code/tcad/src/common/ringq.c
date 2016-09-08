#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"  //wdb_ppp
#include "ringq.h"
#if 1 /* wdb */
//#include <numaif.h>
#include <errno.h>
#endif /* wdb */

#define POWEROF2(x) ((((x)-1) & (x)) == 0)



extern int g_page_sz;


struct ringq *ringq_create(const char *name, unsigned count, unsigned flags)
{
	size_t ring_size;
	struct ringq *r = NULL;
#if 0 /* wdb */
#if 1 /* wdb_as */
    unsigned long nodemask = 0x1;
#else /* wdb_as */
	unsigned long nodemask = 0x2;
#endif /* wdb_as */
    void *hp;
    size_t m_size;
#endif /* wdb */
	if(NULL == name)
	{
		return NULL;
	}

	BUILD_BUG_ON((sizeof(struct ringq) & CACHE_LINE_MASK) != 0);
	BUILD_BUG_ON((offsetof(struct ringq, cons) & CACHE_LINE_MASK) != 0);
	BUILD_BUG_ON((offsetof(struct ringq, prod) & CACHE_LINE_MASK) != 0);

	if(!POWEROF2(count))
	{
		LOG("Requested size is not a power of 2\n");
		return NULL;
	}

	ring_size = count * sizeof(uint64_t) + sizeof(struct ringq);

	r = (struct ringq *)malloc(ring_size);
	if(NULL == r)
	{
		LOG("no enough memory malloc sapce\n");
		return NULL;
	}
#if 0 /* wdb */
    hp = (void *)(((unsigned long)r/g_page_sz)*g_page_sz);
    m_size = ring_size;
    if ( mbind(hp, ((m_size + 2*g_page_sz)/g_page_sz)*g_page_sz, MPOL_BIND, &nodemask, 3, MPOL_MF_MOVE | MPOL_MF_STRICT) != 0 )
    {
        fprintf(stderr, "mbind() failed: %s\n", strerror(errno));
    }
#endif /* wdb */
	memset(r, 0, sizeof(*r));

	r->magic_flag = RINGQ_MAGIC;

	snprintf(r->name, sizeof(r->name), "%s", name);

	r->prod.bulk_default = r->cons.bulk_default = 1;
	r->prod.watermark = count;
	r->prod.sp_enqueue = !!(flags & RING_F_SP_ENQ);
	r->cons.sc_dequeue = !!(flags & RING_F_SC_DEQ);
	r->prod.size = r->cons.size = count;
	r->prod.mask = r->cons.mask = count-1;
	r->prod.head = r->cons.head = 0;
	r->prod.tail = r->cons.tail = 0;

	r->next = NULL;

	return r;
}




