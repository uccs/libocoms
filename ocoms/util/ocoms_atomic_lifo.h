/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Voltaire All rights reserved.
 * Copyright (c) 2010      IBM Corporation.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef OCOMS_ATOMIC_LIFO_H_HAS_BEEN_INCLUDED
#define OCOMS_ATOMIC_LIFO_H_HAS_BEEN_INCLUDED

#include "ocoms/platform/ocoms_config.h"
#include "ocoms/util/ocoms_list.h"

#if OCOMS_ENABLE_MULTI_THREADS
#include "ocoms/sys/atomic.h"
#endif  /* OCOMS_ENABLE_MULTI_THREADS */

BEGIN_C_DECLS

/* Atomic Last In First Out lists. If we are in a multi-threaded environment then the
 * atomicity is insured via the compare-and-swap operation, if not we simply do a read
 * and/or a write.
 *
 * There is a trick. The ghost element at the end of the list. This ghost element has
 * the next pointer pointing to itself, therefore we cannot go past the end of the list.
 * With this approach we will never have a NULL element in the list, so we never have
 * to test for the NULL.
 */
struct ocoms_atomic_lifo_t
{
    ocoms_object_t     super;
    ocoms_list_item_t* ocoms_lifo_head;
    ocoms_list_item_t  ocoms_lifo_ghost;
};

typedef struct ocoms_atomic_lifo_t ocoms_atomic_lifo_t;

OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_atomic_lifo_t);


/* The ghost pointer will never change. The head will change via an atomic
 * compare-and-swap. On most architectures the reading of a pointer is an
 * atomic operation so we don't have to protect it.
 */
static inline bool ocoms_atomic_lifo_is_empty( ocoms_atomic_lifo_t* lifo )
{
    return (lifo->ocoms_lifo_head == &(lifo->ocoms_lifo_ghost) ? true : false);
}


/* Add one element to the LIFO. We will return the last head of the list
 * to allow the upper level to detect if this element is the first one in the
 * list (if the list was empty before this operation).
 */
static inline ocoms_list_item_t* ocoms_atomic_lifo_push( ocoms_atomic_lifo_t* lifo,
                                                       ocoms_list_item_t* item )
{
#if OCOMS_ENABLE_MULTI_THREADS
    do {
        item->ocoms_list_next = lifo->ocoms_lifo_head;
        ocoms_atomic_wmb();
        if( ocoms_atomic_cmpset_ptr( &(lifo->ocoms_lifo_head),
                                    (void*)item->ocoms_list_next,
                                    item ) ) {
            ocoms_atomic_cmpset_32((volatile int32_t*)&item->item_free, 1, 0);
            return (ocoms_list_item_t*)item->ocoms_list_next;
        }
        /* DO some kind of pause to release the bus */
    } while( 1 );
#else
    item->ocoms_list_next = lifo->ocoms_lifo_head;
    lifo->ocoms_lifo_head = item;
    return (ocoms_list_item_t*)item->ocoms_list_next;
#endif  /* OCOMS_ENABLE_MULTI_THREADS */
}

/* Retrieve one element from the LIFO. If we reach the ghost element then the LIFO
 * is empty so we return NULL.
 */
static inline ocoms_list_item_t* ocoms_atomic_lifo_pop( ocoms_atomic_lifo_t* lifo )
{
    ocoms_list_item_t* item;
#if OCOMS_ENABLE_MULTI_THREADS
    while((item = lifo->ocoms_lifo_head) != &(lifo->ocoms_lifo_ghost))
    {
        ocoms_atomic_rmb();
        if(!ocoms_atomic_cmpset_32((volatile int32_t*)&item->item_free, 0, 1))
            continue;
        if( ocoms_atomic_cmpset_ptr( &(lifo->ocoms_lifo_head),
                                    item,
                                    (void*)item->ocoms_list_next ) )
            break;
        ocoms_atomic_cmpset_32((volatile int32_t*)&item->item_free, 1, 0);
        /* Do some kind of pause to release the bus */
    } 
#else
    item = lifo->ocoms_lifo_head;
    lifo->ocoms_lifo_head = (ocoms_list_item_t*)item->ocoms_list_next;
#endif  /* OCOMS_ENABLE_MULTI_THREADS */
    if( item == &(lifo->ocoms_lifo_ghost) ) return NULL;
    item->ocoms_list_next = NULL;
    return item;
}

END_C_DECLS

#endif  /* OCOMS_ATOMIC_LIFO_H_HAS_BEEN_INCLUDED */

