# -*- shell-script -*-
#
# Copyright (c) 2009-2010 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
# Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

# This file is m4_included in the top-level configure.ac

AC_DEFUN([OCOMS_CONFIG_FILES],[
    AC_CONFIG_FILES([
        ocoms/platform/Makefile
        ocoms/asm/Makefile
        ocoms/datatype/Makefile
        ocoms/dstore/Makefile
    ])
])
#        ocoms/util/Makefile
