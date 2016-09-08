#ifndef __FLOWSTAT_H__
#define __FLOWSTAT_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>	// malloc
#include <string.h>	// memset
#include <assert.h>
#include <unistd.h>	// read/write
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* flowdb stat info header.
 * the supported stat info defers from bag to bag,
 * so the stat info as a whole is just a linked list of
 * supported items.
 * NOTE THAT:
 * > with regard to an entire flowdb/bag,
 *   FLOWST_TIME and FLOWST_DU MUST be supported;
 * > with regard to a single cube,
 *   FLOWST_TIME and FLOWST_ROUTER MUST be supported. */
typedef struct flowstat {
	struct flowstat * next;
	uint16_t type;	// format of data
	uint16_t len;	// length in bytes of data
	char data [0];
} flowstat_t;

// used to signal the end of a list in a data file
#define FLOWST_NONE	0

#define FLOWST_TIME	1
/* > all records in question are guaranteed to be within
 *   the time range as defined by oldest and latest;
 * > it's possible that no records exist at oldest or latest at all. */
typedef struct flowstat_time {
	time_t oldest;
	time_t latest;
} flowstat_time_t;

static inline void flowstat_time_init (flowstat_time_t * sttime)
{
	sttime->oldest = 0x7fffffff;
	sttime->latest = 0;
}
static inline void flowstat_time_update (flowstat_time_t * sttime,
				time_t latest)
{
	if (sttime->oldest > latest)
		sttime->oldest = latest;
	if (sttime->latest < latest)
		sttime->latest = latest;
}
static inline void flowstat_time_minus (flowstat_time_t * sttime,
				time_t oldest, time_t latest)
{
	if (latest >= sttime->oldest &&
					oldest <= sttime->latest) {
		if (oldest <= sttime->oldest)
			sttime->oldest = latest + 1;
		if (latest >= sttime->latest)
			sttime->latest = oldest - 1;
		if (sttime->oldest > sttime->latest)
			flowstat_time_init (sttime);
		return;	// ===---
	}
}
static inline void flowstat_time_merge (flowstat_time_t * sttimeto,
				flowstat_time_t * sttimefrom)
{
	if (sttimeto->oldest > sttimefrom->oldest)
		sttimeto->oldest = sttimefrom->oldest;
	if (sttimeto->latest < sttimefrom->latest)
		sttimeto->latest = sttimefrom->latest;
	return;	// ===---
}

#define FLOWST_ROUTER	2
/* all records in question are guaranteed to
 * come from the reported router */
typedef struct flowstat_router {
	uint32_t rip;	// router
} flowstat_router_t;

#define FLOWST_COUNT	3
// if supported, reporting the count of records in a bag or cube
typedef struct flowstat_count {
	uint64_t nrec;	// # of records
} flowstat_count_t;

static inline void flowstat_count_init (flowstat_count_t * stcount)
{ stcount->nrec = 0; }
static inline uint64_t flowstat_count_update (flowstat_count_t * stcount,
				uint64_t nrec)
{ stcount->nrec += nrec; return stcount->nrec; }
static inline void flowstat_count_minus (flowstat_count_t * stcount,
				uint64_t nrec)
{ stcount->nrec -= nrec; }
static inline void flowstat_count_merge (flowstat_count_t * stcountto,
				flowstat_count_t * stcountfrom)
{ stcountto->nrec += stcountfrom->nrec; }

#define FLOWST_DU	4
// the reported number should reflect the real disk usage
typedef struct flowstat_du {
	uint64_t bytes;	// disk usage in bytes
} flowstat_du_t;

static inline void flowstat_du_init (flowstat_du_t * stdu)
{ stdu->bytes = 0; }
static inline uint64_t flowstat_du_update (flowstat_du_t * stdu,
				uint64_t bytes)
{ stdu->bytes += bytes; return stdu->bytes; }
static inline void flowstat_du_minus (flowstat_du_t * stdu,
				uint64_t bytes)
{ stdu->bytes -= bytes; }
static inline void flowstat_du_merge (flowstat_du_t * stduto,
				flowstat_du_t * stdufrom)
{ stduto->bytes += stdufrom->bytes; }

//-----------------------------------------------------
// helpers for manipulating stat info

// alloc and init a stat info header
static inline flowstat_t * flowstat_alloc (
				uint16_t type, uint16_t len)
{
	size_t sz = sizeof (flowstat_t) + len;
	flowstat_t * ret = (flowstat_t *) malloc (sz);
	if (ret != NULL) {
		ret->next = NULL;
		ret->type = type;
		ret->len = len;
	}
	return ret;
}

