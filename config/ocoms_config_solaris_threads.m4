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
dnl OCOMS_CONFIG_SOLARIS_THREADS()
dnl

# ********************************************************************
#
# Internal macros - do not call from outside OCOMS_CONFIG_SOLARIS_THREADS
#
# ********************************************************************

AC_DEFUN([OCOMS_INTL_SOLARIS_TRY_LINK], [
# BEGIN: OCOMS_INTL_SOLARIS_TRY_LINK
#
# Make sure that we can run a small application in C or C++, which
# ever is the current language.  Do make sure that C or C++ is the
# current language.
    AC_TRY_LINK([#include <thread.h>],
                 [thread_t th; thr_join(th, 0, 0);
                 thr_create(0,0,0,0,0,0); ],
                 [$1], [$2])
# END: OCOMS_INTL_SOLARIS_TRY_LINK
])dnl


AC_DEFUN([OCOMS_INTL_SOLARIS_TRY_LINK_F77], [
# BEGIN: OCOMS_INTL_SOLARIS_TRY_LINK_F77
#
# Make sure that we can run a small application in Fortran, with
# pthreads living in a C object file

# Fortran module
cat > conftestf.f <<EOF
      program fpthread
      INTEGER i
      i = 1
      end
EOF

# C module
if test -f conftest.h; then
    ocoms_conftest_h="#include \"conftest.h\""
else
    ocoms_conftest_h=""
fi
cat > conftest.c <<EOF
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
$ocoms_conftest_h

#ifdef __cplusplus
extern "C" {
#endif
void ocoms_pthread()
{
  thread_t th;
  thr_join(th, 0, 0);
  thr_create(0,0,0,0,0,0);
}
#ifdef __cplusplus
}
#endif
EOF

# Try the compile
OCOMS_LOG_COMMAND(
    [$CC $CFLAGS -I. -c conftest.c],
    OCOMS_LOG_COMMAND(
        [$F77 $FFLAGS conftestf.f conftest.o -o conftest $LDFLAGS $LIBS],
        [HAPPY=1],
	[HAPPY=0]),
    [HAPPY=0])

if test "$HAPPY" = "1"; then
   $1
else
    OCOMS_LOG_MSG([here is the C program:], 1)
    OCOMS_LOG_FILE([conftest.c])
    if test -f conftest.h; then
	OCOMS_LOG_MSG([here is contest.h:], 1)
	OCOMS_LOG_FILE([conftest.h])
    fi
    OCOMS_LOG_MSG([here is the fortran program:], 1)
    OCOMS_LOG_FILE([conftestf.f])
    $2
fi

unset HAPPY ocoms_conftest_h
rm -rf conftest*
# END: OCOMS_INTL_SOLARIS_TRY_LINK_F77
])dnl


AC_DEFUN([OCOMS_CONFIG_SOLARIS_THREADS_C], [
if test "$BASECC" = "cc"; then
    STHREAD_CFLAGS="-mt"
    style="Workshop/Forte"
else
    STHREAD_CPPFLAGS="-D_REENTRANT"
    STHREAD_LIBS="-lthread"
    style="-lthread"
fi
AC_MSG_CHECKING([if C compiler and Solaris threads work])
CFLAGS="$STHREAD_CFLAGS $CFLAGS_orig"
CPPFLAGS="$STHREAD_CPPFLAGS $CPPFLAGS_orig"
LDFLAGS="$STHREAD_LDFLAGS $LDFLAGS_orig"
LIBS="$STHREAD_LIBS $LIBS_orig"
AC_LANG_PUSH(C)
OCOMS_INTL_SOLARIS_TRY_LINK(ocoms_sthread_c_success=1,
                          ocoms_sthread_c_success=0)
AC_LANG_POP(C)
if test "$ocoms_sthread_c_success" = "1"; then
    AC_MSG_RESULT([yes - $style])
else
    AC_MSG_RESULT([no])
fi
])dnl


AC_DEFUN([OCOMS_CONFIG_SOLARIS_THREADS_CXX], [
if test "$BASECXX" = "CC"; then
    STHREAD_CXXFLAGS="-mt"
    style="Workshop/Forte"
elif test "$BASECXX" = "KCC"; then
    STHREAD_CXXFLAGS="--backend -mt"
    style="KCC"
else
    STHREAD_CXXCPPFLAGS="-D_REENTRANT"
    STHREAD_LIBS="-lthread"
    style="-lthread"
fi
CXXFLAGS="$STHREAD_CXXFLAGS $CXXFLAGS_orig"
CXXCPPFLAGS="$STHREAD_CXXPPFLAGS $CXXPPFLAGS_orig"
LDFLAGS="$STHREAD_LDFLAGS $LDFLAGS_orig"
LIBS="$STHREAD_LIBS $LIBS_orig"
AC_MSG_CHECKING([if C++ compiler and Solaris threads work])
AC_LANG_PUSH(C++)
OCOMS_INTL_SOLARIS_TRY_LINK(ocoms_sthread_cxx_success=1, 
                          ocoms_sthread_cxx_success=0)
AC_LANG_POP(C++)
if test "$ocoms_sthread_cxx_success" = "1"; then
    AC_MSG_RESULT([yes - $style])
else
    AC_MSG_RESULT([no])
fi
])dnl


AC_DEFUN([OCOMS_CONFIG_SOLARIS_THREADS_FC], [
if test "$OCOMS_WANT_F77_BINDINGS" = "1"; then
    if test "$BASEFC" = "f77"; then
        STHREAD_FFLAGS="-mt"
        style="Workshop/Forte"
    else
        STHREAD_LIBS="-lthread"
        style="-lthread"
    fi
    FFLAGS="$STHREAD_FFLAGS $FFLAGS_orig"
    CFLAGS="$STHREAD_CFLAGS $CFLAGS_orig"
    CPPFLAGS="$STHREAD_CPPFLAGS $CPPFLAGS_orig"
    LDFLAGS="$STHREAD_LDFLAGS $LDFLAGS_orig"
    LIBS="$STHREAD_LIBS $LIBS_orig"
    AC_MSG_CHECKING([if F77 compiler and Solaris threads work])
    AC_LANG_PUSH(C)
    OCOMS_INTL_SOLARIS_TRY_LINK_F77(ocoms_sthread_f77_success=1, 
                                  ocoms_sthread_f77_success=0)
    AC_LANG_POP(C)
    if test "$ocoms_sthread_f77_success" = "1"; then
        AC_MSG_RESULT([yes - $style])
     else
        AC_MSG_RESULT([no])
     fi
else
  ocoms_sthread_f77_success=1
fi
])dnl


AC_DEFUN([OCOMS_CONFIG_SOLARIS_THREADS],[
ocoms_sthread_c_success=0
ocoms_sthread_f77_success=0
ocoms_sthread_cxx_success=0

orig_CFLAGS="$CFLAGS"
orig_FFLAGS="$FFLAGS"
orig_CXXFLAGS="$CXXFLAGS"
orig_CPPFLAGS="$CPPFLAGS"
orig_CXXCPPFLAGS="$CXXCPPFLAGS"
orig_LDFLAGS="$LDFLAGS"
orig_LIBS="$LIBS"

STHREAD_CFLAGS=
STHREAD_FFLAGS=
STHREAD_CXXFLAGS=
STHREAD_CPPFLAGS=
STHREAD_CXXCPPFLAGS=
STHREAD_LDFLAGS=
STHREAD_LIBS=

# Only run C++ and Fortran if those compilers already configured
AC_PROVIDE_IFELSE([AC_PROG_CC], 
                  [OCOMS_CONFIG_SOLARIS_THREADS_C],
                  [ocoms_sthread_c_success=1])

AC_PROVIDE_IFELSE([AC_PROG_CXX], 
                  [OCOMS_CONFIG_SOLARIS_THREADS_CXX],
                  [ocoms_sthread_cxx_success=1])

AC_PROVIDE_IFELSE([AC_PROG_F77], 
                  [OCOMS_CONFIG_SOLARIS_THREADS_FC],
                  [ocoms_sthread_f77_success=1])

CFLAGS="$orig_CFLAGS"
FFLAGS="$orig_FFLAGS"
CXXFLAGS="$orig_CXXFLAGS"
CPPFLAGS="$orig_CPPFLAGS"
CXXCPPFLAGS="$orig_CXXCPPFLAGS"
LDFLAGS="$orig_LDFLAGS"
LIBS="$orig_LIBS"

if test "$ocoms_sthread_c_success" = "1" -a \
        "$ocoms_sthread_cxx_success" = "1" -a \
       "$ocoms_sthread_f77_success" = "1"; then
  internal_useless=1
  $1
else
  internal_useless=1
  $2
fi

unset ocoms_sthread_c_success ocoms_sthread_f77_success ocoms_sthread_cxx_success
unset internal_useless
])dnl

