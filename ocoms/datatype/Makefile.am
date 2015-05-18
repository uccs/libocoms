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

datatype_headers = \
        ocoms_convertor.h \
        ocoms_convertor_internal.h \
        ocoms_datatype_checksum.h \
        ocoms_datatype.h \
        ocoms_datatype_internal.h \
        ocoms_datatype_copy.h \
        ocoms_datatype_memcpy.h \
        ocoms_datatype_pack.h \
        ocoms_datatype_prototypes.h \
        ocoms_datatype_unpack.h


noinst_LTLIBRARIES = \
        libdatatype_reliable.la \
        libdatatype.la

# these sources will be compiled with the special -D
libdatatype_reliable_la_SOURCES = ocoms_datatype_pack.c ocoms_datatype_unpack.c
libdatatype_reliable_la_CFLAGS = -DCHECKSUM $(AM_CFLAGS)

# these sources will be compiled with the normal CFLAGS only
libdatatype_la_SOURCES = \
        $(datatype_headers) \
        ocoms_convertor.c \
        ocoms_convertor_raw.c \
        ocoms_copy_functions.c \
        ocoms_copy_functions_heterogeneous.c \
        ocoms_datatype_add.c \
        ocoms_datatype_clone.c \
        ocoms_datatype_copy.c \
        ocoms_datatype_create.c \
        ocoms_datatype_create_contiguous.c \
        ocoms_datatype_destroy.c \
        ocoms_datatype_dump.c \
        ocoms_datatype_fake_stack.c \
        ocoms_datatype_get_count.c \
        ocoms_datatype_module.c \
        ocoms_datatype_optimize.c \
        ocoms_datatype_pack.c \
        ocoms_datatype_position.c \
        ocoms_datatype_resize.c \
        ocoms_datatype_unpack.c

libdatatype_la_LIBADD = libdatatype_reliable.la

# Conditionally install the header files
#if WANT_INSTALL_HEADERS
#ocomsdir = $(includedir)/ocoms/$(subdir)
#ocoms_HEADERS = $(headers)
#endif

# If we have cuda support, modify file list and flags
if OCOMS_cuda_support
libdatatype_la_SOURCES += ocoms_datatype_cuda.c
datatype_headers += ocoms_datatype_cuda.h
endif
