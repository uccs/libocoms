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
# Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
# Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#


AM_LFLAGS = -Pocoms_util_keyval_yy
LEX_OUTPUT_ROOT = lex.ocoms_util_keyval_yy

noinst_LTLIBRARIES = libocomsutilkeyval.la

# Source code files

libocomsutilkeyval_la_SOURCES = \
	keyval_lex.h \
    keyval_lex.l

clean-local:
	rm -f keyval_lex.c
