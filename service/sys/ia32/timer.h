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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef CCS_SYS_ARCH_TIMER_H
#define CCS_SYS_ARCH_TIMER_H 1


typedef uint64_t ccs_timer_t;


#if CCS_GCC_INLINE_ASSEMBLY

static inline ccs_timer_t
ccs_sys_timer_get_cycles(void)
{
    ccs_timer_t ret;

    __asm__ __volatile__("rdtsc" : "=A"(ret));

    return ret;
}

#define CCS_HAVE_SYS_TIMER_GET_CYCLES 1

#else

ccs_timer_t ccs_sys_timer_get_cycles(void);

#define CCS_HAVE_SYS_TIMER_GET_CYCLES 1

#endif /* CCS_GCC_INLINE_ASSEMBLY */

#endif /* ! CCS_SYS_ARCH_TIMER_H */