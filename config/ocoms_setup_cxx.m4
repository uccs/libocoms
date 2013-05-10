dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
dnl                         University Research and Technology
dnl                         Corporation.  All rights reserved.
dnl Copyright (c) 2004-2006 The University of Tennessee and The University
dnl                         of Tennessee Research Foundation.  All rights
dnl                         reserved.
dnl Copyright (c) 2004-2008 High Performance Computing Center Stuttgart,
dnl                         University of Stuttgart.  All rights reserved.
dnl Copyright (c) 2004-2006 The Regents of the University of California.
dnl                         All rights reserved.
dnl Copyright (c) 2006      Los Alamos National Security, LLC.  All rights
dnl                         reserved. 
dnl Copyright (c) 2007-2009 Sun Microsystems, Inc.  All rights reserved.
dnl Copyright (c) 2008-2009 Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
dnl Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

# This macro is necessary to get the title to be displayed first.  :-)
AC_DEFUN([OCOMS_SETUP_CXX_BANNER],[
    ocoms_show_subtitle "C++ compiler and preprocessor" 
])

# This macro is necessary because PROG_CXX* is REQUIREd by multiple
# places in SETUP_CXX.
AC_DEFUN([OCOMS_PROG_CXX],[
    OCOMS_VAR_SCOPE_PUSH([ocoms_cxxflags_save])
    ocoms_cxxflags_save="$CXXFLAGS"
    AC_PROG_CXX
    AC_PROG_CXXCPP
    CXXFLAGS="$ocoms_cxxflags_save"
    OCOMS_VAR_SCOPE_POP
])

# OCOMS_SETUP_CXX()
# ----------------
# Do everything required to setup the C++ compiler.  Safe to AC_REQUIRE
# this macro.
AC_DEFUN([OCOMS_SETUP_CXX],[
    AC_REQUIRE([OCOMS_SETUP_CXX_BANNER])

    _OCOMS_SETUP_CXX_COMPILER

    OCOMS_CXX_COMPILER_VENDOR([ocoms_cxx_vendor])

    _OCOMS_SETUP_CXX_COMPILER_BACKEND
])

# _OCOMS_SETUP_CXX_COMPILER()
# --------------------------
# Setup the CXX compiler
AC_DEFUN([_OCOMS_SETUP_CXX_COMPILER],[
    OCOMS_VAR_SCOPE_PUSH(ocoms_cxx_compiler_works)

    # Must REQUIRE the PROG_CXX macro and not call it directly here for
    # reasons well-described in the AC2.64 (and beyond) docs.
    AC_REQUIRE([OCOMS_PROG_CXX])
    BASECXX="`basename $CXX`"

    AS_IF([test "x$CXX" = "x"], [CXX=none])
    set dummy $CXX
    ocoms_cxx_argv0=[$]2
    OCOMS_WHICH([$ocoms_cxx_argv0], [OCOMS_CXX_ABSOLUTE])
    AS_IF([test "x$OCOMS_CXX_ABSOLUTE" = "x"], [OCOMS_CXX_ABSOLUTE=none])

    AC_DEFINE_UNQUOTED(OCOMS_CXX, "$CXX", [OCOMS underlying C++ compiler])
    AC_SUBST(OCOMS_CXX_ABSOLUTE)

    OCOMS_VAR_SCOPE_POP
])

