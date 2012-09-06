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
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2009      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/**
 * service_datatype_t interface for OPAL internal data type representation
 *
 * service_datatype_t is a class which represents contiguous or
 * non-contiguous data together with constituent type-related
 * information.
 */

#ifndef CCS_DATATYPE_H_HAS_BEEN_INCLUDED
#define CCS_DATATYPE_H_HAS_BEEN_INCLUDED

#include "ccs_config.h"

#include <stddef.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "service/util/service_object.h"
#include "service/datatype/service_datatype.h"

BEGIN_C_DECLS

/*
 * If there are more basic datatypes than the number of bytes in the int type
 * the bdt_used field of the data description struct should be changed to long.
 *
 * This must match the same definition as in service_datatype_internal.h
 */
#define CCS_DATATYPE_MAX_PREDEFINED 25
/*
 * No more than this number of _Basic_ datatypes in C/CPP or Fortran
 * are supported (in order to not change setup and usage of btypes).
 *
 * XXX TODO Adapt to whatever the OMPI-layer needs
 */
#define CCS_DATATYPE_MAX_SUPPORTED  46


/* flags for the datatypes. */
#define CCS_DATATYPE_FLAG_UNAVAILABLE   0x0001  /**< datatypes unavailable on the build (OS or compiler dependant) */
#define CCS_DATATYPE_FLAG_PREDEFINED    0x0002  /**< cannot be removed: initial and predefined datatypes */
#define CCS_DATATYPE_FLAG_COMMITED      0x0004  /**< ready to be used for a send/recv operation */
#define CCS_DATATYPE_FLAG_OVERLAP       0x0008  /**< datatype is unpropper for a recv operation */
#define CCS_DATATYPE_FLAG_CONTIGUOUS    0x0010  /**< contiguous datatype */
#define CCS_DATATYPE_FLAG_NO_GAPS       0x0020  /**< no gaps around the datatype, aka CCS_DATATYPE_FLAG_CONTIGUOUS and extent == size */
#define CCS_DATATYPE_FLAG_USER_LB       0x0040  /**< has a user defined LB */
#define CCS_DATATYPE_FLAG_USER_UB       0x0080  /**< has a user defined UB */
#define CCS_DATATYPE_FLAG_DATA          0x0100  /**< data or control structure */
/*
 * We should make the difference here between the predefined contiguous and non contiguous
 * datatypes. The CCS_DATATYPE_FLAG_BASIC is held by all predefined contiguous datatypes.
 */
#define CCS_DATATYPE_FLAG_BASIC         (CCS_DATATYPE_FLAG_PREDEFINED | \
                                          CCS_DATATYPE_FLAG_CONTIGUOUS | \
                                          CCS_DATATYPE_FLAG_NO_GAPS |    \
                                          CCS_DATATYPE_FLAG_DATA |       \
                                          CCS_DATATYPE_FLAG_COMMITED)


/**
 * The number of supported entries in the data-type definition and the
 * associated type.
 */
#define MAX_DT_COMPONENT_COUNT UINT_MAX
typedef uint32_t service_datatype_count_t;

typedef union dt_elem_desc dt_elem_desc_t;

struct dt_type_desc_t {
    service_datatype_count_t  length;  /**< the maximum number of elements in the description array */
    service_datatype_count_t  used;    /**< the number of used elements in the description array */
    dt_elem_desc_t*        desc;
};
typedef struct dt_type_desc_t dt_type_desc_t;


/*
 * The datatype description.
 */
struct service_datatype_t {
    service_object_t      super;    /**< basic superclass */
    uint16_t           flags;    /**< the flags */
    uint16_t           id;       /**< data id, normally the index in the data array. */
    uint32_t           bdt_used; /**< bitset of which basic datatypes are used in the data description */
    size_t             size;     /**< total size in bytes of the memory used by the data if
                                      the data is put on a contiguous buffer */
    CCS_PTRDIFF_TYPE  true_lb;  /**< the true lb of the data without user defined lb and ub */
    CCS_PTRDIFF_TYPE  true_ub;  /**< the true ub of the data without user defined lb and ub */
    CCS_PTRDIFF_TYPE  lb;       /**< lower bound in memory */
    CCS_PTRDIFF_TYPE  ub;       /**< upper bound in memory */
    /* --- cacheline 1 boundary (64 bytes) --- */
    uint32_t           align;    /**< data should be aligned to */
    uint32_t           nbElems;  /**< total number of elements inside the datatype */

    /* Attribute fields */
    char               name[CCS_MAX_OBJECT_NAME];  /**< name of the datatype */
    /* --- cacheline 2 boundary (128 bytes) was 8 bytes ago --- */
    dt_type_desc_t     desc;     /**< the data description */
    dt_type_desc_t     opt_desc; /**< short description of the data used when conversion is useless
                                      or in the send case (without conversion) */

