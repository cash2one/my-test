#ifndef _G_MISC_H
#define _G_MISC_H

#ifdef __cplusplus
extern "C"{
#endif

#ifndef _MISC_H
#define _MISC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct split_var {
	void *addr;
	int maxlen;
};

int str_bin2asc(char *bin_str, int bs_len, char *asc_str, int as_maxlen);

typedef int (*split_fp) (char *str, void *addr, int maxlen);
int sf_s2i(char *str, void *addr, int maxlen);
int sf_s2ul(char *str, void *addr, int maxlen);
int sf_s2us(char *str, void *addr, int maxlen);
int sf_scp(char *str, void *addr, int maxlen);
int str_split(char *orig_str, int delim, split_fp func, struct split_var split_var_table[], int split_var_max);

int getvarbydeli(char *begin, char *end, char *fdeli, int flen, char *ldeli, int llen, char *var, int maxvarlen);
void print_var(char *varname, char *var, int len);

void print_buf(char *buf, int nbuf, int column);
void print_buf2(char *buf, int nbuf, int column);

char *new_strchr(char *string, char *delim);

char *mem_find(char * mem_mother, char * mem_son, int mother_len, int son_len);
/* 删除strline首尾的非可打印字符 */
char *strim(char *strline);

int get_ifr_mac(char *eth, char *mac, int sz);

#ifndef FORWARD
#define FORWARD   1     // not allow to change
#endif
#ifndef BACKWARD
#define BACKWARD -1
#endif
char *search_nosense_char(char *begin, int nsearch, int step);
char *search_sense_char(char *begin, int nsearch, int step);
#define v_search_sense_char(begin, nsearch, step) search_nosense_char((char *)begin, (int)nsearch, (int)step)

unsigned int IP_strtoNum(char * ip);
int execute(char *command, char *buf, int bufmax);

//int get_current_dir(char *cmd, char *path, int max_len);
void change_workdir(char *cmd_path);
void godaemon(void);
int fread_oneline(char *filename, char *buff, int len);

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#ifndef MIN
#define MIN(x,y) \
({ \
	typeof(x) _x = (x); \
	typeof(y) _y = (y); \
	(void) (&_x == &_y); \
	_x < _y ? _x : _y; \
})
#endif

#ifndef MAX
#define MAX(x,y) \
({ \
	typeof(x) _x = (x); \
	typeof(y) _y = (y); \
	(void) (&_x == &_y); \
	_x > _y ? _x : _y; \
})
#endif

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max at all, of course.
 */
#ifndef MIN_T
#define MIN_T(type,x,y) \
({ \
	type __x = (x); \
	type __y = (y); \
	__x < __y ? __x: __y; \
})
#endif

#ifndef MAX_T
#define MAX_T(type,x,y) \
({ \
	type __x = (x); \
	type __y = (y); \
	__x > __y ? __x: __y; \
})
#endif

#ifndef offset_of
#define offset_of(str, member)  ((char *)(&((str *)0)->member) - (char *)0)
#endif

#define ITOA(data) _ITOA(data)
#define _ITOA(data) #data

#endif				// _MISC_H

/************************************************
 *       Filename: file_op.h
 *    Description: file operation
 *
 *        Created: 2009-01-03 00:40:28
 *         Author: david dengwei@venus.com
 ************************************************/
#ifndef _FILE_OP_H
#define _FILE_OP_H

#include <sys/types.h>

int file_length(const char *path, int *len);
int create_path(char *path, const int mode);
int isdir(const char *path);
int isfile(const char *path);
int check_space(const char *dir, unsigned int *free_space);
int make_tmpfile(const char *dir, const char *prefix, char *path, const int maxlen);
int make_tmpdir(const char *dir, const char *prefix, char *path, const int maxlen);
int check_file_size(char *file_path, int max_size);
int check_file_size_fp(FILE *fp, int max_size);
int copy_file_mode(char *src, char *dst, char *mode);
#define copy_file(src, dst) \
	copy_file_mode(src, dst, "w")
