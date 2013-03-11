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

#include "service/platform/ccs_config.h"

#include "service/util/service_list.h"
#include "service/platform/service_constants.h"
#include "service/util/service_graph.h"
#include "service/util/output.h"

static int compare_vertex_distance(const void *item1, const void *item2);

/*
 *  Graph classes
 */


static void service_graph_vertex_construct(service_graph_vertex_t *vertex);
static void service_graph_vertex_destruct(service_graph_vertex_t *vertex);

OBJ_CLASS_INSTANCE(
    service_graph_vertex_t,
    service_list_item_t,
    service_graph_vertex_construct,
    service_graph_vertex_destruct
);


static void service_graph_edge_construct(service_graph_edge_t *edge);
static void service_graph_edge_destruct(service_graph_edge_t *edge);

OBJ_CLASS_INSTANCE(
    service_graph_edge_t,
    service_list_item_t,
    service_graph_edge_construct,
    service_graph_edge_destruct
);

static void service_graph_construct(service_graph_t *graph);
static void service_graph_destruct(service_graph_t *graph);

OBJ_CLASS_INSTANCE(
    service_graph_t,
    service_object_t,
    service_graph_construct,
    service_graph_destruct
);

static void service_adjacency_list_construct(service_adjacency_list_t *aj_list);
static void service_adjacency_list_destruct(service_adjacency_list_t *aj_list);

OBJ_CLASS_INSTANCE(
    service_adjacency_list_t,
    service_list_item_t,
    service_adjacency_list_construct,
    service_adjacency_list_destruct
);



/*
 *
 *      service_graph_vertex_t interface
 *
 */

static void service_graph_vertex_construct(service_graph_vertex_t *vertex)
{
    vertex->in_adj_list = NULL;
    vertex->in_graph = NULL;
    vertex->vertex_data = NULL;
    vertex->sibling = NULL;
    vertex->copy_vertex_data = NULL;
    vertex->free_vertex_data = NULL;
    vertex->alloc_vertex_data = NULL;
    vertex->compare_vertex = NULL;
    vertex->print_vertex = NULL;
}

static void service_graph_vertex_destruct(service_graph_vertex_t *vertex)
{
    vertex->in_adj_list = NULL;
    vertex->in_graph = NULL;
    vertex->sibling = NULL;
    vertex->copy_vertex_data = NULL;
    vertex->alloc_vertex_data = NULL;
    vertex->compare_vertex = NULL;
    if (NULL != vertex->free_vertex_data) {
        vertex->free_vertex_data(vertex->vertex_data);
    }
    vertex->vertex_data = NULL;
    vertex->print_vertex = NULL;
}


/*
 *
 *      service_graph_edge_t interface
 *
 */

static void service_graph_edge_construct(service_graph_edge_t *edge)
{
    edge->end = NULL;
    edge->start = NULL;
    edge->weight = 0;
    edge->in_adj_list = NULL;
}


static void service_graph_edge_destruct(service_graph_edge_t *edge)
{
    edge->end = NULL;
    edge->start = NULL;
    edge->weight = 0;
    edge->in_adj_list = NULL;
}


/*
 *
 *     service_graph_t  interface
 *
 */


static void service_graph_construct(service_graph_t *graph)
{
    graph->adjacency_list = OBJ_NEW(service_list_t);
    graph->number_of_vertices = 0;
    graph->number_of_edges = 0;
}

static void service_graph_destruct(service_graph_t *graph)
{
    service_adjacency_list_t *aj_list;

    while (false == service_list_is_empty(graph->adjacency_list)) {
        aj_list = (service_adjacency_list_t *)service_list_remove_first(graph->adjacency_list);
        OBJ_RELEASE(aj_list);
    }
    OBJ_RELEASE(graph->adjacency_list);
    graph->number_of_vertices = 0;
    graph->number_of_edges = 0;
}

/*
 *
 *     service_adjacency_list  interface
 *
 */

static void service_adjacency_list_construct(service_adjacency_list_t *aj_list)
{
    aj_list->vertex = NULL;
    aj_list->edges = OBJ_NEW(service_list_t);
}

static void service_adjacency_list_destruct(service_adjacency_list_t *aj_list)
{
   service_graph_edge_t *edge;

   aj_list->vertex = NULL;
   while (false == service_list_is_empty(aj_list->edges)) {
       edge = (service_graph_edge_t *)service_list_remove_first(aj_list->edges);
       OBJ_RELEASE(edge);
   }
   OBJ_RELEASE(aj_list->edges);

}

/**
 * This function deletes all the edges that are connected *to* a
 * vertex.
 * 
 * @param graph
 * @param vertex
 */
static void delete_all_edges_conceded_to_vertex(service_graph_t *graph, service_graph_vertex_t *vertex)
{
    service_adjacency_list_t *aj_list;
    service_list_item_t *aj_list_item;
    service_graph_edge_t *edge;
    service_list_item_t *edge_item;

    /**
     * for all the adjacency list in the graph
     */
    for (aj_list_item = service_list_get_first(graph->adjacency_list);
         aj_list_item != service_list_get_end(graph->adjacency_list);
         aj_list_item  = service_list_get_next(aj_list_item)) {
        aj_list = (service_adjacency_list_t *) aj_list_item;
        /**
         * for all the edges in the adjacency list
         */
        edge_item = service_list_get_first(aj_list->edges);
        while (edge_item != service_list_get_end(aj_list->edges)) {
            edge = (service_graph_edge_t *)edge_item;
            edge_item  = service_list_get_next(edge_item);

            /**
             * if the edge is ended in the vertex
             */
            if (edge->end == vertex) {
                /* Delete this edge  */
                service_list_remove_item(edge->in_adj_list->edges, (service_list_item_t*)edge);
                /* distract this edge */
                OBJ_RELEASE(edge);
            }
        }
    }
}

/**
 * This graph API adds a vertex to graph. The most common use
 * for this API is while building a graph.
 * 
 * @param graph The graph that the vertex will be added to.
 * @param vertex The vertex we want to add.
 */
void service_graph_add_vertex(service_graph_t *graph, service_graph_vertex_t *vertex)
{
   service_adjacency_list_t *aj_list;
   service_list_item_t *item;

   /**
    * Find if this vertex already exists in the graph.
    */
   for (item = service_list_get_first(graph->adjacency_list);
        item != service_list_get_end(graph->adjacency_list);
        item  = service_list_get_next(item)) {
       aj_list = (service_adjacency_list_t *) item;
       if (aj_list->vertex == vertex) {
           /* If this vertex exists, dont do anything. */
           return;
       }
   }
   /* Construct a new adjacency list */
   aj_list = OBJ_NEW(service_adjacency_list_t);
   aj_list->vertex = vertex;
   /* point the vertex to the adjacency list of the vertex (for easy searching) */
   vertex->in_adj_list = aj_list;
   /* Append the new creates adjacency list to the graph */ 
   service_list_append(graph->adjacency_list, (service_list_item_t*)aj_list);
   /* point the vertex to the graph it belongs to (mostly for debug uses)*/
   vertex->in_graph = graph;
   /* increase the number of vertices in the graph */
   graph->number_of_vertices++;
}


/**
 * This graph API adds an edge (connection between two
 * vertices) to a graph. The most common use
 * for this API is while building a graph.
 * 
 * @param graph The graph that this edge will be added to.
 * @param edge The edge that we want to add.
 * 
 * @return int Success or error. this API can return an error if
 *         one of the vertices is not in the graph.
 */
int service_graph_add_edge(service_graph_t *graph, service_graph_edge_t *edge) 
{
    service_adjacency_list_t *aj_list, *start_aj_list= NULL, *end_aj_list;
    service_list_item_t *item;
    bool start_found = false, end_found = false;


    /**
     * find the vertices that this edge should connect.
     */
    for (item = service_list_get_first(graph->adjacency_list);
         item != service_list_get_end(graph->adjacency_list);
         item  = service_list_get_next(item)) {
        aj_list = (service_adjacency_list_t *) item;
        if (aj_list->vertex == edge->start) {
            start_found = true;
            start_aj_list = aj_list;
        }
        if (aj_list->vertex == edge->end) {
            end_found = true;
            end_aj_list = aj_list;
        }
    }
    /**
     * if one of the vertices either the start or the end is not
     * found - return an error.
     */
    if (false == start_found && false == end_found) {
        return CCS_ERROR;
    }
    /* point the edge to the adjacency list of the start vertex (for easy search) */
    edge->in_adj_list=start_aj_list;
    /* append the edge to the adjacency list of the start vertex */
    service_list_append(start_aj_list->edges, (service_list_item_t*)edge);
    /* increase the graph size */
    graph->number_of_edges++;
    return CCS_SUCCESS;
}


