#!/bin/sh
#
# ccs_get_version is created from ccs_get_version.m4 and ccs_get_version.m4sh.
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



# CCS_GET_VERSION(version_file, variable_prefix)
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
        ccs_ver_need_svn=0
        ;;
    *)
        ccs_ver_need_svn=1
esac


if test -z "$srcfile"; then
    option="--help"
else

    : ${ccs_ver_need_svn=1}
    : ${svnversion_result=-1}

        if test -f "$srcfile"; then
        srcdir=`dirname $srcfile`
        ccs_vers=`sed -n "
	t clear
	: clear
	s/^major/CCS_MAJOR_VERSION/
	s/^minor/CCS_MINOR_VERSION/
	s/^release/CCS_RELEASE_VERSION/
	s/^greek/CCS_GREEK_VERSION/
	s/^want_svn/CCS_WANT_SVN/
	s/^svn_r/CCS_SVN_R/
	s/^date/CCS_RELEASE_DATE/
	t print
	b
	: print
	p" < "$srcfile"`
	eval "$ccs_vers"

        # Only print release version if it isn't 0
        if test $CCS_RELEASE_VERSION -ne 0 ; then
            CCS_VERSION="$CCS_MAJOR_VERSION.$CCS_MINOR_VERSION.$CCS_RELEASE_VERSION"
        else
            CCS_VERSION="$CCS_MAJOR_VERSION.$CCS_MINOR_VERSION"
        fi
        CCS_VERSION="${CCS_VERSION}${CCS_GREEK_VERSION}"
        CCS_BASE_VERSION=$CCS_VERSION

        if test $CCS_WANT_SVN -eq 1 && test $ccs_ver_need_svn -eq 1 ; then
            if test "$svnversion_result" != "-1" ; then
                CCS_SVN_R=$svnversion_result
            fi
            if test "$CCS_SVN_R" = "-1" ; then

                d=`date '+%m-%d-%Y'`
                if test -d "$srcdir/.svn" ; then
                    CCS_SVN_R=r`svnversion "$srcdir"`
                    if test $? != 0; then
                        CCS_SVN_R="unknown svn version (svnversion not found); $d"
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
                        CCS_SVN_R=hg`hg -v -R "$srcdir" tip | grep ^changeset: | head -n 1 | cut -d: -f3`
                    else
                        CCS_SVN_R="unknown hg version (hg not found); $d"
                    fi
                fi
                if test "CCS_SVN_R" = ""; then
                    CCS_SVN_R="svn$d"
                fi

            fi
            CCS_VERSION="${CCS_VERSION}${CCS_SVN_R}"
        fi
    fi


    if test "$option" = ""; then
	option="--full"
    fi
fi

case "$option" in
    --full|-v|--version)
	echo $CCS_VERSION
	;;
    --major)
	echo $CCS_MAJOR_VERSION
	;;
    --minor)
	echo $CCS_MINOR_VERSION
	;;
    --release)
	echo $CCS_RELEASE_VERSION
	;;
    --greek)
	echo $CCS_GREEK_VERSION
	;;
    --svn)
	echo $CCS_SVN_R
	;;
    --base)
        echo $CCS_BASE_VERSION
        ;;
    --release-date)
        echo $CCS_RELEASE_DATE
        ;;
    --all)
        echo ${CCS_VERSION} ${CCS_MAJOR_VERSION} ${CCS_MINOR_VERSION} ${CCS_RELEASE_VERSION} ${CCS_GREEK_VERSION} ${CCS_SVN_R}
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
