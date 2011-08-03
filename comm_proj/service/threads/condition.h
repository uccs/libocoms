/* 
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#ifndef CCS_CONDITION_SPINLOCK_H
#define CCS_CONDITION_SPINLOCK_H

#include "ccs_config.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#if CCS_HAVE_POSIX_THREADS
#include <pthread.h>
#elif CCS_HAVE_SOLARIS_THREADS
#include <thread.h>
#include <synch.h>
#endif

#include "service/threads/mutex.h"
#if 0
#include "opal/runtime/ccs_progress.h"
#endif

#if 0
#include "opal/runtime/ccs_cr.h"
#endif

BEGIN_C_DECLS

/*
 * Combine pthread support w/ polled progress to allow run-time selection
 * of threading vs. non-threading progress.
 */

struct service_condition_t {
    service_object_t super;
    volatile int c_waiting;
    volatile int c_signaled;
#if CCS_HAVE_POSIX_THREADS
    pthread_cond_t c_cond;
#elif CCS_HAVE_SOLARIS_THREADS
    cond_t c_cond;
#endif
    char *name;
};
typedef struct service_condition_t service_condition_t;

CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_condition_t);


static inline int service_condition_wait(service_condition_t *c, service_mutex_t *m)
{
    int rc = 0;
    c->c_waiting++;

#if CCS_ENABLE_DEBUG && !CCS_ENABLE_MULTI_THREADS
    if (service_mutex_check_locks && 0 == m->m_lock_debug) {                                         \
        service_output(0, "Warning -- mutex not locked in condition_wait"); \
    }                                                                   \
    m->m_lock_debug--;
#endif

    if (ccs_using_threads()) {
#if CCS_HAVE_POSIX_THREADS && CCS_ENABLE_MULTI_THREADS
        rc = pthread_cond_wait(&c->c_cond, &m->m_lock_pthread);
#elif CCS_HAVE_SOLARIS_THREADS && CCS_ENABLE_MULTI_THREADS
        rc = cond_wait(&c->c_cond, &m->m_lock_solaris);
#else
        if (c->c_signaled) {
            c->c_waiting--;
            service_mutex_unlock(m);
            ccs_progress();
            CCS_CR_TEST_CHECKPOINT_READY_STALL();
            service_mutex_lock(m);
            return 0;
        }
        while (c->c_signaled == 0) {
            service_mutex_unlock(m);
            ccs_progress();
            CCS_CR_TEST_CHECKPOINT_READY_STALL();
            service_mutex_lock(m);
        }
#endif
    } else {
        while (c->c_signaled == 0) {
            ccs_progress();
            CCS_CR_TEST_CHECKPOINT_READY_STALL();
        }
    }

#if CCS_ENABLE_DEBUG && !CCS_ENABLE_MULTI_THREADS
    m->m_lock_debug++;
#endif

    c->c_signaled--;
    c->c_waiting--;
    return rc;
}

static inline int service_condition_timedwait(service_condition_t *c,
                                           service_mutex_t *m,
                                           const struct timespec *abstime)
{
    struct timeval tv;
    struct timeval absolute;
    int rc = 0;

#if CCS_ENABLE_DEBUG && !CCS_ENABLE_MULTI_THREADS
    if (service_mutex_check_locks && 0 == m->m_lock_debug) {                                         \
        service_output(0, "Warning -- mutex not locked in condition_wait"); \
    }                                                                   \
    m->m_lock_debug--;
#endif

    c->c_waiting++;
    if (ccs_using_threads()) {
#if CCS_HAVE_POSIX_THREADS && CCS_ENABLE_MULTI_THREADS
        rc = pthread_cond_timedwait(&c->c_cond, &m->m_lock_pthread, abstime);
#elif CCS_HAVE_SOLARIS_THREADS && CCS_ENABLE_MULTI_THREADS
        /* deal with const-ness */
        timestruc_t to;
        to.tv_sec = abstime->tv_sec;
        to.tv_nsec = abstime->tv_nsec;
        rc = cond_timedwait(&c->c_cond, &m->m_lock_solaris, &to);
#else
        absolute.tv_sec = abstime->tv_sec;
        absolute.tv_usec = abstime->tv_nsec * 1000;
        gettimeofday(&tv,NULL);
        if (c->c_signaled == 0) {
            do {
                service_mutex_unlock(m);
                ccs_progress();
                gettimeofday(&tv,NULL);
                service_mutex_lock(m);
                } while (c->c_signaled == 0 &&  
                         (tv.tv_sec <= absolute.tv_sec ||
                          (tv.tv_sec == absolute.tv_sec && tv.tv_usec < absolute.tv_usec)));
        }
#endif
    } else {
        absolute.tv_sec = abstime->tv_sec;
        absolute.tv_usec = abstime->tv_nsec * 1000;
        gettimeofday(&tv,NULL);
        if (c->c_signaled == 0) {
            do {
                ccs_progress();
                gettimeofday(&tv,NULL);
                } while (c->c_signaled == 0 &&  
                         (tv.tv_sec <= absolute.tv_sec ||
                          (tv.tv_sec == absolute.tv_sec && tv.tv_usec < absolute.tv_usec)));
        }
    }

#if CCS_ENABLE_DEBUG && !CCS_ENABLE_MULTI_THREADS
    m->m_lock_debug++;
#endif

    if (c->c_signaled != 0) c->c_signaled--;
    c->c_waiting--;
    return rc;
}

static inline int service_condition_signal(service_condition_t *c)
{
    if (c->c_waiting) {
        c->c_signaled++;
#if CCS_HAVE_POSIX_THREADS && CCS_ENABLE_MULTI_THREADS
        if(ccs_using_threads()) {
            pthread_cond_signal(&c->c_cond);
        }
#elif CCS_HAVE_SOLARIS_THREADS && CCS_ENABLE_MULTI_THREADS
        if(ccs_using_threads()) {
            cond_signal(&c->c_cond);
        }
#endif
    }
    return 0;
}

static inline int service_condition_broadcast(service_condition_t *c)
{
    c->c_signaled = c->c_waiting;
#if CCS_HAVE_POSIX_THREADS && CCS_ENABLE_MULTI_THREADS
    if (ccs_using_threads()) {
        if( 1 == c->c_waiting ) {
            pthread_cond_signal(&c->c_cond);
        } else {
            pthread_cond_broadcast(&c->c_cond);
        }
    }
#elif CCS_HAVE_SOLARIS_THREADS && CCS_ENABLE_MULTI_THREADS
    if (ccs_using_threads()) {
        cond_broadcast(&c->c_cond);
    }
#endif
    return 0;
}

END_C_DECLS

#endif

