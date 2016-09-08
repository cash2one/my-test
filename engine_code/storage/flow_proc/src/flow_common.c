#include "gms_flow.h"

#define utaf_fetch_and_dec(x) __sync_fetch_and_sub ((volatile int *)(x), (int)1)
extern unsigned int net_mnt_ip_lookup(unsigned int ip);

void *gms_config_recv(void*arg);

spinlock_t utaf_nf_aged_lock[16];
spinlock_t utaf_ssn_age_qlock[16];
struct hlist_head utaf_nf_aged_list[16];
struct session_item_ringq_group session_group;
volatile time_t g_utaf_time_jiffies;
time_t g_traffic_last_time;
static volatile int __tm_out_flag = 0;
static spinlock_t       utaf_tmout_lst_hdlck;
static tmout_lst_hdr_t *utaf_tmout_lst_heads = NULL;
UTAF_DECLARE_PER_LCORE(int,_lcore_id);
UTAF_DECLARE_PER_LCORE(int,lthrd_id);


struct hlist_head g_utaf_timeout_ring[UTAF_AGE_RING_SIZE][16];
spinlock_t        g_utaf_timeout_rlck[UTAF_AGE_RING_SIZE][16];

static struct ringq *utaf_ssn_age_queue[SESSION_MAX_AGE_CORE];


struct hlist_head utaf_l_si_bunch[16];

int utaf_age_index_current = 0;

static spinlock_t utaf_empty_qlock;
typedef struct __empty_thrd_para_s
{
	int f_index;
	short thrd_start;
	short thrd_stop;
	int   athrd_cur;
	int   athrd_max;
} __empty_thrd_para_t;

static __empty_thrd_para_t __empty_thrd_para[2] =
{
	{ 0, 0,  8,                        0, (SESSION_MAX_AGE_CORE/2) },
	{ 0, 8, 16, (SESSION_MAX_AGE_CORE/2),  SESSION_MAX_AGE_CORE }
};

