/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2012 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008-2013 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012-2013 Los Alamos National Security, LLC. All rights
 *                         reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <errno.h>

#include "ocoms/platform/ocoms_stdint.h"
#if 0
#include "ocoms/util/show_help.h"
#include "ocoms/runtime/ocoms.h"
#endif
#include "ocoms/mca/mca.h"
#include "ocoms/mca/base/mca_base_vari.h"
#include "ocoms/mca/base/mca_base_pvar.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/util/output.h"
#include "ocoms/util/ocoms_environ.h"

static ocoms_pointer_array_t ocoms_mca_base_var_groups;
static ocoms_hash_table_t ocoms_mca_base_var_group_index_hash;
static int ocoms_mca_base_var_group_count = 0;
static int ocoms_mca_base_var_groups_timestamp = 0;
static bool ocoms_mca_base_var_group_initialized = false;

static void ocoms_mca_base_var_group_constructor (ocoms_mca_base_var_group_t *group);
static void ocoms_mca_base_var_group_destructor (ocoms_mca_base_var_group_t *group);
OBJ_CLASS_INSTANCE(ocoms_mca_base_var_group_t, ocoms_object_t,
                   ocoms_mca_base_var_group_constructor,
                   ocoms_mca_base_var_group_destructor);

int ocoms_mca_base_var_group_init (void)
{
    int ret;

    if (!ocoms_mca_base_var_group_initialized) {
        OBJ_CONSTRUCT(&ocoms_mca_base_var_groups, ocoms_pointer_array_t);

        /* These values are arbitrary */
        ret = ocoms_pointer_array_init (&ocoms_mca_base_var_groups, 128, 16384, 128);
        if (OCOMS_SUCCESS != ret) {
            return ret;
        }

        OBJ_CONSTRUCT(&ocoms_mca_base_var_group_index_hash, ocoms_hash_table_t);
        ret = ocoms_hash_table_init (&ocoms_mca_base_var_group_index_hash, 256);
        if (OCOMS_SUCCESS != ret) {
            return ret;
        }

        ocoms_mca_base_var_group_initialized = true;
        ocoms_mca_base_var_group_count = 0;
    }

    return OCOMS_SUCCESS;
}

int ocoms_mca_base_var_group_finalize (void)
{
    ocoms_object_t *object;
    int size, i;

    if (ocoms_mca_base_var_group_initialized) {
        size = ocoms_pointer_array_get_size(&ocoms_mca_base_var_groups);
        for (i = 0 ; i < size ; ++i) {
            object = ocoms_pointer_array_get_item (&ocoms_mca_base_var_groups, i);
            if (NULL != object) {
                OBJ_RELEASE(object);
            }
        }
        OBJ_DESTRUCT(&ocoms_mca_base_var_groups);
        OBJ_DESTRUCT(&ocoms_mca_base_var_group_index_hash);
        ocoms_mca_base_var_group_count = 0;
        ocoms_mca_base_var_group_initialized = false;
    }

    return OCOMS_SUCCESS;
}

int ocoms_mca_base_var_group_get_internal (const int group_index, ocoms_mca_base_var_group_t **group, bool invalidok)
{
    if (group_index < 0) {
        return OCOMS_ERR_NOT_FOUND;
    }

    *group = (ocoms_mca_base_var_group_t *) ocoms_pointer_array_get_item (&ocoms_mca_base_var_groups,
                                                                   group_index);
    if (NULL == *group || (!invalidok && !(*group)->group_isvalid)) {
        *group = NULL;
        return OCOMS_ERR_NOT_FOUND;
    }

    return OCOMS_SUCCESS;
}