#define append_file(src, dst) \
	copy_file_mode(src, dst, "a")
int delete_file(const char *path);

#endif

#ifndef _V_STRING_H
#define _V_STRING_H

#include <sys/types.h>

int ca_strncpy(char *dst, int size, const char *src, int len);
static inline int ca_strcpy(char *dst, int size, char *src)
{
	if (src && src[0]) {
		return ca_strncpy(dst, size, src, strlen(src));
	} else {
		if (dst) {
			dst[0] = '\0';
		}

		return 0;
	}
}

#if 0
int ca_strcpy(char *dst, int size, const char *src);
#endif

/* 
 *	Find the first occurrence of the substring.
 */
char *ca_memstr(const char *haystack, int range, const char *needle, int needle_len);

/* 
 *	Find the last occurrence of the substring.
 */
char *ca_memrstr(const char *haystack, int range, const char *needle, int needle_len);

int ca_memcpy(void *dst, int size, const void *src, int n);

char *ca_memnchr(char *str, const char c, int n);

char *ca_memrchr(const void *s, char c, int n);

#endif

#ifndef __V_OPTIMIZE_H__
#define __V_OPTIMIZE_H__

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#endif

#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <sys/types.h>

int get_cpukhz();
#define cputick_init() get_cpukhz()	// 定义别名

// 禄帽碌脙驴陋禄煤脪脭潞贸 CPU 碌脛脭脣脳陋麓脦脢媒
// 脦陋64脦禄脡猫卤赂陆酶脨脨鹿媒脨脼赂脛
#define rdtscll(val) \
do { \
	register unsigned int __a, __d; \
	asm volatile ("rdtsc" : "=a" (__a), "=d"(__d)); \
	(val) = ((unsigned long long) __a) | (((unsigned long long) __d) << 32); \
} while (0);

#define get_cputick(time) rdtscll(time)

#if 0
static inline unsigned long long get_cputick()
{
	unsigned long long time;

	rdtscll(time);
	return time;
}
#endif

unsigned long long cputick2ms(unsigned long long diff_track);
unsigned long long cputick2ns(unsigned long long diff_track);

#endif

#ifndef _CRC_H
#define _CRC_H

int CRC_32(unsigned long crc_offset, char *aData, unsigned long aSize);

#endif
#ifndef _MD5_H
#define _MD5_H

#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

/* MD5 context. */
typedef struct {
	unsigned long state[4];	/* state (ABCD) */
	unsigned long count[2];	/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} MD5_CTX;