/*将x转换为2的倍数，输出转换后的值，eg：输入3 输出4*/
static inline int get_power(int x)
{
	int y = 1;
	while(x > y)
	{
		y = y * 2;
	}
	return y;
}
int init_session_tables(void)
{
	int x = g_flow_info_cfg.session_len/g_flow_info_cfg.thread_num;
	int i = 0;
	int y = 0;
	int j = 0;
	size_t m_size;
	session_item_header *sih = NULL;
	session_table *si;
	struct hlist_head bunch_hdr;
	int bunch_cnt;
	y = get_power(x);
	for(i = 0; i < g_flow_info_cfg.thread_num ; i++){
		session_group.session_item_ringq[i].first = NULL;
		spinlock_init(&(session_group.session_item_rq_lock[i]));

		m_size = (y * (SESSION_HEAD_SIZE + SESSION_SIZE));
		session_item_header *sih = (session_item_header *)malloc(m_size);
		if(NULL == sih)
		{

			mnt_debug("malloc size: %d  mem no ....\n",m_size);
			return UTAF_FAIL;
		}
		printf("mem success ...........\n");
		bunch_cnt = 0;
		bunch_hdr.first = NULL;	 

		for (j = 0; j < y; j++)
		{
			sih->session_item_magic = SESSION_MAGIC;
			sih->ringq_id = i;

			si = (session_table *)((uint8_t *)sih + SESSION_HEAD_SIZE);
			spinlock_init(&(si->item_lock));
			spinlock_init(&(si->tmrng_lck));
			si->list.next = bunch_hdr.first;
			bunch_hdr.first = &si->list;
			bunch_cnt++;
			if (bunch_cnt >= SI_BUNCH_SIZE)
			{
				si->tm_lst.next = session_group.session_item_ringq[i].first;
				session_group.session_item_ringq[i].first = &si->tm_lst;
				bunch_cnt = 0;
				bunch_hdr.first = NULL;
			}
			sih = (session_item_header *)((char *)sih + (SESSION_HEAD_SIZE + SESSION_SIZE));


		}
		if (bunch_hdr.first)
		{
			si = (session_table *)bunch_hdr.first;
			si->tm_lst.next = session_group.session_item_ringq[i].first;
			session_group.session_item_ringq[i].first = &si->tm_lst;
		}
		for (i = 0; i < 16; i++)
		{
			utaf_l_si_bunch[i].first = NULL;
		}


	}
	return UTAF_OK;

}
static __inline__ void utaf_atomic_add64(unsigned long i, volatile time_t *target)
{
	__asm__ __volatile__(
			"lock; add %1,%0"
			:"=m" (*target)
			:"ir" (i), "m" (*target));
}
static __inline__ void utaf_atomic_set(int i, volatile int *target)
{
	__asm__ __volatile__(
			"lock; xchgl %1,%0"
			:"=m" (*target)
			:"ir" (i), "m" (*target));
}
void utaf_ssn_age_timer_cb(void)
{
	utaf_atomic_set((int)1, &__tm_out_flag);
}
static void timer_handler(int32_t sig)
{
	utaf_atomic_add64(1, &g_utaf_time_jiffies);  
	utaf_ssn_age_timer_cb();

}
uint32_t TimerListInit(void) //wdb_ppp
{
	struct itimerval value, ovalue;
	struct sigaction sg_act;

	g_utaf_time_jiffies = time(NULL);

	extern time_t g_traffic_last_time;
	g_traffic_last_time = g_utaf_time_jiffies;

	value.it_value.tv_sec = 1;
	value.it_value.tv_usec = /* 500000 */ 0;
	value.it_interval.tv_sec = 1;
	value.it_interval.tv_usec = /* 500000 */ 0;

	/* 初始化定时器列表 */
#if 0 /* wdb_ppp */
	g_pstTimerHead = (st_timer_header *)MALLOC(sizeof(st_timer_header));
	if (NULL == g_pstTimerHead)
	{
		return -1;
	}

	g_pstTimerHead->num = 0;
	g_pstTimerHead->head = NULL;
	g_pstTimerHead->tail = NULL;
#endif /* wdb_ppp */

	setitimer(ITIMER_REAL, &value, &ovalue);
	sg_act.sa_handler  = timer_handler;
	sg_act.sa_mask     = (sigset_t){{0,}};
	sg_act.sa_flags    = SA_RESTART | SA_NODEFER;
	sg_act.sa_restorer = NULL;

	if ( sigaction(SIGALRM, &sg_act, NULL) != 0 )
	{
		fprintf(stderr, "sigaction() failed. exit...\n");
		return -2;
	}

	return 0;
}

static void utaf_ssn_age_tmout_lst_heads_init(void)
{
	    tmout_lst_hdr_t *tl;
		int i;

		spinlock_init(&utaf_tmout_lst_hdlck);
		tl = (tmout_lst_hdr_t *)malloc(g_flow_info_cfg.session_len*sizeof(tmout_lst_hdr_t));
		if (!tl)
		{
			fprintf(stderr, "malloc for 'tmout_lst_heads' failed\n");
			exit(0);
		}
#if 0
		hp = (void *)(((unsigned long)tl/g_page_sz)*g_page_sz);
		m_size = session_item_num*sizeof(tmout_lst_hdr_t);
		if ( mbind(hp, ((m_size + 2*g_page_sz)/g_page_sz)*g_page_sz, MPOL_BIND, &nodemask, 3, MPOL_MF_MOVE | MPOL_MF_STRICT) != 0 )
		{
			fprintf(stderr, "mbind() failed: %s\n", strerror(errno));
		}
#endif
		for (i = 0; i < g_flow_info_cfg.session_len; i++)
		{
			tl[i].next = utaf_tmout_lst_heads;
			utaf_tmout_lst_heads = &tl[i];
		}
}
struct ringq *ringq_create(const char *name, unsigned count, unsigned flags)
{
	size_t ring_size;
	struct ringq *r = NULL;
	if(NULL == name)
	{
		return NULL;
	}

