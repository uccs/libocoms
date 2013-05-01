dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2007 Sun Microsystems, Inc.  All rights reserved.
dnl Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
dnl Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl
dnl defines:
dnl   OCOMS_$1_USE_PRAGMA_IDENT
dnl   OCOMS_$1_USE_IDENT
dnl   OCOMS_$1_USE_CONST_CHAR_IDENT
dnl

# OCOMS_CHECK_IDENT(compiler-env, compiler-flags,
# file-suffix, lang) Try to compile a source file containing
# a #pragma ident, and determine whether the ident was
# inserted into the resulting object file
# -----------------------------------------------------------
AC_DEFUN([OCOMS_CHECK_IDENT], [
    AC_MSG_CHECKING([for $4 ident string support])

    ocoms_pragma_ident_happy=0
    ocoms_ident_happy=0
    ocoms_static_const_char_happy=0
    _OCOMS_CHECK_IDENT(
        [$1], [$2], [$3],
        [[#]pragma ident], [],
        [ocoms_pragma_ident_happy=1
         ocoms_message="[#]pragma ident"],
        _OCOMS_CHECK_IDENT(
            [$1], [$2], [$3],
            [[#]ident], [],
            [ocoms_ident_happy=1
             ocoms_message="[#]ident"],
            _OCOMS_CHECK_IDENT(
                [$1], [$2], [$3],
                [[#]pragma comment(exestr, ], [)],
                [ocoms_pragma_comment_happy=1
                 ocoms_message="[#]pragma comment"],
                [ocoms_static_const_char_happy=1
                 ocoms_message="static const char[[]]"])))

    AC_DEFINE_UNQUOTED([OCOMS_$1_USE_PRAGMA_IDENT],
        [$ocoms_pragma_ident_happy], [Use #pragma ident strings for $4 files])
    AC_DEFINE_UNQUOTED([OCOMS_$1_USE_IDENT],
        [$ocoms_ident_happy], [Use #ident strings for $4 files])
    AC_DEFINE_UNQUOTED([OCOMS_$1_USE_PRAGMA_COMMENT],
        [$ocoms_pragma_comment_happy], [Use #pragma comment for $4 files])
    AC_DEFINE_UNQUOTED([OCOMS_$1_USE_CONST_CHAR_IDENT],
        [$ocoms_static_const_char_happy], [Use static const char[] strings for $4 files])

    AC_MSG_RESULT([$ocoms_message])

    unset ocoms_pragma_ident_happy ocoms_ident_happy ocoms_static_const_char_happy ocoms_message
])

# _OCOMS_CHECK_IDENT(compiler-env, compiler-flags,
# file-suffix, header_prefix, header_suffix, action-if-success, action-if-fail)
# Try to compile a source file containing a #-style ident,
# and determine whether the ident was inserted into the
# resulting object file
# -----------------------------------------------------------
AC_DEFUN([_OCOMS_CHECK_IDENT], [
    eval ocoms_compiler="\$$1"
    eval ocoms_flags="\$$2"

    ocoms_ident="string_not_coincidentally_inserted_by_the_compiler"
    cat > conftest.$3 <<EOF
$4 "$ocoms_ident" $5
int main(int argc, char** argv);
int main(int argc, char** argv) { return 0; }
EOF

    # "strings" won't always return the ident string.  objdump isn't
    # universal (e.g., OS X doesn't have it), and ...other
    # complications.  So just try to "grep" for the string in the
    # resulting object file.  If the ident is found in "strings" or
    # the grep succeeds, rule that we have this flavor of ident.

    OCOMS_LOG_COMMAND([$ocoms_compiler $ocoms_flags -c conftest.$3 -o conftest.${OBJEXT}],
                     [AS_IF([test -f conftest.${OBJEXT}],
                            [ocoms_output="`strings -a conftest.${OBJEXT} | grep $ocoms_ident`"
                             grep $ocoms_ident conftest.${OBJEXT} 2>&1 1>/dev/null
                             ocoms_status=$?
                             AS_IF([test "$ocoms_output" != "" -o "$ocoms_status" = "0"],
                                   [$6],
                                   [$7])],
                            [OCOMS_LOG_MSG([the failed program was:])
                             OCOMS_LOG_FILE([conftest.$3])
                             $7]
                            [$7])])

    unset ocoms_compiler ocoms_flags ocoms_output ocoms_status
    rm -rf conftest.* conftest${EXEEXT}
])dnl
