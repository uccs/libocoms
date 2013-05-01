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
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2011      NVIDIA Corporation.  All rights reserved.
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
#include <stdio.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "ocoms/primitives/prefetch.h"
#include "ocoms/util/arch.h"
#include "ocoms/util/output.h"

#include "ocoms/datatype/ocoms_datatype_internal.h"
#include "ocoms/datatype/ocoms_datatype.h"
#include "ocoms/datatype/ocoms_convertor.h"
#include "ocoms/datatype/ocoms_datatype_checksum.h"
#include "ocoms/datatype/ocoms_datatype_prototypes.h"
#include "ocoms/datatype/ocoms_convertor_internal.h"
#if OCOMS_CUDA_SUPPORT
#include "ocoms/datatype/ocoms_datatype_cuda.h"
#define MEMCPY_CUDA( DST, SRC, BLENGTH, CONVERTOR ) \
    CONVERTOR->cbmemcpy( (DST), (SRC), (BLENGTH) )
#endif

extern int ocoms_convertor_create_stack_with_pos_general( ocoms_convertor_t* convertor,
                                                         int starting_point, const int* sizes );

static void ocoms_convertor_construct( ocoms_convertor_t* convertor )
{
    convertor->pStack         = convertor->static_stack;
    convertor->stack_size     = DT_STATIC_STACK_SIZE;
    convertor->partial_length = 0;
    convertor->remoteArch     = ocoms_local_arch;
    convertor->flags          = OCOMS_DATATYPE_FLAG_NO_GAPS | CONVERTOR_COMPLETED;
#if OCOMS_CUDA_SUPPORT
    convertor->cbmemcpy       = &memcpy;
#endif
}


static void ocoms_convertor_destruct( ocoms_convertor_t* convertor )
{
    ocoms_convertor_cleanup( convertor );
}

OBJ_CLASS_INSTANCE(ocoms_convertor_t, ocoms_object_t, ocoms_convertor_construct, ocoms_convertor_destruct );

static ocoms_convertor_master_t* ocoms_convertor_master_list = NULL;

extern conversion_fct_t ocoms_datatype_heterogeneous_copy_functions[OCOMS_DATATYPE_MAX_PREDEFINED];
extern conversion_fct_t ocoms_datatype_copy_functions[OCOMS_DATATYPE_MAX_PREDEFINED];

void ocoms_convertor_destroy_masters( void )
{
    ocoms_convertor_master_t* master = ocoms_convertor_master_list;

    while( NULL != master ) {
        ocoms_convertor_master_list = master->next;
        master->next = NULL;
        /* Cleanup the conversion function if not one of the defaults */
        if( (master->pFunctions != ocoms_datatype_heterogeneous_copy_functions) &&
            (master->pFunctions != ocoms_datatype_copy_functions) )
            free( master->pFunctions );

        free( master );
        master = ocoms_convertor_master_list;
    }
}

/**
 * Find or create a convertor suitable for the remote architecture. If there
 * is already a master convertor for this architecture then return it.
 * Otherwise, create and initialize a full featured master convertor.
 */
