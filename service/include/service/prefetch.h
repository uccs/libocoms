/*
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/** @file
 *
 * Compiler-specific prefetch functions
 *
 * A small set of prefetch / prediction interfaces for using compiler
 * directives to improve memory prefetching and branch prediction
 */

#ifndef CCS_PREFETCH_H
#define CCS_PREFETCH_H

#if defined(c_plusplus) || defined(__cplusplus)
/* C++ code */

#if OMPI_CXX_HAVE_BUILTIN_EXPECT
#define CCS_LIKELY(expression) __builtin_expect(!!(expression), 1)
#define CCS_UNLIKELY(expression) __builtin_expect(!!(expression), 0)
#else
#define CCS_LIKELY(expression) (expression)
#define CCS_UNLIKELY(expression) (expression)
#endif

#if OMPI_CXX_HAVE_BUILTIN_PREFETCH
#define CCS_PREFETCH(address,rw,locality) __builtin_prefetch(address,rw,locality)
#else
#define CCS_PREFETCH(address,rw,locality)
#endif

#else
/* C code */

#if CCS_C_HAVE_BUILTIN_EXPECT
#define CCS_LIKELY(expression) __builtin_expect(!!(expression), 1)
#define CCS_UNLIKELY(expression) __builtin_expect(!!(expression), 0)
#else
#define CCS_LIKELY(expression) (expression)
#define CCS_UNLIKELY(expression) (expression)
#endif

#if CCS_C_HAVE_BUILTIN_PREFETCH
#define CCS_PREFETCH(address,rw,locality) __builtin_prefetch(address,rw,locality)
#else
#define CCS_PREFETCH(address,rw,locality)
#endif

#endif

#endif
