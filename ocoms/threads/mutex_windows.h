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

#ifndef  OCOMS_MUTEX_WINDOWS_H
#define  OCOMS_MUTEX_WINDOWS_H 1

/**
 * @file:
 *
 * Mutual exclusion functions: Windows implementation.
 *
 * Functions for locking of critical sections.
 *
 * On Windows, base everything on InterlockedExchange().
 */

#include "ocoms/platform/ocoms_config.h"
#include "ocoms/util/ocoms_object.h"
#include "ocoms/sys/atomic.h"

BEGIN_C_DECLS

struct ocoms_mutex_t {
    ocoms_object_t super;
    volatile LONG m_lock;

#if OCOMS_ENABLE_DEBUG
    int m_lock_debug;
    const char *m_lock_file;
    int m_lock_line;
#endif
};

OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_mutex_t);


static inline int ocoms_mutex_trylock(ocoms_mutex_t *m)
{
	return (0 == InterlockedExchange(&m->m_lock, 1) ? 1 : 0);
}


static inline void ocoms_mutex_lock(ocoms_mutex_t *m)
{
    while (InterlockedExchange(&m->m_lock, 1)) {
        while (m->m_lock == 1) {
            /* spin */;
        }
    }
}


static inline void ocoms_mutex_unlock(ocoms_mutex_t *m)
{
    InterlockedExchange(&m->m_lock, 0);
}


static inline int ocoms_mutex_atomic_trylock(ocoms_mutex_t *m)
{
    return ocoms_mutex_trylock(m);
}


static inline void ocoms_mutex_atomic_lock(ocoms_mutex_t *m)
{
   ocoms_mutex_lock(m);
}


static inline void ocoms_mutex_atomic_unlock(ocoms_mutex_t *m)
{
    ocoms_mutex_unlock(m);
}

END_C_DECLS

#endif  /* OCOMS_MUTEX_WINDOWS_H */
