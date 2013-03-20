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
dnl OCOMS_CONFIG_POSIX_THREADS()
dnl
dnl Configure posix threads, setting the following variables (but
dnl  not calling AC_SUBST on them).

# ********************************************************************
#
# Internal macros - do not call from outside OCOMS_CONFIG_POSIX_THREADS
#
# ********************************************************************


AC_DEFUN([OCOMS_INTL_PTHREAD_TRY_LINK], [
# BEGIN: OCOMS_INTL_PTHREAD_TRY_LINK
#
# Make sure that we can run a small application in C or C++, which
# ever is the current language.  Do make sure that C or C++ is the
# current language.
#
# As long as this is not being run....
# pthread_t may be anything from an int to a struct -- init with self-tid.
#
    AC_TRY_LINK([#include <pthread.h>],
                 [pthread_t th=pthread_self(); pthread_join(th, 0);
                 pthread_attr_init(0); pthread_cleanup_push(0, 0);
                 pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                 [$1], [$2])
# END: OCOMS_INTL_PTHREAD_TRY_LINK
])dnl


# ********************************************************************
#
# Try to compile thread support without any special flags
#
# ********************************************************************
AC_DEFUN([OCOMS_INTL_POSIX_THREADS_PLAIN_C], [
#
# C compiler
#
if test "$ocoms_pthread_c_success" = "0"; then
  AC_MSG_CHECKING([if C compiler and POSIX threads work as is])
  if test "$HAVE_POSIX_THREADS" = "1" ; then
    run_this_test=1
  else
    case "${host_cpu}-${host_os}" in
      *solaris*)
        AC_MSG_RESULT([no - Solaris, not checked])
        run_this_test=0
      ;;
      *-aix* | *-freebsd*)
        if test "`echo $CPPFLAGS | $GREP 'D_THREAD_SAFE'`" = ""; then
          PTHREAD_CPPFLAGS="-D_THREAD_SAFE"
          CPPFLAGS="$CPPFLAGS $PTHREAD_CPPFLAGS"
        fi
        run_this_test=1
      ;;
      *)
        if test "`echo $CPPFLAGS | $GREP 'D_REENTRANT'`" = ""; then
          PTHREAD_CPPFLAGS="-D_REENTRANT"
          CPPFLAGS="$CPPFLAGS $PTHREAD_CPPFLAGS"
        fi
        run_this_test=1
      ;;
    esac
  fi

  if test "$run_this_test" = "1" ; then
    AC_LANG_PUSH(C)
    OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_c_success=1,
                              ocoms_pthread_c_success=0)
    AC_LANG_POP(C)
    if test "$ocoms_pthread_c_success" = "1"; then
      AC_MSG_RESULT([yes])
    else
      PTHREAD_CPPFLAGS=
      CPPFLAGS="$orig_CPPFLAGS"
      AC_MSG_RESULT([no])
    fi
  fi
fi
])dnl


AC_DEFUN([OCOMS_INTL_POSIX_THREADS_PLAIN_CXX], [
#
# C++ compiler
#
if test "$ocoms_pthread_cxx_success" = "0"; then
  AC_MSG_CHECKING([if C++ compiler and POSIX threads work as is])
  if test "$HAVE_POSIX_THREADS" = "1" ; then
    run_this_test=1
  else
    case "${host_cpu}-${host_os}" in
      *solaris*)
        AC_MSG_RESULT([no - Solaris, not checked])
        run_this_test=0
      ;;
      *-aix* | *-freebsd*)
        if test "`echo $CXXCPPFLAGS | $GREP 'D_THREAD_SAFE'`" = ""; then
          PTHREAD_CXXCPPFLAGS="-D_THREAD_SAFE"
          CXXCPPFLAGS="$CXXCPPFLAGS $PTHREAD_CXXCPPFLAGS"
        fi
        run_this_test=1
      ;;
      *)
        if test "`echo $CXXCPPFLAGS | $GREP 'D_REENTRANT'`" = ""; then
          PTHREAD_CXXCPPFLAGS="-D_REENTRANT"
          CXXCPPFLAGS="$CXXCPPFLAGS $PTHREAD_CXXCPPFLAGS"
        fi
        run_this_test=1
      ;;
    esac
  fi

  if test "$run_this_test" = "1" ; then
    AC_LANG_PUSH(C++)
    OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_cxx_success=1, 
                              ocoms_pthread_cxx_success=0)
    AC_LANG_POP(C++)
    if test "$ocoms_pthread_cxx_success" = "1"; then
      AC_MSG_RESULT([yes])
    else
      PTHREAD_CXXCPPFLAGS=
      CXXCPPFLAGS="$orig_CXXCPPFLAGS"
      AC_MSG_RESULT([no])
    fi
  fi
