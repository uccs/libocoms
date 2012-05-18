/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 *
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/**
 * @file
 *
 * Progress engine for Open MPI
 */

#ifndef CCS_RUNTIME_CCS_PROGRESS_H
#define CCS_RUNTIME_CCS_PROGRESS_H

BEGIN_C_DECLS

#include "ccs_config.h"
#include "service/threads/mutex.h"

/**
 * Initialize the progress engine
 *
 * Initialize the progress engine, including constructing the 
 * proper locks and allocating space for the progress registration
 * functions.  At this point, any function in the progress engine
 * interface may be called.
 */
CCS_DECLSPEC int ccs_progress_init(void);


/** 
 * Shut down the progress engine
 *
 * Shut down the progress engine.  This includes deregistering all
 * registered callbacks and freeing all resources.  After finalize
 * returns, no calls into the progress interface are allowed.
 */
CCS_DECLSPEC int ccs_progress_finalize(void);


/**
 * Progress all pending events
 *
 * Progress all pending events.  All registered event handlers will be
 * called every call into ccs_progress().  The event library will be
 * called if ccs_progress_event_users is greater than 0 (adjustments
 * can be made by calling ccs_progress_event_users_add() and
 * ccs_progress_event_users_delete()) or the time since the last call
 * into the event library is greater than the progress tick rate (by
 * default, 10ms).
 */
CCS_DECLSPEC void ccs_progress(void);


/**
 * Control how the event library is called
 *
 * Adjust the flags argument used to call ccs_event_loop() from
 * ccs_progress().  The default argument is CCS_EVLOOP_ONELOOP,
 * meaning that the call to ccs_event_loop() will block pending
 * events, but may block for a period of time.
 *
 * @param flags     One of the valid vlags argument to 
 *                  ccs_event_loop().
 * @return          Previous value of flags used to call
 *                  ccs_event_loop().
 */
CCS_DECLSPEC int ccs_progress_set_event_flag(int flags);


/**
 * Increase the number of users of the event library
 *
 * Increase the number of users of the event library.  This count is
 * used by ccs_progress to determine if ccs_event_loop() should be
 * called every call to ccs_progress() or only after a time has
 * elapsed since the last call (by default, 10ms).  The count defaults
 * to 0, meaning that ccs_progress_event_users_increment() must be
 * called at least once for the event loop to be called on every entry
 * to ccs_progress().
 *
 */
CCS_DECLSPEC void ccs_progress_event_users_increment(void);


/**
 * Decrease the number of users of the event library
 *
 * Decrease the number of users of the event library.  This count is
 * used by ccs_progress to determine if ccs_event_loop() should be
 * called every call to ccs_progress() or only after a time has
 * elapsed since the last call (by default, 10ms).
 */
CCS_DECLSPEC void ccs_progress_event_users_decrement(void);


/**
 * Set whether ccs_progress() should yield when idle
 *
 * Set whether ccs_progress() should yield the processor (either by
 * sched_yield() or SwitchToThread()) if no events were progressed
 * during the progress loop.  The return value of the callback
 * functions is used to determine whether or not yielding is required.
 * By default, the event loop will yield when the progress function is
 * idle.
 *
 * @param   yieldopt  Whether to yield when idle.
 * @return         Previous value of the yield_when_idle option.
 */
CCS_DECLSPEC bool ccs_progress_set_yield_when_idle(bool yieldopt);


/**
 * Set time between calls into the event library
 *
 * Set time between calls into the event library when there are no
 * users of the event library (set by
 * ccs_progress_event_users_increment() and
 * ccs_progress_event_users_decrement()).
 *
 * @param   polltime  Time (in microseconds) between calls to the event
 *                    library
 */
CCS_DECLSPEC void ccs_progress_set_event_poll_rate(int microseconds);


/**
 * Progress callback function typedef
 * 
 * Prototype for the a progress function callback.  Progress function
 * callbacks can be registered with ccs_progress_register() and
 * deregistered with ccs_progress_deregister().  It should be noted
 * that either registering or deregistering a function callback is an
 * extraordinarily expensive operation and should not be used for
 * potentially short callback lifetimes.
 *
 * @return         Number of events progressed during the callback
 */
typedef int (*ccs_progress_callback_t)(void);


/**
 * Register an event to be progressed
 *
 * Register an event to be progressed during calls to ccs_progress().
 * Please read the note in ccs_progress_callback_t.
 */
CCS_DECLSPEC int ccs_progress_register(ccs_progress_callback_t cb);


/**
 * Deregister previously registered event
 *
 * Deregister an event to be progressed during calls to ccs_progress().
 * Please read the note in ccs_progress_callback_t.
 */
CCS_DECLSPEC int ccs_progress_unregister(ccs_progress_callback_t cb);


CCS_DECLSPEC extern volatile int32_t ccs_progress_thread_count;
CCS_DECLSPEC extern int ccs_progress_spin_count;

static inline bool ccs_progress_threads(void) 
{ 
    return (ccs_progress_thread_count > 0); 
}


/**
 * Progress until flag is true or poll iterations completed
 */
static inline bool ccs_progress_spin(volatile bool* complete)
{
    int32_t c;
    SERVICE_THREAD_ADD32(&ccs_progress_thread_count,1);
    for (c = 0; c < ccs_progress_spin_count; c++) {
        if (true == *complete) {
             SERVICE_THREAD_ADD32(&ccs_progress_thread_count,-1);
             return true;
        }
        ccs_progress();
    }
    SERVICE_THREAD_ADD32(&ccs_progress_thread_count,-1);
    return false;
}


/**
 * \internal
 * Don't use this variable; use the ccs_progress_recursion_depth()
 * function.
 */
CCS_DECLSPEC extern 
#if CCS_ENABLE_MULTI_THREADS
volatile 
#endif
uint32_t ccs_progress_recursion_depth_counter;

/**
 * Return the current level of recursion -- 0 means that we are not
 * under an ccs_progress() call at all.  1 means that you're in the
 * top-level ccs_progress() function (i.e., not deep in recursion).
 * Higher values mean that you're that many levels deep in recursion.
 */
static inline uint32_t ccs_progress_recursion_depth(void) 
{
    return ccs_progress_recursion_depth_counter;
}
/**
 * Return the current progress event mode.
 * The flag controls blocking/non-blcoking modes of event handling
 */
int ccs_progress_get_flag(void);

END_C_DECLS

#endif

