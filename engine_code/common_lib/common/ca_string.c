#include <sys/types.h>
#include <string.h>

#include "misc.h" /* for macro MIN */
static inline int _v_strncpy(char *dst, int size, const char*src, int len)
{
	int l;
	
	l = MIN(size - 1, len);
	memcpy(dst, src, l);
	dst[l] = 0;

	return l;
}

int v_strncpy(char *dst, int size, const char *src, int len)
{
	if (unlikely(size < 1 || len < 1 || src == NULL)) {
		dst[0] = '\0';
		return 0;
	}

	return _v_strncpy(dst, size, src, len);
}

#if 0
int v_strcpy(char *dst, int size, const char *src)
{
	int len = strlen(src);
	
	if (unlikely(size < 1 || len < 1)) {
		dst[0] = '\0';
		return 0;
	}

	return _v_strncpy(dst, size, src, len);
}
#endif

#ifndef MIPS_LINUX //modi: by kzk, refine v_memstr, v_memrstr at mips machine
char *v_memstr(const char *haystack, int range, const char *needle, int needle_len)
{
	char *p = (char *)haystack;
	int h_len = range;
	
	if(unlikely(needle_len < 1 || !needle))
		return (char*)haystack;

	if(unlikely(needle_len > range || !range || !haystack))
		return NULL;
		
	
	while(h_len >= needle_len) {
		if(!memcmp(p, needle, needle_len))
			return p;
		p++;
		h_len--;
	}

	return NULL;
}

char *v_memrstr(const char * haystack, int range, const char * needle, int needle_len)
{
	char *p = (char *)(haystack + range);
	
	if(unlikely(needle_len < 1 || !needle))
		return (char *)haystack;

	if(unlikely(needle_len > range || !range || !haystack))
		return NULL;

	p -= needle_len;

	while(p >= haystack) {
		if(!memcmp(p, needle, needle_len))
			return p;
		p--;
	}

	return NULL;
}
#else
//note: by kzk, in the last char in haystack is not included when comparing; be care of the offset
char *v_memstr(const char * haystack, int range, const char * needle, int needle_len)
{
	char *p = haystack;
	char *end = NULL;
	
	if(unlikely(needle_len < 1 || !needle))
		return haystack;

	if(unlikely(needle_len > range || !range || !haystack))
		return NULL;
		
	end = (haystack + range - needle_len);

	while(p <= end) {
		p = v_memchr(p, end + 1, *needle);
		if (p) {
			if(!memcmp(p, needle, needle_len)) {
				return p;
			} else {
				p++;
			}
		} else {
			return NULL;
		}
	}

	return NULL;
}

char *v_memrstr(const char * haystack, int range, const char * needle, int needle_len)
{
	char *p = haystack + range;
	
	if(unlikely(needle_len < 1 || !needle))
		return haystack;

	if(unlikely(needle_len > range || !range || !haystack))
		return NULL;

	p -= needle_len;

	while(p >= haystack) {
		p = v_memrchr(haystack, (*)(p + 1), *needle);
		if (p) {
			if(!memcmp(p, needle, needle_len)) {
				return p;
			} else {
				p--;
			}
		} else {
			return NULL;
		}
	}

	return NULL;
}
#endif


int v_memcpy(void *dst, int size, const void *src, int n)
{
	int l;

	if (unlikely(size < 1 || n < 1))
		return 0;

	l = MIN(size, n);
	memcpy(dst, src, l);

	return l;
}

char * v_memnchr(char * str, const char c, int n)
{
        char *p = str;

        while (p < str + n) {

                if (*p == c) {
                        goto out;
                }
                p++;
        }

        p = NULL;
out:
        return p;
}
/*
char* v_memrchr(const char *begin, const char *end,char ch)
{
	const char *p = end - 1;
	
	if(unlikely(!begin || !end)){
		return NULL;
	}

	while(p >= begin){
		if(*p == ch){
			return(char*)p;
		}
		--p;
	}
	return NULL;
}
*/

/* jiaoyf,change the type :int -> int8_t*/
char  * 
v_memrchr(const void *s, char c, int n)
{
	char *p =NULL;
	char *ps = NULL;

	if(unlikely(!s || n <= 0)){
		return NULL;
	}

	ps = (char*)s;
	p = ps + n -1;
	
	while((p > ps)&&((*p) != c) ){
		p --;
	}
	
	if((*p) !=  c ){
		return NULL;
	}
	return p;
}

