dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
dnl
dnl Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
dnl Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl


# OCOMS_CHECK_COMPILER_VERSION_ID()
# ----------------------------------------------------
# Try to figure out the compiler's name and version to detect cases,
# where users compile Open MPI with one version and compile the application
# with a different compiler.
#
AC_DEFUN([OCOMS_CHECK_COMPILER_VERSION_ID],
[
    OCOMS_CHECK_COMPILER(FAMILYID)
    OCOMS_CHECK_COMPILER_STRINGIFY(FAMILYNAME)
    OCOMS_CHECK_COMPILER(VERSION)
    OCOMS_CHECK_COMPILER_STRINGIFY(VERSION_STR)
])dnl


AC_DEFUN([OCOMS_CHECK_COMPILER], [
    lower=m4_tolower($1)
    AC_CACHE_CHECK([for compiler $lower], ocoms_cv_compiler_[$1],
    [
            CPPFLAGS_orig=$CPPFLAGS
            CPPFLAGS="-I${top_ocoms_srcdir}/ocoms/include $CPPFLAGS"
            AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include "mpi_portable_platform.h"

int main (int argc, char * argv[])
{
    FILE * f;
    f=fopen("conftestval", "w");
    if (!f) exit(1);
    fprintf (f, "%d", PLATFORM_COMPILER_$1);
    return 0;
}
            ], [
                eval ocoms_cv_compiler_$1=`cat conftestval`;
            ], [
                eval ocoms_cv_compiler_$1=0
            ], [
                eval ocoms_cv_compiler_$1=0
            ])
            CPPFLAGS=$CPPFLAGS_orig
    ])
    AC_DEFINE_UNQUOTED([OCOMS_BUILD_PLATFORM_COMPILER_$1], $ocoms_cv_compiler_[$1],
                       [The compiler $lower which OMPI was built with])
])dnl


AC_DEFUN([OCOMS_CHECK_COMPILER_STRINGIFY], [
    lower=m4_tolower($1)
    AC_CACHE_CHECK([for compiler $lower], ocoms_cv_compiler_[$1],
    [
            CPPFLAGS_orig=$CPPFLAGS
            CPPFLAGS="-I${top_ocoms_srcdir}/ocoms/include $CPPFLAGS"
            AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include "mpi_portable_platform.h"

int main (int argc, char * argv[])
{
    FILE * f;
    f=fopen("conftestval", "w");
    if (!f) exit(1);
    fprintf (f, "%s", _STRINGIFY(PLATFORM_COMPILER_$1));
    return 0;
}
            ], [
                eval ocoms_cv_compiler_$1=`cat conftestval`;
            ], [
                eval ocoms_cv_compiler_$1=UNKNOWN
            ], [
                eval ocoms_cv_compiler_$1=UNKNOWN
            ])
            CPPFLAGS=$CPPFLAGS_orig
    ])
    AC_DEFINE_UNQUOTED([OCOMS_BUILD_PLATFORM_COMPILER_$1], $ocoms_cv_compiler_[$1],
                       [The compiler $lower which OMPI was built with])
])dnl
