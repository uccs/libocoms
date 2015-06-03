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
 * On alpha, everything is load-locked, store-conditional...
 */

#if OCOMS_WANT_SMP_LOCKS

#define MB()  __asm__ __volatile__ ("mb");
#define RMB() __asm__ __volatile__ ("mb");
#define WMB() __asm__ __volatile__ ("wmb");

#else

#define MB()
#define RMB()
#define WMB()

#endif


/**********************************************************************
 *
 * Define constants for PowerPC 32
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
    RMB();
}


static inline void ocoms_atomic_wmb(void)
{
    WMB();
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
   int32_t ret;
   
   __asm __volatile__ (
		       "1:  ldl_l %0, %1        \n\t"
		       "cmpeq %0, %2, %0        \n\t"
		       "beq %0, 2f              \n\t"
		       "mov %3, %0              \n\t"
		       "stl_c %0, %1            \n\t"
		       "beq %0, 1b              \n\t"
		       "jmp 3f                  \n"
		       "2:  mov $31, %0         \n"
		       "3:                      \n"
		       : "=&r" (ret), "+m" (*addr)
		       : "r" (oldval), "r" (newval)
		       : "memory");

    return ret;
}


static inline int ocoms_atomic_cmpset_acq_32(volatile int32_t *addr,
                                           int32_t oldval,
                                           int32_t newval)
{
    int rc;

    rc = ocoms_atomic_cmpset_32(addr, oldval, newval);
    ocoms_atomic_rmb();

    return rc;
}


static inline int ocoms_atomic_cmpset_rel_32(volatile int32_t *addr,
                                           int32_t oldval,
                                           int32_t newval)
{
    ocoms_atomic_wmb();
    return ocoms_atomic_cmpset_32(addr, oldval, newval);
}


static inline int ocoms_atomic_cmpset_64( volatile int64_t *addr,
                                         int64_t oldval, int64_t newval)
{
    int32_t ret;

    __asm__ __volatile__ (
			  "1:  ldq_l %0, %1     \n\t"
			  "cmpeq %0, %2, %0     \n\t"
			  "beq %0, 2f           \n\t"
			  "mov %3, %0           \n\t"
			  "stq_c %0, %1         \n\t"
			  "beq %0, 1b           \n\t"
			  "jmp 3f               \n"
			  "2:  mov $31, %0      \n"
			  "3:                   \n"
			  : "=&r" (ret), "+m" (*addr)
			  : "r" (oldval), "r" (newval)
			  : "memory");

    return ret;
}


static inline int ocoms_atomic_cmpset_acq_64(volatile int64_t *addr,
                                           int64_t oldval,
                                           int64_t newval)
{
    int rc;

    rc = ocoms_atomic_cmpset_64(addr, oldval, newval);
    ocoms_atomic_rmb();

    return rc;
}


static inline int ocoms_atomic_cmpset_rel_64(volatile int64_t *addr,
                                           int64_t oldval,
                                           int64_t newval)
{
    ocoms_atomic_wmb();
    return ocoms_atomic_cmpset_64(addr, oldval, newval);
}


#endif /* OCOMS_GCC_INLINE_ASSEMBLY */


#endif /* ! OCOMS_SYS_ARCH_ATOMIC_H */
