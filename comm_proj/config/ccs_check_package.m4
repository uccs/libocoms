# -*- shell-script -*-
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
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# _CCS_CHECK_PACKAGE_HEADER(prefix, header, dir-prefix,
#                            [action-if-found], [action-if-not-found])
# --------------------------------------------------------------------
AC_DEFUN([_CCS_CHECK_PACKAGE_HEADER], [
    # This is stolen from autoconf to peek under the covers to get the
    # cache variable for the library check.  one should not copy this
    # code into other places unless you want much pain and suffering
    AS_VAR_PUSHDEF([ccs_Header], [ac_cv_header_$2])

    # so this sucks, but there's no way to get through the progression
    # of header includes without killing off the cache variable and trying
    # again...  
    unset ccs_Header

    ccs_check_package_header_happy="no"
    AS_IF([test "$3" = "/usr" -o "$3" = "/usr/local"],
           [ # try as is...
            AC_VERBOSE([looking for header without includes])
            AC_CHECK_HEADER([$2], [ccs_check_package_header_happy="yes"],
                            [ccs_check_package_header_happy="no"])
            AS_IF([test "$ccs_check_package_header_happy" = "no"],
                  [# no go on the as is - reset the cache and try again
                   unset ccs_Header])])

    AS_IF([test "$ccs_check_package_header_happy" = "no"],
          [AS_IF([test "$3" != ""],
                 [$1_CPPFLAGS="$$1_CPPFLAGS -I$3/include"
                  CPPFLAGS="$CPPFLAGS -I$3/include"])
           AC_CHECK_HEADER([$2], [$4], [$5])],
          [$4])
    unset ccs_check_package_header_happy])
    
    dnl AS_VAR_POPDEF([ccs_Header])dnl
])


# _CCS_CHECK_PACKAGE_LIB(prefix, library, function, extra-libraries,
#                         dir-prefix, libdir,
#                         [action-if-found], [action-if-not-found]])
# --------------------------------------------------------------------
AC_DEFUN([_CCS_CHECK_PACKAGE_LIB], [
    # This is stolen from autoconf to peek under the covers to get the
    # cache variable for the library check.  one should not copy this
    # code into other places unless you want much pain and suffering
    AS_LITERAL_IF([$2],
                  [AS_VAR_PUSHDEF([ccs_Lib], [ac_cv_lib_$2_$3])],
                  [AS_VAR_PUSHDEF([ccs_Lib], [ac_cv_lib_$2''_$3])])dnl

    # see comment above
    unset ccs_Lib
    ccs_check_package_lib_happy="no"
    AS_IF([test "$6" != ""],
          [ # libdir was specified - search only there
           $1_LDFLAGS="$$1_LDFLAGS -L$6"
           LDFLAGS="$LDFLAGS -L$6"
           AC_CHECK_LIB([$2], [$3], 
                        [ccs_check_package_lib_happy="yes"], 
                        [ccs_check_package_lib_happy="no"], [$4])
           AS_IF([test "$ccs_check_package_lib_happy" = "no"],
                 [LDFLAGS="$ccs_check_package_$1_save_LDFLAGS"
                  $1_LDFLAGS="$ccs_check_package_$1_orig_LDFLAGS"
                  unset ccs_Lib])],
          [ # libdir was not specified - go through search path
           ccs_check_package_libdir="$5"
           AS_IF([test "$ccs_check_package_libdir" = "" -o "$ccs_check_package_libdir" = "/usr" -o "$ccs_check_package_libdir" = "/usr/local"],
               [ # try as is...
                AC_VERBOSE([looking for library without search path])
                AC_CHECK_LIB([$2], [$3], 
                        [ccs_check_package_lib_happy="yes"], 
                        [ccs_check_package_lib_happy="no"], [$4])
                AS_IF([test "$ccs_check_package_lib_happy" = "no"],
                    [ # no go on the as is..  see what happens later...
                     LDFLAGS="$ccs_check_package_$1_save_LDFLAGS"
                     $1_LDFLAGS="$ccs_check_package_$1_orig_LDFLAGS"
                     unset ccs_Lib])])

           AS_IF([test "$ccs_check_package_lib_happy" = "no"],
               [AS_IF([test "$ccs_check_package_libdir" != ""],
                    [$1_LDFLAGS="$$1_LDFLAGS -L$ccs_check_package_libdir/lib"
                     LDFLAGS="$LDFLAGS -L$ccs_check_package_libdir/lib"
                     AC_VERBOSE([looking for library in lib])
                     AC_CHECK_LIB([$2], [$3], 
                               [ccs_check_package_lib_happy="yes"], 
                               [ccs_check_package_lib_happy="no"], [$4])
                     AS_IF([test "$ccs_check_package_lib_happy" = "no"],
                         [ # no go on the as is..  see what happens later...
                          LDFLAGS="$ccs_check_package_$1_save_LDFLAGS"
                          $1_LDFLAGS="$ccs_check_package_$1_orig_LDFLAGS"
                          unset ccs_Lib])])])

           AS_IF([test "$ccs_check_package_lib_happy" = "no"],
               [AS_IF([test "$ccs_check_package_libdir" != ""],
                    [$1_LDFLAGS="$$1_LDFLAGS -L$ccs_check_package_libdir/lib64"
                     LDFLAGS="$LDFLAGS -L$ccs_check_package_libdir/lib64"
                     AC_VERBOSE([looking for library in lib64])
                     AC_CHECK_LIB([$2], [$3], 
                               [ccs_check_package_lib_happy="yes"], 
                               [ccs_check_package_lib_happy="no"], [$4])
                     AS_IF([test "$ccs_check_package_lib_happy" = "no"],
                         [ # no go on the as is..  see what happens later...
                          LDFLAGS="$ccs_check_package_$1_save_LDFLAGS"
                          $1_LDFLAGS="$ccs_check_package_$1_orig_LDFLAGS"
                          unset ccs_Lib])])])])

    AS_IF([test "$ccs_check_package_lib_happy" = "yes"],
          [$1_LIBS="-l$2 $4"
           $7], [$8])

    AS_VAR_POPDEF([ccs_Lib])dnl
])
    

