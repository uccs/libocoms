/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
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
 * Copyright (c) 2007-2013 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2009      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/**
 * ocoms_datatype_t interface for OCOMS internal data type representation
 *
 * ocoms_datatype_t is a class which represents contiguous or
 * non-contiguous data together with constituent type-related
 * information.
 */

#ifndef OCOMS_DATATYPE_H_HAS_BEEN_INCLUDED
#define OCOMS_DATATYPE_H_HAS_BEEN_INCLUDED

#include "ocoms/platform/ocoms_config.h"

#include <stddef.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "ocoms/util/ocoms_object.h"

BEGIN_C_DECLS

/*
 * If there are more basic datatypes than the number of bytes in the int type
 * the bdt_used field of the data description struct should be changed to long.
 *
 * This must match the same definition as in ocoms_datatype_internal.h
 */
#if !defined(OCOMS_DATATYPE_MAX_PREDEFINED)
#define OCOMS_DATATYPE_MAX_PREDEFINED 25
#endif
/*
 * No more than this number of _Basic_ datatypes in C/CPP or Fortran
 * are supported (in order to not change setup and usage of btypes).
 *
 * XXX TODO Adapt to whatever the OMPI-layer needs
 */
#define OCOMS_DATATYPE_MAX_SUPPORTED  47


/* flags for the datatypes. */
#define OCOMS_DATATYPE_FLAG_UNAVAILABLE   0x0001  /**< datatypes unavailable on the build (OS or compiler dependant) */
#define OCOMS_DATATYPE_FLAG_PREDEFINED    0x0002  /**< cannot be removed: initial and predefined datatypes */
#define OCOMS_DATATYPE_FLAG_COMMITED      0x0004  /**< ready to be used for a send/recv operation */
#define OCOMS_DATATYPE_FLAG_OVERLAP       0x0008  /**< datatype is unpropper for a recv operation */
#define OCOMS_DATATYPE_FLAG_CONTIGUOUS    0x0010  /**< contiguous datatype */
#define OCOMS_DATATYPE_FLAG_NO_GAPS       0x0020  /**< no gaps around the datatype, aka OCOMS_DATATYPE_FLAG_CONTIGUOUS and extent == size */
#define OCOMS_DATATYPE_FLAG_USER_LB       0x0040  /**< has a user defined LB */
#define OCOMS_DATATYPE_FLAG_USER_UB       0x0080  /**< has a user defined UB */
#define OCOMS_DATATYPE_FLAG_DATA          0x0100  /**< data or control structure */
/*
 * We should make the difference here between the predefined contiguous and non contiguous
 * datatypes. The OCOMS_DATATYPE_FLAG_BASIC is held by all predefined contiguous datatypes.
 */
#define OCOMS_DATATYPE_FLAG_BASIC         (OCOMS_DATATYPE_FLAG_PREDEFINED | \
                                          OCOMS_DATATYPE_FLAG_CONTIGUOUS | \
                                          OCOMS_DATATYPE_FLAG_NO_GAPS |    \
                                          OCOMS_DATATYPE_FLAG_DATA |       \
                                          OCOMS_DATATYPE_FLAG_COMMITED)


/**
 * The number of supported entries in the data-type definition and the
 * associated type.
 */
#define MAX_DT_COMPONENT_COUNT UINT_MAX
typedef uint32_t ocoms_datatype_count_t;

typedef union dt_elem_desc dt_elem_desc_t;

struct dt_type_desc_t {
    ocoms_datatype_count_t  length;  /**< the maximum number of elements in the description array */
    ocoms_datatype_count_t  used;    /**< the number of used elements in the description array */
    dt_elem_desc_t*        desc;
};
typedef struct dt_type_desc_t dt_type_desc_t;


/*
 * The datatype description.
 */
struct ocoms_datatype_t {
    ocoms_object_t      super;    /**< basic superclass */
    uint16_t           flags;    /**< the flags */
    uint16_t           id;       /**< data id, normally the index in the data array. */
    uint32_t           bdt_used; /**< bitset of which basic datatypes are used in the data description */
    size_t             size;     /**< total size in bytes of the memory used by the data if
                                      the data is put on a contiguous buffer */
    OCOMS_PTRDIFF_TYPE  true_lb;  /**< the true lb of the data without user defined lb and ub */
    OCOMS_PTRDIFF_TYPE  true_ub;  /**< the true ub of the data without user defined lb and ub */
    OCOMS_PTRDIFF_TYPE  lb;       /**< lower bound in memory */
    OCOMS_PTRDIFF_TYPE  ub;       /**< upper bound in memory */
    /* --- cacheline 1 boundary (64 bytes) --- */
    uint32_t           align;    /**< data should be aligned to */
    size_t             nbElems;  /**< total number of elements inside the datatype */

