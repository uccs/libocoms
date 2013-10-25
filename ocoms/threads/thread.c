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
 * Copyright (c) 2010      Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/threads/threads.h"
#include "ocoms/platform/ocoms_constants.h"

bool ocoms_debug_threads = false;

static void ocoms_thread_construct(ocoms_thread_t *t);

OBJ_CLASS_INSTANCE(ocoms_thread_t,
                   ocoms_object_t,
                   ocoms_thread_construct, NULL);


/*
 * Constructor
 */
static void ocoms_thread_construct(ocoms_thread_t *t)
{
    t->t_run = 0;
#if OCOMS_HAVE_POSIX_THREADS
    t->t_handle = (pthread_t) -1;
#elif OCOMS_HAVE_SOLARIS_THREADS
    t->t_handle = (thread_t) -1;
#endif
}

#if OCOMS_HAVE_POSIX_THREADS

/************************************************************************
 * POSIX threads
 ************************************************************************/

int ocoms_thread_start(ocoms_thread_t *t)
{
    int rc;

    if (OCOMS_ENABLE_DEBUG) {
        if (NULL == t->t_run || t->t_handle != (pthread_t) -1) {
            return OCOMS_ERR_BAD_PARAM;
        }
    }

    rc = pthread_create(&t->t_handle, NULL, (void*(*)(void*)) t->t_run, t);

    return (rc == 0) ? OCOMS_SUCCESS : OCOMS_ERROR;
}


int ocoms_thread_join(ocoms_thread_t *t, void **thr_return)
{
    int rc = pthread_join(t->t_handle, thr_return);
    t->t_handle = (pthread_t) -1;
    return (rc == 0) ? OCOMS_SUCCESS : OCOMS_ERROR;
}


bool ocoms_thread_self_compare(ocoms_thread_t *t)
{
    return t->t_handle == pthread_self();
}


ocoms_thread_t *ocoms_thread_get_self(void)
{
    ocoms_thread_t *t = OBJ_NEW(ocoms_thread_t);
    t->t_handle = pthread_self();
    return t;
}

void ocoms_thread_kill(ocoms_thread_t *t, int sig)
{
    pthread_kill(t->t_handle, sig);
}


#elif OCOMS_HAVE_SOLARIS_THREADS

/************************************************************************
 * Solaris threads
 ************************************************************************/

int ocoms_thread_start(ocoms_thread_t *t)
{
    int rc;

    if (OCOMS_ENABLE_DEBUG) {
        if (NULL == t->t_run || t->t_handle != (thread_t) -1) {
            return OCOMS_ERR_BAD_PARAM;
        }
    }

    rc = thr_create(NULL, 0, (void*(*)(void*)) t->t_run, t, NULL,
                    &t->t_handle);

    return (rc == 0) ? OCOMS_SUCCESS : OCOMS_ERROR;
}


int ocoms_thread_join(ocoms_thread_t *t, void **thr_return)
{
    int rc = thr_join(t->t_handle, NULL, thr_return);
    t->t_handle = (thread_t) -1;
    return (rc == 0) ? OCOMS_SUCCESS : OCOMS_ERROR;
}


bool ocoms_thread_self_compare(ocoms_thread_t *t)
{
    return t->t_handle == thr_self();
}


ocoms_thread_t *ocoms_thread_get_self(void)
{
    ocoms_thread_t *t = OBJ_NEW(ocoms_thread_t);
    t->t_handle = thr_self();
    return t;
}

void ocoms_thread_kill(ocoms_thread_t *t, int sig)
{
    thr_kill(t->t_handle, sig);
}


#else

/************************************************************************
 * No thread support
 ************************************************************************/

int ocoms_thread_start(ocoms_thread_t *t)
{
    return OCOMS_ERROR;
}


int ocoms_thread_join(ocoms_thread_t *t, void **thr_return)
{
    return OCOMS_ERROR;
}


bool ocoms_thread_self_compare(ocoms_thread_t *t)
{
    return true;
}

ocoms_thread_t *ocoms_thread_get_self(void)
{
    return NULL;
}

void ocoms_thread_kill(ocoms_thread_t *t, int sig)
{
}

#endif
