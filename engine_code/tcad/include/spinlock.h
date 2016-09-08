#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__


/**
 * The rte_spinlock_t type.
 */
typedef struct {
	volatile int locked; /**< lock status 0 = unlocked, 1 = locked */
} spinlock_t;

/**
 * A static spinlock initializer.
 */
#define SPINLOCK_INITIALIZER { 0 }

/**
 * Initialize the spinlock to an unlocked state.
 *
 * @param sl
 *   A pointer to the spinlock.
 */
static inline void spinlock_init(spinlock_t *sl)
{
	sl->locked = 0;
}



/**
 * Take the spinlock.
 *
 * @param sl
 *   A pointer to the spinlock.
 */
static inline void spinlock_lock(spinlock_t *sl)
{

	int lock_val = 1;
	asm volatile (
			"1:\n"
			"lock; xchg %[locked], %[lv]\n"  /* wdb_lfix */
			"test %[lv], %[lv]\n"
			"jz 3f\n"
			"2:\n"
			"pause\n"
			"cmp $0, %[locked]\n"
			"jnz 2b\n"
			"jmp 1b\n"
			"3:\n"
			: [locked] "=m" (sl->locked), [lv] "=q" (lock_val)
			: "[lv]" (lock_val)
			: "memory");

}


/**
 * Release the spinlock.
 *
 * @param sl
 *   A pointer to the spinlock.
 */
static inline void spinlock_unlock (spinlock_t *sl)
{

	int unlock_val = 0;
	asm volatile (
			"lock; xchg %[locked], %[ulv]\n"  /* wdb_lfix */
			: [locked] "=m" (sl->locked), [ulv] "=q" (unlock_val)
			: "[ulv]" (unlock_val)
			: "memory");

}


/**
 * Try to take the lock.
 *
 * @param sl
 *   A pointer to the spinlock.
 * @return
 *   1 if the lock is successfully taken; 0 otherwise.
 */
static inline int spinlock_trylock (spinlock_t *sl)
{
	int lockval = 1;

	asm volatile (
			"lock; xchg %[locked], %[lockval]"  /* wdb_lfix */
			: [locked] "=m" (sl->locked), [lockval] "=q" (lockval)
			: "[lockval]" (lockval)
			: "memory");

	return (lockval == 0);

}


/**
 * Test if the lock is taken.
 *
 * @param sl
 *   A pointer to the spinlock.
 * @return
 *   1 if the lock is currently taken; 0 otherwise.
 */
static inline int rte_spinlock_is_locked (spinlock_t *sl)
{
	return sl->locked;
}




#endif
