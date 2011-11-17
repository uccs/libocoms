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

#include "ccs_config.h"

#include <stdio.h>
#include <string.h>

#include "opal/class/ccs_list.h"
#include "service/mca/mca.h"
#include "service/mca/base/base.h"
#include "opal/mca/base/mca_base_param_internal.h"
#include "service/util/keyval_parse.h"

static void save_value(const char *name, const char *value);

static char * file_being_read;

int mca_base_parse_paramfile(const char *paramfile)
{
    file_being_read = (char*)paramfile;
    
    return ccs_util_keyval_parse(paramfile, save_value);
}

static void save_value(const char *name, const char *value)
{
    service_list_item_t *item;
    mca_base_param_file_value_t *fv;

    /* First traverse through the list and ensure that we don't
       already have a param of this name.  If we do, just replace the
       value. */

    for (item = service_list_get_first(&mca_base_param_file_values);
         service_list_get_end(&mca_base_param_file_values) != item;
         item = service_list_get_next(item)) {
        fv = (mca_base_param_file_value_t *) item;
        if (0 == strcmp(name, fv->mbpfv_param)) {
            if (NULL != fv->mbpfv_value ) {
                free(fv->mbpfv_value);
            }
            if (NULL != value) {
                fv->mbpfv_value = strdup(value);
            } else {
                fv->mbpfv_value = NULL;
            }
            fv->mbpfv_file = strdup(file_being_read);
            return;
        }
    }

    /* We didn't already have the param, so append it to the list */

    fv = OBJ_NEW(mca_base_param_file_value_t);
    if (NULL != fv) {
        fv->mbpfv_param = strdup(name);
        if (NULL != value) {
            fv->mbpfv_value = strdup(value);
        } else {
            fv->mbpfv_value = NULL;
        }
        fv->mbpfv_file = strdup(file_being_read);
        service_list_append(&mca_base_param_file_values, (service_list_item_t*) fv);
    }
}
