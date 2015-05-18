/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006-2012 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2010-2013 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2011-2012 University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ocoms_config.h"

#include <string.h>
#include <ctype.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>

#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/ocoms_pointer_array.h"

#include "ocoms/util/output.h"
#include "ocoms/util/cmd_line.h"
#include "ocoms/util/argv.h"
#include "ocoms/util/error.h"
#include "ocoms/mca/base/mca_base_pvar.h"

#include "ocoms/util/ocoms_info_support.h"

bool ocoms_info_pretty = true;

const char *ocoms_info_type_all = "all";
const char *ocoms_info_type_ocoms = "ocoms";
const char *ocoms_info_component_all = "all";
const char *ocoms_info_param_all = "all";

int ocoms_info_init(int argc, char **argv,
                   ocoms_cmd_line_t *ocoms_info_cmd_line)
{
    int ret;
    bool want_help = false;
    bool cmd_error = false;
    char **app_env = NULL, **global_env = NULL;

    /* add the cmd line options */
    ocoms_cmd_line_make_opt3(ocoms_info_cmd_line, '\0', NULL, "param", 2,
                            "Show MCA parameters.  The first parameter is the framework (or the keyword \"all\"); the second parameter is the specific component name (or the keyword \"all\").");
    ocoms_cmd_line_make_opt3(ocoms_info_cmd_line, 'a', NULL, "all", 0,
                             "Show all MCA parameters.");
    ocoms_cmd_line_make_opt3(ocoms_info_cmd_line, '\0', NULL, "params", 2,
                            "Synonym for --param");
    ocoms_cmd_line_make_opt3(ocoms_info_cmd_line, 'v', NULL, "version", 0,
                            "Show product version.");
    ocoms_cmd_line_make_opt3(ocoms_info_cmd_line, 'h', NULL, "help", 0,
                            "Show this help message");
    /* set our threading level */
    ocoms_set_using_threads(false);

    /* Initialize the ocoms_output system */
    if (!ocoms_output_init()) {
        return OCOMS_ERROR;
    }

    /* Do the parsing */
    ret = ocoms_cmd_line_parse(ocoms_info_cmd_line, false, argc, argv);
    if (OCOMS_SUCCESS != ret) {
        cmd_error = true;
        if (OCOMS_ERR_SILENT != ret) {
            fprintf(stderr, "%s: command line error (%s)\n", argv[0],
                    ocoms_strerror(ret));
        }
    }
    if (!cmd_error &&
        (ocoms_cmd_line_is_taken(ocoms_info_cmd_line, "help") ||
         ocoms_cmd_line_is_taken(ocoms_info_cmd_line, "h"))) {
        char *usage;

        want_help = true;
        usage = ocoms_cmd_line_get_usage_msg(ocoms_info_cmd_line);
        printf("%s\n", usage);
//        char* str;
//        str = ocoms_show_help_string("help-ocoms_info.txt", "usage",
//                                    true, usage);
//        if (NULL != str) {
//            printf("%s", str);
//            free(str);
//        }
        free(usage);
    }

    /* If we had a cmd line parse error, or we showed the help
       message, it's time to exit. */
    if (cmd_error || want_help) {
        OBJ_RELEASE(ocoms_info_cmd_line);
        exit(cmd_error ? 1 : 0);
    }

    ocoms_mca_base_cmd_line_process_args(ocoms_info_cmd_line, &app_env, &global_env);
    return OCOMS_SUCCESS;
}

void ocoms_info_finalize(void)
{
}

