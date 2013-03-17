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
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif  /* HAVE_STRING_H */
#include <ctype.h>

#include "ocoms/util/service_object.h"
#include "ocoms/util/service_list.h"
#include "ocoms/threads/mutex.h"
#include "ocoms/util/argv.h"
#include "ocoms/util/cmd_line.h"
#include "ocoms/util/output.h"
#include "ocoms/mca/base/mca_base_param.h"
#include "ocoms/platform/service_constants.h"


/*
 * Some usage message constants
 *
 * Max width for param listings before the description will be listed
 * on the next line
 */
#define PARAM_WIDTH 25
/*
 * Max length of any line in the usage message
 */
#define MAX_WIDTH 76

/*
 * Description of a command line option
 */
struct service_cmd_line_option_t {
    service_list_item_t super;

    char clo_short_name;
    char *clo_single_dash_name;
    char *clo_long_name;

    int clo_num_params;
    char *clo_description;

    service_cmd_line_type_t clo_type;
    char *clo_mca_param_env_var;
    void *clo_variable_dest;
    bool clo_variable_set;
};
typedef struct service_cmd_line_option_t service_cmd_line_option_t;
static void option_constructor(service_cmd_line_option_t *cmd);
static void option_destructor(service_cmd_line_option_t *cmd);

OBJ_CLASS_INSTANCE(service_cmd_line_option_t,
                   service_list_item_t,
                   option_constructor, option_destructor);

/*
 * An option that was used in the argv that was parsed
 */
struct service_cmd_line_param_t {
    service_list_item_t super;

    /* Note that clp_arg points to storage "owned" by someone else; it
       has the original option string by referene, not by value.
       Hence, it should not be free()'ed. */

    char *clp_arg;

    /* Pointer to the existing option.  This is also by reference; it
       should not be free()ed. */

    service_cmd_line_option_t *clp_option;

    /* This argv array is a list of all the parameters of this option.
       It is owned by this parameter, and should be freed when this
       param_t is freed. */

    int clp_argc;
    char **clp_argv;
};
typedef struct service_cmd_line_param_t service_cmd_line_param_t;
static void param_constructor(service_cmd_line_param_t *cmd);
static void param_destructor(service_cmd_line_param_t *cmd);
OBJ_CLASS_INSTANCE(service_cmd_line_param_t,
                   service_list_item_t,
                   param_constructor, param_destructor);

/*
 * Instantiate the service_cmd_line_t class
 */
static void cmd_line_constructor(service_cmd_line_t *cmd);
static void cmd_line_destructor(service_cmd_line_t *cmd);
OBJ_CLASS_INSTANCE(service_cmd_line_t,
                   service_object_t,
                   cmd_line_constructor,
                   cmd_line_destructor);

/*
 * Private variables
 */
static char special_empty_token[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, '\0'
};

/*
 * Private functions
 */
static int make_opt(service_cmd_line_t *cmd, service_cmd_line_init_t *e);
static void free_parse_results(service_cmd_line_t *cmd);
static int split_shorts(service_cmd_line_t *cmd,
                        char *token, char **args, 
                        int *output_argc, char ***output_argv, 
                        int *num_args_used, bool ignore_unknown);
static service_cmd_line_option_t *find_option(service_cmd_line_t *cmd, 
                                      const char *option_name) __service_attribute_nonnull__(1) __service_attribute_nonnull__(2);
static void set_dest(service_cmd_line_option_t *option, char *sval);
static void fill(const service_cmd_line_option_t *a, char result[3][BUFSIZ]);
static int qsort_callback(const void *a, const void *b);


/*
 * Create an entire command line handle from a table
 */
int service_cmd_line_create(service_cmd_line_t *cmd,
                         service_cmd_line_init_t *table)
{
    int i, ret = OCOMS_SUCCESS;

    /* Check bozo case */

    if (NULL == cmd) {
        return OCOMS_ERR_BAD_PARAM;
    }
    OBJ_CONSTRUCT(cmd, service_cmd_line_t);

    /* Ensure we got a table */

    if (NULL == table) {
        return OCOMS_SUCCESS;
    }

    /* Loop through the table */

    for (i = 0; ; ++i) {

        /* Is this the end? */

        if ('\0' == table[i].ocl_cmd_short_name &&
            NULL == table[i].ocl_cmd_single_dash_name &&
            NULL == table[i].ocl_cmd_long_name) {
            break;
        }

        /* Nope -- it's an entry.  Process it. */

        ret = make_opt(cmd, &table[i]);
        if (OCOMS_SUCCESS != ret) {
            return ret;
        }
    }

    return ret;
}

