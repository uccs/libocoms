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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef SERVICE_FREE_LIST_H
#define SERVICE_FREE_LIST_H

#include "service/platform/ccs_config.h"
#include "service/util/service_atomic_lifo.h"
#include "service/threads/mutex.h"
#include "service/threads/condition.h"
#include "service/platform/service_constants.h"
#include "service/primitives/prefetch.h"
#if 0
#include "service/prefetch.h"
#include "service/threads/condition.h"
#include "service/platform/service_constants.h"
#include "service/runtime/opal.h"
#endif

BEGIN_C_DECLS

struct mca_mem_pool_t;
struct service_free_list_item_t;
    
typedef void (*service_free_list_item_init_fn_t) (
        struct service_free_list_item_t*, void* ctx);

typedef struct allocator_handle_t{
    void *allocator_context;
    uint32_t flags;
} allocator_handle_t;

typedef void* (*service_free_list_alloc_fn_t)(
    void* context,
    size_t size,
    size_t align,
    uint32_t flags,
    void** registration);

typedef void (*service_free_list_free_fn_t)(
    void* context,
    void* addr,
    void* registration);

struct service_free_list_t
{
    service_atomic_lifo_t super;
    size_t fl_max_to_alloc;
    size_t fl_num_allocated;
    size_t fl_num_per_alloc;
    size_t fl_num_waiting;
    size_t fl_frag_size;                /* size of the fragment descriptor */
    size_t fl_frag_alignment;           /* fragment descriptor alignment */
    size_t fl_payload_buffer_size;      /* size of payload buffer */
    size_t fl_payload_buffer_alignment; /* payload buffer alignment */
    service_class_t* fl_frag_class;
    service_mutex_t fl_lock;
    service_condition_t fl_condition; 
    service_list_t fl_allocations;
    service_free_list_item_init_fn_t item_init;
    void* ctx;
    allocator_handle_t alloc_handle;
    service_free_list_alloc_fn_t alloc;
    service_free_list_free_fn_t free;
};
typedef struct service_free_list_t service_free_list_t;
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_free_list_t);


struct service_free_list_item_t
{ 
    service_list_item_t super; 
    void *registration;
    void *ptr;
}; 
typedef struct service_free_list_item_t service_free_list_item_t; 
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_free_list_item_t);

/**
 * Initialize a free list.
 *
 * @param free_list (IN)           Free list.
 * @param element_size (IN)        Size of each element.
 * @param element_class (IN)       service_class_t of element - used to initialize allocated elements.
 * @param num_elements_to_alloc    Initial number of elements to allocate.
 * @param max_elements_to_alloc    Maximum number of elements to allocate.
 * @param num_elements_per_alloc   Number of elements to grow by per allocation.
 * @param mpool                    Optional memory pool for allocation.s
 */
 
CCS_DECLSPEC int service_free_list_init_ex(
    service_free_list_t *free_list, 
    size_t element_size,
    size_t alignment,
    service_class_t* element_class,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    service_free_list_item_init_fn_t item_init,
    void *ctx,
    service_free_list_alloc_fn_t alloc,
    service_free_list_free_fn_t free,
    allocator_handle_t handle,
    ccs_progress_fn_t ccs_progress
    );

/**
 * Initialize a free list.  - this will replace service_free_list_init_ex
 *
 * @param free_list                (IN)  Free list.
 * @param frag_size                (IN)  Size of each element - allocated by malloc.
 * @param frag_alignment           (IN)  Fragment alignment.
 * @param frag_class               (IN)  service_class_t of element - used to initialize allocated elements.
 * @param payload_buffer_size      (IN)  Size of payload buffer - allocated from mpool.
 * @param payload_buffer_alignment (IN)  Payload buffer alignment.
 * @param num_elements_to_alloc    (IN)  Initial number of elements to allocate.
 * @param max_elements_to_alloc    (IN)  Maximum number of elements to allocate.
 * @param num_elements_per_alloc   (IN)  Number of elements to grow by per allocation.
 * @param mpool                    (IN)  Optional memory pool for allocation.s
 * @param item_init                (IN)
 * @param ctx                      (IN)
 */
 
CCS_DECLSPEC int service_free_list_init_ex_new(
    service_free_list_t *free_list, 
    size_t frag_size,
    size_t frag_alignment,
    service_class_t* frag_class,
    size_t payload_buffer_size,
    size_t payload_buffer_alignment,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    service_free_list_item_init_fn_t item_init,
    void *ctx,
    service_free_list_alloc_fn_t alloc,
    service_free_list_free_fn_t free,
    allocator_handle_t handle,
    ccs_progress_fn_t ccs_progress
    );

/**
 * Initialize a free list. - this will replace service_free_list_init
 *
 * @param free_list                (IN)  Free list.
 * @param frag_size                (IN)  Size of each element - allocated by malloc.
 * @param frag_alignment           (IN)  Fragment alignment.
 * @param frag_class               (IN)  service_class_t of element - used to initialize allocated elements.
 * @param payload_buffer_size      (IN)  Size of payload buffer - allocated from mpool.
 * @param payload_buffer_alignment (IN)  Payload buffer alignment.
 * @param num_elements_to_alloc    (IN)  Initial number of elements to allocate.
 * @param max_elements_to_alloc    (IN)  Maximum number of elements to allocate.
 * @param num_elements_per_alloc   (IN)  Number of elements to grow by per allocation.
 * @param mpool                    (IN)  Optional memory pool for allocation.s
 */
