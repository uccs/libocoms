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
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "service/platform/ocoms_config.h"
#include "service/util/service_atomic_lifo.h"

static void service_atomic_lifo_construct( service_atomic_lifo_t* lifo )
{
    OBJ_CONSTRUCT( &(lifo->service_lifo_ghost), service_list_item_t );
    lifo->service_lifo_ghost.service_list_next = &(lifo->service_lifo_ghost);
    lifo->service_lifo_head = &(lifo->service_lifo_ghost);
}

OBJ_CLASS_INSTANCE( service_atomic_lifo_t,
                    service_object_t,
                    service_atomic_lifo_construct,
                    NULL );
