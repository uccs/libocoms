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

#include "ccs_config.h"
#include "service/platform/service_constants.h"
#include "service/datatype/service_datatype.h"
#include "service/datatype/service_datatype_internal.h"

int32_t service_datatype_resize( service_datatype_t* type, CCS_PTRDIFF_TYPE lb, CCS_PTRDIFF_TYPE extent )
{
    type->lb = lb;
    type->ub = lb + extent;

    type->flags &= ~CCS_DATATYPE_FLAG_NO_GAPS;
    if( (extent == (CCS_PTRDIFF_TYPE)type->size) &&
        (type->flags & CCS_DATATYPE_FLAG_CONTIGUOUS) ) {
        type->flags |= CCS_DATATYPE_FLAG_NO_GAPS;
    }
    return CCS_SUCCESS;
}
