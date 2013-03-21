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

SUBDIRS = keyval

dist_pkgdata_DATA = 

#AM_LFLAGS = -Popal_show_help_yy
#LEX_OUTPUT_ROOT = lex.opal_show_help_yy

noinst_LTLIBRARIES = libocomsutil.la

# Source code files

util_headers = \
        argv.h \
        cmd_line.h \
        fd.h \
        output.h \
        ocoms_atomic_lifo.h \
        ocoms_bitmap.h \
        ocoms_free_list.h \
        ocoms_list.h \
        ocoms_object.h \
        ocoms_pointer_array.h \
        ocoms_rb_tree.h \
        ocoms_graph.h \
        ocoms_environ.h \
        ocoms_value_array.h \
        printf.h \
        ocoms_hash_table.h \
        if.h \
        arch.h \
        crc.h \
        os_path.h \
        path.h \
        keyval_parse.h

libocomsutil_la_SOURCES = \
        $(util_headers) \
        argv.c \
        cmd_line.c \
        fd.c \
        output.c \
        ocoms_atomic_lifo.c \
        ocoms_free_list.c \
        ocoms_list.c \
        ocoms_object.c \
        ocoms_pointer_array.c \
        ocoms_rb_tree.c \
        ocoms_graph.c \
        ocoms_environ.c \
        ocoms_value_array.c \
        ocoms_bitmap.c \
        printf.c \
        ocoms_hash_table.c \
        if.c \
        arch.c \
        crc.c \
        os_path.c \
        path.c \
        keyval_parse.c

libocomsutil_la_LIBADD =  \
    keyval/libocomsutilkeyval.la

libocomsutil_la_DEPENDENCIES = \
    keyval/libocomsutilkeyval.la

CFLAGS += @RTE_CFLAGS@
LDFLAGS += @RTE_LDFLAGS@

