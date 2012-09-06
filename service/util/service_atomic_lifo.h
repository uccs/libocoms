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
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef SERVICE_ATOMIC_LIFO_H_HAS_BEEN_INCLUDED
#define SERVICE_ATOMIC_LIFO_H_HAS_BEEN_INCLUDED

#include "ccs_config.h"
#include "service/util/service_list.h"

#if CCS_ENABLE_MULTI_THREADS
#include "service/sys/atomic.h"
#endif  /* CCS_ENABLE_MULTI_THREADS */

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
struct service_atomic_lifo_t
{
    service_object_t     super;
    service_list_item_t* service_lifo_head;
    service_list_item_t  service_lifo_ghost;
};

typedef struct service_atomic_lifo_t service_atomic_lifo_t;

CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_atomic_lifo_t);


/* The ghost pointer will never change. The head will change via an atomic
 * compare-and-swap. On most architectures the reading of a pointer is an
 * atomic operation so we don't have to protect it.
 */
static inline bool service_atomic_lifo_is_empty( service_atomic_lifo_t* lifo )
{
    return (lifo->service_lifo_head == &(lifo->service_lifo_ghost) ? true : false);
}


/* Add one element to the LIFO. We will return the last head of the list
 * to allow the upper level to detect if this element is the first one in the
 * list (if the list was empty before this operation).
 */
static inline service_list_item_t* service_atomic_lifo_push( service_atomic_lifo_t* lifo,
                                                       service_list_item_t* item )
{
#if CCS_ENABLE_MULTI_THREADS
    do {
        item->service_list_next = lifo->service_lifo_head;
        service_atomic_wmb();
        if( service_atomic_cmpset_ptr( &(lifo->service_lifo_head),
                                    (void*)item->service_list_next,
                                    item ) ) {
            service_atomic_cmpset_32((volatile int32_t*)&item->item_free, 1, 0);
            return (service_list_item_t*)item->service_list_next;
        }
        /* DO some kind of pause to release the bus */
    } while( 1 );
#else
    item->service_list_next = lifo->service_lifo_head;
    lifo->service_lifo_head = item;
    return (service_list_item_t*)item->service_list_next;
#endif  /* CCS_ENABLE_MULTI_THREADS */
}

/* Retrieve one element from the LIFO. If we reach the ghost element then the LIFO
 * is empty so we return NULL.
 */
static inline service_list_item_t* service_atomic_lifo_pop( service_atomic_lifo_t* lifo )
{
    service_list_item_t* item;
#if CCS_ENABLE_MULTI_THREADS
    while((item = lifo->service_lifo_head) != &(lifo->service_lifo_ghost))
    {
        service_atomic_rmb();
        if(!service_atomic_cmpset_32((volatile int32_t*)&item->item_free, 0, 1))
            continue;
        if( service_atomic_cmpset_ptr( &(lifo->service_lifo_head),
                                    item,
                                    (void*)item->service_list_next ) )
            break;
        service_atomic_cmpset_32((volatile int32_t*)&item->item_free, 1, 0);
        /* Do some kind of pause to release the bus */
    } 
#else
    item = lifo->service_lifo_head;
    lifo->service_lifo_head = (service_list_item_t*)item->service_list_next;
#endif  /* CCS_ENABLE_MULTI_THREADS */
    if( item == &(lifo->service_lifo_ghost) ) return NULL;
    item->service_list_next = NULL;
    return item;
}

END_C_DECLS

#endif  /* SERVICE_ATOMIC_LIFO_H_HAS_BEEN_INCLUDED */

