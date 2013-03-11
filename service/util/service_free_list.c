/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2009 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006-2007 Mellanox Technologies. All rights reserved.
 * Copyright (c) 2010      Cisco Systems, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "service/platform/ccs_config.h"

#include "service/util/service_free_list.h"
#include "service/primitives/align.h"


static void service_free_list_construct(service_free_list_t* fl);
static void service_free_list_destruct(service_free_list_t* fl);

OBJ_CLASS_INSTANCE(service_free_list_t, service_atomic_lifo_t,
        service_free_list_construct, service_free_list_destruct);

OBJ_CLASS_INSTANCE(service_free_list_item_t, 
                   service_list_item_t,
                   NULL, NULL); 

typedef struct service_free_list_item_t service_free_list_memory_t;

static void service_free_list_construct(service_free_list_t* fl)
{
    OBJ_CONSTRUCT(&fl->fl_lock, service_mutex_t);
    OBJ_CONSTRUCT(&fl->fl_condition, service_condition_t);
    fl->fl_max_to_alloc = 0;
    fl->fl_num_allocated = 0;
    fl->fl_num_per_alloc = 0;
    fl->fl_num_waiting = 0;
    fl->fl_frag_size = sizeof(service_free_list_item_t);
    fl->fl_frag_alignment = 0;
    fl->fl_payload_buffer_size=0;
    fl->fl_payload_buffer_alignment=0;
    fl->fl_frag_class = OBJ_CLASS(service_free_list_item_t);
    fl->alloc_handle.allocator_context = NULL;
    fl->alloc_handle.flags = 0;
    fl->alloc = NULL;
    fl->free = NULL;
    fl->ctx = NULL;
    OBJ_CONSTRUCT(&(fl->fl_allocations), service_list_t);
}

static void service_free_list_destruct(service_free_list_t* fl)
{
    service_list_item_t *item;
    service_free_list_memory_t *fl_mem;

#if 0 && CCS_ENABLE_DEBUG
    if(service_list_get_size(&fl->super) != fl->fl_num_allocated) {
        service_output(0, "service_free_list: %d allocated %d returned: %s:%d\n",
            fl->fl_num_allocated, service_list_get_size(&fl->super),
            fl->super.super.cls_init_file_name, fl->super.super.cls_init_lineno);
    }
#endif

    if( NULL != fl->free ) {
        while(NULL != (item = service_list_remove_first(&(fl->fl_allocations)))) {
            fl_mem = (service_free_list_memory_t*)item;

            fl->free(fl->alloc_handle.allocator_context, fl_mem->ptr, fl_mem->registration);

            /* destruct the item (we constructed it), then free the memory chunk */
            OBJ_DESTRUCT(item);
            free(item);
        }
    } else {
        while(NULL != (item = service_list_remove_first(&(fl->fl_allocations)))) {
            /* destruct the item (we constructed it), then free the memory chunk */
            OBJ_DESTRUCT(item);
            free(item);
        }
    }

    OBJ_DESTRUCT(&fl->fl_allocations);
    OBJ_DESTRUCT(&fl->fl_condition);
    OBJ_DESTRUCT(&fl->fl_lock);
}

int service_free_list_init_ex(service_free_list_t *flist,
    size_t elem_size,
    size_t alignment,
    service_class_t* elem_class,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    service_free_list_item_init_fn_t item_init,
    void* ctx,
    service_free_list_alloc_fn_t alloc,
    service_free_list_free_fn_t free,
    allocator_handle_t handle,
    ccs_progress_fn_t ccs_progress
    )
{
    /* alignment must be more than zero and power of two */
    if(alignment <= 1 || (alignment & (alignment - 1)))
        return CCS_ERROR;

    if(elem_size > flist->fl_frag_size)
        flist->fl_frag_size = elem_size;
    flist->fl_frag_alignment = alignment;
    if(elem_class)
        flist->fl_frag_class = elem_class;
    flist->fl_payload_buffer_size=flist->fl_frag_size-
        flist->fl_frag_class->cls_sizeof;
    flist->fl_payload_buffer_alignment=alignment;
    flist->fl_max_to_alloc = max_elements_to_alloc;
    flist->fl_num_allocated = 0;
    flist->fl_num_per_alloc = num_elements_per_alloc;
    flist->item_init = item_init;
    flist->ctx = ctx;

    flist->alloc_handle = handle;
    flist->alloc    = alloc;
    flist->free     = free;
    flist->fl_condition.ccs_progress_fn = ccs_progress;
    /* Sanity check: runtime functions alloc/free must be
     * either both given or not.
     */
    assert((NULL != flist->alloc && NULL != flist->free ) ||
           (NULL == flist->alloc && NULL == flist->free));

    if(num_elements_to_alloc)
        return service_free_list_grow(flist, num_elements_to_alloc);
    return CCS_SUCCESS;
}

