/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2011-2014 Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 */

#include <time.h>
#include <string.h>

#include "ocoms_config.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/platform/ocoms_stdint.h"

#include "ocoms/util/ocoms_hash_table.h"
#include "ocoms/util/ocoms_pointer_array.h"
#include "ocoms/datatype/ocoms_datatype.h"
#include "ocoms/util/error.h"
#include "ocoms/util/output.h"

#include "ocoms/dstore/base/base.h"
#include "dstore_hash.h"


static int init(struct ocoms_dstore_base_module_t *imod);
static void finalize(struct ocoms_dstore_base_module_t *imod);
// FIXME:
#if 0
static int store(struct ocoms_dstore_base_module_t *imod,
                 const ocoms_identifier_t *proc,
                 ocoms_value_t *val);
static int fetch(struct ocoms_dstore_base_module_t *imod,
                 const ocoms_identifier_t *proc,
                 const char *key,
                 ocoms_list_t *kvs);
static int remove_data(struct ocoms_dstore_base_module_t *imod,
                       const ocoms_identifier_t *proc, const char *key);
#endif

#if 1
mca_dstore_hash_module_t ocoms_dstore_hash_module = {
    {
        init,
        finalize
    }
};
//FiXME:
#else
mca_dstore_hash_module_t ocoms_dstore_hash_module = {
    {
        init,
        finalize,
        store,
        fetch,
        remove_data
    }
};
#endif

/* Initialize our hash table */
static int init(struct ocoms_dstore_base_module_t *imod)
{
    mca_dstore_hash_module_t *mod;

    mod = (mca_dstore_hash_module_t*)imod;
    OBJ_CONSTRUCT(&mod->hash_data, ocoms_hash_table_t);
    ocoms_hash_table_init(&mod->hash_data, 256);
    return OCOMS_SUCCESS;
}

static void finalize(struct ocoms_dstore_base_module_t *imod)
{
    ocoms_dstore_proc_data_t *proc_data;
    uint64_t key;
    char *node;
    mca_dstore_hash_module_t *mod;

    mod = (mca_dstore_hash_module_t*)imod;

    /* to assist in getting a clean valgrind, cycle thru the hash table
     * and release all data stored in it
     */
    if (OCOMS_SUCCESS == ocoms_hash_table_get_first_key_uint64(&mod->hash_data, &key,
                                                             (void**)&proc_data,
                                                             (void**)&node)) {
        if (NULL != proc_data) {
            OBJ_RELEASE(proc_data);
        }
        while (OCOMS_SUCCESS == ocoms_hash_table_get_next_key_uint64(&mod->hash_data, &key,
                                                                   (void**)&proc_data,
                                                                   node, (void**)&node)) {
            if (NULL != proc_data) {
                OBJ_RELEASE(proc_data);
            }
        }
    }
    OBJ_DESTRUCT(&mod->hash_data);
}

//FIXME:
#if 0
static int ocoms_copy_value(ocoms_value_t **dest, ocoms_value_t *src)
{
    ocoms_value_t *p;

    /* create the new object */
    *dest = OBJ_NEW(ocoms_value_t);
    if (NULL == *dest) {
        return OCOMS_ERR_OUT_OF_RESOURCE;
    }
    p = *dest;

    /* copy the type and key */
    if (NULL != src->key) {
        p->key = strdup(src->key);
    }

    if (NULL != src->value.ptr && 0 < src->value.size) {
        p->value.ptr = malloc(src->value.size);
        memcpy(p->value.ptr, src->value.ptr, src->value.size);
        p->value.size = src->value.size;
    } else {
        p->value.ptr = NULL;
        p->value.size = 0;
    }

    return OCOMS_SUCCESS;
}

static int store(struct ocoms_dstore_base_module_t *imod,
                 const ocoms_identifier_t *uid,
                 ocoms_value_t *val)
{
    ocoms_dstore_proc_data_t *proc_data;
    ocoms_value_t *kv;
    ocoms_identifier_t id;
    mca_dstore_hash_module_t *mod;
    int rc;

    mod = (mca_dstore_hash_module_t*)imod;

    /* to protect alignment, copy the identifier across */
    memcpy(&id, uid, sizeof(ocoms_identifier_t));

    ocoms_output_verbose(1, ocoms_dstore_base_framework.framework_output,
                        "dstore:hash:store storing data for proc %lu", id);

    /* lookup the proc data object for this proc */
    if (NULL == (proc_data = ocoms_dstore_base_lookup_proc(&mod->hash_data, id))) {
        /* unrecoverable error */
        ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                             "%s dstore:hash:store: storing data for proc %lu unrecoverably failed",
                             __func__, id);
        return OCOMS_ERR_OUT_OF_RESOURCE;
    }

    /* see if we already have this key in the data - means we are updating
     * a pre-existing value
     */
    kv = ocoms_dstore_base_lookup_keyval(proc_data, val->key);
