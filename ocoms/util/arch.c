/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2006 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ocoms/platform/ocoms_config.h"

#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/util/arch.h"

int ocoms_cache_line_size = 128;

int32_t ocoms_arch_compute_local_id( uint32_t *me )
{
    *me = (OCOMS_ARCH_HEADERMASK | OCOMS_ARCH_UNUSEDMASK);

    /* Handle the size of long (can hold a pointer) */
    if( 8 == sizeof(long) )
        ocoms_arch_setmask( me, OCOMS_ARCH_LONGIS64 );

    /* sizeof bool */
    if (1 == sizeof(bool) ) {
        ocoms_arch_setmask( me, OCOMS_ARCH_BOOLIS8);
    } else if (2 == sizeof(bool)) {
        ocoms_arch_setmask( me, OCOMS_ARCH_BOOLIS16);
    } else if (4 == sizeof(bool)) {
        ocoms_arch_setmask( me, OCOMS_ARCH_BOOLIS32);
    }

    /* sizeof fortran logical
     *
     * RHC: technically, use of the ompi_ prefix is
     * an abstraction violation. However, this is actually
     * an error in our configure scripts that transcends
     * all the data types and eventually should be fixed.
     * The guilty part is f77_check.m4. Fixing it right
     * now is beyond a reasonable scope - this comment is
     * placed here to explain the abstraction break and
     * indicate that it will eventually be fixed
     */
#if 0 /* Pasha: disable fortran logical sizhe check */
    if (1 == sizeof(ocoms_fortran_logical_t) ) {
        ocoms_arch_setmask( me, OCOMS_ARCH_LOGICALIS8);
    } else if (2 == sizeof(ocoms_fortran_logical_t)) {
        ocoms_arch_setmask( me, OCOMS_ARCH_LOGICALIS16);
    } else if (4 == sizeof(ocoms_fortran_logical_t)) {
        ocoms_arch_setmask( me, OCOMS_ARCH_LOGICALIS32);
    }
#endif

    /* Initialize the information regarding the long double */
    if( 12 == sizeof(long double) )
        ocoms_arch_setmask( me, OCOMS_ARCH_LONGDOUBLEIS96 );
    else if( 16 == sizeof(long double) )
        ocoms_arch_setmask( me, OCOMS_ARCH_LONGDOUBLEIS128 );

    /* Big endian or little endian ? That's the question */
    if( ocoms_arch_isbigendian() )
        ocoms_arch_setmask( me, OCOMS_ARCH_ISBIGENDIAN );

    /* What's the maximum exponent ? */
    if ( LDBL_MAX_EXP == 16384 )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDEXPSIZEIS15 );

    /* How about the length in bits of the mantissa */
    if ( LDBL_MANT_DIG == 64 )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDMANTDIGIS64 );
    else if ( LDBL_MANT_DIG == 105 )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDMANTDIGIS105 );
    else if ( LDBL_MANT_DIG == 106 )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDMANTDIGIS106 );
    else if ( LDBL_MANT_DIG == 107 )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDMANTDIGIS107 );
    else if ( LDBL_MANT_DIG == 113 )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDMANTDIGIS113 );

    /* Intel data representation or Sparc ? */
    if( ocoms_arch_ldisintel() )
        ocoms_arch_setmask( me, OCOMS_ARCH_LDISINTEL );

    return OCOMS_SUCCESS;
}

int32_t ocoms_arch_checkmask ( uint32_t *var, uint32_t mask )
{
    unsigned int tmpvar = *var;

    /* Check whether the headers are set correctly,
       or whether this is an erroneous integer */
    if( !((*var) & OCOMS_ARCH_HEADERMASK) ) {
        if( (*var) & OCOMS_ARCH_HEADERMASK2 ) {
            char* pcDest, *pcSrc;
            /* Both ends of this integer have the wrong settings,
               maybe its just the wrong endian-representation. Try
               to swap it and check again. If it looks now correct,
               keep this version of the variable
            */

            pcDest = (char *) &tmpvar;
            pcSrc  = (char *) var + 3;
            *pcDest++ = *pcSrc--;
            *pcDest++ = *pcSrc--;
            *pcDest++ = *pcSrc--;
            *pcDest++ = *pcSrc--;

            if( (tmpvar & OCOMS_ARCH_HEADERMASK) && (!(tmpvar & OCOMS_ARCH_HEADERMASK2)) ) {
                *var = tmpvar;
            } else
                return -1;
        } else
            return -1;
    }

    /* Here is the real evaluation of the bitmask */
    return ( ((*var) & mask) == mask );
}
