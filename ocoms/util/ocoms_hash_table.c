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

#include "ocoms/platform/ocoms_config.h"

#include <string.h>
#include <stdlib.h>

#include "ocoms/util/output.h"
#include "ocoms/platform/ocoms_constants.h"
#include "ocoms/util/ocoms_list.h"
#include "ocoms/util/ocoms_hash_table.h"

/*
 * ocoms_hash_table_t
 */

#define HASH_MULTIPLIER 31

static void ocoms_hash_table_construct(ocoms_hash_table_t* ht);
static void ocoms_hash_table_destruct(ocoms_hash_table_t* ht);


OBJ_CLASS_INSTANCE(
    ocoms_hash_table_t, 
    ocoms_object_t,
    ocoms_hash_table_construct,
    ocoms_hash_table_destruct
);


static void ocoms_hash_table_construct(ocoms_hash_table_t* ht)
{
    OBJ_CONSTRUCT(&ht->ht_nodes, ocoms_list_t);
    ht->ht_table = NULL;
    ht->ht_table_size = 0;
    ht->ht_size = 0;
}


static void ocoms_hash_table_destruct(ocoms_hash_table_t* ht)
{
    size_t i;
    ocoms_hash_table_remove_all(ht);
    for(i=0; i<ht->ht_table_size; i++) {
        OBJ_DESTRUCT(ht->ht_table+i);
    }
    if(NULL != ht->ht_table) {
        free(ht->ht_table);
    }
    OBJ_DESTRUCT(&ht->ht_nodes);
}


int ocoms_hash_table_init(ocoms_hash_table_t* ht, size_t table_size)
{
    size_t i;
    size_t power2 = 1;
    size_t tmp = table_size;
    while(tmp) {
       tmp >>= 1;
       power2 <<= 1;
    }

    ht->ht_mask = power2-1;
    ht->ht_table = (ocoms_list_t *)malloc(power2 * sizeof(ocoms_list_t));
    if(NULL == ht->ht_table) {
        return OCOMS_ERR_OUT_OF_RESOURCE;
    }
    for(i=ht->ht_table_size; i<power2; i++) {
        ocoms_list_t* list = ht->ht_table+i;
        OBJ_CONSTRUCT(list, ocoms_list_t);
    }
    ht->ht_table_size = power2;
    return OCOMS_SUCCESS;
}

int ocoms_hash_table_remove_all(ocoms_hash_table_t* ht)
{
    size_t i;
    for(i=0; i<ht->ht_table_size; i++) {
        ocoms_list_t* list = ht->ht_table+i;
        while(ocoms_list_get_size(list)) {
            ocoms_list_item_t *item = ocoms_list_remove_first(list);
            OBJ_RELEASE(item);
        }
    }

    while(ocoms_list_get_size(&ht->ht_nodes)) {
        ocoms_list_item_t* item = ocoms_list_remove_first(&ht->ht_nodes);
        OBJ_RELEASE(item);
    }
    ht->ht_size = 0;
    return OCOMS_SUCCESS;
}
 
/***************************************************************************/

/*
 *  ocoms_uint32_hash_node_t
 */

struct ocoms_uint32_hash_node_t
{
    ocoms_list_item_t super;
    uint32_t hn_key;
    void *hn_value;
};
typedef struct ocoms_uint32_hash_node_t ocoms_uint32_hash_node_t;

static OBJ_CLASS_INSTANCE(ocoms_uint32_hash_node_t,
                          ocoms_list_item_t,
                          NULL,
                          NULL);


int ocoms_hash_table_get_value_uint32(ocoms_hash_table_t* ht, uint32_t key,
				     void **ptr)
{
    ocoms_list_t* list = ht->ht_table + (key & ht->ht_mask);
    ocoms_uint32_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_get_value_uint32:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERROR;
    }
#endif
    for(node =  (ocoms_uint32_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_uint32_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_uint32_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key == key) {
	    *ptr = node->hn_value;
            return OCOMS_SUCCESS;
        }
    } 
    return OCOMS_ERR_NOT_FOUND;
}


