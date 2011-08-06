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
dnl Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
dnl Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
dnl Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
dnl
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl
dnl Portions of this file derived from GASNet v1.12 (see "GASNet"
dnl comments, below)
dnl Copyright 2004,  Dan Bonachea <bonachea@cs.berkeley.edu>
dnl
dnl IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
dnl DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
dnl OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
dnl CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
dnl 
dnl THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
dnl INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
dnl AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
dnl ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
dnl PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
dnl

AC_DEFUN([CCS_CONFIGURE_SETUP],[

# Some helper script functions.  Unfortunately, we cannot use $1 kinds
# of arugments here because of the m4 substitution.  So we have to set
# special variable names before invoking the function.  :-\

ccs_show_title() {
  cat <<EOF

============================================================================
== ${1}
============================================================================
EOF
}


ccs_show_subtitle() {
  cat <<EOF

*** ${1}
EOF
}


ccs_show_subsubtitle() {
  cat <<EOF

+++ ${1}
EOF
}

ccs_show_subsubsubtitle() {
  cat <<EOF

--- ${1}
EOF
}

#
# Save some stats about this build
#

CCS_CONFIGURE_USER="`whoami`"
CCS_CONFIGURE_HOST="`hostname | head -n 1`"
CCS_CONFIGURE_DATE="`date`"

#
# Save these details so that they can be used in ccs_info later
#
AC_SUBST(CCS_CONFIGURE_USER)
AC_SUBST(CCS_CONFIGURE_HOST)
AC_SUBST(CCS_CONFIGURE_DATE)])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([CCS_BASIC_SETUP],[
#
# Save some stats about this build
#

CCS_CONFIGURE_USER="`whoami`"
CCS_CONFIGURE_HOST="`hostname | head -n 1`"
CCS_CONFIGURE_DATE="`date`"

#
# Make automake clean emacs ~ files for "make clean"
#

CLEANFILES="*~ .\#*"
AC_SUBST(CLEANFILES)

#
# See if we can find an old installation of OMPI to overwrite
#

# Stupid autoconf 2.54 has a bug in AC_PREFIX_PROGRAM -- if ccs_clean
# is not found in the path and the user did not specify --prefix,
# we'll get a $prefix of "."

ccs_prefix_save="$prefix"
AC_PREFIX_PROGRAM(ccs_clean)
if test "$prefix" = "."; then
    prefix="$ccs_prefix_save"
fi
unset ccs_prefix_save

#
# Basic sanity checking; we can't install to a relative path
#

case "$prefix" in
  /*/bin)
    prefix="`dirname $prefix`"
    echo installing to directory \"$prefix\" 
    ;;
  /*) 
    echo installing to directory \"$prefix\" 
    ;;
  NONE)
    echo installing to directory \"$ac_default_prefix\" 
    ;;
  @<:@a-zA-Z@:>@:*)
    echo installing to directory \"$prefix\" 
    ;;
  *) 
    AC_MSG_ERROR(prefix "$prefix" must be an absolute directory path) 
    ;;
esac

# Allow the --enable-dist flag to be passed in

AC_ARG_ENABLE(dist, 
    AC_HELP_STRING([--enable-dist],
		   [guarantee that that the "dist" make target will be functional, although may not guarantee that any other make target will be functional.]),
    CCS_WANT_DIST=yes, CCS_WANT_DIST=no)

if test "$CCS_WANT_DIST" = "yes"; then
    AC_MSG_WARN([Configuring in 'make dist' mode])
    AC_MSG_WARN([Most make targets may be non-functional!])
fi

# BEGIN: Derived from GASNet

# Suggestion from Paul Hargrove to disable --program-prefix and
# friends.  Heavily influenced by GASNet 1.12 acinclude.m4
# functionality to do the same thing (copyright listed at top of this
# file).

# echo program_prefix=$program_prefix  program_suffix=$program_suffix program_transform_name=$program_transform_name
# undo prefix autoconf automatically adds during cross-compilation
if test "$cross_compiling" = yes && test "$program_prefix" = "${target_alias}-" ; then
    program_prefix=NONE
fi
# normalize empty prefix/suffix
if test -z "$program_prefix" ; then
    program_prefix=NONE
fi
if test -z "$program_suffix" ; then
    program_suffix=NONE
fi
# undo transforms caused by empty prefix/suffix
if expr "$program_transform_name" : 's.^..$' >/dev/null || \
   expr "$program_transform_name" : 's.$$..$' >/dev/null || \
   expr "$program_transform_name" : 's.$$..;s.^..$' >/dev/null ; then
    program_transform_name="s,x,x,"
fi
if test "$program_prefix$program_suffix$program_transform_name" != "NONENONEs,x,x," ; then
    AC_MSG_WARN([*** The Open MPI configure script does not support --program-prefix, --program-suffix or --program-transform-name. Users are recommended to instead use --prefix with a unique directory and make symbolic links as desired for renaming.])
    AC_MSG_ERROR([*** Cannot continue])
fi

# END: Derived from GASNet
])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([CCS_LOG_MSG],[
# 1 is the message
# 2 is whether to put a prefix or not
if test -n "$2"; then
    echo "configure:__oline__: $1" >&5
else
    echo $1 >&5
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([CCS_LOG_FILE],[
# 1 is the filename
if test -n "$1" -a -f "$1"; then
    cat $1 >&5
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([CCS_LOG_COMMAND],[
# 1 is the command
# 2 is actions to do if success
# 3 is actions to do if fail
echo "configure:__oline__: $1" >&5
$1 1>&5 2>&1
ccs_status=$?
CCS_LOG_MSG([\$? = $ccs_status], 1)
if test "$ccs_status" = "0"; then
    unset ccs_status
    $2
else
    unset ccs_status
    $3
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([CCS_UNIQ],[
# 1 is the variable name to be uniq-ized
ccs_name=$1

# Go through each item in the variable and only keep the unique ones

ccs_count=0
for val in ${$1}; do
    ccs_done=0
    ccs_i=1
    ccs_found=0

    # Loop over every token we've seen so far

    ccs_done="`expr $ccs_i \> $ccs_count`"
    while test "$ccs_found" = "0" -a "$ccs_done" = "0"; do

	# Have we seen this token already?  Prefix the comparison with
	# "x" so that "-Lfoo" values won't be cause an error.

	ccs_eval="expr x$val = x\$ccs_array_$ccs_i"
	ccs_found=`eval $ccs_eval`

	# Check the ending condition

	ccs_done="`expr $ccs_i \>= $ccs_count`"

	# Increment the counter

	ccs_i="`expr $ccs_i + 1`"
    done

    # If we didn't find the token, add it to the "array"

    if test "$ccs_found" = "0"; then
	ccs_eval="ccs_array_$ccs_i=$val"
	eval $ccs_eval
	ccs_count="`expr $ccs_count + 1`"
    else
	ccs_i="`expr $ccs_i - 1`"
    fi
done

# Take all the items in the "array" and assemble them back into a
# single variable

ccs_i=1
ccs_done="`expr $ccs_i \> $ccs_count`"
ccs_newval=
while test "$ccs_done" = "0"; do
    ccs_eval="ccs_newval=\"$ccs_newval \$ccs_array_$ccs_i\""
    eval $ccs_eval

    ccs_eval="unset ccs_array_$ccs_i"
    eval $ccs_eval

    ccs_done="`expr $ccs_i \>= $ccs_count`"
    ccs_i="`expr $ccs_i + 1`"
done

# Done; do the assignment

ccs_newval="`echo $ccs_newval`"
ccs_eval="$ccs_name=\"$ccs_newval\""
eval $ccs_eval

# Clean up

unset ccs_name ccs_i ccs_done ccs_newval ccs_eval ccs_count])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

# Macro that serves as an alternative to using `which <prog>`. It is
# preferable to simply using `which <prog>` because backticks (`) (aka
# backquotes) invoke a sub-shell which may source a "noisy"
# ~/.whatever file (and we do not want the error messages to be part
# of the assignment in foo=`which <prog>`). This macro ensures that we
# get a sane executable value.
AC_DEFUN([CCS_WHICH],[
# 1 is the variable name to do "which" on
# 2 is the variable name to assign the return value to

CCS_VAR_SCOPE_PUSH([ccs_prog ccs_file ccs_dir ccs_sentinel])

ccs_prog=$1

IFS_SAVE=$IFS
IFS="$PATH_SEPARATOR"
for ccs_dir in $PATH; do
    if test -x "$ccs_dir/$ccs_prog"; then
        $2="$ccs_dir/$ccs_prog"
        break
    fi
done
IFS=$IFS_SAVE

CCS_VAR_SCOPE_POP
])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

# Declare some variables; use CCS_VAR_SCOPE_END to ensure that they
# are cleaned up / undefined.
AC_DEFUN([CCS_VAR_SCOPE_PUSH],[

    # Is the private index set?  If not, set it.
    if test "x$ccs_scope_index" = "x"; then
        ccs_scope_index=1
    fi

    # First, check to see if any of these variables are already set.
    # This is a simple sanity check to ensure we're not already
    # overwriting pre-existing variables (that have a non-empty
    # value).  It's not a perfect check, but at least it's something.
    for ccs_var in $1; do
        ccs_str="ccs_str=\"\$$ccs_var\""
        eval $ccs_str

        if test "x$ccs_str" != "x"; then
            AC_MSG_WARN([Found configure shell variable clash!])
            AC_MSG_WARN([[CCS_VAR_SCOPE_PUSH] called on "$ccs_var",])
            AC_MSG_WARN([but it is already defined with value "$ccs_str"])
            AC_MSG_WARN([This usually indicates an error in configure.])
            AC_MSG_ERROR([Cannot continue])
        fi
    done

    # Ok, we passed the simple sanity check.  Save all these names so
    # that we can unset them at the end of the scope.
    ccs_str="ccs_scope_$ccs_scope_index=\"$1\""
    eval $ccs_str
    unset ccs_str

    env | grep ccs_scope
    ccs_scope_index=`expr $ccs_scope_index + 1`
])dnl

# Unset a bunch of variables that were previously set
AC_DEFUN([CCS_VAR_SCOPE_POP],[
    # Unwind the index
    ccs_scope_index=`expr $ccs_scope_index - 1`
    ccs_scope_test=`expr $ccs_scope_index \> 0`
    if test "$ccs_scope_test" = "0"; then
        AC_MSG_WARN([[CCS_VAR_SCOPE_POP] popped too many OMPI configure scopes.])
        AC_MSG_WARN([This usually indicates an error in configure.])
        AC_MSG_ERROR([Cannot continue])
    fi

    # Get the variable names from that index
    ccs_str="ccs_str=\"\$ccs_scope_$ccs_scope_index\""
    eval $ccs_str

    # Iterate over all the variables and unset them all
    for ccs_var in $ccs_str; do
        unset $ccs_var
    done
])dnl


dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

#
# CCS_WITH_OPTION_MIN_MAX_VALUE(NAME,DEFAULT_VALUE,LOWER_BOUND,UPPER_BOUND)
# Defines a variable CCS_MAX_xxx, with "xxx" being specified as parameter $1 as "variable_name".
# If not set at configure-time using --with-max-xxx, the default-value ($2) is assumed.
# If set, value is checked against lower (value >= $3) and upper bound (value <= $4)
#
AC_DEFUN([CCS_WITH_OPTION_MIN_MAX_VALUE], [
    max_value=[$2]
    AC_MSG_CHECKING([maximum length of ]m4_translit($1, [_], [ ]))
    AC_ARG_WITH([max-]m4_translit($1, [_], [-]),
        AC_HELP_STRING([--with-max-]m4_translit($1, [_], [-])[=VALUE],
                       [maximum length of ]m4_translit($1, [_], [ ])[s.  VALUE argument has to be specified (default: [$2]).]))
    if test ! -z "$with_max_[$1]" -a "$with_max_[$1]" != "no" ; then
        # Ensure it's a number (hopefully an integer!), and >0
        expr $with_max_[$1] + 1 > /dev/null 2> /dev/null
        AS_IF([test "$?" != "0"], [happy=0],
              [AS_IF([test $with_max_[$1] -ge $3 -a $with_max_[$1] -le $4],
                     [happy=1], [happy=0])])

        # If badness in the above tests, bail
        AS_IF([test "$happy" = "0"],
              [AC_MSG_RESULT([bad value ($with_max_[$1])])
               AC_MSG_WARN([--with-max-]m4_translit($1, [_], [-])[s value must be >= $3 and <= $4])
               AC_MSG_ERROR([Cannot continue])])
        max_value=$with_max_[$1]
    fi
    AC_MSG_RESULT([$max_value])
    AC_DEFINE_UNQUOTED([CCS_MAX_]m4_toupper($1), $max_value,
                       [Maximum length of ]m4_translit($1, [_], [ ])[s (default is $2)])
    [CCS_MAX_]m4_toupper($1)=$max_value
    AC_SUBST([CCS_MAX_]m4_toupper($1))
])dnl
