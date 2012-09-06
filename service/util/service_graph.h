/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
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
/**
 * @file 
 * The service_graph interface is used to provide a generic graph infrastructure
 * to Open-MPI. The graph is represented as an adjacentcy list.
 * The graph is a list of vertices. The graph is a weighted directional graph.
 * Each vertex contains a pointer to a vertex data. 
 * This pointer can point to the structure that this vertex belongs to.    
 */
#ifndef SERVICE_GRAPH_H
#define SERVICE_GRAPH_H

#include "ccs_config.h"
#include <stdio.h>
#include <stdlib.h>
#include "service/util/service_object.h"
#include "service/util/service_list.h"
#include "service/util/service_pointer_array.h"
#include "service/util/service_value_array.h"

BEGIN_C_DECLS

    /* When two vertices are not connected, the distance between them is infinite. */
#define DISTANCE_INFINITY 0x7fffffff

    /* A class for vertex */
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_graph_vertex_t);

    /* A class for an edge (a connection between two verices) */
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_graph_edge_t);

    /* A class for an adjacency list  */
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_adjacency_list_t);

    /* A class for graph */
CCS_DECLSPEC OBJ_CLASS_DECLARATION(service_graph_t);

  /**
   * Function pointer for coping a vertex data from one vertex to
   * another
   *
   * @param dst The destination pointer of vertex_data
   * @param src The source pointer of the vertex_data
   * 
   *
   */
typedef void (*service_graph_copy_vertex_data)(void **dst, void *src);

/**
 * free vertex data.
 * @param vertex_data
 * 
 * The vertex data can point to the structure that this vertex
 * belongs to.
 */
typedef void (*service_graph_free_vertex_data)(void *vertex_data);

/**
 * Allocate vertex data.
 */
typedef void *(*service_graph_alloc_vertex_data)(void);

/**
 * Compare two vertices data.
 * 
 *@param vertex_data1 
 *@param vertex_data2
 *
 *@return int The comparition results. 1- vertex_data1 is bigger
 *        then vertex_data2, 0- vertex_data1 is equal to
 *        vertex_data2 and -1- vertex_data1 is smaller the
 *        vertex_data2.
 */
typedef int  (*service_graph_compare_vertex_data)(void *vertex_data1, void *vertex_data2);

/**
 * print a vertex data.
 * 
 * @param vertex_data
 */
typedef char *(*service_graph_print_vertex)(void *vertex_data);


/**
 * A vertex class.
 */
struct service_graph_vertex_t {
    service_list_item_t             super; /* A pointer to a vertex parent */
    void                         *in_graph; /* A pointer to the graph that this vertex belongs to */
    void                         *in_adj_list; /* A pointer to the adjacency that this vertex belongs to */
    void                         *vertex_data;/* A pointer to some data. this pointer can point to the struct the this*/
                                              /* vertex belongs to*/
    struct service_graph_vertex_t   *sibling;/* A pointer to a sibling vertex. */
                                          /* if this vertex was copied this pointer will point to the source vertex */
                                          /* This pointer is for internal uses. */
    service_graph_copy_vertex_data  copy_vertex_data; /* A function to copy vertex data */
    service_graph_free_vertex_data  free_vertex_data; /* A function to print vertex data */
    service_graph_alloc_vertex_data alloc_vertex_data;/* A function to allocate vertex data */
    service_graph_compare_vertex_data compare_vertex; /* A function to compare between two vertices data */
    service_graph_print_vertex        print_vertex;   /* A function to print vertex data */
};

/**
 * A type for vertex.
 */
typedef struct service_graph_vertex_t service_graph_vertex_t;

/**
 * An service_adjacency_list_t class
 */
struct service_adjacency_list_t {
    service_list_item_t     super;   /* A pointer to vertex parent */
    service_graph_vertex_t *vertex;  /* The adjacency_list is for adjacent of this vertex */ 
    service_list_t         *edges;   /* An edge list for all the adjacent and their weights */
};

/**
 * A type for service_adjacency_list_t
 */
typedef struct service_adjacency_list_t service_adjacency_list_t;

/**
 * An edge class. (connection between two vertices.) Since the
 * graph is a directional graph, each vertex contains a start
 * and an end pointers for the start vertex and the end vertex
 * of this edge. Since the graph is a weighted graph, the edges
 * contains a weight field.
 */
struct service_graph_edge_t {
    service_list_item_t         super;  /* A pointer to the edge parent */
    service_graph_vertex_t      *start; /* The start vertex. */
    service_graph_vertex_t      *end;   /* The end vertex */
    uint32_t                 weight; /* The weight of this edge */
    service_adjacency_list_t    *in_adj_list; /* The adjacency list in witch this edge in.*/
                                           /* This adjacency list contains the start vertex of this edge*/
                                           /* and its for internal uses */
};

