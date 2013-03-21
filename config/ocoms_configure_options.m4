dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
dnl                         University Research and Technology
dnl                         Corporation.  All rights reserved.
dnl Copyright (c) 2004-2005 The University of Tennessee and The University
dnl                         of Tennessee Research Foundation.  All rights
dnl                         reserved.
dnl Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
dnl                         University of Stuttgart.  All rights reserved.
dnl Copyright (c) 2004-2005 The Regents of the University of California.
dnl                         All rights reserved.
dnl Copyright (c) 2006-2010 Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
dnl Copyright (c) 2009      IBM Corporation.  All rights reserved.
dnl Copyright (c) 2009      Los Alamos National Security, LLC.  All rights
dnl                         reserved.
dnl Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
dnl Copyright (c) 2011      NVIDIA Corporation.  All rights reserved.
dnl
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

AC_DEFUN([OCOMS_CONFIGURE_OPTIONS],[
ocoms_show_subtitle "OCOMS Configuration options"


#
# Is this a developer copy?
#

if test -d .svn -o -d .hg; then
    OCOMS_DEVEL=1
else
    OCOMS_DEVEL=0
fi


#
# Code coverage options
#

AC_MSG_CHECKING([if want to run code coverage])
AC_ARG_ENABLE(coverage,
              AC_HELP_STRING([--enable-coverage],
                             [enable code coverage files to be generated]))
if test "$enable_coverage" = "yes"; then
    if test "$enable_shared" = "yes"; then 
        AC_MSG_WARN([Code coverage can run only with static libraries. Please 
run configure with --enable-static --disable-shared if 
you want code coverage. Also ensure that you execute 
make clean too ensure removal of all leftover shared 
mpi libraries])
        AC_MSG_ERROR([Cannot continue processing])
    fi
    AC_MSG_RESULT([yes])
    WANT_COVERAGE=1
else 
    AC_MSG_RESULT([no])
    WANT_COVERAGE=0
fi


#
# Branch Probabilities options
#

AC_MSG_CHECKING([if want to compile with branch probabilities])
AC_ARG_ENABLE(branch-probabilities,
              AC_HELP_STRING([--enable-branch-probabilities],
                             [enable profile arcs and branch probability optimization]))
if test "$enable_branch_probabilities" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_BRANCH_PROBABILITIES=1
else 
    AC_MSG_RESULT([no])
    WANT_BRANCH_PROBABILITIES=0
fi


#
# Memory debugging
#

AC_MSG_CHECKING([if want to debug memory usage])
AC_ARG_ENABLE(mem-debug, 
    AC_HELP_STRING([--enable-mem-debug],
                   [enable memory debugging (debugging only) (default: disabled)]))
if test "$enable_mem_debug" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_MEM_DEBUG=1
else
    AC_MSG_RESULT([no])
    WANT_MEM_DEBUG=0
fi
#################### Early development override ####################
if test "$WANT_MEM_DEBUG" = "0" -a -z "$enable_mem_debug" -a "$OCOMS_DEVEL" = 1; then
    WANT_MEM_DEBUG=1
    echo "--> developer override: enable mem profiling by default"
fi
#################### Early development override ####################
AC_DEFINE_UNQUOTED(OCOMS_ENABLE_MEM_DEBUG, $WANT_MEM_DEBUG,
    [Whether we want the memory profiling or not])

#
# Memory profiling
#

AC_MSG_CHECKING([if want to profile memory usage])
AC_ARG_ENABLE(mem-profile, 
    AC_HELP_STRING([--enable-mem-profile],
                   [enable memory profiling (debugging only) (default: disabled)]))
if test "$enable_mem_profile" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_MEM_PROFILE=1
else
    AC_MSG_RESULT([no])
    WANT_MEM_PROFILE=0
fi
#################### Early development override ####################
if test "$WANT_MEM_PROFILE" = "0" -a -z "$enable_mem_profile" -a "$OCOMS_DEVEL" = 1; then
    WANT_MEM_PROFILE=1
    echo "--> developer override: enable mem profiling by default"
fi
#################### Early development override ####################
AC_DEFINE_UNQUOTED(OCOMS_ENABLE_MEM_PROFILE, $WANT_MEM_PROFILE,
    [Whether we want the memory profiling or not])

#
# Developer picky compiler options
#

AC_MSG_CHECKING([if want developer-level compiler pickyness])
AC_ARG_ENABLE(picky, 
    AC_HELP_STRING([--enable-picky],
                   [enable developer-level compiler pickyness when building Open MPI (default: disabled)]))
if test "$enable_picky" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_PICKY_COMPILER=1
else
    AC_MSG_RESULT([no])
    WANT_PICKY_COMPILER=0
fi
#################### Early development override ####################
if test "$WANT_PICKY_COMPILER" = "0" -a -z "$enable_picky" -a "$OCOMS_DEVEL" = 1; then
    WANT_PICKY_COMPILER=1
    echo "--> developer override: enable picky compiler by default"
fi
#################### Early development override ####################

#
# Developer debugging
#

AC_MSG_CHECKING([if want developer-level debugging code])
AC_ARG_ENABLE(debug, 
    AC_HELP_STRING([--enable-debug],
                   [enable developer-level debugging code (not for general MPI users!) (default: disabled)]))
if test "$enable_debug" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_DEBUG=1
else
    AC_MSG_RESULT([no])
    WANT_DEBUG=0
fi
#################### Early development override ####################
if test "$WANT_DEBUG" = "0" -a -z "$enable_debug" -a "$OCOMS_DEVEL" = 1; then
    WANT_DEBUG=1
    echo "--> developer override: enable debugging code by default"
fi
#################### Early development override ####################
if test "$WANT_DEBUG" = "0"; then
    CFLAGS="-DNDEBUG $CFLAGS"
    CXXFLAGS="-DNDEBUG $CXXFLAGS"
fi
AC_DEFINE_UNQUOTED(OCOMS_ENABLE_DEBUG, $WANT_DEBUG,
    [Whether we want developer-level debugging code or not])

AC_ARG_ENABLE(debug-symbols,
    AC_HELP_STRING([--disable-debug-symbols],
        [Disable adding compiler flags to enable debugging symbols if --enable-debug is specified.  For non-debugging builds, this flag has no effect.]))

#
# Do we want to install all of OCOMS/ORTE and OMPI's header files?
#

AC_MSG_CHECKING([if want to install project-internal header files])
AC_ARG_WITH(devel-headers,
    AC_HELP_STRING([--with-devel-headers],
                   [normal MPI users/applications do not need this (mpi.h and mpif.h are ALWAYS installed).  Developer headers are only necessary for MCA module authors (default: disabled).]))
if test "$with_devel_headers" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_INSTALL_HEADERS=1
else
    AC_MSG_RESULT([no])
    WANT_INSTALL_HEADERS=0
fi
AM_CONDITIONAL(WANT_INSTALL_HEADERS, test "$WANT_INSTALL_HEADERS" = 1)


#
# Do we want the pretty-print stack trace feature?
#

AC_MSG_CHECKING([if want pretty-print stacktrace])
AC_ARG_ENABLE([pretty-print-stacktrace],
    [AC_HELP_STRING([--enable-pretty-print-stacktrace],
                    [Pretty print stacktrace on process signal (default: enabled)])])
if test "$enable_pretty_print_stacktrace" = "no" ; then
    AC_MSG_RESULT([no])
    WANT_PRETTY_PRINT_STACKTRACE=0
else
    AC_MSG_RESULT([yes])
    WANT_PRETTY_PRINT_STACKTRACE=1
fi
AC_DEFINE_UNQUOTED([OCOMS_WANT_PRETTY_PRINT_STACKTRACE],
                   [$WANT_PRETTY_PRINT_STACKTRACE],
                   [if want pretty-print stack trace feature])


#
# Do we want PTY support?
#

AC_MSG_CHECKING([if want pty support])
AC_ARG_ENABLE(pty-support,
    AC_HELP_STRING([--enable-pty-support],
                   [Enable/disable PTY support for STDIO forwarding.  (default: enabled)]))
if test "$enable_pty_support" = "no" ; then
    AC_MSG_RESULT([no])
    OCOMS_ENABLE_PTY_SUPPORT=0
else
    AC_MSG_RESULT([yes])
    OCOMS_ENABLE_PTY_SUPPORT=1
fi
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_PTY_SUPPORT], [$OCOMS_ENABLE_PTY_SUPPORT],
                   [Whether user wants PTY support or not])


