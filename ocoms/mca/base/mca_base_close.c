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

#include "ocoms/platform/ocoms_config.h"
#include "ocoms/mca/mca.h"

#include "ocoms/mca/base/base.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/mca/base/mca_base_component_repository.h"
#if 0
#include "ocoms/util/output.h"
#endif

/*
 * Main MCA shutdown.
 */
int ocoms_mca_base_close(void)
{
  extern bool ocoms_mca_base_opened;
  if (ocoms_mca_base_opened) {

      /* release the default paths */
      if (NULL != ocoms_mca_base_system_default_path) {
          free(ocoms_mca_base_system_default_path);
      }
      if (NULL != ocoms_mca_base_user_default_path) {
          free(ocoms_mca_base_user_default_path);
      }
      
    /* Close down the component repository */
    ocoms_mca_base_component_repository_finalize();

    /* Shut down the dynamic component finder */
    ocoms_mca_base_component_find_finalize();

    /* Close opal output stream 0 */
    ocoms_output_close(0);
  }
  ocoms_mca_base_opened = false;

  /* All done */

  return OCOMS_SUCCESS;
}