/*
 * Append a command line entry to the previously constructed command line
 */
int service_cmd_line_make_opt_mca(service_cmd_line_t *cmd,
                               service_cmd_line_init_t entry)
{
    /* Ensure we got an entry */
    if ('\0' == entry.ocl_cmd_short_name &&
        NULL == entry.ocl_cmd_single_dash_name &&
        NULL == entry.ocl_cmd_long_name) {
        return OCOMS_SUCCESS;
    }

    return make_opt(cmd, &entry);
}


/*
 * Create a command line option, --long-name and/or -s (short name).
 */
int service_cmd_line_make_opt(service_cmd_line_t *cmd, char short_name, 
                          const char *long_name, int num_params, 
                          const char *desc)
{
    service_cmd_line_init_t e;

    e.ocl_mca_type_name = NULL;
    e.ocl_mca_component_name = NULL;
    e.ocl_mca_param_name = NULL;

    e.ocl_cmd_short_name = short_name;
    e.ocl_cmd_single_dash_name = NULL;
    e.ocl_cmd_long_name = long_name;

    e.ocl_num_params = num_params;

    e.ocl_variable_dest = NULL;
    e.ocl_variable_type = OCOMS_CMD_LINE_TYPE_NULL;

    e.ocl_description = desc;

    return make_opt(cmd, &e);
}


/*
 * Create a command line option, --long-name and/or -s (short name).
 */
int service_cmd_line_make_opt3(service_cmd_line_t *cmd, char short_name, 
                            const char *sd_name, const char *long_name, 
                            int num_params, const char *desc)
{
    service_cmd_line_init_t e;

    e.ocl_mca_type_name = NULL;
    e.ocl_mca_component_name = NULL;
    e.ocl_mca_param_name = NULL;

    e.ocl_cmd_short_name = short_name;
    e.ocl_cmd_single_dash_name = sd_name;
    e.ocl_cmd_long_name = long_name;

    e.ocl_num_params = num_params;

    e.ocl_variable_dest = NULL;
    e.ocl_variable_type = OCOMS_CMD_LINE_TYPE_NULL;

    e.ocl_description = desc;

    return make_opt(cmd, &e);
}


/*
 * Parse a command line according to a pre-built OPAL command line
 * handle.
 */