/*#if OCOMS_ENABLE_DEBUG
    char *_data_type = ocoms_dss.lookup_data_type(val->type);
    ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                         "%s dstore:hash:store: %s key %s[%s] for proc %lu",
                         __func__, (NULL == kv ? "storing" : "updating"),
                         val->key, _data_type, id);
    free (_data_type);
#endif*/

    if (NULL != kv) {
        ocoms_list_remove_item(&proc_data->data, &kv->super);
        OBJ_RELEASE(kv);
    }
    /* create the copy */
    if (OCOMS_SUCCESS != (rc = ocoms_copy_value(&kv, val))) {
        OCOMS_ERROR_LOG(rc);
        return rc;
    }
    ocoms_list_append(&proc_data->data, &kv->super);

    return OCOMS_SUCCESS;
}

static int fetch(struct ocoms_dstore_base_module_t *imod,
                 const ocoms_identifier_t *uid,
                 const char *key, ocoms_list_t *kvs)
{
    ocoms_dstore_proc_data_t *proc_data;
    ocoms_value_t *kv, *knew;
    ocoms_identifier_t id;
    mca_dstore_hash_module_t *mod;
    int rc;

    mod = (mca_dstore_hash_module_t*)imod;

    /* to protect alignment, copy the identifier across */
    memcpy(&id, uid, sizeof(ocoms_identifier_t));

    ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                         "%s dstore:hash:fetch: searching for key %s on proc %lu",
                         __func__, (NULL == key) ? "NULL" : key, id);

    /* lookup the proc data object for this proc */
    if (NULL == (proc_data = ocoms_dstore_base_lookup_proc(&mod->hash_data, id))) {
        ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                             "%s dstore_hash:fetch data for proc %lu not found",
                             __func__, id);
        return OCOMS_ERR_NOT_FOUND;
    }

    /* if the key is NULL, that we want everything */
    if (NULL == key) {
        OCOMS_LIST_FOREACH(kv, &proc_data->data, ocoms_value_t) {
            /* copy the value */
            if (OCOMS_SUCCESS != (rc = ocoms_copy_value(&knew, kv))) {
                OCOMS_ERROR_LOG(rc);
                return rc;
            }
            ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                                 "%s dstore:hash:fetch: adding data for key %s on proc %lu",
                                 __func__, (NULL == kv->key) ? "NULL" : kv->key, id);

            /* add it to the output list */
            ocoms_list_append(kvs, &knew->super);
        }
        return OCOMS_SUCCESS;
    }

    /* find the value */
    if (NULL == (kv = ocoms_dstore_base_lookup_keyval(proc_data, key))) {
        ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                             "%s dstore_hash:fetch key %s for proc %lu not found",
                             __func__, (NULL == key) ? "NULL" : key, id);
        return OCOMS_ERR_NOT_FOUND;
    }

    /* create the copy */
    if (OCOMS_SUCCESS != (rc = ocoms_copy_value(&knew, kv))) {
        OCOMS_ERROR_LOG(rc);
        return rc;
    }
    /* add it to the output list */
    ocoms_list_append(kvs, &knew->super);

    return OCOMS_SUCCESS;
}

static int remove_data(struct ocoms_dstore_base_module_t *imod,
                       const ocoms_identifier_t *uid, const char *key)
{
    ocoms_dstore_proc_data_t *proc_data;
    ocoms_value_t *kv;
    ocoms_identifier_t id;
    mca_dstore_hash_module_t *mod;

    mod = (mca_dstore_hash_module_t*)imod;

    /* to protect alignment, copy the identifier across */
    memcpy(&id, uid, sizeof(ocoms_identifier_t));

    /* lookup the specified proc */
    if (NULL == (proc_data = ocoms_dstore_base_lookup_proc(&mod->hash_data, id))) {
        /* no data for this proc */
        return OCOMS_SUCCESS;
    }

    /* if key is NULL, remove all data for this proc */
    if (NULL == key) {
        while (NULL != (kv = (ocoms_value_t *) ocoms_list_remove_first(&proc_data->data))) {
            OBJ_RELEASE(kv);
        }
        /* remove the proc_data object itself from the jtable */
        ocoms_hash_table_remove_value_uint64(&mod->hash_data, id);
        /* cleanup */
        OBJ_RELEASE(proc_data);
        return OCOMS_SUCCESS;
    }

    /* remove this item */
    OCOMS_LIST_FOREACH(kv, &proc_data->data, ocoms_value_t) {
        if (0 == strcmp(key, kv->key)) {
            ocoms_list_remove_item(&proc_data->data, &kv->super);
            OBJ_RELEASE(kv);
            break;
        }
    }

    return OCOMS_SUCCESS;
}
#endif