    /* Attribute fields */
    char               name[OCOMS_MAX_OBJECT_NAME];  /**< name of the datatype */
    /* --- cacheline 2 boundary (128 bytes) was 8-12 bytes ago --- */
    dt_type_desc_t     desc;     /**< the data description */
    dt_type_desc_t     opt_desc; /**< short description of the data used when conversion is useless
                                      or in the send case (without conversion) */

    uint32_t           btypes[OCOMS_DATATYPE_MAX_SUPPORTED];
                                 /**< basic elements count used to compute the size of the
                                      datatype for remote nodes. The length of the array is dependent on
                                      the maximum number of datatypes of all top layers.
                                      Reason being is that Fortran is not at the OCOMS layer. */
    /* --- cacheline 5 boundary (320 bytes) was 32-36 bytes ago --- */

    /* size: 352, cachelines: 6, members: 15 */
    /* last cacheline: 28-32 bytes */
};

typedef struct ocoms_datatype_t ocoms_datatype_t;

OCOMS_DECLSPEC OBJ_CLASS_DECLARATION( ocoms_datatype_t );

OCOMS_DECLSPEC extern const ocoms_datatype_t* ocoms_datatype_basicDatatypes[OCOMS_DATATYPE_MAX_PREDEFINED];
OCOMS_DECLSPEC extern const size_t ocoms_datatype_local_sizes[OCOMS_DATATYPE_MAX_PREDEFINED];

/* Local Architecture as provided by ocoms_arch_compute_local_id() */
OCOMS_DECLSPEC extern uint32_t ocoms_local_arch;

/*
 * The OCOMS-layer's Basic datatypes themselves.
 */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_empty;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_loop;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_end_loop;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_lb;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_ub;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_int1;       /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_int2;       /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_int4;       /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_int8;       /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_int16;      /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_uint1;      /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_uint2;      /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_uint4;      /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_uint8;      /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_uint16;     /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_float2;     /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_float4;     /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_float8;     /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_float12;    /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_float16;    /* in bytes */
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_float_complex;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_double_complex;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_long_double_complex;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_bool;
OCOMS_DECLSPEC extern const ocoms_datatype_t ocoms_datatype_wchar;


/*
 * Functions exported externally
 */
int ocoms_datatype_register_params(void);
OCOMS_DECLSPEC int32_t ocoms_datatype_init( void );
OCOMS_DECLSPEC int32_t ocoms_datatype_finalize( void );
OCOMS_DECLSPEC ocoms_datatype_t* ocoms_datatype_create( int32_t expectedSize );
OCOMS_DECLSPEC int32_t ocoms_datatype_create_desc( ocoms_datatype_t * datatype, int32_t expectedSize );
OCOMS_DECLSPEC int32_t ocoms_datatype_commit( ocoms_datatype_t * pData );
OCOMS_DECLSPEC int32_t ocoms_datatype_destroy( ocoms_datatype_t** );

static inline int32_t
ocoms_datatype_is_committed( const ocoms_datatype_t* type )
{
    return ((type->flags & OCOMS_DATATYPE_FLAG_COMMITED) == OCOMS_DATATYPE_FLAG_COMMITED);
}

static inline int32_t
ocoms_datatype_is_overlapped( const ocoms_datatype_t* type )
{
    return ((type->flags & OCOMS_DATATYPE_FLAG_OVERLAP) == OCOMS_DATATYPE_FLAG_OVERLAP);
}

static inline int32_t
ocoms_datatype_is_valid( const ocoms_datatype_t* type )
{
    return !((type->flags & OCOMS_DATATYPE_FLAG_UNAVAILABLE) == OCOMS_DATATYPE_FLAG_UNAVAILABLE);
}

static inline int32_t
ocoms_datatype_is_predefined( const ocoms_datatype_t* type )
{
    return (type->flags & OCOMS_DATATYPE_FLAG_PREDEFINED);
}

/*
 * This function return true (1) if the datatype representation depending on the count
 * is contiguous in the memory. And false (0) otherwise.
 */
static inline int32_t
ocoms_datatype_is_contiguous_memory_layout( const ocoms_datatype_t* datatype, int32_t count )
{
    if( !(datatype->flags & OCOMS_DATATYPE_FLAG_CONTIGUOUS) ) return 0;
    if( (count == 1) || (datatype->flags & OCOMS_DATATYPE_FLAG_NO_GAPS) ) return 1;
    return 0;
}


OCOMS_DECLSPEC void ocoms_datatype_dump( const ocoms_datatype_t* pData );
OCOMS_DECLSPEC void ocoms_datatype_dump_v2( const ocoms_datatype_t* pData, bool dump_to_stderr );
/* data creation functions */
OCOMS_DECLSPEC int32_t ocoms_datatype_clone( const ocoms_datatype_t * src_type, ocoms_datatype_t * dest_type );
OCOMS_DECLSPEC int32_t ocoms_datatype_create_contiguous( int count, const ocoms_datatype_t* oldType, ocoms_datatype_t** newType );
OCOMS_DECLSPEC int32_t ocoms_datatype_resize( ocoms_datatype_t* type, OCOMS_PTRDIFF_TYPE lb, OCOMS_PTRDIFF_TYPE extent );
OCOMS_DECLSPEC int32_t ocoms_datatype_add( ocoms_datatype_t* pdtBase, const ocoms_datatype_t* pdtAdd, uint32_t count,
                                         OCOMS_PTRDIFF_TYPE disp, OCOMS_PTRDIFF_TYPE extent );

