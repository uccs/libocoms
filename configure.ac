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
# call OCOMS_GET_VERSION (etc.) before AC_INIT.  So use the shell
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

OCOMS_CONFIGURE_SETUP
ocoms_show_title "Configuring project_name_long"

#
# Setup some things that must be done before AM-INIT-AUTOMAKE
#

ocoms_show_subtitle "Startup tests"
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AC_DEFINE_UNQUOTED(OCOMS_ARCH, "$target", [OCOMS architecture string])
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
OCOMS_LOAD_PLATFORM

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
#AM_CONDITIONAL([PROJECT_OCOMS], m4_ifdef([project_ompi], [true], [false]))

ocoms_show_subtitle "Checking versions"

. $srcdir/VERSION

#
# Hardwire all progress threads to be off
#
enable_progress_threads="no"
ORTE_ENABLE_PROGRESS_THREADS=0
AC_DEFINE_UNQUOTED([ORTE_ENABLE_PROGRESS_THREADS], [$ORTE_ENABLE_PROGRESS_THREADS],
                   [Hardcode the ORTE progress thread to be off])
OCOMS_ENABLE_PROGRESS_THREADS=0
AC_DEFINE_UNQUOTED([OCOMS_ENABLE_PROGRESS_THREADS], [$OCOMS_ENABLE_PROGRESS_THREADS],
                   [Hardcode the OCOMS progress thread to be off])


# List header files to generate

AM_CONFIG_HEADER([ocoms/platform/ocoms_config.h])

ocoms_show_subtitle "Initialization, setup"

OCOMS_TOP_BUILDDIR="`pwd`"
AC_SUBST(OCOMS_TOP_BUILDDIR)
cd "$srcdir"
OCOMS_TOP_SRCDIR="`pwd`"
AC_SUBST(OCOMS_TOP_SRCDIR)
cd "$OCOMS_TOP_BUILDDIR"

AC_MSG_NOTICE([builddir: $OCOMS_TOP_BUILDDIR])
AC_MSG_NOTICE([srcdir: $OCOMS_TOP_SRCDIR])
if test "$OCOMS_TOP_BUILDDIR" != "$OCOMS_TOP_SRCDIR"; then
    AC_MSG_NOTICE([Detected VPATH build])
fi

# Setup the top of the ocoms/platform/ocoms_config.h file

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

#ifndef OCOMS_CONFIG_H
#define OCOMS_CONFIG_H

/*#include "ocoms_config_top.h" */

])

AH_BOTTOM([
#include "ocoms/platform/ocoms_config_bottom.h"
#endif /* OCOMS_CONFIG_H */
])

# Other basic setup stuff (shared with components)

OCOMS_BASIC_SETUP

top_ocoms_srcdir="$OCOMS_TOP_SRCDIR"
AC_SUBST(top_ocoms_srcdir)
top_ocoms_builddir="$OCOMS_TOP_BUILDDIR"
AC_SUBST(top_ocoms_builddir)

# Version number calculation
syscmd(test -e .hg) dnl
ifelse(sysval, 0, dnl
       [define([svnversion], esyscmd([sh -c "hg identify -n| tr -d '\n'"]))]) dnl

syscmd(test -e .git) dnl
ifelse(sysval, 0, dnl
       [define([svnversion], esyscmd([sh -c "git rev-parse --short=7 HEAD| tr -d '\n'"]))]) dnl
 
MAJOR_VERSION="$major"
MINOR_VERSION="$minor"
MICRO_VERSION=svnversion

VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION
PACKAGE_VERSION="$VERSION"
PACKAGE_STRING="${PACKAGE_NAME} ${PACKAGE_VERSION}"

AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)

AC_CONFIG_FILES([ocoms/libocoms_version.h libocoms.spec])

