/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2006 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2013      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/output.h"
#include "ocoms/mca/mca.h"
#include "ocoms/mca/base/base.h"
#include "ocoms/mca/base/mca_base_component_repository.h"
#include "ocoms/platform/ocoms_constants.h"

void ocoms_mca_base_component_unload (const ocoms_mca_base_component_t *component, int output_id)
{
    int ret;

    /* Unload */
    ocoms_output_verbose(10, output_id, 
                        "mca: base: close: unloading component %s",
                        component->mca_component_name);

    /* XXX -- TODO -- Replace reserved by mca_project_name for 1.9 */
    ret = ocoms_mca_base_var_group_find (component->reserved, component->mca_type_name,
                                   component->mca_component_name);
    if (0 <= ret) {
        ocoms_mca_base_var_group_deregister (ret);
    }

    ocoms_mca_base_component_repository_release((ocoms_mca_base_component_t *) component);    
}

void ocoms_mca_base_component_close (const ocoms_mca_base_component_t *component, int output_id)
{
    /* Close */
    if (NULL != component->mca_close_component) {
        component->mca_close_component();
        ocoms_output_verbose(10, output_id, 
                            "mca: base: close: component %s closed",
                            component->mca_component_name);
    }

    ocoms_mca_base_component_unload (component, output_id);
}

int ocoms_mca_base_framework_components_close (ocoms_mca_base_framework_t *framework,
                                         const ocoms_mca_base_component_t *skip)
{
    return ocoms_mca_base_components_close (framework->framework_output,
                                      &framework->framework_components,
                                      skip);
}

int ocoms_mca_base_components_close(int output_id, ocoms_list_t *components, 
                              const ocoms_mca_base_component_t *skip)
{
    ocoms_mca_base_component_list_item_t *cli, *next;

    /* Close and unload all components in the available list, except the
       "skip" item.  This is handy to close out all non-selected
       components.  It's easier to simply remove the entire list and
       then simply re-add the skip entry when done. */

    OCOMS_LIST_FOREACH_SAFE(cli, next, components, ocoms_mca_base_component_list_item_t) {
        if (skip == cli->cli_component) {
            continue;
        }

        ocoms_mca_base_component_close (cli->cli_component, output_id);
        ocoms_list_remove_item (components, &cli->super);

        OBJ_RELEASE(cli);
    }

    /* All done */
    return OCOMS_SUCCESS;
}
