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

#ifndef OCOMS_DATATYPE_INTERNAL_H_HAS_BEEN_INCLUDED
#define OCOMS_DATATYPE_INTERNAL_H_HAS_BEEN_INCLUDED

#include "ocoms/platform/ocoms_config.h"

#include <stddef.h>
#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#if defined(VERBOSE)
#include "ocoms/util/output.h"

extern int ocoms_datatype_dfd;

#  define DDT_DUMP_STACK( PSTACK, STACK_POS, PDESC, NAME ) \
     ocoms_datatype_dump_stack( (PSTACK), (STACK_POS), (PDESC), (NAME) )
#  if defined(ACCEPT_C99)
#    define DUMP( ARGS... )          ocoms_output(ocoms_datatype_dfd, __VA_ARGS__)
#  else
#    if defined(__GNUC__) && !defined(__STDC__)
#      define DUMP(ARGS...)          ocoms_output( ocoms_datatype_dfd, ARGS)
#  else
static inline void DUMP( char* fmt, ... )
{
   va_list list;

   va_start( list, fmt );
   ocoms_output( ocoms_datatype_dfd, fmt, list );
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
#        define __ocoms_attribute_unused_tmp__  __ocoms_attribute_unused__
#      else
#        define __ocoms_attribute_unused_tmp__
#      endif  
static inline void DUMP( char* fmt __ocoms_attribute_unused_tmp__, ... )
{
#if defined(__PGI)
           /* Some compilers complain if we have "..." arguments and no
              corresponding va_start() */
           va_list arglist;
           va_start(arglist, fmt);
           va_end(arglist);
#endif
}
#         undef __ocoms_attribute_unused_tmp__
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
#define OCOMS_DATATYPE_LOOP           0
#define OCOMS_DATATYPE_END_LOOP       1
#define OCOMS_DATATYPE_LB             2
#define OCOMS_DATATYPE_UB             3
#define OCOMS_DATATYPE_FIRST_TYPE     4 /* Number of first real type */
#define OCOMS_DATATYPE_INT1           4
#define OCOMS_DATATYPE_INT2           5
#define OCOMS_DATATYPE_INT4           6
#define OCOMS_DATATYPE_INT8           7
#define OCOMS_DATATYPE_INT16          8
#define OCOMS_DATATYPE_UINT1          9
#define OCOMS_DATATYPE_UINT2          10
#define OCOMS_DATATYPE_UINT4          11
#define OCOMS_DATATYPE_UINT8          12
#define OCOMS_DATATYPE_UINT16         13
#define OCOMS_DATATYPE_FLOAT2         14
#define OCOMS_DATATYPE_FLOAT4         15
#define OCOMS_DATATYPE_FLOAT8         16
#define OCOMS_DATATYPE_FLOAT12        17
#define OCOMS_DATATYPE_FLOAT16        18
#define OCOMS_DATATYPE_FLOAT_COMPLEX  19
#define OCOMS_DATATYPE_DOUBLE_COMPLEX 20
#define OCOMS_DATATYPE_LONG_DOUBLE_COMPLEX 21
#define OCOMS_DATATYPE_BOOL           22
#define OCOMS_DATATYPE_WCHAR          23
#define OCOMS_DATATYPE_UNAVAILABLE    24

#ifndef OCOMS_DATATYPE_MAX_PREDEFINED
#define OCOMS_DATATYPE_MAX_PREDEFINED (OCOMS_DATATYPE_UNAVAILABLE+1)
#elif OCOMS_DATATYPE_MAX_PREDEFINED <= OCOMS_DATATYPE_UNAVAILABLE
/*
 * If the number of basic datatype should change update
 * OCOMS_DATATYPE_MAX_PREDEFINED in ocoms_datatype.h
 */
#error OCOMS_DATATYPE_MAX_PREDEFINED should be updated to the next value after the OCOMS_DATATYPE_UNAVAILABLE define
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
    OCOMS_PTRDIFF_TYPE       extent;           /**< extent of each block (in bytes) */
    OCOMS_PTRDIFF_TYPE       disp;             /**< displacement of the first block */
};
typedef struct ddt_elem_desc ddt_elem_desc_t;