#
# Do we want to disable weak symbols for some reason?
#

AC_MSG_CHECKING([if want weak symbol support])
AC_ARG_ENABLE(weak-symbols,
    AC_HELP_STRING([--enable-weak-symbols],
                   [use weak symbols, if available (default: enabled)]))
if test "$enable_weak_symbols" != "no"; then
    AC_MSG_RESULT([yes])
    WANT_WEAK_SYMBOLS=1
else
    AC_MSG_RESULT([no])
    WANT_WEAK_SYMBOLS=0
fi


#
# Do we want to allow DLOPEN?
#

AC_MSG_CHECKING([if want dlopen support])
AC_ARG_ENABLE([dlopen],
    [AC_HELP_STRING([--enable-dlopen],
                    [Whether build should attempt to use dlopen (or
                     similar) to dynamically load components.
                     Disabling dlopen implies --disable-mca-dso.
                     (default: enabled)])])
if test "$enable_dlopen" = "no" ; then
    enable_mca_dso="no"
    enable_mca_static="yes"
    OCOMS_ENABLE_DLOPEN_SUPPORT=0
    AC_MSG_RESULT([no])
else
    OCOMS_ENABLE_DLOPEN_SUPPORT=1
    AC_MSG_RESULT([yes])
fi


#
# Heterogeneous support
#

