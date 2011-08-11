/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2009      Sun Microsystems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef CCS_INFO_H
#define CCS_INFO_H

#include "ccs_config.h"
#include <string.h>

#include "mpi.h"
#include "service/util/service_list.h"
#include "service/util/service_pointer_array.h"
#include "service/threads/mutex.h"


/**
 * \internal
 * ccs_info_t structure. MPI_Info is a pointer to this structure
 */
struct ccs_info_t {
  service_list_t super; 
  /**< generic list pointer which is the container for (key,value)
       pairs */
  int i_f_to_c_index; 
  /**< fortran handle for info. This is needed for translation from
       fortran to C and vice versa */
  service_mutex_t *i_lock;
  /**< Mutex for thread safety */
  bool i_freed;
  /**< Whether this info has been freed or not */
};
/**
 * \internal
 * Convenience typedef
 */
typedef struct ccs_info_t ccs_info_t;

/**
 * Padded struct to maintain back compatibiltiy.
 * See ompi/communicator/communicator.h comments with struct ccs_communicator_t
 * for full explanation why we chose the following padding construct for predefines.
 */
#define PREDEFINED_INFO_PAD (sizeof(void*) * 32)

struct ccs_predefined_info_t {
    struct ccs_info_t info;
    char padding[PREDEFINED_INFO_PAD - sizeof(ccs_info_t)];
};
typedef struct ccs_predefined_info_t ccs_predefined_info_t;


/**
 * \internal
 *
 * ccs_info_entry_t object. Each item in ccs_info_list is of this
 * type. It contains (key,value) pairs
 */
struct ccs_info_entry_t {
    opal_list_item_t super; /**< required for service_list_t type */
    char *ie_value; /**< value part of the (key, value) pair.
                  * Maximum length is MPI_MAX_INFO_VAL */
    char ie_key[MPI_MAX_INFO_KEY + 1]; /**< "key" part of the (key, value)
                                     * pair */ 
};
/**
 * \internal
 * Convenience typedef
 */
typedef struct ccs_info_entry_t ccs_info_entry_t;

BEGIN_C_DECLS

/**
 * Table for Fortran <-> C translation table
 */ 
extern opal_pointer_array_t ccs_info_f_to_c_table;

/**
 * Global instance for MPI_INFO_NULL
 */
CCS_DECLSPEC extern ccs_predefined_info_t ccs_mpi_info_null;

/**
 * \internal
 * Some declarations needed to use OBJ_NEW and OBJ_DESTRUCT macros
 */
CCS_DECLSPEC OBJ_CLASS_DECLARATION(ccs_info_t);

/**
 * \internal
 * Some declarations needed to use OBJ_NEW and OBJ_DESTRUCT macros
 */
CCS_DECLSPEC OBJ_CLASS_DECLARATION(ccs_info_entry_t);

/**
 * This function is invoked during ccs_mpi_init() and sets up
 * MPI_Info handling.
 */
int ccs_info_init(void);

/**
 * This functions is called during ccs_mpi_finalize() and shuts
 * down MPI_Info handling.
 */
int ccs_info_finalize(void);

/**
 *   ccs_info_dup - Duplicate an 'MPI_Info' object
 *
 *   @param info source info object (handle)
 *   @param newinfo pointer to the new info object (handle)
 *
 *   @retval MPI_SUCCESS upon success
 *   @retval MPI_ERR_NO_MEM if out of memory
 *
 *   Not only will the (key, value) pairs be duplicated, the order
 *   of keys will be the same in 'newinfo' as it is in 'info'.  When
 *   an info object is no longer being used, it should be freed with
 *   'MPI_Info_free'.
 */
int ccs_info_dup (ccs_info_t *info, ccs_info_t **newinfo);

/*
 * Set a new key,value pair on info.
 *
 * @param info pointer to ccs_info_t object
 * @param key pointer to the new key object
 * @param value pointer to the new value object
 *
 * @retval MPI_SUCCESS upon success
 * @retval MPI_ERR_NO_MEM if out of memory
 */
CCS_DECLSPEC int ccs_info_set (ccs_info_t *info, char *key, char *value);

/**
 * ccs_info_free - Free an 'MPI_Info' object.
 *
 *   @param info pointer to info (ccs_info_t *) object to be freed (handle)
 *
 *   @retval MPI_SUCCESS
 *   @retval MPI_ERR_ARG
 *
 *   Upon successful completion, 'info' will be set to
 *   'MPI_INFO_NULL'.  Free the info handle and all of its keys and
 *   values.
 */
int ccs_info_free (ccs_info_t **info);

  /**
   *   Get a (key, value) pair from an 'MPI_Info' object and assign it
   *   into a boolen output.
   *
   *   @param info Pointer to ccs_info_t object
   *   @param key null-terminated character string of the index key
   *   @param value Boolean output value
   *   @param flag true (1) if 'key' defined on 'info', false (0) if not
   *               (logical)
   *
   *   @retval MPI_SUCCESS
   *
   *   If found, the string value will be cast to the boolen output in
   *   the following manner:
   *
   *   - If the string value is digits, the return value is "(bool)
   *     atoi(value)"
   *   - If the string value is (case-insensitive) "yes" or "true", the
   *     result is true
   *   - If the string value is (case-insensitive) "no" or "false", the
   *     result is false
   *   - All other values are false
   */