void ocoms_info_do_params(bool want_all_in, bool want_internal,
                         ocoms_pointer_array_t *mca_types,
                         ocoms_cmd_line_t *ocoms_info_cmd_line)
{
    ocoms_mca_base_var_info_lvl_t max_level = OCOMS_INFO_LVL_9;
    int count;
    char *type, *component, *str;
    bool found;
    int i;
    bool want_all = false;
    char *p;

    if (ocoms_cmd_line_is_taken(ocoms_info_cmd_line, "param")) {
        p = "param";
    } else if (ocoms_cmd_line_is_taken(ocoms_info_cmd_line, "params")) {
        p = "params";
    } else {
        p = "foo";  /* should never happen, but protect against segfault */
    }

    if (NULL != (str = ocoms_cmd_line_get_param (ocoms_info_cmd_line, "level", 0, 0))) {
        char *tmp;

        errno = 0;
        max_level = strtol (str, &tmp, 10) + OCOMS_INFO_LVL_1 - 1;
        if (0 != errno || '\0' != tmp[0] || max_level < OCOMS_INFO_LVL_1 || max_level > OCOMS_INFO_LVL_9) {
            char *usage = ocoms_cmd_line_get_usage_msg(ocoms_info_cmd_line);
//            ocoms_show_help("help-ocoms_info.txt", "invalid-level", true, str);
            free(usage);
            exit(1);
        }
    } else if (want_all_in) {
        /* if not specified default to level 9 if all components are requested */
        max_level = OCOMS_INFO_LVL_9;
    }

    if (want_all_in) {
        want_all = true;
    } else {
        /* See if the special param "all" was given to --param; that
         * supercedes any individual type
         */
        count = ocoms_cmd_line_get_ninsts(ocoms_info_cmd_line, p);
        for (i = 0; i < count; ++i) {
            type = ocoms_cmd_line_get_param(ocoms_info_cmd_line, p, (int)i, 0);
            if (0 == strcmp(ocoms_info_type_all, type)) {
                want_all = true;
                break;
            }
        }
    }

    /* Show the params */

    if (want_all) {
        for (i = 0; i < mca_types->size; ++i) {
            if (NULL == (type = (char *)ocoms_pointer_array_get_item(mca_types, i))) {
                continue;
            }
            ocoms_info_show_mca_params(type, ocoms_info_component_all, max_level, want_internal);
        }
    } else {
        for (i = 0; i < count; ++i) {
            type = ocoms_cmd_line_get_param(ocoms_info_cmd_line, p, (int)i, 0);
            component = ocoms_cmd_line_get_param(ocoms_info_cmd_line, p, (int)i, 1);

            for (found = false, i = 0; i < mca_types->size; ++i) {
                if (NULL == (str = (char *)ocoms_pointer_array_get_item(mca_types, i))) {
                    continue;
                }
                if (0 == strcmp(str, type)) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                char *usage = ocoms_cmd_line_get_usage_msg(ocoms_info_cmd_line);
//                ocoms_show_help("help-ocoms_info.txt", "not-found", true, type);
                free(usage);
                exit(1);
            }

            ocoms_info_show_mca_params(type, component, max_level, want_internal);
        }
    }
}

