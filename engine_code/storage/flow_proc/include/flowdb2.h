/**
 * flowdb intends to provide a clean and simple interface for
 * applications to store and query huge amount of flow records
 * in an obscure repository. The internal structure of the repo
 * is wholely hidden from applications, so a flow repo can be
 * built by using whatever technologies.
 *
 * The language used in the queries is similar to SQL, with few
 * customizations as specific to flow data.
 */

#ifndef __FLOWDB2_H__
#define __FLOWDB2_H__

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "flowstat.h"
#include "flowrec.h"
#include "flowerr.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* handle of a repo.
 * the detail is invisible to outside. */
typedef struct flowdb2 flowdb2_t;

/* handle of a query.
 * the detail is invisible to outside. */
typedef struct flowquery flowquery_t;

typedef struct flowqueryattr {
	unsigned int nqueue;	// # of result queues
	unsigned int rate;	// check one out of rate
	unsigned int sample;
	unsigned int fix;
} flowqueryattr_t;

/* query callback handlers.
 * using callback handlers makes it easier to implement
 * preemptive scheduling of cocurrent queries, and it also
 * reduces the number of working threads.
 * each query can have two handlers associated, one is
 * called when a cube is about to be scaned, and the other
 * is called when a match record is found. cube handler can
 * dramatically accelerate such queries that perform
 * statistical calculations only. */
typedef struct flowqueryops {
// return codes of cube handler
#define CH_BYPASS	0	// do NOT scan the cube
#define CH_DETAIL	1	// DO scan the cube
#define CH_ABORT	2	// stop the query now
	int (* cube_handler) (
					void * user,
					int qid,	// to locate the queue-specific data
					flowstat_t * cubestat,
					int all_match	// all records in the cube match the condition
					);
// return codes of rec handler
#define RH_CONT		0	// continue with next rec
#define RH_ABORT	1	// stop the query now
	int (* rec_handler) (
					void * user,
					int qid,	// see above
					flowrec_t * rec,	// a temp buffer
					int nrec	// always 1, but...
					);
} flowqueryops_t;

#define QS_READY	0
#define QS_ACTIVE	1
#define QS_PAUSED	2
#define QS_ZOMBIE	3

#define QE_NONE		0
#define QE_FINISHED	1
#define QE_CANCELED	2
#define QE_ABORT	3
#define QE_LIMIT	4
#define QE_OTHER	5

#pragma pack(1)
typedef struct flowquerystat {
	int state;	// QS_XXX
	int ecode;	// QE_XXX
	unsigned ncand;	// # of CUBES to scan
	unsigned nscan;	// # of CUBES scanned
	unsigned nbusi;	// # of CUBES being scanned
	uint64_t npass;	// # of matches
} flowquerystat_t;
#pragma pack()

/* --- open and close --- */

/* the config file looks like this:
 -----------------
 # supported flowbag formats.
 # flowbagformat name libname
 flowbagformat flat libflowbag_flat.so
 flowbagformat sdpart libflowbag_sdpart.so
 flowbagformat xpart libflowbag_xpart.so

 # data partitions.
 # partition name limit
 partition p1 1024GB	# 1TB
 partition p2 2048GB	# 2TB

 # data pathes.
 # flowbag path format partition mode
 flowbag /ntars/var/flowdata flat p1 ro
 flowbag /ntars/var/flowdb_1 sdpart p1 rw
 flowbag /ntars/var/flowdb_2 xpart p2 rw

 # miscellaneous.
 max_coqueries 4
 num_qthreads 10
 lbinterval 3600
 ......
 */
flowdb2_t * flowdb2_open (const char * ini, int flag/* O_RDONLY/O_WRONLY/O_RDWR */);
void flowdb2_close (flowdb2_t * fdb);

/* --- insertion --- */

/* insert new records into the repo, and return the # of
 * records actually inserted.
 * the disk usage is checked at fixed interval, and some
 * old records may get deleted to keep the disk usage bellow
 * the configured limitation. */
int flowdb2_insert (
				flowdb2_t * fdb,
				flowrec_t * rec,	// the array of records to be inserted
				int count	// the number of records to be inserted
				);

/* --- query --- */

#define MAX_QUEUE	128
/* commit a query, without waiting for it to finish, or start. */
flowquery_t * flowquery_commit (
				flowdb2_t * fdb,
				void * cond,	// a condition string (char *) or a checker object
				int is_checker,	// non-zero if 'cond' is an object
				flowqueryattr_t qa,	// query attributes
				flowqueryops_t * qops,	// callback functions
				void * user,	// argument of the callbacks
				flowquerystat_t * is	// the initial state
				);
void flowquery_pause	(flowquery_t * q);
void flowquery_resume	(flowquery_t * q);
void flowquery_cancel	(flowquery_t * q, int ecode);
int  flowquery_stat		(flowquery_t * q, flowquerystat_t * qs);
int  flowquery_isdone	(flowquery_t * q);
void flowquery_wait		(flowquery_t * q, flowquerystat_t * qs);

/* --- deletion --- */

/* delete all records that match the condition, and return
 * the amount of disk space reclaimed, in bytes.
 * XXX: it can be VERY slow if the condition is specified such
 * that there are a lot of data files containing both targets
 * and survivals. in fact, there is no good reason to provide this,
 * since flowdb_insert() takes care of not to flood the disk.
 * anyhow, it's provided, for integrity. a lazy bag could
 * do nothing and simply return zero. */
uint64_t flowdb2_trim (
				flowdb2_t * fdb,
				const char * where
				);

/* --- statistics --- */

/* the returned list must be free()ed */
flowstat_t * flowdb2_stat (
				flowdb2_t * fdb
				);

/* --- misc --- */

/* return the last error */
uint32_t flowdb2_error (
				flowdb2_t * fdb
				);

#ifdef __cplusplus
}
#endif

#endif	/* __FLOWDB2_H__ */