int ocoms_hash_table_set_value_uint32(ocoms_hash_table_t* ht,
				    uint32_t key, void* value)
{
    ocoms_list_t* list = ht->ht_table + (key & ht->ht_mask);
    ocoms_uint32_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_set_value_uint32:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif
    for(node =  (ocoms_uint32_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_uint32_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_uint32_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key == key) {
            node->hn_value = value;
            return OCOMS_SUCCESS;
        }
    } 

    node = (ocoms_uint32_hash_node_t*)ocoms_list_remove_first(&ht->ht_nodes); 
    if(NULL == node) {
        node = OBJ_NEW(ocoms_uint32_hash_node_t);
        if(NULL == node)
            return OCOMS_ERR_OUT_OF_RESOURCE;
    }
    node->hn_key = key;
    node->hn_value = value;
    ocoms_list_append(list, (ocoms_list_item_t*)node);
    ht->ht_size++;
    return OCOMS_SUCCESS;
}


int ocoms_hash_table_remove_value_uint32(ocoms_hash_table_t* ht, uint32_t key)
{
    ocoms_list_t* list = ht->ht_table + (key & ht->ht_mask);
    ocoms_uint32_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_remove_value_uint32:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif
    for(node =  (ocoms_uint32_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_uint32_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_uint32_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key == key) {
            ocoms_list_remove_item(list, (ocoms_list_item_t*)node);
            ocoms_list_append(&ht->ht_nodes, (ocoms_list_item_t*)node);
            ht->ht_size--;
            return OCOMS_SUCCESS;
        }
    } 
    return OCOMS_ERR_NOT_FOUND;
}

/***************************************************************************/

/*
 *  ocoms_uint64_hash_node_t
 */

struct ocoms_uint64_hash_node_t
{
    ocoms_list_item_t super;
    uint64_t hn_key;
    void* hn_value;
};
typedef struct ocoms_uint64_hash_node_t ocoms_uint64_hash_node_t;

static OBJ_CLASS_INSTANCE(ocoms_uint64_hash_node_t,
                          ocoms_list_item_t,
                          NULL,
                          NULL);


int ocoms_hash_table_get_value_uint64(ocoms_hash_table_t* ht, uint64_t key,
				     void **ptr)
{
    ocoms_list_t* list = ht->ht_table + (key & ht->ht_mask);
    ocoms_uint64_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_get_value_uint64:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERROR;
    }
#endif
    for(node =  (ocoms_uint64_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_uint64_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_uint64_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key == key) {
            *ptr = node->hn_value;
	    return OCOMS_SUCCESS;
        }
    } 
    return OCOMS_ERR_NOT_FOUND;
}


int ocoms_hash_table_set_value_uint64(ocoms_hash_table_t* ht,
				    uint64_t key, void* value)
{
    ocoms_list_t* list = ht->ht_table + (key & ht->ht_mask);
    ocoms_uint64_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_set_value_uint64:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif
    for(node =  (ocoms_uint64_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_uint64_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_uint64_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key == key) {
            node->hn_value = value;
            return OCOMS_SUCCESS;
        }
    } 

    node = (ocoms_uint64_hash_node_t*)ocoms_list_remove_first(&ht->ht_nodes); 
    if(NULL == node) {
        node = OBJ_NEW(ocoms_uint64_hash_node_t);
        if(NULL == node) {
            return OCOMS_ERR_OUT_OF_RESOURCE;
        }
    }
    node->hn_key = key;
    node->hn_value = value;
    ocoms_list_append(list, (ocoms_list_item_t*)node);
    ht->ht_size++;
    return OCOMS_SUCCESS;
}


int ocoms_hash_table_remove_value_uint64(ocoms_hash_table_t* ht, uint64_t key)
{
    ocoms_list_t* list = ht->ht_table + (key & ht->ht_mask);
    ocoms_uint64_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_remove_value_uint64:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif
    for(node =  (ocoms_uint64_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_uint64_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_uint64_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key == key) {
            ocoms_list_remove_item(list, (ocoms_list_item_t*)node);
            ocoms_list_append(&ht->ht_nodes, (ocoms_list_item_t*)node);
            ht->ht_size--;
            return OCOMS_SUCCESS;
        }
    } 
    return OCOMS_ERR_NOT_FOUND;
}

/***************************************************************************/

/*
 *  ocoms_ptr_hash_node_t
 */

struct ocoms_ptr_hash_node_t
{
    ocoms_list_item_t super;
    void*  hn_key;
    size_t hn_key_size;
    void*  hn_value;
};
typedef struct ocoms_ptr_hash_node_t ocoms_ptr_hash_node_t;