# RPM build info
RPM_RELEASE=1
AC_SUBST(RPM_RELEASE)
RPM_DIR=${top_fca_dir}
AC_SUBST(RPM_DIR)
RPM_TARBALL=${PACKAGE}-${VERSION}.tar.gz
AC_SUBST(RPM_TARBALL)

BUILD_DATE="`date +'%Y%m%d'`"
AC_SUBST([BUILD_DATE])

BUILD_TIME="`date +'1%H%M%S'`"
AC_SUBST([BUILD_TIME])

BUILD_HOST="`hostname`"
AC_SUBST([BUILD_HOST])

BUILD_USER="`whoami`"
AC_SUBST([BUILD_USER])

############################################################################
# Configuration options
############################################################################

OCOMS_CONFIGURE_OPTIONS
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

ocoms_enable_shared="$enable_shared"
ocoms_enable_static="$enable_static"
AM_ENABLE_SHARED
AM_DISABLE_STATIC

OCOMS_SETUP_WRAPPER_INIT
# There is no [need for] ORTE_SETUP_WRAPPER_INIT; this is not an
# accidental omission
m4_ifdef([project_ompi], [OCOMS_SETUP_WRAPPER_INIT])

##################################
# Check for known incompatibility
##################################

# Do *not* print a message that we're checking the OS because this
# test is *not* meant to be an all-inclusive "if it passes this test,
# then configure must succeed" test.  This test is *only* mean to
# screen out the versions of OS X where we know OCOMS will cause kernel
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
ocoms_show_title "Compiler and preprocessor tests"

##################################
# C compiler characteristics
##################################

OCOMS_SETUP_CC

# Does the compiler support "ident"-like constructs?

OCOMS_CHECK_IDENT([CC], [CFLAGS], [c], [C])

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
# We only need these types if we're building the OCOMS project, but
# OCOMS currently doesn't protect for their lack of presence well.
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

# We only need these types if we're building the OCOMS project, but
# OCOMS currently doesn't protect for their lack of presence well.
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

OCOMS_C_GET_ALIGNMENT(_Bool, OCOMS_ALIGNMENT_BOOL)
OCOMS_C_GET_ALIGNMENT(int8_t, OCOMS_ALIGNMENT_INT8)
OCOMS_C_GET_ALIGNMENT(int16_t, OCOMS_ALIGNMENT_INT16)
OCOMS_C_GET_ALIGNMENT(int32_t, OCOMS_ALIGNMENT_INT32)
OCOMS_C_GET_ALIGNMENT(int64_t, OCOMS_ALIGNMENT_INT64)
if test "$ac_cv_type_int128_t" = yes ; then
  OCOMS_C_GET_ALIGNMENT(int128_t, OCOMS_ALIGNMENT_INT128)
fi
OCOMS_C_GET_ALIGNMENT(char, OCOMS_ALIGNMENT_CHAR)
OCOMS_C_GET_ALIGNMENT(short, OCOMS_ALIGNMENT_SHORT)
OCOMS_C_GET_ALIGNMENT(wchar_t, OCOMS_ALIGNMENT_WCHAR)
OCOMS_C_GET_ALIGNMENT(int, OCOMS_ALIGNMENT_INT)
OCOMS_C_GET_ALIGNMENT(long, OCOMS_ALIGNMENT_LONG)
if test "$ac_cv_type_long_long" = yes; then
    OCOMS_C_GET_ALIGNMENT(long long, OCOMS_ALIGNMENT_LONG_LONG)
fi
OCOMS_C_GET_ALIGNMENT(float, OCOMS_ALIGNMENT_FLOAT)
OCOMS_C_GET_ALIGNMENT(double, OCOMS_ALIGNMENT_DOUBLE)
if test "$ac_cv_type_long_double" = yes; then
    OCOMS_C_GET_ALIGNMENT(long double, OCOMS_ALIGNMENT_LONG_DOUBLE)
fi