AC_MSG_CHECKING([if want heterogeneous support])
AC_ARG_ENABLE([heterogeneous],
    [AC_HELP_STRING([--enable-heterogeneous],
                    [Enable features required for heterogeneous
                     platform support (default: disabled)])])
if test "$enable_heterogeneous" = "yes" ; then
     AC_MSG_RESULT([yes])
     ocoms_want_heterogeneous=1
else
     AC_MSG_RESULT([no])
     ocoms_want_heterogeneous=0
fi
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_HETEROGENEOUS_SUPPORT], 
                   [$ocoms_want_heterogeneous], 
                   [Enable features required for heterogeneous support])


#
# Internal trace file logging (debugging)
#

AC_MSG_CHECKING([if want trace file debugging])
AC_ARG_ENABLE([trace],
    [AC_HELP_STRING([--enable-trace],
                    [Enable internal tracing of OMPI/ORTE/OCOMS calls -- used only for developer debugging, not tracing of MPI applications (default: disabled)])])
if test "$enable_trace" = "yes"; then
    AC_MSG_RESULT([yes])
    ocoms_want_trace=1
else
    AC_MSG_RESULT([no])
    ocoms_want_trace=0
fi
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_TRACE], [$ocoms_want_trace],
                   [Enable run-time tracing of internal functions])



#
# Cross-compile data
#
AC_ARG_WITH([cross],
    [AC_HELP_STRING([--with-cross=FILE],
        [Specify configure values that can not be determined in a cross-compilation environment.  See the Open MPI FAQ.])])
if test "$with_cross" = "yes" ; then
    AC_MSG_ERROR([--with-cross argument must include FILE option])
elif test "$with_cross" = "no" ; then
    AC_MSG_ERROR([--without-cross is not a valid argument])
elif test "$with_cross" != "" ; then
    if test ! -r $with_cross ; then
        AC_MSG_ERROR([could not find cross-compile data file $with_cross])
    fi

    # eval into environment
    OCOMS_LOG_MSG([Loading cross-compile file $with_cross, with contents below])
    OCOMS_LOG_FILE([$with_cross])
    . "$with_cross"
fi

#
# --with-ft=TYPE
#  TYPE:
#    - LAM (synonym for 'cr' currently)
#    - cr
#    - orcm
#  /* General FT sections */
#  #if OCOMS_ENABLE_FT == 0 /* FT Disabled globaly */
#  #if OCOMS_ENABLE_FT == 1 /* FT Enabled globaly */
#  /* CR Specific sections */
#  #if OCOMS_ENABLE_FT_CR == 0 /* FT Ckpt/Restart Disabled */
#  #if OCOMS_ENABLE_FT_CR == 1 /* FT Ckpt/Restart Enabled */
#
AC_MSG_CHECKING([if want fault tolerance])
AC_ARG_WITH(ft,
    [AC_HELP_STRING([--with-ft=TYPE],
            [Specify the type of fault tolerance to enable. Options: LAM (LAM/MPI-like), cr (Checkpoint/Restart), orcm (OpenRCM) (default: disabled)])],
        [ocoms_want_ft=1],
        [ocoms_want_ft=0])
if test "$with_ft" = "no" -o "$ocoms_want_ft" = "0"; then
    ocoms_want_ft=0
    ocoms_want_ft_cr=0
    AC_MSG_RESULT([Disabled fault tolerance])
