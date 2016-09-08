#ifndef _BYTEORDER_H_
#define _BYTEORDER_H_

#include <stdint.h>

/*
 * An internal function to swap bytes in a 16-bit value.
 *
 * It is used by rte_bswap16() when the value is constant. Do not use
 * this function directly; rte_bswap16() is preferred.
 */
static inline uint16_t
constant_bswap16(uint16_t x)
{
	return (uint16_t)(((x & 0x00ffU) << 8) |
		((x & 0xff00U) >> 8));
}


/*
 * An internal function to swap bytes in a 32-bit value.
 *
 * It is used by rte_bswap32() when the value is constant. Do not use
 * this function directly; rte_bswap32() is preferred.
 */
static inline uint32_t
rte_constant_bswap32(uint32_t x)
{
	return  ((x & 0x000000ffUL) << 24) |
		((x & 0x0000ff00UL) << 8) |
		((x & 0x00ff0000UL) >> 8) |
		((x & 0xff000000UL) >> 24);
}


/*
 * An internal function to swap bytes of a 64-bit value.
 *
 * It is used by rte_bswap64() when the value is constant. Do not use
 * this function directly; rte_bswap64() is preferred.
 */
static inline uint64_t
constant_bswap64(uint64_t x)
{
	return  ((x & 0x00000000000000ffULL) << 56) |
		((x & 0x000000000000ff00ULL) << 40) |
		((x & 0x0000000000ff0000ULL) << 24) |
		((x & 0x00000000ff000000ULL) <<  8) |
		((x & 0x000000ff00000000ULL) >>  8) |
		((x & 0x0000ff0000000000ULL) >> 24) |
		((x & 0x00ff000000000000ULL) >> 40) |
		((x & 0xff00000000000000ULL) >> 56);
}




/*
 * An architecture-optimized byte swap for a 16-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap16().
 */
static inline uint16_t arch_bswap16(uint16_t _x)
{
	register uint16_t x = _x;
	asm volatile ("xchgb %b[x1],%h[x2]"
		      : [x1] "=Q" (x)
		      : [x2] "0" (x)
		      );
	return x;
}


/*
 * An architecture-optimized byte swap for a 32-bit value.
 *
 * Do not use this function directly. The preferred function is rte_bswap32().
 */
static inline uint32_t arch_bswap32(uint32_t _x)
{
	register uint32_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}


/* 64-bit mode */
static inline uint64_t rte_arch_bswap64(uint64_t _x)
{
	register uint64_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}



/**
 * Swap bytes in a 16-bit value.
 */
#define bswap16(x) ((uint16_t)(__builtin_constant_p(x) ?		\
				   constant_bswap16(x) :		\
				   arch_bswap16(x)))

#ifndef RTE_FORCE_INTRINSICS
/**
 * Swap bytes in a 32-bit value.
 */
#define bswap32(x) ((uint32_t)(__builtin_constant_p(x) ?		\
				   constant_bswap32(x) :		\
				   arch_bswap32(x)))

/**
 * Swap bytes in a 64-bit value.
 */
#define bswap64(x) ((uint64_t)(__builtin_constant_p(x) ?		\
				   constant_bswap64(x) :		\
				   arch_bswap64(x)))

#else

/* __builtin_bswap16 is only available gcc 4.8 and upwards */

/**
 * Swap bytes in a 32-bit value.
 */
#define bswap32(x) __builtin_bswap32(x)

/**
 * Swap bytes in a 64-bit value.
 */
#define bswap64(x) __builtin_bswap64(x)

#endif



/**
 * Convert a 16-bit value from CPU order to little endian.
 */
#define cpu_to_le_16(x) (x)

/**
 * Convert a 32-bit value from CPU order to little endian.
 */
#define cpu_to_le_32(x) (x)

/**
 * Convert a 64-bit value from CPU order to little endian.
 */
#define cpu_to_le_64(x) (x)


/**
 * Convert a 16-bit value from CPU order to big endian.
 */
#define cpu_to_be_16(x) bswap16(x)

/**
 * Convert a 32-bit value from CPU order to big endian.
 */
#define cpu_to_be_32(x) bswap32(x)

/**
 * Convert a 64-bit value from CPU order to big endian.
 */
#define cpu_to_be_64(x) bswap64(x)


#endif

