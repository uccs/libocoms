/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2009      IBM Corporation.  All rights reserved.
 * Copyright (c) 2009      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef _OCOMS_CRC_H_
#define _OCOMS_CRC_H_

#include "ocoms/platform/ocoms_config.h"

#include <stddef.h>

BEGIN_C_DECLS

#define CRC_POLYNOMIAL ((unsigned int)0x04c11db7)
#define CRC_INITIAL_REGISTER ((unsigned int)0xffffffff)


#define OCOMS_CSUM( SRC, LEN )  ocoms_uicsum( SRC, LEN )
#define OCOMS_CSUM_PARTIAL( SRC, LEN, UI1, UI2 ) \
    ocoms_uicsum_partial( SRC, LEN, UI1, UI2 )
#define OCOMS_CSUM_BCOPY_PARTIAL( SRC, DST, LEN1, LEN2, UI1, UI2 ) \
    ocoms_bcopy_uicsum_partial( SRC, DST, LEN1, LEN2, UI1, UI2 )
#define OCOMS_CSUM_ZERO  0


OCOMS_DECLSPEC unsigned long
ocoms_bcopy_csum_partial(
    const void *  source,
    void *  destination,
    size_t copylen,
    size_t csumlen,
    unsigned long*  lastPartialLong,
    size_t*  lastPartialLength
    );

static inline unsigned long
ocoms_bcopy_csum (
    const void *  source,
    void *  destination,
    size_t copylen,
    size_t csumlen
    )
{
    unsigned long plong = 0;
    size_t plength = 0;
    return ocoms_bcopy_csum_partial(source, destination, copylen, csumlen, &plong, &plength);
}
                                                                                                                  
OCOMS_DECLSPEC unsigned int 
ocoms_bcopy_uicsum_partial (
    const void *  source,
    void *  destination,
    size_t copylen,
    size_t csumlen,
    unsigned int*  lastPartialInt,
    size_t*  lastPartialLength
    );

static inline unsigned int
ocoms_bcopy_uicsum (
    const void *  source,
    void *  destination,
    size_t copylen,
    size_t csumlen
    )
{
    unsigned int pint = 0;
    size_t plength = 0;
    return ocoms_bcopy_uicsum_partial(source, destination, copylen, csumlen, &pint, &plength);
}
                                                                                                                  
OCOMS_DECLSPEC unsigned long 
ocoms_csum_partial (
    const void *  source,
    size_t csumlen,
    unsigned long*  lastPartialLong,
    size_t*  lastPartialLength
    );


static inline unsigned long 
ocoms_csum(const void *  source, size_t csumlen)
{
    unsigned long lastPartialLong = 0;
    size_t lastPartialLength = 0;
    return ocoms_csum_partial(source, csumlen, &lastPartialLong, &lastPartialLength);
}
/*
 * The buffer passed to this function is assumed to be 16-bit aligned
 */
static inline uint16_t
ocoms_csum16 (const void *  source, size_t csumlen)
{
    uint16_t *src = (uint16_t *) source;
    register uint32_t csum = 0;

    while (csumlen > 1) {
	    csum += *src++;
        csumlen -= 2;
    }
    /* Add leftover byte, if any */ 
    if(csumlen > 0)
        csum += *((unsigned char*)src);
    /* Fold 32-bit checksum to 16 bits */
    while(csum >> 16) {
        csum = (csum & 0xFFFF) + (csum >> 16);    
    }        
    return csum;
}

OCOMS_DECLSPEC unsigned int
ocoms_uicsum_partial (
    const void *  source,
    size_t csumlen,
    unsigned int *  lastPartialInt,
    size_t*  lastPartialLength
    );

static inline unsigned int 
ocoms_uicsum(const void *  source, size_t csumlen)
{
    unsigned int lastPartialInt = 0;
    size_t lastPartialLength = 0;
    return ocoms_uicsum_partial(source, csumlen, &lastPartialInt, &lastPartialLength);
}
                                                                                                                  
/*
 * CRC Support
 */

void ocoms_initialize_crc_table(void);

OCOMS_DECLSPEC unsigned int 
ocoms_bcopy_uicrc_partial(
    const void *  source,
    void *  destination,
    size_t copylen,
    size_t crclen,
    unsigned int partial_crc);

static inline unsigned int 
ocoms_bcopy_uicrc(
    const void *  source, 
    void *  destination,
    size_t copylen, 
    size_t crclen)
{
    return ocoms_bcopy_uicrc_partial(source, destination, copylen, crclen, CRC_INITIAL_REGISTER);
}

OCOMS_DECLSPEC unsigned int 
ocoms_uicrc_partial(
    const void *  source, 
    size_t crclen, 
    unsigned int partial_crc);


static inline unsigned int 
ocoms_uicrc(const void *  source, size_t crclen)
{
    return ocoms_uicrc_partial(source, crclen, CRC_INITIAL_REGISTER);
}
                                                                                                                  
END_C_DECLS

#endif