/**
 * This graph API removes an edge (a connection between two
 * vertices) from the graph. The most common use of this API is
 * while destructing a graph or while removing vertices from a
 * graph. while removing vertices from a graph, we should also
 * remove the connections from and to the vertices that we are
 * removing.
 * 
 * @param graph The graph that this edge will be remove from.
 * @param edge the edge that we want to remove.
 */
void service_graph_remove_edge (service_graph_t *graph, service_graph_edge_t *edge)
{
    /* remove the edge from the list it belongs to */
    service_list_remove_item(edge->in_adj_list->edges, (service_list_item_t*)edge);
    /* decrees the number of edges in the graph */
    graph->number_of_edges--;
    /* Note that the edge is not destructed - the caller should destruct this edge. */
}

/**
 * This graph API remove a vertex from graph. The most common
 * use for this API is while distracting a graph or while
 * removing relevant vertices from a graph.
 * 
 * @param graph The graph that the vertex will be remove from.
 * @param vertex The vertex we want to remove.
 */

void service_graph_remove_vertex(service_graph_t *graph, service_graph_vertex_t *vertex)
{
    service_adjacency_list_t *adj_list;
    service_graph_edge_t *edge;

    /**
     * remove all the edges of this vertex and destruct them.
     */
    adj_list = vertex->in_adj_list;
    while (false == service_list_is_empty(adj_list->edges)) {
        edge = (service_graph_edge_t *)service_list_remove_first(adj_list->edges);
        OBJ_RELEASE(edge);
    }
    /**
     * remove the adjscency list of this vertex from the graph and
     * destruct it.
     */
    service_list_remove_item(graph->adjacency_list, (service_list_item_t*)adj_list);
    OBJ_RELEASE(adj_list);
    /**
     * delete all the edges that connected *to* the vertex.
     */
    delete_all_edges_conceded_to_vertex(graph, vertex);
    /* destruct the vertex */
    OBJ_RELEASE(vertex);
    /* decrease the number of vertices in the graph. */
    graph->number_of_vertices--;
}

/**
 * This graph API tell us if two vertices are adjacent
 * 
 * @param graph The graph that the vertices belongs to.
 * @param vertex1 first vertex.
 * @param vertex2 second vertex.
 * 
 * @return uint32_t the weight of the connection between the two
 *         vertices or infinity if the vertices are not
 *         connected.
 */
uint32_t service_graph_adjacent(service_graph_t *graph, service_graph_vertex_t *vertex1, service_graph_vertex_t *vertex2)
{
    service_adjacency_list_t *adj_list;
    service_list_item_t *item;
    service_graph_edge_t *edge;

    /**
     * Verify that the first vertex belongs to the graph.
     */
    if (graph != vertex1->in_graph) {
        CCS_OUTPUT((0,"service_graph_adjacent 1 Vertex1 %p not in the graph %p\n",(void *)vertex1,(void *)graph));
        return DISTANCE_INFINITY;
    }
    /**
     * Verify that the second vertex belongs to the graph.
     */
    if (graph != vertex2->in_graph) {
        CCS_OUTPUT((0,"service_graph_adjacent 2 Vertex2 %p not in the graph %p\n",(void *)vertex2,(void *)graph));
        return DISTANCE_INFINITY;
    }
    /**
     * If the first vertex and the second vertex are the same
     * vertex, the distance between the is 0.
     */
    if (vertex1 == vertex2) {
        return 0;
    }
    /**
     * find the second vertex in the adjacency list of the first
     * vertex.
     */
    adj_list = (service_adjacency_list_t *) vertex1->in_adj_list;
    for (item = service_list_get_first(adj_list->edges);
         item != service_list_get_end(adj_list->edges);
         item  = service_list_get_next(item)) {
        edge = (service_graph_edge_t *)item;
        if (edge->end == vertex2) {
            /* if the second vertex was found in the adjacency list of the first one, return the weight */
            return edge->weight;
        }
    }
    /* if the second vertex was not found in the adjacency list of the first one, return infinity */
    return DISTANCE_INFINITY;
}