ocoms_convertor_master_t* ocoms_convertor_find_or_create_master( uint32_t remote_arch )
{
    ocoms_convertor_master_t* master = ocoms_convertor_master_list;
    int i;
    size_t* remote_sizes;

    while( NULL != master ) {
        if( master->remote_arch == remote_arch )
            return master;
        master = master->next;
    }
    /**
     * Create a new convertor matching the specified architecture and add it to the
     * master convertor list.
     */
    master = (ocoms_convertor_master_t*)malloc( sizeof(ocoms_convertor_master_t) );
    master->next = ocoms_convertor_master_list;
    ocoms_convertor_master_list = master;
    master->remote_arch = remote_arch;
    master->flags       = 0;
    master->hetero_mask = 0;
    /**
     * Most of the sizes will be identical, so for now just make a copy of
     * the local ones. As master->remote_sizes is defined as being an array of
     * consts we have to manually cast it before using it for writing purposes.
     */
    remote_sizes = (size_t*)master->remote_sizes;
    memcpy(remote_sizes, ocoms_datatype_local_sizes, sizeof(size_t) * OCOMS_DATATYPE_MAX_PREDEFINED);
    /**
     * If the local and remote architecture are the same there is no need
     * to check for the remote data sizes. They will always be the same as
     * the local ones.
     */
    if( master->remote_arch == ocoms_local_arch ) {
        master->pFunctions = ocoms_datatype_copy_functions;
        master->flags |= CONVERTOR_HOMOGENEOUS;
        return master;
    }

    /* Find out the remote bool size */
    if( ocoms_arch_checkmask( &master->remote_arch, OCOMS_ARCH_BOOLIS8 ) ) {
        remote_sizes[OCOMS_DATATYPE_BOOL] = 1;
    } else if( ocoms_arch_checkmask( &master->remote_arch, OCOMS_ARCH_BOOLIS16 ) ) {
        remote_sizes[OCOMS_DATATYPE_BOOL] = 2;
    } else if( ocoms_arch_checkmask( &master->remote_arch, OCOMS_ARCH_BOOLIS32 ) ) {
        remote_sizes[OCOMS_DATATYPE_BOOL] = 4;
    } else {
        ocoms_output( 0, "Unknown sizeof(bool) for the remote architecture\n" );
    }

    /**
     * Now we can compute the conversion mask. For all sizes where the remote
     * and local architecture differ a conversion is needed. Moreover, if the
     * 2 architectures don't have the same endianess all data with a length
     * over 2 bytes (with the exception of logicals) have to be byte-swapped.
     */
    for( i = OCOMS_DATATYPE_FIRST_TYPE; i < OCOMS_DATATYPE_MAX_PREDEFINED; i++ ) {
        if( remote_sizes[i] != ocoms_datatype_local_sizes[i] )
            master->hetero_mask |= (((uint32_t)1) << i);
    }
    if( ocoms_arch_checkmask( &master->remote_arch, OCOMS_ARCH_ISBIGENDIAN ) !=
        ocoms_arch_checkmask( &ocoms_local_arch, OCOMS_ARCH_ISBIGENDIAN ) ) {
        uint32_t hetero_mask = 0;

        for( i = OCOMS_DATATYPE_FIRST_TYPE; i < OCOMS_DATATYPE_MAX_PREDEFINED; i++ ) {
            if( remote_sizes[i] > 1 )
                hetero_mask |= (((uint32_t)1) << i);
        }
        hetero_mask &= ~(((uint32_t)1) << OCOMS_DATATYPE_BOOL);
        master->hetero_mask |= hetero_mask;
    }
    master->pFunctions = (conversion_fct_t*)malloc( sizeof(ocoms_datatype_heterogeneous_copy_functions) );
    /**
     * Usually the heterogeneous functions are slower than the copy ones. Let's
     * try to minimize the usage of the heterogeneous versions.
     */
    for( i = OCOMS_DATATYPE_FIRST_TYPE; i < OCOMS_DATATYPE_MAX_PREDEFINED; i++ ) {
        if( master->hetero_mask & (((uint32_t)1) << i) )
            master->pFunctions[i] = ocoms_datatype_heterogeneous_copy_functions[i];
        else
            master->pFunctions[i] = ocoms_datatype_copy_functions[i];
    }

    /* We're done so far, return the mater convertor */
    return master;
}


ocoms_convertor_t* ocoms_convertor_create( int32_t remote_arch, int32_t mode )
{
    ocoms_convertor_t* convertor = OBJ_NEW(ocoms_convertor_t);
    ocoms_convertor_master_t* master;

    master = ocoms_convertor_find_or_create_master( remote_arch );

    convertor->remoteArch = remote_arch;
    convertor->stack_pos  = 0;
    convertor->flags      = master->flags;
    convertor->master     = master;

    return convertor;
}

#define OCOMS_CONVERTOR_SET_STATUS_BEFORE_PACK_UNPACK( CONVERTOR, IOV, OUT, MAX_DATA ) \
    do {                                                                \
        /* protect against over packing data */                         \
        if( OCOMS_UNLIKELY((CONVERTOR)->flags & CONVERTOR_COMPLETED) ) { \
            (IOV)[0].iov_len = 0;                                       \
            *(OUT) = 0;                                                 \
            *(MAX_DATA) = 0;                                            \
            return 1;  /* nothing to do */                              \
        }                                                               \
        (CONVERTOR)->checksum = OCOMS_CSUM_ZERO;                         \
        (CONVERTOR)->csum_ui1 = 0;                                      \
        (CONVERTOR)->csum_ui2 = 0;                                      \
        assert( (CONVERTOR)->bConverted < (CONVERTOR)->local_size );    \
    } while(0)

