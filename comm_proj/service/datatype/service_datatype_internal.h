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
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef CCS_DATATYPE_INTERNAL_H_HAS_BEEN_INCLUDED
#define CCS_DATATYPE_INTERNAL_H_HAS_BEEN_INCLUDED

#include "ccs_config.h"

#include <stddef.h>
#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#if defined(VERBOSE)
#include "opal/util/output.h"

extern int service_datatype_dfd;

#  define DDT_DUMP_STACK( PSTACK, STACK_POS, PDESC, NAME ) \
     service_datatype_dump_stack( (PSTACK), (STACK_POS), (PDESC), (NAME) )
#  if defined(ACCEPT_C99)
#    define DUMP( ARGS... )          ccs_output(service_datatype_dfd, __VA_ARGS__)
#  else
#    if defined(__GNUC__) && !defined(__STDC__)
#      define DUMP(ARGS...)          ccs_output( service_datatype_dfd, ARGS)
#  else
static inline void DUMP( char* fmt, ... )
{
   va_list list;

   va_start( list, fmt );
   ccs_output( service_datatype_dfd, fmt, list );
   va_end( list );
}
#      define DUMP                   printf
#    endif  /* __GNUC__ && !__STDC__ */
#  endif  /* ACCEPT_C99 */
#else
#  define DDT_DUMP_STACK( PSTACK, STACK_POS, PDESC, NAME )
#  if defined(ACCEPT_C99)
#    define DUMP(ARGS...)
#  else
#    if defined(__GNUC__) && !defined(__STDC__)
#      define DUMP(ARGS...)
#    else
       /* If we do not compile with PGI, mark the parameter as unused */
#      if !defined(__PGI)
#        define __service_attribute_unused_tmp__  __service_attribute_unused__
#      else
#        define __service_attribute_unused_tmp__
#      endif  
static inline void DUMP( char* fmt __service_attribute_unused_tmp__, ... )
{
#if defined(__PGI)
           /* Some compilers complain if we have "..." arguments and no
              corresponding va_start() */
           va_list arglist;
           va_start(arglist, fmt);
           va_end(arglist);
#endif
}
#         undef __service_attribute_unused_tmp__
#    endif  /* __GNUC__ && !__STDC__ */
#  endif  /* ACCEPT_C99 */
#endif  /* VERBOSE */


/*
 * There 3 types of predefined data types.
 * - the basic one composed by just one basic datatype which are
 *   definitively contiguous
 * - the derived ones where the same basic type is used multiple times.
 *   They should be most of the time contiguous.
 * - and finally the derived one where multiple basic types are used.
 *   Depending on the architecture they can be contiguous or not.
 *
 * At the OPAL-level we do not care from which language the datatype came from
 * (C, C++ or FORTRAN), we only focus on their internal representation in
 * the host memory.
 *
 * NOTE: This (and in ompi_datatype_internal.h) is the id-order to obey
 */
#define CCS_DATATYPE_LOOP           0
#define CCS_DATATYPE_END_LOOP       1
#define CCS_DATATYPE_LB             2
#define CCS_DATATYPE_UB             3
#define CCS_DATATYPE_FIRST_TYPE     4 /* Number of first real type */
#define CCS_DATATYPE_INT1           4
#define CCS_DATATYPE_INT2           5
#define CCS_DATATYPE_INT4           6
#define CCS_DATATYPE_INT8           7
#define CCS_DATATYPE_INT16          8
#define CCS_DATATYPE_UINT1          9
#define CCS_DATATYPE_UINT2          10
#define CCS_DATATYPE_UINT4          11
#define CCS_DATATYPE_UINT8          12
#define CCS_DATATYPE_UINT16         13
#define CCS_DATATYPE_FLOAT2         14
#define CCS_DATATYPE_FLOAT4         15
#define CCS_DATATYPE_FLOAT8         16
#define CCS_DATATYPE_FLOAT12        17
#define CCS_DATATYPE_FLOAT16        18
#define CCS_DATATYPE_COMPLEX8       19
#define CCS_DATATYPE_COMPLEX16      20
#define CCS_DATATYPE_COMPLEX32      21
#define CCS_DATATYPE_BOOL           22
#define CCS_DATATYPE_WCHAR          23
#define CCS_DATATYPE_UNAVAILABLE    24