else
    ocoms_want_ft=1
    ocoms_want_ft_cr=0
    ocoms_want_ft_type=none

    as_save_IFS=$IFS
    IFS=","
    for opt in $with_ft; do
        IFS=$as_save_IFS

        # Default value
        if test "$opt" = "" -o "$opt" = "yes"; then
            ocoms_want_ft_cr=1
        elif test "$opt" = "LAM"; then
            ocoms_want_ft_cr=1
        elif test "$opt" = "lam"; then
            ocoms_want_ft_cr=1
        elif test "$opt" = "CR"; then
            ocoms_want_ft_cr=1
        elif test "$opt" = "cr"; then
            ocoms_want_ft_cr=1
        elif test "$opt" = "orcm"; then
            ocoms_want_ft_orcm=1
        elif test "$opt" = "ORCM"; then
            ocoms_want_ft_orcm=1
        else
            AC_MSG_RESULT([Unrecognized FT TYPE: $opt])
            AC_MSG_ERROR([Cannot continue])
        fi
    done
    if test "$ocoms_want_ft_cr" = 1; then
        ocoms_want_ft_type="cr"
    elif test "$ocoms_want_ft_orcm" = 1; then
        ocoms_want_ft_type="orcm"
    fi

    AC_MSG_RESULT([Enabled $ocoms_want_ft_type (Specified $with_ft)])
    AC_MSG_WARN([**************************************************])
    AC_MSG_WARN([*** Fault Tolerance Integration into Open MPI is *])
    AC_MSG_WARN([*** a research quality implementation, and care  *])
    AC_MSG_WARN([*** should be used when choosing to enable it.   *])
    AC_MSG_WARN([**************************************************])
fi
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_FT], [$ocoms_want_ft],
                   [Enable fault tolerance general components and logic])
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_FT_CR], [$ocoms_want_ft_cr],
                   [Enable fault tolerance checkpoint/restart components and logic])
AM_CONDITIONAL(WANT_FT, test "$ocoms_want_ft" = "1")

#
# Do we want to install binaries?
#
AC_ARG_ENABLE([binaries],
    [AC_HELP_STRING([--enable-binaries],
        [Build and install binaries required for Open MPI, such as the wrapper compilers.   Useful for multi-lib installations.  (default: enabled)])])
AM_CONDITIONAL([OCOMS_INSTALL_BINARIES], [test "$enable_binaries" != "no"])

AC_ARG_ENABLE([script-wrapper-compilers],
  [AC_HELP_STRING([--enable-script-wrapper-compilers],
     [Use less featured script-based wrapper compilers instead of the standard C-based wrapper compilers.  This option ignores the --disable-binaries option and is mainly useful in cross-compile environments])])
  if test "$enable_script_wrapper_compilers" = "yes"; then
      WANT_SCRIPT_WRAPPER_COMPILERS=1
  else
      WANT_SCRIPT_WRAPPER_COMPILERS=0
  fi
AM_CONDITIONAL([OCOMS_WANT_SCRIPT_WRAPPER_COMPILERS], [test "$enable_script_wrapper_compilers" = "yes"])

#
# Support per-user config files?
#
AC_ARG_ENABLE([per-user-config-files],
   [AC_HELP_STRING([--enable-per-user-config-files],
      [Disable per-user configuration files, to save disk accesses during job start-up.  This is likely desirable for large jobs.  Note that this can also be acheived by environment variables at run-time.  (default: enabled)])])
if test "$enable_per_user_config_files" = "no" ; then
  result=0
else
  result=1
fi
AC_DEFINE_UNQUOTED([OCOMS_WANT_HOME_CONFIG_FILES], [$result],
     [Enable per-user config files])

#
# Do we want to enable IPv6 support?
#
AC_MSG_CHECKING([if want IPv6 support])
AC_ARG_ENABLE([ipv6],
    [AC_HELP_STRING([--enable-ipv6],
        [Enable IPv6 support, but only if the underlying system supports it (default: disabled)])])
if test "$enable_ipv6" = "yes"; then
    AC_MSG_RESULT([yes])
    ocoms_want_ipv6=1
else
    AC_MSG_RESULT([no])
    ocoms_want_ipv6=0
fi
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_IPV6], [$ocoms_want_ipv6],
                   [Enable IPv6 support, but only if the underlying system supports it])


#
# Package/brand string
#
AC_MSG_CHECKING([if want package/brand string])
AC_ARG_WITH([package-string],
     [AC_HELP_STRING([--with-package-string=STRING],
                     [Use a branding string throughout Open MPI])])
if test "$with_package_string" = "" -o "$with_package_string" = "no"; then
    with_package_string="Open MPI $OCOMS_CONFIGURE_USER@$OCOMS_CONFIGURE_HOST Distribution"
