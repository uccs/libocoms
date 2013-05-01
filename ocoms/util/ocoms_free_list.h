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
 * Copyright (c) 2010      IBM Corporation.  All rights reserved.
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OCOMS_FREE_LIST_H
#define OCOMS_FREE_LIST_H

#include "ocoms/platform/ocoms_config.h"
#include "ocoms/util/ocoms_atomic_lifo.h"
#include "ocoms/threads/mutex.h"
#include "ocoms/threads/condition.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/primitives/prefetch.h"
#if 0
#include "ocoms/prefetch.h"
#include "ocoms/threads/condition.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/runtime/opal.h"
#endif

BEGIN_C_DECLS

struct mca_mem_pool_t;
struct ocoms_free_list_item_t;
    
typedef void (*ocoms_free_list_item_init_fn_t) (
        struct ocoms_free_list_item_t*, void* ctx);

typedef struct allocator_handle_t{
    void *allocator_context;
    uint32_t flags;
} allocator_handle_t;

typedef void* (*ocoms_free_list_alloc_fn_t)(
    void* context,
    size_t size,
    size_t align,
    uint32_t flags,
    void** registration);

typedef void (*ocoms_free_list_free_fn_t)(
    void* context,
    void* addr,
    void* registration);

struct ocoms_free_list_t
{
    ocoms_atomic_lifo_t super;
    size_t fl_max_to_alloc;
    size_t fl_num_allocated;
    size_t fl_num_per_alloc;
    size_t fl_num_waiting;
    size_t fl_frag_size;                /* size of the fragment descriptor */
    size_t fl_frag_alignment;           /* fragment descriptor alignment */
    size_t fl_payload_buffer_size;      /* size of payload buffer */
    size_t fl_payload_buffer_alignment; /* payload buffer alignment */
    ocoms_class_t* fl_frag_class;
    ocoms_mutex_t fl_lock;
    ocoms_condition_t fl_condition; 
    ocoms_list_t fl_allocations;
    ocoms_free_list_item_init_fn_t item_init;
    void* ctx;
    allocator_handle_t alloc_handle;
    ocoms_free_list_alloc_fn_t alloc;
    ocoms_free_list_free_fn_t free;
};
typedef struct ocoms_free_list_t ocoms_free_list_t;
OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_free_list_t);


struct ocoms_free_list_item_t
{ 
    ocoms_list_item_t super; 
    void *registration;
    void *ptr;
}; 
typedef struct ocoms_free_list_item_t ocoms_free_list_item_t; 
OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_free_list_item_t);

/**
 * Initialize a free list.
 *
 * @param free_list (IN)           Free list.
 * @param element_size (IN)        Size of each element.
 * @param element_class (IN)       ocoms_class_t of element - used to initialize allocated elements.
 * @param num_elements_to_alloc    Initial number of elements to allocate.
 * @param max_elements_to_alloc    Maximum number of elements to allocate.
 * @param num_elements_per_alloc   Number of elements to grow by per allocation.
 * @param mpool                    Optional memory pool for allocation.s
 */
 
OCOMS_DECLSPEC int ocoms_free_list_init_ex(
    ocoms_free_list_t *free_list, 
    size_t element_size,
    size_t alignment,
    ocoms_class_t* element_class,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    ocoms_free_list_item_init_fn_t item_init,
    void *ctx,
    ocoms_free_list_alloc_fn_t alloc,
    ocoms_free_list_free_fn_t free,
    allocator_handle_t handle,
    ocoms_progress_fn_t ocoms_progress
    );

/**
 * Initialize a free list.  - this will replace ocoms_free_list_init_ex
 *
 * @param free_list                (IN)  Free list.
 * @param frag_size                (IN)  Size of each element - allocated by malloc.
 * @param frag_alignment           (IN)  Fragment alignment.
 * @param frag_class               (IN)  ocoms_class_t of element - used to initialize allocated elements.
 * @param payload_buffer_size      (IN)  Size of payload buffer - allocated from mpool.
 * @param payload_buffer_alignment (IN)  Payload buffer alignment.
 * @param num_elements_to_alloc    (IN)  Initial number of elements to allocate.
 * @param max_elements_to_alloc    (IN)  Maximum number of elements to allocate.
 * @param num_elements_per_alloc   (IN)  Number of elements to grow by per allocation.
 * @param mpool                    (IN)  Optional memory pool for allocation.s
 * @param item_init                (IN)
 * @param ctx                      (IN)
 */
 
