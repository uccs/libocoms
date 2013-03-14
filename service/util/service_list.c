/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2007 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Voltaire All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "service/platform/ocoms_config.h"

#include "service/util/service_list.h"
#include "service/platform/service_constants.h"

/*
 *  List classes
 */

static void service_list_item_construct(service_list_item_t*);
static void service_list_item_destruct(service_list_item_t*);

OBJ_CLASS_INSTANCE(
    service_list_item_t,
    service_object_t,
    service_list_item_construct,
    service_list_item_destruct
);

static void service_list_construct(service_list_t*);
static void service_list_destruct(service_list_t*);

OBJ_CLASS_INSTANCE(
    service_list_t,
    service_object_t,
    service_list_construct,
    service_list_destruct
);


/*
 *
 *      service_list_link_item_t interface
 *
 */

static void service_list_item_construct(service_list_item_t *item)
{
    item->service_list_next = item->service_list_prev = NULL;
    item->item_free = 1;
#if OCOMS_ENABLE_DEBUG
    item->service_list_item_refcount = 0;
    item->service_list_item_belong_to = NULL;
#endif
}

static void service_list_item_destruct(service_list_item_t *item)
{
#if OCOMS_ENABLE_DEBUG
    assert( 0 == item->service_list_item_refcount );
    assert( NULL == item->service_list_item_belong_to );
#endif  /* OCOMS_ENABLE_DEBUG */
}


/*
 *
 *      service_list_list_t interface
 *
 */

static void service_list_construct(service_list_t *list)
{
#if OCOMS_ENABLE_DEBUG
    /* These refcounts should never be used in assertions because they
       should never be removed from this list, added to another list,
       etc.  So set them to sentinel values. */

    OBJ_CONSTRUCT( &(list->service_list_sentinel), service_list_item_t );
    list->service_list_sentinel.service_list_item_refcount  = 1;
    list->service_list_sentinel.service_list_item_belong_to = list;
#endif

    list->service_list_sentinel.service_list_next = &list->service_list_sentinel;
    list->service_list_sentinel.service_list_prev = &list->service_list_sentinel;
    list->service_list_length = 0;
}


/*
 * Reset all the pointers to be NULL -- do not actually destroy
 * anything.
 */
static void service_list_destruct(service_list_t *list)
{
    service_list_construct(list);
}


/*
 * Insert an item at a specific place in a list
 */
bool service_list_insert(service_list_t *list, service_list_item_t *item, long long idx)
{
    /* Adds item to list at index and retains item. */
    int     i;
    volatile service_list_item_t *ptr, *next;
    
    if ( idx >= (long long)list->service_list_length ) {
        return false;
    }
    
    if ( 0 == idx )
    {
        service_list_prepend(list, item);
    } else {
#if OCOMS_ENABLE_DEBUG
        /* Spot check: ensure that this item is previously on no
           lists */

        assert(0 == item->service_list_item_refcount);
#endif
        /* pointer to element 0 */
        ptr = list->service_list_sentinel.service_list_next;
        for ( i = 0; i < idx-1; i++ )
            ptr = ptr->service_list_next;

        next = ptr->service_list_next;
        item->service_list_next = next;
        item->service_list_prev = ptr;
        next->service_list_prev = item;
        ptr->service_list_next = item;

#if OCOMS_ENABLE_DEBUG
        /* Spot check: ensure this item is only on the list that we
           just insertted it into */

        service_atomic_add( &(item->service_list_item_refcount), 1 );
        assert(1 == item->service_list_item_refcount);
        item->service_list_item_belong_to = list;
#endif
    }
    
    list->service_list_length++;    
    return true;
}


static
void
service_list_transfer(service_list_item_t *pos, service_list_item_t *begin,
                   service_list_item_t *end)
{
    volatile service_list_item_t *tmp;

    if (pos != end) {
        /* remove [begin, end) */
        end->service_list_prev->service_list_next = pos;
        begin->service_list_prev->service_list_next = end;
        pos->service_list_prev->service_list_next = begin;

        /* splice into new position before pos */
        tmp = pos->service_list_prev;
        pos->service_list_prev = end->service_list_prev;
        end->service_list_prev = begin->service_list_prev;
        begin->service_list_prev = tmp;
#if OCOMS_ENABLE_DEBUG
        {
            volatile service_list_item_t* item = begin;
            while( pos != item ) {
                item->service_list_item_belong_to = pos->service_list_item_belong_to;
                item = item->service_list_next;
                assert(NULL != item);
            }
        }
#endif  /* OCOMS_ENABLE_DEBUG */
    }
}


void
service_list_join(service_list_t *thislist, service_list_item_t *pos, 
               service_list_t *xlist)
{
    if (0 != service_list_get_size(xlist)) {
        service_list_transfer(pos, service_list_get_first(xlist),
                           service_list_get_end(xlist));

        /* fix the sizes */
        thislist->service_list_length += xlist->service_list_length;
        xlist->service_list_length = 0;
    }
}


void
service_list_splice(service_list_t *thislist, service_list_item_t *pos,
                 service_list_t *xlist, service_list_item_t *first,
                 service_list_item_t *last)
{ 
    size_t change = 0;
    service_list_item_t *tmp;

    if (first != last) {
        /* figure out how many things we are going to move (have to do
         * first, since last might be end and then we wouldn't be able
         * to run the loop) 
         */
        for (tmp = first ; tmp != last ; tmp = service_list_get_next(tmp)) {
            change++;
        }

        service_list_transfer(pos, first, last);

        /* fix the sizes */
        thislist->service_list_length += change;
        xlist->service_list_length -= change;
    }
}


int service_list_sort(service_list_t* list, service_list_item_compare_fn_t compare)
{
    service_list_item_t* item;
    service_list_item_t** items;
    size_t i, index=0;

    if (0 == list->service_list_length) {
        return OCOMS_SUCCESS;
    }
    items = (service_list_item_t**)malloc(sizeof(service_list_item_t*) * 
                                       list->service_list_length);

    if (NULL == items) {
        return OCOMS_ERR_OUT_OF_RESOURCE;
    }

    while(NULL != (item = service_list_remove_first(list))) {
        items[index++] = item;
    }
    
    qsort(items, index, sizeof(service_list_item_t*), 
          (int(*)(const void*,const void*))compare);
    for (i=0; i<index; i++) {
        service_list_append(list,items[i]);
    }
    free(items);
    return OCOMS_SUCCESS;
}