static void ocoms_info_show_mca_group_params(const ocoms_mca_base_var_group_t *group, ocoms_mca_base_var_info_lvl_t max_level, bool want_internal)
{
    const int *variables, *groups;
    const ocoms_mca_base_pvar_t *pvar;
    const char *group_component;
    const ocoms_mca_base_var_t *var;
    char **strings, *message;
    bool requested = true;
    int ret, i, j, count;

    variables = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_vars, const int);
    count = ocoms_value_array_get_size((ocoms_value_array_t *)&group->group_vars);
    /* the default component name is "base". depending on how the
     * group was registered the group may or not have this set.  */
    group_component = group->group_component ? group->group_component : "base";

    /* check if this group may be disabled due to a selection variable */
    if (0 != strcmp (group_component, "base")) {
        int var_id;
        /* read the selection parameter */
        var_id = ocoms_mca_base_var_find (group->group_project, group->group_framework, NULL, NULL);
        if (0 <= var_id) {
            const ocoms_mca_base_var_storage_t *value;
            char **requested_components;
            bool include_mode;

            ocoms_mca_base_var_get_value (var_id, &value, NULL, NULL);
            if (NULL != value->stringval && '\0' != value->stringval[0]) {
                ocoms_mca_base_component_parse_requested (value->stringval, &include_mode, &requested_components);

                for (i = 0, requested = !include_mode ; requested_components[i] ; ++i) {
                    if (0 == strcmp (requested_components[i], group->group_component)) {
                        requested = include_mode;
                        break;
                    }
                }

                ocoms_argv_free (requested_components);
            }
        }
    }

    for (i = 0 ; i < count ; ++i) {
        ret = ocoms_mca_base_var_get(variables[i], &var);
        if (OCOMS_SUCCESS != ret || ((var->mbv_flags & MCA_BASE_VAR_FLAG_INTERNAL) &&
                                    !want_internal) ||
            max_level < var->mbv_info_lvl) {
            continue;
        }
        ret = ocoms_mca_base_var_dump(variables[i], &strings, !ocoms_info_pretty ? MCA_BASE_VAR_DUMP_PARSABLE : MCA_BASE_VAR_DUMP_READABLE);
        if (OCOMS_SUCCESS != ret) {
            continue;
        }

        for (j = 0 ; strings[j] ; ++j) {
            if (0 == j && ocoms_info_pretty) {
                asprintf (&message, "MCA%s %s", requested ? "" : " (disabled)", group->group_framework);
                ocoms_info_out(message, message, strings[j]);
                free(message);
            } else {
                ocoms_info_out("", "", strings[j]);
            }
            free(strings[j]);
        }
        if (!ocoms_info_pretty) {
            /* generate an entry indicating whether this variable is disabled or not. if the
             * format in mca_base_var/pvar.c changes this needs to be changed as well */
            asprintf (&message, "mca:%s:%s:param:%s:disabled:%s", group->group_framework,
                      group_component, var->mbv_full_name, requested ? "false" : "true");
            ocoms_info_out("", "", message);
            free (message);
        }
        free(strings);
    }

    variables = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_pvars, const int);
    count = ocoms_value_array_get_size((ocoms_value_array_t *)&group->group_pvars);

    for (i = 0 ; i < count ; ++i) {
        ret = ocoms_mca_base_pvar_get(variables[i], &pvar);
        if (OCOMS_SUCCESS != ret || max_level < pvar->verbosity) {
            continue;
        }

        ret = ocoms_mca_base_pvar_dump (variables[i], &strings, !ocoms_info_pretty ? MCA_BASE_VAR_DUMP_PARSABLE : MCA_BASE_VAR_DUMP_READABLE);
        if (OCOMS_SUCCESS != ret) {
            continue;
        }

        for (j = 0 ; strings[j] ; ++j) {
            if (0 == j && ocoms_info_pretty) {
                asprintf (&message, "MCA%s %s", requested ? "" : " (disabled)", group->group_framework);
                ocoms_info_out(message, message, strings[j]);
                free(message);
            } else {
                ocoms_info_out("", "", strings[j]);
            }
            free(strings[j]);
        }
        if (!ocoms_info_pretty) {
            /* generate an entry indicating whether this variable is disabled or not. if the
             * format in mca_base_var/pvar.c changes this needs to be changed as well */
            asprintf (&message, "mca:%s:%s:pvar:%s:disabled:%s", group->group_framework,
                      group_component, pvar->name, requested ? "false" : "true");
            ocoms_info_out("", "", message);
            free (message);
        }
        free(strings);
    }

    groups = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_subgroups, const int);
    count = ocoms_value_array_get_size((ocoms_value_array_t *)&group->group_subgroups);

    for (i = 0 ; i < count ; ++i) {
        ret = ocoms_mca_base_var_group_get(groups[i], &group);
        if (OCOMS_SUCCESS != ret) {
            continue;
        }
        ocoms_info_show_mca_group_params(group, max_level, want_internal);
    }
}

void ocoms_info_show_mca_params(const char *type, const char *component,
                               ocoms_mca_base_var_info_lvl_t max_level, bool want_internal)
{
    const ocoms_mca_base_var_group_t *group;
    int ret;

    if (0 == strcmp (component, "all")) {
        ret = ocoms_mca_base_var_group_find("*", type, NULL);
        if (0 > ret) {
            return;
        }

        (void) ocoms_mca_base_var_group_get(ret, &group);

        ocoms_info_show_mca_group_params(group, max_level, want_internal);
    } else {
        ret = ocoms_mca_base_var_group_find("*", type, component);
        if (0 > ret) {
            return;
        }

        (void) ocoms_mca_base_var_group_get(ret, &group);
        ocoms_info_show_mca_group_params(group, max_level, want_internal);
    }
}