struct ddt_loop_desc {
    ddt_elem_id_description common;           /**< basic data description and flags */
    uint32_t                loops;            /**< number of elements */
    uint32_t                items;            /**< number of items in the loop */
    size_t                  unused;           /**< not used right now */
    OCOMS_PTRDIFF_TYPE       extent;           /**< extent of the whole loop */
};
typedef struct ddt_loop_desc ddt_loop_desc_t;

struct ddt_endloop_desc {
    ddt_elem_id_description common;           /**< basic data description and flags */
    uint32_t                items;            /**< number of elements */
    uint32_t                unused;           /**< not used right now */
    size_t                  size;             /**< real size of the data in the loop */
    OCOMS_PTRDIFF_TYPE       first_elem_disp;  /**< the displacement of the first block in the loop */
};
typedef struct ddt_endloop_desc ddt_endloop_desc_t;

union dt_elem_desc {
    ddt_elem_desc_t    elem;
    ddt_loop_desc_t    loop;
    ddt_endloop_desc_t end_loop;
};

#define CREATE_LOOP_START( _place, _count, _items, _extent, _flags )           \
    do {                                                                       \
        (_place)->loop.common.type   = OCOMS_DATATYPE_LOOP;                     \
        (_place)->loop.common.flags  = (_flags) & ~OCOMS_DATATYPE_FLAG_DATA;    \
        (_place)->loop.loops         = (_count);                               \
        (_place)->loop.items         = (_items);                               \
        (_place)->loop.extent        = (_extent);                              \
        (_place)->loop.unused        = -1;                                     \
    } while(0)

#define CREATE_LOOP_END( _place, _items, _first_item_disp, _size, _flags )     \
    do {                                                                       \
        (_place)->end_loop.common.type = OCOMS_DATATYPE_END_LOOP;               \
        (_place)->end_loop.common.flags = (_flags) & ~OCOMS_DATATYPE_FLAG_DATA; \
        (_place)->end_loop.items = (_items);                                   \
        (_place)->end_loop.first_elem_disp = (_first_item_disp);               \
        (_place)->end_loop.size = (_size);  /* the size inside the loop */     \
        (_place)->end_loop.unused = -1;                                        \
    } while(0)

#define CREATE_ELEM( _place, _type, _flags, _count, _disp, _extent )           \
    do {                                                                       \
        (_place)->elem.common.flags = (_flags) | OCOMS_DATATYPE_FLAG_DATA;      \
        (_place)->elem.common.type  = (_type);                                 \
        (_place)->elem.count        = (_count);                                \
        (_place)->elem.disp         = (_disp);                                 \
        (_place)->elem.extent       = (_extent);                               \
        (_place)->elem.blocklen     = 1;                                       \
    } while(0)
/*
 * This array holds the descriptions desc.desc[2] of the predefined basic datatypes.
 */
OCOMS_DECLSPEC extern union dt_elem_desc ocoms_datatype_predefined_elem_desc[2 * OCOMS_DATATYPE_MAX_PREDEFINED];
struct ocoms_datatype_t;

/* Other fields starting after bdt_used (index of OCOMS_DATATYPE_LOOP should be ONE) */
/*
 * NOTE: The order of initialization *MUST* match the order of the OCOMS_DATATYPE_-numbers.
 * Unfortunateley, I don't get the preprocessor to replace
 *     OCOMS_DATATYPE_INIT_BTYPES_ARRAY_ ## OCOMS_DATATYPE ## NAME
 * into
 *     OCOMS_DATATYPE_INIT_BTYPES_ARRAY_[0-21], then order and naming would _not_ matter....
 */

#define OCOMS_DATATYPE_INIT_BTYPES_ARRAY_UNAVAILABLE { 0 }
#define OCOMS_DATATYPE_INIT_BTYPES_ARRAY(NAME) { [OCOMS_DATATYPE_ ## NAME] = 1 }

#define OCOMS_DATATYPE_INIT_NAME(NAME) "OCOMS_" #NAME