/**
 * A type for an edge
 */
typedef struct service_graph_edge_t service_graph_edge_t;


/**
 * A graph class.
 */
struct service_graph_t {
    service_object_t       super;
    service_list_t         *adjacency_list;
    int                 number_of_edges;
    int                 number_of_vertices;
};

/**
 * A type for graph class
 */
typedef struct service_graph_t service_graph_t; 

/**
 * This structure represent the distance (weight) of a vertex
 * from some point in the graph.
 */
struct vertex_distance_from_t {
    service_graph_vertex_t *vertex;
    uint32_t            weight;
};

/**
 * A type for vertex distance.
 */
typedef struct vertex_distance_from_t vertex_distance_from_t;

/**
 * This graph API adds a vertex to graph. The most common use
 * for this API is while building a graph.
 * 
 * @param graph The graph that the vertex will be added to.
 * @param vertex The vertex we want to add.
 */
CCS_DECLSPEC void service_graph_add_vertex(service_graph_t *graph, service_graph_vertex_t *vertex);

/**
 * This graph API remove a vertex from graph. The most common
 * use for this API is while distracting a graph or while
 * removing relevant vertices from a graph.
 * 
 * @param graph The graph that the vertex will be remove from.
 * @param vertex The vertex we want to remove.
 */
CCS_DECLSPEC void service_graph_remove_vertex(service_graph_t *graph, service_graph_vertex_t *vertex);

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
CCS_DECLSPEC int service_graph_add_edge(service_graph_t *graph, service_graph_edge_t *edge); 

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
CCS_DECLSPEC void service_graph_remove_edge (service_graph_t *graph, service_graph_edge_t *edge);

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
CCS_DECLSPEC uint32_t service_graph_adjacent(service_graph_t *graph, service_graph_vertex_t *vertex1, service_graph_vertex_t *vertex2);

/**
 * This Graph API returns the order of the graph (number of
 * vertices)
 * 
 * @param graph
 * 
 * @return int
 */
CCS_DECLSPEC int service_graph_get_order(service_graph_t *graph);

/**
 * This Graph API returns the size of the graph (number of
 * edges)
 *
 * @param graph
 * 
 * @return int
 */
CCS_DECLSPEC int service_graph_get_size(service_graph_t *graph);

/**
 * This graph API finds a vertex in the graph according the
 * vertex data.
 * @param graph the graph we searching in.
 * @param vertex_data the vertex data we are searching according
 *                    to.
 * 
 * @return service_graph_vertex_t* The vertex founded or NULL.
 */
CCS_DECLSPEC service_graph_vertex_t *service_graph_find_vertex(service_graph_t *graph, void *vertex_data);


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
CCS_DECLSPEC int service_graph_get_graph_vertices(service_graph_t *graph, service_pointer_array_t *vertices_list);

/**
 * This graph API returns all the adjacent of a vertex and the
 * distance (weight) of those adjacent and the vertex.
 * 
 * @param graph
 * @param vertex The reference vertex
 * @param adjacent An allocated pointer array of vertices and
 *                  their distance from the reference vertex.
 *                  Note that this pointer should be free after
 *                  usage by the user
 * 
 * @return int the number of adjacent in the list.
 */
CCS_DECLSPEC int service_graph_get_adjacent_vertices(service_graph_t *graph, service_graph_vertex_t *vertex, service_value_array_t *adjacent);

/**
 * This graph API duplicates a graph. Note that this API does
 * not copy the graph but builds a new graph while coping just
 * the vertices data.
 * 
 * @param dest The new created graph.
 * @param src The graph we want to duplicate.
 */
CCS_DECLSPEC void service_graph_duplicate(service_graph_t **dest, service_graph_t *src);

/**
 * This graph API finds the shortest path between two vertices.
 * 
 * @param graph
 * @param vertex1 The start vertex.
 * @param vertex2 The end vertex.
 * 
 * @return uint32_t the distance between the two vertices.
 */
CCS_DECLSPEC uint32_t service_graph_spf(service_graph_t *graph, service_graph_vertex_t *vertex1, service_graph_vertex_t *vertex2);

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
CCS_DECLSPEC uint32_t service_graph_dijkstra(service_graph_t *graph, service_graph_vertex_t *vertex, service_value_array_t *distance_array);

/**
 * This graph API prints a graph - mostly for debug uses.
 * @param graph
 */
CCS_DECLSPEC void service_graph_print(service_graph_t *graph);

END_C_DECLS


#endif /* SERVICE_GRAPH_H */

