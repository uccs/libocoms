
#include "ccs_config.h"

#include "service/platform/service_constants.h"
#include "service/util/keyval_parse.h"
#include "service/util/keyval/keyval_lex.h"
#include "service/util/output.h"
#include "service/threads/mutex.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif  /* HAVE_STRING_H */

static const char *keyval_filename;
static service_keyval_parse_fn_t keyval_callback;
static char *key_buffer = NULL;
static size_t key_buffer_len = 0;
static service_mutex_t keyval_mutex;

static int parse_line(void);
static void parse_error(int num);

int service_util_keyval_parse_init(void)
{
    OBJ_CONSTRUCT(&keyval_mutex, service_mutex_t);

    return CCS_SUCCESS;
}


int
service_util_keyval_parse_finalize(void)
{
    if (NULL != key_buffer) free(key_buffer);

    OBJ_DESTRUCT(&keyval_mutex);

    return CCS_SUCCESS;
}


int
service_util_keyval_parse(const char *filename,
                       service_keyval_parse_fn_t callback)
{
    int val;
    int ret = CCS_SUCCESS;;

    SERVICE_THREAD_LOCK(&keyval_mutex);

    keyval_filename = filename;
    keyval_callback = callback;

    /* Open the opal */
    service_util_keyval_yyin = fopen(keyval_filename, "r");
    if (NULL == service_util_keyval_yyin) {
        ret = CCS_ERR_NOT_FOUND;
        goto cleanup;
    }

    service_util_keyval_parse_done = false;
    service_util_keyval_yynewlines = 1;
    service_util_keyval_init_buffer(service_util_keyval_yyin);
    while (!service_util_keyval_parse_done) {
        val = service_util_keyval_yylex();
        switch (val) {
        case SERVICE_UTIL_KEYVAL_PARSE_DONE:
            /* This will also set service_util_keyval_parse_done to true, so just
               break here */
            break;

        case SERVICE_UTIL_KEYVAL_PARSE_NEWLINE:
            /* blank line!  ignore it */
            break;

        case SERVICE_UTIL_KEYVAL_PARSE_SINGLE_WORD:
            parse_line();
            break;

        default:
            /* anything else is an error */
            parse_error(1);
            break;
        }
    }
    fclose(service_util_keyval_yyin);

cleanup:
    SERVICE_THREAD_UNLOCK(&keyval_mutex);
    return ret;
}



static int parse_line(void)
{
    int val;

    /* Save the name name */
    if (key_buffer_len < strlen(service_util_keyval_yytext) + 1) {
        char *tmp;
        key_buffer_len = strlen(service_util_keyval_yytext) + 1;
        tmp = (char*)realloc(key_buffer, key_buffer_len);
        if (NULL == tmp) {
            free(key_buffer);
            key_buffer_len = 0;
            key_buffer = NULL;
            return CCS_ERR_TEMP_OUT_OF_RESOURCE;
        }
        key_buffer = tmp;
    }

    strncpy(key_buffer, service_util_keyval_yytext, key_buffer_len);

    /* The first thing we have to see is an "=" */

    val = service_util_keyval_yylex();
    if (service_util_keyval_parse_done || SERVICE_UTIL_KEYVAL_PARSE_EQUAL != val) {
        parse_error(2);
        return CCS_ERROR;
    }

    /* Next we get the value */

    val = service_util_keyval_yylex();
    if (SERVICE_UTIL_KEYVAL_PARSE_SINGLE_WORD == val ||
        SERVICE_UTIL_KEYVAL_PARSE_VALUE == val) {
        keyval_callback(key_buffer, service_util_keyval_yytext);

        /* Now we need to see the newline */

        val = service_util_keyval_yylex();
        if (SERVICE_UTIL_KEYVAL_PARSE_NEWLINE == val ||
            SERVICE_UTIL_KEYVAL_PARSE_DONE == val) {
            return CCS_SUCCESS;
        }
    }

    /* Did we get an EOL or EOF? */

    else if (SERVICE_UTIL_KEYVAL_PARSE_DONE == val ||
             SERVICE_UTIL_KEYVAL_PARSE_NEWLINE == val) {
        keyval_callback(key_buffer, NULL);
        return CCS_SUCCESS;
    }

    /* Nope -- we got something unexpected.  Bonk! */
    parse_error(3);
    return CCS_ERROR;
}


static void parse_error(int num)
{
    /* JMS need better error/warning message here */
    service_output(0, "keyval parser: error %d reading file %s at line %d:\n  %s\n",
                num, keyval_filename, service_util_keyval_yynewlines, service_util_keyval_yytext);
}
