
#ifndef __CDPI_UNIX_INCLUDE_FILE__
#define __CDPI_UNIX_INCLUDE_FILE__

#include "linux_compat.h"

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <netinet/in.h>
#if defined(__NetBSD__) || defined(__OpenBSD__)
#include <netinet/in_systm.h>
#if defined(__OpenBSD__)
#include <pthread.h>
#endif
#endif
#endif

#ifndef WIN32
#ifndef __KERNEL__

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#else
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#endif
#endif

#endif /* __CDPI_UNIX_INCLUDE_FILE__ */
