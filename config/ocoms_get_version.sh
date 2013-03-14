#!/bin/sh
#
# ocoms_get_version is created from ocoms_get_version.m4 and ocoms_get_version.m4sh.
#
# Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2008      Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#



# OCOMS_GET_VERSION(version_file, variable_prefix)
# -----------------------------------------------
# parse version_file for version information, setting
# the following shell variables:
#
#  prefix_VERSION
#  prefix_BASE_VERSION
#  prefix_MAJOR_VERSION
#  prefix_MINOR_VERSION
#  prefix_RELEASE_VERSION
#  prefix_GREEK_VERSION
#  prefix_WANT_SVN
#  prefix_SVN_R
#  prefix_RELEASE_DATE



srcfile="$1"
option="$2"

case "$option" in
    # svnversion can take a while to run.  If we don't need it, don't run it.
    --major|--minor|--release|--greek|--base|--help)
        ocoms_ver_need_svn=0
        ;;
    *)
        ocoms_ver_need_svn=1
esac


if test -z "$srcfile"; then
    option="--help"
else

    : ${ocoms_ver_need_svn=1}
    : ${svnversion_result=-1}

        if test -f "$srcfile"; then
        srcdir=`dirname $srcfile`
        ocoms_vers=`sed -n "
	t clear
	: clear
	s/^major/OCOMS_MAJOR_VERSION/
	s/^minor/OCOMS_MINOR_VERSION/
	s/^release/OCOMS_RELEASE_VERSION/
	s/^greek/OCOMS_GREEK_VERSION/
	s/^want_svn/OCOMS_WANT_SVN/
	s/^svn_r/OCOMS_SVN_R/
	s/^date/OCOMS_RELEASE_DATE/
	t print
	b
	: print
	p" < "$srcfile"`
	eval "$ocoms_vers"

        # Only print release version if it isn't 0
        if test $OCOMS_RELEASE_VERSION -ne 0 ; then
            OCOMS_VERSION="$OCOMS_MAJOR_VERSION.$OCOMS_MINOR_VERSION.$OCOMS_RELEASE_VERSION"
        else
            OCOMS_VERSION="$OCOMS_MAJOR_VERSION.$OCOMS_MINOR_VERSION"
        fi
        OCOMS_VERSION="${OCOMS_VERSION}${OCOMS_GREEK_VERSION}"
        OCOMS_BASE_VERSION=$OCOMS_VERSION

        if test $OCOMS_WANT_SVN -eq 1 && test $ocoms_ver_need_svn -eq 1 ; then
            if test "$svnversion_result" != "-1" ; then
                OCOMS_SVN_R=$svnversion_result
            fi
            if test "$OCOMS_SVN_R" = "-1" ; then

                d=`date '+%m-%d-%Y'`
                if test -d "$srcdir/.svn" ; then
                    OCOMS_SVN_R=r`svnversion "$srcdir"`
                    if test $? != 0; then
                        OCOMS_SVN_R="unknown svn version (svnversion not found); $d"
                    fi
                elif test -d "$srcdir/.hg" ; then
                    # Check to see if we can find the hg command
                    # (remember that $? reflects the status of the
                    # *last* command in a pipe change, so if "hg .. |
                    # cut ..." runs and "hg" is not found, $? will
                    # reflect the status of "cut", not hg not being
                    # found.  So test for hg specifically first.
                    hg --version > /dev/null 2>&1
                    if test $? = 0; then
                        OCOMS_SVN_R=hg`hg -v -R "$srcdir" tip | grep ^changeset: | head -n 1 | cut -d: -f3`
                    else
                        OCOMS_SVN_R="unknown hg version (hg not found); $d"
                    fi
                fi
                if test "OCOMS_SVN_R" = ""; then
                    OCOMS_SVN_R="svn$d"
                fi

            fi
            OCOMS_VERSION="${OCOMS_VERSION}${OCOMS_SVN_R}"
        fi
    fi


    if test "$option" = ""; then
	option="--full"
    fi
fi

case "$option" in
    --full|-v|--version)
	echo $OCOMS_VERSION
	;;
    --major)
	echo $OCOMS_MAJOR_VERSION
	;;
    --minor)
	echo $OCOMS_MINOR_VERSION
	;;
    --release)
	echo $OCOMS_RELEASE_VERSION
	;;
    --greek)
	echo $OCOMS_GREEK_VERSION
	;;
    --svn)
	echo $OCOMS_SVN_R
	;;
    --base)
        echo $OCOMS_BASE_VERSION
        ;;
    --release-date)
        echo $OCOMS_RELEASE_DATE
        ;;
    --all)
        echo ${OCOMS_VERSION} ${OCOMS_MAJOR_VERSION} ${OCOMS_MINOR_VERSION} ${OCOMS_RELEASE_VERSION} ${OCOMS_GREEK_VERSION} ${OCOMS_SVN_R}
        ;;
    -h|--help)
	cat <<EOF
$0 <srcfile> <option>

<srcfile> - Text version file
<option>  - One of:
    --full         - Full version number
    --major        - Major version number
    --minor        - Minor version number
    --release      - Release version number
    --greek        - Greek (alpha, beta, etc) version number
    --svn          - Subversion repository number
    --all          - Show all version numbers, separated by :
    --base         - Show base version number (no svn number)
    --release-date - Show the release date
    --help         - This message
EOF
        ;;
    *)
        echo "Unrecognized option $option.  Run $0 --help for options"
        ;;
esac

# All done

exit 0
