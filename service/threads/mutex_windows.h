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
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef  CCS_MUTEX_WINDOWS_H
#define  CCS_MUTEX_WINDOWS_H 1

/**
 * @file:
 *
 * Mutual exclusion functions: Windows implementation.
 *
 * Functions for locking of critical sections.
 *
 * On Windows, base everything on InterlockedExchange().
 */

#include "ccs_config.h"
#include "service/util/service_object.h"
#include "service/sys/atomic.h"

BEGIN_C_DECLS

struct service_mutex_t {
    service_object_t super;
    volatile LONG m_lock;

#if CCS_ENABLE_DEBUG
    int m_lock_debug;
    const char *m_lock_file;
    int m_lock_line;
#endif
};

CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_mutex_t);


static inline int service_mutex_trylock(service_mutex_t *m)
{
	return (0 == InterlockedExchange(&m->m_lock, 1) ? 1 : 0);
}


static inline void service_mutex_lock(service_mutex_t *m)
{
    while (InterlockedExchange(&m->m_lock, 1)) {
        while (m->m_lock == 1) {
            /* spin */;
        }
    }
}


static inline void service_mutex_unlock(service_mutex_t *m)
{
    InterlockedExchange(&m->m_lock, 0);
}


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

END_C_DECLS

#endif  /* CCS_MUTEX_WINDOWS_H */
