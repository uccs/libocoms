# -*- shell-script -*-
#
# Copyright (c) 2004-2009 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2010 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2007 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2006-2011 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2006-2008 Sun Microsystems, Inc.  All rights reserved.
# Copyright (c) 2006-2011 Los Alamos National Security, LLC.  All rights
#                         reserved.
# Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#


############################################################################
# Initialization, version number, and other random setup/init stuff
############################################################################

# Load in everything found by autogen.pl
m4_include([config/autogen_found_items.m4])
# Load the version code.  Because this is also used directly as a
AC_LANG([C])

# Init autoconf

# We don't have the version number to put in here yet, and we can't
# call CCS_GET_VERSION (etc.) before AC_INIT.  So use the shell
# version.  project_name_* comes from config/project_list.m4, which
# was set during autogen.sh.

AC_INIT([project_name_long],
        [0.1],
        [], [project_name_short])
AC_PREREQ(2.60)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)

#
# Start it up
#

CCS_CONFIGURE_SETUP
ccs_show_title "Configuring project_name_long"

#
# Setup some things that must be done before AM-INIT-AUTOMAKE
#

ccs_show_subtitle "Startup tests"
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AC_DEFINE_UNQUOTED(CCS_ARCH, "$target", [CCS architecture string])
AS_IF([test "$host" != "$target"],
      [AC_MSG_WARN([Cross-compile detected])
       AC_MSG_WARN([Cross-compiling is only partially supported])
       AC_MSG_WARN([Proceed at your own risk!])])
# AC_USE_SYSTEM_EXTENSIONS alters CFLAGS (e.g., adds -g -O2)
CFLAGS_save=$CFLAGS
AC_USE_SYSTEM_EXTENSIONS
# AC_USE_SYSTEM_EXTENSIONS will modify CFLAGS if nothing was in there
# beforehand.  We don't want that.  So if there was nothing in
# CFLAGS, put nothing back in there.
AS_IF([test -z "$CFLAGS_save"], [CFLAGS=])
CFLAGS_save=

# Get our platform support file.  This has to be done very, very early
# because it twiddles random bits of autoconf
CCS_LOAD_PLATFORM

#
# Init automake
#
AM_INIT_AUTOMAKE([foreign dist-bzip2 subdir-objects no-define 1.11 tar-ustar])

# SILENT_RULES is new in AM 1.11, but we require 1.11 or higher via
# autogen.  Limited testing shows that calling SILENT_RULES directly
# works in more cases than adding "silent-rules" to INIT_AUTOMAKE
# (even though they're supposed to be identical).  Shrug.
#AM_SILENT_RULES([yes])
AM_SILENT_RULES([no])

# Set up project specific AM_CONDITIONALs
#AM_CONDITIONAL([PROJECT_CCS], m4_ifdef([project_ompi], [true], [false]))

ccs_show_subtitle "Checking versions"

CCS_SAVE_VERSION([CCS], [Common Communication Substrate], [$srcdir/VERSION],
                          [ccs/include/version.h])

#
# Hardwire all progress threads to be off
#
enable_progress_threads="no"
ORTE_ENABLE_PROGRESS_THREADS=0
AC_DEFINE_UNQUOTED([ORTE_ENABLE_PROGRESS_THREADS], [$ORTE_ENABLE_PROGRESS_THREADS],
                   [Hardcode the ORTE progress thread to be off])
CCS_ENABLE_PROGRESS_THREADS=0
AC_DEFINE_UNQUOTED([CCS_ENABLE_PROGRESS_THREADS], [$CCS_ENABLE_PROGRESS_THREADS],
                   [Hardcode the CCS progress thread to be off])


# List header files to generate

AM_CONFIG_HEADER([ccs/include/ccs_config.h])
#m4_ifdef([project_orte],
#         [AM_CONFIG_HEADER([orte/include/orte_config.h])])

# override/fixup the version numbers set by AC_INIT, since on
# developer builds, there's no good way to know what the version is
# before running configure :(.  We only use the base version number
# (ie, no svn r numbers) for the version set in AC_INIT.  This will
# always match reality because we add the VERSION file (the only way
# to change the major.minor.release{greek}) into the configure
# dependencies.  PACKAGE_VERSION the AC_DEFINE doesn't change once set
# the first time -- AC_INIT's input (so it doesn't have an r number in
# it).  PACKAGE_VERSION the AC_SUBST can be rewritten along the way,
# and we'd like it to have the r number in it so that it shows up in
# the tarball name, so it is set to the full version here.
PACKAGE_VERSION="$CCS_VERSION"
PACKAGE_STRING="${PACKAGE_NAME} ${PACKAGE_VERSION}"
VERSION="${PACKAGE_VERSION}"

ccs_show_subtitle "Initialization, setup"

CCS_TOP_BUILDDIR="`pwd`"
AC_SUBST(CCS_TOP_BUILDDIR)
cd "$srcdir"
CCS_TOP_SRCDIR="`pwd`"
AC_SUBST(CCS_TOP_SRCDIR)
cd "$CCS_TOP_BUILDDIR"

AC_MSG_NOTICE([builddir: $CCS_TOP_BUILDDIR])
AC_MSG_NOTICE([srcdir: $CCS_TOP_SRCDIR])
if test "$CCS_TOP_BUILDDIR" != "$CCS_TOP_SRCDIR"; then
    AC_MSG_NOTICE([Detected VPATH build])
fi

# Setup the top of the ccs/include/ccs_config.h file

