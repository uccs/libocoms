/*
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
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

#ifndef OCOMS_PREFETCH_H
#define OCOMS_PREFETCH_H

#if defined(c_plusplus) || defined(__cplusplus)
/* C++ code */

#if OMPI_CXX_HAVE_BUILTIN_EXPECT
#define OCOMS_LIKELY(expression) __builtin_expect(!!(expression), 1)
#define OCOMS_UNLIKELY(expression) __builtin_expect(!!(expression), 0)
#else
#define OCOMS_LIKELY(expression) (expression)
#define OCOMS_UNLIKELY(expression) (expression)
#endif

#if OMPI_CXX_HAVE_BUILTIN_PREFETCH
#define OCOMS_PREFETCH(address,rw,locality) __builtin_prefetch(address,rw,locality)
#else
#define OCOMS_PREFETCH(address,rw,locality)
#endif

#else
/* C code */

#if OCOMS_C_HAVE_BUILTIN_EXPECT
#define OCOMS_LIKELY(expression) __builtin_expect(!!(expression), 1)
#define OCOMS_UNLIKELY(expression) __builtin_expect(!!(expression), 0)
#else
#define OCOMS_LIKELY(expression) (expression)
#define OCOMS_UNLIKELY(expression) (expression)
#endif

#if OCOMS_C_HAVE_BUILTIN_PREFETCH
#define OCOMS_PREFETCH(address,rw,locality) __builtin_prefetch(address,rw,locality)
#else
#define OCOMS_PREFETCH(address,rw,locality)
#endif

#endif

#endif