OCOMS_DECLSPEC int ocoms_free_list_init_ex_new(
    ocoms_free_list_t *free_list, 
    size_t frag_size,
    size_t frag_alignment,
    ocoms_class_t* frag_class,
    size_t payload_buffer_size,
    size_t payload_buffer_alignment,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    ocoms_free_list_item_init_fn_t item_init,
    void *ctx,
    ocoms_free_list_alloc_fn_t alloc,
    ocoms_free_list_free_fn_t free,
    allocator_handle_t handle,
    ocoms_progress_fn_t ocoms_progress
    );

/**
 * Initialize a free list. - this will replace ocoms_free_list_init
 *
 * @param free_list                (IN)  Free list.
 * @param frag_size                (IN)  Size of each element - allocated by malloc.
 * @param frag_alignment           (IN)  Fragment alignment.
 * @param frag_class               (IN)  ocoms_class_t of element - used to initialize allocated elements.
 * @param payload_buffer_size      (IN)  Size of payload buffer - allocated from mpool.
 * @param payload_buffer_alignment (IN)  Payload buffer alignment.
 * @param num_elements_to_alloc    (IN)  Initial number of elements to allocate.
 * @param max_elements_to_alloc    (IN)  Maximum number of elements to allocate.
 * @param num_elements_per_alloc   (IN)  Number of elements to grow by per allocation.
 * @param mpool                    (IN)  Optional memory pool for allocation.s
 */
static inline int ocoms_free_list_init_new(
    ocoms_free_list_t *free_list, 
    size_t frag_size,
    size_t frag_alignment,
    ocoms_class_t* frag_class,
    size_t payload_buffer_size,
    size_t payload_buffer_alignment,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    ocoms_free_list_alloc_fn_t alloc,
    ocoms_free_list_free_fn_t free,
    allocator_handle_t handle,
    ocoms_progress_fn_t ocoms_progress
    )
{
    return ocoms_free_list_init_ex_new(free_list,
            frag_size, frag_alignment, frag_class,
            payload_buffer_size, payload_buffer_alignment,
            num_elements_to_alloc, max_elements_to_alloc,
            num_elements_per_alloc, NULL, NULL, alloc, free, handle, ocoms_progress);
}

OCOMS_DECLSPEC int ocoms_free_list_grow(ocoms_free_list_t* flist, size_t num_elements);

/* Grow the free list to be *at least* size elements.  This function
   will not shrink the list if it is already larger than size and may
   grow it past size if necessary (it will grow in
   num_elements_per_alloc chunks) */
OCOMS_DECLSPEC int ocoms_free_list_resize(ocoms_free_list_t *flist, size_t size);

/**
 * Attemp to obtain an item from a free list. 
 *
 * @param fl (IN)        Free list.
 * @param item (OUT)     Allocated item.
 * @param rc (OUT)       OCOMS_SUCCESS or error status on failure.
 *
 * If the requested item is not available the free list is grown to 
 * accomodate the request - unless the max number of allocations has 
 * been reached.  If this is the case - an out of resource error is 
 * returned to the caller.
 */
 
