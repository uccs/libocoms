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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/threads/condition.h"


static void ocoms_condition_construct(ocoms_condition_t *c)
{
    c->c_waiting = 0;
    c->c_signaled = 0;
#if OCOMS_HAVE_POSIX_THREADS
    pthread_cond_init(&c->c_cond, NULL);
#endif
    c->name = NULL;
    c->ocoms_progress_fn = NULL;
}


static void ocoms_condition_destruct(ocoms_condition_t *c)
{
#if OCOMS_HAVE_POSIX_THREADS
    pthread_cond_destroy(&c->c_cond);
#endif
    if (NULL != c->name) {
        free(c->name);
    }
}

OBJ_CLASS_INSTANCE(ocoms_condition_t,
                   ocoms_object_t,
                   ocoms_condition_construct,
                   ocoms_condition_destruct);
