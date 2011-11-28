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
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

EXTRA_DIST = CMakeLists.txt

AM_CPPFLAGS =

SUBDIRS = \
	include \
    $(LIBLTDL_SUBDIR) \
	asm \
    datatype \
    util 

# libltdl is included by variable because if --disable-dlopen was
# used, there will be no generated Makefile in that directory (and
# therefore make distclean will fail).
DIST_SUBDIRS = \
	include \
	$(LIBLTDL_SUBDIR) \
	asm \
    datatype \
	util 

# Build the main OPAL library

lib_LTLIBRARIES = libservice.la
libservice_la_SOURCES =
libservice_la_LIBADD = \
        $(LIBLTDL) \
        asm/libasm.la \
        datatype/libdatatype.la \
        util/libserviceutil.la

libservice_la_DEPENDENCIES = $(libservice_la_LIBADD)
libservice_la_LDFLAGS =
#libservice_la_LDFLAGS = -version-info $(libservice_so_version)

# included subdirectory Makefile.am's and appended-to variables
headers =
noinst_LTLIBRARIES =
dist_pkgdata_DATA =
libservice_la_SOURCES += $(headers)

# Conditionally install the header files

#if WANT_INSTALL_HEADERS
#servicedir = $(includedir)/ccs/$(subdir)
#nobase_service_HEADERS = $(headers)
#
## This is somewhat of a hack -- libltdl is installed by libtoolize,
## and us installing the header here is slightly less kludgey than
## hacking their Makefile.am.
#nobase_service_HEADERS += libltdl/ltdl.h
#endif

include memoryhooks/Makefile.am
include threads/Makefile.am
include mca/base/Makefile.am
#include util/Makefile.am