# We only need these types if we're building the OCOMS project, but
# OCOMS currently doesn't protect for their lack of presence well.
if test "$ac_cv_type_float__Complex" = yes; then
    OCOMS_C_GET_ALIGNMENT(float _Complex, OCOMS_ALIGNMENT_FLOAT_COMPLEX)
fi
if test "$ac_cv_type_double__Complex" = yes; then
    OCOMS_C_GET_ALIGNMENT(double _Complex, OCOMS_ALIGNMENT_DOUBLE_COMPLEX)
fi
if test "$ac_cv_type_long_double__Complex" = yes; then
    OCOMS_C_GET_ALIGNMENT(long double _Complex, OCOMS_ALIGNMENT_LONG_DOUBLE_COMPLEX)
fi

OCOMS_C_GET_ALIGNMENT(void *, OCOMS_ALIGNMENT_VOID_P)

#
# Does the C compiler native support "bool"? (i.e., without
# <stdbool.h> or any other help)
#

AC_MSG_CHECKING(for C bool type)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT],
[[bool bar, foo = true; bar = foo;]])],
[OCOMS_NEED_C_BOOL=0 MSG=yes],[OCOMS_NEED_C_BOOL=1 MSG=no])
AC_DEFINE_UNQUOTED(OCOMS_NEED_C_BOOL, $OCOMS_NEED_C_BOOL,
    [Whether the C compiler supports "bool" without any other help (such as <stdbool.h>)])
AC_MSG_RESULT([$MSG])
AC_CHECK_SIZEOF(_Bool)


AC_DEFINE([OCOMS_FORTRAN_HANDLE_MAX], [128], [Bogus type for OCOMS])
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
    CFLAGS="$OCOMS_CFLAGS_BEFORE_PICKY -Werror -ansi"
fi
AC_C_INLINE
# Microsoft compilers support 2 versions of restrict. One for functions, and
# one for variables. The problem is that they don't have an equivalent
# syntax, and the autoconf restrict detection is unable to detect them
# correctly. It detect the restrict keyword as __restrict which break the
# rules for function syntax which is declspec(restrict).
if test "x$ocoms_cv_c_compiler_vendor" != "xmicrosoft"; then
    AC_C_RESTRICT
fi
OCOMS_C_WEAK_SYMBOLS
if test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS_save"
fi

if test "x$CC" = "xicc"; then
    OCOMS_CHECK_ICC_VARARGS
fi

# If we want the profiling layer:
# - If the C compiler has weak symbols, use those.
# - If not, then set to compile the code again with #define's in a
#   separate directory.

if test "$WANT_WEAK_SYMBOLS" = "0"; then
    OCOMS_C_HAVE_WEAK_SYMBOLS=0
fi
if test "$WANT_MPI_PROFILING" = "1"; then
    if test "$OCOMS_C_HAVE_WEAK_SYMBOLS" = "1"; then
        OCOMS_PROFILING_COMPILE_SEPARATELY=0
    else
        OCOMS_PROFILING_COMPILE_SEPARATELY=1
    fi
else
    OCOMS_PROFILING_COMPILE_SEPARATELY=0
fi

# Check if we support the offsetof compiler directive

OCOMS_CHECK_OFFSETOF


##################################
# C++ compiler characteristics
##################################

# We don't need C++ unless we're building Open MPI; ORTE and OCOMS do
# not use C++ at all.  The OCOMS macro name appears to be a bit of a
# misnomer; I'm not sure why it was split into a second macro and put
# into OCOMS...?  All it does is setup the C++ compiler (the OCOMS macro
# sets up the C++ MPI bindings, etc.).  Perhaps it was moved to OCOMS
# just on the rationale that all compiler setup should be done in
# OCOMS...?  Shrug.
m4_ifdef([project_ocoms], [OCOMS_SETUP_CXX
                          OCOMS_SETUP_CXX])
##################################
# Only after setting up both
# C and C++ check compiler attributes.
##################################

ocoms_show_subtitle "Compiler characteristics"

