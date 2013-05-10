/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OCOMS_MEMORY_MEMORY_INTERNAL_H
#define OCOMS_MEMORY_MEMORY_INTERNAL_H

#include "ocoms/platform/ocoms_config.h"
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

/* see memory.h for meaning */
#define OCOMS_MEMORY_FREE_SUPPORT   0x0001
#define OCOMS_MEMORY_MUNMAP_SUPPORT 0x0002
#define OCOMS_MEMORY_CHUNK_SUPPORT  0x0004

BEGIN_C_DECLS

OCOMS_DECLSPEC void ocoms_mem_hooks_set_support(int support);

OCOMS_DECLSPEC void ocoms_mem_hooks_release_hook(void *buf, size_t length, bool from_alloc);

END_C_DECLS


#endif /* OCOMS_MEMORY_MEMORY_INTERNAL_H */
