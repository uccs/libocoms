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
 * Copyright (c) 2008-2010 Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */


#include "service/platform/ccs_config.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if CCS_WANT_LIBLTDL
  #ifndef __WINDOWS__
    #if CCS_LIBLTDL_INTERNAL
      #include "service/libltdl/ltdl.h"
    #else
      #include "ltdl.h"
    #endif
  #else
    #include "ltdl.h"
  #endif
#endif

#include "service/util/service_list.h"
#include "service/mca/mca.h"
#include "service/mca/base/base.h"
#include "service/mca/base/mca_base_component_repository.h"
#include "service/platform/service_constants.h"

#if CCS_WANT_LIBLTDL

/*
 * Private types
 */
struct repository_item_t {
  service_list_item_t super;

  char ri_type[MCA_BASE_MAX_TYPE_NAME_LEN + 1];
  lt_dlhandle ri_dlhandle;
  const ccs_mca_base_component_t *ri_component_struct;
  service_list_t ri_dependencies;
};
typedef struct repository_item_t repository_item_t;
static void ri_constructor(service_object_t *obj);
static void ri_destructor(service_object_t *obj);
static OBJ_CLASS_INSTANCE(repository_item_t, service_list_item_t, 
                          ri_constructor, ri_destructor);

struct dependency_item_t {
  service_list_item_t super;

  repository_item_t *di_repository_entry;
};
typedef struct dependency_item_t dependency_item_t;
static void di_constructor(service_object_t *obj);
static void di_destructor(service_object_t *obj);
static OBJ_CLASS_INSTANCE(dependency_item_t, service_list_item_t, 
                          di_constructor, di_destructor);

#endif /* CCS_WANT_LIBLTDL */


/*
 * Private variables
 */
static bool initialized = false;


#if CCS_WANT_LIBLTDL

static service_list_t repository;


/*
 * Private functions
 */
static repository_item_t *find_component(const char *type, const char *name);
static int link_items(repository_item_t *src, repository_item_t *depend);

#if CCS_HAVE_LTDL_ADVISE
lt_dladvise ccs_mca_dladvise;
#endif

#endif /* CCS_WANT_LIBLTDL */


/*
 * Initialize the repository
 */
int ccs_mca_base_component_repository_init(void)
{
  /* Setup internal structures */

  if (!initialized) {
#if CCS_WANT_LIBLTDL
    /* Initialize libltdl */

    if (lt_dlinit() != 0) {
      return CCS_ERR_OUT_OF_RESOURCE;
    }

#if CCS_HAVE_LTDL_ADVISE
    if (lt_dladvise_init(&ccs_mca_dladvise)) {
        return CCS_ERR_OUT_OF_RESOURCE;
    }

    if (lt_dladvise_ext(&ccs_mca_dladvise)) {
        return CCS_ERROR;
    }

    if (lt_dladvise_global(&ccs_mca_dladvise)) {
        return CCS_ERROR;
    }
#endif

    OBJ_CONSTRUCT(&repository, service_list_t);
#endif
    initialized = true;
  }

  /* All done */

  return CCS_SUCCESS;
}


/*
 * Add a newly-opened dyanmic component to the repository of open
 * components.  The component's type, handle, and public struct are
 * saved.
 */
int ccs_mca_base_component_repository_retain(char *type, 
                                         lt_dlhandle component_handle, 
                                         const ccs_mca_base_component_t *component_struct)
{
#if CCS_WANT_LIBLTDL
  repository_item_t *ri;

  /* Allocate a new repository item */

  ri = OBJ_NEW(repository_item_t);
  if (NULL == ri) {
    return CCS_ERR_OUT_OF_RESOURCE;
  }

  /* Initialize the repository item */

  strncpy(ri->ri_type, type, MCA_BASE_MAX_TYPE_NAME_LEN);
  ri->ri_type[MCA_BASE_MAX_TYPE_NAME_LEN] = '\0';
  ri->ri_dlhandle = component_handle;
  ri->ri_component_struct = component_struct;

  /* Append the new item to the repository */

  service_list_append(&repository, (service_list_item_t *) ri);

  /* All done */

  return CCS_SUCCESS;
#else
  return CCS_ERR_NOT_SUPPORTED;
#endif
}


/*
 * Bump up the refcount on a component
 */
int ccs_mca_base_component_repository_retain_component(const char *type, 
                                                   const char *name)
{
#if CCS_WANT_LIBLTDL
    repository_item_t *ri = find_component(type, name);
    if (NULL != ri) {
        OBJ_RETAIN(ri);
        return CCS_SUCCESS;
    }
    return CCS_ERR_NOT_FOUND;
#else
    return CCS_ERR_NOT_SUPPORTED;
#endif
}


/*
 * Create a dependency from one component entry to another
 */
int ccs_mca_base_component_repository_link(const char *src_type, 
                                       const char *src_name,
                                       const char *depend_type,
                                       const char *depend_name)
{
#if CCS_WANT_LIBLTDL
  repository_item_t *src, *depend;

  /* Look up the two components */

  src = find_component(src_type, src_name);
  if (NULL == src) {
    return CCS_ERR_BAD_PARAM;
  }
  depend = find_component(depend_type, depend_name);
  if (NULL == depend) {
    return CCS_ERR_BAD_PARAM;
  }

  /* Link them */

  return link_items(src, depend);
#else
    return CCS_ERR_NOT_SUPPORTED;
#endif
}


