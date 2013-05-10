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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

/** @file */

#ifndef OCOMS_UTIL_KEYVAL_PARSE_H
#define OCOMS_UTIL_KEYVAL_PARSE_H

#include "ocoms/platform/ocoms_config.h"

BEGIN_C_DECLS

/**
 * Callback triggered for each key = value pair
 *
 * Callback triggered from ocoms_util_keyval_parse for each key = value
 * pair.  Both key and value will be pointers into static buffers.
 * The buffers must not be free()ed and contents may be overwritten
 * immediately after the callback returns.
 */
typedef void (*ocoms_keyval_parse_fn_t)(const char *key, const char *value);

/**
 * Parse \c filename, made up of key = value pairs.
 *
 * Parse \c filename, made up of key = value pairs.  For each line
 * that appears to contain a key = value pair, \c callback will be
 * called exactly once.  In a multithreaded context, calls to
 * ocoms_util_keyval_parse() will serialize multiple calls.
 */
OCOMS_DECLSPEC int ocoms_util_keyval_parse(const char *filename, 
                                         ocoms_keyval_parse_fn_t callback);

OCOMS_DECLSPEC int ocoms_util_keyval_parse_init(void);

OCOMS_DECLSPEC int ocoms_util_keyval_parse_finalize(void);

END_C_DECLS

#endif