#define OCOMS_FREE_LIST_GET(fl, item, rc) \
{ \
    rc = OCOMS_SUCCESS; \
    item = (ocoms_free_list_item_t*) ocoms_atomic_lifo_pop(&((fl)->super)); \
    if( OCOMS_UNLIKELY(NULL == item) ) { \
        if(ocoms_using_threads()) { \
            ocoms_mutex_lock(&((fl)->fl_lock)); \
            ocoms_free_list_grow((fl), (fl)->fl_num_per_alloc); \
            ocoms_mutex_unlock(&((fl)->fl_lock)); \
        } else { \
            ocoms_free_list_grow((fl), (fl)->fl_num_per_alloc); \
        } \
        item = (ocoms_free_list_item_t*) ocoms_atomic_lifo_pop(&((fl)->super)); \
        if( OCOMS_UNLIKELY(NULL == item) ) rc = OCOMS_ERR_TEMP_OUT_OF_RESOURCE; \
    }  \
} 

/**
 * Blocking call to obtain an item from a free list.
 *
 * @param fl (IN)        Free list.
 * @param item (OUT)     Allocated item.
 * @param rc (OUT)       OCOMS_SUCCESS or error status on failure.
 *
 * If the requested item is not available the free list is grown to 
 * accomodate the request - unless the max number of allocations has 
 * been reached. In this case the caller is blocked until an item
 * is returned to the list.
 */

#define OCOMS_FREE_LIST_WAIT(fl, item, rc)                                  \
    rc = __ocoms_free_list_wait( (fl), &(item) )

static inline int __ocoms_free_list_wait( ocoms_free_list_t* fl,
                                         ocoms_free_list_item_t** item )
{
    *item = (ocoms_free_list_item_t*)ocoms_atomic_lifo_pop(&((fl)->super));
    while( NULL == *item ) {
        if( !OCOMS_THREAD_TRYLOCK(&((fl)->fl_lock)) ) {
            if((fl)->fl_max_to_alloc <= (fl)->fl_num_allocated) {
                (fl)->fl_num_waiting++;
                ocoms_condition_wait(&((fl)->fl_condition), &((fl)->fl_lock));
                (fl)->fl_num_waiting--;
            } else {
                if(ocoms_free_list_grow((fl), (fl)->fl_num_per_alloc)
                        == OCOMS_SUCCESS) {
                    if( 0 < (fl)->fl_num_waiting ) {
                        if( 1 == (fl)->fl_num_waiting ) {
                            ocoms_condition_signal(&((fl)->fl_condition));
                        } else {
                            ocoms_condition_broadcast(&((fl)->fl_condition));
                        }
                    }
                } else {
                    (fl)->fl_num_waiting++;
                    ocoms_condition_wait(&((fl)->fl_condition), &((fl)->fl_lock));
                    (fl)->fl_num_waiting--;
                }
            }
        } else {
            /* If I wasn't able to get the lock in the begining when I finaly grab it
             * the one holding the lock in the begining already grow the list. I will
             * release the lock and try to get a new element until I succeed.
             */
            OCOMS_THREAD_LOCK(&((fl)->fl_lock));
        }
        OCOMS_THREAD_UNLOCK(&((fl)->fl_lock));
        *item = (ocoms_free_list_item_t*)ocoms_atomic_lifo_pop(&((fl)->super));
    }
    return OCOMS_SUCCESS;
} 

/**
 * Return an item to a free list. 
 *
 * @param fl (IN)        Free list.
 * @param item (OUT)     Allocated item.
 *
 */
 
#define OCOMS_FREE_LIST_RETURN(fl, item)                                 \
    do {                                                                \
        ocoms_list_item_t* original;                                     \
                                                                        \
        original = ocoms_atomic_lifo_push( &(fl)->super,                 \
                                          &(item)->super);              \
        if( &(fl)->super.ocoms_lifo_ghost == original ) {                \
            OCOMS_THREAD_LOCK(&(fl)->fl_lock);                           \
            if((fl)->fl_num_waiting > 0) {                              \
                if( 1 == (fl)->fl_num_waiting ) {                       \
                    ocoms_condition_signal(&((fl)->fl_condition));       \
                } else {                                                \
                    ocoms_condition_broadcast(&((fl)->fl_condition));    \
                }                                                       \
            }                                                           \
            OCOMS_THREAD_UNLOCK(&(fl)->fl_lock);                         \
        }                                                               \
    } while(0)
    
END_C_DECLS
#endif 

