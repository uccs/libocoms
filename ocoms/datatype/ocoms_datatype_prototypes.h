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

#ifndef OCOMS_DATATYPE_PROTOTYPES_H_HAS_BEEN_INCLUDED
#define OCOMS_DATATYPE_PROTOTYPES_H_HAS_BEEN_INCLUDED

#include "ocoms/platform/ocoms_config.h"


BEGIN_C_DECLS

/*
 * First the public ones
 */

OCOMS_DECLSPEC int32_t
ocoms_unpack_general( ocoms_convertor_t* pConvertor,
                     struct iovec* iov, uint32_t* out_size,
                     size_t* max_data );
OCOMS_DECLSPEC int32_t
ocoms_unpack_general_checksum( ocoms_convertor_t* pConvertor,
                              struct iovec* iov, uint32_t* out_size,
                              size_t* max_data );

/*
 * Now the internal functions
 */
int32_t
ocoms_pack_homogeneous_contig( ocoms_convertor_t* pConv,
                          struct iovec* iov, uint32_t* out_size,
                          size_t* max_data );
int32_t
ocoms_pack_homogeneous_contig_checksum( ocoms_convertor_t* pConv,
                                   struct iovec* iov, uint32_t* out_size,
                                   size_t* max_data );
int32_t
ocoms_pack_homogeneous_contig_with_gaps( ocoms_convertor_t* pConv,
                                    struct iovec* iov, uint32_t* out_size,
                                    size_t* max_data );
int32_t
ocoms_pack_homogeneous_contig_with_gaps_checksum( ocoms_convertor_t* pConv,
                                             struct iovec* iov, uint32_t* out_size,
                                             size_t* max_data );
int32_t
ocoms_generic_simple_pack( ocoms_convertor_t* pConvertor,
                          struct iovec* iov, uint32_t* out_size,
                          size_t* max_data );
int32_t
ocoms_generic_simple_pack_checksum( ocoms_convertor_t* pConvertor,
                                   struct iovec* iov, uint32_t* out_size,
                                   size_t* max_data );
int32_t
ocoms_unpack_homogeneous_contig( ocoms_convertor_t* pConv,
                                struct iovec* iov, uint32_t* out_size,
                                size_t* max_data );
int32_t
ocoms_unpack_homogeneous_contig_checksum( ocoms_convertor_t* pConv,
                                         struct iovec* iov, uint32_t* out_size,
                                         size_t* max_data );
int32_t
ocoms_generic_simple_unpack( ocoms_convertor_t* pConvertor,
                            struct iovec* iov, uint32_t* out_size,
                            size_t* max_data );
int32_t
ocoms_generic_simple_unpack_checksum( ocoms_convertor_t* pConvertor,
                                     struct iovec* iov, uint32_t* out_size,
                                     size_t* max_data );

END_C_DECLS

#endif  /* OCOMS_DATATYPE_PROTOTYPES_H_HAS_BEEN_INCLUDED */
