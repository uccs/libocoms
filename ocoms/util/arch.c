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
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ocoms/platform/ocoms_config.h"

#include "ocoms/platform/service_constants.h"
#include "ocoms/util/arch.h"

int service_cache_line_size = 128;

int32_t service_arch_compute_local_id( uint32_t *me )
{
    *me = (SERVICE_ARCH_HEADERMASK | SERVICE_ARCH_UNUSEDMASK);

    /* Handle the size of long (can hold a pointer) */
    if( 8 == sizeof(long) )
        service_arch_setmask( me, SERVICE_ARCH_LONGIS64 );

    /* sizeof bool */
    if (1 == sizeof(bool) ) {
        service_arch_setmask( me, SERVICE_ARCH_BOOLIS8);
    } else if (2 == sizeof(bool)) {
        service_arch_setmask( me, SERVICE_ARCH_BOOLIS16);
    } else if (4 == sizeof(bool)) {
        service_arch_setmask( me, SERVICE_ARCH_BOOLIS32);
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
        service_arch_setmask( me, SERVICE_ARCH_LOGICALIS8);
    } else if (2 == sizeof(ocoms_fortran_logical_t)) {
        service_arch_setmask( me, SERVICE_ARCH_LOGICALIS16);
    } else if (4 == sizeof(ocoms_fortran_logical_t)) {
        service_arch_setmask( me, SERVICE_ARCH_LOGICALIS32);
    }
#endif

    /* Initialize the information regarding the long double */
    if( 12 == sizeof(long double) )
        service_arch_setmask( me, SERVICE_ARCH_LONGDOUBLEIS96 );
    else if( 16 == sizeof(long double) )
        service_arch_setmask( me, SERVICE_ARCH_LONGDOUBLEIS128 );

    /* Big endian or little endian ? That's the question */
    if( service_arch_isbigendian() )
        service_arch_setmask( me, SERVICE_ARCH_ISBIGENDIAN );

    /* What's the maximum exponent ? */
    if ( LDBL_MAX_EXP == 16384 )
        service_arch_setmask( me, SERVICE_ARCH_LDEXPSIZEIS15 );

    /* How about the length in bits of the mantissa */
    if ( LDBL_MANT_DIG == 64 )
        service_arch_setmask( me, SERVICE_ARCH_LDMANTDIGIS64 );
    else if ( LDBL_MANT_DIG == 105 )
        service_arch_setmask( me, SERVICE_ARCH_LDMANTDIGIS105 );
    else if ( LDBL_MANT_DIG == 106 )
        service_arch_setmask( me, SERVICE_ARCH_LDMANTDIGIS106 );
    else if ( LDBL_MANT_DIG == 107 )
        service_arch_setmask( me, SERVICE_ARCH_LDMANTDIGIS107 );
    else if ( LDBL_MANT_DIG == 113 )
        service_arch_setmask( me, SERVICE_ARCH_LDMANTDIGIS113 );

    /* Intel data representation or Sparc ? */
    if( service_arch_ldisintel() )
        service_arch_setmask( me, SERVICE_ARCH_LDISINTEL );

    return OCOMS_SUCCESS;
}

int32_t service_arch_checkmask ( uint32_t *var, uint32_t mask )
{
    unsigned int tmpvar = *var;

    /* Check whether the headers are set correctly,
       or whether this is an erroneous integer */
    if( !((*var) & SERVICE_ARCH_HEADERMASK) ) {
        if( (*var) & SERVICE_ARCH_HEADERMASK2 ) {
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

            if( (tmpvar & SERVICE_ARCH_HEADERMASK) && (!(tmpvar & SERVICE_ARCH_HEADERMASK2)) ) {
                *var = tmpvar;
            } else
                return -1;
        } else
            return -1;
    }

    /* Here is the real evaluation of the bitmask */
    return ( ((*var) & mask) == mask );
}
