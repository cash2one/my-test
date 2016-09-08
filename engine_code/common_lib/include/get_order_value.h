/*******************************************
 *  get net or host order value.
 *  get_order_value.h
 *  Ford
 *  2007-04-03
 *******************************************/
#ifndef __GET_VALUE_H__
#define __GET_VALUE_H__

#include <sys/types.h>

/* add for utm-arm; Ford; 2007-03-31; begin */
/* get net order value */
static inline int16_t get_netorder_16(u_int8_t *data)
{
	return (int16_t)(((u_int8_t)data[0]<<8) | ((u_int8_t)data[1]));
}

static inline u_int16_t get_netorder_u16(u_int8_t *data)
{
	return (u_int16_t)(((u_int8_t)data[0]<<8) | ((u_int8_t)data[1]));
}

static inline int32_t get_netorder_32(u_int8_t *data)
{
	return (int32_t)(((u_int8_t)data[0]<<24) | ((u_int8_t)data[1]<<16) 
			| ((u_int8_t)data[2]<<8) | ((u_int8_t)data[3]));
}

static inline u_int32_t get_netorder_u32(u_int8_t *data)
{
	return (u_int32_t)(((u_int8_t)data[0]<<24) | ((u_int8_t)data[1]<<16)
			| ((u_int8_t)data[2]<<8) | ((u_int8_t)data[3]));
}

static inline int64_t get_netorder_64(u_int8_t *data)
{
	u_int64_t first;
	u_int64_t second;

	first = (u_int64_t)(((u_int8_t)data[0]<<24) | ((u_int8_t)data[1]<<16)
			| ((u_int8_t)data[2]<<8) | ((u_int8_t)data[0]));
	second =(u_int64_t)(((u_int8_t)data[4]<<24) | ((u_int8_t)data[5]<<16)
			| ((u_int8_t)data[6]<<8) | ((u_int8_t)data[7]));

	return (int64_t)((first<<32)|second);
}

static inline u_int64_t get_netorder_u64(u_int8_t *data)
{
	u_int64_t first;
	u_int64_t second;

	first = (u_int64_t)(((u_int8_t)data[0]<<24) | ((u_int8_t)data[1]<<16) 
			| ((u_int8_t)data[2]<<8) | ((u_int8_t)data[0]));
	second =(u_int64_t)(((u_int8_t)data[4]<<24) | ((u_int8_t)data[5]<<16) 
			| ((u_int8_t)data[6]<<8) | ((u_int8_t)data[7]));
		
	return (u_int64_t)((first<<32)|second);
}

/* get host order value */
static inline int16_t get_hostorder_16(u_int8_t *data)
{
	return (int16_t)(((u_int8_t)data[1]<<8)|((u_int8_t)data[0]));
}

static inline u_int16_t get_hostorder_u16(u_int8_t *data)
{
	return (u_int16_t)(((u_int8_t)data[1]<<8) | ((u_int8_t)data[0]));
}

static inline int32_t get_hostorder_32(u_int8_t *data)
{
	return (int32_t)(((u_int8_t)data[3]<<24) | ((u_int8_t)data[2]<<16)
			| ((u_int8_t)data[1]<<8) | ((u_int8_t)data[0]));
}

static inline u_int32_t get_hostorder_u32(u_int8_t *data)
{
        return (u_int32_t)(((u_int8_t)data[3]<<24) | ((u_int8_t)data[2]<<16)
			| ((u_int8_t)data[1]<<8) | ((u_int8_t)data[0]));
}

static inline int64_t get_hostorder_64(u_int8_t *data)
{
	u_int64_t first;
	u_int64_t second;

        first = (u_int64_t)(((u_int8_t)data[7]<<24) | ((u_int8_t)data[6]<<16) 
			| ((u_int8_t)data[5]<<8) | ((u_int8_t)data[4]));
	second = (u_int64_t)(((u_int8_t)data[3]<<24) | ((u_int8_t)data[2]<<16)
			| ((u_int8_t)data[1]<<8) | ((u_int8_t)data[0]));

	return (int64_t)((first<<32)|second);
}

static inline u_int64_t get_hostorder_u64(u_int8_t *data)
{
	u_int64_t first;
	u_int64_t second;

	first = (u_int64_t)(((u_int8_t)data[7]<<24) | ((u_int8_t)data[6]<<16) 
			| ((u_int8_t)data[5]<<8) | ((u_int8_t)data[4]));
	second = (u_int64_t)(((u_int8_t)data[3]<<24) | ((u_int8_t)data[2]<<16)
			| ((u_int8_t)data[1]<<8) | ((u_int8_t)data[0]));

	return (u_int64_t)((first<<32)|second);
}
/* tmp add for utm-arm; Ford; 2007-03-31; end */
#endif
