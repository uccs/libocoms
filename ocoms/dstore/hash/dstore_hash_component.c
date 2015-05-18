/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2013-2014 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 * These symbols are in a file by themselves to provide nice linker
 * semantics.  Since linkers generally pull in symbols by object
 * files, keeping these symbols as the only symbols in this file
 * prevents utility programs such as "ompi_info" from having to import
 * entire components just to query their version and parameters.
 */

#include "ocoms_config.h"
#include "ocoms/platform/ocoms_constants.h"

#include "ocoms/mca/base/base.h"
#include "ocoms/util/error.h"

#include "ocoms/dstore/dstore.h"
#include "ocoms/dstore/base/base.h"
#include "dstore_hash.h"

static ocoms_dstore_base_module_t *component_create(ocoms_list_t *attrs);
static int dstore_hash_query(ocoms_mca_base_module_t **module, int *priority);

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */
ocoms_dstore_base_component_t mca_dstore_hash_component = {
    {
        OCOMS_DSTORE_BASE_VERSION_2_0_0,

        /* Component name and version */
        "hash",
        1, //OCOMS_MAJOR_VERSION,
        0, //OCOMS_MINOR_VERSION,
        0, //OCOMS_RELEASE_VERSION,

        /* Component open and close functions */
        NULL,
        NULL,
        dstore_hash_query,
        NULL
    },
    {
        /* The component is checkpoint ready */
        MCA_BASE_METADATA_PARAM_CHECKPOINT
    },
    component_create,
    NULL,
    NULL
};

static int dstore_hash_query(ocoms_mca_base_module_t **module, int *priority)
{
    /* we are always available, but only as storage */
    *priority = 80;
    *module = NULL;
    return OCOMS_SUCCESS;
}

/* this component ignores any input attributes */
static ocoms_dstore_base_module_t *component_create(ocoms_list_t *attrs)
{
    mca_dstore_hash_module_t *mod;

    mod = (mca_dstore_hash_module_t*)malloc(sizeof(mca_dstore_hash_module_t));
    if (NULL == mod) {
        OCOMS_ERROR_LOG(OCOMS_ERR_OUT_OF_RESOURCE);
        return NULL;
    }
    /* copy the APIs across */
    memcpy(mod, &ocoms_dstore_hash_module.api, sizeof(ocoms_dstore_base_module_t));
    /* let the module init itself */
    if (OCOMS_SUCCESS != mod->api.init((struct ocoms_dstore_base_module_t*)mod)) {
        /* release the module and return the error */
        free(mod);
        return NULL;
    }
    return (ocoms_dstore_base_module_t*)mod;
}