static void ocoms_ptr_hash_node_construct(ocoms_ptr_hash_node_t* hn)
{
    hn->hn_key_size = 0;
    hn->hn_key = NULL;
    hn->hn_value = NULL;
}

static void ocoms_ptr_hash_node_destruct(ocoms_ptr_hash_node_t* hn)
{
    if(NULL != hn->hn_key) {
        free(hn->hn_key);
    }
}

static OBJ_CLASS_INSTANCE(ocoms_ptr_hash_node_t,
                          ocoms_list_item_t,
                          ocoms_ptr_hash_node_construct,
                          ocoms_ptr_hash_node_destruct);


static inline uint32_t ocoms_hash_value(size_t mask, const void *key,
                                       size_t keysize)
{
    size_t h, i;
    const unsigned char *p;
    
    h = 0;
    p = (const unsigned char *)key;
    for (i = 0; i < keysize; i++, p++)
        h = HASH_MULTIPLIER*h + *p;
    return (uint32_t)(h & mask);
}

int ocoms_hash_table_get_value_ptr(ocoms_hash_table_t* ht, const void* key,
				  size_t key_size, void **ptr)
{
    ocoms_list_t* list = ht->ht_table + ocoms_hash_value(ht->ht_mask, key, 
                                                       key_size);
    ocoms_ptr_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_get_value_ptr:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERROR;
    }
#endif
    for(node =  (ocoms_ptr_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_ptr_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_ptr_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key_size == key_size &&
            memcmp(node->hn_key, key, key_size) == 0) {
            *ptr = node->hn_value;
	    return OCOMS_SUCCESS;
        }
    } 
    return OCOMS_ERR_NOT_FOUND;
}


int ocoms_hash_table_set_value_ptr(ocoms_hash_table_t* ht, const void* key,
                                  size_t key_size, void* value)
{
    ocoms_list_t* list = ht->ht_table + ocoms_hash_value(ht->ht_mask, key,
                                                       key_size);
    ocoms_ptr_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_set_value_ptr:"
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif
    for(node =  (ocoms_ptr_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_ptr_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_ptr_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key_size == key_size &&
            memcmp(node->hn_key, key, key_size) == 0) {
            node->hn_value = value;
            return OCOMS_SUCCESS;
        }
    } 

    node = (ocoms_ptr_hash_node_t*)ocoms_list_remove_first(&ht->ht_nodes); 
    if(NULL == node) {
        node = OBJ_NEW(ocoms_ptr_hash_node_t);
        if(NULL == node) {
            return OCOMS_ERR_OUT_OF_RESOURCE;
        }
    }
    node->hn_key = malloc(key_size);
    node->hn_key_size = key_size;
    node->hn_value = value;
    memcpy(node->hn_key, key, key_size);
    ocoms_list_append(list, (ocoms_list_item_t*)node);
    ht->ht_size++;
    return OCOMS_SUCCESS;
}


int ocoms_hash_table_remove_value_ptr(ocoms_hash_table_t* ht,
                                     const void* key, size_t key_size)
{
    ocoms_list_t* list = ht->ht_table + ocoms_hash_value(ht->ht_mask,
                                                       key, key_size);
    ocoms_ptr_hash_node_t *node;

#if OCOMS_ENABLE_DEBUG
    if(ht->ht_table_size == 0) {
        ocoms_output(0, "ocoms_hash_table_remove_value_ptr: "
		   "ocoms_hash_table_init() has not been called");
        return OCOMS_ERR_BAD_PARAM;
    }
#endif
    for(node =  (ocoms_ptr_hash_node_t*)ocoms_list_get_first(list);
        node != (ocoms_ptr_hash_node_t*)ocoms_list_get_end(list);
        node =  (ocoms_ptr_hash_node_t*)ocoms_list_get_next(node)) {
        if (node->hn_key_size == key_size &&
            memcmp(node->hn_key, key, key_size) == 0) {
            free(node->hn_key);
            node->hn_key = NULL;
            node->hn_key_size = 0;
            ocoms_list_remove_item(list, (ocoms_list_item_t*)node);
            ocoms_list_append(&ht->ht_nodes, (ocoms_list_item_t*)node);
            ht->ht_size--;
            return OCOMS_SUCCESS;
        }
    } 
 return OCOMS_ERR_NOT_FOUND;
}