/** 
 * Return 0 if everything went OK and if there is still room before the complete
 *          conversion of the data (need additional call with others input buffers )
 *        1 if everything went fine and the data was completly converted
 *       -1 something wrong occurs.
 */
int32_t ocoms_convertor_pack( ocoms_convertor_t* pConv,
                             struct iovec* iov, uint32_t* out_size,
                             size_t* max_data )
{
    OCOMS_CONVERTOR_SET_STATUS_BEFORE_PACK_UNPACK( pConv, iov, out_size, max_data );

    if( OCOMS_LIKELY(pConv->flags & CONVERTOR_NO_OP) ) {
        /**
         * We are doing conversion on a contiguous datatype on a homogeneous
         * environment. The convertor contain minimal informations, we only
         * use the bConverted to manage the conversion.
         */
        uint32_t i;
        unsigned char* base_pointer;
        size_t pending_length = pConv->local_size - pConv->bConverted;

        *max_data = pending_length;
        ocoms_convertor_get_current_pointer( pConv, (void**)&base_pointer );

        for( i = 0; i < *out_size; i++ ) {
            if( iov[i].iov_len >= pending_length ) {
                goto complete_contiguous_data_pack;
            }
            if( OCOMS_LIKELY(NULL == iov[i].iov_base) )
                iov[i].iov_base = (IOVBASE_TYPE *) base_pointer;
            else
#if OCOMS_CUDA_SUPPORT
                MEMCPY_CUDA( iov[i].iov_base, base_pointer, iov[i].iov_len, pConv );
#else
                MEMCPY( iov[i].iov_base, base_pointer, iov[i].iov_len );
#endif
            pending_length -= iov[i].iov_len;
            base_pointer += iov[i].iov_len;
        }
        *max_data -= pending_length;
        pConv->bConverted += (*max_data);
        return 0;

complete_contiguous_data_pack:
        iov[i].iov_len = pending_length;
        if( OCOMS_LIKELY(NULL == iov[i].iov_base) )
            iov[i].iov_base = (IOVBASE_TYPE *) base_pointer;
        else
#if OCOMS_CUDA_SUPPORT
            MEMCPY_CUDA( iov[i].iov_base, base_pointer, iov[i].iov_len, pConv );
#else
            MEMCPY( iov[i].iov_base, base_pointer, iov[i].iov_len );
#endif
        pConv->bConverted = pConv->local_size;
        *out_size = i + 1;
        pConv->flags |= CONVERTOR_COMPLETED;
        return 1;
    }

    return pConv->fAdvance( pConv, iov, out_size, max_data );
}


int32_t ocoms_convertor_unpack( ocoms_convertor_t* pConv,
                               struct iovec* iov, uint32_t* out_size,
                               size_t* max_data )
{
    OCOMS_CONVERTOR_SET_STATUS_BEFORE_PACK_UNPACK( pConv, iov, out_size, max_data );

    if( OCOMS_LIKELY(pConv->flags & CONVERTOR_NO_OP) ) {
        /**
         * We are doing conversion on a contiguous datatype on a homogeneous
         * environment. The convertor contain minimal informations, we only
         * use the bConverted to manage the conversion.
         */
        uint32_t i;
        unsigned char* base_pointer;
        size_t pending_length = pConv->local_size - pConv->bConverted;

        *max_data = pending_length;
        ocoms_convertor_get_current_pointer( pConv, (void**)&base_pointer );

        for( i = 0; i < *out_size; i++ ) {
            if( iov[i].iov_len >= pending_length ) {
                goto complete_contiguous_data_unpack;
            }
#if OCOMS_CUDA_SUPPORT
            MEMCPY_CUDA( base_pointer, iov[i].iov_base, iov[i].iov_len, pConv );
#else
            MEMCPY( base_pointer, iov[i].iov_base, iov[i].iov_len );
#endif
            pending_length -= iov[i].iov_len;
            base_pointer += iov[i].iov_len;
        }
        *max_data -= pending_length;
        pConv->bConverted += (*max_data);
        return 0;

complete_contiguous_data_unpack:
        iov[i].iov_len = pending_length;
#if OCOMS_CUDA_SUPPORT
        MEMCPY_CUDA( base_pointer, iov[i].iov_base, iov[i].iov_len, pConv );
#else
        MEMCPY( base_pointer, iov[i].iov_base, iov[i].iov_len );
#endif
        pConv->bConverted = pConv->local_size;
        *out_size = i + 1;
        pConv->flags |= CONVERTOR_COMPLETED;
        return 1;
    }

    return pConv->fAdvance( pConv, iov, out_size, max_data );
}

