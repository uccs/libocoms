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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef MCA_BASE_H
#define MCA_BASE_H

#include "service/platform/ocoms_config.h"

#include "service/util/service_object.h"
#include "service/util/service_list.h"

/*
 * These units are large enough to warrant their own .h files
 */
#include "service/mca/mca.h"
#include "service/mca/base/mca_base_param.h"
#include "service/util/cmd_line.h"

BEGIN_C_DECLS

/*
 * Structure for making plain lists of components
 */
struct ocoms_mca_base_component_list_item_t {
    service_list_item_t super;
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
OCOMS_DECLSPEC extern int ocoms_mca_base_param_component_path;
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
OCOMS_DECLSPEC int ocoms_mca_base_open(ocoms_mca_service_install_dirs_t install_dirs);

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
                                  service_list_t *components_available,
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
OCOMS_DECLSPEC int ocoms_mca_base_is_component_required(service_list_t *components_available,
                                                 ocoms_mca_base_component_t *component,
                                                 bool exclusive,
                                                 bool *is_required);

/* ocoms_mca_base_cmd_line.c */

OCOMS_DECLSPEC int ocoms_mca_base_cmd_line_setup(service_cmd_line_t *cmd);
OCOMS_DECLSPEC int ocoms_mca_base_cmd_line_process_args(service_cmd_line_t *cmd,
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
                                          char **requested_component_names,
                                          bool include_mode,
                                          service_list_t *found_components,
                                          bool open_dso_components);

/* Safely release some memory allocated by ocoms_mca_base_component_find()
   (i.e., is safe to call even if you never called
   ocoms_mca_base_component_find()). */
OCOMS_DECLSPEC int ocoms_mca_base_component_find_finalize(void);

/* ocoms_mca_base_components_open.c */

OCOMS_DECLSPEC int ocoms_mca_base_components_open(const char *type_name, int output_id,
                                           const ocoms_mca_base_component_t **static_components,
                                           service_list_t *components_available,
                                           bool open_dso_components);

/* ocoms_mca_base_components_close.c */

OCOMS_DECLSPEC int ocoms_mca_base_components_close(int output_id, service_list_t *components_available, 
                                            const ocoms_mca_base_component_t *skip);

END_C_DECLS

#endif /* MCA_BASE_H */
