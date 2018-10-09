/* Implementation of the Graph ADT */

#include<stdio.h>
#include<stdlib.h>
#include"graph.h"
#include"readData.h"

#define TRUE 1
#define FALSE 0

typedef struct vertNode *vNode;

/* A simple node for our given representation */
struct vertNode {
    int vertex;
    vNode next;
};

/* Can't say in advance how large the graph will be */
struct graphRep {
    int nV;
    int nE;
    vNode *connections;
};

/* Helper function */
static int inVertexList(vNode given, int vertex);
static vNode insertInside(vNode list, int vertex);

/* Generate a new graph with vertices, this graph is empty */
Graph newGraph(int vertices) {
    /* Allocate memory for the graph */
    Graph g = malloc(sizeof(struct graphRep));
    g->nV = vertices;
    g->nE = 0;
    g->connections = malloc(vertices * sizeof(vNode));
    int i = 0;
    /* Appropriate process to create graph */
    while (i < vertices) {
	g->connections[i] = NULL;
	i++;
    }
    return g;
}

/* Check if a given vertex is in the connection list */
static int inVertexList(vNode given, int vertex) {
    vNode current = given;
    while (current != NULL) {
	if (current->vertex == vertex) return TRUE;
	current = current->next;
    }
    return FALSE;
}

/* Generic function to add a connection between two vertices in a graph */
void addConnection(Graph g, int start, int end) {
    if (!inVertexList(g->connections[start], end)) {
	g->connections[start] = insertInside(g->connections[start], end);
	g->nE++;
    }
}

/* Insert inside a vertex list */
static vNode insertInside(vNode list, int vertex) {
    vNode new = malloc(sizeof(struct vertNode));
    new->vertex = vertex;
    new->next = list;
    return new;
}

/* Show to map */
void showGraph(Graph g) {
    printf("nVE: %d, nE: %d\n", g->nV, g->nE);
    int i = 0;
    while (i < g->nV) {
	vNode current = g->connections[i];
	if (current == NULL) printf("%d connects to nothing\n", i);
	while (current != NULL) {
	    printf("%d connects to %d\n", i, current->vertex);
	    current = current->next;
	}
	i++;
    }
}
    