	BUILD_BUG_ON((sizeof(struct ringq) & CACHE_LINE_MASK) != 0);
	BUILD_BUG_ON((offsetof(struct ringq, cons) & CACHE_LINE_MASK) != 0);
	BUILD_BUG_ON((offsetof(struct ringq, prod) & CACHE_LINE_MASK) != 0);

	if(!POWEROF2(count))
	{
		return NULL;
	}

	ring_size = count * sizeof(uint64_t) + sizeof(struct ringq);

	r = (struct ringq *)malloc(ring_size);
	if(NULL == r)
	{
		return NULL;
	}
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
static void utaf_tmout_lst_hdr_free(tmout_lst_hdr_t *tl)
{
	spinlock_lock(&utaf_tmout_lst_hdlck);
	tl->next = utaf_tmout_lst_heads;
	utaf_tmout_lst_heads = tl;
	spinlock_unlock(&utaf_tmout_lst_hdlck);
}

static void utaf_dist_aged_ssn(int q_id, uint64_t obj)
{
	spinlock_lock(&utaf_ssn_age_qlock[q_id]);
	if (0 != ringq_enqueue(utaf_ssn_age_queue[q_id], obj))
	{
		fprintf(stderr, "__BUG__ [Error]: %s(%lu) failed\n", __FUNCTION__, obj); /* __wdb__ */
		/* break; */
	}
	spinlock_unlock(&utaf_ssn_age_qlock[q_id]);
}

static tmout_lst_hdr_t *utaf_tmout_lst_hdr_alloc(void)
{
	tmout_lst_hdr_t *tl;
	spinlock_lock(&utaf_tmout_lst_hdlck);
	tl = utaf_tmout_lst_heads;
	if (tl)
	{
		utaf_tmout_lst_heads = tl->next;
	}
	spinlock_unlock(&utaf_tmout_lst_hdlck);
	return tl;
}
static void empty_aged_bucket(int f_index, int thrd_start, int thrd_stop, int *athrd_cur, int athrd_max)
{
	tmout_lst_hdr_t *tlh;
	session_table *si;
	//struct hlist_node *n, *t, **tmp_tl, **tmp_tl11;  //wdb_fix-Mar5
	int i;
#if 0 /* zdw wdb_lfix-2 */
	register          int last_hash;
	register unsigned int pause_cnt;
#endif /* wdb_lfix-2 */

	for (i = thrd_start; i < thrd_stop; i++)
	{
		tlh = utaf_tmout_lst_hdr_alloc();
		if (!tlh)
		{
			printf("wdb: __BUG__ ! malloc for timeout_list failed.\n");
			return;
		}

		//tmp_tl = &tlh->timeout_lst_head.first;

		spinlock_lock(&g_utaf_timeout_rlck[f_index][i]);

		tlh->timeout_lst_head.first = g_utaf_timeout_ring[f_index][i].first;
		if (tlh->timeout_lst_head.first)
		{
			tlh->timeout_lst_head.first->pprev = &tlh->timeout_lst_head.first;
		}
		g_utaf_timeout_ring[f_index][i].first = NULL;

#if 0
#if 1 /* wdb_lfix-2 */
		last_hash = -1;
#endif /* wdb_lfix-2 */

		hlist_for_each_entry_safe(si, t, n, &tlh->timeout_lst_head, tm_lst)
		{
#if 1 /* wdb_lfix-2 */
			if (si->hash == last_hash)
			{
				pause_cnt = 30;
				while (pause_cnt--) { asm volatile ("pause"); }
			}
			last_hash = si->hash;
#endif /* wdb_lfix-2 */

#ifdef UTAF_LOCK_SPINLOCK
			spinlock_lock(&base[si->hash].lock);
#else
			write_lock(&base[si->hash].lock);
#endif
			__hlist_del(&si->list);
			(base[si->hash].link_len)--;

#ifdef UTAF_LOCK_SPINLOCK
			spinlock_unlock(&base[si->hash].lock);
#else
			write_unlock(&base[si->hash].lock);
#endif
			si->hash = -1;
			tmp_tl = &si->tm_lst.next;

		}
		m_wmb();  //wdb_without_thrd_crting ???
		spinlock_unlock(&g_utaf_timeout_rlck[f_index][i]);


/** zdw  think this part doesn't work **/
		spinlock_lock(&utaf_nf_aged_lock[i]);

		*tmp_tl = utaf_nf_aged_list[i].first;
		if (utaf_nf_aged_list[i].first)
		{
			utaf_nf_aged_list[i].first->pprev = tmp_tl;
		}
		utaf_nf_aged_list[i].first = NULL;

		spinlock_unlock(&utaf_nf_aged_lock[i]);
#endif
		m_wmb();  //wdb_without_thrd_crting ???
		spinlock_unlock(&g_utaf_timeout_rlck[f_index][i]);

		if (tlh)
		{
			if (tlh->timeout_lst_head.first)
			{
				 utaf_dist_aged_ssn(i, (uint64_t)tlh); 
#if 0
				utaf_dist_aged_ssn(*athrd_cur, (uint64_t)tlh);
				/** zdw: age theard_num and item_theard_num is equal as this isn't work **/
				(*athrd_cur)++;
				if (*athrd_cur == athrd_max)
				{
					(*athrd_cur) -= (session_age_thread_num/2);
				}
#endif
			}
			else
			{
				utaf_tmout_lst_hdr_free(tlh);
			}
		}
	}


	return;
}


static void *empty_thrd(void *data)
{
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
#if 0 /* wdb_lfix-3 */
	cpu_set_t mask;

	CPU_ZERO(&mask);
	//CPU_SET(0, &mask);
	if ( ((__empty_thrd_para_t *)data)->thrd_start == 0 )
	{
		/* CPU_SET(38, &mask); //wdb_core */
		CPU_SET(__calcu_cores[0], &mask);  //wdb_core
	}
	else if ( ((__empty_thrd_para_t *)data)->thrd_start == 8 )
	{
		/* CPU_SET(39, &mask); //wdb_core */
		CPU_SET(__calcu_cores[1], &mask);  //wdb_core
	}
	else
	{
		fprintf(stderr, "%s() - 'thrd_start' out of band.\n", __FUNCTION__);
		exit(1);
	}
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
		fprintf(stderr, "%s() - set thread affinity failed.\n", __FUNCTION__);
		exit(1);
	}
#endif /* wdb_lfix-3 */

