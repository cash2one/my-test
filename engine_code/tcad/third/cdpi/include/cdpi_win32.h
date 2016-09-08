
#ifndef __CDPI_WIN32_INCLUDE_FILE__
#define __CDPI_WIN32_INCLUDE_FILE__

#ifdef WIN32
#include <Winsock2.h> /* winsock.h is included automatically */
#include <process.h>
#include <io.h>
#include <getopt.h> /* getopt from: http://www.pwilson.net/sample.html. */
#include <process.h> /* for getpid() and the exec..() family */

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define snprintf	_snprintf

extern char* strsep(char **stringp, const char *delim);

#define __attribute__(x)
#include <stdint.h>
#ifndef __GNUC__
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   uint;
typedef unsigned long  u_long;
#endif
typedef u_char  u_int8_t;
typedef u_short u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned __int64 u_int64_t;


#define pthread_t                HANDLE
#define pthread_mutex_t          HANDLE
#define pthread_rwlock_t         pthread_mutex_t
#define pthread_rwlock_init      pthread_mutex_init
#define pthread_rwlock_wrlock    pthread_mutex_lock
#define pthread_rwlock_rdlock    pthread_mutex_lock
#define pthread_rwlock_unlock    pthread_mutex_unlock
#define pthread_rwlock_destroy	 phread_mutex_destroy

extern unsigned long waitForNextEvent(unsigned long ulDelay /* ms */);

#define sleep(a /* sec */) waitForNextEvent(1000*a /* ms */)

#endif /* Win32 */

#endif /* __CDPI_WIN32_INCLUDE_FILE__ */