AH_TOP([/* -*- c -*-
 *
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 * Function: - OS, CPU and compiler dependent configuration
 */

#ifndef CCS_CONFIG_H
#define CCS_CONFIG_H

/*#include "ccs_config_top.h" */

])

AH_BOTTOM([
#include "ccs_config_bottom.h"
#endif /* CCS_CONFIG_H */
])

# Other basic setup stuff (shared with components)

CCS_BASIC_SETUP

top_ccs_srcdir="$CCS_TOP_SRCDIR"
AC_SUBST(top_ccs_srcdir)
top_ccs_builddir="$CCS_TOP_BUILDDIR"
AC_SUBST(top_ccs_builddir)

############################################################################
# Configuration options
############################################################################

CCS_CONFIGURE_OPTIONS
#m4_ifdef([project_orte], [ORTE_CONFIGURE_OPTIONS])

if test "$enable_binaries" = "no" -a "$enable_dist" = "yes"; then
    AC_MSG_WARN([--disable-binaries is incompatible with --enable dist])
    AC_MSG_ERROR([Cannot continue])
fi

############################################################################
# Libtool: part one
# (before C compiler setup)
############################################################################

#
# Part one of libtool magic.  Enable static so that we have the --with
# tests done up here and can check for OS.  Save the values of
# $enable_static and $enable_shared before setting the defaults,
# because if the user specified --[en|dis]able-[static|shared] on the
# command line, they'll already be set.  In this way, we can tell if
# the user requested something or if the default was set here.
#

ccs_enable_shared="$enable_shared"
ccs_enable_static="$enable_static"
AM_ENABLE_SHARED
AM_DISABLE_STATIC

CCS_SETUP_WRAPPER_INIT
# There is no [need for] ORTE_SETUP_WRAPPER_INIT; this is not an
# accidental omission
m4_ifdef([project_ompi], [CCS_SETUP_WRAPPER_INIT])

##################################
# Check for known incompatibility
##################################

# Do *not* print a message that we're checking the OS because this
# test is *not* meant to be an all-inclusive "if it passes this test,
# then configure must succeed" test.  This test is *only* mean to
# screen out the versions of OS X where we know CCS will cause kernel
# panics because of bad implementations of pty's.  See
# https://svn.open-mpi.org/trac/ompi/ticket/1637 for details.

# We do not support OS X before version 10.4 (Tiger)
case $host_os in
# Corresponds to OS X 10.0 - 10.3 (additional [] quoting for m4)
darwin[[4567]]*)
    AC_MSG_WARN([Open MPI does not support OS X prior to version 10.4 (Tiger)])
    AC_MSG_ERROR([Cannot continue])
esac

############################################################################
# Check for compilers and preprocessors
############################################################################
ccs_show_title "Compiler and preprocessor tests"

##################################
# C compiler characteristics
##################################

CCS_SETUP_CC

# Does the compiler support "ident"-like constructs?

CCS_CHECK_IDENT([CC], [CFLAGS], [c], [C])

#
# Check for some types
#

AC_CHECK_TYPES(int8_t)
AC_CHECK_TYPES(uint8_t)
AC_CHECK_TYPES(int16_t)
AC_CHECK_TYPES(uint16_t)
AC_CHECK_TYPES(int32_t)
AC_CHECK_TYPES(uint32_t)
AC_CHECK_TYPES(int64_t)
AC_CHECK_TYPES(uint64_t)
AC_CHECK_TYPES(int128_t)
AC_CHECK_TYPES(uint128_t)
AC_CHECK_TYPES(long long)

AC_CHECK_TYPES(long double)
# We only need these types if we're building the CCS project, but
# CCS currently doesn't protect for their lack of presence well.
AC_CHECK_TYPES(float _Complex)
AC_CHECK_TYPES(double _Complex)
AC_CHECK_TYPES(long double _Complex)

AC_CHECK_TYPES(intptr_t)
AC_CHECK_TYPES(uintptr_t)
AC_CHECK_TYPES(mode_t)
AC_CHECK_TYPES(ssize_t)
AC_CHECK_TYPES(ptrdiff_t)

#
# Check for type sizes
#

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
if test $ac_cv_type_long_long = yes; then
    AC_CHECK_SIZEOF(long long)
fi
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
if test $ac_cv_type_long_double = yes; then
    AC_CHECK_SIZEOF(long double)
fi

# We only need these types if we're building the CCS project, but
# CCS currently doesn't protect for their lack of presence well.
if test "$ac_cv_type_float__Complex" = yes; then
    AC_CHECK_SIZEOF(float _Complex)
fi
if test "$ac_cv_type_double__Complex" = yes; then
    AC_CHECK_SIZEOF(double _Complex)
fi
if test "$ac_cv_type_long_double__Complex" = yes; then
    AC_CHECK_SIZEOF(long double _Complex)
fi

AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(size_t)
if test $ac_cv_type_ssize_t = yes ; then
    AC_CHECK_SIZEOF(ssize_t)
fi
if test $ac_cv_type_ptrdiff_t = yes; then
    AC_CHECK_SIZEOF(ptrdiff_t)
fi
AC_CHECK_SIZEOF(wchar_t)


#
# Check for type alignments
#

