#
# Copyright (c) 2004-2009 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2006-2011 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2009-2011 Oak Ridge National Labs.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# mpif-common.h is not generated, but mpif.h and mpif-config.h are.
# See big comments in these files for an explanation.

# note - headers and nodist_headers will go in ${includedir}/openmpi,
# not ${includedir}/
headers =
nodist_headers = ccs_config.h

# Install these in $(includedir)
#include_HEADERS = mpi_portable_platform.h
include_HEADERS = 

# Always install these in $(pkgincludedir)
pkginclude_HEADERS =

noinst_LTLIBRARIES =

EXTRA_DIST = $(headers)

#if WANT_INSTALL_HEADERS 
#ccsdir = $(includedir)/ccs
#nobase_dist_ccs_HEADERS = $(headers)
#nobase_nodist_ccs_HEADERS = $(nodist_headers)
#endif

# Remove the auto-generated mpi-ext.h file
# Since there is no mpi-ext.h.in, autogen does not know to cleanup this file.
distclean-local:

