/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved. 
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved. 
 * Copyright (c) 2013-2014 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
/** @file:
 */

#ifndef MCA_DSTORE_BASE_H
#define MCA_DSTORE_BASE_H

#include "ocoms_config.h"
//#include "ocoms/types.h"

#include "ocoms/mca/mca.h"
#include "ocoms/mca/base/mca_base_framework.h"
//#include "ocoms/mca/event/event.h"
#include "ocoms/util/ocoms_hash_table.h"
#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/ocoms_pointer_array.h"
#include "ocoms/datatype/ocoms_datatype.h"

#include "ocoms/dstore/dstore.h"

BEGIN_C_DECLS

OCOMS_DECLSPEC extern ocoms_mca_base_framework_t ocoms_dstore_base_framework;

/**
 * Select a dstore module
 */
OCOMS_DECLSPEC int ocoms_dstore_base_select(void);

/* DSTORE is an oddball framework in that it:
 *
 * has an active storage component that issues handle-specific
 * modules. This is done to provide separate storage areas that
 * are isolated from each other, and thus don't have to worry
 * about overlapping keys
 *
 * a backfill module used to attempt to retrieve data that has
 * been requested, but that the handle-specific storage module
 * does not contain. This is used in situations where data has
 * not been provided at startup, and we need to retrieve it
 * solely on-demand
 */
typedef struct {
    ocoms_dstore_base_component_t *storage_component;
    ocoms_dstore_base_module_t *backfill_module;
    ocoms_pointer_array_t handles;   // array of open datastore handles
} ocoms_dstore_base_t;

OCOMS_DECLSPEC extern ocoms_dstore_base_t ocoms_dstore_base;

typedef struct {
    ocoms_object_t super;
    char *name;
    ocoms_dstore_base_module_t *module;
} ocoms_dstore_handle_t;
OBJ_CLASS_DECLARATION(ocoms_dstore_handle_t);

/**
 * Data for a particular ocoms process
 * The name association is maintained in the
 * proc_data hash table.
 */
typedef struct {
    /** Structure can be put on lists (including in hash tables) */
    ocoms_list_item_t super;
    bool loaded;
    /* List of ocoms_value_t structures containing all data
       received from this process, sorted by key. */
    ocoms_list_t data;
} ocoms_dstore_proc_data_t;
OBJ_CLASS_DECLARATION(ocoms_dstore_proc_data_t);

OCOMS_DECLSPEC int ocoms_dstore_base_open(const char *name, ocoms_list_t *attrs);
OCOMS_DECLSPEC int ocoms_dstore_base_update(int dstorehandle, ocoms_list_t *attrs);
OCOMS_DECLSPEC int ocoms_dstore_base_close(int dstorehandle);
OCOMS_DECLSPEC int ocoms_dstore_base_store(int dstorehandle,
                                         const ocoms_identifier_t *id,
                                         ocoms_value_t *kv);
OCOMS_DECLSPEC int ocoms_dstore_base_fetch(int dstorehandle,
                                         const ocoms_identifier_t *id,
                                         const char *key,
                                         ocoms_list_t *kvs);
OCOMS_DECLSPEC int ocoms_dstore_base_remove_data(int dstorehandle,
                                               const ocoms_identifier_t *id,
                                               const char *key);

/* support */
OCOMS_DECLSPEC ocoms_dstore_proc_data_t* ocoms_dstore_base_lookup_proc(ocoms_hash_table_t *jtable,
                                                                    ocoms_identifier_t id);


OCOMS_DECLSPEC ocoms_value_t* ocoms_dstore_base_lookup_keyval(ocoms_dstore_proc_data_t *proc_data,
                                                           const char *key);


END_C_DECLS

#endif
