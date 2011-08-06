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
# Copyright (c) 2011      NVIDIA Corporation.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

headers = \
        service_convertor.h \
        service_convertor_internal.h \
        service_datatype_checksum.h \
        service_datatype.h \
        service_datatype_internal.h \
        service_datatype_copy.h \
        service_datatype_memcpy.h \
        service_datatype_pack.h \
        service_datatype_prototypes.h \
        service_datatype_unpack.h


noinst_LTLIBRARIES = \
        libdatatype_reliable.la \
        libdatatype.la

# these sources will be compiled with the special -D
libdatatype_reliable_la_SOURCES = service_datatype_pack.c service_datatype_unpack.c
libdatatype_reliable_la_CFLAGS = -DCHECKSUM $(AM_CFLAGS)

# these sources will be compiled with the normal CFLAGS only
libdatatype_la_SOURCES = \
        service_convertor.c \
        service_convertor_raw.c \
        service_copy_functions.c \
        service_copy_functions_heterogeneous.c \
        service_datatype_add.c \
        service_datatype_clone.c \
        service_datatype_copy.c \
        service_datatype_create.c \
        service_datatype_create_contiguous.c \
        service_datatype_destroy.c \
        service_datatype_dump.c \
        service_datatype_fake_stack.c \
        service_datatype_get_count.c \
        service_datatype_module.c \
        service_datatype_optimize.c \
        service_datatype_pack.c \
        service_datatype_position.c \
        service_datatype_resize.c \
        service_datatype_unpack.c

libdatatype_la_LIBADD = libdatatype_reliable.la

# Conditionally install the header files
#if WANT_INSTALL_HEADERS
#servicedir = $(includedir)/ccs/$(subdir)
#service_HEADERS = $(headers)
#endif

# If we have cuda support, modify file list and flags
if CCS_cuda_support
libdatatype_la_SOURCES += service_datatype_cuda.c
headers += service_datatype_cuda.h
AM_CPPFLAGS = $(service_datatype_CPPFLAGS)
libdatatype_la_LIBADD += $(service_datatype_LIBS)
endif