	UTAF_PER_LCORE(_lcore_id) = ((__empty_thrd_para_t *)data)->thrd_start;

	while (1)
	{
		spinlock_lock(&utaf_empty_qlock);
		if ( ((__empty_thrd_para_t *)data)->f_index == -1 )
		{
			spinlock_unlock(&utaf_empty_qlock);
			nanosleep(&sleeptime, NULL);
			continue;
		}
		spinlock_unlock(&utaf_empty_qlock);


		empty_aged_bucket( ((__empty_thrd_para_t *)data)->f_index,
				(int)(((__empty_thrd_para_t *)data)->thrd_start), (int)(((__empty_thrd_para_t *)data)->thrd_stop),
				&(((__empty_thrd_para_t *)data)->athrd_cur), ((__empty_thrd_para_t *)data)->athrd_max);

		spinlock_lock(&utaf_empty_qlock);
		((__empty_thrd_para_t *)data)->f_index = -1;
		spinlock_unlock(&utaf_empty_qlock);
	}

	return NULL;
}
				

static void empty_thrd_crt(int f_index)
{
	pthread_t tid0, tid1;

	spinlock_init(&utaf_empty_qlock);

	__empty_thrd_para[0].f_index = f_index;
	__empty_thrd_para[0].thrd_start = 0;
	__empty_thrd_para[0].thrd_stop = (1 == g_flow_info_cfg.thread_num ? 1 : g_flow_info_cfg.thread_num/2);
	__empty_thrd_para[0].athrd_cur = 0;
	__empty_thrd_para[0].athrd_max = g_flow_info_cfg.thread_num/2;

	__empty_thrd_para[1].f_index = f_index;
	__empty_thrd_para[1].thrd_start = g_flow_info_cfg.thread_num/2;
	__empty_thrd_para[1].thrd_stop = g_flow_info_cfg.thread_num;
	__empty_thrd_para[1].athrd_cur = g_flow_info_cfg.thread_num/2;
	__empty_thrd_para[1].athrd_max = g_flow_info_cfg.thread_num;


	printf("__empty_thrd_para[0]: %d, %d -> %d, %d -> %d\n", __empty_thrd_para[0].f_index,
			__empty_thrd_para[0].thrd_start, __empty_thrd_para[0].thrd_stop,
			__empty_thrd_para[0].athrd_cur, __empty_thrd_para[0].athrd_max);
	printf("__empty_thrd_para[1]: %d, %d -> %d, %d -> %d\n", __empty_thrd_para[1].f_index,
			__empty_thrd_para[1].thrd_start, __empty_thrd_para[1].thrd_stop,
			__empty_thrd_para[1].athrd_cur, __empty_thrd_para[1].athrd_max);

	if ( pthread_create(&tid0, NULL, empty_thrd, &__empty_thrd_para[0]) != 0 )
	{
		fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
		return;
	}
	if (1 != g_flow_info_cfg.thread_num) {
		if ( pthread_create(&tid1, NULL, empty_thrd, &__empty_thrd_para[1]) != 0 )
		{
			fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
			return;
		}
	}

	if ( pthread_detach(tid0) != 0 )
	{
		fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid0, strerror(errno));
	}
	if ( pthread_detach(tid1) != 0 )
	{
		fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid1, strerror(errno));
	}
}
/*
 *  start : f_index_put_thrd_crt();
 *
 * */

