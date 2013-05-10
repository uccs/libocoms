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

int32_t ocoms_datatype_create_contiguous( int count, const ocoms_datatype_t* oldType,
                                         ocoms_datatype_t** newType )
{
    ocoms_datatype_t* pdt;

    if( 0 == count ) {
        pdt = ocoms_datatype_create( 0 );
        ocoms_datatype_add( pdt, &ocoms_datatype_empty, 0, 0, 0 );
    } else {
        pdt = ocoms_datatype_create( oldType->desc.used + 2 );
        ocoms_datatype_add( pdt, oldType, count, 0, (oldType->ub - oldType->lb) );
    }
    *newType = pdt;
    return OCOMS_SUCCESS;
}
