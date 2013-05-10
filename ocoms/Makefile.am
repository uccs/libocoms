#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2009 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2009 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2009-2011 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
# Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

AM_CPPFLAGS =

SUBDIRS = \
	platform \
    $(LIBLTDL_SUBDIR) \
	asm \
    datatype \
    util 

# libltdl is included by variable because if --disable-dlopen was
# used, there will be no generated Makefile in that directory (and
# therefore make distclean will fail).
DIST_SUBDIRS = \
	platform \
	$(LIBLTDL_SUBDIR) \
	asm \
    datatype \
	util 

# Build the main OPAL library

lib_LTLIBRARIES = libocoms.la
libocoms_la_SOURCES =
libocoms_la_LIBADD = \
        $(LIBLTDL) \
        asm/libasm.la \
        datatype/libdatatype.la \
        util/libocomsutil.la

libocoms_la_DEPENDENCIES = $(libocoms_la_LIBADD)
libocoms_la_LDFLAGS =
#libocoms_la_LDFLAGS = -version-info $(libocoms_so_version)

# included subdirectory Makefile.am's and appended-to variables
headers =
noinst_LTLIBRARIES =
dist_pkgdata_DATA =
libocoms_la_SOURCES += $(headers)

# Conditionally install the header files

#if WANT_INSTALL_HEADERS
#ocomsdir = $(includedir)/ocoms/$(subdir)
#nobase_ocoms_HEADERS = $(headers)
#
## This is somewhat of a hack -- libltdl is installed by libtoolize,
## and us installing the header here is slightly less kludgey than
## hacking their Makefile.am.
#nobase_ocoms_HEADERS += libltdl/ltdl.h
#endif

include memoryhooks/Makefile.am
include threads/Makefile.am
include mca/base/Makefile.am
include primitives/Makefile.am
#include util/Makefile.am

