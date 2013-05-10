/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2009 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2006 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include <stddef.h>

#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/datatype/ocoms_datatype.h"
#include "ocoms/datatype/ocoms_datatype_internal.h"
#include "limits.h"
#include "ocoms/primitives/prefetch.h"

static void ocoms_datatype_construct( ocoms_datatype_t* pData )
{
    int i;

    pData->size               = 0;
    pData->id                 = 0;
    pData->nbElems            = 0;
    pData->bdt_used           = 0;
    for( i = 0; i < OCOMS_DATATYPE_MAX_PREDEFINED; i++ )
        pData->btypes[i]      = 0;
    pData->btypes[OCOMS_DATATYPE_LOOP]    = 0;

    pData->opt_desc.desc      = NULL;
    pData->opt_desc.length    = 0;
    pData->opt_desc.used      = 0;
    pData->align              = 1;
    pData->flags              = OCOMS_DATATYPE_FLAG_CONTIGUOUS;
    pData->true_lb            = LONG_MAX;
    pData->true_ub            = LONG_MIN;
    pData->lb                 = LONG_MAX;
    pData->ub                 = LONG_MIN;
    pData->name[0]            = '\0';
}

static void ocoms_datatype_destruct( ocoms_datatype_t* datatype )
{
    if (!ocoms_datatype_is_predefined(datatype)) {
        if( datatype->desc.desc != NULL ) {
            free( datatype->desc.desc );
            datatype->desc.length = 0;
            datatype->desc.used   = 0;
        }
        if( datatype->opt_desc.desc != NULL ) {
            if( datatype->opt_desc.desc != datatype->desc.desc )
                free( datatype->opt_desc.desc );
            datatype->opt_desc.length = 0;
            datatype->opt_desc.used   = 0;
            datatype->opt_desc.desc   = NULL;
        }
    }
    /**
     * As the default description and the optimized description can point to the
     * same memory location we should keep the default location pointer until we
     * know what we should do with the optimized description.
     */
    datatype->desc.desc   = NULL;

    /* make sure the name is set to empty */
    datatype->name[0] = '\0';
}

OBJ_CLASS_INSTANCE(ocoms_datatype_t, ocoms_object_t, ocoms_datatype_construct, ocoms_datatype_destruct);

ocoms_datatype_t* ocoms_datatype_create( int32_t expectedSize )
{
    ocoms_datatype_t* datatype = (ocoms_datatype_t*)OBJ_NEW(ocoms_datatype_t);

    if( expectedSize == -1 ) expectedSize = DT_INCREASE_STACK;
    datatype->desc.length = expectedSize + 1;  /* one for the fake elem at the end */
    datatype->desc.used   = 0;
    datatype->desc.desc   = (dt_elem_desc_t*)calloc(datatype->desc.length, sizeof(dt_elem_desc_t));
    /* BEWARE: an upper-layer configured with OCOMS_MAX_OBJECT_NAME different than the OPAL-layer will not work! */
    memset( datatype->name, 0, OCOMS_MAX_OBJECT_NAME );
    return datatype;
}

int32_t ocoms_datatype_create_desc( ocoms_datatype_t * datatype, int32_t expectedSize )
{
    if( expectedSize == -1 )
        expectedSize = DT_INCREASE_STACK;
    datatype->desc.length = expectedSize + 1;  /* one for the fake elem at the end */
    datatype->desc.used   = 0;
    datatype->desc.desc   = (dt_elem_desc_t*)calloc(datatype->desc.length, sizeof(dt_elem_desc_t));
    if (NULL == datatype->desc.desc)
        return OCOMS_ERR_OUT_OF_RESOURCE;
    return OCOMS_SUCCESS;
}