static int group_find_by_name (const char *full_name, int *index, bool invalidok)
{
    ocoms_mca_base_var_group_t *group;
    void *tmp;
    int rc;

    rc = ocoms_hash_table_get_value_ptr (&ocoms_mca_base_var_group_index_hash, full_name,
                                        strlen (full_name), &tmp);
    if (OCOMS_SUCCESS != rc) {
        return rc;
    }

    rc = ocoms_mca_base_var_group_get_internal ((int)(uintptr_t) tmp, &group, false);
    if (OCOMS_SUCCESS != rc) {
        return rc;
    }

    if (invalidok || group->group_isvalid) {
        *index = (int)(uintptr_t) tmp;
        return OCOMS_SUCCESS;
    }

    return OCOMS_ERR_NOT_FOUND;
}

static int group_find (const char *project_name, const char *framework_name,
                       const char *component_name, bool invalidok)
{
    char *full_name;
    int ret, index=0;

    if (!ocoms_mca_base_var_initialized) {
        return OCOMS_ERR_NOT_FOUND;
    }

    /* TODO -- deal with the project name correctly (including the wildcard '*') */
    project_name = NULL;

    ret = ocoms_mca_base_var_generate_full_name4(project_name, framework_name, component_name,
                                           NULL, &full_name);
    if (OCOMS_SUCCESS != ret) {
        return OCOMS_ERROR;
    }

    ret = group_find_by_name(full_name, &index, invalidok);
    free (full_name);

    return (0 > ret) ? ret : index;
}

static int group_register (const char *project_name, const char *framework_name,
                           const char *component_name, const char *description)
{
    ocoms_mca_base_var_group_t *group;
    int group_id, parent_id = -1;
    int ret;

    if (NULL == project_name && NULL == framework_name && NULL == component_name) {
        /* don't create a group with no name (maybe we should create a generic group?) */
        return -1;
    }

    /* XXX -- remove this once the project name is available in the component structure */
    if (framework_name || component_name) {
        project_name = NULL;
    }

    group_id = group_find (project_name, framework_name, component_name, true);
    if (0 <= group_id) {
        (void) ocoms_mca_base_var_group_get_internal (group_id, &group, true);
        group->group_isvalid = true;
        ocoms_mca_base_var_groups_timestamp++;

        /* group already exists. return it's index */
        return group_id;
    }

    group = OBJ_NEW(ocoms_mca_base_var_group_t);

    group->group_isvalid = true;

    if (NULL != project_name) {
        group->group_project = strdup (project_name);
        if (NULL == group->group_project) {
            OBJ_RELEASE(group);
            return OCOMS_ERR_OUT_OF_RESOURCE;
        }
    }
    if (NULL != framework_name) {
        group->group_framework = strdup (framework_name);
        if (NULL == group->group_framework) {
            OBJ_RELEASE(group);
            return OCOMS_ERR_OUT_OF_RESOURCE;
        }
    }
    if (NULL != component_name) {
        group->group_component = strdup (component_name);
        if (NULL == group->group_component) {
            OBJ_RELEASE(group);
            return OCOMS_ERR_OUT_OF_RESOURCE;
        }
    }
    if (NULL != description) {
        group->group_description = strdup (description);
        if (NULL == group->group_description) {
            OBJ_RELEASE(group);
            return OCOMS_ERR_OUT_OF_RESOURCE;
        }
    }

    if (NULL != framework_name && NULL != component_name) {
        if (component_name) {
            parent_id = group_register (project_name, framework_name, NULL, NULL);
        } else if (framework_name && project_name) {
            parent_id = group_register (project_name, NULL, NULL, NULL);
        }
    }

    /* avoid groups of the form ocoms_ocoms, ompi_ompi, etc */
    if (NULL != project_name && NULL != framework_name &&
        (0 == strcmp (project_name, framework_name))) {
        project_name = NULL;
    }

    /* build the group name */
    ret = ocoms_mca_base_var_generate_full_name4 (NULL, project_name, framework_name, component_name,
                                            &group->group_full_name);
    if (OCOMS_SUCCESS != ret) {
        OBJ_RELEASE(group);
        return ret;
    }

    group_id = ocoms_pointer_array_add (&ocoms_mca_base_var_groups, group);
    if (0 > group_id) {
        OBJ_RELEASE(group);
        return OCOMS_ERROR;
    }

    ocoms_hash_table_set_value_ptr (&ocoms_mca_base_var_group_index_hash, group->group_full_name,
                                   strlen (group->group_full_name), (void *)(uintptr_t) group_id);

    ocoms_mca_base_var_group_count++;
    ocoms_mca_base_var_groups_timestamp++;

    if (0 <= parent_id) {
        ocoms_mca_base_var_group_t *parent_group;

        (void) ocoms_mca_base_var_group_get_internal(parent_id, &parent_group, false);
        ocoms_value_array_append_item (&parent_group->group_subgroups, &group_id);
    }

    return group_id;
}

