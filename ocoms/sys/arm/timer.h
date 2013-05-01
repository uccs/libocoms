/*
 * Copyright (c) 2008      The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OCOMS_SYS_ARCH_TIMER_H
#define OCOMS_SYS_ARCH_TIMER_H 1

#include <sys/times.h>

typedef uint64_t ocoms_timer_t;

static inline ocoms_timer_t
ocoms_sys_timer_get_cycles(void)
{
    ocoms_timer_t ret;
    struct tms accurate_clock;

    times(&accurate_clock);
    ret = accurate_clock.tms_utime + accurate_clock.tms_stime;

    return ret;
}

#define OCOMS_HAVE_SYS_TIMER_GET_CYCLES 1

#endif /* ! OCOMS_SYS_ARCH_TIMER_H */
