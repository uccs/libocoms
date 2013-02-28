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
# Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# This makefile.am does not stand on its own - it is included from service/Makefile.am

headers += \
	service/align.h \
	service/service_socket_errno.h \
	service/types.h \
	service/prefetch.h \
	service/hash_string.h

nodist_headers += \
	service/version.h

#include service/sys/Makefile.am

service/install_path.h: $(top_builddir)/config.status $(srcdir)/service/install_path.h.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@

service/version.h: $(top_builddir)/config.status $(srcdir)/service/version.h.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@