/**
 * This Graph API returns the order of the graph (number of
 * vertices)
 * 
 * @param graph
 * 
 * @return int
 */
int service_graph_get_order(service_graph_t *graph)
{
    return graph->number_of_vertices;
}

/**
 * This Graph API returns the size of the graph (number of
 * edges)
 *
 * @param graph
 * 
 * @return int
 */
int service_graph_get_size(service_graph_t *graph)
{
    return graph->number_of_edges;
}

/**
 * This graph API finds a vertex in the graph according the
 * vertex data.
 * @param graph the graph we searching in.
 * @param vertex_data the vertex data we are searching according
 *                    to.
 * 
 * @return service_graph_vertex_t* The vertex founded or NULL.
 */
service_graph_vertex_t *service_graph_find_vertex(service_graph_t *graph, void *vertex_data)
{
    service_adjacency_list_t *aj_list;
    service_list_item_t *item;

    /**
     * Run on all the vertices of the graph
     */
    for (item = service_list_get_first(graph->adjacency_list);
         item != service_list_get_end(graph->adjacency_list);
         item  = service_list_get_next(item)) {
        aj_list = (service_adjacency_list_t *) item;
        if (NULL != aj_list->vertex->compare_vertex) {
            /* if the vertex data of a vertex is equal to the vertex data */
            if (0 == aj_list->vertex->compare_vertex(aj_list->vertex->vertex_data, vertex_data)) {
                /* return the found vertex */
                return aj_list->vertex;
            }
        }
    }
    /* if a vertex is not found, return NULL */
    return NULL;
}


/**
 * This graph API returns an array of pointers of all the
 * vertices in the graph.
 * 
 * 
 * @param graph
 * @param vertices_list an array of pointers of all the
 *         vertices in the graph vertices.
 * 
 * @return int returning the graph order (the
 *                    number of vertices in the returned array)
 */
int service_graph_get_graph_vertices(service_graph_t *graph, service_pointer_array_t *vertices_list)
{
    service_adjacency_list_t *aj_list;
    service_list_item_t *item;
    int i;

    /**
     * If the graph order is 0, return NULL.
     */
    if (0 == graph->number_of_vertices) {
        return 0;
    }
    /* Run on all the vertices of the graph */
    for (item = service_list_get_first(graph->adjacency_list), i = 0;
         item != service_list_get_end(graph->adjacency_list);
         item  = service_list_get_next(item), i++) {
        aj_list = (service_adjacency_list_t *) item;
        /* Add the vertex to the vertices array */
        service_pointer_array_add(vertices_list,(void *)aj_list->vertex);
    }
    /* return the vertices list */
    return graph->number_of_vertices;
}

/**
 * This graph API returns all the adjacents of a vertex and the
 * distance (weight) of those adjacents and the vertex.
 * 
 * @param graph
 * @param vertex The reference vertex
 * @param adjacents An allocated pointer array of vertices and
 *                  their distance from the reference vertex.
 *                  Note that this pointer should be free after
 *                  usage by the user
 * 
 * @return int the number of adjacents in the list.
 */
int service_graph_get_adjacent_vertices(service_graph_t *graph, service_graph_vertex_t *vertex, service_value_array_t *adjacents)
{
    service_adjacency_list_t *adj_list;
    service_graph_edge_t *edge;    
    int adjacents_number;
    service_list_item_t *item;
    vertex_distance_from_t distance_from;
    int i;

    /**
     * Verify that the vertex belongs to the graph.
     */
    if (graph != vertex->in_graph) {
        CCS_OUTPUT((0,"Vertex %p not in the graph %p\n", (void *)vertex, (void *)graph));
        return 0;
    }
    /**
     * find the adjacency list that this vertex belongs to
     */
    adj_list = (service_adjacency_list_t *) vertex->in_adj_list;
    /* find the number of adjcents of this vertex */
    adjacents_number = service_list_get_size(adj_list->edges);
    /* Run on all the edges from this vertex */
    for (item = service_list_get_first(adj_list->edges), i = 0;
         item != service_list_get_end(adj_list->edges);
         item  = service_list_get_next(item), i++) {
        edge = (service_graph_edge_t *)item;
        /* assign vertices and their weight in the adjcents list */
        distance_from.vertex = edge->end;
        distance_from.weight = edge->weight;
        service_value_array_append_item(adjacents, &distance_from);
    }
    /* return the number of the adjacents in the list */
    return adjacents_number;
}