OCOMS_CHECK_ATTRIBUTES
OCOMS_CHECK_COMPILER_VERSION_ID


##################################
# Assembler Configuration
##################################

ocoms_show_subtitle "Assembler"

AM_PROG_AS
OCOMS_CONFIG_ASM

# checkpoint results
AC_CACHE_SAVE


##################################
# Header files
##################################

ocoms_show_title "Header file tests"

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
# Define OCOMS_USE_STDBOOL_H as approrpaite.
AC_CHECK_HEADERS([stdbool.h], [have_stdbool_h=1], [have_stdbool_h=0])
AC_MSG_CHECKING([if <stdbool.h> works])
if test "$have_stdbool_h" = "1"; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT[
#if HAVE_STDBOOL_H
#include <stdbool.h>
#endif]],
[[bool bar, foo = true; bar = foo;]])],
[OCOMS_USE_STDBOOL_H=1 MSG=yes],[OCOMS_USE_STDBOOL_H=0 MSG=no])
else
    OCOMS_USE_STDBOOL_H=0
    MSG="no (don't have <stdbool.h>)"
fi
AC_DEFINE_UNQUOTED(OCOMS_USE_STDBOOL_H, $OCOMS_USE_STDBOOL_H,
    [Whether to use <stdbool.h> or not])
AC_MSG_RESULT([$MSG])

# checkpoint results
AC_CACHE_SAVE

##################################
# Types
##################################

ocoms_show_title "Type tests"

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
AC_DEFINE_UNQUOTED(OCOMS_HAVE_SA_RESTART, $VALUE,
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

ocoms_show_title "Library and Function tests"

# Darwin doesn't need -lutil, as it's something other than this -lutil.
OCOMS_CHECK_FUNC_LIB([openpty], [util])

AC_CHECK_LIB([nsl], [gethostbyname])

AC_CHECK_LIB([socket], [socket])

# Solaris has sched_yield in -lrt, usually in libc
OCOMS_CHECK_FUNC_LIB([sched_yield], [rt])

# IRIX has dirname in -lgen, usually in libc
OCOMS_CHECK_FUNC_LIB([dirname], [gen])

# Darwin doesn't need -lm, as it's a symlink to libSystem.dylib
OCOMS_CHECK_FUNC_LIB([ceil], [m])

AC_CHECK_FUNCS([asprintf snprintf vasprintf vsnprintf openpty isatty getpwuid fork waitpid execve pipe ptsname setsid mmap tcgetpgrp posix_memalign strsignal sysconf syslog vsyslog regcmp regexec regfree _NSGetEnviron socketpair strncpy_s _strdup usleep mkfifo dbopen dbm_open])

# On some hosts, htonl is a define, so the AC_CHECK_FUNC will get
# confused.  On others, it's in the standard library, but stubbed with
# the magic glibc foo as not implemented.  and on other systems, it's
# just not there.  This covers all cases.
AC_CACHE_CHECK([for htonl define],
  [ocoms_cv_htonl_define],
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
])], [ocoms_cv_htonl_define=yes], [ocoms_cv_htonl_define=no])])
AC_CHECK_FUNC([htonl], [ocoms_have_htonl=yes], [ocoms_have_htonl=no])
AS_IF([test "$ocoms_cv_htonl_define" = "yes" -o "$ocoms_have_htonl" = "yes"],
    [AC_DEFINE_UNQUOTED([HAVE_UNIX_BYTESWAP], [1],
        [whether unix byteswap routines -- htonl, htons, nothl, ntohs -- are available])])

#
# Make sure we can copy va_lists (need check declared, not linkable)
#