static inline int ocoms_convertor_create_stack_with_pos_contig( ocoms_convertor_t* pConvertor,
                                                               size_t starting_point, const size_t* sizes )
{
    dt_stack_t* pStack;   /* pointer to the position on the stack */
    const ocoms_datatype_t* pData = pConvertor->pDesc;
    dt_elem_desc_t* pElems;
    uint32_t count;
    OCOMS_PTRDIFF_TYPE extent;

    pStack = pConvertor->pStack;
    /**
     * The prepare function already make the selection on which data representation
     * we have to use: normal one or the optimized version ?
     */
    pElems = pConvertor->use_desc->desc;

    count = (uint32_t)(starting_point / pData->size);
    extent = pData->ub - pData->lb;

    pStack[0].type     = OCOMS_DATATYPE_LOOP;  /* the first one is always the loop */
    pStack[0].count    = pConvertor->count - count;
    pStack[0].index    = -1;
    pStack[0].disp     = count * extent;

    /* now compute the number of pending bytes */
    count = (uint32_t)(starting_point - count * pData->size);
    /**
     * We save the current displacement starting from the begining
     * of this data.
     */
    if( OCOMS_LIKELY(0 == count) ) {
        pStack[1].type     = pElems->elem.common.type;
        pStack[1].count    = pElems->elem.count;
        pStack[1].disp     = pElems->elem.disp;
    } else {
        pStack[1].type  = OCOMS_DATATYPE_UINT1;
        pStack[1].count = pData->size - count;
        pStack[1].disp  = pData->true_lb + count;
    }
    pStack[1].index    = 0;  /* useless */

    pConvertor->bConverted = starting_point;
    pConvertor->stack_pos = 1;
    assert( 0 == pConvertor->partial_length );
    return OCOMS_SUCCESS;
}

static inline
int ocoms_convertor_create_stack_at_begining( ocoms_convertor_t* convertor,
                                             const size_t* sizes )
{
    dt_stack_t* pStack = convertor->pStack;
    dt_elem_desc_t* pElems;

    /**
     * The prepare function already make the selection on which data representation
     * we have to use: normal one or the optimized version ?
     */
    pElems = convertor->use_desc->desc;

    convertor->stack_pos      = 1;
    convertor->partial_length = 0;
    convertor->bConverted     = 0;
    /**
     * Fill the first position on the stack. This one correspond to the
     * last fake OCOMS_DATATYPE_END_LOOP that we add to the data representation and
     * allow us to move quickly inside the datatype when we have a count.
     */
    pStack[0].index = -1;
    pStack[0].count = convertor->count;
    pStack[0].disp  = 0;

    pStack[1].index = 0;
    pStack[1].disp = 0;
    if( pElems[0].elem.common.type == OCOMS_DATATYPE_LOOP ) {
        pStack[1].count = pElems[0].loop.loops;
    } else {
        pStack[1].count = pElems[0].elem.count;
    }
    return OCOMS_SUCCESS;
}


int32_t ocoms_convertor_set_position_nocheck( ocoms_convertor_t* convertor,
                                             size_t* position )
{
    int32_t rc;

    /**
     * If we plan to rollback the convertor then first we have to set it
     * at the beginning.
     */
    if( (0 == (*position)) || ((*position) < convertor->bConverted) ) {
        rc = ocoms_convertor_create_stack_at_begining( convertor, ocoms_datatype_local_sizes );
        if( 0 == (*position) ) return rc;
    }
    if( OCOMS_LIKELY(convertor->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS) ) {
        rc = ocoms_convertor_create_stack_with_pos_contig( convertor, (*position),
                                                          ocoms_datatype_local_sizes );
    } else {
        rc = ocoms_convertor_generic_simple_position( convertor, position );
    }
    *position = convertor->bConverted;
    return rc;
}


