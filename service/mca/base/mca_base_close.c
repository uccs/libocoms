/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ccs_config.h"
#include "service/mca/mca.h"

#include "service/mca/base/base.h"
#include "ccs/include/ccs_constants.h"
#include "service/mca/base/mca_base_component_repository.h"
#if 0
#include "service/util/output.h"
#endif

/*
 * Main MCA shutdown.
 */
int ccs_mca_base_close(void)
{
  extern bool ccs_mca_base_opened;
  if (ccs_mca_base_opened) {

      /* release the default paths */
      if (NULL != ccs_mca_base_system_default_path) {
          free(ccs_mca_base_system_default_path);
      }
      if (NULL != ccs_mca_base_user_default_path) {
          free(ccs_mca_base_user_default_path);
      }
      
    /* Close down the component repository */
    ccs_mca_base_component_repository_finalize();

    /* Shut down the dynamic component finder */
    ccs_mca_base_component_find_finalize();

    /* Close opal output stream 0 */
    service_output_close(0);
  }
  ccs_mca_base_opened = false;

  /* All done */

  return CCS_SUCCESS;
}
