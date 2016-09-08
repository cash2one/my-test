/*
 * Avoid danger of memory-align problem on ARM
 */
#ifndef _BYTE_ALIGN_H_
#define _BYTE_ALIGN_H_

#include <sys/types.h>

static inline int16_t get_align_16(u_int8_t *data)
{
#ifdef AVOID_ALIGN_BYTE
	return (int16_t)((data[0]<<8)|(data[1]));
#else
	return *((int16_t*)data);
#endif
}

static inline u_int16_t get_align_u16(u_int8_t *data)
{
#ifdef AVOID_ALIGN_BYTE
	return (u_int16_t)((data[0]<<8)|(data[1]));
#else
	return *((u_int16_t*)data);
#endif
}

static inline int32_t get_align_32(u_int8_t *data)
{
#ifdef AVOID_ALIGN_BYTE
	return (int32_t)((data[0]<<24)|(data[1]<<16)|(data[2]<<8)|(data[3]));
#else
	return *((int32_t*)data);
#endif
}

static inline u_int32_t get_align_u32(u_int8_t *data)
{
#ifdef AVOID_ALIGN_BYTE
	return (u_int32_t)((data[0]<<24)|(data[1]<<16)|(data[2]<<8)|(data[3]));
#else
	return *((u_int32_t*)data);
#endif
}

static inline int64_t get_align_64(u_int8_t *data)
{
#ifdef AVOID_ALIGN_BYTE
	u_int64_t first;
	u_int64_t second;

	first = (u_int64_t)((data[0]<<24)|(data[1]<<16)|(data[2]<<8)|(data[0]));
	second =(u_int64_t)((data[4]<<24)|(data[5]<<16)|(data[6]<<8)|(data[7]));

	return (int64_t)((first<<32)|second);
#else
	return *((int64_t*)data);
#endif
}

static inline u_int64_t get_align_u64(u_int8_t *data)
{
#ifdef AVOID_ALIGN_BYTE
	u_int64_t first;
	u_int64_t second;

	first = (u_int64_t)((data[0]<<24)|(data[1]<<16)|(data[2]<<8)|(data[0]));
	second =(u_int64_t)((data[4]<<24)|(data[5]<<16)|(data[6]<<8)|(data[7]));
		
	return (u_int64_t)((first<<32)|second);
#else
	return *((u_int64_t*)data);
#endif
}

#endif
