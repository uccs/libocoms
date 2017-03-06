/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2010      IBM Corporation.  All rights reserved.
 * Copyright (c) 2010      ARM ltd.  All rights reserved.
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

#if OCOMS_WANT_SMP_LOCKS

#define MB()  __asm__ __volatile__ ("dmb" : : : "memory")
#define RMB() __asm__ __volatile__ ("dmb" : : : "memory")
#define WMB() __asm__ __volatile__ ("dmb" : : : "memory")

#else

#define MB()
#define RMB()
#define WMB()

#endif


/**********************************************************************
 *
 * Define constants for ARMv7
 *
 *********************************************************************/
#define OCOMS_HAVE_ATOMIC_MEM_BARRIER 1

#define OCOMS_HAVE_ATOMIC_CMPSET_32 1

#define OCOMS_HAVE_ATOMIC_CMPSET_64 1

#define OCOMS_HAVE_ATOMIC_MATH_32 1
#define OCOMS_HAVE_ATOMIC_ADD_32 1
#define OCOMS_HAVE_ATOMIC_SUB_32 1


/**********************************************************************
 *
 * Memory Barriers
 *
 *********************************************************************/
#if OCOMS_GCC_INLINE_ASSEMBLY

static inline
void ocoms_atomic_mb(void)
{
    MB();
}


static inline
void ocoms_atomic_rmb(void)
{
    RMB();
}


static inline
void ocoms_atomic_wmb(void)
{
    WMB();
}

static inline
void ocoms_atomic_isync()
{
}


/**********************************************************************
 *
 * Atomic math operations
 *
 *********************************************************************/

static inline int ocoms_atomic_cmpset_32(volatile int32_t *addr,
                                        int32_t oldval, int32_t newval)
{
  int32_t ret, tmp;

   __asm__ __volatile__ (
                         "1:  ldrex   %0, [%2]        \n"
                         "    cmp     %0, %3          \n"
                         "    bne     2f              \n"
                         "    strex   %1, %4, [%2]    \n"
                         "    cmp     %1, #0          \n"
                         "    bne     1b              \n"
                         "2:                          \n"

                         : "=&r" (ret), "=&r" (tmp)
                         : "r" (addr), "r" (oldval), "r" (newval)
                         : "cc", "memory");

   return (ret == oldval);
}

/* these two functions aren't inlined in the non-gcc case because then
   there would be two function calls (since neither cmpset_32 nor
   atomic_?mb can be inlined).  Instead, we "inline" them by hand in
   the assembly, meaning there is one function call overhead instead
   of two */
static inline int ocoms_atomic_cmpset_acq_32(volatile int32_t *addr,
                                            int32_t oldval, int32_t newval)
{
    int rc;

    rc = ocoms_atomic_cmpset_32(addr, oldval, newval);
    ocoms_atomic_rmb();

    return rc;
}


static inline int ocoms_atomic_cmpset_rel_32(volatile int32_t *addr,
                                            int32_t oldval, int32_t newval)
{
    ocoms_atomic_wmb();
    return ocoms_atomic_cmpset_32(addr, oldval, newval);
}


static inline int ocoms_atomic_cmpset_64(volatile int64_t *addr,
                                        int64_t oldval, int64_t newval)
{
  int64_t ret;
  int tmp;


   __asm__ __volatile__ (
                         "1:  ldrexd  %0, %H0, [%2]           \n"
                         "    cmp     %0, %3                  \n"
                         "    cmpeq   %H0, %H3                \n"
                         "    bne     2f                      \n"
                         "    strexd  %1, %4, %H4, [%2]       \n"
                         "    cmp     %1, #0                  \n"
                         "    bne     1b                      \n"
                         "2:                                    \n"

                         : "=&r" (ret), "=&r" (tmp)
                         : "r" (addr), "r" (oldval), "r" (newval)
                         : "cc", "memory");

   return (ret == oldval);
}

/* these two functions aren't inlined in the non-gcc case because then
   there would be two function calls (since neither cmpset_64 nor
   atomic_?mb can be inlined).  Instead, we "inline" them by hand in
   the assembly, meaning there is one function call overhead instead
   of two */
static inline int ocoms_atomic_cmpset_acq_64(volatile int64_t *addr,
                                            int64_t oldval, int64_t newval)
{
    int rc;

    rc = ocoms_atomic_cmpset_64(addr, oldval, newval);
    ocoms_atomic_rmb();

    return rc;
}


static inline int ocoms_atomic_cmpset_rel_64(volatile int64_t *addr,
                                            int64_t oldval, int64_t newval)
{
    ocoms_atomic_wmb();
    return ocoms_atomic_cmpset_64(addr, oldval, newval);
}


static inline int32_t ocoms_atomic_add_32(volatile int32_t* v, int inc)
{
   int32_t t;
   int tmp;

   __asm__ __volatile__(
                         "1:  ldrex   %0, [%2]        \n"
                         "    add     %0, %0, %3      \n"
                         "    strex   %1, %0, [%2]    \n"
                         "    cmp     %1, #0          \n"
                         "    bne     1b              \n"

                         : "=&r" (t), "=&r" (tmp)
                         : "r" (v), "r" (inc)
                         : "cc", "memory");


   return t;
}


static inline int32_t ocoms_atomic_sub_32(volatile int32_t* v, int dec)
{
   int32_t t;
   int tmp;

   __asm__ __volatile__(
                         "1:  ldrex   %0, [%2]        \n"
                         "    sub     %0, %0, %3      \n"
                         "    strex   %1, %0, [%2]    \n"
                         "    cmp     %1, #0          \n"
                         "    bne     1b              \n"

                         : "=&r" (t), "=&r" (tmp)
                         : "r" (v), "r" (dec)
                         : "cc", "memory");

   return t;
}

#else /* OCOMS_ASM_ARM_VERSION <=5 or no GCC inline assembly */

#define OCOMS_HAVE_ATOMIC_CMPSET_32 1
#define __kuser_cmpxchg (*((int (*)(int, int, volatile int*))(0xffff0fc0)))
static inline int ocoms_atomic_cmpset_32(volatile int32_t *addr,
                                        int32_t oldval, int32_t newval)
{
    return !(__kuser_cmpxchg(oldval, newval, addr));
}

static inline int ocoms_atomic_cmpset_acq_32(volatile int32_t *addr,
                                            int32_t oldval, int32_t newval)
{
    /* kernel function includes all necessary memory barriers */
    return ocoms_atomic_cmpset_32(addr, oldval, newval);
}

static inline int ocoms_atomic_cmpset_rel_32(volatile int32_t *addr,
                                            int32_t oldval, int32_t newval)
{
    /* kernel function includes all necessary memory barriers */
    return ocoms_atomic_cmpset_32(addr, oldval, newval);
}

#endif

#endif /* ! OCOMS_SYS_ARCH_ATOMIC_H */
