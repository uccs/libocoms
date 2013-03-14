# -*- shell-script -*-
#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2007 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# _OCOMS_CHECK_MX_MAPPER()
# ------------------------
# Check if the MX library provide the necessary functions in order to
# figure out the mapper version and MAC for each board.
AC_DEFUN([_OCOMS_CHECK_MX_MAPPER],[
    AC_CACHE_CHECK([for mx_open_board],
        [ocoms_cv_func_mx_open_board],
        [AC_LINK_IFELSE([AC_LANG_PROGRAM(
                [
#include <mx_extensions.h>
#include <mx_io.h>
#include <mx_internals/mx__fops.h>
                ], [
mx_open_board(0, NULL);
                ])],
            [ocoms_cv_func_mx_open_board="yes"],
            [ocoms_cv_func_mx_open_board="no"])])

    AC_CACHE_CHECK([for mx__get_mapper_state],
        [ocoms_cv_func_mx__get_mapper_state],
        [AC_LINK_IFELSE([AC_LANG_PROGRAM(
                [
#include <mx_extensions.h>
#include <mx_io.h>
#include <mx_internals/mx__driver_interface.h>
                ], [
mx__get_mapper_state(NULL, NULL);
                ])],
             [ocoms_cv_func_mx__get_mapper_state="yes"],
             [ocoms_cv_func_mx__get_mapper_state="no"])])

    AS_IF([test "$ocoms_cv_func_mx_open_board" = "yes" -a "$ocoms_cv_func_mx__get_mapper_state" = "yes"],
          [mx_provide_mapper_state=1
           $2],
          [mx_provide_mapper_state=0
           $3])
    AC_DEFINE_UNQUOTED([MX_HAVE_MAPPER_STATE], [$mx_provide_mapper_state],
                       [MX installation provide access to the mx_open_board and mx__get_mapper_state functions])
])


# OCOMS_CHECK_MX(prefix, [action-if-found], [action-if-not-found])
# --------------------------------------------------------
# check if MX support can be found.  sets prefix_{CPPFLAGS, 
# LDFLAGS, LIBS} as needed and runs action-if-found if there is
# support, otherwise executes action-if-not-found
AC_DEFUN([OCOMS_CHECK_MX],[
    AC_ARG_WITH([mx],
        [AC_HELP_STRING([--with-mx(=DIR)],
             [Build MX (Myrinet Express) support, optionally adding DIR/include, DIR/lib, and DIR/lib64 to the search path for headers and libraries])])
    OCOMS_CHECK_WITHDIR([mx], [$with_mx], [include/myriexpress.h])
    AC_ARG_WITH([mx-libdir],
        [AC_HELP_STRING([--with-mx-libdir=DIR],
             [Search for MX (Myrinet Express) libraries in DIR])])
    OCOMS_CHECK_WITHDIR([mx-libdir], [$with_mx_libdir], [libmyriexpress.*])

    ocoms_check_mx_$1_save_CPPFLAGS="$CPPFLAGS"
    ocoms_check_mx_$1_save_LDFLAGS="$LDFLAGS"
    ocoms_check_mx_$1_save_LIBS="$LIBS"

    AS_IF([test "$with_mx" != "no"],
          [AS_IF([test ! -z "$with_mx" -a "$with_mx" != "yes"],
                 [ocoms_check_mx_dir="$with_mx"])
           AS_IF([test ! -z "$with_mx_libdir" -a "$with_mx_libdir" != "yes"],
                 [ocoms_check_mx_libdir="$with_mx_libdir"])

           OCOMS_CHECK_PACKAGE([$1],
                              [myriexpress.h],
                              [myriexpress],
                              [mx_finalize],
                              [],
                              [$ocoms_check_mx_dir],
                              [$ocoms_check_mx_libdir],
                              [ocoms_check_mx_happy="yes"],
                              [ocoms_check_mx_happy="no"])],
          [ocoms_check_mx_happy="no"])

    CPPFLAGS="$CPPFLAGS $$1_CPPFLAGS"
    LDFLAGS="$LDFLAGS $$1_LDFLAGS"
    LIBS="$LIBS $$1_LIBS"

    # need at least version 1.0
    AS_IF([test "ocoms_check_mx_happy" = "yes"],
          [AC_CACHE_CHECK([for MX version 1.0 or later],
            [ocoms_cv_mx_version_ok],
            [AC_PREPROC_IFELSE(
                [AC_LANG_PROGRAM([
#include <myriexpress.h>
                             ],[
#if MX_API < 0x300
#error "Version less than 0x300"
#endif
                             ])],
            [ocoms_cv_mx_version_ok="yes"],
            [ocoms_cv_mx_version_ok="no"])])])
    AS_IF([test "$ocoms_cv_mx_version_ok" = "no"], [ocoms_check_mx_happy="no"])

    AS_IF([test "$ocoms_check_mx_happy" = "yes"],
          [AC_CHECK_HEADERS([mx_extensions.h],
               [AC_CHECK_FUNCS([mx_forget mx_register_unexp_handler])
                _OCOMS_CHECK_MX_MAPPER()],
               [AC_MSG_WARN([The MX support for Open MPI will be compiled without the
MX extensions, which may result in lower performance.  Please upgrade
to at least MX 1.2.0 to enable the MX extensions.])])])

    CPPFLAGS="$ocoms_check_mx_$1_save_CPPFLAGS"
    LDFLAGS="$ocoms_check_mx_$1_save_LDFLAGS"
    LIBS="$ocoms_check_mx_$1_save_LIBS"

    AS_IF([test "$ocoms_check_mx_happy" = "yes" -a "$enable_progress_threads" = "yes"],
          [AC_MSG_WARN([MX driver does not currently support progress threads.  Disabling BTL.])
           ocoms_check_mx_happy="no"])

    AS_IF([test "$ocoms_check_mx_happy" = "yes"],
          [$2],
          [AS_IF([test ! -z "$with_mx" -a "$with_mx" != "no"],
                 [AC_MSG_ERROR([MX support requested but not found.  Aborting])])
           $3])
])

