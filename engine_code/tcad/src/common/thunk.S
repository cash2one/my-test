#define RW_LOCK_BIAS             0x01000000

#ifndef __ALIGN
#define __ALIGN         .align 4,0x90
#define __ALIGN_STR     ".align 4,0x90"
#endif

#define ALIGN __ALIGN

#ifndef ENTRY
#define ENTRY(name) \
  .globl name; \
  ALIGN; \
  name:
#endif

/* Support for read/write spinlocks. */
        .text
/* rax: pointer to rwlock_t */
ENTRY(__write_lock_failed)
        lock
        addl $RW_LOCK_BIAS,(%rax)
1:      rep
        nop
        cmpl $RW_LOCK_BIAS,(%rax)
        jne 1b
        lock
        subl $RW_LOCK_BIAS,(%rax)
        jnz  __write_lock_failed
        ret

/* rax: pointer to rwlock_t */
ENTRY(__read_lock_failed)
        lock
        incl (%rax)
1:      rep
        nop
        cmpl $1,(%rax)
        js 1b
        lock
        decl (%rax)
        js __read_lock_failed
        ret
