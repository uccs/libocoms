# -*- shell-script -*-
#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2007 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2006-2012 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2009-2011 Oracle and/or its affiliates.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# OCOMS_CHECK_VISIBILITY
# --------------------------------------------------------
AC_DEFUN([OCOMS_CHECK_VISIBILITY],[
    AC_REQUIRE([AC_PROG_GREP])

    # Check if the compiler has support for visibility, like some
    # versions of gcc, icc Sun Studio cc.
    AC_ARG_ENABLE(visibility, 
        AC_HELP_STRING([--enable-visibility],
            [enable visibility feature of certain compilers/linkers (default: enabled)]))

    ocoms_visibility_define=0
    ocoms_msg="whether to enable symbol visibility"

    if test "$enable_visibility" = "no"; then
        AC_MSG_CHECKING([$ocoms_msg])
        AC_MSG_RESULT([no (disabled)]) 
    else
        CFLAGS_orig=$CFLAGS

        ocoms_add=
        case "$ompi_c_vendor" in
        sun)
            # Check using Sun Studio -xldscope=hidden flag
            ocoms_add=-xldscope=hidden
            CFLAGS="$OMPI_CFLAGS_BEFORE_PICKY $ocoms_add -errwarn=%all"
            ;;

        *)
            # Check using -fvisibility=hidden
            ocoms_add=-fvisibility=hidden
            CFLAGS="$OMPI_CFLAGS_BEFORE_PICKY $ocoms_add -Werror"
            ;;
        esac

        AC_MSG_CHECKING([if $CC supports $ocoms_add])
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[
            #include <stdio.h>
            __attribute__((visibility("default"))) int foo;
            ]],[[fprintf(stderr, "Hello, world\n");]])],
            [AS_IF([test -s conftest.err],
                   [$GREP -iq visibility conftest.err
                    # If we find "visibility" in the stderr, then
                    # assume it doesn't work
                    AS_IF([test "$?" = "0"], [ocoms_add=])])
            ], [ocoms_add=])
        AS_IF([test "$ocoms_add" = ""],
              [AC_MSG_RESULT([no])],
              [AC_MSG_RESULT([yes])])

        CFLAGS=$CFLAGS_orig
        OCOMS_VISIBILITY_CFLAGS=$ocoms_add

        if test "$ocoms_add" != "" ; then
            ocoms_visibility_define=1
            AC_MSG_CHECKING([$ocoms_msg])
            AC_MSG_RESULT([yes (via $ocoms_add)])
        elif test "$enable_visibility" = "yes"; then
            AC_MSG_ERROR([Symbol visibility support requested but compiler does not seem to support it.  Aborting])
        else
            AC_MSG_CHECKING([$ocoms_msg])
            AC_MSG_RESULT([no (unsupported)])
        fi
        unset ocoms_add
    fi

    AC_DEFINE_UNQUOTED([OCOMS_C_HAVE_VISIBILITY], [$ocoms_visibility_define],
            [Whether C compiler supports symbol visibility or not])
])
