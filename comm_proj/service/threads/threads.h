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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef CCS_THREAD_H
#define CCS_THREAD_H 1

#include "ccs_config.h"

#if CCS_HAVE_POSIX_THREADS
#include <pthread.h>
#elif CCS_HAVE_SOLARIS_THREADS
#include <thread.h>
#endif

#include "service/util/service_object.h"
#if CCS_ENABLE_DEBUG
#include "service/util/output.h"
#endif

#include "mutex.h"
#include "condition.h"

BEGIN_C_DECLS

typedef void *(*service_thread_fn_t) (service_object_t *);

#define CCS_THREAD_CANCELLED   ((void*)1);

struct service_thread_t {
    service_object_t super;
    service_thread_fn_t t_run;
    void* t_arg;
#ifdef __WINDOWS__
    HANDLE t_handle;
#elif CCS_HAVE_POSIX_THREADS
    pthread_t t_handle;
#elif CCS_HAVE_SOLARIS_THREADS
    thread_t t_handle;
#endif
};

typedef struct service_thread_t service_thread_t;

#if CCS_ENABLE_DEBUG
CCS_DECLSPEC extern bool service_debug_threads;
#endif


CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_thread_t);

#if CCS_ENABLE_DEBUG
#define CCS_ACQUIRE_THREAD(lck, cnd, act)               \
    do {                                                 \
        CCS_THREAD_LOCK((lck));                         \
        if (service_debug_threads) {                        \
            service_output(0, "Waiting for thread %s:%d",   \
                        __FILE__, __LINE__);             \
        }                                                \
        while (*(act)) {                                 \
            service_condition_wait((cnd), (lck));           \
        }                                                \
        if (service_debug_threads) {                        \
            service_output(0, "Thread obtained %s:%d",      \
                        __FILE__, __LINE__);             \
        }                                                \
        *(act) = true;                                   \
    } while(0);
#else
#define CCS_ACQUIRE_THREAD(lck, cnd, act)               \
    do {                                                 \
        CCS_THREAD_LOCK((lck));                         \
        while (*(act)) {                                 \
            service_condition_wait((cnd), (lck));           \
        }                                                \
        *(act) = true;                                   \
    } while(0);
#endif


#if CCS_ENABLE_DEBUG
#define CCS_RELEASE_THREAD(lck, cnd, act)              \
    do {                                                \
        if (service_debug_threads) {                       \
            service_output(0, "Releasing thread %s:%d",    \
                        __FILE__, __LINE__);            \
        }                                               \
        *(act) = false;                                 \
        service_condition_broadcast((cnd));                \
        CCS_THREAD_UNLOCK((lck));                      \
    } while(0);
#else
#define CCS_RELEASE_THREAD(lck, cnd, act)              \
    do {                                                \
        *(act) = false;                                 \
        service_condition_broadcast((cnd));                \
        CCS_THREAD_UNLOCK((lck));                      \
    } while(0);
#endif


#define CCS_WAKEUP_THREAD(cnd, act)        \
    do {                                    \
        *(act) = false;                     \
        service_condition_broadcast((cnd));    \
    } while(0);


CCS_DECLSPEC int  service_thread_start(service_thread_t *);
CCS_DECLSPEC int  service_thread_join(service_thread_t *, void **thread_return);
CCS_DECLSPEC bool service_thread_self_compare(service_thread_t*);
CCS_DECLSPEC service_thread_t *service_thread_get_self(void);
CCS_DECLSPEC void service_thread_kill(service_thread_t *, int sig);

END_C_DECLS

#endif /* CCS_THREAD_H */
