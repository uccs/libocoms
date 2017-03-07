/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
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
 * Cycle counter reading instructions.  Do not use directly - see the
 * timer interface instead
 */

#ifndef OCOMS_SYS_TIMER_H
#define OCOMS_SYS_TIMER_H 1

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/sys/architecture.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* do some quick #define cleanup in cases where we are doing
   testing... */
#ifdef OCOMS_DISABLE_INLINE_ASM
#undef OCOMS_C_GCC_INLINE_ASSEMBLY
#define OCOMS_C_GCC_INLINE_ASSEMBLY 0
#undef OCOMS_CXX_GCC_INLINE_ASSEMBLY
#define OCOMS_CXX_GCC_INLINE_ASSEMBLY 0
#undef OCOMS_C_DEC_INLINE_ASSEMBLY
#define OCOMS_C_DEC_INLINE_ASSEMBLY 0
#undef OCOMS_CXX_DEC_INLINE_ASSEMBLY
#define OCOMS_CXX_DEC_INLINE_ASSEMBLY 0
#undef OCOMS_C_XLC_INLINE_ASSEMBLY
#define OCOMS_C_XLC_INLINE_ASSEMBLY 0
#undef OCOMS_CXX_XLC_INLINE_ASSEMBLY
#define OCOMS_CXX_XLC_INLINE_ASSEMBLY 0
#endif

/* define OCOMS_{GCC,DEC,XLC}_INLINE_ASSEMBLY based on the
   OCOMS_{C,CXX}_{GCC,DEC,XLC}_INLINE_ASSEMBLY defines and whether we
   are in C or C++ */
#if defined(c_plusplus) || defined(__cplusplus)
#define OCOMS_GCC_INLINE_ASSEMBLY OCOMS_CXX_GCC_INLINE_ASSEMBLY
#define OCOMS_DEC_INLINE_ASSEMBLY OCOMS_CXX_DEC_INLINE_ASSEMBLY
#define OCOMS_XLC_INLINE_ASSEMBLY OCOMS_CXX_XLC_INLINE_ASSEMBLY
#else
#define OCOMS_GCC_INLINE_ASSEMBLY OCOMS_C_GCC_INLINE_ASSEMBLY
#define OCOMS_DEC_INLINE_ASSEMBLY OCOMS_C_DEC_INLINE_ASSEMBLY
#define OCOMS_XLC_INLINE_ASSEMBLY OCOMS_C_XLC_INLINE_ASSEMBLY
#endif

/**********************************************************************
 *
 * Load the appropriate architecture files and set some reasonable
 * default values for our support
 *
 *********************************************************************/

BEGIN_C_DECLS

/* If you update this list, you probably also want to update
   ocoms/mca/systimer/linux/configure.m4.  Or not. */

#if defined(DOXYGEN)
/* don't include system-level gorp when generating doxygen files */ 
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_AMD64
#include "ocoms/sys/amd64/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_ARM
#include "ocoms/sys/arm/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_ARM64
#include "ocoms/sys/arm64/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_IA32
#include "ocoms/sys/ia32/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_IA64
#include "ocoms/sys/ia64/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_POWERPC32
#include "ocoms/sys/powerpc/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_POWERPC64
#include "ocoms/sys/powerpc/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_SPARCV9_32
#include "ocoms/sys/sparcv9/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_SPARCV9_64
#include "ocoms/sys/sparcv9/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_WINDOWS
#include "ocoms/sys/win32/timer.h"
#elif OCOMS_ASSEMBLY_ARCH == OCOMS_MIPS
#include "ocoms/sys/mips/timer.h"
#endif

#ifndef DOXYGEN
#ifndef OCOMS_HAVE_SYS_TIMER_GET_CYCLES
#define OCOMS_HAVE_SYS_TIMER_GET_CYCLES 0

typedef int ocoms_timer_t;
#endif
#endif

END_C_DECLS

#endif /* OCOMS_SYS_TIMER_H */
