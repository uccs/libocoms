/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2010 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2009      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2009-2011 Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 * This file is included at the bottom of ccs_config.h, and is
 * therefore a) after all the #define's that were output from
 * configure, and b) included in most/all files in Open MPI.
 *
 * Since this file is *only* ever included by ccs_config.h, and
 * ccs_config.h already has #ifndef/#endif protection, there is no
 * need to #ifndef/#endif protection here.
 */

#ifndef CCS_CONFIG_H 
#error "ccs_config_bottom.h should only be included from ccs_config.h"
#endif

/*
 * If we build a static library, Visual C define the _LIB symbol. In the
 * case of a shared library _USERDLL get defined.
 *
 * OMPI_BUILDING and _LIB define how ccs_config.h
 * handles configuring all of Open MPI's "compatibility" code.  Both
 * constants will always be defined by the end of ccs_config.h.
 *
 * OMPI_BUILDING affects how much compatibility code is included by
 * ccs_config.h.  It will always be 1 or 0.  The user can set the
 * value before including either mpi.h or ccs_config.h and it will be
 * respected.  If ccs_config.h is included before mpi.h, it will
 * default to 1.  If mpi.h is included before ccs_config.h, it will
 * default to 0.
 */
#ifndef OMPI_BUILDING
#define OMPI_BUILDING 1
#endif

/*
 * Flex is trying to include the unistd.h file. As there is no configure
 * option or this, the flex generated files will try to include the file
 * even on platforms without unistd.h (such as Windows). Therefore, if we
 * know this file is not available, we can prevent flex from including it.
 */
#ifndef HAVE_UNISTD_H
#define YY_NO_UNISTD_H
#endif

/**
 * The attribute definition should be included before any potential
 * usage.
 */
#if CCS_HAVE_ATTRIBUTE_ALIGNED
#    define __service_attribute_aligned__(a)    __attribute__((__aligned__(a)))
#    define __service_attribute_aligned_max__   __attribute__((__aligned__))
#else
#    define __service_attribute_aligned__(a)
#    define __service_attribute_aligned_max__
#endif

#if CCS_HAVE_ATTRIBUTE_ALWAYS_INLINE
#    define __service_attribute_always_inline__ __attribute__((__always_inline__))
#else
#    define __service_attribute_always_inline__
#endif

#if CCS_HAVE_ATTRIBUTE_COLD
#    define __service_attribute_cold__          __attribute__((__cold__))
#else
#    define __service_attribute_cold__
#endif

#if CCS_HAVE_ATTRIBUTE_CONST
#    define __service_attribute_const__         __attribute__((__const__))
#else
#    define __service_attribute_const__
#endif

#if CCS_HAVE_ATTRIBUTE_DEPRECATED
#    define __service_attribute_deprecated__    __attribute__((__deprecated__))
#else
#    define __service_attribute_deprecated__
#endif

#if CCS_HAVE_ATTRIBUTE_FORMAT
#    define __service_attribute_format__(a,b,c) __attribute__((__format__(a, b, c)))
#else
#    define __service_attribute_format__(a,b,c)
#endif

/* Use this __atribute__ on function-ptr declarations, only */
#if CCS_HAVE_ATTRIBUTE_FORMAT_FUNCPTR
#    define __service_attribute_format_funcptr__(a,b,c) __attribute__((__format__(a, b, c)))
#else
#    define __service_attribute_format_funcptr__(a,b,c)
#endif

#if CCS_HAVE_ATTRIBUTE_HOT
#    define __service_attribute_hot__           __attribute__((__hot__))
#else
#    define __service_attribute_hot__
#endif

#if CCS_HAVE_ATTRIBUTE_MALLOC
#    define __service_attribute_malloc__        __attribute__((__malloc__))
#else
#    define __service_attribute_malloc__
#endif

#if CCS_HAVE_ATTRIBUTE_MAY_ALIAS
#    define __service_attribute_may_alias__     __attribute__((__may_alias__))
#else
#    define __service_attribute_may_alias__
#endif

#if CCS_HAVE_ATTRIBUTE_NO_INSTRUMENT_FUNCTION
#    define __service_attribute_no_instrument_function__  __attribute__((__no_instrument_function__))
#else
#    define __service_attribute_no_instrument_function__
#endif

#if CCS_HAVE_ATTRIBUTE_NONNULL
#    define __service_attribute_nonnull__(a)    __attribute__((__nonnull__(a)))
#    define __service_attribute_nonnull_all__   __attribute__((__nonnull__))
#else
#    define __service_attribute_nonnull__(a)
#    define __service_attribute_nonnull_all__
#endif

#if CCS_HAVE_ATTRIBUTE_NORETURN
#    define __service_attribute_noreturn__      __attribute__((__noreturn__))
#else
#    define __service_attribute_noreturn__
#endif

/* Use this __atribute__ on function-ptr declarations, only */
#if CCS_HAVE_ATTRIBUTE_NORETURN_FUNCPTR
#    define __service_attribute_noreturn_funcptr__  __attribute__((__noreturn__))
#else
#    define __service_attribute_noreturn_funcptr__
#endif

#if CCS_HAVE_ATTRIBUTE_PACKED
#    define __service_attribute_packed__        __attribute__((__packed__))
#else
#    define __service_attribute_packed__
#endif

#if CCS_HAVE_ATTRIBUTE_PURE
#    define __service_attribute_pure__          __attribute__((__pure__))
#else
#    define __service_attribute_pure__
#endif