    uint32_t           btypes[CCS_DATATYPE_MAX_SUPPORTED];
                                 /**< basic elements count used to compute the size of the
                                      datatype for remote nodes. The length of the array is dependent on
                                      the maximum number of datatypes of all top layers.
                                      Reason being is that Fortran is not at the OPAL layer. */
    /* --- cacheline 5 boundary (320 bytes) was 32 bytes ago --- */

    /* size: 352, cachelines: 6, members: 15 */
    /* last cacheline: 32 bytes */
};

typedef struct service_datatype_t service_datatype_t;

CCS_DECLSPEC OBJ_CLASS_DECLARATION( service_datatype_t );

CCS_DECLSPEC extern const service_datatype_t* service_datatype_basicDatatypes[CCS_DATATYPE_MAX_PREDEFINED];
CCS_DECLSPEC extern const size_t service_datatype_local_sizes[CCS_DATATYPE_MAX_PREDEFINED];

/* Local Architecture as provided by service_arch_compute_local_id() */
CCS_DECLSPEC extern uint32_t service_local_arch;

/*
 * The OPAL-layer's Basic datatypes themselves.
 */
CCS_DECLSPEC extern const service_datatype_t service_datatype_empty;
CCS_DECLSPEC extern const service_datatype_t service_datatype_loop;
CCS_DECLSPEC extern const service_datatype_t service_datatype_end_loop;
CCS_DECLSPEC extern const service_datatype_t service_datatype_lb;
CCS_DECLSPEC extern const service_datatype_t service_datatype_ub;
CCS_DECLSPEC extern const service_datatype_t service_datatype_int1;       /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_int2;       /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_int4;       /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_int8;       /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_int16;      /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_uint1;      /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_uint2;      /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_uint4;      /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_uint8;      /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_uint16;     /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_float2;     /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_float4;     /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_float8;     /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_float12;    /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_float16;    /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_complex8;   /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_complex16;  /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_complex32;  /* in bytes */
CCS_DECLSPEC extern const service_datatype_t service_datatype_bool;
CCS_DECLSPEC extern const service_datatype_t service_datatype_wchar;


/*
 * Functions exported externally
 */
CCS_DECLSPEC int service_datatype_register_params(void);
CCS_DECLSPEC int32_t service_datatype_init( void );
CCS_DECLSPEC int32_t service_datatype_finalize( void );
CCS_DECLSPEC service_datatype_t* service_datatype_create( int32_t expectedSize );
CCS_DECLSPEC int32_t service_datatype_create_desc( service_datatype_t * datatype, int32_t expectedSize );
CCS_DECLSPEC int32_t service_datatype_commit( service_datatype_t * pData );
CCS_DECLSPEC int32_t service_datatype_destroy( service_datatype_t** );

static inline int32_t
service_datatype_is_committed( const service_datatype_t* type )
{
    return ((type->flags & CCS_DATATYPE_FLAG_COMMITED) == CCS_DATATYPE_FLAG_COMMITED);
}

static inline int32_t
service_datatype_is_overlapped( const service_datatype_t* type )
{
    return ((type->flags & CCS_DATATYPE_FLAG_OVERLAP) == CCS_DATATYPE_FLAG_OVERLAP);
}

static inline int32_t
service_datatype_is_valid( const service_datatype_t* type )
{
    return !((type->flags & CCS_DATATYPE_FLAG_UNAVAILABLE) == CCS_DATATYPE_FLAG_UNAVAILABLE);
}

static inline int32_t
service_datatype_is_predefined( const service_datatype_t* type )
{
    return (type->flags & CCS_DATATYPE_FLAG_PREDEFINED);
}

/*
 * This function return true (1) if the datatype representation depending on the count
 * is contiguous in the memory. And false (0) otherwise.
 */
static inline int32_t
service_datatype_is_contiguous_memory_layout( const service_datatype_t* datatype, int32_t count )
{
    if( !(datatype->flags & CCS_DATATYPE_FLAG_CONTIGUOUS) ) return 0;
    if( (count == 1) || (datatype->flags & CCS_DATATYPE_FLAG_NO_GAPS) ) return 1;
    assert( (CCS_PTRDIFF_TYPE)datatype->size != (datatype->ub - datatype->lb) );
    return 0;
}


