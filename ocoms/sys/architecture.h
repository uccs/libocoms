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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/*
 * List of supported architectures
 */

#ifndef OCOMS_SYS_ARCHITECTURE_H
#define OCOMS_SYS_ARCHITECTURE_H


/* Architectures */
#define OCOMS_UNSUPPORTED    0000
#define OCOMS_WINDOWS        0001
#define OCOMS_IA32           0010
#define OCOMS_IA64           0020
#define OCOMS_AMD64          0030
#define OCOMS_ALPHA          0040
#define OCOMS_POWERPC32      0050
#define OCOMS_POWERPC64      0051
#define OCOMS_SPARC          0060
#define OCOMS_SPARCV9_32     0061
#define OCOMS_SPARCV9_64     0062
#define OCOMS_MIPS           0070
#define OCOMS_ARM            0100

/* Formats */
#define OCOMS_DEFAULT        1000  /* standard for given architecture */
#define OCOMS_DARWIN         1001  /* Darwin / OS X on PowerPC */
#define OCOMS_PPC_LINUX      1002  /* Linux on PowerPC */
#define OCOMS_AIX            1003  /* AIX on Power / PowerPC */

#endif /* #ifndef OCOMS_SYS_ARCHITECTURE_H */
