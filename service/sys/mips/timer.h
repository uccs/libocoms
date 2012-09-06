/*
 * Copyright (c) 2008      The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef CCS_SYS_ARCH_TIMER_H
#define CCS_SYS_ARCH_TIMER_H 1

#include <sys/times.h>

typedef uint64_t ccs_timer_t;

static inline ccs_timer_t
ccs_sys_timer_get_cycles(void)
{
    ccs_timer_t ret;
    struct tms accurate_clock;

    times(&accurate_clock);
    ret = accurate_clock.tms_utime + accurate_clock.tms_stime;

    return ret;
}

#define CCS_HAVE_SYS_TIMER_GET_CYCLES 1

#endif /* ! CCS_SYS_ARCH_TIMER_H */
