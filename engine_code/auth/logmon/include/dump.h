#ifndef _DUMP_H
#define _DUMP_H

#include <stdio.h>
#include <stdlib.h>

/* dump macro for debug begin */
#ifdef DEBUG_DUMP
#define DUMP(...) \
	do {\
		printf("DUMP >> ");\
		printf(__VA_ARGS__);\
		printf("\n");\
	} while(0)
#else
#define DUMP(...) \
	do {\
	} while(0)
#endif


#ifdef DEBUG_DUMP_ERR
#define DUMP_ERR(...) \
	do {\
		fprintf(stderr, "** DUMP ERROR <in file %s line %d> >> ", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
		fprintf(stderr, "\n");\
	} while(0)
#else
#define DUMP_ERR(...) \
	do {\
	} while(0)
#endif

#ifdef DEBUG_DUMP
#define DUMP_R(...) \
	do {\
		printf(__VA_ARGS__);\
	} while(0)
#else
#define DUMP_R(...) \
	do {\
	} while(0)
#endif
/** dump macro for debug end **/

#define WARNING(...) \
	do {\
		fprintf(stderr, "** Warning ** <in file %s line %d>: ", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
	} while(0)

#define OOPS(...) \
	do {\
		fprintf(stderr, "** Oops ** <in file %s line %d>: ", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
	} while(0)

#ifndef NDEBUG
#define ERROR(...) \
	do {\
		fprintf(stderr, "** Error ** <in file %s line %d>: ", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
	} while(0)
#else
#define ERROR(...)
#endif /* NDEBUG */

#endif /* _DUMP_H */

