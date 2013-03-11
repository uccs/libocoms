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

#ifndef CCS_SYS_TIMER_H
#define CCS_SYS_TIMER_H 1

#include "service/platform/ccs_config.h"

#include "service/sys/architecture.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* do some quick #define cleanup in cases where we are doing
   testing... */
#ifdef CCS_DISABLE_INLINE_ASM
#undef CCS_C_GCC_INLINE_ASSEMBLY
#define CCS_C_GCC_INLINE_ASSEMBLY 0
#undef CCS_CXX_GCC_INLINE_ASSEMBLY
#define CCS_CXX_GCC_INLINE_ASSEMBLY 0
#undef CCS_C_DEC_INLINE_ASSEMBLY
#define CCS_C_DEC_INLINE_ASSEMBLY 0
#undef CCS_CXX_DEC_INLINE_ASSEMBLY
#define CCS_CXX_DEC_INLINE_ASSEMBLY 0
#undef CCS_C_XLC_INLINE_ASSEMBLY
#define CCS_C_XLC_INLINE_ASSEMBLY 0
#undef CCS_CXX_XLC_INLINE_ASSEMBLY
#define CCS_CXX_XLC_INLINE_ASSEMBLY 0
#endif

/* define CCS_{GCC,DEC,XLC}_INLINE_ASSEMBLY based on the
   CCS_{C,CXX}_{GCC,DEC,XLC}_INLINE_ASSEMBLY defines and whether we
   are in C or C++ */
#if defined(c_plusplus) || defined(__cplusplus)
#define CCS_GCC_INLINE_ASSEMBLY CCS_CXX_GCC_INLINE_ASSEMBLY
#define CCS_DEC_INLINE_ASSEMBLY CCS_CXX_DEC_INLINE_ASSEMBLY
#define CCS_XLC_INLINE_ASSEMBLY CCS_CXX_XLC_INLINE_ASSEMBLY
#else
#define CCS_GCC_INLINE_ASSEMBLY CCS_C_GCC_INLINE_ASSEMBLY
#define CCS_DEC_INLINE_ASSEMBLY CCS_C_DEC_INLINE_ASSEMBLY
#define CCS_XLC_INLINE_ASSEMBLY CCS_C_XLC_INLINE_ASSEMBLY
#endif

/**********************************************************************
 *
 * Load the appropriate architecture files and set some reasonable
 * default values for our support
 *
 *********************************************************************/

BEGIN_C_DECLS

/* If you update this list, you probably also want to update
   ccs/mca/systimer/linux/configure.m4.  Or not. */

#if defined(DOXYGEN)
/* don't include system-level gorp when generating doxygen files */ 
#elif CCS_ASSEMBLY_ARCH == CCS_AMD64
#include "service/sys/amd64/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_ARM
#include "service/sys/arm/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_IA32
#include "service/sys/ia32/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_IA64
#include "service/sys/ia64/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_POWERPC32
#include "service/sys/powerpc/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_POWERPC64
#include "service/sys/powerpc/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_SPARCV9_32
#include "service/sys/sparcv9/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_SPARCV9_64
#include "service/sys/sparcv9/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_WINDOWS
#include "service/sys/win32/timer.h"
#elif CCS_ASSEMBLY_ARCH == CCS_MIPS
#include "service/sys/mips/timer.h"
#endif

#ifndef DOXYGEN
#ifndef CCS_HAVE_SYS_TIMER_GET_CYCLES
#define CCS_HAVE_SYS_TIMER_GET_CYCLES 0

typedef int ccs_timer_t;
#endif
#endif

END_C_DECLS

#endif /* CCS_SYS_TIMER_H */