AC_CHECK_DECL(va_copy, OCOMS_HAVE_VA_COPY=1, OCOMS_HAVE_VA_COPY=0,
    [#include <stdarg.h>])
AC_DEFINE_UNQUOTED(OCOMS_HAVE_VA_COPY, $OCOMS_HAVE_VA_COPY,
    [Whether we have va_copy or not])

AC_CHECK_DECL(__va_copy, OCOMS_HAVE_UNDERSCORE_VA_COPY=1,
    OCOMS_HAVE_UNDERSCORE_VA_COPY=0, [#include <stdarg.h>])
AC_DEFINE_UNQUOTED(OCOMS_HAVE_UNDERSCORE_VA_COPY, $OCOMS_HAVE_UNDERSCORE_VA_COPY,
    [Whether we have __va_copy or not])

AC_CHECK_DECLS(__func__)

# checkpoint results
AC_CACHE_SAVE


##################################
# System-specific tests
##################################

ocoms_show_title "System-specific tests"

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

AC_DEFINE_UNQUOTED(OCOMS_MPI_OFFSET_TYPE, $MPI_OFFSET_TYPE, [Type of MPI_Offset -- has to be defined here and typedef'ed later because mpi.h does not get AC SUBST's])
AC_DEFINE_UNQUOTED(OCOMS_MPI_OFFSET_SIZE, $MPI_OFFSET_SIZE, [Size of the MPI_Offset])
AC_DEFINE_UNQUOTED(OCOMS_OFFSET_DATATYPE, $MPI_OFFSET_DATATYPE, [MPI datatype corresponding to MPI_Offset])

#
# Check for MPI_Aint type.  Yes, there are platforms where
# sizeof(void*) != sizeof(long) (64 bit Windows, apparently).
#
if test $ac_cv_type_ptrdiff_t = yes ; then
    ocoms_ptrdiff_t="ptrdiff_t"
elif test $ac_cv_sizeof_void_p -eq $ac_cv_sizeof_long ; then
    ocoms_ptrdiff_t="long"
elif test $ac_cv_type_long_long = yes -a $ac_cv_sizeof_void_p -eq $ac_cv_sizeof_long_long ; then
    ocoms_ptrdiff_t="long long"
else
    AC_MSG_ERROR([Could not find datatype to emulate ptrdiff_t.  Cannot continue])
fi
AC_DEFINE_UNQUOTED([OCOMS_PTRDIFF_TYPE], [$ocoms_ptrdiff_t],
    [type to use for ptrdiff_t])

# Do we have _SC_NPROCESSORS_ONLN? (only going to pass if we also have
# <unistd.h> and sysconf(), which is ok) OS X 10.4 has <unistd.h> and
# sysconf(), but does not have _SC_NPROCESSORS_ONLN.  Doh!

AC_CACHE_CHECK([for _SC_NPROCESSORS_ONLN],
  [ocoms_cv_have__SC_NPROCESSORS_ONLN],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT
#include <unistd.h>
],
     [int i = _SC_NPROCESSORS_ONLN;])],
     [ocoms_cv_have__SC_NPROCESSORS_ONLN="yes"],
     [ocoms_cv_have__SC_NPROCESSORS_ONLN="no"])])
AS_IF([test "$ocoms_cv_have__SC_NPROCESSORS_ONLN" = "yes"],
      [result=1], [result=0])
AC_DEFINE_UNQUOTED([OCOMS_HAVE__SC_NPROCESSORS_ONLN], [$result],
  [Define to 1 ifyou have the declaration of _SC_NPROCESSORS_ONLN, and to 0 otherwise])

# all: endian

AC_C_BIGENDIAN

OCOMS_CHECK_BROKEN_QSORT

AC_CACHE_CHECK([if word-sized integers must be word-size aligned],
    [ocoms_cv_c_word_size_align],
    [AC_LANG_PUSH(C)
     AC_RUN_IFELSE([AC_LANG_PROGRAM([dnl
#include <stdlib.h>], [[    long data[2] = {0, 0};
    long *lp;
    int *ip;
    ip = (int*) data;
    ip++;
    lp = (long*) ip;
    return lp[0]; ]])],
        [ocoms_cv_c_word_size_align=no],
        [ocoms_cv_c_word_size_align=yes],
        [ocoms_cv_c_word_size_align=yes])])
