#ifndef __UTAF_RWLOCK_H__
#define __UTAF_RWLOCK_H__ 1

#ifndef __ATOMIC_H__
#define __ATOMIC_H__ 1

#define LOCK "lock ; "

typedef struct { volatile int counter; } atomic_t;

static __inline__ int atomic_sub_and_test(int i, atomic_t *v)
{
        unsigned char c;

        __asm__ __volatile__(
                LOCK "subl %2,%0; sete %1"
                :"=m" (v->counter), "=qm" (c)
                :"ir" (i), "m" (v->counter) : "memory");
        return c;
}

static __inline__ void atomic_add(int i, atomic_t *v)
{
        __asm__ __volatile__(
                LOCK "addl %1,%0"
                :"=m" (v->counter)
                :"ir" (i), "m" (v->counter));
}

#endif /* __ATOMIC_H__ */

typedef struct {
        volatile unsigned int lock;
} raw_rwlock_t;

typedef struct {
        raw_rwlock_t raw_lock;
#if 0
#if defined(CONFIG_PREEMPT) && defined(CONFIG_SMP)
        unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
        unsigned int magic, owner_cpu;
        void *owner;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
        struct lockdep_map dep_map;
#endif
#endif
} rwlock_t;

#define write_lock(lock)                _write_lock(lock)
#define read_lock(lock)                 _read_lock(lock)

#define read_unlock(lock)              __raw_read_unlock(&(lock)->raw_lock)
#define write_unlock(lock)             __raw_write_unlock(&(lock)->raw_lock)

#define __cond_lock(x) (x)
#define write_trylock(lock)             __cond_lock(_write_trylock(lock))

#define _raw_read_lock(rwlock)         __raw_read_lock(&(rwlock)->raw_lock)
#define _raw_read_trylock(rwlock)      __raw_read_trylock(&(rwlock)->raw_lock)
#define _raw_read_unlock(rwlock)       __raw_read_unlock(&(rwlock)->raw_lock)
#define _raw_write_lock(rwlock)        __raw_write_lock(&(rwlock)->raw_lock)
#define _raw_write_trylock(rwlock)     __raw_write_trylock(&(rwlock)->raw_lock)
#define _raw_write_unlock(rwlock)      __raw_write_unlock(&(rwlock)->raw_lock)

#define rwlock_acquire_read(l, s, t, i)        do { } while (0)
#define rwlock_acquire(l, s, t, i)             do { } while (0)

#define RW_LOCK_BIAS             0x01000000
#define RW_LOCK_BIAS_STR        "0x01000000"

#define __RAW_RW_LOCK_UNLOCKED            { RW_LOCK_BIAS }
#define RW_DEP_MAP_INIT(lockname)

#define __RW_LOCK_UNLOCKED(lockname) \
        (rwlock_t)      {       .raw_lock = __RAW_RW_LOCK_UNLOCKED,     \
                                RW_DEP_MAP_INIT(lockname) }

#define RW_LOCK_UNLOCKED        __RW_LOCK_UNLOCKED(old_style_rw_init)

#define rwlock_init(lock)                                      \
        do { *(lock) = RW_LOCK_UNLOCKED; } while (0)

#define KBUILD_BASENAME "_usr_"

#define LOCK_PREFIX \
                ".section .smp_locks,\"a\"\n"   \
                "  .align 8\n"                  \
                "  .quad 661f\n" /* address */  \
                ".previous\n"                   \
                "661:\n\tlock; "

#define LOCK_SECTION_NAME ".text.lock."KBUILD_BASENAME

#define LOCK_SECTION_START(extra)               \
        ".subsection 1\n\t"                     \
        extra                                   \
        ".ifndef " LOCK_SECTION_NAME "\n\t"     \
        LOCK_SECTION_NAME ":\n\t"               \
        ".endif\n"

#define LOCK_SECTION_END                        \
        ".previous\n\t"