/**
 * Compute the remote size.
 */
#if OCOMS_ENABLE_HETEROGENEOUS_SUPPORT
#define OCOMS_CONVERTOR_COMPUTE_REMOTE_SIZE(convertor, datatype, bdt_mask) \
{                                                                         \
    if( OCOMS_UNLIKELY(0 != (bdt_mask)) ) {                                \
        ocoms_convertor_master_t* master;                                  \
        int i;                                                            \
        uint32_t mask = datatype->bdt_used;                               \
        convertor->flags ^= CONVERTOR_HOMOGENEOUS;                        \
        master = convertor->master;                                       \
        convertor->remote_size = 0;                                       \
        for( i = OCOMS_DATATYPE_FIRST_TYPE; mask && (i < OCOMS_DATATYPE_MAX_PREDEFINED); i++ ) { \
            if( mask & ((uint32_t)1 << i) ) {                             \
                convertor->remote_size += (datatype->btypes[i] *          \
                                           master->remote_sizes[i]);      \
                mask ^= ((uint32_t)1 << i);                               \
            }                                                             \
        }                                                                 \
        convertor->remote_size *= convertor->count;                       \
        convertor->use_desc = &(datatype->desc);                          \
    }                                                                     \
}
#else
#define OCOMS_CONVERTOR_COMPUTE_REMOTE_SIZE(convertor, datatype, bdt_mask) \
    assert(0 == (bdt_mask))
#endif  /* OCOMS_ENABLE_HETEROGENEOUS_SUPPORT */

/**
 * This macro will initialize a convertor based on a previously created
 * convertor. The idea is the move outside these function the heavy
 * selection of architecture features for the convertors. I consider
 * here that the convertor is clean, either never initialized or already
 * cleaned.
 */
#define OCOMS_CONVERTOR_PREPARE( convertor, datatype, count, pUserBuf )  \
    {                                                                   \
        uint32_t bdt_mask;                                              \
                                                                        \
        /* If the data is empty we just mark the convertor as           \
         * completed. With this flag set the pack and unpack functions  \
         * will not do anything.                                        \
         */                                                             \
        if( OCOMS_UNLIKELY((0 == count) || (0 == datatype->size)) ) {    \
            convertor->flags |= OCOMS_DATATYPE_FLAG_NO_GAPS | CONVERTOR_COMPLETED;  \
            convertor->local_size = convertor->remote_size = 0;         \
            return OCOMS_SUCCESS;                                        \
        }                                                               \
        /* Compute the local in advance */                              \
        convertor->local_size = count * datatype->size;                 \
        convertor->pBaseBuf   = (unsigned char*)pUserBuf;               \
        convertor->count      = count;                                  \
                                                                        \
        /* Grab the datatype part of the flags */                       \
        convertor->flags     &= CONVERTOR_TYPE_MASK;                    \
        convertor->flags     |= (CONVERTOR_DATATYPE_MASK & datatype->flags); \
        convertor->flags     |= (CONVERTOR_NO_OP | CONVERTOR_HOMOGENEOUS); \
        convertor->pDesc      = (ocoms_datatype_t*)datatype;             \
        convertor->bConverted = 0;                                      \
        /* By default consider the optimized description */             \
        convertor->use_desc = &(datatype->opt_desc);                    \
                                                                        \
        convertor->remote_size = convertor->local_size;                 \
        if( OCOMS_LIKELY(convertor->remoteArch == ocoms_local_arch) ) {   \
            if( (convertor->flags & (CONVERTOR_WITH_CHECKSUM | OCOMS_DATATYPE_FLAG_NO_GAPS)) == OCOMS_DATATYPE_FLAG_NO_GAPS ) { \
                return OCOMS_SUCCESS;                                    \
            }                                                           \
            if( ((convertor->flags & (CONVERTOR_WITH_CHECKSUM | OCOMS_DATATYPE_FLAG_CONTIGUOUS)) \
                 == OCOMS_DATATYPE_FLAG_CONTIGUOUS) && (1 == count) ) {             \
                return OCOMS_SUCCESS;                                    \
            }                                                           \
        }                                                               \
                                                                        \
        bdt_mask = datatype->bdt_used & convertor->master->hetero_mask; \
        OCOMS_CONVERTOR_COMPUTE_REMOTE_SIZE( convertor, datatype,        \
                                            bdt_mask );                 \
        assert( NULL != convertor->use_desc->desc );                    \
        /* For predefined datatypes (contiguous) do nothing more */     \
        /* if checksum is enabled then always continue */               \
        if( ((convertor->flags & (CONVERTOR_WITH_CHECKSUM | OCOMS_DATATYPE_FLAG_NO_GAPS)) \
             == OCOMS_DATATYPE_FLAG_NO_GAPS) &&                                     \
            (convertor->flags & (CONVERTOR_SEND | CONVERTOR_HOMOGENEOUS)) ) { \
            return OCOMS_SUCCESS;                                        \
        }                                                               \
        convertor->flags &= ~CONVERTOR_NO_OP;                           \
        {                                                               \
            uint32_t required_stack_length = datatype->btypes[OCOMS_DATATYPE_LOOP] + 1; \
                                                                        \
            if( required_stack_length > convertor->stack_size ) {       \
                convertor->stack_size = required_stack_length;          \
                convertor->pStack     = (dt_stack_t*)malloc(sizeof(dt_stack_t) * \
                                                            convertor->stack_size ); \
            } else {                                                    \
                convertor->pStack = convertor->static_stack;            \
                convertor->stack_size = DT_STATIC_STACK_SIZE;           \
            }                                                           \
        }                                                               \
        ocoms_convertor_create_stack_at_begining( convertor, ocoms_datatype_local_sizes ); \
    }