int ocoms_mca_base_var_group_register (const char *project_name, const char *framework_name,
                                 const char *component_name, const char *description)
{
    return group_register (project_name, framework_name, component_name, description);
}

int ocoms_mca_base_var_group_component_register (const ocoms_mca_base_component_t *component,
                                           const char *description)
{
    /* 1.7 components do not store the project */
    return group_register (NULL, component->mca_type_name,
                           component->mca_component_name, description);
}


int ocoms_mca_base_var_group_deregister (int group_index)
{
    ocoms_mca_base_var_group_t *group;
    int size, i, ret;
    int *params, *subgroups;

    ret = ocoms_mca_base_var_group_get_internal (group_index, &group, false);
    if (OCOMS_SUCCESS != ret) {
        return ret;
    }

    group->group_isvalid = false;

    /* deregister all associated mca parameters */
    size = ocoms_value_array_get_size(&group->group_vars);
    params = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_vars, int);

    for (i = 0 ; i < size ; ++i) {
        const ocoms_mca_base_var_t *var;

        ret = ocoms_mca_base_var_get (params[i], &var);
        if (OCOMS_SUCCESS != ret || !(var->mbv_flags & MCA_BASE_VAR_FLAG_DWG)) {
            continue;
        }

        (void) ocoms_mca_base_var_deregister (params[i]);
    }
    OBJ_DESTRUCT(&group->group_vars);
    OBJ_CONSTRUCT(&group->group_vars, ocoms_value_array_t);

    /* invalidate all associated mca performance variables */
    size = ocoms_value_array_get_size(&group->group_pvars);
    params = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_pvars, int);

    for (i = 0 ; i < size ; ++i) {
        const ocoms_mca_base_pvar_t *var;

        ret = ocoms_mca_base_pvar_get (params[i], &var);
        if (OCOMS_SUCCESS != ret || !(var->flags & MCA_BASE_PVAR_FLAG_IWG)) {
            continue;
        }

        (void) ocoms_mca_base_pvar_mark_invalid (params[i]);
    }
    OBJ_DESTRUCT(&group->group_pvars);
    OBJ_CONSTRUCT(&group->group_pvars, ocoms_value_array_t);

    size = ocoms_value_array_get_size(&group->group_subgroups);
    subgroups = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_subgroups, int);
    for (i = 0 ; i < size ; ++i) {
        (void) ocoms_mca_base_var_group_deregister (subgroups[i]);
    }
    OBJ_DESTRUCT(&group->group_subgroups);
    OBJ_CONSTRUCT(&group->group_subgroups, ocoms_value_array_t);

    ocoms_mca_base_var_groups_timestamp++;

    return OCOMS_SUCCESS;
}

int ocoms_mca_base_var_group_find (const char *project_name,
                             const char *framework_name,
                             const char *component_name)
{
    return group_find (project_name, framework_name, component_name, false);
}

int ocoms_mca_base_var_group_find_by_name (const char *full_name, int *index)
{
    return group_find_by_name (full_name, index, false);
}