/*
 * Macro to initialize the main description for basic types, setting the pointer
 * into the array ocoms_datatype_predefined_type_desc, which is initialized at
 * runtime in ocoms_datatype_init(). Each basic type has two desc-elements....
 */
#define OCOMS_DATATYPE_INIT_DESC_PREDEFINED(NAME)                                     \
    {                                                                                \
        .length = 1, .used = 1,                                                      \
        .desc = &(ocoms_datatype_predefined_elem_desc[2 * OCOMS_DATATYPE_ ## NAME])    \
    }
#define OCOMS_DATATYPE_INIT_DESC_NULL  {.length = 0, .used = 0, .desc = NULL}

#define OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( NAME, FLAGS )                   \
    {                                                                                \
        .super = OCOMS_OBJ_STATIC_INIT(ocoms_datatype_t),                              \
        .flags = OCOMS_DATATYPE_FLAG_UNAVAILABLE | OCOMS_DATATYPE_FLAG_PREDEFINED | (FLAGS), \
        .id = OCOMS_DATATYPE_ ## NAME,                                                \
        .bdt_used = 0,                                                               \
        .size = 0,                                                                   \
        .true_lb = 0, .true_ub = 0, .lb = 0, .ub = 0,                                \
        .align = 0,                                                                  \
        .nbElems = 1,                                                                \
        .name = OCOMS_DATATYPE_INIT_NAME(NAME),                                       \
        .desc = OCOMS_DATATYPE_INIT_DESC_PREDEFINED(UNAVAILABLE),                     \
        .opt_desc = OCOMS_DATATYPE_INIT_DESC_PREDEFINED(UNAVAILABLE),                 \
        .btypes = OCOMS_DATATYPE_INIT_BTYPES_ARRAY_UNAVAILABLE                        \
    }

#define OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE( FLAGS )                               \
    OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( UNAVAILABLE, (FLAGS) )

#define OCOMS_DATATYPE_INITIALIZER_EMPTY( FLAGS )                        \
    {                                                                   \
        .super = OCOMS_OBJ_STATIC_INIT(ocoms_datatype_t),                 \
        .flags = OCOMS_DATATYPE_FLAG_PREDEFINED | (FLAGS),               \
        .id = 0,                                                        \
        .bdt_used = 0,                                                  \
        .size = 0,                                                      \
        .true_lb = 0, .true_ub = 0, .lb = 0, .ub = 0,                   \
        .align = 0,                                                     \
        .nbElems = 1,                                                   \
        .name = OCOMS_DATATYPE_INIT_NAME(EMPTY),                         \
        .desc = OCOMS_DATATYPE_INIT_DESC_NULL,                           \
        .opt_desc = OCOMS_DATATYPE_INIT_DESC_NULL,                       \
        .btypes = OCOMS_DATATYPE_INIT_BTYPES_ARRAY_UNAVAILABLE           \
    }

#define OCOMS_DATATYPE_INIT_BASIC_TYPE( TYPE, NAME, FLAGS )              \
    {                                                                   \
        .super = OCOMS_OBJ_STATIC_INIT(ocoms_datatype_t),                 \
        .flags = OCOMS_DATATYPE_FLAG_PREDEFINED | (FLAGS),               \
        .id = TYPE,                                                     \
        .bdt_used = (((uint32_t)1)<<(TYPE)),                            \
        .size = 0,                                                      \
        .true_lb = 0, .true_ub = 0, .lb = 0, .ub = 0,                   \
        .align = 0,                                                     \
        .nbElems = 1,                                                   \
        .name = OCOMS_DATATYPE_INIT_NAME(NAME),                          \
        .desc = OCOMS_DATATYPE_INIT_DESC_NULL,                           \
        .opt_desc = OCOMS_DATATYPE_INIT_DESC_NULL,                       \
        .btypes = OCOMS_DATATYPE_INIT_BTYPES_ARRAY(NAME)                 \
    }

#define OCOMS_DATATYPE_INIT_BASIC_DATATYPE( TYPE, ALIGN, NAME, FLAGS )                \
    {                                                                                \
        .super = OCOMS_OBJ_STATIC_INIT(ocoms_datatype_t),                              \
        .flags = OCOMS_DATATYPE_FLAG_BASIC | (FLAGS),                                 \
        .id = OCOMS_DATATYPE_ ## NAME,                                                \
        .bdt_used = (((uint32_t)1)<<(OCOMS_DATATYPE_ ## NAME)),                       \
        .size = sizeof(TYPE),                                                        \
        .true_lb = 0, .true_ub = sizeof(TYPE), .lb = 0, .ub = sizeof(TYPE),          \
        .align = (ALIGN),                                                            \
        .nbElems = 1,                                                                \
        .name = OCOMS_DATATYPE_INIT_NAME(NAME),                                       \
        .desc = OCOMS_DATATYPE_INIT_DESC_PREDEFINED(NAME),                            \
        .opt_desc = OCOMS_DATATYPE_INIT_DESC_PREDEFINED(NAME),                        \
        .btypes = OCOMS_DATATYPE_INIT_BTYPES_ARRAY(NAME)                              \
    }

#define OCOMS_DATATYPE_INITIALIZER_LOOP(FLAGS)       OCOMS_DATATYPE_INIT_BASIC_TYPE( OCOMS_DATATYPE_LOOP, LOOP, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_END_LOOP(FLAGS)   OCOMS_DATATYPE_INIT_BASIC_TYPE( OCOMS_DATATYPE_END_LOOP, END_LOOP, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_LB(FLAGS)         OCOMS_DATATYPE_INIT_BASIC_TYPE( OCOMS_DATATYPE_LB, LB, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_UB(FLAGS)         OCOMS_DATATYPE_INIT_BASIC_TYPE( OCOMS_DATATYPE_UB, UB, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_INT1(FLAGS)       OCOMS_DATATYPE_INIT_BASIC_DATATYPE( int8_t, OCOMS_ALIGNMENT_INT8, INT1, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_INT2(FLAGS)       OCOMS_DATATYPE_INIT_BASIC_DATATYPE( int16_t, OCOMS_ALIGNMENT_INT16, INT2, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_INT4(FLAGS)       OCOMS_DATATYPE_INIT_BASIC_DATATYPE( int32_t, OCOMS_ALIGNMENT_INT32, INT4, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_INT8(FLAGS)       OCOMS_DATATYPE_INIT_BASIC_DATATYPE( int64_t, OCOMS_ALIGNMENT_INT64, INT8, FLAGS )
#ifdef HAVE_INT128_T
#define OCOMS_DATATYPE_INITIALIZER_INT16(FLAGS)      OCOMS_DATATYPE_INIT_BASIC_DATATYPE( int128_t, OCOMS_ALIGNMENT_INT128, INT16, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_INT16(FLAGS)      OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( INT16, FLAGS )
#endif
#define OCOMS_DATATYPE_INITIALIZER_UINT1(FLAGS)      OCOMS_DATATYPE_INIT_BASIC_DATATYPE( uint8_t, OCOMS_ALIGNMENT_INT8, UINT1, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_UINT2(FLAGS)      OCOMS_DATATYPE_INIT_BASIC_DATATYPE( uint16_t, OCOMS_ALIGNMENT_INT16, UINT2, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_UINT4(FLAGS)      OCOMS_DATATYPE_INIT_BASIC_DATATYPE( uint32_t, OCOMS_ALIGNMENT_INT32, UINT4, FLAGS )
#define OCOMS_DATATYPE_INITIALIZER_UINT8(FLAGS)      OCOMS_DATATYPE_INIT_BASIC_DATATYPE( uint64_t, OCOMS_ALIGNMENT_INT64, UINT8, FLAGS )
#ifdef HAVE_UINT128_T
#define OCOMS_DATATYPE_INITIALIZER_UINT16(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( uint128_t, OCOMS_ALIGNMENT_INT128, UINT16, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_UINT16(FLAGS)     OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( INT16, FLAGS )
#endif

#if SIZEOF_FLOAT == 2
#define OCOMS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( float, OCOMS_ALIGNMENT_FLOAT, FLOAT2, FLAGS )
#elif SIZEOF_DOUBLE == 2
#define OCOMS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( double, OCOMS_ALIGNMENT_DOUBLE, FLOAT2, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 2
#define OCOMS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( long double, OCOMS_ALIGNMENT_LONG_DOUBLE, FLOAT2, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_FLOAT2(FLAGS)     OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT2, FLAGS )
#endif

#if SIZEOF_FLOAT == 4
#define OCOMS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( float, OCOMS_ALIGNMENT_FLOAT, FLOAT4, FLAGS )
#elif SIZEOF_DOUBLE == 4
#define OCOMS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( double, OCOMS_ALIGNMENT_DOUBLE, FLOAT4, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 4
#define OCOMS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( long double, OCOMS_ALIGNMENT_LONG_DOUBLE, FLOAT4, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_FLOAT4(FLAGS)     OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT4, FLAGS )
#endif

#if SIZEOF_FLOAT == 8
#define OCOMS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( float, OCOMS_ALIGNMENT_FLOAT, FLOAT8, FLAGS )
#elif SIZEOF_DOUBLE == 8
#define OCOMS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( double, OCOMS_ALIGNMENT_DOUBLE, FLOAT8, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 8
#define OCOMS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     OCOMS_DATATYPE_INIT_BASIC_DATATYPE( long double, OCOMS_ALIGNMENT_LONG_DOUBLE, FLOAT8, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_FLOAT8(FLAGS)     OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT8, FLAGS )
#endif

#if SIZEOF_FLOAT == 12
#define OCOMS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    OCOMS_DATATYPE_INIT_BASIC_DATATYPE( float, OCOMS_ALIGNMENT_FLOAT, FLOAT12, FLAGS )
#elif SIZEOF_DOUBLE == 12
#define OCOMS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    OCOMS_DATATYPE_INIT_BASIC_DATATYPE( double, OCOMS_ALIGNMENT_DOUBLE, FLOAT12, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 12
#define OCOMS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    OCOMS_DATATYPE_INIT_BASIC_DATATYPE( long double, OCOMS_ALIGNMENT_LONG_DOUBLE, FLOAT12, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_FLOAT12(FLAGS)    OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT12, FLAGS )
#endif

#if SIZEOF_FLOAT == 16
#define OCOMS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    OCOMS_DATATYPE_INIT_BASIC_DATATYPE( float, OCOMS_ALIGNMENT_FLOAT, FLOAT16, FLAGS )
#elif SIZEOF_DOUBLE == 16
#define OCOMS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    OCOMS_DATATYPE_INIT_BASIC_DATATYPE( double, OCOMS_ALIGNMENT_DOUBLE, FLOAT16, FLAGS )
#elif SIZEOF_LONG_DOUBLE == 16
#define OCOMS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    OCOMS_DATATYPE_INIT_BASIC_DATATYPE( long double, OCOMS_ALIGNMENT_LONG_DOUBLE, FLOAT16, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_FLOAT16(FLAGS)    OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT16, FLAGS )
#endif

#if HAVE_FLOAT__COMPLEX
#define OCOMS_DATATYPE_INITIALIZER_FLOAT_COMPLEX(FLAGS) OCOMS_DATATYPE_INIT_BASIC_DATATYPE( float _Complex, OCOMS_ALIGNMENT_FLOAT_COMPLEX, FLOAT_COMPLEX, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_FLOAT_COMPLEX(FLAGS) OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( FLOAT_COMPLEX, FLAGS)
#endif

#if HAVE_DOUBLE__COMPLEX
#define OCOMS_DATATYPE_INITIALIZER_DOUBLE_COMPLEX(FLAGS) OCOMS_DATATYPE_INIT_BASIC_DATATYPE( double _Complex, OCOMS_ALIGNMENT_DOUBLE_COMPLEX, DOUBLE_COMPLEX, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_DOUBLE_COMPLEX(FLAGS) OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( DOUBLE_COMPLEX, FLAGS)
#endif

#if HAVE_LONG_DOUBLE__COMPLEX
#define OCOMS_DATATYPE_INITIALIZER_LONG_DOUBLE_COMPLEX(FLAGS) OCOMS_DATATYPE_INIT_BASIC_DATATYPE( long double _Complex, OCOMS_ALIGNMENT_LONG_DOUBLE_COMPLEX, LONG_DOUBLE_COMPLEX, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_LONG_DOUBLE_COMPLEX(FLAGS) OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( LONG_DOUBLE_COMPLEX, FLAGS)
#endif

#define OCOMS_DATATYPE_INITIALIZER_BOOL(FLAGS)       OCOMS_DATATYPE_INIT_BASIC_DATATYPE( _Bool, OCOMS_ALIGNMENT_BOOL, BOOL, FLAGS )

#if OCOMS_ALIGNMENT_WCHAR != 0
#define OCOMS_DATATYPE_INITIALIZER_WCHAR(FLAGS)      OCOMS_DATATYPE_INIT_BASIC_DATATYPE( wchar_t, OCOMS_ALIGNMENT_WCHAR, WCHAR, FLAGS )
#else
#define OCOMS_DATATYPE_INITIALIZER_WCHAR(FLAGS)      OCOMS_DATATYPE_INITIALIZER_UNAVAILABLE_NAMED( WCHAR, FLAGS )
#endif

#define BASIC_DDT_FROM_ELEM( ELEM ) (ocoms_datatype_basicDatatypes[(ELEM).elem.common.type])

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

#if OCOMS_ENABLE_DEBUG
#define OCOMS_DATATYPE_SAFEGUARD_POINTER( ACTPTR, LENGTH, INITPTR, PDATA, COUNT ) \
    {                                                                   \
        unsigned char *__lower_bound = (INITPTR), *__upper_bound;       \
        assert( ((LENGTH) != 0) && ((COUNT) != 0) );                    \
        __lower_bound += (PDATA)->true_lb;                              \
        __upper_bound = (INITPTR) + (PDATA)->true_ub +                  \
            ((PDATA)->ub - (PDATA)->lb) * ((COUNT) - 1);                \
        if( ((ACTPTR) < __lower_bound) || ((ACTPTR) >= __upper_bound) ) { \
            ocoms_datatype_safeguard_pointer_debug_breakpoint( (ACTPTR), (LENGTH), (INITPTR), (PDATA), (COUNT) ); \
            ocoms_output( 0, "%s:%d\n\tPointer %p size %lu is outside [%p,%p] for\n\tbase ptr %p count %d and data \n", \
                         __FILE__, __LINE__, (ACTPTR), (unsigned long)(LENGTH), __lower_bound, __upper_bound, \
                         (INITPTR), (COUNT) );                          \
            ocoms_datatype_dump( (PDATA) );                                   \
        }                                                               \
    }

#else
#define OCOMS_DATATYPE_SAFEGUARD_POINTER( ACTPTR, LENGTH, INITPTR, PDATA, COUNT )
#endif  /* OCOMS_ENABLE_DEBUG */

static inline int GET_FIRST_NON_LOOP( const union dt_elem_desc* _pElem )
{
    int element_index = 0;

    /* We dont have to check for the end as we always put an END_LOOP
     * at the end of all datatype descriptions.
     */
    while( _pElem->elem.common.type == OCOMS_DATATYPE_LOOP ) {
        ++_pElem; element_index++;
    }
    return element_index;
}

#define UPDATE_INTERNAL_COUNTERS( DESCRIPTION, POSITION, ELEMENT, COUNTER ) \
    do {                                                                \
        (ELEMENT) = &((DESCRIPTION)[(POSITION)]);                       \
        (COUNTER) = (ELEMENT)->elem.count;                              \
    } while (0)

OCOMS_DECLSPEC int ocoms_datatype_contain_basic_datatypes( const struct ocoms_datatype_t* pData, char* ptr, size_t length );
OCOMS_DECLSPEC int ocoms_datatype_dump_data_flags( unsigned short usflags, char* ptr, size_t length );
OCOMS_DECLSPEC int ocoms_datatype_dump_data_desc( union dt_elem_desc* pDesc, int nbElems, char* ptr, size_t length );

END_C_DECLS
#endif  /* OCOMS_DATATYPE_INTERNAL_H_HAS_BEEN_INCLUDED */
