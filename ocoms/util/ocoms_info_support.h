/*
 * Copyright (c) 2012-2013 Los Alamos National Security, LLC.
 *                         All rights reserved.
* $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/** @file **/

#ifndef OCOMS_INFO_REGISTER_H
#define OCOMS_INFO_REGISTER_H

#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/ocoms_pointer_array.h"
#include "ocoms/util/cmd_line.h"
#include "ocoms/mca/base/base.h"

OCOMS_DECLSPEC int ocoms_info_init(int argc, char **argv,
                                 ocoms_cmd_line_t *ocoms_info_cmd_line);

OCOMS_DECLSPEC void ocoms_info_finalize(void);

OCOMS_DECLSPEC void ocoms_info_do_params(bool want_all_in, bool want_internal,
                                       ocoms_pointer_array_t *mca_type,
                                       ocoms_cmd_line_t *ocoms_info_cmd_line);

OCOMS_DECLSPEC void ocoms_info_show_mca_params(const char *type,
                                             const char *component, 
                                             ocoms_mca_base_var_info_lvl_t max_level,
                                             bool want_internal);

OCOMS_DECLSPEC void ocoms_info_out(const char *pretty_message, const char *plain_message, const char *value);

#endif
