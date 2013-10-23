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
 * Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2009      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2013      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include <stddef.h>

#include "ocoms/util/arch.h"
#include "ocoms/datatype/ocoms_datatype_internal.h"
#include "ocoms/datatype/ocoms_datatype.h"
#include "ocoms/datatype/ocoms_convertor_internal.h"
#include "ocoms/mca/base/mca_base_var.h"

/* by default the debuging is turned off */
int ocoms_datatype_dfd = -1;
bool ocoms_unpack_debug = false;
bool ocoms_pack_debug = false;
bool ocoms_position_debug = false;
bool ocoms_copy_debug = false;

extern int ocoms_cuda_verbose;

/* Using this macro implies that at this point _all_ informations needed
 * to fill up the datatype are known.
 * We fill all the static information, the pointer to desc.desc is setup
 * into an array, which is initialized at runtime.
 * Everything is constant.
 */
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_empty =       OCOMS_DATATYPE_INITIALIZER_EMPTY(0);

OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_loop =        OCOMS_DATATYPE_INITIALIZER_LOOP(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_end_loop =    OCOMS_DATATYPE_INITIALIZER_END_LOOP(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_lb =          OCOMS_DATATYPE_INITIALIZER_LB(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_ub =          OCOMS_DATATYPE_INITIALIZER_UB(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_int1 =        OCOMS_DATATYPE_INITIALIZER_INT1(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_int2 =        OCOMS_DATATYPE_INITIALIZER_INT2(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_int4 =        OCOMS_DATATYPE_INITIALIZER_INT4(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_int8 =        OCOMS_DATATYPE_INITIALIZER_INT8(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_int16 =       OCOMS_DATATYPE_INITIALIZER_INT16(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_uint1 =       OCOMS_DATATYPE_INITIALIZER_UINT1(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_uint2 =       OCOMS_DATATYPE_INITIALIZER_UINT2(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_uint4 =       OCOMS_DATATYPE_INITIALIZER_UINT4(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_uint8 =       OCOMS_DATATYPE_INITIALIZER_UINT8(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_uint16 =      OCOMS_DATATYPE_INITIALIZER_UINT16(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_float2 =      OCOMS_DATATYPE_INITIALIZER_FLOAT2(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_float4 =      OCOMS_DATATYPE_INITIALIZER_FLOAT4(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_float8 =      OCOMS_DATATYPE_INITIALIZER_FLOAT8(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_float12 =     OCOMS_DATATYPE_INITIALIZER_FLOAT12(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_float16 =     OCOMS_DATATYPE_INITIALIZER_FLOAT16(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_float_complex = OCOMS_DATATYPE_INITIALIZER_FLOAT_COMPLEX(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_double_complex = OCOMS_DATATYPE_INITIALIZER_DOUBLE_COMPLEX(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_long_double_complex = OCOMS_DATATYPE_INITIALIZER_LONG_DOUBLE_COMPLEX(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_bool =        OCOMS_DATATYPE_INITIALIZER_BOOL(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_wchar =       OCOMS_DATATYPE_INITIALIZER_WCHAR(0);
OCOMS_DECLSPEC const ocoms_datatype_t ocoms_datatype_unavailable = OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED(UNAVAILABLE, 0);

OCOMS_DECLSPEC dt_elem_desc_t ocoms_datatype_predefined_elem_desc[2 * OCOMS_DATATYPE_MAX_PREDEFINED];

/*
 * NOTE: The order of this array *MUST* match the order in ocoms_datatype_basicDatatypes
 * (use of designated initializers should relax this restrictions some)
 */
OCOMS_DECLSPEC const size_t ocoms_datatype_local_sizes[OCOMS_DATATYPE_MAX_PREDEFINED] =
{
    [OCOMS_DATATYPE_INT1] = sizeof(int8_t),
    [OCOMS_DATATYPE_INT2] = sizeof(int16_t),
    [OCOMS_DATATYPE_INT4] = sizeof(int32_t),
    [OCOMS_DATATYPE_INT8] = sizeof(int64_t),
    [OCOMS_DATATYPE_INT16] = 16,    /* sizeof (int128_t) */
    [OCOMS_DATATYPE_UINT1] = sizeof(uint8_t),
    [OCOMS_DATATYPE_UINT2] = sizeof(uint16_t),
    [OCOMS_DATATYPE_UINT4] = sizeof(uint32_t),
    [OCOMS_DATATYPE_UINT8] = sizeof(uint64_t),
    [OCOMS_DATATYPE_UINT16] = 16,    /* sizeof (uint128_t) */
    [OCOMS_DATATYPE_FLOAT2] = 2,     /* sizeof (float2) */
    [OCOMS_DATATYPE_FLOAT4] = 4,     /* sizeof (float4) */
    [OCOMS_DATATYPE_FLOAT8] = 8,     /* sizeof (float8) */
    [OCOMS_DATATYPE_FLOAT12] = 12,   /* sizeof (float12) */
    [OCOMS_DATATYPE_FLOAT16] = 16,   /* sizeof (float16) */
    [OCOMS_DATATYPE_FLOAT_COMPLEX] = sizeof(float _Complex),
    [OCOMS_DATATYPE_DOUBLE_COMPLEX] = sizeof(double _Complex),
    [OCOMS_DATATYPE_LONG_DOUBLE_COMPLEX] = sizeof(long double _Complex),
    [OCOMS_DATATYPE_BOOL] = sizeof (_Bool),
    [OCOMS_DATATYPE_WCHAR] = sizeof (wchar_t),
};

/*
 * NOTE: The order of this array *MUST* match what is listed in datatype.h
 * (use of designated initializers should relax this restrictions some)
 */
OCOMS_DECLSPEC const ocoms_datatype_t* ocoms_datatype_basicDatatypes[OCOMS_DATATYPE_MAX_PREDEFINED] = {
    [OCOMS_DATATYPE_LOOP] = &ocoms_datatype_loop,
    [OCOMS_DATATYPE_END_LOOP] = &ocoms_datatype_end_loop,
    [OCOMS_DATATYPE_LB] = &ocoms_datatype_lb,
    [OCOMS_DATATYPE_UB] = &ocoms_datatype_ub,
    [OCOMS_DATATYPE_INT1] = &ocoms_datatype_int1,
    [OCOMS_DATATYPE_INT2] = &ocoms_datatype_int2,
    [OCOMS_DATATYPE_INT4] = &ocoms_datatype_int4,
    [OCOMS_DATATYPE_INT8] = &ocoms_datatype_int8,
    [OCOMS_DATATYPE_INT16] = &ocoms_datatype_int16,       /* Yes, double-machine word integers are available */
    [OCOMS_DATATYPE_UINT1] = &ocoms_datatype_uint1,
    [OCOMS_DATATYPE_UINT2] = &ocoms_datatype_uint2,
    [OCOMS_DATATYPE_UINT4] = &ocoms_datatype_uint4,
    [OCOMS_DATATYPE_UINT8] = &ocoms_datatype_uint8,
    [OCOMS_DATATYPE_UINT16] = &ocoms_datatype_uint16,      /* Yes, double-machine word integers are available */
    [OCOMS_DATATYPE_FLOAT2] = &ocoms_datatype_float2,
    [OCOMS_DATATYPE_FLOAT4] = &ocoms_datatype_float4,
    [OCOMS_DATATYPE_FLOAT8] = &ocoms_datatype_float8,
    [OCOMS_DATATYPE_FLOAT12] = &ocoms_datatype_float12,
    [OCOMS_DATATYPE_FLOAT16] = &ocoms_datatype_float16,
    [OCOMS_DATATYPE_FLOAT_COMPLEX] = &ocoms_datatype_float_complex,
    [OCOMS_DATATYPE_DOUBLE_COMPLEX] = &ocoms_datatype_double_complex,
    [OCOMS_DATATYPE_LONG_DOUBLE_COMPLEX] = &ocoms_datatype_long_double_complex,
    [OCOMS_DATATYPE_BOOL] = &ocoms_datatype_bool,
    [OCOMS_DATATYPE_WCHAR] = &ocoms_datatype_wchar,
    [OCOMS_DATATYPE_UNAVAILABLE] = &ocoms_datatype_unavailable,
};


int ocoms_datatype_register_params(void)
{
#if OCOMS_ENABLE_DEBUG
    int ret;

    ret = ocoms_mca_base_var_register ("ocoms", "mpi", NULL, "ddt_unpack_debug",
				 "Whether to output debugging information in the ddt unpack functions (nonzero = enabled)",
				 MCA_BASE_VAR_TYPE_BOOL, NULL, 0, MCA_BASE_VAR_FLAG_SETTABLE, OCOMS_INFO_LVL_3,
				 MCA_BASE_VAR_SCOPE_LOCAL, &ocoms_unpack_debug);
    if (0 > ret) {
	return ret;
    }

    ret = ocoms_mca_base_var_register ("ocoms", "mpi", NULL, "ddt_pack_debug",
				 "Whether to output debugging information in the ddt pack functions (nonzero = enabled)",
				 MCA_BASE_VAR_TYPE_BOOL, NULL, 0, MCA_BASE_VAR_FLAG_SETTABLE, OCOMS_INFO_LVL_3,
				 MCA_BASE_VAR_SCOPE_LOCAL, &ocoms_pack_debug);
    if (0 > ret) {
	return ret;
    }

    ret = ocoms_mca_base_var_register ("ocoms", "mpi", NULL, "ddt_position_debug",
				 "Non zero lead to output generated by the datatype position functions",
				 MCA_BASE_VAR_TYPE_BOOL, NULL, 0, MCA_BASE_VAR_FLAG_SETTABLE, OCOMS_INFO_LVL_3,
				 MCA_BASE_VAR_SCOPE_LOCAL, &ocoms_position_debug);
    if (0 > ret) {
	return ret;
    }

    ret = ocoms_mca_base_var_register ("ocoms", "mpi", NULL, "ddt_copy_debug",
				 "Whether to output debugging information in the ddt copy functions (nonzero = enabled)",
				 MCA_BASE_VAR_TYPE_BOOL, NULL, 0, MCA_BASE_VAR_FLAG_SETTABLE, OCOMS_INFO_LVL_3,
				 MCA_BASE_VAR_SCOPE_LOCAL, &ocoms_copy_debug);
    if (0 > ret) {
	return ret;
    }

#if OCOMS_CUDA_SUPPORT
    /* Set different levels of verbosity in the cuda related code. */
    ret = ocoms_mca_base_var_register ("ocoms", "ocoms", NULL, "cuda_verbose",
                                 "Set level of ocoms cuda verbosity",
                                 MCA_BASE_VAR_TYPE_INT, NULL, 0, MCA_BASE_VAR_FLAG_SETTABLE,
                                 OCOMS_INFO_LVL_8, MCA_BASE_VAR_SCOPE_LOCAL,
                                 &ocoms_cuda_verbose);
    if (0 > ret) {
	return ret;
    }
#endif

#endif /* OCOMS_ENABLE_DEBUG */

    return OCOMS_SUCCESS;
}


int32_t ocoms_datatype_init( void )
{
    const ocoms_datatype_t* datatype;
    int32_t i;

    for( i = OCOMS_DATATYPE_FIRST_TYPE; i < OCOMS_DATATYPE_MAX_PREDEFINED; i++ ) {
        datatype = ocoms_datatype_basicDatatypes[i];

        /* All of the predefined OPAL types don't have any GAPS! */
        datatype->desc.desc[0].elem.common.flags = OCOMS_DATATYPE_FLAG_PREDEFINED |
                                                   OCOMS_DATATYPE_FLAG_DATA |
                                                   OCOMS_DATATYPE_FLAG_CONTIGUOUS |
                                                   OCOMS_DATATYPE_FLAG_NO_GAPS;
        datatype->desc.desc[0].elem.common.type  = i;
        /* datatype->desc.desc[0].elem.blocklen XXX not set at the moment, it will be needed later */
        datatype->desc.desc[0].elem.count        = 1;
        datatype->desc.desc[0].elem.disp         = 0;
        datatype->desc.desc[0].elem.extent       = datatype->size;

        datatype->desc.desc[1].end_loop.common.flags    = 0;
        datatype->desc.desc[1].end_loop.common.type     = OCOMS_DATATYPE_END_LOOP;
        datatype->desc.desc[1].end_loop.items           = 1;
        datatype->desc.desc[1].end_loop.first_elem_disp = datatype->desc.desc[0].elem.disp;
        datatype->desc.desc[1].end_loop.size            = datatype->size;
    }

    return OCOMS_SUCCESS;
}


int32_t ocoms_datatype_finalize( void )
{
    /* As the synonyms are just copies of the internal data we should not free them.
     * Anyway they are over the limit of OCOMS_DATATYPE_MAX_PREDEFINED so they will never get freed.
     */

    /* As they are statically allocated they cannot be released. But we
     * can call OBJ_DESTRUCT, just to free all internally allocated ressources.
     */
#if defined(VERBOSE)
    if( ocoms_datatype_dfd != -1 )
        ocoms_output_close( ocoms_datatype_dfd );
    ocoms_datatype_dfd = -1;
#endif /* VERBOSE */

    /* clear all master convertors */
    ocoms_convertor_destroy_masters();

    return OCOMS_SUCCESS;
}

#if OCOMS_ENABLE_DEBUG
/*
 * Set a breakpoint to this function in your favorite debugger
 * to make it stop on all pack and unpack errors.
 */
int ocoms_datatype_safeguard_pointer_debug_breakpoint( const void* actual_ptr, int length,
                                                      const void* initial_ptr,
                                                      const ocoms_datatype_t* pData,
                                                      int count )
{
    return 0;
}
#endif  /* OCOMS_ENABLE_DEBUG */