int32_t ocoms_convertor_prepare_for_recv( ocoms_convertor_t* convertor,
                                         const struct ocoms_datatype_t* datatype,
                                         int32_t count,
                                         const void* pUserBuf )
{
    /* Here I should check that the data is not overlapping */

    convertor->flags |= CONVERTOR_RECV;
#if OCOMS_CUDA_SUPPORT
    mca_cuda_convertor_init(convertor, pUserBuf);
#endif

    OCOMS_CONVERTOR_PREPARE( convertor, datatype, count, pUserBuf );

    if( convertor->flags & CONVERTOR_WITH_CHECKSUM ) {
#if OCOMS_ENABLE_HETEROGENEOUS_SUPPORT
        if( !(convertor->flags & CONVERTOR_HOMOGENEOUS) ) {
            convertor->fAdvance = ocoms_unpack_general_checksum;
        } else
#endif
        if( convertor->pDesc->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS ) {
            convertor->fAdvance = ocoms_unpack_homogeneous_contig_checksum;
        } else {
            convertor->fAdvance = ocoms_generic_simple_unpack_checksum;
        }
    } else {
#if OCOMS_ENABLE_HETEROGENEOUS_SUPPORT
        if( !(convertor->flags & CONVERTOR_HOMOGENEOUS) ) {
            convertor->fAdvance = ocoms_unpack_general;
        } else
#endif
        if( convertor->pDesc->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS ) {
            convertor->fAdvance = ocoms_unpack_homogeneous_contig;
        } else {
            convertor->fAdvance = ocoms_generic_simple_unpack;
        }
    }
    return OCOMS_SUCCESS;
}


int32_t ocoms_convertor_prepare_for_send( ocoms_convertor_t* convertor,
                                         const struct ocoms_datatype_t* datatype,
                                         int32_t count,
                                         const void* pUserBuf )
{
    convertor->flags |= CONVERTOR_SEND;
#if OCOMS_CUDA_SUPPORT
    mca_cuda_convertor_init(convertor, pUserBuf);
#endif

    OCOMS_CONVERTOR_PREPARE( convertor, datatype, count, pUserBuf );

    if( convertor->flags & CONVERTOR_WITH_CHECKSUM ) {
        if( datatype->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS ) {
            if( ((datatype->ub - datatype->lb) == (OCOMS_PTRDIFF_TYPE)datatype->size)
                || (1 >= convertor->count) )
                convertor->fAdvance = ocoms_pack_homogeneous_contig_checksum;
            else
                convertor->fAdvance = ocoms_pack_homogeneous_contig_with_gaps_checksum;
        } else {
            convertor->fAdvance = ocoms_generic_simple_pack_checksum;
        }
    } else {
        if( datatype->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS ) {
            if( ((datatype->ub - datatype->lb) == (OCOMS_PTRDIFF_TYPE)datatype->size)
                || (1 >= convertor->count) )
                convertor->fAdvance = ocoms_pack_homogeneous_contig;
            else
                convertor->fAdvance = ocoms_pack_homogeneous_contig_with_gaps;
        } else {
            convertor->fAdvance = ocoms_generic_simple_pack;
        }
    }
    return OCOMS_SUCCESS;
}

