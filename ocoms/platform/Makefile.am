#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2009 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2010-2011 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

headers = \
	ocoms_config_top.h \
	ocoms_config_bottom.h \
	ocoms_stdint.h \
	ocoms_portable_platform.h \
	ocoms_constants.h

nodist_headers = ocoms_config.h

#include ocoms/Makefile.am

#EXTRA_DIST = CMakeLists.txt $(headers)

if WANT_INSTALL_HEADERS 
ocomsdir = $(includedir)/ocoms
nobase_dist_ocoms_HEADERS = $(headers)
nobase_nodist_ocoms_HEADERS = $(nodist_headers)
endif