int service_cmd_line_parse(service_cmd_line_t *cmd, bool ignore_unknown,
                        int argc, char **argv)
{
    int i, j, orig, ret;
    service_cmd_line_option_t *option;
    service_cmd_line_param_t *param;
    bool is_unknown;
    bool is_option;
    bool has_unknowns;
    char **shortsv;
    int shortsc;
    int num_args_used;

    /* Bozo check */

    if (0 == argc || NULL == argv) {
        return OCOMS_SUCCESS;
    }

    /* Thread serialization */

    service_mutex_lock(&cmd->lcl_mutex);

    /* Free any parsed results that are already on this handle */

    free_parse_results(cmd);

    /* Analyze each token */

    cmd->lcl_argc = argc;
    cmd->lcl_argv = service_argv_copy(argv);

    /* Now traverse the easy-to-parse sequence of tokens.  Note that
       incrementing i must happen elsehwere; it can't be the third
       clause in the "if" statement. */

    param = NULL;
    option = NULL;
    has_unknowns = false;
    for (i = 1; i < cmd->lcl_argc; ) {
        is_unknown = false;
        is_option = false;

        /* Are we done?  i.e., did we find the special "--" token?  If
           so, copy everying beyond it into the tail (i.e., don't
           bother copying the "--" into the tail). */

        if (0 == strcmp(cmd->lcl_argv[i], "--")) {
            ++i;
            while (i < cmd->lcl_argc) {
                service_argv_append(&cmd->lcl_tail_argc, &cmd->lcl_tail_argv, 
                                 cmd->lcl_argv[i]);
                ++i;
            }

            break;
        }

        /* If it's not an option, then we've found an unrecognized
           token. */

        else if ('-' != cmd->lcl_argv[i][0]) {
            is_unknown = true;
        }

        /* Nope, this is supposedly an option.  Is it a long name? */

        else if (0 == strncmp(cmd->lcl_argv[i], "--", 2)) {
            is_option = true;
            option = find_option(cmd, cmd->lcl_argv[i] + 2);
        }

        /* It could be a short name.  Is it? */

        else {
            option = find_option(cmd, cmd->lcl_argv[i] + 1);

            /* If we didn't find it, try to split it into shorts.  If
               we find the short option, replace lcl_argv[i] and
               insert the rest into lcl_argv starting after position
               i.  If we don't find the short option, don't do
               anything to lcl_argv so that it can fall through to the
               error condition, below. */

            if (NULL == option) {
                shortsv = NULL;
                shortsc = 0;
                ret = split_shorts(cmd, cmd->lcl_argv[i] + 1, 
                                   &(cmd->lcl_argv[i + 1]),
                                   &shortsc, &shortsv, 
                                   &num_args_used, ignore_unknown);
                if (OCOMS_SUCCESS == ret) {
                    option = find_option(cmd, shortsv[0] + 1);

                    if (NULL != option) {
                        service_argv_delete(&cmd->lcl_argc,
			                 &cmd->lcl_argv, i,
                                         1 + num_args_used);
                        service_argv_insert(&cmd->lcl_argv, i, shortsv);
                        cmd->lcl_argc = service_argv_count(cmd->lcl_argv);
                    } else {
                        is_unknown = true;
                    }
                    service_argv_free(shortsv);
                } else {
                    is_unknown = true;
                }
            }

            if (NULL != option) {
                is_option = true;
            }
        }

        /* If we figured out above that this is an option, handle it */

        if (is_option) {
            if (NULL == option) {
                is_unknown = true;
            } else {
                is_unknown = false;
                orig = i;
                ++i;

                /* Suck down the following parameters that belong to
                   this option.  If we run out of parameters, or find
                   that any of them are the special_empty_param
                   (insertted by split_shorts()), then print an error
                   and return. */

                param = OBJ_NEW(service_cmd_line_param_t);
                if (NULL == param) {
                    service_mutex_unlock(&cmd->lcl_mutex);
                    return OCOMS_ERR_OUT_OF_RESOURCE;
                }
                param->clp_arg = cmd->lcl_argv[i];
                param->clp_option = option;

                /* If we have any parameters to this option, suck down
                   tokens starting one beyond the token that we just
                   recognized */

                for (j = 0; j < option->clo_num_params; ++j, ++i) {

                    /* If we run out of parameters, error */

                    if (i >= cmd->lcl_argc) {
                        service_output(0, "Error: option \"%s\" did not have "
                                    "enough parameters (%d)",
                                    cmd->lcl_argv[orig],
                                    option->clo_num_params);
                        OBJ_RELEASE(param);
                        i = cmd->lcl_argc;
                        break;
                    } else {
                        if (0 == strcmp(cmd->lcl_argv[i], 
                                        special_empty_token)) {
                            service_output(0, "Error: option \"%s\" did not have "
                                        "enough parameters (%d)",
                                        cmd->lcl_argv[orig],
                                        option->clo_num_params);
                            if (NULL != param->clp_argv)
                                service_argv_free(param->clp_argv);
                            OBJ_RELEASE(param);
                            i = cmd->lcl_argc;
                            break;
                        } 

                        /* Otherwise, save this parameter */

                        else {
                            /* Save in the argv on the param entry */

                            service_argv_append(&param->clp_argc,
                                             &param->clp_argv, 
                                             cmd->lcl_argv[i]);

                            /* If it's the first, save it in the
                               variable dest and/or MCA parameter */

                            if (0 == j &&
                                (NULL != option->clo_mca_param_env_var ||
                                 NULL != option->clo_variable_dest)) {
                                set_dest(option, cmd->lcl_argv[i]);
                            }
                        }
                    }
                }

                /* If there are no options to this command, see if we
                   need to set a boolean value to "true". */

                if (0 == option->clo_num_params) {
                    set_dest(option, "1");
                }

                /* If we succeeded in all that, save the param to the
                   list on the service_cmd_line_t handle */

                if (NULL != param) {
                    service_list_append(&cmd->lcl_params, &param->super);
                }
            }
        }

        /* If we figured out above that this was an unknown option,
           handle it.  Copy everything (including the current token)
           into the tail.  If we're not ignoring unknowns, then print
           an error and return. 
        */

        if (is_unknown) {
            has_unknowns = true;
            if (!ignore_unknown) {
                service_output(0, "Error: unknown option \"%s\"", 
                            cmd->lcl_argv[i]);
            }
            while (i < cmd->lcl_argc) {
                service_argv_append(&cmd->lcl_tail_argc, &cmd->lcl_tail_argv, 
                                 cmd->lcl_argv[i]);
                ++i;
            }
        }
    }

    /* Thread serialization */

    service_mutex_unlock(&cmd->lcl_mutex);

    /* All done */
    if(has_unknowns && !ignore_unknown) {
        return OCOMS_ERR_BAD_PARAM;
    }

    return OCOMS_SUCCESS;
}


/*
 * Return a consolidated "usage" message for a OPAL command line handle.
 */
char *service_cmd_line_get_usage_msg(service_cmd_line_t *cmd)
{
    size_t i, len;
    int argc;
    size_t j;
    char **argv;
    char *ret, temp[MAX_WIDTH * 2], line[MAX_WIDTH * 2];
    char *start, *desc, *ptr;
    service_list_item_t *item;
    service_cmd_line_option_t *option, **sorted;
    bool filled;

    /* Thread serialization */

    service_mutex_lock(&cmd->lcl_mutex);

    /* Make an argv of all the usage strings */

    argc = 0;
    argv = NULL;
    ret = NULL;

    /* First, take the original list and sort it */

    sorted = (service_cmd_line_option_t**)malloc(sizeof(service_cmd_line_option_t *) * 
                                         service_list_get_size(&cmd->lcl_options));
    if (NULL == sorted) {
        return NULL;
    }
    for (i = 0, item = service_list_get_first(&cmd->lcl_options); 
         service_list_get_end(&cmd->lcl_options) != item;
         ++i, item = service_list_get_next(item)) {
        sorted[i] = (service_cmd_line_option_t *) item;
    }
    qsort(sorted, i, sizeof(service_cmd_line_option_t*), qsort_callback);

    /* Now go through the sorted array and make the strings */

    for (j = 0; j < service_list_get_size(&cmd->lcl_options); ++j) {
        option = sorted[j];
        if (NULL != option->clo_description) {
            
            /* Build up the output line */

            filled = false;
            line[0] = '\0';
            if ('\0' != option->clo_short_name) {
                line[0] = '-';
                line[1] = option->clo_short_name;
                filled = true;
            } else {
                line[0] = ' ';
                line[1] = ' ';
            }
            line[2] = '\0';
            line[3] = '\0';
            if (NULL != option->clo_single_dash_name) {
                line[2] = (filled) ? '|' : ' ';
                strcat(line, "-");
                strcat(line, option->clo_single_dash_name);
                filled = true;
            }
            if (NULL != option->clo_long_name) {
                if (filled) {
                    strcat(line, "|");
                } else {
                    strcat(line, " ");
                }
                strcat(line, "--");
                strcat(line, option->clo_long_name);
                filled = true;
            }
            strcat(line, " ");
            for (i = 0; (int)i < option->clo_num_params; ++i) {
                len = sizeof(temp);
                snprintf(temp, len, "<arg%d> ", (int)i);
                strcat(line, temp);
            }
            if (option->clo_num_params > 0) {
                strcat(line, " ");
            }

            /* If we're less than param width, then start adding the
               description to this line.  Otherwise, finish this line
               and start adding the description on the next line. */

            if (strlen(line) > PARAM_WIDTH) {
                service_argv_append(&argc, &argv, line);

                /* Now reset the line to be all blanks up to
                   PARAM_WIDTH so that we can start adding the
                   description */

                memset(line, ' ', PARAM_WIDTH);
                line[PARAM_WIDTH] = '\0';
            } else {

                /* Add enough blanks to the end of the line so that we
                   can start adding the description */

                for (i = strlen(line); i < PARAM_WIDTH; ++i) {
                    line[i] = ' ';
                }
                line[i] = '\0';
            }

            /* Loop over adding the description to the array, breaking
               the string at most at MAX_WIDTH characters.  We need a
               modifyable description (for simplicity), so strdup the
               clo_description (because it's likely a compiler
               constant, and may barf if we write temporary \0's in
               the middle). */

            desc = strdup(option->clo_description);
            if (NULL == desc) {
                free(sorted);
                return strdup("");
            }
            start = desc;
            len = strlen(desc);
            do {

                /* Trim off leading whitespace */

                while (isspace(*start) && start < desc + len) {
                    ++start;
                }
                if (start >= desc + len) {
                    break;
                }

                /* Last line */

                if (strlen(start) < (MAX_WIDTH - PARAM_WIDTH)) {
                    strcat(line, start);
                    service_argv_append(&argc, &argv, line);
                    break;
                }

                /* We have more than 1 line's worth left -- find this
                   line's worth and add it to the array.  Then reset
                   and loop around to get the next line's worth. */

                for (ptr = start + (MAX_WIDTH - PARAM_WIDTH); 
                     ptr > start; --ptr) {
                    if (isspace(*ptr)) {
                        *ptr = '\0';
                        strcat(line, start);
                        service_argv_append(&argc, &argv, line);

                        start = ptr + 1;
                        memset(line, ' ', PARAM_WIDTH);
                        line[PARAM_WIDTH] = '\0';
                        break;
                    }
                }

                /* If we got all the way back to the beginning of the
                   string, then go forward looking for a whitespace
                   and break there. */

                if (ptr == start) {
                    for (ptr = start + (MAX_WIDTH - PARAM_WIDTH); 
                         ptr < start + len; ++ptr) {
                        if (isspace(*ptr)) {
                            *ptr = '\0';

                            strcat(line, start);
                            service_argv_append(&argc, &argv, line);
                            
                            start = ptr + 1;
                            memset(line, ' ', PARAM_WIDTH);
                            line[PARAM_WIDTH] = '\0';
                            break;
                        }
                    }

                    /* If we reached the end of the string with no
                       whitespace, then just add it on and be done */

                    if (ptr >= start + len) {
                        strcat(line, start);
                        service_argv_append(&argc, &argv, line);
                        start = desc + len + 1;
                    }
                }
            } while (start < desc + len);
            free(desc);
        }
    }
    if (NULL != argv) {
        ret = service_argv_join(argv, '\n');
        service_argv_free(argv);
    } else {
        ret = strdup("");
    }
    free(sorted);

    /* Thread serialization */
    service_mutex_unlock(&cmd->lcl_mutex);

    /* All done */
    return ret;
}


