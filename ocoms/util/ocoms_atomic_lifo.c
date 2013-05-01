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
#include "ocoms/util/ocoms_atomic_lifo.h"

static void ocoms_atomic_lifo_construct( ocoms_atomic_lifo_t* lifo )
{
    OBJ_CONSTRUCT( &(lifo->ocoms_lifo_ghost), ocoms_list_item_t );
    lifo->ocoms_lifo_ghost.ocoms_list_next = &(lifo->ocoms_lifo_ghost);
    lifo->ocoms_lifo_head = &(lifo->ocoms_lifo_ghost);
}

OBJ_CLASS_INSTANCE( ocoms_atomic_lifo_t,
                    ocoms_object_t,
                    ocoms_atomic_lifo_construct,
                    NULL );