CCS_DECLSPEC void service_datatype_dump( const service_datatype_t* pData );
/* data creation functions */
CCS_DECLSPEC int32_t service_datatype_clone( const service_datatype_t * src_type, service_datatype_t * dest_type );
CCS_DECLSPEC int32_t service_datatype_create_contiguous( int count, const service_datatype_t* oldType, service_datatype_t** newType );
CCS_DECLSPEC int32_t service_datatype_resize( service_datatype_t* type, CCS_PTRDIFF_TYPE lb, CCS_PTRDIFF_TYPE extent );
CCS_DECLSPEC int32_t service_datatype_add( service_datatype_t* pdtBase, const service_datatype_t* pdtAdd, uint32_t count,
                                         CCS_PTRDIFF_TYPE disp, CCS_PTRDIFF_TYPE extent );

static inline int32_t
service_datatype_type_lb( const service_datatype_t* pData, CCS_PTRDIFF_TYPE* disp )
{
    *disp = pData->lb;
    return 0;
}

static inline int32_t
service_datatype_type_ub( const service_datatype_t* pData, CCS_PTRDIFF_TYPE* disp )
{
    *disp = pData->ub;
    return 0;
}

static inline int32_t
service_datatype_type_size( const service_datatype_t* pData, size_t *size )
{
    *size = pData->size;
    return 0;
}

static inline int32_t
service_datatype_type_extent( const service_datatype_t* pData, CCS_PTRDIFF_TYPE* extent )
{
    *extent = pData->ub - pData->lb;
    return 0;
}

static inline int32_t
service_datatype_get_extent( const service_datatype_t* pData, CCS_PTRDIFF_TYPE* lb, CCS_PTRDIFF_TYPE* extent)
{
    *lb = pData->lb; *extent = pData->ub - pData->lb;
    return 0;
}

static inline int32_t
service_datatype_get_true_extent( const service_datatype_t* pData, CCS_PTRDIFF_TYPE* true_lb, CCS_PTRDIFF_TYPE* true_extent)
{
    *true_lb = pData->true_lb;
    *true_extent = (pData->true_ub - pData->true_lb);
    return 0;
}

CCS_DECLSPEC int32_t
service_datatype_get_element_count( const service_datatype_t* pData, size_t iSize );
CCS_DECLSPEC int32_t
service_datatype_set_element_count( const service_datatype_t* pData, uint32_t count, size_t* length );
CCS_DECLSPEC int32_t
service_datatype_copy_content_same_ddt( const service_datatype_t* pData, int32_t count,
                                     char* pDestBuf, char* pSrcBuf );

CCS_DECLSPEC const service_datatype_t*
service_datatype_match_size( int size, uint16_t datakind, uint16_t datalang );

/*
 *
 */
CCS_DECLSPEC int32_t
service_datatype_sndrcv( void *sbuf, int32_t scount, const service_datatype_t* sdtype, void *rbuf,
                      int32_t rcount, const service_datatype_t* rdtype);

/*
 *
 */
CCS_DECLSPEC int32_t
service_datatype_get_args( const service_datatype_t* pData, int32_t which,
                        int32_t * ci, int32_t * i,
                        int32_t * ca, CCS_PTRDIFF_TYPE* a,
                        int32_t * cd, service_datatype_t** d, int32_t * type);
CCS_DECLSPEC int32_t
service_datatype_set_args( service_datatype_t* pData,
                        int32_t ci, int32_t ** i,
                        int32_t ca, CCS_PTRDIFF_TYPE* a,
                        int32_t cd, service_datatype_t** d,int32_t type);
CCS_DECLSPEC int32_t
service_datatype_copy_args( const service_datatype_t* source_data,
                         service_datatype_t* dest_data );
CCS_DECLSPEC int32_t
service_datatype_release_args( service_datatype_t* pData );

/*
 *
 */
CCS_DECLSPEC size_t
service_datatype_pack_description_length( const service_datatype_t* datatype );

/*
 *
 */
CCS_DECLSPEC int
service_datatype_get_pack_description( service_datatype_t* datatype,
                                    const void** packed_buffer );

/*
 *
 */
struct ccs_proc_t;
CCS_DECLSPEC service_datatype_t*
service_datatype_create_from_packed_description( void** packed_buffer,
                                              struct ccs_proc_t* remote_processor );

#if CCS_ENABLE_DEBUG
/*
 * Set a breakpoint to this function in your favorite debugger
 * to make it stop on all pack and unpack errors.
 */
CCS_DECLSPEC int
service_datatype_safeguard_pointer_debug_breakpoint( const void* actual_ptr, int length,
                                                  const void* initial_ptr,
                                                  const service_datatype_t* pData,
                                                  int count );
#endif  /* CCS_ENABLE_DEBUG */

END_C_DECLS
#endif  /* CCS_DATATYPE_H_HAS_BEEN_INCLUDED */
