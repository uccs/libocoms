
#include "ocoms/platform/ocoms_config.h"

#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/util/keyval_parse.h"
#include "ocoms/util/keyval/keyval_lex.h"
#include "ocoms/util/output.h"
#include "ocoms/threads/mutex.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif  /* HAVE_STRING_H */

static const char *keyval_filename;
static ocoms_keyval_parse_fn_t keyval_callback;
static char *key_buffer = NULL;
static size_t key_buffer_len = 0;
static ocoms_mutex_t keyval_mutex;

static int parse_line(void);
static void parse_error(int num);

int ocoms_util_keyval_parse_init(void)
{
    OBJ_CONSTRUCT(&keyval_mutex, ocoms_mutex_t);

    return OCOMS_SUCCESS;
}


int
ocoms_util_keyval_parse_finalize(void)
{
    if (NULL != key_buffer) free(key_buffer);

    OBJ_DESTRUCT(&keyval_mutex);

    return OCOMS_SUCCESS;
}


int
ocoms_util_keyval_parse(const char *filename,
                       ocoms_keyval_parse_fn_t callback)
{
    int val;
    int ret = OCOMS_SUCCESS;;

    OCOMS_THREAD_LOCK(&keyval_mutex);

    keyval_filename = filename;
    keyval_callback = callback;

    /* Open the opal */
    ocoms_util_keyval_yyin = fopen(keyval_filename, "r");
    if (NULL == ocoms_util_keyval_yyin) {
        ret = OCOMS_ERR_NOT_FOUND;
        goto cleanup;
    }

    ocoms_util_keyval_parse_done = false;
    ocoms_util_keyval_yynewlines = 1;
    ocoms_util_keyval_init_buffer(ocoms_util_keyval_yyin);
    while (!ocoms_util_keyval_parse_done) {
        val = ocoms_util_keyval_yylex();
        switch (val) {
        case OCOMS_UTIL_KEYVAL_PARSE_DONE:
            /* This will also set ocoms_util_keyval_parse_done to true, so just
               break here */
            break;

        case OCOMS_UTIL_KEYVAL_PARSE_NEWLINE:
            /* blank line!  ignore it */
            break;

        case OCOMS_UTIL_KEYVAL_PARSE_SINGLE_WORD:
            parse_line();
            break;

        default:
            /* anything else is an error */
            parse_error(1);
            break;
        }
    }
    fclose(ocoms_util_keyval_yyin);
    ocoms_util_keyval_yylex_destroy ();

cleanup:
    OCOMS_THREAD_UNLOCK(&keyval_mutex);
    return ret;
}



static int parse_line(void)
{
    int val;

    /* Save the name name */
    if (key_buffer_len < strlen(ocoms_util_keyval_yytext) + 1) {
        char *tmp;
        key_buffer_len = strlen(ocoms_util_keyval_yytext) + 1;
        tmp = (char*)realloc(key_buffer, key_buffer_len);
        if (NULL == tmp) {
            free(key_buffer);
            key_buffer_len = 0;
            key_buffer = NULL;
            return OCOMS_ERR_TEMP_OUT_OF_RESOURCE;
        }
        key_buffer = tmp;
    }

    strncpy(key_buffer, ocoms_util_keyval_yytext, key_buffer_len);

    /* The first thing we have to see is an "=" */

    val = ocoms_util_keyval_yylex();
    if (ocoms_util_keyval_parse_done || OCOMS_UTIL_KEYVAL_PARSE_EQUAL != val) {
        parse_error(2);
        return OCOMS_ERROR;
    }

    /* Next we get the value */

    val = ocoms_util_keyval_yylex();
    if (OCOMS_UTIL_KEYVAL_PARSE_SINGLE_WORD == val ||
        OCOMS_UTIL_KEYVAL_PARSE_VALUE == val) {
        keyval_callback(key_buffer, ocoms_util_keyval_yytext);

        /* Now we need to see the newline */

        val = ocoms_util_keyval_yylex();
        if (OCOMS_UTIL_KEYVAL_PARSE_NEWLINE == val ||
            OCOMS_UTIL_KEYVAL_PARSE_DONE == val) {
            return OCOMS_SUCCESS;
        }
    }

    /* Did we get an EOL or EOF? */

    else if (OCOMS_UTIL_KEYVAL_PARSE_DONE == val ||
             OCOMS_UTIL_KEYVAL_PARSE_NEWLINE == val) {
        keyval_callback(key_buffer, NULL);
        return OCOMS_SUCCESS;
    }

    /* Nope -- we got something unexpected.  Bonk! */
    parse_error(3);
    return OCOMS_ERROR;
}


static void parse_error(int num)
{
    /* JMS need better error/warning message here */
    ocoms_output(0, "keyval parser: error %d reading file %s at line %d:\n  %s\n",
                num, keyval_filename, ocoms_util_keyval_yynewlines, ocoms_util_keyval_yytext);
}
