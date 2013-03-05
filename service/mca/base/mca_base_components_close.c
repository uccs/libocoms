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

#include "service/platform/ccs_config.h"

#include "service/util/service_list.h"
#include "service/util/output.h"
#include "service/mca/mca.h"
#include "service/mca/base/base.h"
#include "service/mca/base/mca_base_component_repository.h"
#include "service/platform/service_constants.h"

int ccs_mca_base_components_close(int output_id, 
                              service_list_t *components_available, 
                              const ccs_mca_base_component_t *skip)
{
  service_list_item_t *item;
  ccs_mca_base_component_priority_list_item_t *pcli, *skipped_pcli = NULL;
  const ccs_mca_base_component_t *component;

  /* Close and unload all components in the available list, except the
     "skip" item.  This is handy to close out all non-selected
     components.  It's easier to simply remove the entire list and
     then simply re-add the skip entry when done. */

  for (item = service_list_remove_first(components_available);
       NULL != item; 
       item = service_list_remove_first(components_available)) {
    pcli = (ccs_mca_base_component_priority_list_item_t *) item;
    component = pcli->super.cli_component;

    if (component != skip) {

      /* Close */


      if (NULL != component->mca_close_component) {
        component->mca_close_component();
        service_output_verbose(10, output_id, 
                            "mca: base: close: component %s closed",
                           component->mca_component_name);
      }

      /* Unload */

      service_output_verbose(10, output_id, 
                          "mca: base: close: unloading component %s",
                         component->mca_component_name);
      ccs_mca_base_component_repository_release((ccs_mca_base_component_t *) component);
      free(pcli);
    } else {
      skipped_pcli = pcli;
    }
  }

  /* If we found it, re-add the skipped component to the available
     list (see above comment) */

  if (NULL != skipped_pcli) {
    service_list_append(components_available, (service_list_item_t *) skipped_pcli);
  }

  /*
   * If we are not the verbose output stream, and we shouldn't skip
   * any components, close the output stream.  If there's a skip
   * component, this is a 'choose one' framework and we're closing the
   * unchoosen components, but will still be using the framework.
   */
  if (0 != output_id && NULL == skip) {
      service_output_close (output_id);
  }
  /* All done */
  return CCS_SUCCESS;
}
