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

/*
 * As the new type has the same commit state as the old one, I have to copy the fake
 * OCOMS_DATATYPE_END_LOOP from the description (both normal and optimized).
 *
 * Clone all the values from oldType into newType without allocating a new datatype.
 */
int32_t ocoms_datatype_clone( const ocoms_datatype_t * src_type, ocoms_datatype_t * dest_type )
{
    int32_t desc_length = src_type->desc.used + 1;  /* +1 because of the fake OCOMS_DATATYPE_END_LOOP entry */
    dt_elem_desc_t* temp = dest_type->desc.desc;    /* temporary copy of the desc pointer */

    /* copy _excluding_ the super object, we want to keep the cls_destruct_array */
    memcpy( (char*)dest_type + sizeof(ocoms_object_t),
            (char*)src_type + sizeof(ocoms_object_t),
            sizeof(ocoms_datatype_t)-sizeof(ocoms_object_t) );

    dest_type->flags &= (~OCOMS_DATATYPE_FLAG_PREDEFINED);
    dest_type->desc.desc = temp;

    /**
     * Allow duplication of MPI_UB and MPI_LB.
     */
    if( 0 != src_type->desc.used ) {
        memcpy( dest_type->desc.desc, src_type->desc.desc, sizeof(dt_elem_desc_t) * desc_length );
        if( 0 != src_type->opt_desc.used ) {
            if( src_type->opt_desc.desc == src_type->desc.desc) {
                dest_type->opt_desc = dest_type->desc;
            } else {
                desc_length = dest_type->opt_desc.used + 1;
                dest_type->opt_desc.desc = (dt_elem_desc_t*)malloc( desc_length * sizeof(dt_elem_desc_t) );
                /*
                 * Yes, the dest_type->opt_desc.length is just the opt_desc.used of the old Type.
                 */
                dest_type->opt_desc.length = src_type->opt_desc.used;
                dest_type->opt_desc.used = src_type->opt_desc.used;
                memcpy( dest_type->opt_desc.desc, src_type->opt_desc.desc, desc_length * sizeof(dt_elem_desc_t) );
            }
        }
    }
    dest_type->id  = src_type->id;  /* preserve the default id. This allow us to
                                     * copy predefined types. */
    return OCOMS_SUCCESS;
}
