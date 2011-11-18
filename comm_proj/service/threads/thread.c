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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ccs_config.h"

#include "service/threads/threads.h"
#include "service/include/service/constants.h"

bool service_debug_threads = false;

static void service_thread_construct(service_thread_t *t);

OBJ_CLASS_INSTANCE(service_thread_t,
                   service_object_t,
                   service_thread_construct, NULL);


/*
 * Constructor
 */
static void service_thread_construct(service_thread_t *t)
{
    t->t_run = 0;
#ifdef __WINDOWS__
    t->t_handle = (HANDLE)NULL;
#elif CCS_HAVE_POSIX_THREADS
    t->t_handle = (pthread_t) -1;
#elif CCS_HAVE_SOLARIS_THREADS
    t->t_handle = (thread_t) -1;
#endif
}


#ifdef __WINDOWS__

/************************************************************************
 * Windows threads
 ************************************************************************/

int service_thread_start(service_thread_t *t)
{
    DWORD tid;

    if (CCS_ENABLE_DEBUG) {
        if (NULL == t->t_run || t->t_handle != (HANDLE) -1L) {
            return CCS_ERR_BAD_PARAM;
        }
    }

    t->t_handle = CreateThread(NULL,    /* default security attributes */
                               0,       /* default stack size */
                               (LPTHREAD_START_ROUTINE) t->t_run,
                               t,       /* argument */
                               0,       /* default creation flags */
                               &tid);

    if (t->t_handle == NULL) {
        return CCS_ERROR;
    }

    return CCS_SUCCESS;
}


int service_thread_join(service_thread_t *t, void **thr_return)
{
    DWORD rc;

    if (WaitForSingleObject(t->t_handle, INFINITE) != WAIT_OBJECT_0) {
        return CCS_ERROR;
    }
    if (!GetExitCodeThread(t->t_handle, &rc)) {
        return CCS_ERROR;
    }

    if( NULL != thr_return ) {
        *thr_return = (void *)((intptr_t)rc);
    }

    return CCS_SUCCESS;
}


bool service_thread_self_compare(service_thread_t *t)
{
    HANDLE thread_handle;
    thread_handle = GetCurrentThread();
    return (thread_handle == t->t_handle ? true : false);
}


service_thread_t *service_thread_get_self(void)
{
    service_thread_t *t = OBJ_NEW(service_thread_t);
    t->t_handle = GetCurrentThread();
    return t;
}


void service_thread_kill(service_thread_t *t, int sig)
{
}

#elif CCS_HAVE_POSIX_THREADS

/************************************************************************
 * POSIX threads
 ************************************************************************/

int service_thread_start(service_thread_t *t)
{
    int rc;

    if (CCS_ENABLE_DEBUG) {
        if (NULL == t->t_run || t->t_handle != (pthread_t) -1) {
            return CCS_ERR_BAD_PARAM;
        }
    }

    rc = pthread_create(&t->t_handle, NULL, (void*(*)(void*)) t->t_run, t);

    return (rc == 0) ? CCS_SUCCESS : CCS_ERROR;
}


int service_thread_join(service_thread_t *t, void **thr_return)
{
    int rc = pthread_join(t->t_handle, thr_return);
    t->t_handle = (pthread_t) -1;
    return (rc == 0) ? CCS_SUCCESS : CCS_ERROR;
}


bool service_thread_self_compare(service_thread_t *t)
{
    return t->t_handle == pthread_self();
}


service_thread_t *service_thread_get_self(void)
{
    service_thread_t *t = OBJ_NEW(service_thread_t);
    t->t_handle = pthread_self();
    return t;
}

void service_thread_kill(service_thread_t *t, int sig)
{
    pthread_kill(t->t_handle, sig);
}


#elif CCS_HAVE_SOLARIS_THREADS

/************************************************************************
 * Solaris threads
 ************************************************************************/

int service_thread_start(service_thread_t *t)
{
    int rc;

    if (CCS_ENABLE_DEBUG) {
        if (NULL == t->t_run || t->t_handle != (thread_t) -1) {
            return CCS_ERR_BAD_PARAM;
        }
    }

    rc = thr_create(NULL, 0, (void*(*)(void*)) t->t_run, t, NULL,
                    &t->t_handle);

    return (rc == 0) ? CCS_SUCCESS : CCS_ERROR;
}


int service_thread_join(service_thread_t *t, void **thr_return)
{
    int rc = thr_join(t->t_handle, NULL, thr_return);
    t->t_handle = (thread_t) -1;
    return (rc == 0) ? CCS_SUCCESS : CCS_ERROR;
}


bool service_thread_self_compare(service_thread_t *t)
{
    return t->t_handle == thr_self();
}


service_thread_t *service_thread_get_self(void)
{
    service_thread_t *t = OBJ_NEW(service_thread_t);
    t->t_handle = thr_self();
    return t;
}

void service_thread_kill(service_thread_t *t, int sig)
{
    thr_kill(t->t_handle, sig);
}


#else

/************************************************************************
 * No thread support
 ************************************************************************/

int service_thread_start(service_thread_t *t)
{
    return CCS_ERROR;
}


int service_thread_join(service_thread_t *t, void **thr_return)
{
    return CCS_ERROR;
}


bool service_thread_self_compare(service_thread_t *t)
{
    return true;
}

service_thread_t *service_thread_get_self(void)
{
    return NULL;
}

void service_thread_kill(service_thread_t *t, int sig)
{
}

#endif