# CCS_CHECK_PACKAGE(prefix, 
#                    header, 
#                    library, 
#                    function, 
#                    extra-libraries, 
#                    dir-prefix,
#                    libdir-prefix,
#                    [action-if-found], [action-if-not-found]
# -----------------------------------------------------------
# check for package defined by header and libs, and probably
# located in dir-prefix, possibly with libs in libdir-prefix.
# Both dir-prefix and libdir-prefix can be empty.  Will set
# prefix_{CPPFLAGS, LDFLAGS, LIBS} as needed
AC_DEFUN([CCS_CHECK_PACKAGE],[
    ccs_check_package_$1_save_CPPFLAGS="$CPPFLAGS"
    ccs_check_package_$1_save_LDFLAGS="$LDFLAGS"
    ccs_check_package_$1_save_LIBS="$LIBS"

    ccs_check_package_$1_orig_CPPFLAGS="$$1_CPPFLAGS"
    ccs_check_package_$1_orig_LDFLAGS="$$1_LDFLAGS"
    ccs_check_package_$1_orig_LIBS="$$1_LIBS"

    _CCS_CHECK_PACKAGE_HEADER([$1], [$2], [$6],
          [_CCS_CHECK_PACKAGE_LIB([$1], [$3], [$4], [$5], [$6], [$7],
                [ccs_check_package_happy="yes"],
                [ccs_check_package_happy="no"])],
          [ccs_check_package_happy="no"])

    AS_IF([test "$ccs_check_package_happy" = "yes"],
          [$8],
          [$1_CPPFLAGS="$ccs_check_package_$1_orig_CPPFLAGS"
           $1_LDFLAGS="$ccs_check_package_$1_orig_LDFLAGS"
           $1_LIBS="$ccs_check_package_$1_orig_LIBS"
           $9])

    CPPFLAGS="$ccs_check_package_$1_save_CPPFLAGS"
    LDFLAGS="$ccs_check_package_$1_save_LDFLAGS"
    LIBS="$ccs_check_package_$1_save_LIBS"
]) 