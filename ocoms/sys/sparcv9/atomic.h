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
 * On sparc v9, use casa and casxa (compare and swap) instructions.
 */

#define ASI_P "0x80"

#if OCOMS_WANT_SMP_LOCKS
#define MEMBAR(type) __asm__  __volatile__ ("membar " type : : : "memory")
#else
#define MEMBAR(type)
#endif


/**********************************************************************
 *
 * Define constants for Sparc v9 (Ultra Sparc)
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
    MEMBAR("#LoadLoad | #LoadStore | #StoreStore | #StoreLoad");
}


static inline void ocoms_atomic_rmb(void)
{
    MEMBAR("#LoadLoad");
}


static inline void ocoms_atomic_wmb(void)
{
    MEMBAR("#StoreStore");
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
   /* casa [reg(rs1)] %asi, reg(rs2), reg(rd)
    *
    * if (*(reg(rs1)) == reg(rs1) )
    *    swap reg(rd), *(reg(rs1))
    * else
    *    reg(rd) = *(reg(rs1))
    */

   int32_t ret = newval;

   __asm__ __volatile__("casa [%1] " ASI_P ", %2, %0"
                      : "+r" (ret)
                      : "r" (addr), "r" (oldval));
   return (ret == oldval);
}


static inline int ocoms_atomic_cmpset_acq_32( volatile int32_t *addr,
                                             int32_t oldval, int32_t newval)
{
   int rc;

   rc = ocoms_atomic_cmpset_32(addr, oldval, newval);
   ocoms_atomic_rmb();

   return rc;
}


static inline int ocoms_atomic_cmpset_rel_32( volatile int32_t *addr,
                                             int32_t oldval, int32_t newval)
{
   ocoms_atomic_wmb();
   return ocoms_atomic_cmpset_32(addr, oldval, newval);
}


#if OCOMS_ASSEMBLY_ARCH == OCOMS_SPARCV9_64

static inline int ocoms_atomic_cmpset_64( volatile int64_t *addr,
                                         int64_t oldval, int64_t newval)
{
    /* casa [reg(rs1)] %asi, reg(rs2), reg(rd)
     *
     * if (*(reg(rs1)) == reg(rs1) )
     *    swap reg(rd), *(reg(rs1))
     * else
     *    reg(rd) = *(reg(rs1))
     */
   int64_t ret = newval;

   __asm__ __volatile__("casxa [%1] " ASI_P ", %2, %0"
                      : "+r" (ret)
                      : "r" (addr), "r" (oldval));
   return (ret == oldval);
}

#else /* OCOMS_ASSEMBLY_ARCH == OCOMS_SPARCV9_64 */

static inline int ocoms_atomic_cmpset_64( volatile int64_t *addr,
                                         int64_t oldval, int64_t newval)
{
    /* casa [reg(rs1)] %asi, reg(rs2), reg(rd)
     *
     * if (*(reg(rs1)) == reg(rs1) )
     *    swap reg(rd), *(reg(rs1))
     * else
     *    reg(rd) = *(reg(rs1))
     *
     */
    long long ret = newval;

    __asm__ __volatile__(
                       "ldx %0, %%g1               \n\t" /* g1 = ret */
                       "ldx %2, %%g2               \n\t" /* g2 = oldval */
                       "casxa [%1] " ASI_P ", %%g2, %%g1 \n\t"
                       "stx %%g1, %0               \n"
                       : "+m"(ret)
                       : "r"(addr), "m"(oldval)
                       : "%g1", "%g2"
                       );

   return (ret == oldval);
}

#endif /* OCOMS_ASSEMBLY_ARCH == OCOMS_SPARCV9_64 */

static inline int ocoms_atomic_cmpset_acq_64( volatile int64_t *addr,
                                             int64_t oldval, int64_t newval)
{
   int rc;
   
   rc = ocoms_atomic_cmpset_64(addr, oldval, newval);
   ocoms_atomic_rmb();
   
   return rc;
}


static inline int ocoms_atomic_cmpset_rel_64( volatile int64_t *addr,
                                             int64_t oldval, int64_t newval)
{
   ocoms_atomic_wmb();
   return ocoms_atomic_cmpset_64(addr, oldval, newval);
}

#endif /* OCOMS_GCC_INLINE_ASSEMBLY */


#endif /* ! OCOMS_SYS_ARCH_ATOMIC_H */