static void *put_f_index_and_wait(void *data)
{
	struct timespec sleeptime = {.tv_nsec = 100, .tv_sec = 0 }; /* 0.1 milli-second */
	//cpu_set_t mask;
	register int f_index;

#if 0
	CPU_ZERO(&mask);
	/* CPU_SET(0, &mask); //wdb_core */
	CPU_SET(__misc_cores, &mask);  //wdb_core
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
		fprintf(stderr, "%s() - set thread affinity failed.\n", __FUNCTION__);
		exit(1);
	}
#endif

	while (1)
	{
		m_wmb();

		if (utaf_fetch_and_dec(&__tm_out_flag) == 1)
		{
			f_index = utaf_age_index_current + 1;
			__builtin_ia32_lfence();
			f_index = f_index%UTAF_AGE_RING_SIZE;

#if 0 /* wdb_sglpkt_age */
			__f0_index = utaf_age0_index_current + 1;
			__builtin_ia32_lfence();
			__f0_index = __f0_index%UTAF_AGE_RING0_SIZE;
#endif /* wdb_sglpkt_age */

			spinlock_lock(&utaf_empty_qlock);
			__empty_thrd_para[0].f_index = f_index;
			__empty_thrd_para[1].f_index = f_index;
			spinlock_unlock(&utaf_empty_qlock);

#if 1 /* wdb_lfix-5 */
			//utaf_timer_handler_cb(); //wdb_ppp
#endif /* wdb_lfix-5 */

			while (1)
			{
				spinlock_lock(&utaf_empty_qlock);
				if ( __empty_thrd_para[0].f_index == -1 && __empty_thrd_para[1].f_index == -1 )
				{
					spinlock_unlock(&utaf_empty_qlock);
					break;
				}
				spinlock_unlock(&utaf_empty_qlock);
				nanosleep(&sleeptime, NULL);
			}

			m_wmb();
			utaf_atomic_set(f_index, &utaf_age_index_current);
#if 0 /* wdb_sglpkt_age */
			m_wmb();
			utaf_atomic_set(__f0_index, &utaf_age0_index_current);
#endif /* wdb_sglpkt_age */
			m_wmb();
		}
		else
		{
			nanosleep(&sleeptime, NULL);
		}
	}

	return NULL;
}

static void f_index_put_thrd_crt(void)
{
	pthread_t tid;

	if ( pthread_create(&tid, NULL, put_f_index_and_wait, NULL) != 0 )
	{
		fprintf(stderr, "pthread_create() failed. - %s\n", strerror(errno));
		return;
	}
	if ( pthread_detach(tid) != 0 )
	{
		fprintf(stderr, "pthread_detach(%lu) failed. - %s\n", tid, strerror(errno));
	}
}
/*
 *
 * end: f_index_put_thrd_crt;
 *
 * */

