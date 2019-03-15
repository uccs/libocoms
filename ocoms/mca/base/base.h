/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
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

#ifndef MCA_BASE_H
#define MCA_BASE_H

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/util/ocoms_object.h"
#include "ocoms/util/ocoms_list.h"

/*
 * These units are large enough to warrant their own .h files
 */
#include "ocoms/mca/mca.h"
#include "ocoms/mca/base/mca_base_var.h"
#include "ocoms/mca/base/mca_base_framework.h"
#include "ocoms/util/cmd_line.h"

BEGIN_C_DECLS

/*
 * Structure for making plain lists of components
 */
struct ocoms_mca_base_component_list_item_t {
    ocoms_list_item_t super;
    const ocoms_mca_base_component_t *cli_component;
};
typedef struct ocoms_mca_base_component_list_item_t ocoms_mca_base_component_list_item_t;
OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_mca_base_component_list_item_t);

/*
 * Structure for making priority lists of components
 */
struct ocoms_mca_base_component_priority_list_item_t {
    ocoms_mca_base_component_list_item_t super;
    int cpli_priority;
};
typedef struct ocoms_mca_base_component_priority_list_item_t 
    ocoms_mca_base_component_priority_list_item_t;

OCOMS_DECLSPEC OBJ_CLASS_DECLARATION(ocoms_mca_base_component_priority_list_item_t);


/*
 * Public variables
 */
OCOMS_DECLSPEC extern char *ocoms_mca_base_component_path;
OCOMS_DECLSPEC extern bool ocoms_mca_base_component_show_load_errors;
OCOMS_DECLSPEC extern bool ocoms_mca_base_component_disable_dlopen;
OCOMS_DECLSPEC extern char *ocoms_mca_base_system_default_path;
OCOMS_DECLSPEC extern char *ocoms_mca_base_user_default_path;

/*
 * Public functions
 */

/**
 * First function called in the MCA.
 *
 * @return OCOMS_SUCCESS Upon success
 * @return OCOMS_ERROR Upon failure
 * 
 * This function starts up the entire MCA.  It initializes a bunch
 * of built-in MCA parameters, and initialized the MCA component
 * repository.
 *
 * It must be the first MCA function invoked.  It is normally
 * invoked during ompi_mpi_init() and specifically invoked in the
 * special case of the laminfo command.
 */
OCOMS_DECLSPEC int ocoms_mca_base_open(ocoms_mca_ocoms_install_dirs_t install_dirs);

/**
 * Last function called in the MCA
 *
 * @return OCOMS_SUCCESS Upon success
 * @return OCOMS_ERROR Upon failure
 *
 * This function closes down the entire MCA.  It clears all MCA
 * parameters and closes down the MCA component respository.  
 *
 * It must be the last MCA function invoked.  It is normally invoked
 * during ompi_mpi_finalize() and specifically invoked during the
 * special case of the laminfo command.
 */
OCOMS_DECLSPEC int ocoms_mca_base_close(void);

/**
 * A generic select function
 *
 */
OCOMS_DECLSPEC int ocoms_mca_base_select(const char *type_name, int output_id,
                                  ocoms_list_t *components_available,
                                  ocoms_mca_base_module_t **best_module,
                                  ocoms_mca_base_component_t **best_component);

/**
 * A function for component query functions to discover if they have
 * been explicitly required to or requested to be selected.
 *
 * exclusive: If the specified component is the only component that is
 *            available for selection.
 *
 */
OCOMS_DECLSPEC int ocoms_mca_base_is_component_required(ocoms_list_t *components_available,
                                                 ocoms_mca_base_component_t *component,
                                                 bool exclusive,
                                                 bool *is_required);

/* ocoms_mca_base_cmd_line.c */

OCOMS_DECLSPEC int ocoms_mca_base_cmd_line_setup(ocoms_cmd_line_t *cmd);
OCOMS_DECLSPEC int ocoms_mca_base_cmd_line_process_args(ocoms_cmd_line_t *cmd,
                                                 char ***app_env,
                                                 char ***global_env);

/* ocoms_mca_base_component_compare.c */

OCOMS_DECLSPEC int ocoms_mca_base_component_compare_priority(ocoms_mca_base_component_priority_list_item_t *a,
                                                      ocoms_mca_base_component_priority_list_item_t *b);