fi
])dnl

AC_DEFUN([OCOMS_INTL_POSIX_THREADS_PLAIN], [
# BEGIN: OCOMS_INTL_POSIX_THREADS_PLAIN
#
# Check if can compile without any special flags
# we throw -D_REENTRANT or -D_THREAD_SAFE in here, just in
# case.  Some systems (OS X, for example) generally don't need
# the defines, but then will on one system header here or there
# why take chances?
#

# Only run C++ and Fortran if those compilers already configured
AC_PROVIDE_IFELSE([AC_PROG_CC],
                  [OCOMS_INTL_POSIX_THREADS_PLAIN_C],
                  [ocoms_pthread_c_success=1])

AC_PROVIDE_IFELSE([AC_PROG_CXX], 
                  [OCOMS_INTL_POSIX_THREADS_PLAIN_CXX], 
                  [ocoms_pthread_cxx_success=1])

# End: OCOMS_INTL_POSIX_THREADS_PLAIN
])dnl


# ********************************************************************
#
# Try to compile thread support with special compiler flags
#
# ********************************************************************
AC_DEFUN([OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS_C], [
#
# C compiler
#
if test "$ocoms_pthread_c_success" = "0"; then
  for pf in $pflags; do
    AC_MSG_CHECKING([if C compiler and POSIX threads work with $pf])
    CFLAGS="$orig_CFLAGS $pf"
    AC_LANG_PUSH(C)
    OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_c_success=1,
                              ocoms_pthread_c_success=0)
    AC_LANG_POP(C)
    if test "$ocoms_pthread_c_success" = "1"; then
      PTHREAD_CFLAGS="$pf"
      AC_MSG_RESULT([yes])
      break
    else
      PTHREAD_CFLAGS=
      CFLAGS="$orig_CFLAGS"
      AC_MSG_RESULT([no])
    fi
  done
fi
])


AC_DEFUN([OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS_CXX], [
#
# C++ compiler
#
if test "$ocoms_pthread_cxx_success" = "0"; then
  for pf in $pflags; do
    AC_MSG_CHECKING([if C++ compiler and POSIX threads work with $pf])
    CXXFLAGS="$orig_CXXFLAGS $pf"
    AC_LANG_PUSH(C++)
    OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_cxx_success=1,
                              ocoms_pthread_cxx_success=0)
    AC_LANG_POP(C++)
    if test "$ocoms_pthread_cxx_success" = "1"; then
      PTHREAD_CXXFLAGS="$pf"
      AC_MSG_RESULT([yes])
      break
    else
      PTHREAD_CXXFLAGS=
      CXXFLAGS="$orig_CXXFLAGS"
      AC_MSG_RESULT([no])
    fi
  done
fi
])

AC_DEFUN([OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS],[
# Begin: OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS
#
# If above didn't work, try some super-special compiler flags 
# that get evaluated to the "right" things.
#
# -Kthread:
# -kthread:  FreeBSD kernel threads
# -pthread:  Modern GCC (most all platforms)
# -pthreads: GCC on solaris
# -mthreads:
# -mt:       Solaris native compilers / HP-UX aCC
#
# Put -mt before -mthreads because HP-UX aCC will properly compile
# with -mthreads (reading as -mt), but emit a warning about unknown
# flags hreads.  Stupid compilers.

case "${host_cpu}-${host_os}" in
  *solaris*)
    pflags="-pthread -pthreads -mt"
  ;;
  *)
    pflags="-Kthread -kthread -pthread -pthreads -mt -mthreads"
  ;;
esac

# Only run C++ and Fortran if those compilers already configured
AC_PROVIDE_IFELSE([AC_PROG_CC], 
                  [OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS_C],
                  [ocoms_pthread_c_success=1])

AC_PROVIDE_IFELSE([AC_PROG_CXX], 
                  [OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS_CXX], 
                  [ocoms_pthread_cxx_success=1])

# End: OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS
])dnl


