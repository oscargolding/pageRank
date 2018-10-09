/* Implementation of the Graph ADT */

#include<stdio.h>
#include<stdlib.h>
#include"graph.h"
#include"readData.h"

#define TRUE 1
#define FALSE 0

typedef struct vertNode *vNode;
typedef struct baseVertex *bVertex;

/* A simple node for our given representation */
struct vertNode {
    int vertex;
    vNode next;
};

/* Can't say in advance how large the graph will be */
struct graphRep {
    int nV;
    int nE;
    bVertex *connections;
};

struct baseVertex {
    int outGoing;
    int inComing;
    int length;
    double pageRank;
    vNode node;
    vNode refs;
};

/* Helper function */
static int inVertexList(bVertex given, int vertex);
static vNode insertInside(bVertex list, int vertex);
static vNode insertReference(bVertex list, int vertex);

/* Generate a new graph with vertices, this graph is empty */
Graph newGraph(int vertices) {
    /* Allocate memory for the graph */
    Graph g = malloc(sizeof(struct graphRep));
    g->nV = vertices;
    g->nE = 0;
    g->connections = malloc(vertices * sizeof(bVertex));
    int i = 0;
    double conver = (double)vertices;
    double pVal = 1/conver;
    /* Appropriate process to create graph */
    while (i < vertices) {
	g->connections[i] = malloc(sizeof(struct baseVertex));
	g->connections[i]->outGoing = 0;
	g->connections[i]->inComing = 0;
	g->connections[i]->length = 0;
	g->connections[i]->node = NULL;
	g->connections[i]->refs = NULL;
	g->connections[i]->pageRank = pVal;
	i++;
    }
    return g;
}

/* Check if a given vertex is in the connection list */
static int inVertexList(bVertex given, int vertex) {
    vNode current = given->node;
    while (current != NULL) {
	if (current->vertex == vertex) return TRUE;
	current = current->next;
    }
    return FALSE;
}

/* Generic function to add a connection between two vertices in a graph */
/* Rules are to ignore parallel edges and recursive vertexes */
void addConnection(Graph g, int start, int end) {
    if (start == end) return;
    if (!inVertexList(g->connections[start], end)) {
	g->connections[start]->node = insertInside(g->connections[start], end);
	g->connections[end]->refs = insertReference(g->connections[end], start);
	g->connections[end]->inComing++;
	g->connections[start]->outGoing++;
	g->nE++;	
    }
}
static vNode insertReference(bVertex list, int vertex) {
    vNode new = malloc(sizeof(struct vertNode));
    new->vertex = vertex;
    new->next = list->refs;
    return new;
}
/* Insert inside a vertex list */
static vNode insertInside(bVertex list, int vertex) {
    vNode new = malloc(sizeof(struct vertNode));
    new->vertex = vertex;
    new->next = list->node;
    list->length++;
    return new;
}

/* Show to map */
void showGraph(Graph g) {
    printf("nVE: %d, nE: %d\n", g->nV, g->nE);
    int i = 0;
    while (i < g->nV) {
	vNode current = g->connections[i]->node;
	bVertex given = g->connections[i];
	printf("%d has %d outgoing links, %d incoming and %.7lf pagerank\n", i,
	       given->outGoing, given->inComing, given->pageRank);
	if (current == NULL) printf("%d connects to nothing\n", i);
	while (current != NULL) {
	    printf("%d connects to %d\n", i, current->vertex);
	    current = current->next;
	}
	i++;
    }
    printf("########## Detailing the references ##########\n");
    i = 0;
    while (i < g->nV) {
	vNode refCurr = g->connections[i]->refs;
	bVertex given = g->connections[i];
	printf("%d has %d outgoing links, %d incoming and %.7lf pagerank\n", i,
	       given->outGoing, given->inComing, given->pageRank);
	if (refCurr == NULL) printf("%d connects to nothing\n", i);
	while (refCurr != NULL) {
	    printf("%d is referenced by %d\n", i, refCurr->vertex);
	    refCurr = refCurr->next;
	}
	i++;
    }
}
    