CCS_C_GET_ALIGNMENT(_Bool, CCS_ALIGNMENT_BOOL)
CCS_C_GET_ALIGNMENT(int8_t, CCS_ALIGNMENT_INT8)
CCS_C_GET_ALIGNMENT(int16_t, CCS_ALIGNMENT_INT16)
CCS_C_GET_ALIGNMENT(int32_t, CCS_ALIGNMENT_INT32)
CCS_C_GET_ALIGNMENT(int64_t, CCS_ALIGNMENT_INT64)
if test "$ac_cv_type_int128_t" = yes ; then
  CCS_C_GET_ALIGNMENT(int128_t, CCS_ALIGNMENT_INT128)
fi
CCS_C_GET_ALIGNMENT(char, CCS_ALIGNMENT_CHAR)
CCS_C_GET_ALIGNMENT(short, CCS_ALIGNMENT_SHORT)
CCS_C_GET_ALIGNMENT(wchar_t, CCS_ALIGNMENT_WCHAR)
CCS_C_GET_ALIGNMENT(int, CCS_ALIGNMENT_INT)
CCS_C_GET_ALIGNMENT(long, CCS_ALIGNMENT_LONG)
if test "$ac_cv_type_long_long" = yes; then
    CCS_C_GET_ALIGNMENT(long long, CCS_ALIGNMENT_LONG_LONG)
fi
CCS_C_GET_ALIGNMENT(float, CCS_ALIGNMENT_FLOAT)
CCS_C_GET_ALIGNMENT(double, CCS_ALIGNMENT_DOUBLE)
if test "$ac_cv_type_long_double" = yes; then
    CCS_C_GET_ALIGNMENT(long double, CCS_ALIGNMENT_LONG_DOUBLE)
fi

# We only need these types if we're building the CCS project, but
# CCS currently doesn't protect for their lack of presence well.
if test "$ac_cv_type_float__Complex" = yes; then
    CCS_C_GET_ALIGNMENT(float _Complex, CCS_ALIGNMENT_FLOAT_COMPLEX)
fi
if test "$ac_cv_type_double__Complex" = yes; then
    CCS_C_GET_ALIGNMENT(double _Complex, CCS_ALIGNMENT_DOUBLE_COMPLEX)
fi
if test "$ac_cv_type_long_double__Complex" = yes; then
    CCS_C_GET_ALIGNMENT(long double _Complex, CCS_ALIGNMENT_LONG_DOUBLE_COMPLEX)
fi

CCS_C_GET_ALIGNMENT(void *, CCS_ALIGNMENT_VOID_P)

#
# Does the C compiler native support "bool"? (i.e., without
# <stdbool.h> or any other help)
#

AC_MSG_CHECKING(for C bool type)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT],
[[bool bar, foo = true; bar = foo;]])],
[CCS_NEED_C_BOOL=0 MSG=yes],[CCS_NEED_C_BOOL=1 MSG=no])
AC_DEFINE_UNQUOTED(CCS_NEED_C_BOOL, $CCS_NEED_C_BOOL,
    [Whether the C compiler supports "bool" without any other help (such as <stdbool.h>)])
AC_MSG_RESULT([$MSG])
AC_CHECK_SIZEOF(_Bool)

#
# Check for other compiler characteristics
#

if test "$GCC" = "yes"; then

    # gcc 2.96 will emit oodles of warnings if you use "inline" with
    # -pedantic (which we do in developer builds).  However,
    # "__inline__" is ok.  So we have to force gcc to select the
    # right one.  If you use -pedantic, the AC_C_INLINE test will fail
    # (because it names a function foo() -- without the (void)).  So
    # we turn off all the picky flags, turn on -ansi mode (which is
    # implied by -pedantic), and set warnings to be errors.  Hence,
    # this does the following (for 2.96):
    #
    # - causes the check for "inline" to emit a warning, which then
    # fails
    # - checks for __inline__, which then emits no error, and works
    #
    # This also works nicely for gcc 3.x because "inline" will work on
    # the first check, and all is fine.  :-)

    CFLAGS_save="$CFLAGS"
    CFLAGS="$CCS_CFLAGS_BEFORE_PICKY -Werror -ansi"
fi
AC_C_INLINE
# Microsoft compilers support 2 versions of restrict. One for functions, and
# one for variables. The problem is that they don't have an equivalent
# syntax, and the autoconf restrict detection is unable to detect them
# correctly. It detect the restrict keyword as __restrict which break the
# rules for function syntax which is declspec(restrict).
if test "x$ccs_cv_c_compiler_vendor" != "xmicrosoft"; then
    AC_C_RESTRICT
fi
CCS_C_WEAK_SYMBOLS
if test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS_save"
fi

if test "x$CC" = "xicc"; then
    CCS_CHECK_ICC_VARARGS
fi

# If we want the profiling layer:
# - If the C compiler has weak symbols, use those.
# - If not, then set to compile the code again with #define's in a
#   separate directory.

if test "$WANT_WEAK_SYMBOLS" = "0"; then
    CCS_C_HAVE_WEAK_SYMBOLS=0
fi
if test "$WANT_MPI_PROFILING" = "1"; then
    if test "$CCS_C_HAVE_WEAK_SYMBOLS" = "1"; then
        CCS_PROFILING_COMPILE_SEPARATELY=0
    else
        CCS_PROFILING_COMPILE_SEPARATELY=1
    fi
else
    CCS_PROFILING_COMPILE_SEPARATELY=0
fi

# Check if we support the offsetof compiler directive

CCS_CHECK_OFFSETOF


##################################
# C++ compiler characteristics
##################################