#if CCS_HAVE_ATTRIBUTE_SENTINEL
#    define __service_attribute_sentinel__      __attribute__((__sentinel__))
#else
#    define __service_attribute_sentinel__
#endif

#if CCS_HAVE_ATTRIBUTE_UNUSED
#    define __service_attribute_unused__        __attribute__((__unused__))
#else
#    define __service_attribute_unused__
#endif

#if CCS_HAVE_ATTRIBUTE_VISIBILITY
#    define __service_attribute_visibility__(a) __attribute__((__visibility__(a)))
#else
#    define __service_attribute_visibility__(a)
#endif

#if CCS_HAVE_ATTRIBUTE_WARN_UNUSED_RESULT
#    define __service_attribute_warn_unused_result__ __attribute__((__warn_unused_result__))
#else
#    define __service_attribute_warn_unused_result__
#endif

#if CCS_HAVE_ATTRIBUTE_WEAK_ALIAS
#    define __service_attribute_weak_alias__(a) __attribute__((__weak__, __alias__(a)))
#else
#    define __service_attribute_weak_alias__(a)
#endif

#  if CCS_C_HAVE_VISIBILITY
#    define CCS_DECLSPEC           __service_attribute_visibility__("default")
#    define CCS_MODULE_DECLSPEC    __service_attribute_visibility__("default")
#  else
#    define CCS_DECLSPEC
#    define CCS_MODULE_DECLSPEC
#  endif

/*
 * BEGIN_C_DECLS should be used at the beginning of your declarations,
 * so that C++ compilers don't mangle their names.  Use END_C_DECLS at
 * the end of C declarations.
 */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#if defined(c_plusplus) || defined(__cplusplus)
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS } 
#else
#define BEGIN_C_DECLS          /* empty */
#define END_C_DECLS            /* empty */
#endif

#ifndef HAVE_PTRDIFF_T
typedef CCS_PTRDIFF_TYPE ptrdiff_t;
#endif

/*
 * If we're in C, we may need to bring in the bool type and true/false
 * constants.  CCS_NEED_C_BOOL will be true if the compiler either
 * needs <stdbool.h> or doesn't define the bool type at all.
 */
#if !(defined(c_plusplus) || defined(__cplusplus))
#    if CCS_NEED_C_BOOL
#        if CCS_USE_STDBOOL_H
             /* If we're using <stdbool.h>, there is an implicit
                assumption that the C++ bool is the same size and has
                the same alignment.  However, configure may have
                disabled the MPI C++ bindings, so if "_Bool" exists,
                then use that sizeof. */
#            include <stdbool.h>
             /* This section exists because AC_SIZEOF(bool) may not be
                run in configure if we're not building the MPI C++
                bindings. */
#            undef SIZEOF_BOOL
#            if SIZEOF__BOOL > 0
#                define SIZEOF_BOOL SIZEOF__BOOL
#            else
                 /* If all else fails, assume it's 1 */
#                define SIZEOF_BOOL 1
#            endif
#        else
             /* We need to create a bool type and ensure that it's the
                same size / alignment as the C++ bool size /
                alignment */
#            define false 0
#            define true 1
#            if SIZEOF_BOOL == SIZEOF_CHAR && CCS_ALIGNMENT_CXX_BOOL == CCS_ALIGNMENT_CHAR
typedef unsigned char bool;
#            elif SIZEOF_BOOL == SIZEOF_SHORT && CCS_ALIGNMENT_CXX_BOOL == CCS_ALIGNMENT_SHORT
typedef short bool;
#            elif SIZEOF_BOOL == SIZEOF_INT && CCS_ALIGNMENT_CXX_BOOL == CCS_ALIGNMENT_INT
typedef int bool;
#            elif SIZEOF_BOOL == SIZEOF_LONG && CCS_ALIGNMENT_CXX_BOOL == CCS_ALIGNMENT_LONG
typedef long bool;
#            elif defined(SIZEOF_LONG_LONG) && defined(CCS_ALIGNMENT_LONG) && SIZEOF_BOOL == SIZEOF_LONG && CCS_ALIGNMENT_CXX_BOOL == CCS_ALIGNMENT_LONG
typedef long long bool;
             /* If we have _Bool, use that */
#            elif SIZEOF__BOOL > 0
#                undef SIZEOF_BOOL
#                define bool _Bool
#                define SIZEOF_BOOL SIZEOF__BOOL
#            else
             /* If all else fails, just make bool be an unsigned char
                and size of 1 */
typedef unsigned char bool;
#                define SIZEOF_BOOL 1
#            endif
#        endif  /* CCS_USE_STDBOOL_H */
#    endif  /* CCS_NEED_C_BOOL */
#else
  bummer
#endif

  /**
   * Because of the way we're using the opal_object inside Open MPI (i.e.
   * dynamic resolution at run-time to derive all objects from the basic
   * type), on Windows we have to build everything on C++ mode, simply
   * because the C mode does not support dynamic resolution in DLL.  Therefore,
   * no automatic conversion is allowed. All types have to be explicitly casted
   * or the compiler generate an error. This is true even for the void* type. As
   * we use void* to silence others compilers in the resolution of the addr member
   * of the iovec structure, we have now to find a way around. The simplest solution
   * is to define a special type for this field (just for casting). It can be
   * set to void* on all platforms with the exception of windows where it has to be
   * char*.
   */
#if defined(__WINDOWS__)
#define IOVBASE_TYPE  char
#else
#define IOVBASE_TYPE  void
#endif  /* defined(__WINDOWS__) */