OCOMS_DECLSPEC int ocoms_mca_base_component_compare(const ocoms_mca_base_component_t *a,
                                             const ocoms_mca_base_component_t *b);
OCOMS_DECLSPEC int ocoms_mca_base_component_compatible(const ocoms_mca_base_component_t *a,
                                                const ocoms_mca_base_component_t *b);
OCOMS_DECLSPEC char * ocoms_mca_base_component_to_string(const ocoms_mca_base_component_t *a);

/* ocoms_mca_base_component_find.c */

OCOMS_DECLSPEC int ocoms_mca_base_component_find(const char *directory, const char *type,
                                                 const ocoms_mca_base_component_t *static_components[],
                                                 const char *requested_components,
                                                 ocoms_list_t *found_components,
                                                 bool open_dso_components, ocoms_mca_base_open_flag_t flags);

/**
 * Parse the requested component string and return an ocoms_argv of the requested
 * (or not requested) components.
 */
int ocoms_mca_base_component_parse_requested (const char *requested, bool *include_mode,
                                        char ***requested_component_names);

/**
 * Filter a list of components based on a comma-delimted list of names and/or
 * a set of meta-data flags.
 *
 * @param[in,out] components List of components to filter
 * @param[in] output_id Output id to write to for error/warning/debug messages
 * @param[in] filter_names Comma delimited list of components to use. Negate with ^.
 * May be NULL.
 * @param[in] filter_flags Metadata flags components are required to have set (CR ready)
 *
 * @returns OCOMS_SUCCESS On success
 * @returns OCOMS_ERR_NOT_FOUND If some component in {filter_names} is not found in
 * {components}. Does not apply to negated filters.
 * @returns ocoms error code On other error.
 *
 * This function closes and releases any components that do not match the filter_name and
 * filter flags.
 */
OCOMS_DECLSPEC int ocoms_mca_base_components_filter (const char *framework_name, ocoms_list_t *components, int output_id,
                                                     const char *filter_names, uint32_t filter_flags,
                                                     ocoms_mca_base_open_flag_t flags);



/* Safely release some memory allocated by ocoms_mca_base_component_find()
   (i.e., is safe to call even if you never called
   ocoms_mca_base_component_find()). */
OCOMS_DECLSPEC int ocoms_mca_base_component_find_finalize(void);

/* ocoms_mca_base_components_register.c */
OCOMS_DECLSPEC int ocoms_mca_base_framework_components_register (struct ocoms_mca_base_framework_t *framework,
                                                          ocoms_mca_base_register_flag_t flags);

/* ocoms_mca_base_components_open.c */
OCOMS_DECLSPEC int ocoms_mca_base_framework_components_open (struct ocoms_mca_base_framework_t *framework,
                                                      ocoms_mca_base_open_flag_t flags);

OCOMS_DECLSPEC int ocoms_mca_base_components_open(const char *type_name, int output_id,
                                           const ocoms_mca_base_component_t **static_components,
                                           ocoms_list_t *components_available,
                                           bool open_dso_components);

/* ocoms_mca_base_components_close.c */
/**
 * Close and release a component.
 *
 * @param[in] component Component to close
 * @param[in] output_id Output id for debugging output
 *
 * After calling this function the component may no longer be used.
 */
OCOMS_DECLSPEC void ocoms_mca_base_component_close (const ocoms_mca_base_component_t *component, int output_id);

/**
 * Release a component without closing it.
 * @param[in] component Component to close
 * @param[in] output_id Output id for debugging output
 *
 * After calling this function the component may no longer be used.
 */
void ocoms_mca_base_component_unload (const ocoms_mca_base_component_t *component, int output_id);

OCOMS_DECLSPEC int ocoms_mca_base_components_close(int output_id, ocoms_list_t *components_available, 
                                            const ocoms_mca_base_component_t *skip);

OCOMS_DECLSPEC int ocoms_mca_base_framework_components_close (struct ocoms_mca_base_framework_t *framework,
						       const ocoms_mca_base_component_t *skip);

OCOMS_DECLSPEC void ocoms_mca_base_set_component_template(char *template);
END_C_DECLS

#endif /* MCA_BASE_H */
