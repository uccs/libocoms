/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2013 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2008-2012 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011-2013 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/argv.h"
#include "ocoms/util/output.h"
#include "ocoms/mca/mca.h"
#include "ocoms/mca/base/base.h"
#include "ocoms/platform/ocoms_constants.h"

/*
 * Local functions
 */
static int open_components(ocoms_mca_base_framework_t *framework);

struct ocoms_mca_base_dummy_framework_list_item_t {
    ocoms_list_item_t super;
    ocoms_mca_base_framework_t framework;
};

/**
 * Function for finding and opening either all MCA components, or the
 * one that was specifically requested via a MCA parameter.
 */
int ocoms_mca_base_framework_components_open (ocoms_mca_base_framework_t *framework,
                                        ocoms_mca_base_open_flag_t flags)
{
    /* Open flags are not used at this time. Suppress compiler warning. */
    if (flags & MCA_BASE_OPEN_FIND_COMPONENTS) {
        /* Find and load requested components */
        int ret = ocoms_mca_base_component_find(NULL, framework->framework_name,
                                          framework->framework_static_components,
                                          framework->framework_selection,
                                          &framework->framework_components, true);
        if (OCOMS_SUCCESS != ret) {
            return ret;
        }
    }
    /* Open all registered components */
    return open_components (framework);
}

int ocoms_mca_base_components_open (const char *type_name, int output_id,
			      const ocoms_mca_base_component_t **static_components,
			      ocoms_list_t *components_available,
			      bool open_dso_components)
{
    /* create a dummy framework --  this leaks -- i know -- but it is temporary */
    ocoms_mca_base_register_flag_t register_flags;
    ocoms_mca_base_framework_t *dummy_framework;
    ocoms_list_item_t *item;
    int ret;

    dummy_framework = calloc (1, sizeof(*dummy_framework));

    dummy_framework->framework_static_components = static_components;
    dummy_framework->framework_output = output_id;
    dummy_framework->framework_name   = strdup(type_name);

    if (open_dso_components) {
        register_flags = MCA_BASE_REGISTER_STATIC_ONLY;
    } else {
        register_flags = MCA_BASE_REGISTER_DEFAULT;
     }


    ret = ocoms_mca_base_framework_components_register (dummy_framework, register_flags);
    if (OCOMS_SUCCESS != ret) {
        free (dummy_framework);
        return ret;
     }

    ret = ocoms_mca_base_framework_components_open (dummy_framework, 0);
    if (OCOMS_SUCCESS != ret) {
        (void) ocoms_mca_base_framework_components_close (dummy_framework, NULL);
        free (dummy_framework);
        return ret;
    }

    OBJ_CONSTRUCT(components_available, ocoms_list_t);

    while (NULL != (item = ocoms_list_remove_first(&dummy_framework->framework_components))) {
        ocoms_list_append(components_available, item);
    }

    OBJ_DESTRUCT(&dummy_framework->framework_components);
     return OCOMS_SUCCESS;
}


/*
 * Traverse the entire list of found components (a list of
 * ocoms_mca_base_component_t instances).  If the requested_component_names
 * array is empty, or the name of each component in the list of found
 * components is in the requested_components_array, try to open it.
 * If it opens, add it to the components_available list.
 */
static int open_components(ocoms_mca_base_framework_t *framework)
{
    ocoms_list_t *components = &framework->framework_components;
    uint32_t open_only_flags = MCA_BASE_METADATA_PARAM_NONE;
    int output_id = framework->framework_output;
    ocoms_mca_base_component_list_item_t *cli, *next;
    int ret;

    /*
     * Pre-process the list with parameter constraints
     * e.g., If requested to select only CR enabled components
     *       then only make available those components.
     *
     * JJH Note: Currently checkpoint/restart is the only user of this
     *           functionality. If other component constraint options are
     *           added, then this logic can be used for all contraint
     *           options.
     *
     * NTH: Logic moved to ocoms_mca_base_components_filter.
     */
#if (OCOMS_ENABLE_FT == 1) && (OCOMS_ENABLE_FT_CR == 1)
    if (ocoms_mca_base_component_distill_checkpoint_ready) {
        open_only_flags |= MCA_BASE_METADATA_PARAM_CHECKPOINT;
    }
#endif  /* (OCOMS_ENABLE_FT == 1) && (OCOMS_ENABLE_FT_CR == 1) */

    /* If ocoms_mca_base_framework_register_components was called with the MCA_BASE_COMPONENTS_ALL flag 
       we need to trim down and close any extra components we do not want open */
    ret = ocoms_mca_base_components_filter (framework->framework_name, &framework->framework_components,
                                      framework->framework_output, framework->framework_selection,
                                      open_only_flags);
    if (OCOMS_SUCCESS != ret) {
        return ret;
    }

    
    /* Announce */
    ocoms_output_verbose(10, output_id,
                        "mca: base: components_open: opening %s components",
                        framework->framework_name);

    /* Traverse the list of components */
    OCOMS_LIST_FOREACH_SAFE(cli, next, components, ocoms_mca_base_component_list_item_t) {
        const ocoms_mca_base_component_t *component = cli->cli_component;
    
        
        ocoms_output_verbose(10, output_id, 
                            "mca: base: components_open: found loaded component %s",
                            component->mca_component_name);

	if (NULL != component->mca_open_component) {
	    /* Call open if register didn't call it already */
            ret = component->mca_open_component();
            if (OCOMS_SUCCESS == ret) {
                ocoms_output_verbose(10, output_id, 
                                    "mca: base: components_open: "
                                    "component %s open function successful",
                                    component->mca_component_name);
            } else {
		if (OCOMS_ERR_NOT_AVAILABLE != ret) {
		    /* If the component returns OCOMS_ERR_NOT_AVAILABLE,
		       it's a cue to "silently ignore me" -- it's not a
		       failure, it's just a way for the component to say
		       "nope!".  

		       Otherwise, however, display an error.  We may end
		       up displaying this twice, but it may go to separate
		       streams.  So better to be redundant than to not
		       display the error in the stream where it was
		       expected. */
		    if (ocoms_mca_base_component_show_load_errors) {
			ocoms_output(0, "mca: base: components_open: "
				    "component %s / %s open function failed",
				    component->mca_type_name,
				    component->mca_component_name);
		    }
		    ocoms_output_verbose(10, output_id, 
					"mca: base: components_open: "
					"component %s open function failed",
					component->mca_component_name);
		}

                ocoms_mca_base_component_close (component, output_id);

		ocoms_list_remove_item (components, &cli->super);
		OBJ_RELEASE(cli);
	    }
	}
    }
    
    /* All done */
    
    return OCOMS_SUCCESS;
}