static inline int32_t
ocoms_datatype_type_lb( const ocoms_datatype_t* pData, OCOMS_PTRDIFF_TYPE* disp )
{
    *disp = pData->lb;
    return 0;
}

static inline int32_t
ocoms_datatype_type_ub( const ocoms_datatype_t* pData, OCOMS_PTRDIFF_TYPE* disp )
{
    *disp = pData->ub;
    return 0;
}

static inline int32_t
ocoms_datatype_type_size( const ocoms_datatype_t* pData, size_t *size )
{
    *size = pData->size;
    return 0;
}

static inline int32_t
ocoms_datatype_type_extent( const ocoms_datatype_t* pData, OCOMS_PTRDIFF_TYPE* extent )
{
    *extent = pData->ub - pData->lb;
    return 0;
}

static inline int32_t
ocoms_datatype_get_extent( const ocoms_datatype_t* pData, OCOMS_PTRDIFF_TYPE* lb, OCOMS_PTRDIFF_TYPE* extent)
{
    *lb = pData->lb; *extent = pData->ub - pData->lb;
    return 0;
}

static inline int32_t
ocoms_datatype_get_true_extent( const ocoms_datatype_t* pData, OCOMS_PTRDIFF_TYPE* true_lb, OCOMS_PTRDIFF_TYPE* true_extent)
{
    *true_lb = pData->true_lb;
    *true_extent = (pData->true_ub - pData->true_lb);
    return 0;
}

OCOMS_DECLSPEC ssize_t
ocoms_datatype_get_element_count( const ocoms_datatype_t* pData, size_t iSize );
OCOMS_DECLSPEC int32_t
ocoms_datatype_set_element_count( const ocoms_datatype_t* pData, size_t count, size_t* length );
OCOMS_DECLSPEC int32_t
ocoms_datatype_copy_content_same_ddt( const ocoms_datatype_t* pData, int32_t count,
                                     char* pDestBuf, char* pSrcBuf );

OCOMS_DECLSPEC const ocoms_datatype_t*
ocoms_datatype_match_size( int size, uint16_t datakind, uint16_t datalang );

/*
 *
 */
OCOMS_DECLSPEC int32_t
ocoms_datatype_sndrcv( void *sbuf, int32_t scount, const ocoms_datatype_t* sdtype, void *rbuf,
                      int32_t rcount, const ocoms_datatype_t* rdtype);

/*
 *
 */
OCOMS_DECLSPEC int32_t
ocoms_datatype_get_args( const ocoms_datatype_t* pData, int32_t which,
                        int32_t * ci, int32_t * i,
                        int32_t * ca, OCOMS_PTRDIFF_TYPE* a,
                        int32_t * cd, ocoms_datatype_t** d, int32_t * type);
OCOMS_DECLSPEC int32_t
ocoms_datatype_set_args( ocoms_datatype_t* pData,
                        int32_t ci, int32_t ** i,
                        int32_t ca, OCOMS_PTRDIFF_TYPE* a,
                        int32_t cd, ocoms_datatype_t** d,int32_t type);
OCOMS_DECLSPEC int32_t
ocoms_datatype_copy_args( const ocoms_datatype_t* source_data,
                         ocoms_datatype_t* dest_data );
OCOMS_DECLSPEC int32_t
ocoms_datatype_release_args( ocoms_datatype_t* pData );

/*
 *
 */
OCOMS_DECLSPEC size_t
ocoms_datatype_pack_description_length( const ocoms_datatype_t* datatype );

/*
 *
 */
OCOMS_DECLSPEC int
ocoms_datatype_get_pack_description( ocoms_datatype_t* datatype,
                                    const void** packed_buffer );

/*
 *
 */
struct ocoms_proc_t;
OCOMS_DECLSPEC ocoms_datatype_t*
ocoms_datatype_create_from_packed_description( void** packed_buffer,
                                              struct ocoms_proc_t* remote_processor );

#if OCOMS_ENABLE_DEBUG
/*
 * Set a breakpoint to this function in your favorite debugger
 * to make it stop on all pack and unpack errors.
 */
OCOMS_DECLSPEC int
ocoms_datatype_safeguard_pointer_debug_breakpoint( const void* actual_ptr, int length,
                                                  const void* initial_ptr,
                                                  const ocoms_datatype_t* pData,
                                                  int count );
#endif  /* OCOMS_ENABLE_DEBUG */

END_C_DECLS
#endif  /* OCOMS_DATATYPE_H_HAS_BEEN_INCLUDED */