/**
 * This graph API finds the shortest path between two vertices.
 * 
 * @param graph
 * @param vertex1 The start vertex.
 * @param vertex2 The end vertex.
 * 
 * @return uint32_t the distance between the two vertices.
 */

uint32_t service_graph_spf(service_graph_t *graph, service_graph_vertex_t *vertex1, service_graph_vertex_t *vertex2)
{
    service_value_array_t *distance_array;
    uint32_t items_in_distance_array, spf = DISTANCE_INFINITY;
    vertex_distance_from_t *vertex_distance;
    uint32_t i;

    /**
     * Verify that the first vertex belongs to the graph.
     */
    if (graph != vertex1->in_graph) {
        CCS_OUTPUT((0,"service_graph_spf 1 Vertex1 %p not in the graph %p\n",(void *)vertex1,(void *)graph));
        return DISTANCE_INFINITY;
    }
    /**
     * Verify that the second vertex belongs to the graph.
     */
    if (graph != vertex2->in_graph) {
        CCS_OUTPUT((0,"service_graph_spf 2 Vertex2 %p not in the graph %p\n",(void *)vertex2,(void *)graph));
        return DISTANCE_INFINITY;
    }
    /**
     * Run Dijkstra algorithm on the graph from the start vertex.
     */
    distance_array = OBJ_NEW(service_value_array_t);
    service_value_array_init(distance_array, sizeof(vertex_distance_from_t));
    service_value_array_reserve(distance_array,50);
    items_in_distance_array = service_graph_dijkstra(graph, vertex1, distance_array);
    /**
     * find the end vertex in the distance array that Dijkstra
     * algorithm returned.
     */
    for (i = 0; i < items_in_distance_array; i++) {
        vertex_distance = service_value_array_get_item(distance_array, i);
        if (vertex_distance->vertex == vertex2) {
            spf = vertex_distance->weight;
            break;
        }
    }
    OBJ_RELEASE(distance_array);
    /* return the distance (weight) to the end vertex */
    return spf;
}

/**
 * Compare the distance between two vertex distance items. this
 * function is used for sorting an array of vertices distance by
 * qsort function.
 * 
 * @param item1 a void pointer to vertex distance structure
 * @param item2 a void pointer to vertex distance structure
 * 
 * @return int 1 - the first item weight is higher then the
 *         second item weight. 0 - the weights are equal. -1 -
 *         the second item weight is higher the the first item
 *         weight.
 */
static int compare_vertex_distance(const void *item1, const void *item2)
{
    vertex_distance_from_t *vertex_dist1, *vertex_dist2;

    /* convert the void pointers to vertex distance pointers. */
    vertex_dist1 = (vertex_distance_from_t *)item1;
    vertex_dist2 = (vertex_distance_from_t *)item2;

    /* If the first item weight is higher then the second item weight return 1*/
    if (vertex_dist1->weight > vertex_dist2->weight) {
        return 1;
    }
    /* If they are equal return 0 */
    else if (vertex_dist1->weight == vertex_dist2->weight) {
        return 0;
    }
    /* if you reached here then the second item weight is higher the the first item weight */
    return -1;
}


/**
 * This graph API returns the distance (weight) from a reference
 * vertex to all other vertices in the graph using the Dijkstra
 * algorithm
 * 
 * @param graph
 * @param vertex The reference vertex.
 * @param distance_array An array of vertices and
 *         their distance from the reference vertex.
 * 
 * @return uint32_t the size of the distance array
 */
