/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2010 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2006 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2009      Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "service/platform/ocoms_config.h"
#include "service/platform/service_constants.h"
#include "service/datatype/service_datatype.h"
#include "service/datatype/service_datatype_internal.h"

int32_t service_datatype_create_contiguous( int count, const service_datatype_t* oldType,
                                         service_datatype_t** newType )
{
    service_datatype_t* pdt;

    if( 0 == count ) {
        pdt = service_datatype_create( 0 );
        service_datatype_add( pdt, &service_datatype_empty, 0, 0, 0 );
    } else {
        pdt = service_datatype_create( oldType->desc.used + 2 );
        service_datatype_add( pdt, oldType, count, 0, (oldType->ub - oldType->lb) );
    }
    *newType = pdt;
    return OCOMS_SUCCESS;
}
