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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ccs_config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif  /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif  /* HAVE_SYS_MMAN_H */

#include "service/include/service/constants.h"
#include "opal/memoryhooks/memory.h"
#include "opal/memoryhooks/memory_internal.h"
#include "opal/class/service_list.h"
#include "service/util/service_object.h"
#include "opal/sys/atomic.h"

/* 
 * local types
 */
struct callback_list_item_t {
    service_list_item_t super;
    service_mem_hooks_callback_fn_t *cbfunc;
    void *cbdata;
};
typedef struct callback_list_item_t callback_list_item_t;
static OBJ_CLASS_INSTANCE(callback_list_item_t, service_list_item_t, NULL, NULL);

/*
 * local data
 */
static int hooks_support = 0;

static service_list_t release_cb_list;
static service_atomic_lock_t release_lock;
static int release_run_callbacks;

int
service_mem_hooks_init(void)
{
    OBJ_CONSTRUCT(&release_cb_list, service_list_t);

    service_atomic_init(&release_lock, CCS_ATOMIC_UNLOCKED);

    /* delay running callbacks until there is something in the
       registration */
    release_run_callbacks = false;
    service_atomic_mb();

    return CCS_SUCCESS;
}


int
service_mem_hooks_finalize(void)
{
    service_list_item_t *item;

    /* don't try to run callbacks any more */
    release_run_callbacks = false;
    service_atomic_mb();

    /* aquire the lock, just to make sure no one is currently
       twiddling with the list.  We know this won't last long, since
       no new calls will come in after we set run_callbacks to false */
    service_atomic_lock(&release_lock);

    /* clean out the lists */
    while (NULL != (item = service_list_remove_first(&release_cb_list))) {
        OBJ_RELEASE(item);
    }
    OBJ_DESTRUCT(&release_cb_list);

    service_atomic_unlock(&release_lock);

    return CCS_SUCCESS;
}


/* called from memory manager / memory-manager specific hooks */
void
service_mem_hooks_set_support(int support)
{
    hooks_support = support;
}


/* called from the memory manager / memory-manager specific hooks */
void
service_mem_hooks_release_hook(void *buf, size_t length, bool from_alloc)
{
    service_list_item_t *item;

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

    service_atomic_lock(&release_lock);
    item = service_list_get_first(&release_cb_list);
    while(item != service_list_get_end(&release_cb_list)) {
        service_list_item_t* next = service_list_get_next(item);
        callback_list_item_t *cbitem = (callback_list_item_t*) item;
        item = next;

        service_atomic_unlock(&release_lock);
        cbitem->cbfunc(buf, length, cbitem->cbdata, (bool) from_alloc);
        service_atomic_lock(&release_lock);
    }
    service_atomic_unlock(&release_lock);
}


int
service_mem_hooks_support_level(void)
{
    return hooks_support;
}


int
service_mem_hooks_register_release(service_mem_hooks_callback_fn_t *func, void *cbdata)
{
    service_list_item_t *item;
    callback_list_item_t *cbitem, *new_cbitem;
    int ret = CCS_SUCCESS;

    if (0 == ((CCS_MEMORY_FREE_SUPPORT|CCS_MEMORY_MUNMAP_SUPPORT) & hooks_support)) {
        return CCS_ERR_NOT_SUPPORTED;
    }

    /* pre-allocate a callback item on the assumption it won't be
       found.  We can't call OBJ_NEW inside the lock because it might
       call alloc / realloc */
    new_cbitem = OBJ_NEW(callback_list_item_t);
    if (NULL == new_cbitem) {
        ret = CCS_ERR_OUT_OF_RESOURCE;
        goto done;
    }

    service_atomic_lock(&release_lock);
    /* we either have or are about to have a registration that needs
       calling back.  Let the system know it needs to run callbacks
       now */
    release_run_callbacks = true;
    service_atomic_mb();

    /* make sure the callback isn't already in the list */
    for (item = service_list_get_first(&release_cb_list) ;
         item != service_list_get_end(&release_cb_list) ;
         item = service_list_get_next(item)) {
        cbitem = (callback_list_item_t*) item;

        if (cbitem->cbfunc == func) {
            ret = CCS_EXISTS;
            goto done;
        }
    }

    new_cbitem->cbfunc = func;
    new_cbitem->cbdata = cbdata;

    service_list_append(&release_cb_list, (service_list_item_t*) new_cbitem);

 done:
    service_atomic_unlock(&release_lock);

    if (CCS_EXISTS == ret && NULL != new_cbitem) {
        OBJ_RELEASE(new_cbitem);
    }

    return ret;
}


int
service_mem_hooks_unregister_release(service_mem_hooks_callback_fn_t* func)
{
    service_list_item_t *item;
    service_list_item_t *found_item = NULL;
    callback_list_item_t *cbitem;
    int ret = CCS_ERR_NOT_FOUND;

    service_atomic_lock(&release_lock);

    /* make sure the callback isn't already in the list */
    for (item = service_list_get_first(&release_cb_list) ;
         item != service_list_get_end(&release_cb_list) ;
         item = service_list_get_next(item)) {
        cbitem = (callback_list_item_t*) item;

        if (cbitem->cbfunc == func) {
            service_list_remove_item(&release_cb_list, item);
            found_item = item;
            ret = CCS_SUCCESS;
            break;
        }
    }

    service_atomic_unlock(&release_lock);

    /* OBJ_RELEASE calls free, so we can't release until we get out of
       the lock */
    if (NULL != found_item) {
        OBJ_RELEASE(item);
    }

    return ret;
}