# ********************************************************************
#
# Try to compile thread support with extra libs
#
# ********************************************************************
AC_DEFUN([OCOMS_INTL_POSIX_THREADS_LIBS_C],[
#
# C compiler
#
if test "$ocoms_pthread_c_success" = "0"; then
  for pl in $plibs; do
    AC_MSG_CHECKING([if C compiler and POSIX threads work with $pl])
    case "${host_cpu}-${host-_os}" in
      *-aix* | *-freebsd*)
        if test "`echo $CPPFLAGS | $GREP 'D_THREAD_SAFE'`" = ""; then
          PTHREAD_CPPFLAGS="-D_THREAD_SAFE"
          CPPFLAGS="$CPPFLAGS $PTHREAD_CPPFLAGS"
        fi
      ;;
      *)
        if test "`echo $CPPFLAGS | $GREP 'D_REENTRANT'`" = ""; then
          PTHREAD_CPPFLAGS="-D_REENTRANT"
          CPPFLAGS="$CPPFLAGS $PTHREAD_CPPFLAGS"
        fi
      ;;
    esac
    LIBS="$orig_LIBS $pl"
    AC_LANG_PUSH(C)
    OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_c_success=1,
                              ocoms_pthread_c_success=0)
    AC_LANG_POP(C)
    if test "$ocoms_pthread_c_success" = "1"; then
      PTHREAD_LIBS="$pl"
      AC_MSG_RESULT([yes])
    else
      PTHREAD_CPPFLAGS=
      CPPFLAGS="$orig_CPPFLAGS"
      LIBS="$orig_LIBS"
      AC_MSG_RESULT([no])
    fi
  done
fi
])dnl


AC_DEFUN([OCOMS_INTL_POSIX_THREADS_LIBS_CXX],[
#
# C++ compiler
#
if test "$ocoms_pthread_cxx_success" = "0"; then
  if test ! "$ocoms_pthread_c_success" = "0" -a ! "$PTHREAD_LIBS" = "" ; then
    AC_MSG_CHECKING([if C++ compiler and POSIX threads work with $PTHREAD_LIBS])
    case "${host_cpu}-${host-_os}" in
      *-aix* | *-freebsd*)
        if test "`echo $CXXCPPFLAGS | $GREP 'D_THREAD_SAFE'`" = ""; then
          PTHREAD_CXXCPPFLAGS="-D_THREAD_SAFE"
          CXXCPPFLAGS="$CXXCPPFLAGS $PTHREAD_CXXCPPFLAGS"
        fi
      ;;
      *)
        if test "`echo $CXXCPPFLAGS | $GREP 'D_REENTRANT'`" = ""; then
          PTHREAD_CXXCPPFLAGS="-D_REENTRANT"
          CXXCPPFLAGS="$CXXCPPFLAGS $PTHREAD_CXXCPPFLAGS"
        fi
      ;;
    esac
    LIBS="$orig_LIBS $PTHREAD_LIBS"
    AC_LANG_PUSH(C++)
    OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_cxx_success=1, 
                              ocoms_pthread_cxx_success=0)
    AC_LANG_POP(C++)
    if test "$ocoms_pthread_cxx_success" = "1"; then
      AC_MSG_RESULT([yes])
    else
      CXXCPPFLAGS="$orig_CXXCPPFLAGS"
      LIBS="$orig_LIBS"
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([Can not find working threads configuration.  aborting])
    fi
  else 
    for pl in $plibs; do
      AC_MSG_CHECKING([if C++ compiler and POSIX threads work with $pl])
      case "${host_cpu}-${host-_os}" in
        *-aix* | *-freebsd*)
          if test "`echo $CXXCPPFLAGS | $GREP 'D_THREAD_SAFE'`" = ""; then
            PTHREAD_CXXCPPFLAGS="-D_THREAD_SAFE"
            CXXCPPFLAGS="$CXXCPPFLAGS $PTHREAD_CXXCPPFLAGS"
          fi
        ;;
        *)
          if test "`echo $CXXCPPFLAGS | $GREP 'D_REENTRANT'`" = ""; then
            PTHREAD_CXXCPPFLAGS="-D_REENTRANT"
            CXXCPPFLAGS="$CXXCPPFLAGS $PTHREAD_CXXCPPFLAGS"
          fi
        ;;
      esac
      LIBS="$orig_LIBS $pl"
      AC_LANG_PUSH(C++)
      OCOMS_INTL_PTHREAD_TRY_LINK(ocoms_pthread_cxx_success=1, 
                                ocoms_pthread_cxx_success=0)
      AC_LANG_POP(C++)
      if test "$ocoms_pthread_cxx_success" = "1"; then
	PTHREAD_LIBS="$pl"
        AC_MSG_RESULT([yes])
      else
        PTHREAD_CXXCPPFLAGS=
        CXXCPPFLAGS="$orig_CXXCPPFLAGS"
        LIBS="$orig_LIBS"
        AC_MSG_RESULT([no])
      fi
    done
  fi