/*
 * Test if a given option was taken on the parsed command line.
 */
bool service_cmd_line_is_taken(service_cmd_line_t *cmd, const char *opt)
{
    return (service_cmd_line_get_ninsts(cmd, opt) > 0);
}


/*
 * Return the number of instances of an option found during parsing.
 */
int service_cmd_line_get_ninsts(service_cmd_line_t *cmd, const char *opt)
{
    int ret;
    service_list_item_t *item;
    service_cmd_line_param_t *param;
    service_cmd_line_option_t *option;

    /* Thread serialization */

    service_mutex_lock(&cmd->lcl_mutex);

    /* Find the corresponding option.  If we find it, look through all
       the parsed params and see if we have any matches. */

    ret = 0;
    option = find_option(cmd, opt);
    if (NULL != option) {
        for (item = service_list_get_first(&cmd->lcl_params); 
             service_list_get_end(&cmd->lcl_params) != item;
             item = service_list_get_next(item)) {
            param = (service_cmd_line_param_t *) item;
            if (param->clp_option == option) {
                ++ret;
            }
        }
    }

    /* Thread serialization */

    service_mutex_unlock(&cmd->lcl_mutex);

    /* All done */

    return ret;
}


/*
 * Return a specific parameter for a specific instance of a option
 * from the parsed command line.
 */
char *service_cmd_line_get_param(service_cmd_line_t *cmd, const char *opt, int inst, 
                              int idx)
{
    int num_found;
    service_list_item_t *item;
    service_cmd_line_param_t *param;
    service_cmd_line_option_t *option;

    /* Thread serialization */

    service_mutex_lock(&cmd->lcl_mutex);

    /* Find the corresponding option.  If we find it, look through all
       the parsed params and see if we have any matches. */

    num_found = 0;
    option = find_option(cmd, opt);
    if (NULL != option) {

        /* Ensure to check for the case where the user has asked for a
           parameter index greater than we will have */

        if (idx < option->clo_num_params) {
            for (item = service_list_get_first(&cmd->lcl_params); 
                 service_list_get_end(&cmd->lcl_params) != item;
                 item = service_list_get_next(item)) {
                param = (service_cmd_line_param_t *) item;
                if (param->clp_option == option) {
                    if (num_found == inst) {
                        service_mutex_unlock(&cmd->lcl_mutex);
                        return param->clp_argv[idx];
                    }
                    ++num_found;
                }
            }
        }
    }
    
    /* Thread serialization */
    
    service_mutex_unlock(&cmd->lcl_mutex);
    
    /* All done */

    return NULL;
}


