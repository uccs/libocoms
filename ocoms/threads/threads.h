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
 * Copyright (c) 2010      Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2010      Oracle and/or its affiliates.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OCOMS_THREAD_H
#define OCOMS_THREAD_H 1

#include "ocoms/platform/ocoms_config.h"

#if OCOMS_HAVE_POSIX_THREADS
#include <pthread.h>
#elif OCOMS_HAVE_SOLARIS_THREADS
#include <thread.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include "ocoms/util/ocoms_object.h"
#if OCOMS_ENABLE_DEBUG
#include "ocoms/util/output.h"
#endif

#include "mutex.h"
#include "condition.h"

BEGIN_C_DECLS

typedef void *(*ocoms_thread_fn_t) (ocoms_object_t *);

#define OCOMS_THREAD_CANCELLED   ((void*)1);

struct ocoms_thread_t {
    ocoms_object_t super;
    ocoms_thread_fn_t t_run;
    void* t_arg;
#ifdef OCOMS_HAVE_POSIX_THREADS
    pthread_t t_handle;
#elif OCOMS_HAVE_SOLARIS_THREADS
    thread_t t_handle;
#endif
};

typedef struct ocoms_thread_t ocoms_thread_t;

#if OCOMS_ENABLE_DEBUG
OCOMS_DECLSPEC extern bool ocoms_debug_threads;
#endif


OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_thread_t);

#if OCOMS_ENABLE_DEBUG
#define OCOMS_ACQUIRE_THREAD(lck, cnd, act)               \
    do {                                                 \
        OCOMS_THREAD_LOCK((lck));                         \
        if (ocoms_debug_threads) {                        \
            ocoms_output(0, "Waiting for thread %s:%d",   \
                        __FILE__, __LINE__);             \
        }                                                \
        while (*(act)) {                                 \
            ocoms_condition_wait((cnd), (lck));           \
        }                                                \
        if (ocoms_debug_threads) {                        \
            ocoms_output(0, "Thread obtained %s:%d",      \
                        __FILE__, __LINE__);             \
        }                                                \
        *(act) = true;                                   \
    } while(0);
#else
#define OCOMS_ACQUIRE_THREAD(lck, cnd, act)               \
    do {                                                 \
        OCOMS_THREAD_LOCK((lck));                         \
        while (*(act)) {                                 \
            ocoms_condition_wait((cnd), (lck));           \
        }                                                \
        *(act) = true;                                   \
    } while(0);
#endif


#if OCOMS_ENABLE_DEBUG
#define OCOMS_RELEASE_THREAD(lck, cnd, act)              \
    do {                                                \
        if (ocoms_debug_threads) {                       \
            ocoms_output(0, "Releasing thread %s:%d",    \
                        __FILE__, __LINE__);            \
        }                                               \
        *(act) = false;                                 \
        ocoms_condition_broadcast((cnd));                \
        OCOMS_THREAD_UNLOCK((lck));                      \
    } while(0);
#else
#define OCOMS_RELEASE_THREAD(lck, cnd, act)              \
    do {                                                \
        *(act) = false;                                 \
        ocoms_condition_broadcast((cnd));                \
        OCOMS_THREAD_UNLOCK((lck));                      \
    } while(0);
#endif


#define OCOMS_WAKEUP_THREAD(cnd, act)        \
    do {                                    \
        *(act) = false;                     \
        ocoms_condition_broadcast((cnd));    \
    } while(0);


OCOMS_DECLSPEC int  ocoms_thread_start(ocoms_thread_t *);
OCOMS_DECLSPEC int  ocoms_thread_join(ocoms_thread_t *, void **thread_return);
OCOMS_DECLSPEC bool ocoms_thread_self_compare(ocoms_thread_t*);
OCOMS_DECLSPEC ocoms_thread_t *ocoms_thread_get_self(void);
OCOMS_DECLSPEC void ocoms_thread_kill(ocoms_thread_t *, int sig);

END_C_DECLS

#endif /* OCOMS_THREAD_H */