static inline int service_free_list_init_new(
    service_free_list_t *free_list, 
    size_t frag_size,
    size_t frag_alignment,
    service_class_t* frag_class,
    size_t payload_buffer_size,
    size_t payload_buffer_alignment,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    service_free_list_alloc_fn_t alloc,
    service_free_list_free_fn_t free,
    allocator_handle_t handle,
    ccs_progress_fn_t ccs_progress
    )
{
    return service_free_list_init_ex_new(free_list,
            frag_size, frag_alignment, frag_class,
            payload_buffer_size, payload_buffer_alignment,
            num_elements_to_alloc, max_elements_to_alloc,
            num_elements_per_alloc, NULL, NULL, alloc, free, handle, ccs_progress);
}

CCS_DECLSPEC int service_free_list_grow(service_free_list_t* flist, size_t num_elements);

/* Grow the free list to be *at least* size elements.  This function
   will not shrink the list if it is already larger than size and may
   grow it past size if necessary (it will grow in
   num_elements_per_alloc chunks) */
CCS_DECLSPEC int service_free_list_resize(service_free_list_t *flist, size_t size);

/**
 * Attemp to obtain an item from a free list. 
 *
 * @param fl (IN)        Free list.
 * @param item (OUT)     Allocated item.
 * @param rc (OUT)       CCS_SUCCESS or error status on failure.
 *
 * If the requested item is not available the free list is grown to 
 * accomodate the request - unless the max number of allocations has 
 * been reached.  If this is the case - an out of resource error is 
 * returned to the caller.
 */
 
#define SERVICE_FREE_LIST_GET(fl, item, rc) \
{ \
    rc = CCS_SUCCESS; \
    item = (service_free_list_item_t*) service_atomic_lifo_pop(&((fl)->super)); \
    if( CCS_UNLIKELY(NULL == item) ) { \
        if(ccs_using_threads()) { \
            service_mutex_lock(&((fl)->fl_lock)); \
            service_free_list_grow((fl), (fl)->fl_num_per_alloc); \
            service_mutex_unlock(&((fl)->fl_lock)); \
        } else { \
            service_free_list_grow((fl), (fl)->fl_num_per_alloc); \
        } \
        item = (service_free_list_item_t*) service_atomic_lifo_pop(&((fl)->super)); \
        if( CCS_UNLIKELY(NULL == item) ) rc = CCS_ERR_TEMP_OUT_OF_RESOURCE; \
    }  \
} 

/**
 * Blocking call to obtain an item from a free list.
 *
 * @param fl (IN)        Free list.
 * @param item (OUT)     Allocated item.
 * @param rc (OUT)       CCS_SUCCESS or error status on failure.
 *
 * If the requested item is not available the free list is grown to 
 * accomodate the request - unless the max number of allocations has 
 * been reached. In this case the caller is blocked until an item
 * is returned to the list.
 */

#define SERVICE_FREE_LIST_WAIT(fl, item, rc)                                  \
    rc = __service_free_list_wait( (fl), &(item) )

static inline int __service_free_list_wait( service_free_list_t* fl,
                                         service_free_list_item_t** item )
{
    *item = (service_free_list_item_t*)service_atomic_lifo_pop(&((fl)->super));
    while( NULL == *item ) {
        if( !SERVICE_THREAD_TRYLOCK(&((fl)->fl_lock)) ) {
            if((fl)->fl_max_to_alloc <= (fl)->fl_num_allocated) {
                (fl)->fl_num_waiting++;
                service_condition_wait(&((fl)->fl_condition), &((fl)->fl_lock));
                (fl)->fl_num_waiting--;
            } else {
                if(service_free_list_grow((fl), (fl)->fl_num_per_alloc)
                        == CCS_SUCCESS) {
                    if( 0 < (fl)->fl_num_waiting ) {
                        if( 1 == (fl)->fl_num_waiting ) {
                            service_condition_signal(&((fl)->fl_condition));
                        } else {
                            service_condition_broadcast(&((fl)->fl_condition));
                        }
                    }
                } else {
                    (fl)->fl_num_waiting++;
                    service_condition_wait(&((fl)->fl_condition), &((fl)->fl_lock));
                    (fl)->fl_num_waiting--;
                }
            }
        } else {
            /* If I wasn't able to get the lock in the begining when I finaly grab it
             * the one holding the lock in the begining already grow the list. I will
             * release the lock and try to get a new element until I succeed.
             */
            SERVICE_THREAD_LOCK(&((fl)->fl_lock));
        }
        SERVICE_THREAD_UNLOCK(&((fl)->fl_lock));
        *item = (service_free_list_item_t*)service_atomic_lifo_pop(&((fl)->super));
    }
    return CCS_SUCCESS;
} 

/**
 * Return an item to a free list. 
 *
 * @param fl (IN)        Free list.
 * @param item (OUT)     Allocated item.
 *
 */
 
#define SERVICE_FREE_LIST_RETURN(fl, item)                                 \
    do {                                                                \
        service_list_item_t* original;                                     \
                                                                        \
        original = service_atomic_lifo_push( &(fl)->super,                 \
                                          &(item)->super);              \
        if( &(fl)->super.service_lifo_ghost == original ) {                \
            SERVICE_THREAD_LOCK(&(fl)->fl_lock);                           \
            if((fl)->fl_num_waiting > 0) {                              \
                if( 1 == (fl)->fl_num_waiting ) {                       \
                    service_condition_signal(&((fl)->fl_condition));       \
                } else {                                                \
                    service_condition_broadcast(&((fl)->fl_condition));    \
                }                                                       \
            }                                                           \
            SERVICE_THREAD_UNLOCK(&(fl)->fl_lock);                         \
        }                                                               \
    } while(0)
    
END_C_DECLS
#endif 

