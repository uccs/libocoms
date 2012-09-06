# -*- shell-script -*-
#
# Copyright (c) 2009-2010 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# This file is m4_included in the top-level configure.ac

AC_DEFUN([CCS_CONFIG_FILES],[
    AC_CONFIG_FILES([
        service/include/Makefile
        service/asm/Makefile
        service/datatype/Makefile
        ccs/Makefile
    ])
])
#        service/util/Makefile
