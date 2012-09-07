dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
dnl                         University Research and Technology
dnl                         Corporation.  All rights reserved.
dnl Copyright (c) 2004-2005 The University of Tennessee and The University
dnl                         of Tennessee Research Foundation.  All rights
dnl                         reserved.
dnl Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
dnl                         University of Stuttgart.  All rights reserved.
dnl Copyright (c) 2004-2005 The Regents of the University of California.
dnl                         All rights reserved.
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

# _CCS_C_WEAK_SYMBOLS(action_if_found, [action_if_not_found])
# ------------------------------------------------------------
AC_DEFUN([_CCS_C_WEAK_SYMBOLS],[
    # need two files because icc will incorrectly not create the
    # symbols if they are not used in the object file in which they
    # are defined.  Blah!
    # To get to compile with icc, have them in a separate header.
    cat > conftest_weak.h <<EOF
int real(int i);
int fake(int i);
EOF

    cat > conftest_weak.c <<EOF
#include "conftest_weak.h"
#pragma weak fake = real
int real(int i) { return i; }
EOF

    cat > conftest.c <<EOF
#include "conftest_weak.h"
int main() { return fake(3); }
EOF

# Try the compile
CCS_LOG_COMMAND(
    [$CC $CFLAGS  -c conftest_weak.c],
    CCS_LOG_COMMAND(
        [$CC $CFLAGS  conftest.c conftest_weak.o -o conftest $LDFLAGS $LIBS],
        [ccs_c_weak_symbols_happy=1],
	[ccs_c_weak_symbols_happy=0]),
    [ccs_c_weak_symbols_happy=0])

    AS_IF([test "$ccs_c_weak_symbols_happy" = "1"], [$1], [$2])

    unset ccs_c_weak_symbols_happy
    rm -f conftest_weak.h conftest_weak.c conftest.c conftest
])


# CCS_C_WEAK_SYMBOLS()
# ---------------------
# sets CCS_C_WEAK_SYMBOLS=1 if C compiler has support for weak symbols
AC_DEFUN([CCS_C_WEAK_SYMBOLS],[
    AC_CACHE_CHECK([for weak symbol support],
                   [ccs_cv_c_weak_symbols],
                   [_CCS_C_WEAK_SYMBOLS([ccs_cv_c_weak_symbols="yes"],
                                         [ccs_cv_c_weak_symbols="no"])])

    AS_IF([test "$ccs_cv_c_weak_symbols" = "yes"],
          [CCS_C_HAVE_WEAK_SYMBOLS=1], [CCS_C_HAVE_WEAK_SYMBOLS=0])

    AC_DEFINE_UNQUOTED(CCS_HAVE_WEAK_SYMBOLS, $CCS_C_HAVE_WEAK_SYMBOLS,
          [Whether we have weak symbols or not])
]) dnl