CCS_DECLSPEC int ccs_info_get_bool (ccs_info_t *info, char *key, bool *value,
                                      int *flag);

/**
 *   Get a (key, value) pair from an 'MPI_Info' object
 *
 *   @param info Pointer to ccs_info_t object
 *   @param key null-terminated character string of the index key
 *   @param valuelen maximum length of 'value' (integer)
 *   @param value null-terminated character string of the value
 *   @param flag true (1) if 'key' defined on 'info', false (0) if not
 *               (logical)
 *
 *   @retval MPI_SUCCESS
 *
 *   In C and C++, 'valuelen' should be one less than the allocated
 *   space to allow for for the null terminator.
 */
CCS_DECLSPEC int ccs_info_get (ccs_info_t *info, char *key, int valuelen,
                                 char *value, int *flag);

/**
 * Delete a (key,value) pair from "info"
 *
 * @param info ccs_info_t pointer on which we need to operate
 * @param key The key portion of the (key,value) pair that
 *            needs to be deleted
 *
 * @retval MPI_SUCCESS
 * @retval MPI_ERR_NOKEY
 */
int ccs_info_delete (ccs_info_t *info, char *key);

/**
 *   @param info - ccs_info_t pointer object (handle)
 *   @param key - null-terminated character string of the index key
 *   @param valuelen - length of the value associated with 'key' (integer)
 *   @param flag - true (1) if 'key' defined on 'info', false (0) if not
 *   (logical)
 *
 *   @retval MPI_SUCCESS
 *   @retval MPI_ERR_ARG
 *   @retval MPI_ERR_INFO_KEY
 *
 *   The length returned in C and C++ does not include the end-of-string
 *   character.  If the 'key' is not found on 'info', 'valuelen' is left
 *   alone.
 */
CCS_DECLSPEC int ccs_info_get_valuelen (ccs_info_t *info, char *key, int *valuelen,
                              int *flag);

/**
 *   ccs_info_get_nthkey - Get a key indexed by integer from an 'MPI_Info' o
 *
 *   @param info Pointer to ccs_info_t object
 *   @param n index of key to retrieve (integer)
 *   @param key character string of at least 'MPI_MAX_INFO_KEY' characters
 *
 *   @retval MPI_SUCCESS
 *   @retval MPI_ERR_ARG
 */
int ccs_info_get_nthkey (ccs_info_t *info, int n, char *key);

/**
 * Convert value string to boolean
 *
 * Convert value string \c value into a boolean, using the
 * interpretation rules specified in MPI-2 Section 4.10.  The
 * strings "true", "false", and integer numbers can be converted
 * into booleans.  All others will return \c CCS_ERR_BAD_PARAM
 *
 * @param value Value string for info key to interpret
 * @param interp returned interpretation of the value key
 *
 * @retval CCS_SUCCESS string was successfully interpreted
 * @retval CCS_ERR_BAD_PARAM string was not able to be interpreted
 */
CCS_DECLSPEC int ccs_info_value_to_bool(char *value, bool *interp);

/**
 * Convert value string to integer
 *
 * Convert value string \c value into a integer, using the
 * interpretation rules specified in MPI-2 Section 4.10.  
 * All others will return \c CCS_ERR_BAD_PARAM
 *
 * @param value Value string for info key to interpret
 * @param interp returned interpretation of the value key
 *
 * @retval CCS_SUCCESS string was successfully interpreted
 * @retval CCS_ERR_BAD_PARAM string was not able to be interpreted
 */
int ccs_info_value_to_int(char *value, int *interp);

END_C_DECLS

/**
 * Return whether this info has been freed already or not.
 *
 * @param info Pointer to ccs_info_t object.
 *
 * @retval true If the info has already been freed
 * @retval false If the info has not yet been freed
 *
 * If the info has been freed, return true.  This will likely only
 * happen in a reliable manner if ccs_debug_handle_never_free is
 * true, in which case an extra OBJ_RETAIN is set on the object during
 * OBJ_NEW, meaning that the user will never be able to actually free
 * the underlying object.  It's a good way to find out if a process is
 * unintentionally using a freed handle.
 */
static inline bool ccs_info_is_freed(ccs_info_t *info)
{
  return info->i_freed;
}


/**
 * Get the number of keys defined on on an MPI_Info object
 * @param info Pointer to ccs_info_t object.
 * @param nkeys Pointer to nkeys, which needs to be filled up.
 *
 * @retval The number of keys defined on info
 */
static inline int 
ccs_info_get_nkeys(ccs_info_t *info, int *nkeys) 
{
    *nkeys = (int) opal_list_get_size(&(info->super));
    return MPI_SUCCESS;
}

#endif /* CCS_INFO_H */
