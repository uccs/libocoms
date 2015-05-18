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
 *
 * The Database Framework - used for internal storage of
 * information relating to modex and other OMPI operations
 *
 */

#ifndef OCOMS_DB_H
#define OCOMS_DB_H

#include "ocoms_config.h"
//#include "ocoms/types.h"

#include "ocoms/mca/mca.h"
//#include "ocoms/mca/event/event.h"
#include "ocoms/datatype/ocoms_datatype.h"

#include "ocoms/dstore/dstore_types.h"

/**
 * DATABASE DESIGN
 *
 * Each API function is treated as blocking.
 * 
 */

BEGIN_C_DECLS

typedef uint64_t ocoms_identifier_t;
typedef struct {
    ocoms_list_item_t super;            /* required for this to be on lists */
    char *key;                          /* key string */
    struct {
        void* ptr;
        unsigned size;
    } value;
} ocoms_value_t;

OCOMS_DECLSPEC OBJ_CLASS_DECLARATION( ocoms_value_t );

/* declare a global handle until such time
 * as someone figures out how to separate the various
 * datastore channels
 */
OCOMS_DECLSPEC extern int ocoms_dstore_internal;

OCOMS_DECLSPEC extern int ocoms_dstore_peer;
OCOMS_DECLSPEC extern int ocoms_dstore_internal;
OCOMS_DECLSPEC extern int ocoms_dstore_nonpeer;

/****    DEFINE THE PUBLIC API'S    ****/
/*
 * Open a database
 * 
 * Open a database for access. The name field is purely for
 * debug purposes and has no implementation relevance.
 * Just like the standard POSIX file open, the call will return
 * a unique "handle" that must be provided with any subsequent
 * call to store or fetch data from this database.
 *
 * The attributes parameter can be used to pass any desired
 * optional directives to the active storage component. These
 * are passed as a list of ocoms_value_t's.
 *
 * NOTE: calls to these APIs must be thread-protected as there
 * is NO internal thread safety.
 */
typedef int (*ocoms_dstore_base_API_open_fn_t)(const char *name,
                                              ocoms_list_t *attributes);

/*
 * Update an existing handle
 *
 * Sometimes an existing handle requires an update to its attributes, so
 * provide an API for doing so
 */
typedef int (*ocoms_dstore_base_API_update_fn_t)(int dstorehandle,
                                                ocoms_list_t *attributes);

/*
 * Close a database handle
 *
 * Close the specified database handle. A -1 handle indicates
 * that ALL open database handles are to be closed.
 */
typedef int (*ocoms_dstore_base_API_close_fn_t)(int dstorehandle);

/*
 * Store a data value against the primary key  - overwrites any data
 * of matching key that is already present. The data is copied into the database
 * and therefore does not need to be preserved by the caller.
 */
typedef int (*ocoms_dstore_base_API_store_fn_t)(int dstorehandle,
                                               const ocoms_identifier_t *id,
                                               ocoms_value_t *kv);

/*
 * Retrieve data
 *
 * Retrieve data for the given primary key associated with the specified key. Wildcards
 * are supported here as well. Caller is responsible for releasing the returned list
 * of ocoms_value_t objects.
 */
typedef int (*ocoms_dstore_base_API_fetch_fn_t)(int dstorehandle,
                                               const ocoms_identifier_t *id,
                                               const char *key,
                                               ocoms_list_t *kvs);

/*
 * Delete data
 *
 * Delete the data for the given primary key that is associated with the specified key.
 * If a NULL key is provided, all data for the given primary key will be deleted.
 */
typedef int (*ocoms_dstore_base_API_remove_fn_t)(int dstorehandle,
                                                const ocoms_identifier_t *id,
                                                const char *key);

/*
 * the standard public API data structure
 */
typedef struct {
    ocoms_dstore_base_API_open_fn_t           open;
    ocoms_dstore_base_API_update_fn_t         update;
    ocoms_dstore_base_API_close_fn_t          close;
    ocoms_dstore_base_API_store_fn_t          store;
    ocoms_dstore_base_API_fetch_fn_t          fetch;
    ocoms_dstore_base_API_remove_fn_t         remove;
} ocoms_dstore_base_API_t;



/****    DEFINE THE MODULE API'S    ****/
/* Note that each datastore handle will be associated with
 * a single active module. Thus, storing and fetching data
 * from that module does not require that we pass in the
 * handle itself.
 *
 * NOTE: the call to actually store/fetch data in a given
 * datastore handle must be protected against threaded operations
 * as there is NO thread protection inside the various modules.
 */
struct ocoms_dstore_base_module_t;

/*
 * Initialize the module
 */
typedef int (*ocoms_dstore_base_module_init_fn_t)(struct ocoms_dstore_base_module_t *mod);

/*
 * Finalize the module
 */
typedef void (*ocoms_dstore_base_module_finalize_fn_t)(struct ocoms_dstore_base_module_t *mod);

/* store the data in this module */
typedef int (*ocoms_dstore_base_module_store_fn_t)(struct ocoms_dstore_base_module_t *mod,
                                                  const ocoms_identifier_t *id,
                                                  ocoms_value_t *kv);

/* fetch data from the module */
typedef int (*ocoms_dstore_base_module_fetch_fn_t)(struct ocoms_dstore_base_module_t *mod,
                                                  const ocoms_identifier_t *id,
                                                  const char *key,
                                                  ocoms_list_t *kvs);

/* remove data */
typedef int (*ocoms_dstore_base_module_remove_fn_t)(struct ocoms_dstore_base_module_t *mod,
                                                   const ocoms_identifier_t *id,
                                                   const char *key);

/*
 * the standard module data structure
 */
typedef struct {
    ocoms_dstore_base_module_init_fn_t            init;
    ocoms_dstore_base_module_finalize_fn_t        finalize;
    ocoms_dstore_base_module_store_fn_t           store;
    ocoms_dstore_base_module_fetch_fn_t           fetch;
    ocoms_dstore_base_module_remove_fn_t          remove;
} ocoms_dstore_base_module_t;

/*
 * the component data structure
 */

/* create and return a datastore module */
typedef ocoms_dstore_base_module_t* (*mca_dstore_base_component_create_hdl_fn_t)(ocoms_list_t *attributes);

/* update an existing handle */
typedef int (*mca_dstore_base_component_update_hdl_fn_t)(int hdl, ocoms_list_t *attributes);

/* provide a chance for the component to finalize */
typedef void (*mca_dstore_base_component_finalize_fn_t)(void);

typedef struct {
    ocoms_mca_base_component_t                base_version;
    ocoms_mca_base_component_data_t           base_data;
    mca_dstore_base_component_create_hdl_fn_t create_handle;
    mca_dstore_base_component_update_hdl_fn_t update_handle;
    mca_dstore_base_component_finalize_fn_t   finalize;
} ocoms_dstore_base_component_t;

/*
 * Macro for use in components that are of type dstore
 */
#define OCOMS_DSTORE_BASE_VERSION_2_0_0 \
  MCA_BASE_VERSION_2_0_0, \
  "dstore", 2, 0, 0

/* Global structure for accessing store functions */
OCOMS_DECLSPEC extern ocoms_dstore_base_API_t ocoms_dstore;  /* holds base function pointers */

END_C_DECLS

#endif