/* this will replace service_free_list_init_ex */
int service_free_list_init_ex_new(service_free_list_t *flist,
    size_t frag_size,
    size_t frag_alignment,
    service_class_t* frag_class,
    size_t payload_buffer_size,
    size_t payload_buffer_alignment,
    int num_elements_to_alloc,
    int max_elements_to_alloc,
    int num_elements_per_alloc,
    service_free_list_item_init_fn_t item_init,
    void* ctx,
    service_free_list_alloc_fn_t alloc,
    service_free_list_free_fn_t free,
    allocator_handle_t handle,
    ccs_progress_fn_t ccs_progress
    )
{
    /* alignment must be more than zero and power of two */
    if (frag_alignment <= 1 || (frag_alignment & (frag_alignment - 1)))
        return CCS_ERROR;
    if (0 < payload_buffer_size) {
        if (payload_buffer_alignment <= 1 || (payload_buffer_alignment & (payload_buffer_alignment - 1)))
            return CCS_ERROR;
    }

    if (frag_size > flist->fl_frag_size)
        flist->fl_frag_size = frag_size;
    if (frag_class)
        flist->fl_frag_class = frag_class;
    flist->fl_payload_buffer_size=payload_buffer_size;
    flist->fl_max_to_alloc = max_elements_to_alloc;
    flist->fl_num_allocated = 0;
    flist->fl_num_per_alloc = num_elements_per_alloc;
    flist->fl_frag_alignment = frag_alignment;
    flist->fl_payload_buffer_alignment = payload_buffer_alignment;
    flist->item_init = item_init;
    flist->ctx = ctx;

    flist->alloc_handle = handle;
    flist->alloc    = alloc;
    flist->free     = free;
    flist->fl_condition.ccs_progress_fn = ccs_progress;
    /* Sanity check: runtime functions alloc/free must be
     * either both given or not.
     */
    assert((NULL != flist->alloc && NULL != flist->free ) ||
           (NULL == flist->alloc && NULL == flist->free));

    if (num_elements_to_alloc)
        return service_free_list_grow(flist, num_elements_to_alloc);
    return CCS_SUCCESS;
}
int service_free_list_grow(service_free_list_t* flist, size_t num_elements)
{
    unsigned char *ptr, *runtime_alloc_ptr = NULL;
    service_free_list_memory_t *alloc_ptr;
    size_t i, alloc_size, head_size, elem_size = 0;
    void *reg = NULL;

    if(flist->fl_max_to_alloc > 0)
        if(flist->fl_num_allocated + num_elements > flist->fl_max_to_alloc)
            num_elements = flist->fl_max_to_alloc - flist->fl_num_allocated;

    if(num_elements == 0)
        return CCS_ERR_TEMP_OUT_OF_RESOURCE;

    head_size = (NULL == flist->alloc) ? flist->fl_frag_size:
        flist->fl_frag_class->cls_sizeof;
    head_size = CCS_ALIGN(head_size, flist->fl_frag_alignment, size_t);

    /* calculate head allocation size */
    alloc_size = num_elements * head_size + sizeof(service_free_list_memory_t) +
        flist->fl_frag_alignment;

    alloc_ptr = (service_free_list_memory_t*)malloc(alloc_size);

    if(NULL == alloc_ptr)
        return CCS_ERR_TEMP_OUT_OF_RESOURCE;

    /* allocate the rest from the runtime */
    if(flist->alloc != NULL) {
        elem_size = CCS_ALIGN(flist->fl_payload_buffer_size, 
                flist->fl_payload_buffer_alignment, size_t);
        if(elem_size != 0) {
            runtime_alloc_ptr = (unsigned char *) flist->alloc(flist->alloc_handle.allocator_context,
                   num_elements * elem_size, flist->fl_payload_buffer_alignment,
                   flist->alloc_handle.flags, &reg);
            if(NULL == runtime_alloc_ptr) {
                free(alloc_ptr);
                return CCS_ERR_TEMP_OUT_OF_RESOURCE;
            }
        }
    }

    /* make the alloc_ptr a list item, save the chunk in the allocations list,
     * and have ptr point to memory right after the list item structure */
    OBJ_CONSTRUCT(alloc_ptr, service_free_list_item_t);
    service_list_append(&(flist->fl_allocations), (service_list_item_t*)alloc_ptr);

    alloc_ptr->registration = reg;
    alloc_ptr->ptr = runtime_alloc_ptr;

    ptr = (unsigned char*)alloc_ptr + sizeof(service_free_list_memory_t);
    ptr = CCS_ALIGN_PTR(ptr, flist->fl_frag_alignment, unsigned char*);

    for(i=0; i<num_elements; i++) {
        service_free_list_item_t* item = (service_free_list_item_t*)ptr;
        item->registration = reg;
        item->ptr = runtime_alloc_ptr;

        OBJ_CONSTRUCT_INTERNAL(item, flist->fl_frag_class);
        
        /* run the initialize function if present */
        if(flist->item_init) { 
            flist->item_init(item, flist->ctx);
        }

        service_atomic_lifo_push(&(flist->super), &(item->super));
        ptr += head_size;
        runtime_alloc_ptr += elem_size;
        
    }
    flist->fl_num_allocated += num_elements;
    return CCS_SUCCESS;
}

/**
 * This function resize the free_list to contain at least the specified
 * number of elements. We do not create all of them in the same memory
 * segment. Instead we will several time the fl_num_per_alloc elements
 * until we reach the required number of the maximum allowed by the 
 * initialization.
 */
int
service_free_list_resize(service_free_list_t* flist, size_t size)
{
    ssize_t inc_num;
    int ret = CCS_SUCCESS;

    if (flist->fl_num_allocated > size) {
        return CCS_SUCCESS;
    }
    SERVICE_THREAD_LOCK(&((flist)->fl_lock));
    inc_num = (ssize_t)size - (ssize_t)flist->fl_num_allocated;
    while( inc_num > 0 ) {
        ret = service_free_list_grow(flist, flist->fl_num_per_alloc);
        if( CCS_SUCCESS != ret ) break;
        inc_num = (ssize_t)size - (ssize_t)flist->fl_num_allocated;
    }
    SERVICE_THREAD_UNLOCK(&((flist)->fl_lock));

    return ret;
}
