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

/**
 * @file
 *
 * High resolution systimer / cycle counter
 *
 * High resolution systimer / cycle counter interface.  This interface is
 * intended to hide the system-dependent nature of systimers that provide
 * higher resolution and lower calling cost than gettimeofday().
 * Unlike most component interfaces, there should only ever be one
 * component available to be built on a particular platform.
 * Therefore, a number of #defines are available to determine how well
 * the platform supports high resolution systimers:
 *
 * <UL>
 *  <LI><PRE>CCS_TIMER_CYCLE_NATIVE</PRE> Whether
 *    ccs_systimer_base_get_cycle() is implemented directly or computed
 *    from some other data (such as a high res systimer)</LI>
 *  <LI><PRE>CCS_TIMER_CYCLE_SUPPORTED</PRE> Whether 
 *    ccs_systimer_base_get_cycle() is supported on the current
 *    platform.</LI>
 *  <LI><PRE>CCS_TIMER_USEC_SUPPORTED</PRE> Whether
 *    ccs_systimer_base_get_usec() is supported on the current
 *    platform or implemented on top of gettimeofday(), which
 *    may be unsuitable for some uses.
 * </UL>
 *
 * The cycle count may not be the cycle count of the CPU itself, if
 * there is another sufficiently close counter with better behavior
 * characteristics (like the Time Base counter on many Power/PowerPC
 * platforms).  The function ccs_systimer_base_get_freq() returns the
 * frequency of the cycle counter in use, *NOT* the frequency of the
 * main CPU.
 *
 * Unless otherwise noted, no attempt is made to cope with the the
 * differences in counters on SMP machines.  If your process switches
 * CPUs, your systimer results may change.
 *
 * Build time priorities are allocated as follows:
 *
 * - 0   gettimeofday() wrapper
 * - 10  Assembly systimers with bad frequency search (Linux)
 * - 20  NIC software stack (QSNet, Myrinet?)
 * - 30  Operating systems with native interfaces
 */

#ifndef CCS_MCA_TIMER_TIMER_H
#define CCS_MCA_TIMER_TIMER_H

#include "ccs_config.h"

#include "service/mca/mca.h"
#include "service/mca/base/base.h"


/**
 * Structure for systimer components.
 */
struct ccs_systimer_base_component_2_0_0_t {
    /** MCA base component */
    ccs_mca_base_component_t systimerc_version;
    /** MCA base data */
    ccs_mca_base_component_data_t systimerc_data;
};
/**
 * Convenience typedef
 */
typedef struct ccs_systimer_base_component_2_0_0_t ccs_systimer_base_component_2_0_0_t;

/*
 * Macro for use in components that are of type systimer
 */
#define CCS_TIMER_BASE_VERSION_2_0_0 \
    MCA_BASE_VERSION_2_0_0, \
    "systimer", 2, 0, 0

#endif /* CCS_MCA_TIMER_TIMER_H */