/*
 * These functions can be used in order to create an IDENTICAL copy of one convertor. In this
 * context IDENTICAL means that the datatype and count and all other properties of the basic
 * convertor get replicated on this new convertor. However, the references to the datatype
 * are not increased. This function take special care about the stack. If all the cases the
 * stack is created with the correct number of entries but if the copy_stack is true (!= 0)
 * then the content of the old stack is copied on the new one. The result will be a convertor
 * ready to use starting from the old position. If copy_stack is false then the convertor
 * is created with a empty stack (you have to use ocoms_convertor_set_position before using it).
 */
int ocoms_convertor_clone( const ocoms_convertor_t* source,
                          ocoms_convertor_t* destination,
                          int32_t copy_stack )
{
    destination->remoteArch        = source->remoteArch;
    destination->flags             = source->flags;
    destination->pDesc             = source->pDesc;
    destination->use_desc          = source->use_desc;
    destination->count             = source->count;
    destination->pBaseBuf          = source->pBaseBuf;
    destination->fAdvance          = source->fAdvance;
    destination->master            = source->master;
    destination->local_size        = source->local_size;
    destination->remote_size       = source->remote_size;
    /* create the stack */
    if( OCOMS_UNLIKELY(source->stack_size > DT_STATIC_STACK_SIZE) ) {
        destination->pStack = (dt_stack_t*)malloc(sizeof(dt_stack_t) * source->stack_size );
    } else {
        destination->pStack = destination->static_stack;
    }
    destination->stack_size = source->stack_size;

    /* initialize the stack */
    if( OCOMS_LIKELY(0 == copy_stack) ) {
        destination->bConverted = -1;
        destination->stack_pos  = -1;
    } else {
        memcpy( destination->pStack, source->pStack, sizeof(dt_stack_t) * (source->stack_pos+1) );
        destination->bConverted = source->bConverted;
        destination->stack_pos  = source->stack_pos;
    }
#if OCOMS_CUDA_SUPPORT
    destination->cbmemcpy   = source->cbmemcpy;
#endif
    return OCOMS_SUCCESS;
}


void ocoms_convertor_dump( ocoms_convertor_t* convertor )
{
    printf( "Convertor %p count %d stack position %d bConverted %ld\n", (void*)convertor,
            convertor->count, convertor->stack_pos, (unsigned long)convertor->bConverted );
    printf( "\tlocal_size %ld remote_size %ld flags %X stack_size %d pending_length %d\n",
            (unsigned long)convertor->local_size, (unsigned long)convertor->remote_size,
            convertor->flags, convertor->stack_size, convertor->partial_length );
    ocoms_datatype_dump( convertor->pDesc );
    printf( "Actual stack representation\n" );
    ocoms_datatype_dump_stack( convertor->pStack, convertor->stack_pos,
                              convertor->pDesc->desc.desc, convertor->pDesc->name );
}


void ocoms_datatype_dump_stack( const dt_stack_t* pStack, int stack_pos,
                               const union dt_elem_desc* pDesc, const char* name )
{
    ocoms_output( 0, "\nStack %p stack_pos %d name %s\n", (void*)pStack, stack_pos, name );
    for( ; stack_pos >= 0; stack_pos-- ) {
        ocoms_output( 0, "%d: pos %d count %d disp %ld ", stack_pos, pStack[stack_pos].index,
                     (int)pStack[stack_pos].count, (long)pStack[stack_pos].disp );
        if( pStack->index != -1 )
            ocoms_output( 0, "\t[desc count %d disp %ld extent %ld]\n",
                         pDesc[pStack[stack_pos].index].elem.count,
                         (long)pDesc[pStack[stack_pos].index].elem.disp,
                         (long)pDesc[pStack[stack_pos].index].elem.extent );
        else
            ocoms_output( 0, "\n" );
    }
    ocoms_output( 0, "\n" );
}
