/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ccs_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if 0
#include "service/runtime/opal.h"
#endif
#include "service/util/service_list.h"
#include "service/util/output.h"
#include "service/mca/mca.h"
#include "service/mca/base/base.h"
#include "service/mca/base/mca_base_component_repository.h"
#include "service/platform/service_constants.h"


int ccs_mca_base_select(const char *type_name, int output_id,
                    service_list_t *components_available,
                    ccs_mca_base_module_t **best_module,
                    ccs_mca_base_component_t **best_component)
{
    ccs_mca_base_component_list_item_t *cli = NULL;
    ccs_mca_base_component_t *component = NULL;
    ccs_mca_base_module_t *module = NULL;
    service_list_item_t *item = NULL;
    int priority = 0, best_priority = INT32_MIN;

    *best_module = NULL;
    *best_component = NULL;

    service_output_verbose(10, output_id,
                        "mca:base:select: Auto-selecting %s components",
                        type_name);

    /*
     * Traverse the list of available components.
     * For each call their 'query' functions to determine relative priority.
     */
    for (item  = service_list_get_first(components_available);
         item != service_list_get_end(components_available);
         item  = service_list_get_next(item) ) {
        cli = (ccs_mca_base_component_list_item_t *) item;
        component = (ccs_mca_base_component_t *) cli->cli_component;

        /*
         * If there is a query function then use it.
         */
        if (NULL == component->mca_query_component) {
            service_output_verbose(5, output_id,
                                "mca:base:select:(%5s) Skipping component [%s]. It does not implement a query function",
                                type_name, component->mca_component_name );
            continue;
        }

        /*
         * Query this component for the module and priority
         */
        service_output_verbose(5, output_id,
                            "mca:base:select:(%5s) Querying component [%s]",
                            type_name, component->mca_component_name);

        component->mca_query_component(&module, &priority);

        /*
         * If no module was returned, then skip component
         */
        if (NULL == module) {
            service_output_verbose(5, output_id,
                                "mca:base:select:(%5s) Skipping component [%s]. Query failed to return a module",
                                type_name, component->mca_component_name );
            continue;
        }

        /*
         * Determine if this is the best module we have seen by looking the priority
         */
        service_output_verbose(5, output_id,
                            "mca:base:select:(%5s) Query of component [%s] set priority to %d", 
                            type_name, component->mca_component_name, priority);
        if (priority > best_priority) {
            best_priority  = priority;
            *best_component = component;
            *best_module    = module;
        }
    }


    /*
     * Finished querying all components.
     * Make sure we found something in the process.
     */
    if (NULL == *best_component) {
        service_output_verbose(5, output_id,
                            "mca:base:select:(%5s) No component selected!",
                            type_name);
        /*
         * Still close the non-selected components
         */
        ccs_mca_base_components_close(0, /* Pass 0 to keep this from closing the output handle */
                                  components_available,
                                  NULL);
        return CCS_ERR_NOT_FOUND;
    }

    service_output_verbose(5, output_id,
                        "mca:base:select:(%5s) Selected component [%s]",
                        type_name, (*best_component)->mca_component_name);
#if 0 /* Pasha: Not sure if we need this one */
    if (ccs_profile) {
        service_output(0, "%s:%s", type_name, (*best_component)->mca_component_name);
    }
#endif
    
    /*
     * Close the non-selected components
     */
    ccs_mca_base_components_close(output_id,
                              components_available,
                              (ccs_mca_base_component_t *) (*best_component));


    return CCS_SUCCESS;
}