int 
ocoms_hash_table_get_first_key_uint32(ocoms_hash_table_t *ht, uint32_t *key, 
                                     void **value, void **node)
{
    size_t i;
    ocoms_uint32_hash_node_t *list_node;

    /* Go through all the lists and return the first element off the
       first non-empty list */
    
    for (i = 0; i < ht->ht_table_size; ++i) {
        if (ocoms_list_get_size(ht->ht_table + i) > 0) {
            list_node = (ocoms_uint32_hash_node_t*)
                ocoms_list_get_first(ht->ht_table + i);
            *node = list_node;
            *key = list_node->hn_key;
            *value = list_node->hn_value;
            return OCOMS_SUCCESS;
        }
    }

    /* The hash table is empty */

    return OCOMS_ERROR;
}


int 
ocoms_hash_table_get_next_key_uint32(ocoms_hash_table_t *ht, uint32_t *key,
                                    void **value, void *in_node, 
                                    void **out_node)
{
    size_t i;
    ocoms_list_t *list;
    ocoms_list_item_t *item;
    ocoms_uint32_hash_node_t *next;

    /* Try to simply get the next value in the list.  If there isn't
       one, find the next non-empty list and take the first value */

    next = (ocoms_uint32_hash_node_t*) in_node;
    list = ht->ht_table + (next->hn_key & ht->ht_mask);
    item = ocoms_list_get_next(next);
    if (ocoms_list_get_end(list) == item) {
        item = NULL;
        for (i = (list - ht->ht_table) + 1; i < ht->ht_table_size; ++i) {
            if (ocoms_list_get_size(ht->ht_table + i) > 0) {
                item = ocoms_list_get_first(ht->ht_table + i);
                break;
            }
        }

        /* If we didn't find another non-empty list after this one,
           then we're at the end of the hash table */

        if (NULL == item) {
            return OCOMS_ERROR;
        }
    }

    /* We found it.  Save the values (use "next" to avoid some
       typecasting) */

    *out_node = (void *) item;
    next = (ocoms_uint32_hash_node_t *) *out_node;
    *key = next->hn_key;
    *value = next->hn_value;

    return OCOMS_SUCCESS;
}


int 
ocoms_hash_table_get_first_key_uint64(ocoms_hash_table_t *ht, uint64_t *key,
                                     void **value, void **node)
{
    size_t i;
    ocoms_uint64_hash_node_t *list_node;

    /* Go through all the lists and return the first element off the
       first non-empty list */
    
    for (i = 0; i < ht->ht_table_size; ++i) {
        if (ocoms_list_get_size(ht->ht_table + i) > 0) {
            list_node = (ocoms_uint64_hash_node_t*)
                ocoms_list_get_first(ht->ht_table + i);
            *node = list_node;
            *key = list_node->hn_key;
            *value = list_node->hn_value;
            return OCOMS_SUCCESS;
        }
    }

    /* The hash table is empty */

    return OCOMS_ERROR;
}


int 
ocoms_hash_table_get_next_key_uint64(ocoms_hash_table_t *ht, uint64_t *key,
                                    void **value, void *in_node, 
                                    void **out_node)
{
    size_t i;
    ocoms_list_t *list;
    ocoms_list_item_t *item;
    ocoms_uint64_hash_node_t *next;

    /* Try to simply get the next value in the list.  If there isn't
       one, find the next non-empty list and take the first value */

    next = (ocoms_uint64_hash_node_t*) in_node;
    list = ht->ht_table + (next->hn_key & ht->ht_mask);
    item = ocoms_list_get_next(next);
    if (ocoms_list_get_end(list) == item) {
        item = NULL;
        for (i = (list - ht->ht_table) + 1; i < ht->ht_table_size; ++i) {
            if (ocoms_list_get_size(ht->ht_table + i) > 0) {
                item = ocoms_list_get_first(ht->ht_table + i);
                break;
            }
        }

        /* If we didn't find another non-empty list after this one,
           then we're at the end of the hash table */

        if (NULL == item) {
            return OCOMS_ERROR;
        }
    }

    /* We found it.  Save the values (use "next" to avoid some
       typecasting) */

    *out_node = (void *) item;
    next = (ocoms_uint64_hash_node_t *) *out_node;
    *key = next->hn_key;
    *value = next->hn_value;

    return OCOMS_SUCCESS;
}
