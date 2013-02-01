/* 
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * Copyright (c) 2008      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#ifndef CCS_THREADS_TSD_H
#define CCS_THREADS_TSD_H

#include "ccs_config.h"

#if CCS_HAVE_POSIX_THREADS
#include <pthread.h>
#elif CCS_HAVE_SOLARIS_THREADS
#include <thread.h>
#endif

#include "service/include/service_constants.h"

BEGIN_C_DECLS

/**
 * @file
 *
 * Thread Specific Datastore Interface
 *
 * Functions for providing thread-specific datastore capabilities.
 */


/**
 * Prototype for callback when tsd data is being destroyed
 */
typedef void (*service_tsd_destructor_t)(void *value);

#if defined(DOXYGEN)

/**
 * Typedef for thread-specific data key
 */
typedef void* service_tsd_key_t;


/**
 * Create thread-specific data key
 *
 * Create a thread-specific data key visible to all threads in the
 * current process.  The returned key is valid in all threads,
 * although the values bound to the key by service_tsd_setspecific() are
 * allocated on a per-thread basis and persist for the life of the
 * calling thread.
 *
 * Upon key creation, the value NULL is associated with the new key in
 * all active threads.  When a new thread is created, the value NULL
 * is associated with all defined keys in the new thread.
 *
 * The destructor parameter may be NULL.  At thread exit, if
 * destructor is non-NULL AND the thread has a non-NULL value
 * associated with the key, the function is called with the current
 * value as its argument.
 *
 * @param key[out]       The key for accessing thread-specific data
 * @param destructor[in] Cleanup function to call when a thread exits
 *
 * @retval CCS_SUCCESS  Success
 * @retval EAGAIN        The system lacked the necessary resource to 
 *                       create another thread specific data key
 * @retval ENOMEM        Insufficient memory exists to create the key
 */
CCS_DECLSPEC int service_tsd_key_create(service_tsd_key_t *key, 
                                      service_tsd_destructor_t destructor);


/**
 * Delete a thread-specific data key
 *
 * Delete a thread-specific data key previously returned by
 * service_tsd_key_create().  The destructor associated with the key is
 * not fired in any thread and memory cleanup is the responsibility of
 * the caller.
 *
 * @note Unlike pthread_key_delete, this function should not be called
 * from within a destructor.  It can not be universally supported at
 * this time.
 *
 * @param key[in]       The key for accessing thread-specific data
 *
 * @retval CCS_SUCCESS Success
 * @retval EINVAL       Invalid key
 */
CCS_DECLSPEC int service_tsd_key_delete(service_tsd_key_t key);


/**
 * Set a thread-specific data value
 *
 * Associates value with key in the current thread.  The value for the
 * key in other threads is not changed.  Different threads may assign
 * different values to the same key.
 *
 * @note This function should not be called within
 * service_tsd_key_delete().
 *
 * @param key[in]       Thread specific data key to modify
 * @param value[in]     Value to associate with key
 *
 * @retval CCS_SUCCESS Success
 * @retval ENOMEM       Insufficient memory exists to associate the
 *                      value with the key
 * @retval EINVAL       Invalid key
 */
CCS_DECLSPEC int service_tsd_setspecific(service_tsd_key_t key, void *value);


/**
 * Get a thread-specific data value
 *
 * Get the data associated with the given key, as set by
 * service_tsd_setspecific().  If service_tsd_setspecific() hasn't been
 * called in the current thread with the given key, NULL is returned
 * in valuep.
 *
 * @param key[in]       Thread specific data key to modify
 * @param value[out]     Value to associate with key
 *
 * @retval CCS_SUCCESS Success
 * @retval ENOMEM       Insufficient memory exists to associate the
 *                      value with the key
 * @retval EINVAL       Invalid key
 */
CCS_DECLSPEC int service_tsd_getspecific(service_tsd_key_t key, void **valuep);

#elif CCS_HAVE_POSIX_THREADS

typedef pthread_key_t service_tsd_key_t;

static inline int
service_tsd_key_create(service_tsd_key_t *key,
                    service_tsd_destructor_t destructor)
{
    return pthread_key_create(key, destructor);
}

static inline int
service_tsd_key_delete(service_tsd_key_t key)
{
    return pthread_key_delete(key);
}

static inline int
service_tsd_setspecific(service_tsd_key_t key, void *value)
{
    return pthread_setspecific(key, value);
}

static inline int
service_tsd_getspecific(service_tsd_key_t key, void **valuep)
{
    *valuep = pthread_getspecific(key);
    return CCS_SUCCESS;
}

#elif CCS_HAVE_SOLARIS_THREADS

typedef thread_key_t service_tsd_key_t;

static inline int
service_tsd_key_create(service_tsd_key_t *key,
                    service_tsd_destructor_t destructor)
{
    return thr_keycreate(key, destructor);
}

static inline int
service_tsd_key_delete(service_tsd_key_t key)
{
    return CCS_SUCCESS;
}

static inline int
service_tsd_setspecific(service_tsd_key_t key, void *value)
{
    return thr_setspecific(key, value);
}

static inline int
service_tsd_getspecific(service_tsd_key_t key, void **valuep)
{
    return thr_getspecific(key, valuep);
}

#elif defined(__WINDOWS__)

/* BWB - FIX ME - this is still not quite right -- we also need to
   implement support for running the destructors when a thread exits,
   but I'm not sure we have a framework for doing that just yet. */

typedef DWORD service_tsd_key_t;

static inline int
service_tsd_key_create(service_tsd_key_t *key,
                    service_tsd_destructor_t destructor)
{
    *key = TlsAlloc();

    return (*key == TLS_OUT_OF_INDEXES) ? CCS_ERROR : CCS_SUCCESS;
}

static inline int
service_tsd_key_delete(service_tsd_key_t key)
{
    key = TlsFree(key);

    return (key == 0) ? CCS_ERROR : CCS_SUCCESS;
}

static inline int
service_tsd_setspecific(service_tsd_key_t key, void *value)
{
    BOOL ret = TlsSetValue(key, (LPVOID) value);

    return (ret) ? CCS_SUCCESS : CCS_ERROR;
}

static inline int
service_tsd_getspecific(service_tsd_key_t key, void **valuep)
{
    *valuep = TlsGetValue(key);
    return CCS_SUCCESS;
}

#else

typedef int service_tsd_key_t;

CCS_DECLSPEC int service_tsd_key_create(service_tsd_key_t *key, 
                                      service_tsd_destructor_t destructor);

CCS_DECLSPEC int service_tsd_key_delete(service_tsd_key_t key);

CCS_DECLSPEC int service_tsd_setspecific(service_tsd_key_t key, void *value);

CCS_DECLSPEC int service_tsd_getspecific(service_tsd_key_t key, void **valuep);

#endif

END_C_DECLS

#endif /* CCS_MTHREADS_TSD_H */
