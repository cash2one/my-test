#ifndef _FIXENDIAN_H
#define _FIXENDIAN_H

//for i386
#define FIX_LITTLE_ENDIAN

#ifdef FIX_LITTLE_ENDIAN
#define fix_endian(x) (((x)<<24 & 0xff000000) | ((x)<<8  & 0x00ff0000) | ((x)>>8  & 0x0000ff00) | ((x)>>24 & 0x000000ff))
#else
#define fix_endian(x)  (x)
#endif

#endif //_FIXENDIAN_H