int ocoms_mca_base_var_group_add_var (const int group_index, const int param_index)
{
    ocoms_mca_base_var_group_t *group;
    int size, i, ret;
    int *params;

    ret = ocoms_mca_base_var_group_get_internal (group_index, &group, false);
    if (OCOMS_SUCCESS != ret) {
        return ret;
    }

    size = ocoms_value_array_get_size(&group->group_vars);
    params = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_vars, int);
    for (i = 0 ; i < size ; ++i) {
        if (params[i] == param_index) {
            return i;
        }
    }

    if (OCOMS_SUCCESS !=
        (ret = ocoms_value_array_append_item (&group->group_vars, &param_index))) {
        return ret;
    }

    ocoms_mca_base_var_groups_timestamp++;

    /* return the group index */
    return (int) ocoms_value_array_get_size (&group->group_vars) - 1;
}

int ocoms_mca_base_var_group_add_pvar (const int group_index, const int param_index)
{
    ocoms_mca_base_var_group_t *group;
    int size, i, ret;
    int *params;

    ret = ocoms_mca_base_var_group_get_internal (group_index, &group, false);
    if (OCOMS_SUCCESS != ret) {
        return ret;
    }

    size = ocoms_value_array_get_size(&group->group_pvars);
    params = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_pvars, int);
    for (i = 0 ; i < size ; ++i) {
        if (params[i] == param_index) {
            return i;
        }
    }

    if (OCOMS_SUCCESS !=
        (ret = ocoms_value_array_append_item (&group->group_pvars, &param_index))) {
        return ret;
    }

    ocoms_mca_base_var_groups_timestamp++;

    /* return the group index */
    return (int) ocoms_value_array_get_size (&group->group_pvars) - 1;
}

int ocoms_mca_base_var_group_get (const int group_index, const ocoms_mca_base_var_group_t **group)
{
    return ocoms_mca_base_var_group_get_internal (group_index, (ocoms_mca_base_var_group_t **) group, false);
}

int ocoms_mca_base_var_group_set_var_flag (const int group_index, int flags, bool set)
{
    ocoms_mca_base_var_group_t *group;
    int size, i, ret;
    int *vars;

    ret = ocoms_mca_base_var_group_get_internal (group_index, &group, false);
    if (OCOMS_SUCCESS != ret) {
        return ret;
    }

    /* set the flag on each valid variable */
    size = ocoms_value_array_get_size(&group->group_vars);
    vars = OCOMS_VALUE_ARRAY_GET_BASE(&group->group_vars, int);

    for (i = 0 ; i < size ; ++i) {
        if (0 <= vars[i]) {
            (void) ocoms_mca_base_var_set_flag (vars[i], flags, set);
        }
    }

    return OCOMS_SUCCESS;
}


static void ocoms_mca_base_var_group_constructor (ocoms_mca_base_var_group_t *group)
{
    memset ((char *) group + sizeof (group->super), 0, sizeof (*group) - sizeof (group->super));

    OBJ_CONSTRUCT(&group->group_subgroups, ocoms_value_array_t);
    ocoms_value_array_init (&group->group_subgroups, sizeof (int));

    OBJ_CONSTRUCT(&group->group_vars, ocoms_value_array_t);
    ocoms_value_array_init (&group->group_vars, sizeof (int));

    OBJ_CONSTRUCT(&group->group_pvars, ocoms_value_array_t);
    ocoms_value_array_init (&group->group_pvars, sizeof (int));
}

static void ocoms_mca_base_var_group_destructor (ocoms_mca_base_var_group_t *group)
{
    free (group->group_full_name);
    group->group_full_name = NULL;

    free (group->group_description);
    group->group_description = NULL;

    free (group->group_project);
    group->group_project = NULL;

    free (group->group_framework);
    group->group_framework = NULL;

    free (group->group_component);
    group->group_component = NULL;

    OBJ_DESTRUCT(&group->group_subgroups);
    OBJ_DESTRUCT(&group->group_vars);
    OBJ_DESTRUCT(&group->group_pvars);
}

int ocoms_mca_base_var_group_get_count (void)
{
    return ocoms_mca_base_var_group_count;
}

int ocoms_mca_base_var_group_get_stamp (void)
{
    return ocoms_mca_base_var_groups_timestamp;
}
