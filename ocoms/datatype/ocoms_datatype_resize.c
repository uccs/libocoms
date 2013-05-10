/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2009 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
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
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/datatype/ocoms_datatype.h"
#include "ocoms/datatype/ocoms_datatype_internal.h"

int32_t ocoms_datatype_resize( ocoms_datatype_t* type, OCOMS_PTRDIFF_TYPE lb, OCOMS_PTRDIFF_TYPE extent )
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
