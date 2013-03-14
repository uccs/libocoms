dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2010     Cisco Systems, In.  All rights reserved.
dnl
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl

# OCOMS_CHECK_OS_FLAVOR_SPECIFIC()
# ----------------------------------------------------
# Helper macro from OCOMS-CHECK-OS-FLAVORS(), below.
# $1 = macro to look for
# $2 = suffix of env variable to set with results
AC_DEFUN([OCOMS_CHECK_OS_FLAVOR_SPECIFIC],
[
    AC_MSG_CHECKING([$1])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
     [[#ifndef $1
      error: this isnt $1
      #endif
     ]])],
                      [ocoms_found_$2=yes],
                      [ocoms_found_$2=no])
    AC_MSG_RESULT([$ocoms_found_$2])
])dnl

# OCOMS_CHECK_OS_FLAVORS()
# ----------------------------------------------------
# Try to figure out the various OS flavors out there.
#
AC_DEFUN([OCOMS_CHECK_OS_FLAVORS],
[
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__NetBSD__], [netbsd])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__FreeBSD__], [freebsd])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__OpenBSD__], [openbsd])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__DragonFly__], [dragonfly])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__386BSD__], [386bsd])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__bsdi__], [bsdi])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__APPLE__], [apple])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__linux__], [linux])
    OCOMS_CHECK_OS_FLAVOR_SPECIFIC([__sun__], [sun])

    # check for sockaddr_in (a good sign we have TCP)
    AC_CHECK_TYPES([struct sockaddr_in], 
                   [ocoms_found_sockaddr=yes],
                   [ocoms_found_sockaddr=no], 
                   [AC_INCLUDES_DEFAULT
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif])
])dnl
