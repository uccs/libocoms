# -*- makefile -*-
#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2010 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
#                         reserved.
# Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
# Copyright (c) 2010 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2011-2013 NVIDIA Corporation.  All rights reserved.
# Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
# Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

dstore_headers = \
	dstore.h \
	base/base.h \
	dstore_types.h \
	hash/dstore_hash.h \
	base/static-components.h

noinst_LTLIBRARIES = libdstore.la

# these sources will be compiled with the normal CFLAGS only
libdstore_la_SOURCES = \
        $(dstore_headers) \
	base/dstore_base_frame.c \
	base/dstore_base_select.c \
	base/dstore_base_stubs.c \
	hash/dstore_hash_component.c \
	hash/dstore_hash.c