void output_to_buf(flow_event *store_buf,session_table *si,int i)
{
	
	//strncmp((char *)store_buf,(char *)&si->session_node,sizeof(flow_event));
	store_buf[i] = si->session_node;

}	
void session_item_free(session_table *si)
{
	int lthrd_id;
	session_item_header *sih = (session_item_header *)((uint8_t *)si - SESSION_HEAD_SIZE);
	lthrd_id = sih->ringq_id;

	spinlock_lock(&session_group.session_item_rq_lock[lthrd_id]);
	si->tm_lst.next = session_group.session_item_ringq[lthrd_id].first;
	session_group.session_item_ringq[lthrd_id].first = &si->tm_lst;
	spinlock_unlock(&session_group.session_item_rq_lock[lthrd_id]);
}


static void ssn_age_timeout_cb(tmout_lst_hdr_t *tlh,flow_conn_t *conn)
{
	    struct hlist_head *timeout_lst = &tlh->timeout_lst_head;

		session_table *si;
		struct hlist_node *n;
		struct hlist_node *t;
		struct hlist_head lf_hdr[SESSION_ITEM_MEMBER_NUM];
		struct hlist_node **lf_tl[SESSION_ITEM_MEMBER_NUM];
		int si_cnt[SESSION_ITEM_MEMBER_NUM];
		flow_event store_buf[SI_BUNCH_SIZE+2];

		session_item_header *sih;
		int i; //wdb_without_thrd_crting => move it here
		int ret;
#if 0
		store_buf=(flow_event *)malloc((g_flow_info_cfg.session_len)*sizeof(flow_event));
		if (!store_buf)
		{
			printf("637 malloc fail \n");
			exit(-1);
		}
#endif
		for (i = 0; i < SESSION_ITEM_MEMBER_NUM; i++)
		{
			lf_hdr[i].first = NULL;
			lf_tl[i] = &lf_hdr[i].first;
			si_cnt[i] = 0;
		}
		i = 0;
		memset(store_buf,0,(SI_BUNCH_SIZE+2)*sizeof(flow_event));
		hlist_for_each_entry_safe(si, t, n, timeout_lst, tm_lst)
		{
			
			__hlist_del(&si->tm_lst);
#if 0
			for (i = 0; i < 10; i++)
			{
				rte_prefetch0((void *)((char *)si + 64*i));
			}
#endif
#if 0
			if (0 == si->stat_netflow_num) {
				DEBUG_SESSION_STAT_SESSION_NO_STAT_NETFLOW;
			}
			if (0 == si->age_netflow_num) {
				DEBUG_SESSION_STAT_SESSION_NO_AGE_NETFLOW;
			}
			if (0 == si->stat_netflow_num && 0 == si->age_netflow_num) {
				DEBUG_SESSION_STAT_SESSION_NO_NETFLOW;
			}
#endif

			//output_age_record(si);
			output_to_buf(store_buf,si,i);
			i++;


			sih = (session_item_header *)((uint8_t *)si - SESSION_HEAD_SIZE);

			*(lf_tl[sih->ringq_id]) = &si->list;
			lf_tl[sih->ringq_id] = &si->list.next;
			(si_cnt[sih->ringq_id])++;
			if (si_cnt[sih->ringq_id] >= SI_BUNCH_SIZE)
			{
				/********* do storage to postgresql ****************/
				//ret=do_event_store_bak(conn,store_buf,SI_BUNCH_SIZE);
				ret=do_event_store_bak(conn,store_buf,i);
				if(ret < 0) {
					grd_disconnect(conn->redis_conn);
					gpq_disconnect(conn->psql_conn);
					ret = flow_connect_db(conn);
					if (ret != 0) {
						CA_LOG(LOG_MODULE, LOG_PROC, "Connect database Server error ..........!\n");
					}
				}
				/********* net store to postgresql **********
				 *
				 *	do_net_store(conn,store_buf,SI_BUNCH_SIZE);
				 *
				 *
				 * **/
				//ret = do_net_store(conn,store_buf,SI_BUNCH_SIZE);
				ret = do_net_store(conn,store_buf,i);
				if(ret < 0) {
					grd_disconnect(conn->redis_conn);
					gpq_disconnect(conn->psql_conn);
					ret = flow_connect_db(conn);
					if (ret != 0) {
						CA_LOG(LOG_MODULE, LOG_PROC, "Connect database NET Server error ..........!\n");
					}
				}
				i=0;
				memset(store_buf,0,(SI_BUNCH_SIZE+2)*sizeof(flow_event));
				
				*(lf_tl[sih->ringq_id]) = NULL;
				session_item_free((session_table *)lf_hdr[sih->ringq_id].first);

				lf_hdr[sih->ringq_id].first = NULL;
				lf_tl[sih->ringq_id] = &lf_hdr[sih->ringq_id].first;
				si_cnt[sih->ringq_id] = 0;
			}
		}
#if 1
		if(i != 0)
		{
		/********* do storage to postgresql ****************/
		//ret=do_event_store_bak(conn,store_buf,SI_BUNCH_SIZE);
		ret=do_event_store_bak(conn,store_buf,i);
		if(ret < 0) {
			grd_disconnect(conn->redis_conn);
			gpq_disconnect(conn->psql_conn);
			ret = flow_connect_db(conn);
			if (ret != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Connect database Server error ..........!\n");
		}
		}
		/********* net store to postgresql **********
		 *
		 *	do_net_store(conn,store_buf,SI_BUNCH_SIZE);
		 *
		 *
		 * **/
		//ret = do_net_store(conn,store_buf,SI_BUNCH_SIZE);
		ret = do_net_store(conn,store_buf,i);
		if(ret < 0) {
			grd_disconnect(conn->redis_conn);
			gpq_disconnect(conn->psql_conn);
			ret = flow_connect_db(conn);
			if (ret != 0) {
				CA_LOG(LOG_MODULE, LOG_PROC, "Connect database NET Server error ..........!\n");
		}
		}
		}
#endif
#if 0
		if(store_buf)
		{
			free(store_buf);
		}
#endif
		for (i = 0; i < SESSION_ITEM_MEMBER_NUM; i++)
		{
			if (lf_hdr[i].first)
			{
				*(lf_tl[i]) = NULL;
				session_item_free((session_table *)lf_hdr[i].first);
			}
		}

		utaf_tmout_lst_hdr_free(tlh);

}


