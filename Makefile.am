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
# Copyright (c) 2006-2010 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

SUBDIRS = config service $(MCA_PROJECT_SUBDIRS)
EXTRA_DIST = README VERSION LICENSE autogen.pl

# include examples/Makefile.include

libservice_ladir = $(includedir)

nobase_dist_libservice_la_HEADERS = \
							service/libservice_version.h \
							service/util/arch.h \
							service/util/cmd_line.h \
							service/util/fd.h \
							service/util/keyval_parse.h \
							service/util/output.h \
							service/util/printf.h \
							service/util/service_bitmap.h \
							service/util/service_free_list.h \
							service/util/service_hash_table.h \
							service/util/service_object.h \
							service/util/service_rb_tree.h \
							service/util/argv.h \
							service/util/crc.h \
							service/util/if.h \
							service/util/os_path.h \
							service/util/path.h \
							service/util/service_atomic_lifo.h \
							service/util/service_environ.h \
							service/util/service_graph.h \
							service/util/service_list.h \
							service/util/service_pointer_array.h \
							service/util/service_value_array.h \
							service/mca/mca.h \
							service/mca/base/base.h \
							service/mca/base/mca_base_component_repository.h \
							service/mca/base/mca_base_param.h \
							service/mca/base/mca_base_param_internal.h \
							service/sys/architecture.h \
							service/sys/atomic.h \
							service/sys/atomic_impl.h \
							service/sys/timer.h \
							service/sys/alpha/atomic.h \
							service/sys/amd64/atomic.h \
							service/sys/amd64/timer.h \
							service/sys/arm/atomic.h \
							service/sys/arm/timer.h \
							service/sys/ia32/atomic.h \
							service/sys/ia32/timer.h \
							service/sys/ia64/atomic.h \
							service/sys/ia64/timer.h \
							service/sys/mips/atomic.h \
							service/sys/mips/timer.h \
							service/sys/powerpc/atomic.h \
							service/sys/powerpc/timer.h \
							service/sys/sparc/atomic.h \
							service/sys/sparcv9/atomic.h \
							service/sys/sparcv9/timer.h \
							service/sys/win32/atomic.h \
							service/sys/win32/timer.h \
							service/threads/condition.h \
							service/threads/mutex.h \
							service/threads/mutex_unix.h \
							service/threads/mutex_windows.h \
							service/threads/threads.h \
							service/threads/tsd.h \
							service/memoryhooks/memory.h \
							service/memoryhooks/memory_internal.h \
							service/platform/ccs_config.h \
							service/platform/service_config_top.h \
							service/platform/service_portable_platform.h \
							service/platform/service_config_bottom.h \
							service/platform/service_constants.h \
							service/platform/service_stdint.h \
							service/primitives/align.h \
							service/primitives/prefetch.h \
							service/primitives/hash_string.h \
							service/primitives/service_socket_errno.h \
							service/primitives/types.h

#install-data-hook:
##	sed -i 's@#include "ccs_config.h"@#include "sevice/include/ccs_config.h"@g' `grep -lR '#include "ccs_config.h"' $(libservice_ladir)`
#	rm -rf $(libservice_ladir)/sys
#	mv $(libservice_ladir)/service/sys $(libservice_ladir)
#	rm -rf $(libservice_ladir)/service

ACLOCAL_AMFLAGS = -I config


libservice-tarball:
	-make_dist_tarball
	if [ $$? -ne 0 ]; then \
		echo "*** Problems in make_dist_tarball"; \
		(exit 1); exit 1; \
	fi

libservice_rpms: libservice-tarball rpms-all

rpmspec = $(PACKAGE).spec
rpmmacros =\
		   --define='_rpmdir $(PWD)/rpm-dist'\
		   --define='_srcrpmdir $(PWD)/rpm-dist'\
		   --define='_sourcedir $(PWD)'\
		   --define='_specdir $(PWD)'\
		   --define='_builddir $(PWD)'

rpmopts = --nodeps --buildroot='$(PWD)/_rpm'

rpmcheck:
	@which rpmbuild &> /dev/null; \
		if [ $$? -ne 0 ]; then \
		echo "*** This make target requires an rpm-based linux distribution."; \
		(exit 1); exit 1; \
		fi
	-mkdir -p rpm-dist

srcrpm: rpmcheck $(rpmspec)
	rpmbuild -bs $(rpmmacros) $(rpmopts) $(rpmspec); \
	if [ $$? -ne 0 ]; then \
		echo "*** Problems in rpmbuild -bs"; \
		(exit 1); exit 1; \
	fi

rpms: rpmcheck $(rpmspec)
	rpmbuild -bb $(rpmmacros) $(rpmopts) $(rpmspec); \
	if [ $$? -ne 0 ]; then \
		echo "*** Problems in rpmbuild -bb"; \
		(exit 1); exit 1; \
	fi

ver:
	@echo $(VERSION)

rpms-all: rpms

#rpms-dist:
#	fixed_dest_dir="$$HOME/dailybuild/web/software/fca/v$(MAJOR_VERSION).$(MINOR_VERSION)/downloads";\
#	mkdir -p $$fixed_dest_dir;\
#	echo $(VERSION) > $$fixed_dest_dir/latest_snapshot.txt;\
#	for vm_binrpm_name in `find rpm-dist -name "*.$(build_cpu).rpm"`; do\
#		fixed_binrpm_name=`echo $$vm_binrpm_name | sed -e s,\.rpm\$$,-$(DISTRO)\.rpm,g`;\
#		fixed_binrpm_basename=`basename $$fixed_binrpm_name`;\
#		cp $$vm_binrpm_name $$fixed_dest_dir/$$fixed_binrpm_basename;\
#		if [[ "$$vm_binrpm_name" =~ "$(PACKAGE_NAME)-$(PACKAGE_VERSION)" ]]; then \
#			$(top_srcdir)/packages/support/rpm2tgz.sh $$fixed_dest_dir/$$fixed_binrpm_basename $(prefix) $$fixed_dest_dir;\
#		fi;\
#	done
