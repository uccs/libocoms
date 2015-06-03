/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2010 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserverd.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#ifndef OCOMS_SYS_ARCH_ATOMIC_H
#define OCOMS_SYS_ARCH_ATOMIC_H 1

/*
 * On amd64, we use cmpxchg.
 */


#if OCOMS_WANT_SMP_LOCKS
#define SMPLOCK "lock; "
#define MB() __asm__ __volatile__("": : :"memory")
#else
#define SMPLOCK
#define MB()
#endif


/**********************************************************************
 *
 * Define constants for AMD64 / x86_64 / EM64T / ...
 *
 *********************************************************************/
#define OCOMS_HAVE_ATOMIC_MEM_BARRIER 1

#define OCOMS_HAVE_ATOMIC_CMPSET_32 1

#define OCOMS_HAVE_ATOMIC_CMPSET_64 1

/**********************************************************************
 *
 * Memory Barriers
 *
 *********************************************************************/
#if OCOMS_GCC_INLINE_ASSEMBLY

static inline void ocoms_atomic_mb(void)
{
    MB();
}


static inline void ocoms_atomic_rmb(void)
{
    MB();
}


static inline void ocoms_atomic_wmb(void)
{
    MB();
}

static inline
void ocoms_atomic_isync()
{
}

#endif /* OCOMS_GCC_INLINE_ASSEMBLY */


/**********************************************************************
 *
 * Atomic math operations
 *
 *********************************************************************/
#if OCOMS_GCC_INLINE_ASSEMBLY

static inline int ocoms_atomic_cmpset_32( volatile int32_t *addr,
                                        int32_t oldval, int32_t newval)
{
   unsigned char ret;
   __asm__ __volatile__ (
                       SMPLOCK "cmpxchgl %3,%2   \n\t"
                               "sete     %0      \n\t"
                       : "=qm" (ret), "+a" (oldval), "+m" (*addr)
                       : "q"(newval)
                       : "memory", "cc");

   return (int)ret;
}

#endif /* OCOMS_GCC_INLINE_ASSEMBLY */

#define ocoms_atomic_cmpset_acq_32 ocoms_atomic_cmpset_32
#define ocoms_atomic_cmpset_rel_32 ocoms_atomic_cmpset_32

#if OCOMS_GCC_INLINE_ASSEMBLY

static inline int ocoms_atomic_cmpset_64( volatile int64_t *addr,
                                         int64_t oldval, int64_t newval)
{
   unsigned char ret;
   __asm__ __volatile__ (
                       SMPLOCK "cmpxchgq %3,%2   \n\t"
                               "sete     %0      \n\t"
                       : "=qm" (ret), "+a" (oldval), "+m" (*((volatile long*)addr))
                       : "q"(newval)
                       : "memory", "cc"
                       );

   return (int)ret;
}

#endif /* OCOMS_GCC_INLINE_ASSEMBLY */

#define ocoms_atomic_cmpset_acq_64 ocoms_atomic_cmpset_64
#define ocoms_atomic_cmpset_rel_64 ocoms_atomic_cmpset_64


#if OCOMS_C_GCC_INLINE_ASSEMBLY

#define OCOMS_HAVE_ATOMIC_SWAP_32 1

static inline int32_t ocoms_atomic_swap_32(volatile int32_t *addr, int32_t newval)
{
    int32_t oldval;

    __asm__ __volatile__("xchg %1, %0" :
             "=r" (oldval), "=m" (*addr) :
             "0" (newval), "m" (*addr) :
             "memory");
            return oldval;
}

#endif /* OCOMS_C_GCC_INLINE_ASSEMBLY */


#if OCOMS_C_GCC_INLINE_ASSEMBLY

#define OCOMS_HAVE_ATOMIC_SWAP_64 1

static inline int64_t ocoms_atomic_swap_64(volatile int64_t *addr, int64_t newval)
{
    int64_t oldval;

    __asm__ __volatile__("xchgq %1, %0" :
                         "=r" (oldval) :
                         "m" (*addr), "0" (newval) :
                         "memory");
    return oldval;
}

#endif /* OCOMS_C_GCC_INLINE_ASSEMBLY */

#if OCOMS_GCC_INLINE_ASSEMBLY

#define OCOMS_HAVE_ATOMIC_MATH_32 1
#define OCOMS_HAVE_ATOMIC_MATH_64 1

#define OCOMS_HAVE_ATOMIC_ADD_32 1

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v.
 */
static inline int32_t ocoms_atomic_add_32(volatile int32_t* v, int i)
{
    int ret = i;
   __asm__ __volatile__(
                        SMPLOCK "xaddl %1,%0"
                        :"=m" (*v), "+r" (ret)
                        :"m" (*v)
                        :"memory", "cc"
                        );
   return (ret+i);
}

#define OCOMS_HAVE_ATOMIC_ADD_64 1

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v.
 */
static inline int64_t ocoms_atomic_add_64(volatile int64_t* v, int64_t i)
{
    int64_t ret = i;
   __asm__ __volatile__(
                        SMPLOCK "xaddq %1,%0"
                        :"=m" (*v), "+r" (ret)
                        :"m" (*v)
                        :"memory", "cc"
                        );
   return (ret+i);
}

#define OCOMS_HAVE_ATOMIC_SUB_32 1

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v.
 */
static inline int32_t ocoms_atomic_sub_32(volatile int32_t* v, int i)
{
    int ret = -i;
   __asm__ __volatile__(
                        SMPLOCK "xaddl %1,%0"
                        :"=m" (*v), "+r" (ret)
                        :"m" (*v)
                        :"memory", "cc"
                        );
   return (ret-i);
}

#define OCOMS_HAVE_ATOMIC_SUB_64 1

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v.
 */
static inline int64_t ocoms_atomic_sub_64(volatile int64_t* v, int64_t i)
{
    int64_t ret = -i;
   __asm__ __volatile__(
                        SMPLOCK "xaddq %1,%0"
                        :"=m" (*v), "+r" (ret)
                        :"m" (*v)
                        :"memory", "cc"
                        );
   return (ret-i);
}

#endif /* OCOMS_GCC_INLINE_ASSEMBLY */

#endif /* ! OCOMS_SYS_ARCH_ATOMIC_H */
