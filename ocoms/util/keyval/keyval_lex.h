/* -*- C -*-
 *
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

#ifndef OCOMS_UTIL_KEYVAL_LEX_H_
#define OCOMS_UTIL_KEYVAL_LEX_H_

#include "ocoms/platform/ocoms_config.h"

#ifdef malloc
#undef malloc
#endif
#ifdef realloc
#undef realloc
#endif
#ifdef free
#undef free
#endif

#include <stdio.h>

int ocoms_util_keyval_yylex(void);
int ocoms_util_keyval_init_buffer(FILE *file);

extern FILE *ocoms_util_keyval_yyin;
extern bool ocoms_util_keyval_parse_done;
extern char *ocoms_util_keyval_yytext;
extern int ocoms_util_keyval_yynewlines;

/*
 * Make lex-generated files not issue compiler warnings
 */
#define YY_STACK_USED 0
#define YY_ALWAYS_INTERACTIVE 0
#define YY_NEVER_INTERACTIVE 0
#define YY_MAIN 0
#define YY_NO_UNPUT 1
#define YY_SKIP_YYWRAP 1

enum {
    OCOMS_UTIL_KEYVAL_PARSE_DONE,
    OCOMS_UTIL_KEYVAL_PARSE_ERROR,

    OCOMS_UTIL_KEYVAL_PARSE_NEWLINE,
    OCOMS_UTIL_KEYVAL_PARSE_EQUAL,
    OCOMS_UTIL_KEYVAL_PARSE_SINGLE_WORD,
    OCOMS_UTIL_KEYVAL_PARSE_VALUE,

    OCOMS_UTIL_KEYVAL_PARSE_MAX
};

#endif