# We don't need C++ unless we're building Open MPI; ORTE and CCS do
# not use C++ at all.  The CCS macro name appears to be a bit of a
# misnomer; I'm not sure why it was split into a second macro and put
# into CCS...?  All it does is setup the C++ compiler (the CCS macro
# sets up the C++ MPI bindings, etc.).  Perhaps it was moved to CCS
# just on the rationale that all compiler setup should be done in
# CCS...?  Shrug.
m4_ifdef([project_ccs], [CCS_SETUP_CXX
                          CCS_SETUP_CXX])
#m4_ifdef([project_ompi], [CCS_SETUP_CXX
#                          CCS_SETUP_CXX])

##################################
# Only after setting up both
# C and C++ check compiler attributes.
##################################

ccs_show_subtitle "Compiler characteristics"

CCS_CHECK_ATTRIBUTES
CCS_CHECK_COMPILER_VERSION_ID


##################################
# Assembler Configuration
##################################

ccs_show_subtitle "Assembler"

AM_PROG_AS
CCS_CONFIG_ASM


##################################
# Fortran
##################################

# CCS uses ccs_fortran_logical_t and CCS_FORTRAN_HANDLE_MAX (long
# story; yes, it's an intentional abstraction break).  So ensure to
# define it to a valid C type if we're not building the CCS project.
m4_ifdef([project_ompi], [CCS_SETUP_MPI_FORTRAN],
         [AC_DEFINE([ccs_fortran_logical_t], [int], [Bogus type for CCS])
          AC_DEFINE([CCS_FORTRAN_HANDLE_MAX], [128], [Bogus type for CCS])
])

# checkpoint results
AC_CACHE_SAVE


##################################
# Header files
##################################

ccs_show_title "Header file tests"

AC_CHECK_HEADERS([alloca.h aio.h arpa/inet.h dirent.h \
    dlfcn.h execinfo.h err.h fcntl.h grp.h inttypes.h libgen.h \
    libutil.h memory.h netdb.h netinet/in.h netinet/tcp.h \
    poll.h pthread.h pty.h pwd.h sched.h stdint.h stddef.h \
    stdlib.h string.h strings.h stropts.h sys/fcntl.h sys/ipc.h sys/shm.h \
    sys/ioctl.h sys/mman.h sys/mount.h sys/param.h sys/queue.h \
    sys/resource.h sys/select.h sys/socket.h sys/sockio.h \
    stdarg.h sys/stat.h sys/statfs.h sys/statvfs.h sys/time.h sys/tree.h \
    sys/types.h sys/uio.h net/uio.h sys/utsname.h sys/vfs.h sys/wait.h syslog.h \
    time.h termios.h ulimit.h unistd.h util.h utmp.h malloc.h \
    ifaddrs.h sys/sysctl.h crt_externs.h regex.h signal.h \
    ioLib.h sockLib.h hostLib.h shlwapi.h sys/synch.h limits.h db.h ndbm.h])

# Needed to work around Darwin requiring sys/socket.h for
# net/if.h
AC_CHECK_HEADERS([net/if.h], [], [],
    [#include <stdio.h>
#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
])

# Note that sometimes we have <stdbool.h>, but it doesn't work (e.g.,
# have both Portland and GNU installed; using pgcc will find GNU's
# <stdbool.h>, which all it does -- by standard -- is define "bool" to
# "_Bool" [see
# http://www.opengroup.org/onlinepubs/009695399/basedefs/stdbool.h.html],
# and Portland has no idea what to do with _Bool).

# So first figure out if we have <stdbool.h> (i.e., check the value of
# the macro HAVE_STDBOOL_H from the result of AC_CHECK_HEADERS,
# above).  If we do have it, then check to see if it actually works.
# Define CCS_USE_STDBOOL_H as approrpaite.
AC_CHECK_HEADERS([stdbool.h], [have_stdbool_h=1], [have_stdbool_h=0])
AC_MSG_CHECKING([if <stdbool.h> works])
if test "$have_stdbool_h" = "1"; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT[
#if HAVE_STDBOOL_H
#include <stdbool.h>
#endif]],
[[bool bar, foo = true; bar = foo;]])],
[CCS_USE_STDBOOL_H=1 MSG=yes],[CCS_USE_STDBOOL_H=0 MSG=no])
else
    CCS_USE_STDBOOL_H=0
    MSG="no (don't have <stdbool.h>)"
fi
AC_DEFINE_UNQUOTED(CCS_USE_STDBOOL_H, $CCS_USE_STDBOOL_H,
    [Whether to use <stdbool.h> or not])
AC_MSG_RESULT([$MSG])

# checkpoint results
AC_CACHE_SAVE


##################################
# Types
##################################

ccs_show_title "Type tests"

# Size of pid_t
AC_CHECK_SIZEOF(pid_t)

AC_CHECK_TYPES([socklen_t, struct sockaddr_in, struct sockaddr_in6,
                struct sockaddr_storage],
               [], [], [AC_INCLUDES_DEFAULT
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif])

AC_CHECK_DECLS([AF_UNSPEC, PF_UNSPEC, AF_INET6, PF_INET6],
               [], [], [AC_INCLUDES_DEFAULT
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif])