uint32_t service_graph_dijkstra(service_graph_t *graph, service_graph_vertex_t *vertex, service_value_array_t *distance_array)
{
    int graph_order;
    vertex_distance_from_t *Q, *q_start, *current_vertex;
    service_list_item_t *adj_list_item;
    service_adjacency_list_t *adj_list;
    int number_of_items_in_q;
    int i;
    uint32_t weight;


    /**
     * Verify that the reference vertex belongs to the graph.
     */
    if (graph != vertex->in_graph) {
        CCS_OUTPUT((0,"opal:graph:dijkstra: vertex %p not in the graph %p\n",(void *)vertex,(void *)graph));
        return 0;
    }
    /* get the order of the graph and allocate a working queue accordingly */
    graph_order = service_graph_get_order(graph);
    Q = (vertex_distance_from_t *)malloc(graph_order * sizeof(vertex_distance_from_t));
    /* assign a pointer to the start of the queue */
    q_start = Q;
    /* run on all the vertices of the graph */
    for (adj_list_item = service_list_get_first(graph->adjacency_list), i=0;
         adj_list_item != service_list_get_end(graph->adjacency_list);
         adj_list_item  = service_list_get_next(adj_list_item), i++) {
        adj_list = (service_adjacency_list_t *)adj_list_item;
        /* insert the vertices pointes to the working queue */
        Q[i].vertex = adj_list->vertex;
        /**
         * assign an infinity distance to all the vertices in the queue
         * except the reference vertex which its distance should be 0.
         */
        if (Q[i].vertex == vertex) {
            Q[i].weight = 0;
        }
        else {
            Q[i].weight = DISTANCE_INFINITY;
        }
    }
    number_of_items_in_q = i;
    /* sort the working queue according the distance from the reference vertex */
    qsort(q_start, number_of_items_in_q, sizeof(vertex_distance_from_t), compare_vertex_distance);
    /* while the working queue is not empty */
    while (number_of_items_in_q > 0) {
        /* start to work with the first vertex in the working queue */
        current_vertex = q_start;
        /* remove the first vertex from the queue */
        q_start++;
        /* decrees the number of vertices in the queue */
        number_of_items_in_q--;
        /* find the distance of all other vertices in the queue from the first vertex in the queue */
        for (i = 0; i < number_of_items_in_q; i++) {
            weight = service_graph_adjacent(graph, current_vertex->vertex, q_start[i].vertex);
            /**
             * if the distance from the first vertex in the queue to the I
             * vertex in the queue plus the distance of the first vertex in
             * the queue from the referenced vertex is smaller than the
             * distance of the I vertex from the referenced vertex, assign
             * the lower distance to the I vertex.
             */
            if (current_vertex->weight + weight < q_start[i].weight) {
                q_start[i].weight = weight + current_vertex->weight;
            }
        }
        /* sort again the working queue */
        qsort(q_start, number_of_items_in_q, sizeof(vertex_distance_from_t), compare_vertex_distance);
    }
    /* copy the working queue the the returned distance array */
    for (i = 0; i < graph_order-1; i++) {
        service_value_array_append_item(distance_array, (void *)&(Q[i+1]));
    }
    /* free the working queue */
    free(Q);
    /* assign the distance array size. */
    return graph_order - 1;
}


/**
 * This graph API duplicates a graph. Note that this API does
 * not copy the graph but builds a new graph while coping just
 * the vertex data.
 * 
 * @param dest The new created graph.
 * @param src The graph we want to duplicate.
 */
