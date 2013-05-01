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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif  /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif  /* HAVE_SYS_MMAN_H */

#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/memoryhooks/memory.h"
#include "ocoms/memoryhooks/memory_internal.h"
#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/ocoms_object.h"
#include "ocoms/sys/atomic.h"

/* 
 * local types
 */
struct callback_list_item_t {
    ocoms_list_item_t super;
    ocoms_mem_hooks_callback_fn_t *cbfunc;
    void *cbdata;
};
typedef struct callback_list_item_t callback_list_item_t;
static OBJ_CLASS_INSTANCE(callback_list_item_t, ocoms_list_item_t, NULL, NULL);

/*
 * local data
 */
static int hooks_support = 0;

static ocoms_list_t release_cb_list;
static ocoms_atomic_lock_t release_lock;
static int release_run_callbacks;

int
ocoms_mem_hooks_init(void)
{
    OBJ_CONSTRUCT(&release_cb_list, ocoms_list_t);

    ocoms_atomic_init(&release_lock, OCOMS_ATOMIC_UNLOCKED);

    /* delay running callbacks until there is something in the
       registration */
    release_run_callbacks = false;
    ocoms_atomic_mb();

    return OCOMS_SUCCESS;
}


int
ocoms_mem_hooks_finalize(void)
{
    ocoms_list_item_t *item;

    /* don't try to run callbacks any more */
    release_run_callbacks = false;
    ocoms_atomic_mb();

    /* aquire the lock, just to make sure no one is currently
       twiddling with the list.  We know this won't last long, since
       no new calls will come in after we set run_callbacks to false */
    ocoms_atomic_lock(&release_lock);

    /* clean out the lists */
    while (NULL != (item = ocoms_list_remove_first(&release_cb_list))) {
        OBJ_RELEASE(item);
    }
    OBJ_DESTRUCT(&release_cb_list);

    ocoms_atomic_unlock(&release_lock);

    return OCOMS_SUCCESS;
}


/* called from memory manager / memory-manager specific hooks */
void
ocoms_mem_hooks_set_support(int support)
{
    hooks_support = support;
}


/* called from the memory manager / memory-manager specific hooks */
void
ocoms_mem_hooks_release_hook(void *buf, size_t length, bool from_alloc)
{
    ocoms_list_item_t *item;

    if (!release_run_callbacks) return;

    /*
     * This is not really thread safe - but we can't hold the lock
     * while calling the callback function as this routine can
     * be called recursively.
     *
     * Instead, we could set a flag if we are already in the callback,
     * and if called recursively queue the new address/length and allow
     * the initial callback to dispatch this
     */

    ocoms_atomic_lock(&release_lock);
    item = ocoms_list_get_first(&release_cb_list);
    while(item != ocoms_list_get_end(&release_cb_list)) {
        ocoms_list_item_t* next = ocoms_list_get_next(item);
        callback_list_item_t *cbitem = (callback_list_item_t*) item;
        item = next;

        ocoms_atomic_unlock(&release_lock);
        cbitem->cbfunc(buf, length, cbitem->cbdata, (bool) from_alloc);
        ocoms_atomic_lock(&release_lock);
    }
    ocoms_atomic_unlock(&release_lock);
}


int
ocoms_mem_hooks_support_level(void)
{
    return hooks_support;
}


int
ocoms_mem_hooks_register_release(ocoms_mem_hooks_callback_fn_t *func, void *cbdata)
{
    ocoms_list_item_t *item;
    callback_list_item_t *cbitem, *new_cbitem;
    int ret = OCOMS_SUCCESS;

    if (0 == ((OCOMS_MEMORY_FREE_SUPPORT|OCOMS_MEMORY_MUNMAP_SUPPORT) & hooks_support)) {
        return OCOMS_ERR_NOT_SUPPORTED;
    }

    /* pre-allocate a callback item on the assumption it won't be
       found.  We can't call OBJ_NEW inside the lock because it might
       call alloc / realloc */
    new_cbitem = OBJ_NEW(callback_list_item_t);
    if (NULL == new_cbitem) {
        ret = OCOMS_ERR_OUT_OF_RESOURCE;
        goto done;
    }

    ocoms_atomic_lock(&release_lock);
    /* we either have or are about to have a registration that needs
       calling back.  Let the system know it needs to run callbacks
       now */
    release_run_callbacks = true;
    ocoms_atomic_mb();

    /* make sure the callback isn't already in the list */
    for (item = ocoms_list_get_first(&release_cb_list) ;
         item != ocoms_list_get_end(&release_cb_list) ;
         item = ocoms_list_get_next(item)) {
        cbitem = (callback_list_item_t*) item;

        if (cbitem->cbfunc == func) {
            ret = OCOMS_EXISTS;
            goto done;
        }
    }

    new_cbitem->cbfunc = func;
    new_cbitem->cbdata = cbdata;

    ocoms_list_append(&release_cb_list, (ocoms_list_item_t*) new_cbitem);

 done:
    ocoms_atomic_unlock(&release_lock);

    if (OCOMS_EXISTS == ret && NULL != new_cbitem) {
        OBJ_RELEASE(new_cbitem);
    }

    return ret;
}


int
ocoms_mem_hooks_unregister_release(ocoms_mem_hooks_callback_fn_t* func)
{
    ocoms_list_item_t *item;
    ocoms_list_item_t *found_item = NULL;
    callback_list_item_t *cbitem;
    int ret = OCOMS_ERR_NOT_FOUND;

    ocoms_atomic_lock(&release_lock);

    /* make sure the callback isn't already in the list */
    for (item = ocoms_list_get_first(&release_cb_list) ;
         item != ocoms_list_get_end(&release_cb_list) ;
         item = ocoms_list_get_next(item)) {
        cbitem = (callback_list_item_t*) item;

        if (cbitem->cbfunc == func) {
            ocoms_list_remove_item(&release_cb_list, item);
            found_item = item;
            ret = OCOMS_SUCCESS;
            break;
        }
    }

    ocoms_atomic_unlock(&release_lock);

    /* OBJ_RELEASE calls free, so we can't release until we get out of
       the lock */
    if (NULL != found_item) {
        OBJ_RELEASE(item);
    }

    return ret;
}
