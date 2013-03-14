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
dnl Copyright (c) 2007-2010 Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2008      Sun Microsystems, Inc.  All rights reserved.
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

######################################################################
#
# OCOMS_CONTRIB
#
# configure the contributed software components.  Currently fairly
# hard-wired, but someday should be much more like OCOMS_MCA.  See
# https://svn.open-mpi.org/trac/ompi/ticket/1162.
#
# USAGE:
#   OCOMS_CONTRIB()
#
######################################################################
AC_DEFUN([OCOMS_CONTRIB],[
    dnl for OCOMS_CONFIGURE_USER env variable
    AC_REQUIRE([OCOMS_CONFIGURE_SETUP])

    # Option to not build some of the contributed software packages
    AC_ARG_ENABLE([contrib-no-build],
        AC_HELP_STRING([--enable-contrib-no-build=LIST],
                        [Comma-separated list of contributed package names that will not be built.  Possible values: ocoms_mpicontrib_list.  Example: "--enable-contrib-no-build=foo,bar" will disable building both the "foo" and "bar" contributed software packages (default: none -- i.e., build all possible contrib packages)]))

    # Parse the list to see what we should not build
    ocoms_show_subtitle "Configuring contributed software packages"
    AC_MSG_CHECKING([which contributed software packages should be disabled])
    if test "$enable_contrib_no_build" = "yes"; then
        AC_MSG_RESULT([yes])
        AC_MSG_ERROR([*** The enable-contrib-no-build flag requires an explicit list
*** of packages to not build.  For example, --enable-contrib-no-build=vt])
    else
        ifs_save="$IFS"
        IFS="${IFS}$PATH_SEPARATOR,"
        msg=
        for item in $enable_contrib_no_build; do
            str="`echo DISABLE_contrib_${item}=1 | sed s/-/_/g`"
            eval $str
            msg="$item $msg"
        done
        IFS="$ifs_save"
    fi
    AC_MSG_RESULT([$msg])
    unset msg

    # List of contrib subdirs to traverse into
    OCOMS_CONTRIB_SUBDIRS=
    OCOMS_CONTRIB_DIST_SUBDIRS=
    OCOMS_MPI_CONTRIBS=

    # Cycle through each of the software packages and
    # configure them if not disabled.  
    m4_foreach(software, [ocoms_mpicontrib_list],
              [_OCOMS_CONTRIB_CONFIGURE(software)])

    # Setup the top-level glue
    AC_DEFINE_UNQUOTED([OCOMS_MPI_CONTRIBS], ["$OCOMS_MPI_CONTRIBS"],
                       [Contributed software packages built with Open MPI])
    AC_SUBST(OCOMS_CONTRIB_SUBDIRS)
    AC_SUBST(OCOMS_CONTRIB_DIST_SUBDIRS)
])dnl


######################################################################
#
# _OCOMS_CONTRIB_SOFTWARE
#
# Setup a specific contributed software package.  This is a subroutine
# because the work to setup each package is essentially the same.
# Currently assumes that there is a configure.m4 file in the
# contributed software directory.  May someday be expanded to handle
# other things.
#
# USAGE:
#   _OCOMS_CONTRIB_SOFTARE([package_name])
#
######################################################################
AC_DEFUN([_OCOMS_CONTRIB_CONFIGURE],[

    ocoms_show_subsubsubtitle "$1 (m4 configuration macro)"

    # Put in a convenient enable/disable switch (it's a little more
    # user friendly than
    # --enable-contrib-no-build=<comma_delimited_list>, although each
    # works just as well as the other).
    AC_ARG_ENABLE([$1],
            [AS_HELP_STRING([--disable-$1],
                            [disable support for contributed package $1 (default: enabled)])])
    AS_IF([test "x$enable_$1" = xno], [DISABLE_contrib_$1=yes])

    OCOMS_CONTRIB_HAPPY=0
    if test "$DISABLE_contrib_$1" = "" -a "$DISABLE_contrib_all" = ""; then
        OCOMS_contrib_$1_CONFIG([OCOMS_CONTRIB_HAPPY=1], [])
        AC_MSG_CHECKING([if contributed component $1 can compile])
        if test "$OCOMS_CONTRIB_HAPPY" = "1"; then
            OCOMS_CONTRIB_SUBDIRS="$OCOMS_CONTRIB_SUBDIRS contrib/$1"
            OCOMS_CONTRIB_DIST_SUBDIRS="$OCOMS_CONTRIB_DIST_SUBDIRS contrib/$1"
            if test "$OCOMS_MPI_CONTRIBS" = ""; then
                OCOMS_MPI_CONTRIBS=$1
            else
                OCOMS_MPI_CONTRIBS="$1, $OCOMS_MPI_CONTRIBS"
            fi
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
        fi
    else
        AC_MSG_NOTICE([disabled via command line switch])
    fi
    AC_DEFINE_UNQUOTED(OCOMS_ENABLE_CONTRIB_$1, [$OCOMS_CONTRIB_HAPPY],
                       [Enable contributed software package $1])
    unset OCOMS_CONTRIB_HAPPY
])dnl
