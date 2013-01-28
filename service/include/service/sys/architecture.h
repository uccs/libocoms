/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
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

/*
 * List of supported architectures
 */

#ifndef CCS_SYS_ARCHITECTURE_H
#define CCS_SYS_ARCHITECTURE_H

/* Architectures */
#define CCS_UNSUPPORTED    0000
#define CCS_WINDOWS        0001
#define CCS_IA32           0010
#define CCS_IA64           0020
#define CCS_AMD64          0030
#define CCS_ALPHA          0040
#define CCS_POWERPC32      0050
#define CCS_POWERPC64      0051
#define CCS_SPARC          0060
#define CCS_SPARCV9_32     0061
#define CCS_SPARCV9_64     0062
#define CCS_MIPS           0070
#define CCS_ARM            0100

/* Formats */
#define CCS_DEFAULT        1000  /* standard for given architecture */
#define CCS_DARWIN         1001  /* Darwin / OS X on PowerPC */
#define CCS_PPC_LINUX      1002  /* Linux on PowerPC */
#define CCS_AIX            1003  /* AIX on Power / PowerPC */

#endif /* #ifndef CCS_SYS_ARCHITECTURE_H */