// add 'item' to a list
static inline void flowstat_add (
				flowstat_t ** head, flowstat_t * item)
{
#ifndef NDEBUG
	// check for duplications
	flowstat_t * tmp;
	for (tmp=*head ; tmp ; tmp=tmp->next) {
		if (tmp->type == item->type)
			assert (0 == 1);
	}
#endif
	item->next = *head;
	*head = item;
}

// free a list of stat items
static inline void flowstat_free (
				flowstat_t * st)
{
	flowstat_t * tmp;
	for (tmp=st ; tmp ; tmp=st) {
		st = tmp->next;
		free (tmp);
	}
}

// find a specified type in a list
static inline flowstat_t * flowstat_find (
				flowstat_t * st, uint16_t type)
{
	flowstat_t * tmp;
	for (tmp=st ; tmp ; tmp=tmp->next)
		if (tmp->type == type)
			return tmp;
	return NULL;
}

// make a duplicate of a list
static inline flowstat_t * flowstat_dup (
				flowstat_t * st)
{
	flowstat_t * ret = NULL, * tmp;
	for (; st ; st=st->next) {
		tmp = flowstat_alloc (st->type, st->len);
		if (tmp == NULL)
			break;
		memcpy (tmp->data, st->data, st->len);
		flowstat_add (&ret, tmp);
	}
	return ret;
}

/* merge st2 to *st1.
 * only items with common types are kept in *st1,
 * in other words, a type is reported only when it's
 * reported by all bags. */
static inline void flowstat_merge (
				flowstat_t ** st1, flowstat_t * st2)
{
	flowstat_t * tmp1, * tmp2, * tmp, * ret = NULL;
	for (tmp2=st2 ; tmp2 ; tmp2=tmp2->next) {
		if (FLOWST_ROUTER == tmp2->type)
			// can not merge
			continue;

		tmp1 = flowstat_find (*st1, tmp2->type);
		if (tmp1 == NULL)
			continue;

		tmp = flowstat_alloc (tmp2->type, tmp2->len);
		if (tmp == NULL)
			break;

		// merge tmp2 to tmp1
		switch (tmp2->type) {
		case FLOWST_TIME:
			{
				flowstat_time_t * tm1 = (flowstat_time_t *) tmp1->data;
				flowstat_time_t * tm2 = (flowstat_time_t *) tmp2->data;
				flowstat_time_merge (tm1, tm2);
			}
			break;
		case FLOWST_COUNT:
			{
				flowstat_count_t * cnt1 = (flowstat_count_t *) tmp1->data;
				flowstat_count_t * cnt2 = (flowstat_count_t *) tmp2->data;
				flowstat_count_merge (cnt1, cnt2);
			}
			break;
		case FLOWST_DU:
			{
				flowstat_du_t * du1 = (flowstat_du_t *) tmp1->data;
				flowstat_du_t * du2 = (flowstat_du_t *) tmp2->data;
				flowstat_du_merge (du1, du2);
			}
			break;
		default:	// bad type
			assert (0 == 1);
			break;
		}
		memcpy (tmp->data, tmp1->data, tmp1->len);
		flowstat_add (&ret, tmp);
	}
	flowstat_free (*st1);
	*st1 = ret;
}

/* save a list to a file.
 * return the bytes written on success, -1 otherwise */
static inline ssize_t flowstat_save (
				flowstat_t * st, int fd)
{
	ssize_t ret = 0, n;
	uint16_t none_type = FLOWST_NONE;

	flowstat_t * tmp;
	for (tmp=st ; tmp ; tmp=tmp->next) {
		n = write (fd, &tmp->type, sizeof (tmp->type));
		if (n != sizeof (tmp->type))
			return -1;
		ret += n;
		n = write (fd, &tmp->len, sizeof (tmp->len));
		if (n != sizeof (tmp->len))
			return -1;
		ret += n;
		n = write (fd, tmp->data, tmp->len);
		if (n != tmp->len)
			return -1;
		ret += n;
	}
	// terminate the list
	n = write (fd, &none_type, sizeof (none_type));
	if (n == sizeof (none_type))
		return (ret+n);
	return -1;
}

// load a list from a file
static inline flowstat_t * flowstat_load (int fd)
{
	flowstat_t * ret = NULL;

	while (1) {
		uint16_t type, len;
		ssize_t n = read (fd, &type, sizeof (type));
		if (n != sizeof (type) ||
						FLOWST_NONE == type)
			break;
		n = read (fd, &len, sizeof (len));
		if (n != sizeof (len))
			break;
		flowstat_t * tmp = flowstat_alloc (type, len);
		if (tmp == NULL)
			break;
		n = read (fd, tmp->data, len);
		if (n != len) {
			flowstat_free (tmp);
			break;
		}
		// add to the list
		flowstat_add (&ret, tmp);
		continue;
	}
	return ret;
}

#ifdef __cplusplus
}
#endif

#endif	/* __FLOWSTAT_H__ */

