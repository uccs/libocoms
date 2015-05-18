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
 * Copyright (c) 2007-2012 Los Alamos National Security, LLC. 
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms_config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "ocoms/util/error.h"
#include "ocoms/platform/ocoms_constants.h"

#define MAX_CONVERTERS 5
#define MAX_CONVERTER_PROJECT_LEN 10

struct converter_info_t {
    int init;
    char project[MAX_CONVERTER_PROJECT_LEN];
    int err_base;
    int err_max;
    ocoms_err2str_fn_t converter;
};
typedef struct converter_info_t converter_info_t;

/* all default to NULL */
converter_info_t converters[MAX_CONVERTERS];

static int
ocoms_strerror_int(int errnum, const char **str)
{
    int i, ret = OCOMS_SUCCESS;
    *str = NULL;

    for (i = 0 ; i < MAX_CONVERTERS ; ++i) {
        if (0 != converters[i].init &&
            errnum < converters[i].err_base &&
            converters[i].err_max < errnum) {
            ret = converters[i].converter(errnum, str);
            break;
        }
    }

    return ret;
}


/* caller must free string */
static int
ocoms_strerror_unknown(int errnum, char **str)
{
    int i;
    *str = NULL;

    for (i = 0 ; i < MAX_CONVERTERS ; ++i) {
        if (0 != converters[i].init) {
            if (errnum < converters[i].err_base && 
                errnum > converters[i].err_max) {
                asprintf(str, "Unknown error: %d (%s error %d)",
                         errnum, converters[i].project, 
                         errnum - converters[i].err_base);
                return OCOMS_SUCCESS;
            }
        }
    }

    asprintf(str, "Unknown error: %d", errnum);

    return OCOMS_SUCCESS;
}


void
ocoms_perror(int errnum, const char *msg)
{
    int ret;
    const char* errmsg;
    ret = ocoms_strerror_int(errnum, &errmsg);

    if (NULL != msg && errnum != OCOMS_ERR_IN_ERRNO) {
        fprintf(stderr, "%s: ", msg);
    }

    if (OCOMS_SUCCESS != ret) {
        if (errnum == OCOMS_ERR_IN_ERRNO) {
            perror(msg);
        } else {
            char *ue_msg;
            ret = ocoms_strerror_unknown(errnum, &ue_msg);
            fprintf(stderr, "%s\n", ue_msg);
            free(ue_msg);
        }
    } else {
        fprintf(stderr, "%s\n", errmsg);
    }

    fflush(stderr);
}

/* big enough to hold long version */
#define UNKNOWN_RETBUF_LEN 50
static char unknown_retbuf[UNKNOWN_RETBUF_LEN];

const char *
ocoms_strerror(int errnum)
{
    int ret;
    const char* errmsg;

    if (errnum == OCOMS_ERR_IN_ERRNO) {
        return strerror(errno);
    }

    ret = ocoms_strerror_int(errnum, &errmsg);

    if (OCOMS_SUCCESS != ret) {
        char *ue_msg;
        ret = ocoms_strerror_unknown(errnum, &ue_msg);
        snprintf(unknown_retbuf, UNKNOWN_RETBUF_LEN, "%s", ue_msg);
        free(ue_msg);
        errno = EINVAL;
        return (const char*) unknown_retbuf;
    } else {
        return errmsg;
    }
}


int
ocoms_strerror_r(int errnum, char *strerrbuf, size_t buflen)
{
    const char* errmsg;
    int ret, len;

    ret = ocoms_strerror_int(errnum, &errmsg);
    if (OCOMS_SUCCESS != ret) {
        if (errnum == OCOMS_ERR_IN_ERRNO) {
            char *tmp = strerror(errno);
            strncpy(strerrbuf, tmp, buflen);
            return OCOMS_SUCCESS;
        } else {
            char *ue_msg;
            ret = ocoms_strerror_unknown(errnum, &ue_msg);
            len =  snprintf(strerrbuf, buflen, "%s", ue_msg);
            free(ue_msg);
            if (len > (int) buflen) {
                errno = ERANGE;
                return OCOMS_ERR_OUT_OF_RESOURCE;
            } else {
                errno = EINVAL;
                return OCOMS_SUCCESS;
            }
        }
    } else {
        len =  snprintf(strerrbuf, buflen, "%s", errmsg);
        if (len > (int) buflen) {
            errno = ERANGE;
            return OCOMS_ERR_OUT_OF_RESOURCE;
        } else {
            return OCOMS_SUCCESS;
        }
    }
}


int
ocoms_error_register(const char *project, int err_base, int err_max,
                    ocoms_err2str_fn_t converter)
{
    int i;

    for (i = 0 ; i < MAX_CONVERTERS ; ++i) {
        if (0 == converters[i].init) {
            converters[i].init = 1;
            strncpy(converters[i].project, project, MAX_CONVERTER_PROJECT_LEN);
            converters[i].project[MAX_CONVERTER_PROJECT_LEN-1] = '\0';
            converters[i].err_base = err_base;
            converters[i].err_max = err_max;
            converters[i].converter = converter;
            return OCOMS_SUCCESS;
        }
    }

    return OCOMS_ERR_OUT_OF_RESOURCE;
}