static void *utaf_ssn_age_thread(void *data)
{
	int *uaps = (int *)data;
	int q_id  = *uaps;
	register unsigned int pause_cnt;
	struct utaf_session_age_core_s *ac = &(session_age_core[q_id]);
	//cpu_set_t mask;
	uint64_t obj;
	struct timespec sleeptime = {.tv_nsec = 0, .tv_sec = 1 }; /* 1 second */

#if 0
	CPU_ZERO(&mask);
	CPU_SET(ac->cid, &mask);
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
		fprintf(stderr, "set thread affinity failed for age thread.\n");
	}
#endif

	UTAF_PER_LCORE(_lcore_id) = ac->cid;
	UTAF_PER_LCORE(lthrd_id) = ac->tid;

	flow_conn_t flow_conn;
	int retn;

	retn = flow_connect_db(&flow_conn);
	if (retn == -1) {
		goto EXIT;
	}
	else if (retn == 1) {
		goto EXIT_1;
	}


	while (1)
	{
		for (q_id = 0; q_id < g_flow_info_cfg.thread_num; q_id++)
		{
			spinlock_lock(&utaf_ssn_age_qlock[q_id]);
			if (0 != ringq_dequeue(utaf_ssn_age_queue[q_id], &obj))
			{
				spinlock_unlock(&utaf_ssn_age_qlock[q_id]);

				pause_cnt = (unsigned int)random();
				pause_cnt = pause_cnt%100 + 1;
				while (pause_cnt--) { asm volatile ("pause"); }

				nanosleep(&sleeptime, NULL);

				continue;
			}
			spinlock_unlock(&utaf_ssn_age_qlock[q_id]);
			ssn_age_timeout_cb((tmout_lst_hdr_t *)obj,&flow_conn);
		}
	}
	/* 关闭redis链接 */
	grd_disconnect(flow_conn.redis_conn);