# SA_RESTART in signal.h
AC_MSG_CHECKING([if SA_RESTART defined in signal.h])
AC_EGREP_CPP(yes, [
#include <signal.h>
#ifdef SA_RESTART
    yes
#endif ], [MSG=yes VALUE=1], [MSG=no VALUE=0])
AC_DEFINE_UNQUOTED(CCS_HAVE_SA_RESTART, $VALUE,
    [Whether we have SA_RESTART in <signal.h> or not])
AC_MSG_RESULT([$MSG])

AC_CHECK_MEMBERS([struct sockaddr.sa_len], [], [], [
#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif])

AC_CHECK_MEMBERS([struct dirent.d_type], [], [], [
#include <sys/types.h>
#include <dirent.h>])

AC_CHECK_MEMBERS([siginfo_t.si_fd],,,[#include <signal.h>])
AC_CHECK_MEMBERS([siginfo_t.si_band],,,[#include <signal.h>])


# checkpoint results
AC_CACHE_SAVE


##################################
# Libraries
##################################

ccs_show_title "Library and Function tests"

# Darwin doesn't need -lutil, as it's something other than this -lutil.
CCS_CHECK_FUNC_LIB([openpty], [util])

AC_CHECK_LIB([nsl], [gethostbyname])

AC_CHECK_LIB([socket], [socket])

# Solaris has sched_yield in -lrt, usually in libc
CCS_CHECK_FUNC_LIB([sched_yield], [rt])

# IRIX has dirname in -lgen, usually in libc
CCS_CHECK_FUNC_LIB([dirname], [gen])

# Darwin doesn't need -lm, as it's a symlink to libSystem.dylib
CCS_CHECK_FUNC_LIB([ceil], [m])

AC_CHECK_FUNCS([asprintf snprintf vasprintf vsnprintf openpty isatty getpwuid fork waitpid execve pipe ptsname setsid mmap tcgetpgrp posix_memalign strsignal sysconf syslog vsyslog regcmp regexec regfree _NSGetEnviron socketpair strncpy_s _strdup usleep mkfifo dbopen dbm_open])

# On some hosts, htonl is a define, so the AC_CHECK_FUNC will get
# confused.  On others, it's in the standard library, but stubbed with
# the magic glibc foo as not implemented.  and on other systems, it's
# just not there.  This covers all cases.
AC_CACHE_CHECK([for htonl define],
  [ccs_cv_htonl_define],
  [AC_PREPROC_IFELSE([AC_LANG_PROGRAM([
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif],[
#ifndef ntohl
#error "ntohl not defined"
#endif
])], [ccs_cv_htonl_define=yes], [ccs_cv_htonl_define=no])])
AC_CHECK_FUNC([htonl], [ccs_have_htonl=yes], [ccs_have_htonl=no])
AS_IF([test "$ccs_cv_htonl_define" = "yes" -o "$ccs_have_htonl" = "yes"],
    [AC_DEFINE_UNQUOTED([HAVE_UNIX_BYTESWAP], [1],
        [whether unix byteswap routines -- htonl, htons, nothl, ntohs -- are available])])

#
# Make sure we can copy va_lists (need check declared, not linkable)
#

AC_CHECK_DECL(va_copy, CCS_HAVE_VA_COPY=1, CCS_HAVE_VA_COPY=0,
    [#include <stdarg.h>])
AC_DEFINE_UNQUOTED(CCS_HAVE_VA_COPY, $CCS_HAVE_VA_COPY,
    [Whether we have va_copy or not])

AC_CHECK_DECL(__va_copy, CCS_HAVE_UNDERSCORE_VA_COPY=1,
    CCS_HAVE_UNDERSCORE_VA_COPY=0, [#include <stdarg.h>])
AC_DEFINE_UNQUOTED(CCS_HAVE_UNDERSCORE_VA_COPY, $CCS_HAVE_UNDERSCORE_VA_COPY,
    [Whether we have __va_copy or not])

AC_CHECK_DECLS(__func__)

# checkpoint results
AC_CACHE_SAVE


##################################
# System-specific tests
##################################

ccs_show_title "System-specific tests"

#
# Test to determine type of MPI_Offset. This is searched in the following order
# int64_t, long long, long, int. If none of these are 8 bytes, then we should
# search for int32_t, long long, long, int.
#
MPI_OFFSET_TYPE="not found"
MPI_OFFSET_DATATYPE="not found"
AC_MSG_CHECKING([checking for type of MPI_Offset])
if test $ac_cv_type_long_long = yes -a $ac_cv_sizeof_long_long = 8; then
    MPI_OFFSET_TYPE="long long"
    MPI_OFFSET_DATATYPE=MPI_LONG_LONG
    MPI_OFFSET_SIZE=8
elif test $ac_cv_type_long = yes -a $ac_cv_sizeof_long = 8; then
    MPI_OFFSET_TYPE="long"
    MPI_OFFSET_DATATYPE=MPI_LONG
    MPI_OFFSET_SIZE=8
elif test $ac_cv_sizeof_int = 8; then
    MPI_OFFSET_TYPE="int"
    MPI_OFFSET_DATATYPE=MPI_INT
    MPI_OFFSET_SIZE=8
elif test $ac_cv_type_long_long = yes -a $ac_cv_sizeof_long_long = 4; then
    MPI_OFFSET_TYPE="long long"
    MPI_OFFSET_DATATYPE=MPI_LONG_LONG
    MPI_OFFSET_SIZE=4
elif test $ac_cv_type_long = yes -a $ac_cv_sizeof_long = 4; then
    MPI_OFFSET_TYPE="long"
    MPI_OFFSET_DATATYPE=MPI_LONG
    MPI_OFFSET_SIZE=4
elif test $ac_cv_sizeof_int = 4; then
    MPI_OFFSET_TYPE="int"
    MPI_OFFSET_DATATYPE=MPI_INT
    MPI_OFFSET_SIZE=4
fi
AC_MSG_RESULT([$MPI_OFFSET_TYPE])
if test "$MPI_OFFSET_TYPE" = "not found"; then
    AC_MSG_WARN([*** Unable to find the right definition for MPI_Offset])
    AC_MSG_ERROR([Cannot continue])
fi
AC_MSG_CHECKING([checking for an MPI datatype for MPI_Offset])
AC_MSG_RESULT([$MPI_OFFSET_DATATYPE])
# this should never happens
if test "$MPI_OFFSET_DATATYPE" = "not found"; then
    AC_MSG_WARN([*** Unable to find an MPI datatype corresponding to MPI_Offset])
    AC_MSG_ERROR([Cannot continue])
fi

AC_DEFINE_UNQUOTED(CCS_MPI_OFFSET_TYPE, $MPI_OFFSET_TYPE, [Type of MPI_Offset -- has to be defined here and typedef'ed later because mpi.h does not get AC SUBST's])
AC_DEFINE_UNQUOTED(CCS_MPI_OFFSET_SIZE, $MPI_OFFSET_SIZE, [Size of the MPI_Offset])
AC_DEFINE_UNQUOTED(CCS_OFFSET_DATATYPE, $MPI_OFFSET_DATATYPE, [MPI datatype corresponding to MPI_Offset])

#
# Check for MPI_Aint type.  Yes, there are platforms where
# sizeof(void*) != sizeof(long) (64 bit Windows, apparently).
#
if test $ac_cv_type_ptrdiff_t = yes ; then
    ccs_ptrdiff_t="ptrdiff_t"
elif test $ac_cv_sizeof_void_p -eq $ac_cv_sizeof_long ; then
    ccs_ptrdiff_t="long"
elif test $ac_cv_type_long_long = yes -a $ac_cv_sizeof_void_p -eq $ac_cv_sizeof_long_long ; then
    ccs_ptrdiff_t="long long"
else
    AC_MSG_ERROR([Could not find datatype to emulate ptrdiff_t.  Cannot continue])
fi
AC_DEFINE_UNQUOTED([CCS_PTRDIFF_TYPE], [$ccs_ptrdiff_t],
    [type to use for ptrdiff_t])

# Do we have _SC_NPROCESSORS_ONLN? (only going to pass if we also have
# <unistd.h> and sysconf(), which is ok) OS X 10.4 has <unistd.h> and
# sysconf(), but does not have _SC_NPROCESSORS_ONLN.  Doh!

AC_CACHE_CHECK([for _SC_NPROCESSORS_ONLN],
  [ccs_cv_have__SC_NPROCESSORS_ONLN],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT
#include <unistd.h>
],
     [int i = _SC_NPROCESSORS_ONLN;])],
     [ccs_cv_have__SC_NPROCESSORS_ONLN="yes"],
     [ccs_cv_have__SC_NPROCESSORS_ONLN="no"])])
AS_IF([test "$ccs_cv_have__SC_NPROCESSORS_ONLN" = "yes"],
      [result=1], [result=0])
AC_DEFINE_UNQUOTED([CCS_HAVE__SC_NPROCESSORS_ONLN], [$result],
  [Define to 1 ifyou have the declaration of _SC_NPROCESSORS_ONLN, and to 0 otherwise])

# all: endian

AC_C_BIGENDIAN

CCS_CHECK_BROKEN_QSORT

AC_CACHE_CHECK([if word-sized integers must be word-size aligned],
    [ccs_cv_c_word_size_align],
    [AC_LANG_PUSH(C)
     AC_RUN_IFELSE([AC_LANG_PROGRAM([dnl
#include <stdlib.h>], [[    long data[2] = {0, 0};
    long *lp;
    int *ip;
    ip = (int*) data;
    ip++;
    lp = (long*) ip;
    return lp[0]; ]])],
        [ccs_cv_c_word_size_align=no],
        [ccs_cv_c_word_size_align=yes],
        [ccs_cv_c_word_size_align=yes])])
AS_IF([test $ccs_cv_c_word_size_align = yes], [results=1], [results=0])
AC_DEFINE_UNQUOTED([CCS_ALIGN_WORD_SIZE_INTEGERS], [$results],
    [set to 1 if word-size integers must be aligned to word-size padding to prevent bus errors])

# all: SYSV semaphores
# all: SYSV shared memory
# all: size of FD_SET
# all: sizeof struct stat members
# all: type of getsockopt optlen
# all: type of recvfrom optlen

#
# Check out what thread support we have
#
CCS_CONFIG_THREADS
m4_ifdef([project_ompi], [CCS_CONFIG_THREADS])

CFLAGS="$CFLAGS $THREAD_CFLAGS"
CPPFLAGS="$CPPFLAGS $THREAD_CPPFLAGS"
m4_ifdef([project_ompi],
         [CXXFLAGS="$CXXFLAGS $THREAD_CXXFLAGS"
          CXXCPPFLAGS="$CXXCPPFLAGS $THREAD_CXXCPPFLAGS"])
LDFLAGS="$LDFLAGS $THREAD_LDFLAGS"
LIBS="$LIBS $THREAD_LIBS"

WRAPPER_EXTRA_CFLAGS="$WRAPPER_EXTRA_CFLAGS $THREAD_CFLAGS"
m4_ifdef([project_ompi],
         [WRAPPER_EXTRA_CXXFLAGS="$WRAPPER_EXTRA_CXXFLAGS $THREAD_CXXFLAGS"
          WRAPPER_EXTRA_FFLAGS="$WRAPPER_EXTRA_FFLAGS $THREAD_FFLAGS"
          WRAPPER_EXTRA_FCFLAGS="$WRAPPER_EXTRA_FCFLAGS $THREAD_FFLAGS"])
WRAPPER_EXTRA_LDFLAGS="$WRAPPER_EXTRA_LDFLAGS $THREAD_LDFLAGS"
# no need to update WRAPPER_EXTRA_LIBS - we'll get it from LT later

#
# What is the local equivalent of "ln -s"
#

AC_PROG_LN_S

AC_PROG_GREP
AC_PROG_EGREP

#
# We need as and lex
#
AM_PROG_AS
AM_PROG_LEX

# If we don't have GNU Flex and we don't have a generated .c file
# (distribution tarballs will have the .c file included, but SVN
# checkouts will not), then error.  Must have GNU Flex -- other
# versions of Lex are not workable (all things being equal, since this
# is *only* required for developers, we decided that it really was not
# worth it to be portable between different versions of lex ;-).

if test -z "$LEX" -o -n "`echo $LEX | $GREP missing`" -o \
    "`basename $LEX`" != "flex"; then
    if test ! -f "$srcdir/service/util/show_help_lex.c"; then
        AC_MSG_WARN([*** Could not find GNU Flex on your system.])
        AC_MSG_WARN([*** GNU Flex required for developer builds of Open MPI.])
        AC_MSG_WARN([*** Other versions of Lex are not supported.])
        AC_MSG_WARN([*** YOU DO NOT NEED FLEX FOR DISTRIBUTION TARBALLS!])
        AC_MSG_WARN([*** If you absolutely cannot install GNU Flex on this system])
        AC_MSG_WARN([*** consider using a distribution tarball, or generate the])
        AC_MSG_WARN([*** following files on another system (using Flex) and])
        AC_MSG_WARN([*** copy them here:])
        for lfile in `find . -name \*.l -print`; do
            cfile="`echo $lfile | cut -d. -f-2`"
            AC_MSG_WARN([***     $cfile.c])
        done
        AC_MSG_ERROR([Cannot continue])
    fi
fi

#
# File system case sensitivity
#

m4_ifdef([project_ccs], [CCS_CASE_SENSITIVE_FS_SETUP])

# AIX: FIONBIO in sys/ioctl.h
# glibc: memcpy

#
# Do we have RLIMIT_NPROC in <sys/resources.h>? (e.g., Solaris does not)
#

AC_CHECK_DECLS([RLIMIT_NPROC], [], [], [
AC_INCLUDES_DEFAULT
#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif])

#
# Do we have RLIMIT_MEMLOCK in <sys/resources.h>? (e.g., Solaris does not)
#

AC_CHECK_DECLS([RLIMIT_MEMLOCK], [], [], [
AC_INCLUDES_DEFAULT
#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif])

# checkpoint results
AC_CACHE_SAVE


##################################
# MCA
##################################

ccs_show_title "Modular Component Architecture (MCA) setup"

AC_MSG_CHECKING([for subdir args])
CCS_CONFIG_SUBDIR_ARGS([ccs_subdir_args])
AC_MSG_RESULT([$ccs_subdir_args])

CCS_MCA

# checkpoint results
AC_CACHE_SAVE

##################################
# MPI Extended Interfaces
##################################

m4_ifdef([project_ompi], [CCS_SETUP_MPI_EXT])

# checkpoint results
AC_CACHE_SAVE

##################################
# Contributed software
##################################

m4_ifdef([project_ompi], [CCS_SETUP_CONTRIB])

# checkpoint results
AC_CACHE_SAVE

##################################
# Visibility
##################################

# Check the visibility declspec at the end to avoid problem with
# the previous tests that are not necessarily prepared for
# the visibility feature.
ccs_show_title "Symbol visibility feature"

CCS_CHECK_VISIBILITY



############################################################################
# Final top-level CCS configuration
############################################################################

ccs_show_title "Final top-level CCS configuration"

############################################################################
# Libtool: part two
# (after C compiler setup = no compiler/linker tests after this)
############################################################################

ccs_show_subtitle "Libtool configuration"

# Use the undocumented solaris_use_stlport4 libtool variable to turn off any
# Cstd/stlport4 linkage. This allows Open MPI to be C++ STL agnostic.
if test "x$ccs_cv_c_compiler_vendor" = "xsun"; then
    solaris_use_stlport4="yes"
fi

dnl Not all versions of LT set the PACKAGE_VERSION
m4_ifdef([LT_PACKAGE_VERSION], [], [m4_define([LT_PACKAGE_VERSION], [1.5.22])])

m4_if(m4_version_compare(m4_defn([LT_PACKAGE_VERSION]), 2.0), -1, [
AC_LIBLTDL_CONVENIENCE(service/libltdl)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
], [
LT_CONFIG_LTDL_DIR([service/libltdl], [subproject])
LTDL_CONVENIENCE
LT_INIT([dlopen win32-dll])
])

CCS_SETUP_LIBLTDL

############################################################################
# final compiler config
############################################################################

m4_ifdef([project_ompi], [ccs_show_subtitle "Compiler flags"],
         [m4_ifdef([project_orte], [ccs_show_subtitle "Compiler flags"])])

#
# This is needed for VPATH builds, so that it will -I the appropriate
# include directory.  We delayed doing it until now just so that
# '-I$(top_srcdir)' doesn't show up in any of the configure output --
# purely aesthetic.
#
# Because ccs_config.h, orte_config.h, and ompi_config.h are all
# created by AC_CONFIG_HEADERS, we don't need to -I the builddir for
# <project>/include.  If we VPATH building, we do need to include the
# source directories, however.
#
if test "$CCS_TOP_BUILDDIR" != "$CCS_TOP_SRCDIR"; then
    # Note the embedded m4 directives here -- we must embed them
    # rather than have successive assignments to these shell
    # variables, lest the $(foo) names try to get evaluated here.
    # Yuck!
    CPPFLAGS='-I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/service/include m4_ifdef([project_orte], [-I$(top_srcdir)/orte/include]) m4_ifdef([project_ompi], [-I$(top_srcdir)/ompi/include])'" $CPPFLAGS"
    # C++ is only relevant if we're building CCS
    m4_ifdef([project_ompi], [CXXCPPFLAGS='-I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/service/include -I$(top_srcdir)/orte/include -I$(top_srcdir)/ompi/include'" $CXXCPPFLAGS"])
else
    CPPFLAGS='-I$(top_srcdir)'" $CPPFLAGS"
    # C++ is only relevant if we're building CCS
    m4_ifdef([project_ompi], [CXXCPPFLAGS='-I$(top_srcdir)'" $CXXCPPFLAGS"])
fi

# CCS/ORTE wants some additional processing of the flags (e.g., get
# versions without optimization for debugger modules).

m4_ifdef([project_orte], [ORTE_SETUP_DEBUGGER_FLAGS],
         [m4_ifdef([project_ompi], [ORTE_SETUP_DEBUGGER_FLAGS])])

#
# Delayed the substitution of CFLAGS and CXXFLAGS until now because
# they may have been modified throughout the course of this script.
#

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(CXXCPPFLAGS)
m4_ifdef([project_ompi], [AC_SUBST(FFLAGS)
                          AC_SUBST(FCFLAGS)

                          AC_SUBST(CCS_LIBMPI_EXTRA_LIBS)
                          AC_SUBST(CCS_LIBMPI_EXTRA_LDFLAGS)])

#
# Aggregate MCA parameters directory
#
AC_SUBST([AMCA_PARAM_SETS_DIR], ['$(pkgdatadir)/amca-param-sets'])

############################################################################
# final wrapper compiler config
############################################################################

ccs_show_subtitle "Wrapper compiler final setup"
# The ORTE and CCS wrapper scripts (i.e., not the C-compiled
# executables) need perl.
AC_PATH_PROG(PERL, perl, perl)

CCS_SETUP_WRAPPER_FINAL
m4_ifdef([project_orte], [ORTE_SETUP_WRAPPER_FINAL])
m4_ifdef([project_ompi], [CCS_SETUP_WRAPPER_FINAL])

# Recreate some defines prefixed with CCS_ so that there are no bare
# autoconf macro defines in mpi.h.  Since AC sometimes changes whether
# things are defined as null tokens or an integer result, two projects
# with different versions of AC can cause problems.
if test $ac_cv_header_stdc = yes; then
    AC_DEFINE(CCS_STDC_HEADERS, 1,
              [Do not use outside of mpi.h.  Define to 1 if you have the ANSI C header files.])
fi
if test $ac_cv_header_sys_time_h = yes ; then
    AC_DEFINE(CCS_HAVE_SYS_TIME_H, 1,
              [Do not use outside of mpi.h.  Define to 1 if you have the <sys/time.h> header file.])
fi
if test $ac_cv_type_long_long = yes ; then
    AC_DEFINE(CCS_HAVE_LONG_LONG, 1,
              [Do not use outside of mpi.h.  Define to 1 if the system has the type `long long'.]) dnl `
fi
if test $ac_cv_header_sys_synch_h = yes ; then
    AC_DEFINE(CCS_HAVE_SYS_SYNCH_H, 1,
              [Do not use outside of mpi.h.  Define to 1 if you have the <sys/synch.h> header file.])
fi

# If there is a local hook for each project, call it.  This allows 3rd
# parties to add configuration steps to CCS, ORTE, and/or CCS simply
# by placing a file in [ccs|orte|ompi]/config/whatever.m4 that
# AC_DEFUN's the appropriate macro name -- no patching is necessary.
# If that macro is defined, we'll run it here.
#
# Unfortunately, aclocal is not smart enough to parse something like
# the following in ompi_mca.m4 (when we're already m4 looping over the
# project list):
#
# m4_foreach(mca_project, [mca_project_list], 
#           [m4_ifdef(mca_project[_CONFIG_LOCAL], mca_project[_CONFIG_LOCAL])])
#
# Meaning that aclocal doesn't see that, for example,
# "ompi_CONFIG_LOCAL" is actually invoked at the bottom and therefore
# go look for an .m4 file that contains it.  Instead, we have to
# manually list the macros here.  *Then* aclocal is smart enough to go
# look for an .m4 file containing each macro, and if found,
# automatically m4_include the corresponding in aclocal.m4.  Bummer.
# :-\

m4_ifdef([ccs_CONFIG_LOCAL], [ccs_CONFIG_LOCAL])

############################################################################
# Party on
############################################################################

RTE_CONFIG

ccs_show_subtitle "Final output"

AC_CONFIG_FILES([
    Makefile

    config/Makefile
    service/Makefile
    ccs/include/Makefile
    ccs/proc/Makefile
])

CCS_CONFIG_FILES
#m4_ifdef([project_orte], [ORTE_CONFIG_FILES])
#m4_ifdef([project_ompi], [CCS_CONFIG_FILES])

AC_OUTPUT