/*
 * Return the number of arguments parsed on a OPAL command line handle.
 */
int service_cmd_line_get_argc(service_cmd_line_t *cmd)
{
    return (NULL != cmd) ? cmd->lcl_argc : OCOMS_ERROR;
}


/*
 * Return a string argument parsed on a OPAL command line handle.
 */
char *service_cmd_line_get_argv(service_cmd_line_t *cmd, int index)
{
    return (NULL == cmd) ? NULL :
        (index >= cmd->lcl_argc || index < 0) ? NULL : cmd->lcl_argv[index];
}


/*
 * Return the entire "tail" of unprocessed argv from a OPAL command
 * line handle.
 */
int service_cmd_line_get_tail(service_cmd_line_t *cmd, int *tailc, char ***tailv)
{
    if (NULL != cmd) {
        service_mutex_lock(&cmd->lcl_mutex);
        *tailc = cmd->lcl_tail_argc;
        *tailv = service_argv_copy(cmd->lcl_tail_argv);
        service_mutex_unlock(&cmd->lcl_mutex);
        return OCOMS_SUCCESS;
    } else {
        return OCOMS_ERROR;
    }
}


/**************************************************************************
 * Static functions
 **************************************************************************/

static void option_constructor(service_cmd_line_option_t *o)
{
    o->clo_short_name = '\0';
    o->clo_single_dash_name = NULL;
    o->clo_long_name = NULL;
    o->clo_num_params = 0;
    o->clo_description = NULL;

    o->clo_type = OCOMS_CMD_LINE_TYPE_NULL;
    o->clo_mca_param_env_var = NULL;
    o->clo_variable_dest = NULL;
    o->clo_variable_set = false;
}


static void option_destructor(service_cmd_line_option_t *o)
{
    if (NULL != o->clo_single_dash_name) {
        free(o->clo_single_dash_name);
    }
    if (NULL != o->clo_long_name) {
        free(o->clo_long_name);
    }
    if (NULL != o->clo_description) {
        free(o->clo_description);
    }
    if (NULL != o->clo_mca_param_env_var) {
        free(o->clo_mca_param_env_var);
    }
}


static void param_constructor(service_cmd_line_param_t *p)
{
    p->clp_arg = NULL;
    p->clp_option = NULL;
    p->clp_argc = 0;
    p->clp_argv = NULL;
}


static void param_destructor(service_cmd_line_param_t *p)
{
    if (NULL != p->clp_argv) {
        service_argv_free(p->clp_argv);
    }
}


static void cmd_line_constructor(service_cmd_line_t *cmd)
{
    /* Initialize the mutex.  Since we're creating (and therefore the
       only thread that has this instance), there's no need to lock it
       right now. */

    OBJ_CONSTRUCT(&cmd->lcl_mutex, service_mutex_t);

    /* Initialize the lists */

    OBJ_CONSTRUCT(&cmd->lcl_options, service_list_t);
    OBJ_CONSTRUCT(&cmd->lcl_params, service_list_t);

    /* Initialize the argc/argv pairs */

    cmd->lcl_argc = 0;
    cmd->lcl_argv = NULL;
    cmd->lcl_tail_argc = 0;
    cmd->lcl_tail_argv = NULL;
}


static void cmd_line_destructor(service_cmd_line_t *cmd)
{
    service_list_item_t *item;

    /* Free the contents of the options list (do not free the list
       itself; it was not allocated from the heap) */

    for (item = service_list_remove_first(&cmd->lcl_options);
         NULL != item;
         item = service_list_remove_first(&cmd->lcl_options)) {
        OBJ_RELEASE(item);
    }

    /* Free any parsed results */

    free_parse_results(cmd);

    /* Destroy the lists */

    OBJ_DESTRUCT(&cmd->lcl_options);
    OBJ_DESTRUCT(&cmd->lcl_params);

    /* Destroy the mutex */

    OBJ_DESTRUCT(&cmd->lcl_mutex);
}