fi
AC_DEFINE_UNQUOTED([OCOMS_PACKAGE_STRING], ["$with_package_string"],
     [package/branding string for Open MPI])
AC_MSG_RESULT([$with_package_string])

#
# Ident string
#
AC_MSG_CHECKING([if want ident string])
AC_ARG_WITH([ident-string],
     [AC_HELP_STRING([--with-ident-string=STRING],
                     [Embed an ident string into Open MPI object files])])
if test "$with_ident_string" = "" -o "$with_ident_string" = "no"; then
    with_ident_string="%VERSION%"
fi
with_ident_string="`echo $with_ident_string | sed -e 's/%VERSION%/'$OCOMS_VERSION/`"
AC_DEFINE_UNQUOTED([OCOMS_IDENT_STRING], ["$with_ident_string"],
     [ident string for Open MPI])
AC_MSG_RESULT([$with_ident_string])


#
# Use alternative checksum algorithm
#
AC_MSG_CHECKING([if want to use an alternative checksum algo for messages])
AC_ARG_WITH([dst-checksum],
     [AC_HELP_STRING([--with-dst-checksum],
                     [Use an alternative checksum algorithm for messages])])
if test "$with_dst_checksum" = "yes"; then
    AC_MSG_RESULT([yes])
    CFLAGS="-DOCOMS_CSUM_DST $CFLAGS"
else
    AC_MSG_RESULT([no])
fi


#
# User level (mpi.h.in) visible maximum lengths of strings.
# These may be required in lower-level libraries to set up matching
# data-structures (e.g. OCOMS_MAX_OBJECT_NAME).
#
# Default values (as of OMPI-1.3), and some sane minimum and maximum values
#

# No lower and upper bound required or enforced
OCOMS_WITH_OPTION_MIN_MAX_VALUE(processor_name,  256,  16, 1024)

# Min length according to information passed in ompi/errhandler/errcode.c
OCOMS_WITH_OPTION_MIN_MAX_VALUE(error_string,    256,  64, 1024)

# Min length according to MPI-2.1, p. 236 (information passed in ompi/communicator/comm.c: min only 48)
OCOMS_WITH_OPTION_MIN_MAX_VALUE(object_name,      64,  64,  256)

# Min and Max length according to MPI-2.1, p. 287 is 32; longest key in ROMIO however 33
OCOMS_WITH_OPTION_MIN_MAX_VALUE(info_key,         36,  34,  255)

# No lower and upper bound required or enforced!
OCOMS_WITH_OPTION_MIN_MAX_VALUE(info_val,        256,  32, 1024)

# Min length according to _POSIX_HOST_NAME_MAX=255 (4*HOST_NAME_MAX)
OCOMS_WITH_OPTION_MIN_MAX_VALUE(port_name,      1024, 255, 2048)

# Min length accroding to MPI-2.1, p. 418
OCOMS_WITH_OPTION_MIN_MAX_VALUE(datarep_string,  128,  64,  256)

# How to build libltdl
AC_ARG_WITH([libltdl],
    [AC_HELP_STRING([--with-libltdl(=DIR)],
         [Where to find libltdl (this option is ignored if --disable-dlopen is used).  DIR can take one of three values: "internal", "external", or a valid directory name.  "internal" (or no DIR value) forces Open MPI to use its internal copy of libltdl.  "external" forces Open MPI to use an external installation of libltdl.  Supplying a valid directory name also forces Open MPI to use an external installation of libltdl, and adds DIR/include, DIR/lib, and DIR/lib64 to the search path for headers and libraries.])])

#
# Checkpoint/restart enabled debugging
#
AC_MSG_CHECKING([if want checkpoint/restart enabled debugging option])
AC_ARG_ENABLE([crdebug],
    [AC_HELP_STRING([--enable-crdebug],
            [enable checkpoint/restart debugging functionality (default: disabled)])])
 
if test "$ocoms_want_ft" = "0"; then
    ocoms_want_prd=0
    AC_MSG_RESULT([Disabled (fault tolerance disabled --without-ft)])
elif test "$enable_crdebug" = "yes"; then
    ocoms_want_prd=1
    AC_MSG_RESULT([Enabled])
else
    ocoms_want_prd=0
    AC_MSG_RESULT([Disabled])
fi
 
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_CRDEBUG], [$ocoms_want_prd],
    [Whether we want checkpoint/restart enabled debugging functionality or not])

