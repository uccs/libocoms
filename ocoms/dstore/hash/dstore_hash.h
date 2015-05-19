/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2014      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OCOMS_DSTORE_HASH_H
#define OCOMS_DSTORE_HASH_H

#include "ocoms/util/ocoms_hash_table.h"
#include "ocoms/dstore/dstore.h"

BEGIN_C_DECLS


OCOMS_MODULE_DECLSPEC extern ocoms_dstore_base_component_t mca_dstore_hash_component;

typedef struct {
    ocoms_dstore_base_module_t api;
    ocoms_hash_table_t hash_data;
} mca_dstore_hash_module_t;
OCOMS_MODULE_DECLSPEC extern mca_dstore_hash_module_t ocoms_dstore_hash_module;

END_C_DECLS

#endif /* OCOMS_DSTORE_HASH_H */
