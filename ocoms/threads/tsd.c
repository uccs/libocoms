/* 
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#include "ocoms/platform/ocoms_config.h"

#include <stdio.h>
# if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <errno.h>

#include "ocoms/threads/tsd.h"

#if !OCOMS_HAVE_POSIX_THREADS && !OCOMS_HAVE_SOLARIS_THREADS && !defined(__WINDOWS__)

#define TSD_ENTRIES 32

struct tsd_entry_t {
    bool used;
    void *value;
    service_tsd_destructor_t destructor;
};
typedef struct tsd_entry_t tsd_entry_t;

static tsd_entry_t entries[TSD_ENTRIES];
static bool atexit_registered = false;

static void
run_destructors(void)
{
    int i;

    for (i = 0; i < TSD_ENTRIES ; ++i) {
        service_tsd_destructor_t destructor;
        void *value;

        if (entries[i].used) {
            destructor = entries[i].destructor;
            value = entries[i].value;
            
            entries[i].used = false;
            entries[i].destructor = NULL;
            entries[i].value = NULL;
            
            destructor(value);
        }
    }
}

int
service_tsd_key_create(service_tsd_key_t *key, 
                    service_tsd_destructor_t destructor)
{
    int i;

    if (!atexit_registered) {
        atexit_registered = true;
        if (0 != atexit(run_destructors)) {
            return OCOMS_ERR_TEMP_OUT_OF_RESOURCE;
        }
    }

    for (i = 0 ; i < TSD_ENTRIES ; ++i) {
        if (entries[i].used == false) {
            entries[i].used = true;
            entries[i].value = NULL;
            entries[i].destructor = destructor;
            *key = i;
            break;
        }
    }
    if (i == TSD_ENTRIES) return ENOMEM;

    return OCOMS_SUCCESS;
}


int
service_tsd_key_delete(service_tsd_key_t key)
{
    if (!entries[key].used) return OCOMS_ERR_BAD_PARAM;

    entries[key].used = false;
    entries[key].value = NULL;
    entries[key].destructor = NULL;

    return OCOMS_SUCCESS;
}


int
service_tsd_setspecific(service_tsd_key_t key, void *value)
{
    if (!entries[key].used) return OCOMS_ERR_BAD_PARAM;
    entries[key].value = value;
    return OCOMS_SUCCESS;
}


int
service_tsd_getspecific(service_tsd_key_t key, void **valuep)
{
    if (!entries[key].used) return OCOMS_ERR_BAD_PARAM;
    *valuep = entries[key].value;
    return OCOMS_SUCCESS;
}

#endif