EXIT_1:
	/* 关闭postgreSQL链接 */
	gpq_disconnect(flow_conn.psql_conn);
EXIT:
	//pthread_exit((void*)tid);
	return NULL;
}
						
int utaf_ssn_age_thread_crt(void)
{
	int i, j;
	uint64_t num = 0;
	pthread_t thr_id;
	int utaf_q_id[15];


	printf("session age thread num: %lu\n", g_flow_info_cfg.thread_num);

	utaf_ssn_age_tmout_lst_heads_init();


	for (i = 0; i < UTAF_AGE_RING_SIZE; i++)
	{
		for (j = 0; j < 16; j++)
		{
			g_utaf_timeout_ring[i][j].first = NULL;
			spinlock_init(&g_utaf_timeout_rlck[i][j]);
		}
	}


	num = g_flow_info_cfg.session_len/g_flow_info_cfg.thread_num;

	printf("utaf_ssn_age_queue num: %lu\n", num);

	for (i = 0; i < g_flow_info_cfg.thread_num ; i++)
	{
		utaf_ssn_age_queue[i] = ringq_create("ssn age queue", num, RING_F_SP_ENQ | RING_F_SC_DEQ);
		if (NULL == utaf_ssn_age_queue[i])
		{
			printf("\n[Error]: Create ssn age queue failed.\n");
			return -1;
		}
		spinlock_init(&utaf_ssn_age_qlock[i]);
	}

	for (i = 0; i < 16; i++)
	{
		spinlock_init(&utaf_nf_aged_lock[i]);
		utaf_nf_aged_list[i].first = NULL;
	}

	for (i = 0; i < g_flow_info_cfg.thread_num; i++) {
		utaf_q_id[i] = i;
		pthread_create(&thr_id, NULL, utaf_ssn_age_thread, /* ( void * )(&(session_age_core[i])) */ &utaf_q_id[i]);
	}

	empty_thrd_crt(-1);

	f_index_put_thrd_crt();

	return 0;
}
int create_conf_recv_pthread(void)
{
	pthread_t tid;
    int err;
    printf("create_conf_recv_thread_start!\n");
    
    // ´´½¨ÈÎÎñ½ÓÊÕÏß³Ì
    err = pthread_create(&tid, NULL, gms_config_recv, NULL);
    if (0 != err) {
        printf("can not create config recv thread\n");
    }
	
	return 0;
}
int net_init(void)
{
	mnt_debug("init_session_tables().... \n");
	if (UTAF_OK != init_session_tables())
	{
		return UTAF_FAIL;
	}
	mnt_debug("TimerListInit ...\n");
	if (TimerListInit() < 0)
	{
		return UTAF_FAIL; 
	}
	mnt_debug("utaf_ssn_age_thread_crt() ... \n");
	if (utaf_ssn_age_thread_crt() < 0)
	{
		return UTAF_FAIL;
	}
	mnt_debug("create_conf_recv_pthread() ... \n");
	if (create_conf_recv_pthread() < 0)
	{
		return UTAF_FAIL;
	}
	mnt_debug("net_mnt_ip_init() .... \n");
	if (net_mnt_ip_init() < 0 )
	{
		return UTAF_FAIL;
	}
	if (flow_event_sock_busi()< 0)
	{
		return UTAF_FAIL;	
	}
	if (create_flow_srv_thr() < 0)
	{
		return UTAF_FAIL;
	}
	mnt_debug("find ip 192.168.222.32 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");
	if(net_mnt_ip_lookup(inet_addr("192.168.222.32")))
	{
		mnt_debug(" init ip find ...............................................................\n");
	}	
	else
	{
		mnt_debug(" init ip not    find ...............................................................\n");
	}
	return UTAF_OK;
	
}