static int make_opt(service_cmd_line_t *cmd, service_cmd_line_init_t *e)
{
    service_cmd_line_option_t *option;

    /* Bozo checks */

    if (NULL == cmd) {
        return OCOMS_ERR_BAD_PARAM;
    } else if ('\0' == e->ocl_cmd_short_name && 
               NULL == e->ocl_cmd_single_dash_name && 
               NULL == e->ocl_cmd_long_name) {
        return OCOMS_ERR_BAD_PARAM;
    } else if (e->ocl_num_params < 0) {
        return OCOMS_ERR_BAD_PARAM;
    }

    /* Allocate and fill an option item */

    option = OBJ_NEW(service_cmd_line_option_t);
    if (NULL == option) {
        return OCOMS_ERR_OUT_OF_RESOURCE;
    }

    option->clo_short_name = e->ocl_cmd_short_name;
    if (NULL != e->ocl_cmd_single_dash_name) {
        option->clo_single_dash_name = strdup(e->ocl_cmd_single_dash_name);
    }
    if (NULL != e->ocl_cmd_long_name) {
        option->clo_long_name = strdup(e->ocl_cmd_long_name);
    }
    option->clo_num_params = e->ocl_num_params;
    if (NULL != e->ocl_description) {
        option->clo_description = strdup(e->ocl_description);
    }

    option->clo_type = e->ocl_variable_type;
    option->clo_variable_dest = e->ocl_variable_dest;
    if (NULL != e->ocl_mca_type_name) {
        option->clo_mca_param_env_var = 
            ocoms_mca_base_param_environ_variable(e->ocl_mca_type_name,
                                            e->ocl_mca_component_name,
                                            e->ocl_mca_param_name);
    }

    /* Append the item, serializing thread access */

    service_mutex_lock(&cmd->lcl_mutex);
    service_list_append(&cmd->lcl_options, &option->super);
    service_mutex_unlock(&cmd->lcl_mutex);

    /* All done */

    return OCOMS_SUCCESS;
}


static void free_parse_results(service_cmd_line_t *cmd)
{
    service_list_item_t *item;

    /* Free the contents of the params list (do not free the list
       itself; it was not allocated from the heap) */

    for (item = service_list_remove_first(&cmd->lcl_params);
         NULL != item; 
         item = service_list_remove_first(&cmd->lcl_params)) {
        OBJ_RELEASE(item);
    }

    /* Free the argv's */

    if (NULL != cmd->lcl_argv) {
        service_argv_free(cmd->lcl_argv);
    }
    cmd->lcl_argv = NULL;
    cmd->lcl_argc = 0;

    if (NULL != cmd->lcl_tail_argv) {
        service_argv_free(cmd->lcl_tail_argv);
    }
    cmd->lcl_tail_argv = NULL;
    cmd->lcl_tail_argc = 0;
}


/*
 * Traverse a token and split it into individual letter options (the
 * token has already been certified to not be a long name and not be a
 * short name).  Ensure to differentiate the resulting options from
 * "single dash" names.
 */
static int split_shorts(service_cmd_line_t *cmd, char *token, char **args, 
                        int *output_argc, char ***output_argv, 
                        int *num_args_used, bool ignore_unknown)
{
    int i, j, len;
    service_cmd_line_option_t *option;
    char fake_token[3];
    int num_args;

    /* Setup that we didn't use any of the args */

    num_args = service_argv_count(args);
    *num_args_used = 0;

    /* Traverse the token.  If it's empty (e.g., if someone passes a
       "-" token, which, since the upper level calls this function as
       (argv[i] + 1), will be empty by the time it gets down here),
       just return that we didn't find a short option. */

    len = (int)strlen(token);
    if (0 == len) {
        return OCOMS_ERR_BAD_PARAM;
    }
    fake_token[0] = '-';
    fake_token[2] = '\0';
    for (i = 0; i < len; ++i) {
        fake_token[1] = token[i];
        option = find_option(cmd, fake_token + 1);

        /* If we don't find the option, either return an error or pass
           it through unmodified to the new argv */

        if (NULL == option) {
            if (!ignore_unknown) {
                return OCOMS_ERR_BAD_PARAM;
            } else {
                service_argv_append(output_argc, output_argv, fake_token);
            }
        } 

        /* If we do find the option, copy it and all of its parameters
           to the output args.  If we run out of paramters (i.e., no
           more tokens in the original argv), that error will be
           handled at a higher level) */

        else {
            service_argv_append(output_argc, output_argv, fake_token);
            for (j = 0; j < option->clo_num_params; ++j) {
                if (*num_args_used < num_args) {
                    service_argv_append(output_argc, output_argv,
                                     args[*num_args_used]);
                    ++(*num_args_used);
                } else {
                    service_argv_append(output_argc, output_argv, 
                                     special_empty_token);
                }
            }
        }
    }

    /* All done */

    return OCOMS_SUCCESS;
}


