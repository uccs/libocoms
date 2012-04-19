/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 */

#ifndef OPAL_IF_BASE_H
#define OPAL_IF_BASE_H

#include "ccs_config.h"

#include "ccs/mca/sysif/sysif.h"

/*
 * Global functions for MCA overall if open and close
 */
BEGIN_C_DECLS

CCS_DECLSPEC int ccs_sysif_base_open(void);
CCS_DECLSPEC int ccs_sysif_base_close(void);

/*
 * Globals
 */
CCS_DECLSPEC extern service_list_t ccs_sysif_components;

END_C_DECLS

#endif /* OPAL_BASE_IF_H */
