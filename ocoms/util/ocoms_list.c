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
 * Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
 * Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ocoms/platform/ocoms_config.h"

#include "ocoms/util/ocoms_list.h"
#include "ocoms/platform/ocoms_constants.h"

/*
 *  List classes
 */

static void ocoms_list_item_construct(ocoms_list_item_t*);
static void ocoms_list_item_destruct(ocoms_list_item_t*);

OBJ_CLASS_INSTANCE(
    ocoms_list_item_t,
    ocoms_object_t,
    ocoms_list_item_construct,
    ocoms_list_item_destruct
);

static void ocoms_list_construct(ocoms_list_t*);
static void ocoms_list_destruct(ocoms_list_t*);

OBJ_CLASS_INSTANCE(
    ocoms_list_t,
    ocoms_object_t,
    ocoms_list_construct,
    ocoms_list_destruct
);


/*
 *
 *      ocoms_list_link_item_t interface
 *
 */

static void ocoms_list_item_construct(ocoms_list_item_t *item)
{
    item->ocoms_list_next = item->ocoms_list_prev = NULL;
    item->item_free = 1;
#if OCOMS_ENABLE_DEBUG
    item->ocoms_list_item_refcount = 0;
    item->ocoms_list_item_belong_to = NULL;
#endif
}

static void ocoms_list_item_destruct(ocoms_list_item_t *item)
{
#if OCOMS_ENABLE_DEBUG
    assert( 0 == item->ocoms_list_item_refcount );
    assert( NULL == item->ocoms_list_item_belong_to );
#endif  /* OCOMS_ENABLE_DEBUG */
}


/*
 *
 *      ocoms_list_list_t interface
 *
 */

static void ocoms_list_construct(ocoms_list_t *list)
{
#if OCOMS_ENABLE_DEBUG
    /* These refcounts should never be used in assertions because they
       should never be removed from this list, added to another list,
       etc.  So set them to sentinel values. */

    OBJ_CONSTRUCT( &(list->ocoms_list_sentinel), ocoms_list_item_t );
    list->ocoms_list_sentinel.ocoms_list_item_refcount  = 1;
    list->ocoms_list_sentinel.ocoms_list_item_belong_to = list;
#endif

    list->ocoms_list_sentinel.ocoms_list_next = &list->ocoms_list_sentinel;
    list->ocoms_list_sentinel.ocoms_list_prev = &list->ocoms_list_sentinel;
    list->ocoms_list_length = 0;
}


/*
 * Reset all the pointers to be NULL -- do not actually destroy
 * anything.
 */
static void ocoms_list_destruct(ocoms_list_t *list)
{
    ocoms_list_construct(list);
}


/*
 * Insert an item at a specific place in a list
 */
bool ocoms_list_insert(ocoms_list_t *list, ocoms_list_item_t *item, long long idx)
{
    /* Adds item to list at index and retains item. */
    int     i;
    volatile ocoms_list_item_t *ptr, *next;
    
    if ( idx >= (long long)list->ocoms_list_length ) {
        return false;
    }
    
    if ( 0 == idx )
    {
        ocoms_list_prepend(list, item);
    } else {
#if OCOMS_ENABLE_DEBUG
        /* Spot check: ensure that this item is previously on no
           lists */

        assert(0 == item->ocoms_list_item_refcount);
#endif
        /* pointer to element 0 */
        ptr = list->ocoms_list_sentinel.ocoms_list_next;
        for ( i = 0; i < idx-1; i++ )
            ptr = ptr->ocoms_list_next;

        next = ptr->ocoms_list_next;
        item->ocoms_list_next = next;
        item->ocoms_list_prev = ptr;
        next->ocoms_list_prev = item;
        ptr->ocoms_list_next = item;

#if OCOMS_ENABLE_DEBUG
        /* Spot check: ensure this item is only on the list that we
           just insertted it into */

        ocoms_atomic_add( &(item->ocoms_list_item_refcount), 1 );
        assert(1 == item->ocoms_list_item_refcount);
        item->ocoms_list_item_belong_to = list;
#endif
    }
    
    list->ocoms_list_length++;    
    return true;
}


static
void
ocoms_list_transfer(ocoms_list_item_t *pos, ocoms_list_item_t *begin,
                   ocoms_list_item_t *end)
{
    volatile ocoms_list_item_t *tmp;

    if (pos != end) {
        /* remove [begin, end) */
        end->ocoms_list_prev->ocoms_list_next = pos;
        begin->ocoms_list_prev->ocoms_list_next = end;
        pos->ocoms_list_prev->ocoms_list_next = begin;

        /* splice into new position before pos */
        tmp = pos->ocoms_list_prev;
        pos->ocoms_list_prev = end->ocoms_list_prev;
        end->ocoms_list_prev = begin->ocoms_list_prev;
        begin->ocoms_list_prev = tmp;
#if OCOMS_ENABLE_DEBUG
        {
            volatile ocoms_list_item_t* item = begin;
            while( pos != item ) {
                item->ocoms_list_item_belong_to = pos->ocoms_list_item_belong_to;
                item = item->ocoms_list_next;
                assert(NULL != item);
            }
        }
#endif  /* OCOMS_ENABLE_DEBUG */
    }
}


void
ocoms_list_join(ocoms_list_t *thislist, ocoms_list_item_t *pos, 
               ocoms_list_t *xlist)
{
    if (0 != ocoms_list_get_size(xlist)) {
        ocoms_list_transfer(pos, ocoms_list_get_first(xlist),
                           ocoms_list_get_end(xlist));

        /* fix the sizes */
        thislist->ocoms_list_length += xlist->ocoms_list_length;
        xlist->ocoms_list_length = 0;
    }
}


void
ocoms_list_splice(ocoms_list_t *thislist, ocoms_list_item_t *pos,
                 ocoms_list_t *xlist, ocoms_list_item_t *first,
                 ocoms_list_item_t *last)
{ 
    size_t change = 0;
    ocoms_list_item_t *tmp;

    if (first != last) {
        /* figure out how many things we are going to move (have to do
         * first, since last might be end and then we wouldn't be able
         * to run the loop) 
         */
        for (tmp = first ; tmp != last ; tmp = ocoms_list_get_next(tmp)) {
            change++;
        }

        ocoms_list_transfer(pos, first, last);

        /* fix the sizes */
        thislist->ocoms_list_length += change;
        xlist->ocoms_list_length -= change;
    }
}


int ocoms_list_sort(ocoms_list_t* list, ocoms_list_item_compare_fn_t compare)
{
    ocoms_list_item_t* item;
    ocoms_list_item_t** items;
    size_t i, index=0;

    if (0 == list->ocoms_list_length) {
        return OCOMS_SUCCESS;
    }
    items = (ocoms_list_item_t**)malloc(sizeof(ocoms_list_item_t*) * 
                                       list->ocoms_list_length);

    if (NULL == items) {
        return OCOMS_ERR_OUT_OF_RESOURCE;
    }

    while(NULL != (item = ocoms_list_remove_first(list))) {
        items[index++] = item;
    }
    
    qsort(items, index, sizeof(ocoms_list_item_t*), 
          (int(*)(const void*,const void*))compare);
    for (i=0; i<index; i++) {
        ocoms_list_append(list,items[i]);
    }
    free(items);
    return OCOMS_SUCCESS;
}