static service_cmd_line_option_t *find_option(service_cmd_line_t *cmd, 
                                      const char *option_name)
{
    service_list_item_t *item;
    service_cmd_line_option_t *option;

    /* Iterate through the list of options hanging off the
       service_cmd_line_t and see if we find a match in either the short
       or long names */

    for (item = service_list_get_first(&cmd->lcl_options);
         service_list_get_end(&cmd->lcl_options) != item;
         item = service_list_get_next(item)) {
        option = (service_cmd_line_option_t *) item;
        if ((NULL != option->clo_long_name &&
             0 == strcmp(option_name, option->clo_long_name)) ||
            (NULL != option->clo_single_dash_name &&
             0 == strcmp(option_name, option->clo_single_dash_name)) ||
            (strlen(option_name) == 1 &&
             option_name[0] == option->clo_short_name)) {
            return option;
        }
    }

    /* Not found */
    
    return NULL;
}


static void set_dest(service_cmd_line_option_t *option, char *sval)
{
    int ival = atoi(sval);
    long lval = strtol(sval, NULL, 10);
    char *str = NULL;

    /* Set MCA param.  We do this in the environment because the MCA
       parameter may not have been registered yet -- and if it isn't
       registered, we don't really want to register a dummy one
       because we don't know what it's type and default value should
       be.  These are solvable problems (e.g., make a re-registration
       overwrite everything), but it's far simpler to just leave the
       registered table alone and set an environment variable with the
       desired value.  The environment variable will get picked up
       during a nromal parameter lookup, and all will be well. */
    
    if (NULL != option->clo_mca_param_env_var) {
        switch(option->clo_type) {
        case OCOMS_CMD_LINE_TYPE_STRING:
        case OCOMS_CMD_LINE_TYPE_INT:
        case OCOMS_CMD_LINE_TYPE_SIZE_T:
            asprintf(&str, "%s=%s", option->clo_mca_param_env_var, sval);
            break;
        case OCOMS_CMD_LINE_TYPE_BOOL:
            asprintf(&str, "%s=1", option->clo_mca_param_env_var);
            break;
        default:
            break;
        }
        if (NULL != str) {
            putenv(str);
        }
    }

    /* Set variable */

    if (NULL != option->clo_variable_dest) {
        switch(option->clo_type) {
        case OCOMS_CMD_LINE_TYPE_STRING:
            *((char**) option->clo_variable_dest) = strdup(sval);
            break;
        case OCOMS_CMD_LINE_TYPE_INT:
            *((int*) option->clo_variable_dest) = ival;
            break;
        case OCOMS_CMD_LINE_TYPE_SIZE_T:
            *((size_t*) option->clo_variable_dest) = lval;
            break;
        case OCOMS_CMD_LINE_TYPE_BOOL:
            *((bool*) option->clo_variable_dest) = 1;
            break;
        default:
            break;
        }
    }
}


/*
 * Helper function to qsort_callback
 */
static void fill(const service_cmd_line_option_t *a, char result[3][BUFSIZ])
{
    int i = 0;

    result[0][0] = '\0';
    result[1][0] = '\0';
    result[2][0] = '\0';

    if ('\0' != a->clo_short_name) {
        snprintf(&result[i][0], BUFSIZ, "%c", a->clo_short_name);
        ++i;
    }
    if (NULL != a->clo_single_dash_name) {
        snprintf(&result[i][0], BUFSIZ, "%s", a->clo_single_dash_name);
        ++i;
    }
    if (NULL != a->clo_long_name) {
        snprintf(&result[i][0], BUFSIZ, "%s", a->clo_long_name);
        ++i;
    }
}


static int qsort_callback(const void *aa, const void *bb)
{
    int ret, i;
    char str1[3][BUFSIZ], str2[3][BUFSIZ];
    const service_cmd_line_option_t *a = *((const service_cmd_line_option_t**) aa);
    const service_cmd_line_option_t *b = *((const service_cmd_line_option_t**) bb);

    /* Icky comparison of command line options.  There are multiple
       forms of each command line option, so we first have to check
       which forms each option has.  Compare, in order: short name,
       single-dash name, long name. */

    fill(a, str1);
    fill(b, str2);

    for (i = 0; i < 3; ++i) {
        if (0 != (ret = strcasecmp(str1[i], str2[i]))) {
            return ret;
        }
    }

    /* Shrug -- they must be equal */

    return 0;
}