void service_graph_duplicate(service_graph_t **dest, service_graph_t *src)
{
    service_adjacency_list_t *aj_list;
    service_list_item_t *aj_list_item, *edg_item;
    service_graph_vertex_t *vertex;
    service_graph_edge_t *edge, *new_edge;

    /* construct a new graph */
    *dest = OBJ_NEW(service_graph_t);
    /* Run on all the vertices of the src graph */
    for (aj_list_item = service_list_get_first(src->adjacency_list);
         aj_list_item != service_list_get_end(src->adjacency_list);
         aj_list_item  = service_list_get_next(aj_list_item)) {
        aj_list = (service_adjacency_list_t *) aj_list_item;
        /* for each vertex in the src graph, construct a new vertex */
        vertex = OBJ_NEW(service_graph_vertex_t);
        /* associate the new vertex to a vertex from the original graph */
        vertex->sibling = aj_list->vertex;
        /* associate the original vertex to the new constructed vertex */
        aj_list->vertex->sibling = vertex;
        /* allocate space to vertex data in the new vertex */
        if (NULL != aj_list->vertex->alloc_vertex_data) {
            vertex->vertex_data = aj_list->vertex->alloc_vertex_data();
            vertex->alloc_vertex_data = aj_list->vertex->alloc_vertex_data;
        }
        /* copy the vertex data from the original vertex  to the new vertex */
        if (NULL != aj_list->vertex->copy_vertex_data) {
            aj_list->vertex->copy_vertex_data(&(vertex->vertex_data), aj_list->vertex->vertex_data);
            vertex->copy_vertex_data = aj_list->vertex->copy_vertex_data;
        }
        /* copy all the fields of the original vertex to the new vertex. */
        vertex->free_vertex_data = aj_list->vertex->free_vertex_data;
        vertex->print_vertex = aj_list->vertex->print_vertex;
        vertex->compare_vertex = aj_list->vertex->compare_vertex;
        vertex->in_graph = *dest;
        /* add the new vertex to the new graph */
        service_graph_add_vertex(*dest, vertex);
    }
    /**
     * Now, copy all the edges from the source graph
     */
    /* Run on all the adjscency lists in the graph */
    for (aj_list_item = service_list_get_first(src->adjacency_list);
         aj_list_item != service_list_get_end(src->adjacency_list);
         aj_list_item  = service_list_get_next(aj_list_item)) {
        aj_list = (service_adjacency_list_t *) aj_list_item;
        /* for all the edges in the adjscency list */
        for (edg_item = service_list_get_first(aj_list->edges);
             edg_item != service_list_get_end(aj_list->edges);
             edg_item = service_list_get_next(edg_item)) {
            edge = (service_graph_edge_t *)edg_item;
            /* construct new edge for the new graph */
            new_edge = OBJ_NEW(service_graph_edge_t);
            /* copy the edge weight from the original edge */
            new_edge->weight = edge->weight;
            /* connect the new edge according to start and end associations to the vertices of the src graph */
            new_edge->start = edge->start->sibling;
            new_edge->end = edge->end->sibling;
            /* add the new edge to the new graph */
            service_graph_add_edge(*dest, new_edge);
        }
    }
}

/**
 * This graph API prints a graph - mostly for debug uses.
 * @param graph
 */
void service_graph_print(service_graph_t *graph)
{
    service_adjacency_list_t *aj_list;
    service_list_item_t *aj_list_item;
    service_graph_edge_t *edge;
    service_list_item_t *edge_item;
    char *tmp_str1, *tmp_str2;
    bool need_free1, need_free2;

    /* print header */
    service_output(0, "      Graph         ");
    service_output(0, "====================");
    /* run on all the vertices of the graph */
    for (aj_list_item = service_list_get_first(graph->adjacency_list);
         aj_list_item != service_list_get_end(graph->adjacency_list);
         aj_list_item  = service_list_get_next(aj_list_item)) {
        aj_list = (service_adjacency_list_t *) aj_list_item;
        /* print vertex data to temporary string*/
        if (NULL != aj_list->vertex->print_vertex) {
            need_free1 = true;
            tmp_str1 = aj_list->vertex->print_vertex(aj_list->vertex->vertex_data);
        }
        else {
            need_free1 = false;
            tmp_str1 = "";
        }
        /* print vertex */
        service_output(0, "V(%s) Connections:",tmp_str1);
        /* run on all the edges of the vertex */
        for (edge_item = service_list_get_first(aj_list->edges);
             edge_item != service_list_get_end(aj_list->edges);
             edge_item  = service_list_get_next(edge_item)) {
            edge = (service_graph_edge_t *)edge_item;
            /* print the vertex data of the vertex in the end of the edge to a temporary string */
            if (NULL != edge->end->print_vertex) {
                need_free2 = true;
                tmp_str2 = edge->end->print_vertex(edge->end->vertex_data);
            }
            else {
                need_free2 = false;
                tmp_str2 = "";
            }
            /* print the edge */
            service_output(0, "    E(%s -> %d -> %s)",tmp_str1, edge->weight, tmp_str2);
            if (need_free2) {
                free(tmp_str2);
            }
        }
        if (need_free1) {
            free(tmp_str1);
        }
    }
}

