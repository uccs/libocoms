#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2007-2010 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

dist_pkgdata_DATA = 

#AM_LFLAGS = -Popal_show_help_yy
#LEX_OUTPUT_ROOT = lex.opal_show_help_yy

noinst_LTLIBRARIES = libserviceutil.la

# Source code files

util_headers = \
        argv.h \
        cmd_line.h \
        fd.h \
        output.h \
        service_atomic_lifo.h \
        service_bitmap.h \
        service_free_list.h \
        service_list.h \
        service_object.h \
        service_pointer_array.h \
        service_rb_tree.h

libserviceutil_la_SOURCES = \
        $(util_headers) \
        argv.c \
        cmd_line.c \
        fd.h \
        fd.c \
        output.c \
        service_atomic_lifo.h \
        service_bitmap.h \
        service_free_list.h \
        service_list.h \
        service_object.h \
        service_pointer_array.c \
        service_pointer_array.h \
        service_rb_tree.h \
        service_rb_tree.c

libserviceutil_la_LIBADD = 

libserviceutil_la_DEPENDENCIES =

# Conditionally install the header files

#if WANT_INSTALL_HEADERS
#opaldir = $(includedir)/ccs/$(subdir)
#opal_HEADERS = $(headers)
#endif
