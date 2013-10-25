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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#ifndef OCOMS_CONDITION_SPINLOCK_H
#define OCOMS_CONDITION_SPINLOCK_H

#include "ocoms/platform/ocoms_config.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#if OCOMS_HAVE_POSIX_THREADS
#include <pthread.h>
#elif OCOMS_HAVE_SOLARIS_THREADS
#include <thread.h>
#include <synch.h>
#endif

#include "ocoms/threads/mutex.h"
#if 0
#include "opal/runtime/ocoms_cr.h"
#endif

BEGIN_C_DECLS

/*
 * Combine pthread support w/ polled progress to allow run-time selection
 * of threading vs. non-threading progress.
 */

typedef int (*ocoms_progress_fn_t)(void);

struct ocoms_condition_t {
    ocoms_object_t super;
    volatile int c_waiting;
    volatile int c_signaled;
#if OCOMS_HAVE_POSIX_THREADS
    pthread_cond_t c_cond;
#elif OCOMS_HAVE_SOLARIS_THREADS
    cond_t c_cond;
#endif
    ocoms_progress_fn_t ocoms_progress_fn;
    char *name;
};
typedef struct ocoms_condition_t ocoms_condition_t;

OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_condition_t);


static inline int ocoms_condition_wait(ocoms_condition_t *c, ocoms_mutex_t *m)
{
    int rc = 0;
    c->c_waiting++;

#if OCOMS_ENABLE_DEBUG && !OCOMS_ENABLE_MULTI_THREADS
    if (ocoms_mutex_check_locks && 0 == m->m_lock_debug) {
        ocoms_output(0, "Warning -- mutex not locked in condition_wait");
    }
    m->m_lock_debug--;
#endif

    if (ocoms_using_threads()) {
#if OCOMS_HAVE_POSIX_THREADS && OCOMS_ENABLE_MULTI_THREADS
        rc = pthread_cond_wait(&c->c_cond, &m->m_lock_pthread);
#elif OCOMS_HAVE_SOLARIS_THREADS && OCOMS_ENABLE_MULTI_THREADS
        rc = cond_wait(&c->c_cond, &m->m_lock_solaris);
#else
        if (c->c_signaled) {
            c->c_waiting--;
            ocoms_mutex_unlock(m);
            c->ocoms_progress_fn();
            ocoms_mutex_lock(m);
            return 0;
        }
        while (c->c_signaled == 0) {
            ocoms_mutex_unlock(m);
            c->ocoms_progress_fn();
            ocoms_mutex_lock(m);
        }
#endif
    } else {
        while (c->c_signaled == 0) {
            c->ocoms_progress_fn();
        }
    }

#if OCOMS_ENABLE_DEBUG && !OCOMS_ENABLE_MULTI_THREADS
    m->m_lock_debug++;
#endif

    c->c_signaled--;
    c->c_waiting--;
    return rc;
}

static inline int ocoms_condition_timedwait(ocoms_condition_t *c,
                                           ocoms_mutex_t *m,
                                           const struct timespec *abstime)
{
    struct timeval tv;
    struct timeval absolute;
    int rc = 0;

#if OCOMS_ENABLE_DEBUG && !OCOMS_ENABLE_MULTI_THREADS
    if (ocoms_mutex_check_locks && 0 == m->m_lock_debug) {                                         \
        ocoms_output(0, "Warning -- mutex not locked in condition_wait"); \
    }                                                                   \
    m->m_lock_debug--;
#endif

    c->c_waiting++;
    if (ocoms_using_threads()) {
#if OCOMS_HAVE_POSIX_THREADS && OCOMS_ENABLE_MULTI_THREADS
        rc = pthread_cond_timedwait(&c->c_cond, &m->m_lock_pthread, abstime);
#elif OCOMS_HAVE_SOLARIS_THREADS && OCOMS_ENABLE_MULTI_THREADS
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
                ocoms_mutex_unlock(m);
                c->ocoms_progress_fn();
                gettimeofday(&tv,NULL);
                ocoms_mutex_lock(m);
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
                c->ocoms_progress_fn();
                gettimeofday(&tv,NULL);
                } while (c->c_signaled == 0 &&  
                         (tv.tv_sec <= absolute.tv_sec ||
                          (tv.tv_sec == absolute.tv_sec && tv.tv_usec < absolute.tv_usec)));
        }
    }

#if OCOMS_ENABLE_DEBUG && !OCOMS_ENABLE_MULTI_THREADS
    m->m_lock_debug++;
#endif

    if (c->c_signaled != 0) c->c_signaled--;
    c->c_waiting--;
    return rc;
}

static inline int ocoms_condition_signal(ocoms_condition_t *c)
{
    if (c->c_waiting) {
        c->c_signaled++;
#if OCOMS_HAVE_POSIX_THREADS && OCOMS_ENABLE_MULTI_THREADS
        if(ocoms_using_threads()) {
            pthread_cond_signal(&c->c_cond);
        }
#elif OCOMS_HAVE_SOLARIS_THREADS && OCOMS_ENABLE_MULTI_THREADS
        if(ocoms_using_threads()) {
            cond_signal(&c->c_cond);
        }
#endif
    }
    return 0;
}

static inline int ocoms_condition_broadcast(ocoms_condition_t *c)
{
    c->c_signaled = c->c_waiting;
#if OCOMS_HAVE_POSIX_THREADS && OCOMS_ENABLE_MULTI_THREADS
    if (ocoms_using_threads()) {
        if( 1 == c->c_waiting ) {
            pthread_cond_signal(&c->c_cond);
        } else {
            pthread_cond_broadcast(&c->c_cond);
        }
    }
#elif OCOMS_HAVE_SOLARIS_THREADS && OCOMS_ENABLE_MULTI_THREADS
    if (ocoms_using_threads()) {
        cond_broadcast(&c->c_cond);
    }
#endif
    return 0;
}

END_C_DECLS

#endif

