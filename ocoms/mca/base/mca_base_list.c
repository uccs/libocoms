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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/util/ocoms_list.h"
#include "ocoms/mca/base/base.h"


/* 
 * Local functions
 */
static void cl_constructor(ocoms_object_t *obj);
static void cpl_constructor(ocoms_object_t *obj);


/*
 * Class instance of the ocoms_mca_base_component_list_item_t class
 */
OBJ_CLASS_INSTANCE(ocoms_mca_base_component_list_item_t, 
                   ocoms_list_item_t, cl_constructor, NULL);


/*
 * Class instance of the ocoms_mca_base_component_priority_list_item_t class
 */
OBJ_CLASS_INSTANCE(ocoms_mca_base_component_priority_list_item_t, 
                   ocoms_mca_base_component_list_item_t, cpl_constructor, NULL);


/*
 * Just do basic sentinel intialization
 */
static void cl_constructor(ocoms_object_t *obj)
{
  ocoms_mca_base_component_list_item_t *cli = (ocoms_mca_base_component_list_item_t *) obj;
  cli->cli_component = NULL;
}


/*
 * Just do basic sentinel intialization
 */
static void cpl_constructor(ocoms_object_t *obj)
{
  ocoms_mca_base_component_priority_list_item_t *cpli = 
    (ocoms_mca_base_component_priority_list_item_t *) obj;
  cpli->cpli_priority = -1;
}
