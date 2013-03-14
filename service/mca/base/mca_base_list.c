/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "service/platform/ocoms_config.h"

#include "service/util/service_list.h"
#include "service/mca/base/base.h"


/* 
 * Local functions
 */
static void cl_constructor(service_object_t *obj);
static void cpl_constructor(service_object_t *obj);


/*
 * Class instance of the ocoms_mca_base_component_list_item_t class
 */
OBJ_CLASS_INSTANCE(ocoms_mca_base_component_list_item_t, 
                   service_list_item_t, cl_constructor, NULL);


/*
 * Class instance of the ocoms_mca_base_component_priority_list_item_t class
 */
OBJ_CLASS_INSTANCE(ocoms_mca_base_component_priority_list_item_t, 
                   ocoms_mca_base_component_list_item_t, cpl_constructor, NULL);


/*
 * Just do basic sentinel intialization
 */
static void cl_constructor(service_object_t *obj)
{
  ocoms_mca_base_component_list_item_t *cli = (ocoms_mca_base_component_list_item_t *) obj;
  cli->cli_component = NULL;
}


/*
 * Just do basic sentinel intialization
 */
static void cpl_constructor(service_object_t *obj)
{
  ocoms_mca_base_component_priority_list_item_t *cpli = 
    (ocoms_mca_base_component_priority_list_item_t *) obj;
  cpli->cpli_priority = -1;
}
