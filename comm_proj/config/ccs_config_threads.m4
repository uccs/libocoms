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
dnl Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

AC_DEFUN([CCS_CONFIG_THREADS],[
#
# Arguments: none
#
# Dependencies: None
#
# Modifies:
#  none - see called tests
#
# configure threads
#

#
# MPI_THREAD_MULTIPLE
#
# --enable-mpi-thread-multiple
#  #if CCS_ENABLE_THREAD_MULTIPLE == 0 /* Not available */
#  #if CCS_ENABLE_THREAD_MULTIPLE == 1 /* Available  */
# 
AC_MSG_CHECKING([if want MPI_THREAD_MULTIPLE support])
AC_ARG_ENABLE([mpi_thread_multiple],
    [AC_HELP_STRING([--enable-mpi-thread-multiple],
                    [Enable MPI_THREAD_MULTIPLE support (default: disabled)])],
    [enable_mpi_threads="$enableval"],
    [enable_mpi_threads="undef"])

# if they did not want CCS thread support, then they cannot have this option
if test "$enable_mpi_threads" = "yes" -a "$enable_ccs_multi_threads" = "no"; then
    AC_MSG_WARN(["failed"])
    AC_MSG_WARN(["*** You have requested MPI_THREAD_MULTIPLE support but thread"])
    AC_MSG_WARN(["*** support was disabled within the CCS code base by"])
    AC_MSG_WARN(["*** --disable-ccs-multi-threads on your configure command."])
    AC_MSG_ERROR(["*** Can not continue."])
# if --disable-mpi-thread-multiple
elif test "$enable_mpi_threads" = "no"; then
    ccs_want_mpi_threads=0
    CCS_ENABLE_THREAD_MULTIPLE=0
    AC_MSG_RESULT([Disabled])
#if requested and CCS thread support is enabled
elif test "$enable_mpi_threads" = "yes" -a "$enable_ccs_multi_threads" = "yes" ; then
    ccs_want_mpi_threads=1
    CCS_ENABLE_THREAD_MULTIPLE=1
    AC_MSG_RESULT([Enabled])
#if requested and CCS thread support was not explicitly enabled or disabled
elif test "$enable_mpi_threads" = "yes" -a "$enable_ccs_multi_threads" = "undef" ; then
   # ensure that CCS thread support is turned on
   CCS_ENABLE_MULTI_THREADS=1
   enable_ccs_multi_threads="yes"
   AC_DEFINE_UNQUOTED([CCS_ENABLE_MULTI_THREADS], [$CCS_ENABLE_MULTI_THREADS],
                      [Whether we should enable thread support within the CCS code base])
    ccs_want_mpi_threads=1
    CCS_ENABLE_THREAD_MULTIPLE=1
    AC_MSG_RESULT([Enabled - CCS thread support automatically enabled])
else
    # Default: disable
    ccs_want_mpi_threads=0
    CCS_ENABLE_THREAD_MULTIPLE=0
    AC_MSG_RESULT([Disabled])
fi
AC_DEFINE_UNQUOTED([CCS_ENABLE_THREAD_MULTIPLE], [$ccs_want_mpi_threads],
                   [Enable MPI_THREAD_MULTIPLE])

])dnl