#
# Check to see if user wants CUDA support in datatype and convertor code.
#
AC_ARG_WITH([cuda],
            [AC_HELP_STRING([--with-cuda(=DIR)],
            [Build cuda support, optionally adding DIR/include, DIR/lib, and DIR/lib64])])
AC_MSG_CHECKING([if --with-cuda is set])

# CUDA support is off by default.  User has to request it.
AS_IF([test "$with_cuda" = "no" -o "x$with_cuda" = "x"],
      [ocoms_check_cuda_happy="no"
       AC_MSG_RESULT([not set (--with-cuda=$with_cuda)])],
      [AS_IF([test "$with_cuda" = "yes"],
             [AS_IF([test "x`ls /usr/local/cuda/include/cuda.h 2> /dev/null`" = "x"],
                    [AC_MSG_RESULT([not found in standard location])
                     AC_MSG_WARN([Expected file /usr/local/cuda/include/cuda.h not found])
                     AC_MSG_ERROR([Cannot continue])],
                    [AC_MSG_RESULT([found])
                     ocoms_check_cuda_happy="yes"
                     with_cuda="/usr/local/cuda"])],
             [AS_IF([test ! -d "$with_cuda"],
                    [AC_MSG_RESULT([not found])
                     AC_MSG_WARN([Directory $with_cuda not found])
                     AC_MSG_ERROR([Cannot continue])],
                    [AS_IF([test "x`ls $with_cuda/include/cuda.h 2> /dev/null`" = "x"],
                           [AC_MSG_RESULT([not found])
                            AC_MSG_WARN([Expected file $with_cuda/include/cuda.h not found])
                            AC_MSG_ERROR([Cannot continue])],
                           [ocoms_check_cuda_happy="yes"
                            AC_MSG_RESULT([found ($with_cuda/include/cuda.h)])])])])])

# Check for optional libdir setting
AC_ARG_WITH([cuda-libdir],
            [AC_HELP_STRING([--with-cuda-libdir=DIR],
            [Search for cuda libraries in DIR])])
AC_MSG_CHECKING([if --with-cuda-libdir is set])

# Only check for the extra cuda libdir if we have passed the --with-cuda tests.
AS_IF([test "$ocoms_check_cuda_happy" = "yes"],
      [AS_IF([test "$with_cuda_libdir" != "yes" -a "$with_cuda_libdir" != "no" -a "x$with_cuda_libdir" != "x"],
             [AS_IF([test ! -d "$with_cuda_libdir"],
                    [AC_MSG_RESULT([not found])
                     AC_MSG_WARN([Directory $with_cuda_libdir not found])
                     AC_MSG_ERROR([Cannot continue])],
                    [AS_IF([test "x`ls $with_cuda_libdir/libcuda.* 2> /dev/null`" = "x"],
                           [AC_MSG_RESULT([not found])
                            AC_MSG_WARN([Expected file $with_cuda_libdir/libcuda.* not found])
                            AC_MSG_ERROR([Cannot continue])],
                           [AC_MSG_RESULT([ok - found directory ($with_cuda_libdir)])])])],
             [with_cuda_libdir=/usr/lib64
              AS_IF([test "x`ls $with_cuda_libdir/libcuda.* 2> /dev/null`" = "x"],
                    [AC_MSG_RESULT([not found])
                     AC_MSG_WARN([Expected file $with_cuda_libdir/libcuda.* not found])
                     AC_MSG_ERROR([Cannot continue])],
                    [AC_MSG_RESULT([ok - found directory ($with_cuda_libdir)])])])],
      [AC_MSG_RESULT([not applicable since --with-cuda is not set])])

AC_MSG_CHECKING([if have cuda support])
if test "$ocoms_check_cuda_happy" = "yes"; then
    AC_MSG_RESULT([yes (-I$with_cuda/include -L$with_cuda_libdir -lcuda)])
    CUDA_SUPPORT=1
    ocoms_datatype_CPPFLAGS="-I$with_cuda/include"
    ocoms_datatype_LIBS="-L$with_cuda_libdir -lcuda"
    AC_SUBST([ocoms_datatype_CPPFLAGS])
    AC_SUBST([ocoms_datatype_LIBS])
else
    AC_MSG_RESULT([no])
    CUDA_SUPPORT=0
fi

AM_CONDITIONAL([OCOMS_cuda_support], [test "x$CUDA_SUPPORT" = "x1"])
AC_DEFINE_UNQUOTED([OCOMS_CUDA_SUPPORT],$CUDA_SUPPORT,
                   [Whether we want cuda device pointer support])

])dnl
