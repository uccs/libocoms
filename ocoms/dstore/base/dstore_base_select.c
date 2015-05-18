/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved. 
 * Copyright (c) 2013-2014 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms_config.h"
#include "ocoms/platform/ocoms_constants.h"

#include "ocoms/util/ocoms_list.h"
#include "ocoms/mca/mca.h"
#include "ocoms/mca/base/base.h"
#include "ocoms/mca/base/mca_base_component_repository.h"
#include "ocoms/util/output.h"

#include "ocoms/dstore/base/base.h"

static bool selected = false;

int 
ocoms_dstore_base_select(void)
{
    ocoms_mca_base_component_list_item_t *cli;
    ocoms_mca_base_component_t *cmp;
    ocoms_mca_base_module_t *md;
    int priority, cmp_pri, mod_pri;
    ocoms_dstore_base_module_t *mod=NULL;
    ocoms_dstore_base_component_t *comp=NULL;

    if (selected) {
        /* ensure we don't do this twice */
        return OCOMS_SUCCESS;
    }
    selected = true;
    
    /* Query all available components and ask if they have a module */
    cmp_pri = -100000;
    mod_pri = -100000;
    OCOMS_LIST_FOREACH(cli, &ocoms_dstore_base_framework.framework_components, ocoms_mca_base_component_list_item_t) {
        cmp = (ocoms_mca_base_component_t*)cli->cli_component;

        ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                            "mca:dstore:select: checking available component %s",
                            cmp->mca_component_name);

        /* If there's no query function, skip it */
        if (NULL == cmp->mca_query_component) {
            ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                                "mca:dstore:select: Skipping component [%s]. It does not implement a query function",
                                cmp->mca_component_name );
            continue;
        }

        /* Query the component */
        ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                            "mca:dstore:select: Querying component [%s]",
                            cmp->mca_component_name);

        /* If the component reports failure, then skip component - however,
         * it is okay to return a NULL module */
        if (OCOMS_SUCCESS != cmp->mca_query_component(&md, &priority)) {
            ocoms_output_verbose(5, ocoms_dstore_base_framework.framework_output,
                                "mca:dstore:select: Skipping component [%s] - not available",
                                cmp->mca_component_name );
            continue;
        }

        /* track the highest priority component that returned a NULL module - this
         * will become our storage element */
        if (NULL == md) {
            if (0 < priority && priority > cmp_pri) {
                comp = (ocoms_dstore_base_component_t*)cmp;
                cmp_pri = priority;
            }
        } else {
            /* track the highest priority module that was returned - this
             * will become our backfill element */
            if (priority > mod_pri) {
                mod = (ocoms_dstore_base_module_t*)md;
                mod_pri = priority;
            }
        }
    }

    if (NULL == comp) {
        /* no components available - that's bad */
        return OCOMS_ERROR;
    }
    ocoms_dstore_base.storage_component = comp;

    /* it's okay not to have a backfill module */
    ocoms_dstore_base.backfill_module = mod;

    return OCOMS_SUCCESS;;
}