# _OCOMS_SETUP_CXX_COMPILER_BACKEND()
# ----------------------------------
# Back end of _OCOMS_SETUP_CXX_COMPILER_BACKEND()
AC_DEFUN([_OCOMS_SETUP_CXX_COMPILER_BACKEND],[
    # Do we want code coverage
    if test "$WANT_COVERAGE" = "1"; then 
        if test "$ocoms_cxx_vendor" = "gnu" ; then
            AC_MSG_WARN([$OCOMS_COVERAGE_FLAGS has been added to CFLAGS (--enable-coverage)])
            WANT_DEBUG=1
            CXXFLAGS="${CXXFLAGS} $OCOMS_COVERAGE_FLAGS"
            WRAPPER_EXTRA_CXXFLAGS="${WRAPPER_EXTRA_CXXFLAGS} $OCOMS_COVERAGE_FLAGS"
        else
            AC_MSG_WARN([Code coverage functionality is currently available only with GCC suite])
            AC_MSG_ERROR([Configure: cannot continue])
        fi
    fi

    # Do we want debugging?
    if test "$WANT_DEBUG" = "1" -a "$enable_debug_symbols" != "no" ; then
        CXXFLAGS="$CXXFLAGS -g"
        OCOMS_UNIQ(CXXFLAGS)
        AC_MSG_WARN([-g has been added to CXXFLAGS (--enable-debug)])
    fi

    # These flags are generally g++-specific; even the g++-impersonating
    # compilers won't accept them.
    OCOMS_CXXFLAGS_BEFORE_PICKY="$CXXFLAGS"
    if test "$WANT_PICKY_COMPILER" = 1 -a "$ocoms_cxx_vendor" = "gnu"; then
        add="-Wall -Wundef -Wno-long-long"

        # see if -Wno-long-double works...
        AC_LANG_PUSH(C++)
        CXXFLAGS_orig="$CXXFLAGS"
        CXXFLAGS="$CXXFLAGS $add -Wno-long-double -fstrict-prototype"
        AC_CACHE_CHECK([if $CXX supports -Wno-long-double],
                   [ocoms_cv_cxx_wno_long_double],
                   [AC_TRY_COMPILE([], [], 
                                   [dnl Alright, the -Wno-long-double did not produce any errors...
                                    dnl Well well, try to extract a warning regarding unrecognized or ignored options
                                    AC_TRY_COMPILE([], [long double test;], 
                                                   [
                                                       ocoms_cv_cxx_wno_long_double="yes"
                                                       if test -s conftest.err ; then
                                                           dnl Yes, it should be "ignor", in order to catch ignoring and ignore
                                                           for i in invalid ignor unrecognized ; do
                                                               $GREP -iq $i conftest.err
                                                               if test "$?" = "0" ; then
                                                                   ocoms_cv_cxx_wno_long_double="no",
                                                                   break;
                                                               fi
                                                           done
                                                       fi
                                                   ],
                                                   [ocoms_cv_cxx_wno_long_double="no"])],
                                   [ocoms_cv_cxx_wno_long_double="no"])])
        CXXFLAGS="$CXXFLAGS_orig"
        AC_LANG_POP(C++)
        if test "$ocoms_cv_cxx_wno_long_double" = "yes" ; then
            add="$add -Wno-long-double"
        fi

        CXXFLAGS="$CXXFLAGS $add"
        OCOMS_UNIQ(CXXFLAGS)
        if test "$add" != "" ; then
            AC_MSG_WARN([$add has been added to CXXFLAGS (--enable-picky)])
        fi
        unset add
    fi

    # See if this version of g++ allows -finline-functions
    if test "$GXX" = "yes"; then
        CXXFLAGS_orig="$CXXFLAGS"
        CXXFLAGS="$CXXFLAGS -finline-functions"
        add=
        AC_CACHE_CHECK([if $CXX supports -finline-functions],
                   [ocoms_cv_cxx_finline_functions],
                   [AC_TRY_COMPILE([], [],
                                   [ocoms_cv_cxx_finline_functions="yes"],
                                   [ocoms_cv_cxx_finline_functions="no"])])
        if test "$ocoms_cv_cxx_finline_functions" = "yes" ; then
            add=" -finline-functions"
        fi
        CXXFLAGS="$CXXFLAGS_orig$add"
        OCOMS_UNIQ(CXXFLAGS)
        if test "$add" != "" ; then
            AC_MSG_WARN([$add has been added to CXXFLAGS])
        fi
        unset add
    fi

    # Make sure we can link with the C compiler
    if[ test "$ocoms_cv_cxx_compiler_vendor" != "microsoft" ]; then
      OCOMS_LANG_LINK_WITH_C([C++], [],
        [cat <<EOF >&2
**********************************************************************
* It appears that your C++ compiler is unable to link against object
* files created by your C compiler.  This generally indicates either
* a conflict between the options specified in CFLAGS and CXXFLAGS
* or a problem with the local compiler installation.  More
* information (including exactly what command was given to the 
* compilers and what error resulted when the commands were executed) is
* available in the config.log file in this directory.
**********************************************************************
EOF
         AC_MSG_ERROR([C and C++ compilers are not link compatible.  Can not continue.])])
    fi

    # If we are on HP-UX, ensure that we're using aCC
    case "$host" in
    *hpux*)
        if test "$BASECXX" = "CC"; then
            AC_MSG_WARN([*** You will probably have problems compiling the MPI 2])
            AC_MSG_WARN([*** C++ bindings with the HP-UX CC compiler.  You should])
            AC_MSG_WARN([*** probably be using the aCC compiler.  Re-run configure])
            AC_MSG_WARN([*** with the environment variable "CXX=aCC".])
        fi
        ;;
    esac

    # Note: gcc-imperonating compilers accept -O3
    if test "$WANT_DEBUG" = "1"; then
        OPTFLAGS=
    else
        if test "$GXX" = yes; then
            OPTFLAGS="-O3"
        else
            OPTFLAGS="-O"
        fi
    fi

    # config/ocoms_ensure_contains_optflags.m4
    OCOMS_ENSURE_CONTAINS_OPTFLAGS(["$CXXFLAGS"])
    AC_MSG_CHECKING([for C++ optimization flags])
    AC_MSG_RESULT([$co_result])
    CXXFLAGS="$co_result"

    # bool type size and alignment
    AC_LANG_PUSH(C++)
    AC_CHECK_SIZEOF(bool)
    OCOMS_C_GET_ALIGNMENT(bool, OCOMS_ALIGNMENT_CXX_BOOL)
    AC_LANG_POP(C++)
])