#ifndef CCS_DATATYPE_MAX_PREDEFINED
#define CCS_DATATYPE_MAX_PREDEFINED 25
#endif

/*
 * If the number of basic datatype should change update
 * CCS_DATATYPE_MAX_PREDEFINED in datatype.h
 */
#if CCS_DATATYPE_MAX_PREDEFINED <= CCS_DATATYPE_UNAVAILABLE
#error CCS_DATATYPE_MAX_PREDEFINED should be updated to the next value after the CCS_DATATYPE_UNAVAILABLE define
#endif

#define DT_INCREASE_STACK     8

BEGIN_C_DECLS

struct ddt_elem_id_description {
    uint16_t   flags;  /**< flags for the record */
    uint16_t   type;   /**< the basic data type id */
};
typedef struct ddt_elem_id_description ddt_elem_id_description;

/* the basic element. A data description is composed
 * by a set of basic elements.
 */
struct ddt_elem_desc {
    ddt_elem_id_description common;           /**< basic data description and flags */
    uint32_t                count;            /**< number of blocks */
    uint32_t                blocklen;         /**< number of elements on each block */
    CCS_PTRDIFF_TYPE       extent;           /**< extent of each block (in bytes) */
    CCS_PTRDIFF_TYPE       disp;             /**< displacement of the first block */
};
typedef struct ddt_elem_desc ddt_elem_desc_t;

struct ddt_loop_desc {
    ddt_elem_id_description common;           /**< basic data description and flags */
    uint32_t                loops;            /**< number of elements */
    uint32_t                items;            /**< number of items in the loop */
    size_t                  unused;           /**< not used right now */
    CCS_PTRDIFF_TYPE       extent;           /**< extent of the whole loop */
};
typedef struct ddt_loop_desc ddt_loop_desc_t;

struct ddt_endloop_desc {
    ddt_elem_id_description common;           /**< basic data description and flags */
    uint32_t                items;            /**< number of elements */
    uint32_t                unused;           /**< not used right now */
    size_t                  size;             /**< real size of the data in the loop */
    CCS_PTRDIFF_TYPE       first_elem_disp;  /**< the displacement of the first block in the loop */
};
typedef struct ddt_endloop_desc ddt_endloop_desc_t;

union dt_elem_desc {
    ddt_elem_desc_t    elem;
    ddt_loop_desc_t    loop;
    ddt_endloop_desc_t end_loop;
};

#define CREATE_LOOP_START( _place, _count, _items, _extent, _flags )           \
    do {                                                                       \
        (_place)->loop.common.type   = CCS_DATATYPE_LOOP;                     \
        (_place)->loop.common.flags  = (_flags) & ~CCS_DATATYPE_FLAG_DATA;    \
        (_place)->loop.loops         = (_count);                               \
        (_place)->loop.items         = (_items);                               \
        (_place)->loop.extent        = (_extent);                              \
        (_place)->loop.unused        = -1;                                     \
    } while(0)

#define CREATE_LOOP_END( _place, _items, _first_item_disp, _size, _flags )     \
    do {                                                                       \
        (_place)->end_loop.common.type = CCS_DATATYPE_END_LOOP;               \
        (_place)->end_loop.common.flags = (_flags) & ~CCS_DATATYPE_FLAG_DATA; \
        (_place)->end_loop.items = (_items);                                   \
        (_place)->end_loop.first_elem_disp = (_first_item_disp);               \
        (_place)->end_loop.size = (_size);  /* the size inside the loop */     \
        (_place)->end_loop.unused = -1;                                        \
    } while(0)

#define CREATE_ELEM( _place, _type, _flags, _count, _disp, _extent )           \
    do {                                                                       \
        (_place)->elem.common.flags = (_flags) | CCS_DATATYPE_FLAG_DATA;      \
        (_place)->elem.common.type  = (_type);                                 \
        (_place)->elem.count        = (_count);                                \
        (_place)->elem.disp         = (_disp);                                 \
        (_place)->elem.extent       = (_extent);                               \
        (_place)->elem.blocklen     = 1;                                       \
    } while(0)
