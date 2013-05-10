/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2007 High Performance Computing Center Stuttgart, 
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

#include "ocoms/util/ocoms_value_array.h"


static void ocoms_value_array_construct(ocoms_value_array_t* array)
{
    array->array_items = NULL;
    array->array_size = 0;
    array->array_item_sizeof = 0;
    array->array_alloc_size = 0;
}

static void ocoms_value_array_destruct(ocoms_value_array_t* array)
{
    if (NULL != array->array_items)
        free(array->array_items);
}

OBJ_CLASS_INSTANCE(
    ocoms_value_array_t,
    ocoms_object_t,
    ocoms_value_array_construct,
    ocoms_value_array_destruct
);


int ocoms_value_array_set_size(ocoms_value_array_t* array, size_t size)
{
#if OCOMS_ENABLE_DEBUG
    if(array->array_item_sizeof == 0) {
        ocoms_output(0, "ocoms_value_array_set_size: item size must be initialized");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif

    if(size > array->array_alloc_size) {
        while(array->array_alloc_size < size)
            array->array_alloc_size <<= 1;
        array->array_items = (unsigned char *)realloc(array->array_items,
            array->array_alloc_size * array->array_item_sizeof);
        if (NULL == array->array_items)
            return OCOMS_ERR_OUT_OF_RESOURCE;
    }
    array->array_size = size;
    return OCOMS_SUCCESS;
}

