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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ccs_config.h"

#include <stdio.h>
#include <string.h>

#include "service/util/cmd_line.h"
#include "service/util/argv.h"
#if 0
#include "service/util/ccs_environ.h"
#endif
#include "service/mca/base/base.h"
#include "service/include/service_constants.h"


/* 
 * Private variables
 */

/*
 * Private functions
 */
static int process_arg(const char *param, const char *value,
                       char ***params, char ***values);
static void add_to_env(char **params, char **values, char ***env);


/*
 * Add -mca to the possible command line options list
 */
int ccs_mca_base_cmd_line_setup(service_cmd_line_t *cmd)
{
    int ret = CCS_SUCCESS;

    ret = service_cmd_line_make_opt3(cmd, '\0', "mca", "mca", 2,
                                  "Pass context-specific MCA parameters; they are considered global if --gmca is not used and only one context is specified (arg0 is the parameter name; arg1 is the parameter value)");
    if (CCS_SUCCESS != ret) {
        return ret;
    }

    ret = service_cmd_line_make_opt3(cmd, '\0', "gmca", "gmca", 2,
                                  "Pass global MCA parameters that are applicable to all contexts (arg0 is the parameter name; arg1 is the parameter value)");

    if (CCS_SUCCESS != ret) {
        return ret;
    }

    {
        service_cmd_line_init_t entry = 
            {"mca", "base", "param_file_prefix", '\0', "am", NULL, 1,
             NULL, CCS_CMD_LINE_TYPE_STRING,
             "Aggregate MCA parameter set file list"
            };
        ret = service_cmd_line_make_opt_mca(cmd, entry);
        if (CCS_SUCCESS != ret) {
            return ret;
        }
    }

    return ret;
}


/*
 * Look for and handle any -mca options on the command line
 */
int ccs_mca_base_cmd_line_process_args(service_cmd_line_t *cmd,
                                   char ***context_env, char ***global_env)
{
  int i, num_insts;
  char **params;
  char **values;

  /* If no relevant parameters were given, just return */

  if (!service_cmd_line_is_taken(cmd, "mca") &&
      !service_cmd_line_is_taken(cmd, "gmca")) {
      return CCS_SUCCESS;
  }

  /* Handle app context-specific parameters */

  num_insts = service_cmd_line_get_ninsts(cmd, "mca");
  params = values = NULL;
  for (i = 0; i < num_insts; ++i) {
      process_arg(service_cmd_line_get_param(cmd, "mca", i, 0), 
                  service_cmd_line_get_param(cmd, "mca", i, 1),
                  &params, &values);
  }
  if (NULL != params) {
      add_to_env(params, values, context_env);
      service_argv_free(params);
      service_argv_free(values);
  }

  /* Handle global parameters */

  num_insts = service_cmd_line_get_ninsts(cmd, "gmca");
  params = values = NULL;
  for (i = 0; i < num_insts; ++i) {
      process_arg(service_cmd_line_get_param(cmd, "gmca", i, 0), 
                  service_cmd_line_get_param(cmd, "gmca", i, 1),
                  &params, &values);
  }
  if (NULL != params) {
      add_to_env(params, values, global_env);
      service_argv_free(params);
      service_argv_free(values);
  }

  /* All done */

  return CCS_SUCCESS;
}


/*
 * Process a single MCA argument.
 */
static int process_arg(const char *param, const char *value,
                       char ***params, char ***values)
{
    int i;
    char *new_str;

    /* Look to see if we've already got an -mca argument for the same
       param.  Check against the list of MCA param's that we've
       already saved arguments for. */

    for (i = 0; NULL != *params && NULL != (*params)[i]; ++i) {
        if (0 == strcmp(param, (*params)[i])) {
            asprintf(&new_str, "%s,%s", (*values)[i], value);
            free((*values)[i]);
            (*values)[i] = new_str;
            
            return CCS_SUCCESS;
        }
    }

    /* If we didn't already have an value for the same param, save
       this one away */
  
    service_argv_append_nosize(params, param);
    service_argv_append_nosize(values, value);

    return CCS_SUCCESS;
}


static void add_to_env(char **params, char **values, char ***env)
{
    int i;
    char *name;

    /* Loop through all the args that we've gotten and make env
       vars of the form OMPI_MCA_*=value. */

    for (i = 0; NULL != params && NULL != params[i]; ++i) {
        name = ccs_mca_base_param_environ_variable(params[i], NULL, NULL);
        service_setenv(name, values[i], true, env);
        free(name);
    }
}