/*
 * This array holds the descriptions desc.desc[2] of the predefined basic datatypes.
 */
CCS_DECLSPEC extern union dt_elem_desc service_datatype_predefined_elem_desc[2 * CCS_DATATYPE_MAX_PREDEFINED];
struct service_datatype_t;

/* Other fields starting after bdt_used (index of CCS_DATATYPE_LOOP should be ONE) */
/*
 * NOTE: The order of initialization *MUST* match the order of the CCS_DATATYPE_-numbers.
 * Unfortunateley, I don't get the preprocessor to replace
 *     CCS_DATATYPE_INIT_BTYPES_ARRAY_ ## CCS_DATATYPE ## NAME
 * into
 *     CCS_DATATYPE_INIT_BTYPES_ARRAY_[0-21], then order and naming would _not_ matter....
 */
#define Z5    0, 0, 0, 0, 0
#define Z10   Z5, Z5
#define Z15   Z10, Z5
#define Z20   Z10, Z10
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UNAVAILABLE { 0, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_LOOP        { 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_END_LOOP    { 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_LB          { 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UB          { 0, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_INT1        { 0, 0, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_INT2        { Z5, 1, }              /*5*/
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_INT4        { Z5, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_INT8        { Z5, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_INT16       { Z5, 0, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UINT1       { Z5, 0, 0, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UINT2       { Z10, 1, }             /*10*/
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UINT4       { Z10, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UINT8       { Z10, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_UINT16      { Z10, 0, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_FLOAT2      { Z10, 0, 0, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_FLOAT4      { Z15, 1, }             /*15*/
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_FLOAT8      { Z15, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_FLOAT12     { Z15, 0, 0, 1, }
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_FLOAT16     { Z15, 0, 0, 0, 1, }
#if HAVE_FLOAT__COMPLEX
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_COMPLEX8    { Z15, 0, 0, 0, 0, 1 }
#else
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_COMPLEX8    { Z15, 2 }    /* two floats */
#endif
#if HAVE_DOUBLE__COMPLEX
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_COMPLEX16   { Z15, 0, 0, 0, 0, 0, 1 }
#else
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_COMPLEX16   { Z15, 0, 2, }
#endif
#if HAVE_LONG_DOUBLE__COMPLEX
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_COMPLEX32   { Z15, 0, 0, 0, 0, 0, 0, 1 }
#else
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_COMPLEX32   { Z15, 0, 0, 0, 2, }
#endif
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_BOOL        { Z20, 0, 1, }          /*22*/
#define CCS_DATATYPE_INIT_BTYPES_ARRAY_WCHAR       { Z20, 0, 0, 1, }

#define CCS_DATATYPE_INIT_NAME(NAME) "CCS_" _CCS_DATATYPE_INIT_NAME(NAME)
#define _CCS_DATATYPE_INIT_NAME(NAME)  #NAME

/*
 * Macro to initialize the main description for basic types, setting the pointer
 * into the array service_datatype_predefined_type_desc, which is initialized at
 * runtime in service_datatype_init(). Each basic type has two desc-elements....
 */
#define CCS_DATATYPE_INIT_DESC_PREDEFINED(NAME)                                     \
    {                                                                                \
        1 /*length*/, 1/*used*/,                                                     \
        &(service_datatype_predefined_elem_desc[2 * CCS_DATATYPE_ ## NAME]) /*desc*/   \
    }
#define CCS_DATATYPE_INIT_DESC_NULL  {0 /*length*/, 0/*used*/, NULL /*desc*/}

#define CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( NAME, FLAGS )                   \
    {                                                                                \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                                       \
        CCS_DATATYPE_FLAG_UNAVAILABLE | CCS_DATATYPE_FLAG_PREDEFINED | (FLAGS) /*flag*/, \
        CCS_DATATYPE_ ## NAME /*id*/,                                               \
        0 /*bdt_used*/,                                                              \
        0 /*size*/,                                                                  \
        0 /*true_lb*/, 0 /*true_ub*/, 0 /*lb*/, 0 /*ub*/,                            \
        0 /*align*/,                                                                 \
        1 /*nbElems*/,                                                               \
        CCS_DATATYPE_INIT_NAME(NAME), /*name*/                                      \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(UNAVAILABLE), /*desc*/                    \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(UNAVAILABLE), /*opt_desc*/                \
        CCS_DATATYPE_INIT_BTYPES_ARRAY_UNAVAILABLE /*btypes*/                       \
    }

#define CCS_DATATYPE_INITIALIZER_UNAVAILABLE( FLAGS )                               \
    CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( UNAVAILABLE, (FLAGS) )

#define CCS_DATATYPE_INITIALIZER_EMPTY( FLAGS )                                     \
    {                                                                                \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                                       \
        CCS_DATATYPE_FLAG_PREDEFINED | (FLAGS) /*flag*/,                            \
        0 /*id*/,                                                                    \
        0 /*bdt_used*/,                                                              \
        0 /*size*/,                                                                  \
        0 /*true_lb*/, 0 /*true_ub*/, 0 /*lb*/, 0 /*ub*/,                            \
        0 /*align*/,                                                                 \
        1 /*nbElems*/,                                                               \
        CCS_DATATYPE_INIT_NAME(EMPTY), /*name*/                                     \
        CCS_DATATYPE_INIT_DESC_NULL, /*desc*/                                       \
        CCS_DATATYPE_INIT_DESC_NULL, /*opt_desc*/                                   \
        CCS_DATATYPE_INIT_BTYPES_ARRAY_UNAVAILABLE /*btypes*/                       \
    }

#define CCS_DATATYPE_INIT_BASIC_TYPE( TYPE, NAME, FLAGS )                           \
    {                                                                                \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                                       \
        CCS_DATATYPE_FLAG_PREDEFINED | (FLAGS) /*flag*/,                            \
        TYPE /*id*/,                                                                 \
        (((uint32_t)1)<<(TYPE)) /*bdt_used*/,                                        \
        0 /*size*/,                                                                  \
        0 /*true_lb*/, 0 /*true_ub*/, 0 /*lb*/, 0 /*ub*/,                            \
        0 /*align*/,                                                                 \
        1 /*nbElems*/,                                                               \
        CCS_DATATYPE_INIT_NAME(NAME), /*name*/                                      \
        CCS_DATATYPE_INIT_DESC_NULL, /*desc*/                                       \
        CCS_DATATYPE_INIT_DESC_NULL, /*opt_desc*/                                   \
        CCS_DATATYPE_INIT_BTYPES_ARRAY_ ## NAME /*btypes*/                          \
    }

#define CCS_DATATYPE_INIT_BASIC_DATATYPE( TYPE, ALIGN, NAME, FLAGS )                \
    {                                                                                \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                                       \
        CCS_DATATYPE_FLAG_BASIC | (FLAGS) /*flag*/,                                 \
        CCS_DATATYPE_ ## NAME /*id*/,                                               \
        (((uint32_t)1)<<(CCS_DATATYPE_ ## NAME)) /*bdt_used*/,                      \
        sizeof(TYPE) /*size*/,                                                       \
        0 /*true_lb*/, sizeof(TYPE) /*true_ub*/, 0 /*lb*/, sizeof(TYPE) /*ub*/,      \
        (ALIGN) /*align*/,                                                           \
        1 /*nbElems*/,                                                               \
        CCS_DATATYPE_INIT_NAME(NAME) /*name*/,                                      \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(NAME) /*desc*/,                           \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(NAME) /*opt_desc*/,                       \
        CCS_DATATYPE_INIT_BTYPES_ARRAY_ ## NAME /*btypes*/                          \
    }

#define CCS_DATATYPE_INITIALIZER_LOOP(FLAGS)       CCS_DATATYPE_INIT_BASIC_TYPE( CCS_DATATYPE_LOOP, LOOP, FLAGS )
#define CCS_DATATYPE_INITIALIZER_END_LOOP(FLAGS)   CCS_DATATYPE_INIT_BASIC_TYPE( CCS_DATATYPE_END_LOOP, END_LOOP, FLAGS )
#define CCS_DATATYPE_INITIALIZER_LB(FLAGS)         CCS_DATATYPE_INIT_BASIC_TYPE( CCS_DATATYPE_LB, LB, FLAGS )
#define CCS_DATATYPE_INITIALIZER_UB(FLAGS)         CCS_DATATYPE_INIT_BASIC_TYPE( CCS_DATATYPE_UB, UB, FLAGS )
#define CCS_DATATYPE_INITIALIZER_INT1(FLAGS)       CCS_DATATYPE_INIT_BASIC_DATATYPE( int8_t, CCS_ALIGNMENT_INT8, INT1, FLAGS )
#define CCS_DATATYPE_INITIALIZER_INT2(FLAGS)       CCS_DATATYPE_INIT_BASIC_DATATYPE( int16_t, CCS_ALIGNMENT_INT16, INT2, FLAGS )
#define CCS_DATATYPE_INITIALIZER_INT4(FLAGS)       CCS_DATATYPE_INIT_BASIC_DATATYPE( int32_t, CCS_ALIGNMENT_INT32, INT4, FLAGS )
#define CCS_DATATYPE_INITIALIZER_INT8(FLAGS)       CCS_DATATYPE_INIT_BASIC_DATATYPE( int64_t, CCS_ALIGNMENT_INT64, INT8, FLAGS )
#ifdef HAVE_INT128_T
#define CCS_DATATYPE_INITIALIZER_INT16(FLAGS)      CCS_DATATYPE_INIT_BASIC_DATATYPE( int128_t, CCS_ALIGNMENT_INT128, INT16, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_INT16(FLAGS)      CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( INT16, FLAGS )
#endif
#define CCS_DATATYPE_INITIALIZER_UINT1(FLAGS)      CCS_DATATYPE_INIT_BASIC_DATATYPE( uint8_t, CCS_ALIGNMENT_INT8, UINT1, FLAGS )
#define CCS_DATATYPE_INITIALIZER_UINT2(FLAGS)      CCS_DATATYPE_INIT_BASIC_DATATYPE( uint16_t, CCS_ALIGNMENT_INT16, UINT2, FLAGS )
#define CCS_DATATYPE_INITIALIZER_UINT4(FLAGS)      CCS_DATATYPE_INIT_BASIC_DATATYPE( uint32_t, CCS_ALIGNMENT_INT32, UINT4, FLAGS )
#define CCS_DATATYPE_INITIALIZER_UINT8(FLAGS)      CCS_DATATYPE_INIT_BASIC_DATATYPE( uint64_t, CCS_ALIGNMENT_INT64, UINT8, FLAGS )
#ifdef HAVE_UINT128_T
#define CCS_DATATYPE_INITIALIZER_UINT16(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( uint128_t, CCS_ALIGNMENT_INT128, UINT16, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_UINT16(FLAGS)     CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( INT16, FLAGS )
#endif

#if SIZEOF_FLOAT == 2
#define CCS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( float, CCS_ALIGNMENT_FLOAT, FLOAT2, FLAGS )
#elif SIZEOF_DOUBLE == 2
#define CCS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( double, CCS_ALIGNMENT_DOUBLE, FLOAT2, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 2
#define CCS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( long double, CCS_ALIGNMENT_LONG_DOUBLE, FLOAT2, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT2, FLAGS )
#endif

#if SIZEOF_FLOAT == 4
#define CCS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( float, CCS_ALIGNMENT_FLOAT, FLOAT4, FLAGS )
#elif SIZEOF_DOUBLE == 4
#define CCS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( double, CCS_ALIGNMENT_DOUBLE, FLOAT4, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 4
#define CCS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( long double, CCS_ALIGNMENT_LONG_DOUBLE, FLOAT4, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT4, FLAGS )
#endif

#if SIZEOF_FLOAT == 8
#define CCS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( float, CCS_ALIGNMENT_FLOAT, FLOAT8, FLAGS )
#elif SIZEOF_DOUBLE == 8
#define CCS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( double, CCS_ALIGNMENT_DOUBLE, FLOAT8, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 8
#define CCS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     CCS_DATATYPE_INIT_BASIC_DATATYPE( long double, CCS_ALIGNMENT_LONG_DOUBLE, FLOAT8, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT8, FLAGS )
#endif

#if SIZEOF_FLOAT == 12
#define CCS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    CCS_DATATYPE_INIT_BASIC_DATATYPE( float, CCS_ALIGNMENT_FLOAT, FLOAT12, FLAGS )
#elif SIZEOF_DOUBLE == 12
#define CCS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    CCS_DATATYPE_INIT_BASIC_DATATYPE( double, CCS_ALIGNMENT_DOUBLE, FLOAT12, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 12
#define CCS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    CCS_DATATYPE_INIT_BASIC_DATATYPE( long double, CCS_ALIGNMENT_LONG_DOUBLE, FLOAT12, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT12, FLAGS )
#endif

#if SIZEOF_FLOAT == 16
#define CCS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    CCS_DATATYPE_INIT_BASIC_DATATYPE( float, CCS_ALIGNMENT_FLOAT, FLOAT16, FLAGS )
#elif SIZEOF_DOUBLE == 16
#define CCS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    CCS_DATATYPE_INIT_BASIC_DATATYPE( double, CCS_ALIGNMENT_DOUBLE, FLOAT16, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 16
#define CCS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    CCS_DATATYPE_INIT_BASIC_DATATYPE( long double, CCS_ALIGNMENT_LONG_DOUBLE, FLOAT16, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT16, FLAGS )
#endif

/*
 * Until we figure out the whole logic behing the _Complex type, disable it. It only exists
 * in C99 (i.e. __STDC_VERSION__ >= 199901L) but [evidently] not all compilers correctly
 * export the __STDC_VERSION__.
 * http://predef.sourceforge.net/prestd.html for more info.
 */

#define CCS_USE_FLOAT__COMPLEX       (0 && HAVE_FLOAT__COMPLEX)
#define CCS_USE_DOUBLE__COMPLEX      (0 && HAVE_DOUBLE__COMPLEX)
#define CCS_USE_LONG_DOUBLE__COMPLEX (0 && HAVE_LONG_DOUBLE__COMPLEX)

#if CCS_USE_FLOAT__COMPLEX && (SIZEOF_FLOAT__COMPLEX == 8)
#define CCS_DATATYPE_INITIALIZER_COMPLEX8(FLAGS)   CCS_DATATYPE_INIT_BASIC_DATATYPE( float _Complex, CCS_ALIGNMENT_FLOAT_COMPLEX, COMPLEX8, (FLAGS) )
#else

#if SIZEOF_FLOAT == 4
typedef struct {
    float r;
    float i;
} service_complex_float_t;

#define CCS_DATATYPE_INITIALIZER_COMPLEX8( FLAGS )                     \
    {                                                                   \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                          \
        CCS_DATATYPE_FLAG_BASIC | (FLAGS) /*flag*/,                    \
        CCS_DATATYPE_COMPLEX8 /*id*/,                                  \
        (((uint32_t)1)<<(CCS_DATATYPE_ ## FLOAT4)) /*bdt_used*/,       \
        sizeof(service_complex_float_t) /*size*/,                          \
        0 /*true_lb*/, sizeof(service_complex_float_t) /*true_ub*/, 0 /*lb*/, sizeof(service_complex_float_t) /*ub*/, \
        CCS_ALIGNMENT_FLOAT /*align*/,                                 \
        2 /*nbElems*/,                                                  \
        CCS_DATATYPE_INIT_NAME(COMPLEX8) /*name*/,                     \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(COMPLEX8) /*desc*/,          \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(COMPLEX8) /*opt_desc*/,      \
        {Z15, 2, } /*btypes*/                                           \
    }
#else
#define CCS_DATATYPE_INITIALIZER_COMPLEX8(FLAGS)  CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( COMPLEX8, FLAGS )
#endif

#endif  /* HAVE_FLOAT__COMPLEX */

#if CCS_USE_DOUBLE__COMPLEX && (SIZEOF_DOUBLE__COMPLEX == 16)
#define CCS_DATATYPE_INITIALIZER_COMPLEX16(FLAGS)  CCS_DATATYPE_INIT_BASIC_DATATYPE( double _Complex, CCS_ALIGNMENT_DOUBLE_COMPLEX, COMPLEX16, (FLAGS) )
#else

#if SIZEOF_DOUBLE == 8
typedef struct {
    double r;
    double i;
} service_complex_double_t;

#define CCS_DATATYPE_INITIALIZER_COMPLEX16( FLAGS )                    \
    {                                                                   \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                          \
        CCS_DATATYPE_FLAG_BASIC | (FLAGS) /*flag*/,                    \
        CCS_DATATYPE_COMPLEX16 /*id*/,                                 \
        (((uint32_t)1)<<(CCS_DATATYPE_ ## FLOAT8)) /*bdt_used*/,       \
        sizeof(service_complex_double_t) /*size*/,                         \
        0 /*true_lb*/, sizeof(service_complex_double_t) /*true_ub*/, 0 /*lb*/, sizeof(service_complex_double_t) /*ub*/, \
        CCS_ALIGNMENT_DOUBLE /*align*/,                                \
        2 /*nbElems*/,                                                  \
        CCS_DATATYPE_INIT_NAME(COMPLEX16) /*name*/,                    \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(COMPLEX16) /*desc*/,         \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(COMPLEX16) /*opt_desc*/,     \
        {Z15, 2, } /*btypes*/                                           \
    }
#else
#define CCS_DATATYPE_INITIALIZER_COMPLEX16(FLAGS) CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( COMPLEX16, FLAGS )
#endif

#endif  /* HAVE_DOUBLE__COMPLEX */

#if CCS_USE_LONG_DOUBLE__COMPLEX && (SIZEOF_LONG_DOUBLE__COMPLEX == 32)
#define CCS_DATATYPE_INITIALIZER_COMPLEX32(FLAGS)   CCS_DATATYPE_INIT_BASIC_DATATYPE( long double _Complex, CCS_ALIGNMENT_LONG_DOUBLE_COMPLEX, COMPLEX32, (FLAGS) )
#else

#if HAVE_LONG_DOUBLE && (SIZEOF_LONG_DOUBLE == 16)
typedef struct {
    long double r;
    long double i;
} service_complex_long_double_t;

#define CCS_DATATYPE_INITIALIZER_COMPLEX32( FLAGS )                    \
    {                                                                   \
        CCS_OBJ_STATIC_INIT(service_datatype_t),                          \
        CCS_DATATYPE_FLAG_BASIC | (FLAGS) /*flag*/,                    \
        CCS_DATATYPE_COMPLEX32 /*id*/,                                 \
        (((uint32_t)1)<<(CCS_DATATYPE_ ## FLOAT16)) /*bdt_used*/,      \
        sizeof(service_complex_long_double_t) /*size*/,                    \
        0 /*true_lb*/, sizeof(service_complex_long_double_t) /*true_ub*/, 0 /*lb*/, sizeof(service_complex_long_double_t) /*ub*/, \
        CCS_ALIGNMENT_LONG_DOUBLE /*align*/,                           \
        2 /*nbElems*/,                                                  \
        CCS_DATATYPE_INIT_NAME(COMPLEX32) /*name*/,                    \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(COMPLEX32) /*desc*/,         \
        CCS_DATATYPE_INIT_DESC_PREDEFINED(COMPLEX32) /*opt_desc*/,     \
        {Z15, 2, } /*btypes*/                                           \
    }
#else
#define CCS_DATATYPE_INITIALIZER_COMPLEX32(FLAGS) CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( COMPLEX32, FLAGS )
#endif

#endif  /* HAVE_LONG_DOUBLE__COMPLEX */

#define CCS_DATATYPE_INITIALIZER_BOOL(FLAGS)       CCS_DATATYPE_INIT_BASIC_DATATYPE( _Bool, CCS_ALIGNMENT_BOOL, BOOL, FLAGS )

#if CCS_ALIGNMENT_WCHAR != 0
#define CCS_DATATYPE_INITIALIZER_WCHAR(FLAGS)      CCS_DATATYPE_INIT_BASIC_DATATYPE( wchar_t, CCS_ALIGNMENT_WCHAR, WCHAR, FLAGS )
#else
#define CCS_DATATYPE_INITIALIZER_WCHAR(FLAGS)      CCS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( WCHAR, FLAGS )
#endif


#define BASIC_DDT_FROM_ELEM( ELEM ) (service_datatype_basicDatatypes[(ELEM).elem.common.type])

#define SAVE_STACK( PSTACK, INDEX, TYPE, COUNT, DISP) \
do { \
   (PSTACK)->index    = (INDEX); \
   (PSTACK)->type     = (TYPE); \
   (PSTACK)->count    = (COUNT); \
   (PSTACK)->disp     = (DISP); \
} while(0)

#define PUSH_STACK( PSTACK, STACK_POS, INDEX, TYPE, COUNT, DISP) \
do { \
   dt_stack_t* pTempStack = (PSTACK) + 1; \
   SAVE_STACK( pTempStack, (INDEX), (TYPE), (COUNT), (DISP) );  \
   (STACK_POS)++; \
   (PSTACK) = pTempStack; \
} while(0)

#if CCS_ENABLE_DEBUG
#define CCS_DATATYPE_SAFEGUARD_POINTER( ACTPTR, LENGTH, INITPTR, PDATA, COUNT ) \
    {                                                                   \
        unsigned char *__lower_bound = (INITPTR), *__upper_bound;       \
        assert( ((LENGTH) != 0) && ((COUNT) != 0) );                    \
        __lower_bound += (PDATA)->true_lb;                              \
        __upper_bound = (INITPTR) + (PDATA)->true_ub +                  \
            ((PDATA)->ub - (PDATA)->lb) * ((COUNT) - 1);                \
        if( ((ACTPTR) < __lower_bound) || ((ACTPTR) >= __upper_bound) ) { \
            service_datatype_safeguard_pointer_debug_breakpoint( (ACTPTR), (LENGTH), (INITPTR), (PDATA), (COUNT) ); \
            ccs_output( 0, "%s:%d\n\tPointer %p size %lu is outside [%p,%p] for\n\tbase ptr %p count %d and data \n", \
                         __FILE__, __LINE__, (ACTPTR), (unsigned long)(LENGTH), __lower_bound, __upper_bound, \
                         (INITPTR), (COUNT) );                          \
            service_datatype_dump( (PDATA) );                                   \
        }                                                               \
    }

#else
#define CCS_DATATYPE_SAFEGUARD_POINTER( ACTPTR, LENGTH, INITPTR, PDATA, COUNT )
#endif  /* CCS_ENABLE_DEBUG */

static inline int GET_FIRST_NON_LOOP( const union dt_elem_desc* _pElem )
{
    int element_index = 0;

    /* We dont have to check for the end as we always put an END_LOOP
     * at the end of all datatype descriptions.
     */
    while( _pElem->elem.common.type == CCS_DATATYPE_LOOP ) {
        ++_pElem; element_index++;
    }
    return element_index;
}

#define UPDATE_INTERNAL_COUNTERS( DESCRIPTION, POSITION, ELEMENT, COUNTER ) \
    do {                                                                \
        (ELEMENT) = &((DESCRIPTION)[(POSITION)]);                       \
        (COUNTER) = (ELEMENT)->elem.count;                              \
    } while (0)

CCS_DECLSPEC int service_datatype_contain_basic_datatypes( const struct service_datatype_t* pData, char* ptr, size_t length );
CCS_DECLSPEC int service_datatype_dump_data_flags( unsigned short usflags, char* ptr, size_t length );
CCS_DECLSPEC int service_datatype_dump_data_desc( union dt_elem_desc* pDesc, int nbElems, char* ptr, size_t length );

END_C_DECLS
#endif  /* CCS_DATATYPE_INTERNAL_H_HAS_BEEN_INCLUDED */
