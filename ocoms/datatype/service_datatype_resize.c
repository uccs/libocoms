/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2009 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"
#include "ocoms/platform/service_constants.h"
#include "ocoms/datatype/service_datatype.h"
#include "ocoms/datatype/service_datatype_internal.h"

int32_t service_datatype_resize( service_datatype_t* type, OCOMS_PTRDIFF_TYPE lb, OCOMS_PTRDIFF_TYPE extent )
{
    type->lb = lb;
    type->ub = lb + extent;

    type->flags &= ~OCOMS_DATATYPE_FLAG_NO_GAPS;
    if( (extent == (OCOMS_PTRDIFF_TYPE)type->size) &&
        (type->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS) ) {
        type->flags |= OCOMS_DATATYPE_FLAG_NO_GAPS;
    }
    return OCOMS_SUCCESS;
}