#define __build_read_lock_ptr(rw, helper)   \
        asm volatile(LOCK_PREFIX "subl $1,(%0)\n\t" \
                     "js 2f\n" \
                     "1:\n" \
                    LOCK_SECTION_START("") \
                     "2:\tcall " helper "\n\t" \
                     "jmp 1b\n" \
                    LOCK_SECTION_END \
                     ::"a" (rw) : "memory")

#define __build_read_lock_const(rw, helper)   \
        asm volatile(LOCK_PREFIX "subl $1,%0\n\t" \
                     "js 2f\n" \
                     "1:\n" \
                    LOCK_SECTION_START("") \
                     "2:\tpushq %%rax\n\t" \
                     "leaq %0,%%rax\n\t" \
                     "call " helper "\n\t" \
                     "popq %%rax\n\t" \
                     "jmp 1b\n" \
                    LOCK_SECTION_END \
                     :"=m" (*((volatile int *)rw))::"memory")

#define __build_read_lock(rw, helper)   do { \
                                                if (__builtin_constant_p(rw)) \
                                                        __build_read_lock_const(rw, helper); \
                                                else \
                                                        __build_read_lock_ptr(rw, helper); \
                                        } while (0)

#define __build_write_lock_ptr(rw, helper) \
        asm volatile(LOCK_PREFIX "subl $" RW_LOCK_BIAS_STR ",(%0)\n\t" \
                     "jnz 2f\n" \
                     "1:\n" \
                     LOCK_SECTION_START("") \
                     "2:\tcall " helper "\n\t" \
                     "jmp 1b\n" \
                     LOCK_SECTION_END \
                     ::"a" (rw) : "memory")

#define __build_write_lock_const(rw, helper) \
        asm volatile(LOCK_PREFIX "subl $" RW_LOCK_BIAS_STR ",%0\n\t" \
                     "jnz 2f\n" \
                     "1:\n" \
                    LOCK_SECTION_START("") \
                     "2:\tpushq %%rax\n\t" \
                     "leaq %0,%%rax\n\t" \
                     "call " helper "\n\t" \
                     "popq %%rax\n\t" \
                     "jmp 1b\n" \
                    LOCK_SECTION_END \
                     :"=m" (*((volatile long *)rw))::"memory")

#define __build_write_lock(rw, helper)  do { \
                                                if (__builtin_constant_p(rw)) \
                                                        __build_write_lock_const(rw, helper); \
                                                else \
                                                        __build_write_lock_ptr(rw, helper); \
                                        } while (0)

static inline void __raw_read_lock(raw_rwlock_t *rw)
{
        __build_read_lock(rw, "__read_lock_failed");
}

static inline void __raw_write_lock(raw_rwlock_t *rw)
{
        __build_write_lock(rw, "__write_lock_failed");
}

static inline void __raw_read_unlock(raw_rwlock_t *rw)
{
        asm volatile(LOCK_PREFIX " ; incl %0" :"=m" (rw->lock) : : "memory");
}

static inline void __raw_write_unlock(raw_rwlock_t *rw)
{
        asm volatile(LOCK_PREFIX " ; addl $" RW_LOCK_BIAS_STR ",%0"
                                : "=m" (rw->lock) : : "memory");
}

static inline int __raw_write_trylock(raw_rwlock_t *lock)
{
        atomic_t *count = (atomic_t *)lock;
        if (atomic_sub_and_test(RW_LOCK_BIAS, count))
                return 1;
        atomic_add(RW_LOCK_BIAS, count);
        return 0;
}

#define fastcall
#define __lockfunc fastcall __attribute__((section(".spinlock.text")))
extern void __lockfunc _read_lock(rwlock_t *lock);
extern void __lockfunc _write_lock(rwlock_t *lock);
extern int __lockfunc _write_trylock(rwlock_t *lock);

#endif /* __UTAF_RWLOCK_H__ */
