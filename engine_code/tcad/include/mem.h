#ifndef __MEM__H__
#define __MEM__H__

#include <stdlib.h>

//#define MALLOC malloc
//#define FREE   free




#define CACHE_LINE_SIZE 64                  /**< Cache line size. */
#define CACHE_LINE_MASK (CACHE_LINE_SIZE-1) /**< Cache line mask. */



#define cache_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))



#endif
