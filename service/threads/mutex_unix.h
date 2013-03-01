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
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef  CCS_MUTEX_UNIX_H
#define  CCS_MUTEX_UNIX_H 1

/**
 * @file:
 *
 * Mutual exclusion functions: Unix implementation.
 *
 * Functions for locking of critical sections.
 *
 * On unix, use pthreads or our own atomic operations as
 * available.
 */

#include "ccs_config.h"

#if CCS_HAVE_POSIX_THREADS
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include <errno.h>
#include <stdio.h>
#elif CCS_HAVE_SOLARIS_THREADS
#include <thread.h>
#include <synch.h>
#endif

#include "service/util/service_object.h"
#include "service/include/service/sys/atomic.h"

BEGIN_C_DECLS

struct service_mutex_t {
    service_object_t super;

#if CCS_HAVE_POSIX_THREADS
    pthread_mutex_t m_lock_pthread;
#elif CCS_HAVE_SOLARIS_THREADS
    mutex_t m_lock_solaris;
#endif

#if !CCS_ENABLE_MULTI_THREADS && CCS_ENABLE_DEBUG
    int m_lock_debug;
    const char *m_lock_file;
    int m_lock_line;
#endif

    service_atomic_lock_t m_lock_atomic;
};
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_mutex_t);

/************************************************************************
 *
 * mutex operations (non-atomic versions)
 *
 ************************************************************************/

#if CCS_HAVE_POSIX_THREADS

/************************************************************************
 * POSIX threads
 ************************************************************************/

static inline int service_mutex_trylock(service_mutex_t *m)
{
#if CCS_ENABLE_DEBUG
    int ret = pthread_mutex_trylock(&m->m_lock_pthread);
    if (ret == EDEADLK) {
        errno = ret;
        perror("service_mutex_trylock()");
        abort();
    }
    return ret;
#else
    return pthread_mutex_trylock(&m->m_lock_pthread);
#endif
}

static inline void service_mutex_lock(service_mutex_t *m)
{
#if CCS_ENABLE_DEBUG
    int ret = pthread_mutex_lock(&m->m_lock_pthread);
    if (ret == EDEADLK) {
        errno = ret;
        perror("service_mutex_lock()");
        abort();
    }
#else
    pthread_mutex_lock(&m->m_lock_pthread);
#endif
}

static inline void service_mutex_unlock(service_mutex_t *m)
{
#if CCS_ENABLE_DEBUG
    int ret = pthread_mutex_unlock(&m->m_lock_pthread);
    if (ret == EPERM) {
        errno = ret;
        perror("service_mutex_unlock");
        abort();
    }
#else
    pthread_mutex_unlock(&m->m_lock_pthread);
#endif
}

#elif CCS_HAVE_SOLARIS_THREADS

/************************************************************************
 * Solaris threads
 ************************************************************************/


static inline int service_mutex_trylock(service_mutex_t *m)
{
    return mutex_trylock(&m->m_lock_solaris);
}

static inline void service_mutex_lock(service_mutex_t *m)
{
    mutex_lock(&m->m_lock_solaris);
}

static inline void service_mutex_unlock(service_mutex_t *m)
{
    mutex_unlock(&m->m_lock_solaris);
}

#elif CCS_HAVE_ATOMIC_SPINLOCKS

/************************************************************************
 * Spin Locks
 ************************************************************************/

static inline int service_mutex_trylock(service_mutex_t *m)
{
    return service_atomic_trylock(&m->m_lock_atomic);
}

static inline void service_mutex_lock(service_mutex_t *m)
{
    service_atomic_lock(&m->m_lock_atomic);
}

static inline void service_mutex_unlock(service_mutex_t *m)
{
    service_atomic_unlock(&m->m_lock_atomic);
}

#else

#error No mutex definition

#endif


/************************************************************************
 *
 * mutex operations (atomic versions)
 *
 ************************************************************************/

#if CCS_HAVE_ATOMIC_SPINLOCKS

/************************************************************************
 * Spin Locks
 ************************************************************************/

static inline int service_mutex_atomic_trylock(service_mutex_t *m)
{
    return service_atomic_trylock(&m->m_lock_atomic);
}

static inline void service_mutex_atomic_lock(service_mutex_t *m)
{
    service_atomic_lock(&m->m_lock_atomic);
}

static inline void service_mutex_atomic_unlock(service_mutex_t *m)
{
    service_atomic_unlock(&m->m_lock_atomic);
}

#else

/************************************************************************
 * Standard locking
 ************************************************************************/

static inline int service_mutex_atomic_trylock(service_mutex_t *m)
{
    return service_mutex_trylock(m);
}

static inline void service_mutex_atomic_lock(service_mutex_t *m)
{
    service_mutex_lock(m);
}

static inline void service_mutex_atomic_unlock(service_mutex_t *m)
{
    service_mutex_unlock(m);
}

#endif

END_C_DECLS

#endif                          /* CCS_MUTEX_UNIX_H */
