/*
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved. 
 * Copyright (c) 2013-2014 Intel Inc. All rights reserved
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#include "ocoms_config.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/platform/ocoms_stdint.h"

#include "ocoms/mca/mca.h"
#include "ocoms/util/error.h"
#include "ocoms/util/output.h"
#include "ocoms/mca/base/base.h"
#include "ocoms/datatype/ocoms_datatype.h"

#include "ocoms/dstore/base/base.h"


int ocoms_dstore_base_open(const char *name, ocoms_list_t *attrs)
{
    ocoms_dstore_handle_t *hdl;
    int index;
    ocoms_dstore_base_module_t *mod;

    /* ask the storage component for a module */
    if (NULL != (mod = ocoms_dstore_base.storage_component->create_handle(attrs))) {
        /* have our module, so create a new dstore_handle */
        hdl = OBJ_NEW(ocoms_dstore_handle_t);
        if (NULL != name) {
            hdl->name = strdup(name);
        }
        hdl->module = mod;
        if (0 > (index = ocoms_pointer_array_add(&ocoms_dstore_base.handles, hdl))) {
            OCOMS_ERROR_LOG(index);
            OBJ_RELEASE(hdl);
        }
        return index;
    }

    /* if we get here, then we were unable to create a module
     * for this scope
     */
    return OCOMS_ERROR;
}

int ocoms_dstore_base_update(int dstorehandle, ocoms_list_t *attrs)
{
    int rc;

    if (dstorehandle < 0) {
        return OCOMS_ERR_NOT_INITIALIZED;
    }

    if (NULL == ocoms_dstore_base.storage_component->update_handle) {
        return OCOMS_SUCCESS;
    }

    if (OCOMS_SUCCESS != (rc = ocoms_dstore_base.storage_component->update_handle(dstorehandle, attrs))) {
        OCOMS_ERROR_LOG(rc);
    }

    return rc;
}

int ocoms_dstore_base_close(int dstorehandle)
{
    ocoms_dstore_handle_t *hdl;
    int i;

    /* if the handle is -1, then close all handles */
    if (dstorehandle < 0) {
        for (i=0; i < ocoms_dstore_base.handles.size; i++) {
            if (NULL != (hdl = (ocoms_dstore_handle_t*)ocoms_pointer_array_get_item(&ocoms_dstore_base.handles, i))) {
                OBJ_RELEASE(hdl);
                ocoms_pointer_array_set_item(&ocoms_dstore_base.handles, i, NULL);
            }
        }
        return OCOMS_SUCCESS;
    }

    /* get the datastore handle */
    if (NULL == (hdl = (ocoms_dstore_handle_t*)ocoms_pointer_array_get_item(&ocoms_dstore_base.handles, dstorehandle))) {
        return OCOMS_ERR_NOT_FOUND;
    }
    ocoms_pointer_array_set_item(&ocoms_dstore_base.handles, dstorehandle, NULL);
    /* release the handle - this will also finalize and free the module */
    OBJ_RELEASE(hdl);

    return OCOMS_SUCCESS;
}


int ocoms_dstore_base_store(int dstorehandle,
                           const ocoms_identifier_t *id,
                           ocoms_value_t *kv)
{
    ocoms_dstore_handle_t *hdl;

    if (dstorehandle < 0) {
        return OCOMS_ERR_NOT_INITIALIZED;
    }

    if (NULL == (hdl = (ocoms_dstore_handle_t*)ocoms_pointer_array_get_item(&ocoms_dstore_base.handles, dstorehandle))) {
        OCOMS_ERROR_LOG(OCOMS_ERR_NOT_FOUND);
        return OCOMS_ERR_NOT_FOUND;
    }

    ocoms_output_verbose(1, ocoms_dstore_base_framework.framework_output,
                        "storing data in %s dstore", (NULL == hdl->name) ? "NULL" : hdl->name);

    return hdl->module->store((struct ocoms_dstore_base_module_t*)hdl->module, id, kv);
}

int ocoms_dstore_base_fetch(int dstorehandle,
                           const ocoms_identifier_t *id,
                           const char *key,
                           ocoms_list_t *kvs)
{
    ocoms_dstore_handle_t *hdl;
    int rc;

    if (dstorehandle < 0) {
        return OCOMS_ERR_NOT_INITIALIZED;
    }

    if (NULL == (hdl = (ocoms_dstore_handle_t*)ocoms_pointer_array_get_item(&ocoms_dstore_base.handles, dstorehandle))) {
        OCOMS_ERROR_LOG(OCOMS_ERR_NOT_FOUND);
        return OCOMS_ERR_NOT_FOUND;
    }

    ocoms_output_verbose(1, ocoms_dstore_base_framework.framework_output,
                        "fetching data from %s dstore", (NULL == hdl->name) ? "NULL" : hdl->name);

    if (OCOMS_SUCCESS == (rc = hdl->module->fetch((struct ocoms_dstore_base_module_t*)hdl->module, id, key, kvs))) {
        /* found the data, so we can just return it */
        return rc;
    }

    /* if the storage module didn't find it, then let the backfill module try
     * to retrieve it if we have one */
    if (NULL != ocoms_dstore_base.backfill_module) {
        rc = ocoms_dstore_base.backfill_module->fetch((struct ocoms_dstore_base_module_t*)ocoms_dstore_base.backfill_module, id, key, kvs);
    }
    return rc;
}

int ocoms_dstore_base_remove_data(int dstorehandle,
                                 const ocoms_identifier_t *id,
                                 const char *key)
{
    ocoms_dstore_handle_t *hdl;

    if (dstorehandle < 0) {
        return OCOMS_ERR_NOT_INITIALIZED;
    }

    if (NULL == (hdl = (ocoms_dstore_handle_t*)ocoms_pointer_array_get_item(&ocoms_dstore_base.handles, dstorehandle))) {
        OCOMS_ERROR_LOG(OCOMS_ERR_NOT_FOUND);
        return OCOMS_ERR_NOT_FOUND;
    }

    ocoms_output_verbose(1, ocoms_dstore_base_framework.framework_output,
                        "removing data from %s dstore", (NULL == hdl->name) ? "NULL" : hdl->name);

    return hdl->module->remove((struct ocoms_dstore_base_module_t*)hdl->module, id, key);
}


/**
 * Find data for a given key in a given proc_data_t
 * container.
 */
ocoms_value_t* ocoms_dstore_base_lookup_keyval(ocoms_dstore_proc_data_t *proc_data,
                                             const char *key)
{
    ocoms_value_t *kv;

    OCOMS_LIST_FOREACH(kv, &proc_data->data, ocoms_value_t) {
        if (0 == strcmp(key, kv->key)) {
            return kv;
        }
    }
    return NULL;
}


/**
 * Find proc_data_t container associated with given
 * ocoms_identifier_t.
 */
ocoms_dstore_proc_data_t* ocoms_dstore_base_lookup_proc(ocoms_hash_table_t *jtable, ocoms_identifier_t id)
{
    ocoms_dstore_proc_data_t *proc_data = NULL;

    ocoms_hash_table_get_value_uint64(jtable, id, (void**)&proc_data);
    if (NULL == proc_data) {
        /* The proc clearly exists, so create a data structure for it */
        proc_data = OBJ_NEW(ocoms_dstore_proc_data_t);
        if (NULL == proc_data) {
            ocoms_output(0, "dstore:hash:lookup_ocoms_proc: unable to allocate proc_data_t\n");
            return NULL;
        }
        ocoms_hash_table_set_value_uint64(jtable, id, proc_data);
    }
    
    return proc_data;
}

