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

#ifndef OCOMS_SYS_ARCH_TIMER_H
#define OCOMS_SYS_ARCH_TIMER_H 1


typedef uint64_t ocoms_timer_t;


#if OCOMS_GCC_INLINE_ASSEMBLY

static inline ocoms_timer_t
ocoms_sys_timer_get_cycles(void)
{
    ocoms_timer_t ret;

    __asm__ __volatile__("rdtsc" : "=A"(ret));

    return ret;
}

#define OCOMS_HAVE_SYS_TIMER_GET_CYCLES 1

#else

ocoms_timer_t ocoms_sys_timer_get_cycles(void);

#define OCOMS_HAVE_SYS_TIMER_GET_CYCLES 1

#endif /* OCOMS_GCC_INLINE_ASSEMBLY */

#endif /* ! OCOMS_SYS_ARCH_TIMER_H */
