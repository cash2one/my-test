#ifndef __COMMON__H__
#define __COMMON__H__


#define WT_KB               (1024ULL)
#define WT_MB               (1024ULL * WT_KB)
#define WT_GB               (1024ULL * WT_MB)
#define GLOBAL_VM_SIZE      (100ULL * WT_GB)

#define PROGRAM_DIR         "/gms/tcad"
#define CDPI_DIR            "/gms/tcad/protocol"
#define DDOS_CONFIG_DIR		"/gms/tcad/ddos"

#define POWEROF2(x) ((((x)-1) & (x)) == 0)

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#ifndef offsetof
#define offsetof(type, field)  ((size_t) &( ((type *)0)->field) )
#endif


#if 0 /* wdb_ppp */
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif /* wdb_ppp */



/**
 * Check if a branch is likely to be taken.
 *
 * This compiler builtin allows the developer to indicate if a branch is
 * likely to be taken. Example:
 *
 *   if (likely(x > 1))
 *      do_stuff();
 *
 */
#ifndef likely
#define likely(x)  __builtin_expect((x),1)
#endif /* likely */

/**
 * Check if a branch is unlikely to be taken.
 *
 * This compiler builtin allows the developer to indicate if a branch is
 * unlikely to be taken. Example:
 *
 *   if (unlikely(x < 1))
 *      do_stuff();
 *
 */
#ifndef unlikely
#define unlikely(x)  __builtin_expect((x),0)
#endif /* unlikely */




#define LOG printf


static inline int get_power(int x)
{
	int y = 1;
	while(x > y)
	{
		y = y * 2;
	}
	return y;
}

inline int set_thread_title(const char* fmt, ...);
int tcad_conf_get_profile_string(char *profile, char *AppName, char *KeyName, char *KeyVal );

#endif
