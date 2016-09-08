#include "rwlock.h"

void __lockfunc _read_lock(rwlock_t *lock)
{
//        preempt_disable();
        rwlock_acquire_read(&lock->dep_map, 0, 0, _RET_IP_);
        _raw_read_lock(lock);
}

void __lockfunc _write_lock(rwlock_t *lock)
{
//        preempt_disable();
        rwlock_acquire(&lock->dep_map, 0, 0, _RET_IP_);
        _raw_write_lock(lock);
}

int __lockfunc _write_trylock(rwlock_t *lock)
{
//        preempt_disable();
        if (_raw_write_trylock(lock)) {
                rwlock_acquire(&lock->dep_map, 0, 1, _RET_IP_);
                return 1;
        }

//        preempt_enable();
        return 0;
}