/*
 * Private variables - set some reasonable screen size defaults
 */

static int centerpoint = 24;
static int screen_width = 78;

/*
 * Prints the passed integer in a pretty or parsable format.
 */
void ocoms_info_out(const char *pretty_message, const char *plain_message, const char *value)
{
    size_t len, max_value_width, value_offset;
    char *spaces = NULL;
    char *filler = NULL;
    char *pos, *v, savev, *v_to_free;

#ifdef HAVE_ISATTY
    /* If we have isatty(), if this is not a tty, then disable
     * wrapping for grep-friendly behavior
     */
    if (0 == isatty(STDOUT_FILENO)) {
        screen_width = INT_MAX;
    }
#endif

#ifdef TIOCGWINSZ
    if (screen_width < INT_MAX) {
        struct winsize size;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, (char*) &size) >= 0) {
            screen_width = size.ws_col;
        }
    }
#endif

    /* Strip leading and trailing whitespace from the string value */
    value_offset = strspn(value, " ");

    v = v_to_free = strdup(value + value_offset);
    len = strlen(v);

    if (len > 0) {
        while (len > 0 && isspace(v[len-1])) len--;
        v[len] = '\0';
    }

    if (ocoms_info_pretty && NULL != pretty_message) {
        if (centerpoint > (int)strlen(pretty_message)) {
            asprintf(&spaces, "%*s", centerpoint -
                     (int)strlen(pretty_message), " ");
        } else {
            spaces = strdup("");
#if OCOMS_ENABLE_DEBUG
            if (centerpoint < (int)strlen(pretty_message)) {
//                ocoms_show_help("help-ocoms_info.txt",
//                               "developer warning: field too long", false,
//                               pretty_message, centerpoint);
            }
#endif
        }
        max_value_width = screen_width - strlen(spaces) - strlen(pretty_message) - 2;
        if (0 < strlen(pretty_message)) {
            asprintf(&filler, "%s%s: ", spaces, pretty_message);
        } else {
            asprintf(&filler, "%s  ", spaces);
        }
        free(spaces);
        spaces = NULL;

        while (true) {
            if (strlen(v) < max_value_width) {
                printf("%s%s\n", filler, v);
                break;
            } else {
                asprintf(&spaces, "%*s", centerpoint + 2, " ");

                /* Work backwards to find the first space before
                 * max_value_width
                 */
                savev = v[max_value_width];
                v[max_value_width] = '\0';
                pos = (char*)strrchr(v, (int)' ');
                v[max_value_width] = savev;
                if (NULL == pos) {
                    /* No space found < max_value_width.  Look for the first
                     * space after max_value_width.
                     */
                    pos = strchr(&v[max_value_width], ' ');

                    if (NULL == pos) {

                        /* There's just no spaces.  So just print it and be done. */

                        printf("%s%s\n", filler, v);
                        break;
                    } else {
                        *pos = '\0';
                        printf("%s%s\n", filler, v);
                        v = pos + 1;
                    }
                } else {
                    *pos = '\0';
                    printf("%s%s\n", filler, v);
                    v = pos + 1;
                }

                /* Reset for the next iteration */
                free(filler);
                filler = strdup(spaces);
                free(spaces);
                spaces = NULL;
            }
        }
        if (NULL != filler) {
            free(filler);
        }
        if (NULL != spaces) {
            free(spaces);
        }
    } else {
        if (NULL != plain_message && 0 < strlen(plain_message)) {
            printf("%s:%s\n", plain_message, value);
        } else {
            printf("%s\n", value);
        }
    }
    if (NULL != v_to_free) {
        free(v_to_free);
    }
}


