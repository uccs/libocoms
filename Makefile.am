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

SUBDIRS = config ocoms $(MCA_PROJECT_SUBDIRS)
EXTRA_DIST = README VERSION LICENSE autogen.pl

# include examples/Makefile.include

libservice_ladir = $(includedir)

nobase_dist_libservice_la_HEADERS = \
							ocoms/libocoms_version.h \
							ocoms/util/arch.h \
							ocoms/util/cmd_line.h \
							ocoms/util/fd.h \
							ocoms/util/keyval_parse.h \
							ocoms/util/output.h \
							ocoms/util/printf.h \
							ocoms/util/service_bitmap.h \
							ocoms/util/service_free_list.h \
							ocoms/util/service_hash_table.h \
							ocoms/util/service_object.h \
							ocoms/util/service_rb_tree.h \
							ocoms/util/argv.h \
							ocoms/util/crc.h \
							ocoms/util/if.h \
							ocoms/util/os_path.h \
							ocoms/util/path.h \
							ocoms/util/service_atomic_lifo.h \
							ocoms/util/service_environ.h \
							ocoms/util/service_graph.h \
							ocoms/util/service_list.h \
							ocoms/util/service_pointer_array.h \
							ocoms/util/service_value_array.h \
							ocoms/mca/mca.h \
							ocoms/mca/base/base.h \
							ocoms/mca/base/mca_base_component_repository.h \
							ocoms/mca/base/mca_base_param.h \
							ocoms/mca/base/mca_base_param_internal.h \
							ocoms/sys/architecture.h \
							ocoms/sys/atomic.h \
							ocoms/sys/atomic_impl.h \
							ocoms/sys/timer.h \
							ocoms/sys/alpha/atomic.h \
							ocoms/sys/amd64/atomic.h \
							ocoms/sys/amd64/timer.h \
							ocoms/sys/arm/atomic.h \
							ocoms/sys/arm/timer.h \
							ocoms/sys/ia32/atomic.h \
							ocoms/sys/ia32/timer.h \
							ocoms/sys/ia64/atomic.h \
							ocoms/sys/ia64/timer.h \
							ocoms/sys/mips/atomic.h \
							ocoms/sys/mips/timer.h \
							ocoms/sys/powerpc/atomic.h \
							ocoms/sys/powerpc/timer.h \
							ocoms/sys/sparc/atomic.h \
							ocoms/sys/sparcv9/atomic.h \
							ocoms/sys/sparcv9/timer.h \
							ocoms/sys/win32/atomic.h \
							ocoms/sys/win32/timer.h \
							ocoms/threads/condition.h \
							ocoms/threads/mutex.h \
							ocoms/threads/mutex_unix.h \
							ocoms/threads/mutex_windows.h \
							ocoms/threads/threads.h \
							ocoms/threads/tsd.h \
							ocoms/memoryhooks/memory.h \
							ocoms/memoryhooks/memory_internal.h \
							ocoms/platform/ocoms_config.h \
							ocoms/platform/service_config_top.h \
							ocoms/platform/service_portable_platform.h \
							ocoms/platform/service_config_bottom.h \
							ocoms/platform/service_constants.h \
							ocoms/platform/service_stdint.h \
							ocoms/primitives/align.h \
							ocoms/primitives/prefetch.h \
							ocoms/primitives/hash_string.h \
							ocoms/primitives/service_socket_errno.h \
							ocoms/primitives/types.h


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

#ver:
#	@echo $(VERSION)

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