fi
])dnl

AC_DEFUN([OCOMS_INTL_POSIX_THREADS_LIBS],[
# Begin: OCOMS_INTL_POSIX_THREADS_LIBS
#
# if we can't find a super-special compiler flags, try some libraries.
# we throw -D_REENTRANT or -D_THREAD_SAFE in here, just in case.  Some
# systems (OS X, for example) generally don't need the defines, but
# then will on one system header here or there why take chances?
#
# libpthreads: AIX - must check before libpthread
# liblthread:  LinuxThreads on FreeBSD
# libpthread:  The usual place (like we can define usual!)
plibs="-lpthreads -llthread -lpthread"

# Only run C++ and Fortran if those compilers already configured
AC_PROVIDE_IFELSE([AC_PROG_CC], 
                  [OCOMS_INTL_POSIX_THREADS_LIBS_C], 
                  [ocoms_pthread_c_success=1])

AC_PROVIDE_IFELSE([AC_PROG_CXX], 
                  [OCOMS_INTL_POSIX_THREADS_LIBS_CXX], 
                  [ocoms_pthread_cxx_success=1])

# End: OCOMS_INTL_POSIX_THREADS_LIBS]
)dnl


#********************************************************************
#
# External macro (aka, the real thing)
#
#********************************************************************
AC_DEFUN([OCOMS_CONFIG_POSIX_THREADS],[
    AC_REQUIRE([AC_PROG_GREP])

ocoms_pthread_c_success=0
ocoms_pthread_cxx_success=0

orig_CFLAGS="$CFLAGS"
orig_FFLAGS="$FFLAGS"
orig_CXXFLAGS="$CXXFLAGS"
orig_CPPFLAGS="$CPPFLAGS"
orig_CXXCPPFLAGS="$CXXCPPFLAGS"
orig_LDFLAGS="$LDFLAGS"
orig_LIBS="$LIBS"

PTHREAD_CFLAGS=
PTHREAD_FFLAGS=
PTHREAD_CXXFLAGS=
PTHREAD_CPPFLAGS=
PTHREAD_CXXCPPFLAGS=
PTHREAD_LDFLAGS=
PTHREAD_LIBS=

# Try with the basics, mam.
OCOMS_INTL_POSIX_THREADS_PLAIN

# Try the super-special compiler flags.
OCOMS_INTL_POSIX_THREADS_SPECIAL_FLAGS

# Try the normal linking methods (that's no fun)
OCOMS_INTL_POSIX_THREADS_LIBS


#
# check to see if we can set error checking mutexes
#

# LinuxThreads
AC_MSG_CHECKING([for PTHREAD_MUTEX_ERRORCHECK_NP])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[#include <pthread.h>]],
        [[pthread_mutexattr_settype(NULL, PTHREAD_MUTEX_ERRORCHECK_NP);]])],
    [result="yes" defval=1], [result="no" defval=0])
AC_MSG_RESULT([$result])
AC_DEFINE_UNQUOTED([OCOMS_HAVE_PTHREAD_MUTEX_ERRORCHECK_NP], [$defval],
            [If PTHREADS implementation supports PTHREAD_MUTEX_ERRORCHECK_NP])

# Mac OS X
AC_MSG_CHECKING([for PTHREAD_MUTEX_ERRORCHECK])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[#include <pthread.h>]],
        [[pthread_mutexattr_settype(NULL, PTHREAD_MUTEX_ERRORCHECK);]])],
    [result="yes" defval=1], [result="no" defval=0])
AC_MSG_RESULT([$result])
AC_DEFINE_UNQUOTED([OCOMS_HAVE_PTHREAD_MUTEX_ERRORCHECK], [$defval],
            [If PTHREADS implementation supports PTHREAD_MUTEX_ERRORCHECK])

CFLAGS="$orig_CFLAGS"
FFLAGS="$orig_FFLAGS"
CXXFLAGS="$orig_CXXFLAGS"
CPPFLAGS="$orig_CPPFLAGS"
CXXCPPFLAGS="$orig_CXXCPPFLAGS"
LDFLAGS="$orig_LDFLAGS"
LIBS="$orig_LIBS"

if test "$ocoms_pthread_c_success" = "1" -a \
        "$ocoms_pthread_cxx_success" = "1"; then
  internal_useless=1
  $1
else
  internal_useless=1
  $2
fi

unset ocoms_pthread_c_success ocoms_pthread_cxx_success
unset internal_useless
])dnl
