dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2007 Sun Microsystems, Inc.  All rights reserved.
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl
dnl defines:
dnl   CCS_$1_USE_PRAGMA_IDENT
dnl   CCS_$1_USE_IDENT
dnl   CCS_$1_USE_CONST_CHAR_IDENT
dnl

# CCS_CHECK_IDENT(compiler-env, compiler-flags,
# file-suffix, lang) Try to compile a source file containing
# a #pragma ident, and determine whether the ident was
# inserted into the resulting object file
# -----------------------------------------------------------
AC_DEFUN([CCS_CHECK_IDENT], [
    AC_MSG_CHECKING([for $4 ident string support])

    ccs_pragma_ident_happy=0
    ccs_ident_happy=0
    ccs_static_const_char_happy=0
    _CCS_CHECK_IDENT(
        [$1], [$2], [$3],
        [[#]pragma ident], [],
        [ccs_pragma_ident_happy=1
         ccs_message="[#]pragma ident"],
        _CCS_CHECK_IDENT(
            [$1], [$2], [$3],
            [[#]ident], [],
            [ccs_ident_happy=1
             ccs_message="[#]ident"],
            _CCS_CHECK_IDENT(
                [$1], [$2], [$3],
                [[#]pragma comment(exestr, ], [)],
                [ccs_pragma_comment_happy=1
                 ccs_message="[#]pragma comment"],
                [ccs_static_const_char_happy=1
                 ccs_message="static const char[[]]"])))

    AC_DEFINE_UNQUOTED([CCS_$1_USE_PRAGMA_IDENT],
        [$ccs_pragma_ident_happy], [Use #pragma ident strings for $4 files])
    AC_DEFINE_UNQUOTED([CCS_$1_USE_IDENT],
        [$ccs_ident_happy], [Use #ident strings for $4 files])
    AC_DEFINE_UNQUOTED([CCS_$1_USE_PRAGMA_COMMENT],
        [$ccs_pragma_comment_happy], [Use #pragma comment for $4 files])
    AC_DEFINE_UNQUOTED([CCS_$1_USE_CONST_CHAR_IDENT],
        [$ccs_static_const_char_happy], [Use static const char[] strings for $4 files])

    AC_MSG_RESULT([$ccs_message])

    unset ccs_pragma_ident_happy ccs_ident_happy ccs_static_const_char_happy ccs_message
])

# _CCS_CHECK_IDENT(compiler-env, compiler-flags,
# file-suffix, header_prefix, header_suffix, action-if-success, action-if-fail)
# Try to compile a source file containing a #-style ident,
# and determine whether the ident was inserted into the
# resulting object file
# -----------------------------------------------------------
AC_DEFUN([_CCS_CHECK_IDENT], [
    eval ccs_compiler="\$$1"
    eval ccs_flags="\$$2"

    ccs_ident="string_not_coincidentally_inserted_by_the_compiler"
    cat > conftest.$3 <<EOF
$4 "$ccs_ident" $5
int main(int argc, char** argv);
int main(int argc, char** argv) { return 0; }
EOF

    # "strings" won't always return the ident string.  objdump isn't
    # universal (e.g., OS X doesn't have it), and ...other
    # complications.  So just try to "grep" for the string in the
    # resulting object file.  If the ident is found in "strings" or
    # the grep succeeds, rule that we have this flavor of ident.

    CCS_LOG_COMMAND([$ccs_compiler $ccs_flags -c conftest.$3 -o conftest.${OBJEXT}],
                     [AS_IF([test -f conftest.${OBJEXT}],
                            [ccs_output="`strings -a conftest.${OBJEXT} | grep $ccs_ident`"
                             grep $ccs_ident conftest.${OBJEXT} 2>&1 1>/dev/null
                             ccs_status=$?
                             AS_IF([test "$ccs_output" != "" -o "$ccs_status" = "0"],
                                   [$6],
                                   [$7])],
                            [CCS_LOG_MSG([the failed program was:])
                             CCS_LOG_FILE([conftest.$3])
                             $7]
                            [$7])])

    unset ccs_compiler ccs_flags ccs_output ccs_status
    rm -rf conftest.* conftest${EXEEXT}
])dnl
