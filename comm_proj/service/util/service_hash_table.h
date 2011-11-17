/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
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
 *
 */

/** @file 
 *
 *  A hash table that may be indexed with either fixed length
 *  (e.g. uint32_t/uint64_t) or arbitrary size binary key
 *  values. However, only one key type may be used in a given table
 *  concurrently.
 */

#ifndef CCS_HASH_TABLE_H
#define CCS_HASH_TABLE_H

#include "ccs_config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "service/util/service_list.h"

BEGIN_C_DECLS

CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_hash_table_t);
                           
struct service_hash_table_t
{
    service_object_t        super;          /**< subclass of service_object_t */
    service_list_t          ht_nodes;       /**< free list of hash nodes */
    service_list_t         *ht_table;       /**< each item is an array of service_fhnode_t nodes */
    size_t              ht_table_size;  /**< size of table */
    size_t              ht_size;        /**< number of values on table */
    size_t              ht_mask;
};
typedef struct service_hash_table_t service_hash_table_t;

                           
    
/**
 *  Initializes the table size, must be called before using
 *  the table.
 *
 *  @param   table   The input hash table (IN).
 *  @param   size    The size of the table, which will be rounded up 
 *                   (if required) to the next highest power of two (IN).
 *  @return  OPAL error code.
 *
 */

CCS_DECLSPEC int service_hash_table_init(service_hash_table_t* ht, size_t table_size);


/**
 *  Returns the number of elements currently stored in the table.
 *
 *  @param   table   The input hash table (IN).
 *  @return  The number of elements in the table.
 *
 */

static inline size_t service_hash_table_get_size(service_hash_table_t *ht)
{
    return ht->ht_size;
}

/**
 *  Remove all elements from the table.
 *
 *  @param   table   The input hash table (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_remove_all(service_hash_table_t *ht);

/**
 *  Retrieve value via uint32_t key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @param   ptr     The value associated with the key
 *  @return  integer return code:
 *           - CCS_SUCCESS       if key was found
 *           - CCS_ERR_NOT_FOUND if key was not found
 *           - CCS_ERROR         other error
 *
 */

CCS_DECLSPEC int service_hash_table_get_value_uint32(service_hash_table_t* table, uint32_t key, 
						   void** ptr);

/**
 *  Set value based on uint32_t key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @param   value   The value to be associated with the key (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_set_value_uint32(service_hash_table_t* table, uint32_t key, void* value);

/**
 *  Remove value based on uint32_t key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_remove_value_uint32(service_hash_table_t* table, uint32_t key);

/**
 *  Retrieve value via uint64_t key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @param   ptr     The value associated with the key
 *  @return  integer return code:
 *           - CCS_SUCCESS       if key was found
 *           - CCS_ERR_NOT_FOUND if key was not found
 *           - CCS_ERROR         other error
 *
 */

CCS_DECLSPEC int service_hash_table_get_value_uint64(service_hash_table_t *table, uint64_t key,
						   void **ptr);

/**
 *  Set value based on uint64_t key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @param   value   The value to be associated with the key (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_set_value_uint64(service_hash_table_t *table, uint64_t key, void* value);

/**
 *  Remove value based on uint64_t key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_remove_value_uint64(service_hash_table_t *table, uint64_t key);

/**
 *  Retrieve value via arbitrary length binary key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @param   ptr     The value associated with the key
 *  @return  integer return code:
 *           - CCS_SUCCESS       if key was found
 *           - CCS_ERR_NOT_FOUND if key was not found
 *           - CCS_ERROR         other error
 *
 */

CCS_DECLSPEC int service_hash_table_get_value_ptr(service_hash_table_t *table, const void* key, 
						size_t keylen, void **ptr);

/**
 *  Set value based on arbitrary length binary key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @param   value   The value to be associated with the key (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_set_value_ptr(service_hash_table_t *table, const void* key, size_t keylen, void* value);

/**
 *  Remove value based on arbitrary length binary key.
 *
 *  @param   table   The input hash table (IN).
 *  @param   key     The input key (IN).
 *  @return  OPAL return code.
 *
 */

CCS_DECLSPEC int service_hash_table_remove_value_ptr(service_hash_table_t *table, const void* key, size_t keylen);


/** The following functions are only for allowing iterating through
    the hash table. The calls return along with a key, a pointer to
    the hash node with the current key, so that subsequent calls do
    not have to traverse all over again to the key (although it may
    just be a simple thing - to go to the array element and then
    traverse through the individual list). But lets take out this
    inefficiency too. This is similar to having an STL iterator in
    functionality */

/**
 *  Get the first 32 bit key from the hash table, which can be used later to
 *  get the next key
 *  @param  table   The hash table pointer (IN)
 *  @param  key     The first key (OUT)
 *  @param  value   The value corresponding to this key (OUT)
 *  @param  node    The pointer to the hash table internal node which stores
 *                  the key-value pair (this is required for subsequent calls
 *                  to get_next_key) (OUT)
 *  @return OPAL error code
 *
 */

CCS_DECLSPEC int service_hash_table_get_first_key_uint32(service_hash_table_t *table, uint32_t *key,
					void **value, void **node);


/**
 *  Get the next 32 bit key from the hash table, knowing the current key 
 *  @param  table    The hash table pointer (IN)
 *  @param  key      The key (OUT)
 *  @param  value    The value corresponding to this key (OUT)
 *  @param  in_node  The node pointer from previous call to either get_first 
                     or get_next (IN)
 *  @param  out_node The pointer to the hash table internal node which stores
 *                   the key-value pair (this is required for subsequent calls
 *                   to get_next_key) (OUT)
 *  @return OPAL error code
 *
 */

CCS_DECLSPEC int service_hash_table_get_next_key_uint32(service_hash_table_t *table, uint32_t *key,
				       void **value, void *in_node,
				       void **out_node);


/**
 *  Get the first 64 key from the hash table, which can be used later to
 *  get the next key
 *  @param  table   The hash table pointer (IN)
 *  @param  key     The first key (OUT)
 *  @param  value   The value corresponding to this key (OUT)
 *  @param  node    The pointer to the hash table internal node which stores
 *                  the key-value pair (this is required for subsequent calls
 *                  to get_next_key) (OUT)
 *  @return OPAL error code
 *
 */

CCS_DECLSPEC int service_hash_table_get_first_key_uint64(service_hash_table_t *table, uint64_t *key,
				       void **value, void **node);


/**
 *  Get the next 64 bit key from the hash table, knowing the current key 
 *  @param  table    The hash table pointer (IN)
 *  @param  key      The key (OUT)
 *  @param  value    The value corresponding to this key (OUT)
 *  @param  in_node  The node pointer from previous call to either get_first 
                     or get_next (IN)
 *  @param  out_node The pointer to the hash table internal node which stores
 *                   the key-value pair (this is required for subsequent calls
 *                   to get_next_key) (OUT)
 *  @return OPAL error code
 *
 */
    
CCS_DECLSPEC int service_hash_table_get_next_key_uint64(service_hash_table_t *table, uint64_t *key,
				       void **value, void *in_node,
				       void **out_node);

END_C_DECLS

#endif  /* CCS_HASH_TABLE_H */