AS_IF([test $ocoms_cv_c_word_size_align = yes], [results=1], [results=0])
AC_DEFINE_UNQUOTED([OCOMS_ALIGN_WORD_SIZE_INTEGERS], [$results],
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
OCOMS_CONFIG_THREADS
m4_ifdef([project_ompi], [OCOMS_CONFIG_THREADS])

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
    if test ! -f "$srcdir/ocoms/util/show_help_lex.c"; then
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

m4_ifdef([project_ocoms], [OCOMS_CASE_SENSITIVE_FS_SETUP])

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

ocoms_show_title "Modular Component Architecture (MCA) setup"

AC_MSG_CHECKING([for subdir args])
OCOMS_CONFIG_SUBDIR_ARGS([ocoms_subdir_args])
AC_MSG_RESULT([$ocoms_subdir_args])

####OCOMS_MCA

# checkpoint results
AC_CACHE_SAVE

##################################
# MPI Extended Interfaces
##################################

m4_ifdef([project_ompi], [OCOMS_SETUP_MPI_EXT])

# checkpoint results
AC_CACHE_SAVE

##################################
# Contributed software
##################################

m4_ifdef([project_ompi], [OCOMS_SETUP_CONTRIB])

# checkpoint results
AC_CACHE_SAVE

##################################
# Visibility
##################################

# Check the visibility declspec at the end to avoid problem with
# the previous tests that are not necessarily prepared for
# the visibility feature.
ocoms_show_title "Symbol visibility feature"

OCOMS_CHECK_VISIBILITY



############################################################################
# Final top-level OCOMS configuration
############################################################################

ocoms_show_title "Final top-level OCOMS configuration"

############################################################################
# Libtool: part two
# (after C compiler setup = no compiler/linker tests after this)
############################################################################

    ocoms_show_subtitle "Libtool configuration"

# Use the undocumented solaris_use_stlport4 libtool variable to turn off any
# Cstd/stlport4 linkage. This allows Open MPI to be C++ STL agnostic.
    if test "x$ocoms_cv_c_compiler_vendor" = "xsun"; then
        solaris_use_stlport4="yes"
    fi

    dnl Not all versions of LT set the PACKAGE_VERSION
    m4_ifdef([LT_PACKAGE_VERSION], [], [m4_define([LT_PACKAGE_VERSION], [1.5.22])])

    m4_if(m4_version_compare(m4_defn([LT_PACKAGE_VERSION]), 2.0), -1, [
    AC_LIBLTDL_CONVENIENCE(ocoms/libltdl)
    AC_LIBTOOL_DLOPEN
    AC_PROG_LIBTOOL
    ], [
    LT_CONFIG_LTDL_DIR([ocoms/libltdl], [subproject])
    LTDL_CONVENIENCE
    LT_INIT([dlopen win32-dll])
    ])

    OCOMS_SETUP_LIBLTDL

############################################################################
# final compiler config
############################################################################

    m4_ifdef([project_ompi], [ocoms_show_subtitle "Compiler flags"],
             [m4_ifdef([project_orte], [ocoms_show_subtitle "Compiler flags"])])

#
# This is needed for VPATH builds, so that it will -I the appropriate
# include directory.  We delayed doing it until now just so that
# '-I$(top_srcdir)' doesn't show up in any of the configure output --
# purely aesthetic.
#
# Because ocoms_config.h, orte_config.h, and ompi_config.h are all
# created by AC_CONFIG_HEADERS, we don't need to -I the builddir for
# <project>/include.  If we VPATH building, we do need to include the
# source directories, however.
#
    if test "$OCOMS_TOP_BUILDDIR" != "$OCOMS_TOP_SRCDIR"; then
        # Note the embedded m4 directives here -- we must embed them
        # rather than have successive assignments to these shell
        # variables, lest the $(foo) names try to get evaluated here.
        # Yuck!
        CPPFLAGS='-I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/ocoms/include m4_ifdef([project_orte], [-I$(top_srcdir)/orte/include]) m4_ifdef([project_ompi], [-I$(top_srcdir)/ompi/include])'" $CPPFLAGS"
        # C++ is only relevant if we're building OCOMS
        m4_ifdef([project_ompi], [CXXCPPFLAGS='-I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/ocoms/include -I$(top_srcdir)/orte/include -I$(top_srcdir)/ompi/include'" $CXXCPPFLAGS"])
    else
        CPPFLAGS='-I$(top_srcdir)'" $CPPFLAGS"
        # C++ is only relevant if we're building OCOMS
        m4_ifdef([project_ompi], [CXXCPPFLAGS='-I$(top_srcdir)'" $CXXCPPFLAGS"])
    fi

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

                              AC_SUBST(OCOMS_LIBMPI_EXTRA_LIBS)
                              AC_SUBST(OCOMS_LIBMPI_EXTRA_LDFLAGS)])