void MD5Init PROTO_LIST((MD5_CTX *));
void MD5Update PROTO_LIST((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final PROTO_LIST((unsigned char[16], MD5_CTX *));

/*-------------------------------------------------------------*/
/*  函数声明                                                   */
/*-------------------------------------------------------------*/
void MessageDigest(unsigned char *szInput, unsigned int inputLen, unsigned char szOutput[16]);
int md5_file(const char *path, unsigned char *checksum);

#endif
/************************************************
 *       Filename: pcre_op.h
 *    Description: 
 *
 *        Created: 2009-01-04 17:43:22
 *         Author: david dengwei@venus.com
 ************************************************/
#ifndef _PCRE_OP_H
#define _PCRE_OP_H

#include <sys/types.h>

#define PCRE_NO_MATCH	0
#define PCRE_RULE_ERR	-1
#define PCRE_MATCH_ERR	-2

typedef struct {
	char *start;
	int len;
} ret_value_t;
/****************************************************************
 *         Name: pcre_strstr
 *  Description: 正则表达式匹配函数
 *  		 隐含条件: !!!!!!!!!!!!
 *  			如果ret_array的start为NULL, 则说明只要赋值
 *  		 	否则，就是写地址
 *  		 
 *       Return: >0 success, 返回参数个数, 第一个参数肯定是返回的总串
 * 		 =0 no match
 *       	 <0 fail when match
 ****************************************************************/
int pcre_strstr(char *check_str, int check_strlen, char *pcre_str, 
			ret_value_t *ret_array, int ret_max_len);

#define MAX_PCRE_RET_NUM 10

#define declare_ret_value(ret_array, NUM) \
	ret_value_t ret_array[NUM];	\
	memset(ret_array, 0, sizeof(ret_array));

#define set_ret_value(ret_array, idx, str) \
	ret_array[idx].start = str; \
	ret_array[idx].len   = sizeof(str);

#define get_ret_value(ret_array, idx, str) \
	v_strncpy(str, sizeof(str), ret_array[idx].start, ret_array[idx].len);

#endif //_PCRE_OP_H

/************************************************
 *       Filename: rc6.h
 *    Description: rc6 interface
 *
 *        Created: 2009-01-09 09:50:45
 *         Author: idp idp@venus.com
 ************************************************/
#ifndef _RC6_H
#define _RC6_H

#define RC6_WORD_SIZE 32	/* word size in bits */
#define RC6_R 	      20	/* based on security estimates */
#define RC6_P32 0xB7E15163	/* Magic constants for key setup */
#define RC6_Q32 0x9E3779B9

/********************************************************
 *	RC6_P32和RC6_Q32的值可根据加密需要进行调整，	*
 *	加密的强度依赖于RC6_P32、RC6_Q32和密钥K		*
 *********************************************************/
/* derived constants */
#define RC6_BYTES   (RC6_WORD_SIZE / 8)	/* bytes per word */
#define RC6_C       ((b + RC6_BYTES - 1) / RC6_BYTES)	/* key in words, rounded up */
#define RC6_R24     (2 * RC6_R + 4)
#define RC6_LGW     5		/* log2(w) -- wussed out */

extern unsigned int S_RC6_S[RC6_R24];	/* Key schedule */

void rc6_key_setup_3(unsigned char *K, int b, unsigned int *rc6_s);
#define rc6_key_setup(K, b) \
	rc6_key_setup_3(K, b, S_RC6_S)

int rc6_encrypt_4(void *pbuf,void *cbuf,int length, unsigned int *rc6_s);
int rc6_decrypt_4(void *cbuf,void *pbuf,int length, unsigned int *rc6_s);

#define rc6_encrypt(pbuf, cbuf, length) \
	rc6_encrypt_4(pbuf, cbuf, length, S_RC6_S)
#define rc6_decrypt(cbuf,pbuf,length) \
	rc6_decrypt_4(cbuf, pbuf, length, S_RC6_S)

int rule_decrypttion(unsigned char *mat);
int rule_encryption(unsigned char *in, unsigned char *out, int out_maxlen);

#endif	// _RC6_H

#if 0
#ifndef _V_TRANSFORM_H
#define _V_TRANSFORM_H
// not transform for ** pointer

#define ca_strtok_r(str, delim, saveptr) \
	(u_int8_t *)strtok_r((char *)(str), (const char *)(delim), (saveptr))

#define ca_strlen(str) \
	(int)strlen((const char *)(str))

#define ca_strpbrk(s, accept) \
	(u_int8_t *)strpbrk((const char *)(s), (const char *)(accept))

#define ca_strtoul(nptr, endptr, base) \
	(u_int)strtoul((const char *)(nptr), (endptr), (int)(base))

#define ca_inet_aton(cp, inp)\
	(int)inet_aton((const char *)(cp), inp)

#define ca_fopen(path, mode) \
	fopen((const char *)path, (const char *)mode)

#define ca_strchr(s, c) \
	(u_int8_t *)strchr((const char *)s, c)

#define ca_fgets(s, size, stream) \
	(u_int8_t *)fgets((char *)s, (int)size, stream)

#define ca_strcmp(s1, s2) \
	(int)strcmp((const char *)s1, (const char *)s2)

#define ca_strncmp(s1, s2, n) \
	(int)strncmp((const char *)s1, (const char *)s2, (size_t)n)

#define ca_strcasecmp(s1, s2) \
	(int)strcasecmp((const char *)s1, (const char *)s2)

#define ca_strncasecmp(s1, s2, n) \
	(int)strncasecmp((const char *)s1, (const char *)s2, (size_t)n)

#define ca_atoi(nptr) \
	(int)atoi((const char *)nptr)

#define ca_snprintf(s1,size, x...) \
	(int)snprintf((char *)s1,size, ##x)
#endif

#endif 

/************************************************
 *       Filename: so_handle.h
 *    Description: so function handle
 *
 *        Created: 2009-03-06 11:17:18
 *         Author: idp idp@venus.com
 ************************************************/
#ifndef _SO_HANDLE_H
#define _SO_HANDLE_H
#include <dlfcn.h>

#define declare_so_handle(so_name) \
	void *so_name##_so_handle = NULL;

static inline void *in_load_so_func(void **handle, char *so_name, char *func_name)
{
	if (*handle == NULL) {
		*handle = dlopen(so_name, RTLD_LAZY);
		if (*handle == NULL) {
//			printf("load %s error\n", so_name);
			return NULL;
		}
	}

	dlerror();
//	char *error = NULL;
	void *func_point = dlsym(*handle, func_name);
//	if ((error = dlerror()) != NULL) {
	if (dlerror() != NULL) {
//		printf("load func_name(%s) from so_name(%s) error: %s\n", func_name, so_name, error);
		dlclose(*handle);
		*handle = NULL;
		return NULL;
	}

	return func_point;
}

#define load_so_func(so_name, func_name) \
	in_load_so_func(&so_name##_so_handle, so_name, func_name)

static inline void in_close_so_handle(void **handle)
{
	if (*handle) {
		dlclose(*handle);
		*handle = NULL;
	}

	return;
}

#define close_so_handle(so_name) \
	in_close_so_handle(&so_name##_so_handle)

typedef struct _so_func_desc {
	char *so_name;		// so name
	char *func_name;	// function name in so

	void *func;		// func
	void *handle;
} so_func_desc;

int common_load_so(so_func_desc *sfd);
int common_close_so(so_func_desc *sfd);

void *load_func_byname(so_func_desc *sfd, char *name);
void *load_func_byid(so_func_desc *sfd, int idx);

#endif	// _SO_HANDLE_H

/************************************************
 *       Filename: sys_op.h
 *    Description: 
 *
 *        Created: 2009-05-15 15:13:50
 *         Author: david dengwei@venus.com
 ************************************************/
#ifndef _SYS_OP_H
#define _SYS_OP_H

// get total mem, unit k
int get_total_mem();
#endif	// _SYS_OP_H

/************************************************
 *       Filename: v_lock.h
 *    Description: 锁定节点的函数, 从hash中抽取
 *
 *        Created: 2009-05-17 11:37:55
 *         Author: david dengwei@venus.com
 ************************************************/
#ifndef _V_LOCK_H
#define _V_LOCK_H

#include <pthread.h>

typedef struct {
	int node_locked;
	pthread_t lock_thread;
	pthread_mutex_t node_mutex;
} lock_t;

static inline void ca_lock_init(lock_t *h)
{
#ifdef _VREENTRANT_V
	memset(h, 0, sizeof(lock_t));
	pthread_mutex_init(&(h->node_mutex), NULL);
#endif
}

static inline void ca_lock(lock_t *h)
{
#ifdef _VREENTRANT_V
	pthread_t my_thread = pthread_self();
	if (!h->node_locked || h->lock_thread != my_thread) {
		if (likely(pthread_mutex_lock(&h->node_mutex) == 0)) {
			h->lock_thread = my_thread;
			h->node_locked = 1;
		}
	} else {
		(h->node_locked)++;
	}
#endif
}

static inline void ca_unlock(lock_t *h)
{
#ifdef _VREENTRANT_V
	if (--(h->node_locked) == 0) {
		pthread_mutex_unlock(&h->node_mutex);
	}
#endif
}

#endif	// _V_LOCK_H


#ifdef __cplusplus
}
#endif

#endif
