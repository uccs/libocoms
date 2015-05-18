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
 * Copyright (c) 2007-2013 Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/threads/mutex.h"

/*
 * If we have progress threads, always default to using threads.
 * Otherwise, wait and see if some upper layer wants to use threads.
 */
bool ocoms_uses_threads = false;
bool ocoms_mutex_check_locks = false;


static void ocoms_mutex_construct(ocoms_mutex_t *m)
{
#if OCOMS_HAVE_POSIX_THREADS

#if OCOMS_ENABLE_DEBUG
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    /* set type to ERRORCHECK so that we catch recursive locks */
#if OMPI_HAVE_PTHREAD_MUTEX_ERRORCHECK_NP
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#elif OMPI_HAVE_PTHREAD_MUTEX_ERRORCHECK
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
#endif /* OMPI_HAVE_PTHREAD_MUTEX_ERRORCHECK_NP */

    pthread_mutex_init(&m->m_lock_pthread, &attr);
    pthread_mutexattr_destroy(&attr);

#else

    /* Without debugging, choose the fastest available mutexes */
    pthread_mutex_init(&m->m_lock_pthread, NULL);

#endif /* OCOMS_ENABLE_DEBUG */

#elif OCOMS_HAVE_SOLARIS_THREADS
    mutex_init(&m->m_lock_solaris, USYNC_THREAD, NULL);
#endif

#if OCOMS_ENABLE_DEBUG && !OCOMS_ENABLE_MULTI_THREADS
    m->m_lock_debug = 0;
    m->m_lock_file = NULL;
    m->m_lock_line = 0;
#endif

#if OCOMS_HAVE_ATOMIC_SPINLOCKS
    ocoms_atomic_init( &m->m_lock_atomic, OCOMS_ATOMIC_UNLOCKED );
#endif
}

static void ocoms_mutex_destruct(ocoms_mutex_t *m)
{
#if OCOMS_HAVE_POSIX_THREADS
    pthread_mutex_destroy(&m->m_lock_pthread);
#elif OCOMS_HAVE_SOLARIS_THREADS
    mutex_destroy(&m->m_lock_solaris);
#endif
}

OBJ_CLASS_INSTANCE(ocoms_mutex_t,
                   ocoms_object_t,
                   ocoms_mutex_construct,
                   ocoms_mutex_destruct);