/*
 * If it's in the repository, close a specified component and remove
 * it from the repository.
 */
void ccs_mca_base_component_repository_release(const ccs_mca_base_component_t *component)
{
#if CCS_WANT_LIBLTDL
  if (initialized) {
    repository_item_t *ri = find_component(component->mca_type_name, 
                                           component->mca_component_name);
    if (NULL != ri) {
      OBJ_RELEASE(ri);
    }
  }
#endif
}


/*
 * Finalize the repository -- close everything that's still open.
 */
void ccs_mca_base_component_repository_finalize(void)
{
#if CCS_WANT_LIBLTDL
  service_list_item_t *item;
  repository_item_t *ri;
#endif

  if (initialized) {
#if CCS_WANT_LIBLTDL

    /* Have to be slightly careful about this because of dependencies,
       particularly on OS's where it matters (i.e., closing a
       component that is depended on by other components actually
       causes missing symbols because the OS actually does unload it
       from memory!), such as OS X.

       So instead of just blindly closing everything, we have iterate
       over the array of open components releasing everything with a
       refcount of 1 -- skip anything with a refcount of more than 1.
       Repeat this procedure until either we have nothing open or we
       made one full pass and no refcounts went to 1 (which is
       technically an error). */

    do {
      for (item = service_list_get_first(&repository);
           service_list_get_end(&repository) != item; ) {
        ri = (repository_item_t *) item;
        item = service_list_get_next(item);
        OBJ_RELEASE(ri);
      }
    } while (service_list_get_size(&repository) > 0);

#if CCS_HAVE_LTDL_ADVISE
    if (lt_dladvise_destroy(&ccs_mca_dladvise)) {
        return;
    }
#endif

    /* Close down libltdl */

    lt_dlexit();
#endif

    initialized = false;
  }
}

#if CCS_WANT_LIBLTDL

static repository_item_t *find_component(const char *type, const char *name)
{
  service_list_item_t *item;
  repository_item_t *ri;

  for (item = service_list_get_first(&repository);
       service_list_get_end(&repository) != item;
       item = service_list_get_next(item)) {
    ri = (repository_item_t *) item;
    if (0 == strcmp(ri->ri_type, type) && 
        0 == strcmp(ri->ri_component_struct->mca_component_name, name)) {
      return ri;
    }
  }

  /* Not found */

  return NULL;
}


static int link_items(repository_item_t *src, repository_item_t *depend)
{
  dependency_item_t *di;

  /* Bozo check */

  if (NULL == src || NULL == depend) {
    return CCS_ERR_BAD_PARAM;
  }

  /* Make a new depedency item */

  di = OBJ_NEW(dependency_item_t);
  if (NULL == di) {
    return CCS_ERR_OUT_OF_RESOURCE;
  }

  /* Initialize the new dependency item */

  di->di_repository_entry = depend;

  /* Add it to the dependency list on the source repository entry */

  service_list_append(&src->ri_dependencies, (service_list_item_t *) di);

  /* Increment the refcount in the dependency */

  OBJ_RETAIN(depend);

  /* All done */

  return CCS_SUCCESS;
}


/*
 * Basic sentinel values, and construct the inner list
 */
static void ri_constructor(service_object_t *obj)
{
  repository_item_t *ri = (repository_item_t *) obj;

  memset(ri->ri_type, 0, sizeof(ri->ri_type));
  ri->ri_dlhandle = NULL;
  ri->ri_component_struct = NULL;

  OBJ_CONSTRUCT(&ri->ri_dependencies, service_list_t);
}


/*
 * Close a component 
 */
static void ri_destructor(service_object_t *obj)
{
  repository_item_t *ri = (repository_item_t *) obj;
  dependency_item_t *di;
  service_list_item_t *item;

  /* Close the component (and potentially unload it from memory */
  lt_dlclose(ri->ri_dlhandle);

  /* It should be obvious, but I'll state it anyway because it bit me
     during debugging: after the dlclose(), the ccs_mca_base_component_t
     pointer is no longer valid because it has [potentially] been
     unloaded from memory.  So don't try to use it.  :-) */

  /* Now go release/close (at a minimum: decrement the refcount) any
     dependencies of this component */

  for (item = service_list_remove_first(&ri->ri_dependencies);
       NULL != item; 
       item = service_list_remove_first(&ri->ri_dependencies)) {
    di = (dependency_item_t *) item;
    OBJ_RELEASE(di);
  }
  OBJ_DESTRUCT(&ri->ri_dependencies);
  service_list_remove_item(&repository, (service_list_item_t *) ri);
}


/*
 * Basic sentinel values
 */
static void di_constructor(service_object_t *obj)
{
  dependency_item_t *di = (dependency_item_t *) obj;

  di->di_repository_entry = NULL;
}


/*
 * When a dependency item is released, go release the repository entry
 * that it points to
 */
static void di_destructor(service_object_t *obj)
{
  dependency_item_t *di = (dependency_item_t *) obj;

  OBJ_RELEASE(di->di_repository_entry);
}

#endif /* CCS_WANT_LIBLTDL */