#
# Aggregate MCA parameters directory
#
    AC_SUBST([AMCA_PARAM_SETS_DIR], ['$(pkgdatadir)/amca-param-sets'])

############################################################################
# final wrapper compiler config
############################################################################

    ocoms_show_subtitle "Wrapper compiler final setup"
# The ORTE and OCOMS wrapper scripts (i.e., not the C-compiled
# executables) need perl.
    AC_PATH_PROG(PERL, perl, perl)

# Recreate some defines prefixed with OCOMS_ so that there are no bare
# autoconf macro defines in mpi.h.  Since AC sometimes changes whether
# things are defined as null tokens or an integer result, two projects
# with different versions of AC can cause problems.
    if test $ac_cv_header_stdc = yes; then
        AC_DEFINE(OCOMS_STDC_HEADERS, 1,
                  [Do not use outside of mpi.h.  Define to 1 if you have the ANSI C header files.])
    fi
    if test $ac_cv_header_sys_time_h = yes ; then
        AC_DEFINE(OCOMS_HAVE_SYS_TIME_H, 1,
                  [Do not use outside of mpi.h.  Define to 1 if you have the <sys/time.h> header file.])
    fi
    if test $ac_cv_type_long_long = yes ; then
        AC_DEFINE(OCOMS_HAVE_LONG_LONG, 1,
                  [Do not use outside of mpi.h.  Define to 1 if the system has the type 'long long'.])
    fi
    if test $ac_cv_header_sys_synch_h = yes ; then
        AC_DEFINE(OCOMS_HAVE_SYS_SYNCH_H, 1,
                  [Do not use outside of mpi.h.  Define to 1 if you have the <sys/synch.h> header file.])
    fi

# If there is a local hook for each project, call it.  This allows 3rd
# parties to add configuration steps to OCOMS, ORTE, and/or OCOMS simply
# by placing a file in [ocoms|orte|ompi]/config/whatever.m4 that
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

    m4_ifdef([ocoms_CONFIG_LOCAL], [ocoms_CONFIG_LOCAL])

############################################################################
# Party on
############################################################################

    RTE_CONFIG

    ocoms_show_subtitle "Final output"
    files="Makefile 
           config/Makefile 
           ocoms/Makefile 
           ocoms/util/Makefile 
           ocoms/util/keyval/Makefile 
           tests/Makefile 
           tests/init/Makefile 
           tests/onesided/Makefile"

#ocoms/Makefile 
    files1="Makefile
           config/Makefile
           ocoms/util/Makefile
           ocoms/util/keyval/Makefile"

    AC_CONFIG_FILES([
            Makefile
            config/Makefile 
            ocoms/Makefile 
            ocoms/util/Makefile 
            ocoms/util/keyval/Makefile 
    ])

    OCOMS_CONFIG_FILES

AC_OUTPUT
