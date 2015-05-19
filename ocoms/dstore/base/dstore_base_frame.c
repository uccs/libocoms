/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved. 
 * Copyright (c) 2014      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#include "ocoms_config.h"
#include "ocoms/platform/ocoms_constants.h"

#include "ocoms/mca/mca.h"
#include "ocoms/util/output.h"
#include "ocoms/mca/base/base.h"
#include "ocoms/datatype/ocoms_datatype.h"

#include "ocoms/dstore/base/base.h"


/*
 * The following file was created by configure.  It contains extern
 * dstorements and the definition of an array of pointers to each
 * module's public mca_base_module_t struct.
 */

#include "ocoms/dstore/base/static-components.h"

ocoms_dstore_base_API_t ocoms_dstore = {
    ocoms_dstore_base_open,
    ocoms_dstore_base_update,
    ocoms_dstore_base_close,
    ocoms_dstore_base_store,
    ocoms_dstore_base_fetch,
    ocoms_dstore_base_remove_data
};
ocoms_dstore_base_t ocoms_dstore_base;

int ocoms_dstore_internal = -1;

static int ocoms_dstore_base_frame_close(void)
{
    ocoms_dstore_handle_t *hdl;
    int i;

    /* cycle across all the active dstore handles and let them cleanup - order
     * doesn't matter in this case
     */
    for (i=0; i < ocoms_dstore_base.handles.size; i++) {
        if (NULL != (hdl = (ocoms_dstore_handle_t*)ocoms_pointer_array_get_item(&ocoms_dstore_base.handles, i))) {
            OBJ_RELEASE(hdl);
        }
    }
    OBJ_DESTRUCT(&ocoms_dstore_base.handles);

    /* let the backfill module finalize, should it wish to do so */
    if (NULL != ocoms_dstore_base.backfill_module && NULL != ocoms_dstore_base.backfill_module->finalize) {
        ocoms_dstore_base.backfill_module->finalize((struct ocoms_dstore_base_module_t*)ocoms_dstore_base.backfill_module);
    }

    return ocoms_mca_base_framework_components_close(&ocoms_dstore_base_framework, NULL);
}

static int ocoms_dstore_base_frame_open(ocoms_mca_base_open_flag_t flags)
{
    OBJ_CONSTRUCT(&ocoms_dstore_base.handles, ocoms_pointer_array_t);
    ocoms_pointer_array_init(&ocoms_dstore_base.handles, 3, INT_MAX, 1);

    /* Open up all available components */
    return ocoms_mca_base_framework_components_open(&ocoms_dstore_base_framework, flags);
}

MCA_BASE_FRAMEWORK_DECLARE(ocoms, dstore, NULL, NULL,
                           ocoms_dstore_base_frame_open,
                           ocoms_dstore_base_frame_close,
                           mca_dstore_base_static_components, 0);

/***  CLASS INSTANCES   ***/
static void hdl_con(ocoms_dstore_handle_t *p)
{
    p->name = NULL;
    p->module = NULL;
}
static void hdl_des(ocoms_dstore_handle_t *p)
{
    ocoms_dstore_base_module_t *mod;

    if (NULL != p->name) {
        free(p->name);
    }
    if (NULL != p->module) {
        mod = (ocoms_dstore_base_module_t*)p->module;
        if (NULL != mod->finalize) {
            mod->finalize((struct ocoms_dstore_base_module_t*)mod);
        }
        free(mod);
    }
}
OBJ_CLASS_INSTANCE(ocoms_dstore_handle_t,
                   ocoms_object_t,
                   hdl_con, hdl_des);

static void proc_data_construct(ocoms_dstore_proc_data_t *ptr)
{
    ptr->loaded = false;
    OBJ_CONSTRUCT(&ptr->data, ocoms_list_t);
}

static void proc_data_destruct(ocoms_dstore_proc_data_t *ptr)
{
    OCOMS_LIST_DESTRUCT(&ptr->data);
}
OBJ_CLASS_INSTANCE(ocoms_dstore_proc_data_t,
                   ocoms_list_item_t,
                   proc_data_construct,
                   proc_data_destruct);


