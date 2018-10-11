/* Implementation of the Graph ADT */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
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
    float oldPageRank;
    float pageRank;
    vNode node;
    vNode refs;
};

/* Helper function */
static int inVertexList(bVertex given, int vertex);
static vNode insertInside(bVertex list, int vertex);
static vNode insertReference(bVertex list, int vertex);

/* pageRank helper functions */
static float pageSummation(Graph g, bVertex spec);
static float weightedInLinks(bVertex j, bVertex i, Graph g);
static float weightedOutLinks(bVertex j, bVertex i, Graph g);
static float sumDiff(Graph g, int N);

/* Generate a new graph with vertices, this graph is empty */
Graph newGraph(int vertices) {
    /* Allocate memory for the graph */
    Graph g = malloc(sizeof(struct graphRep));
    g->nV = vertices;
    g->nE = 0;
    g->connections = malloc(vertices * sizeof(bVertex));
    int i = 0;
    float conver = (float)vertices;
    float pVal = 1/conver;
    /* Appropriate process to create graph */
    while (i < vertices) {
	g->connections[i] = malloc(sizeof(struct baseVertex));
	g->connections[i]->outGoing = 0;
	g->connections[i]->inComing = 0;
	g->connections[i]->length = 0;
	g->connections[i]->node = NULL;
	g->connections[i]->refs = NULL;
	g->connections[i]->pageRank = pVal;
	g->connections[i]->oldPageRank = pVal;
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
	printf("%d has %d outgoing links, %d incoming and %.9f pagerank\n", i,
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
	printf("%d has %d outgoing links, %d incoming and %.7f pagerank\n", i,
	       given->outGoing, given->inComing, given->pageRank);
	if (refCurr == NULL) printf("%d connects to nothing\n", i);
	while (refCurr != NULL) {
	    printf("%d is referenced by %d\n", i, refCurr->vertex);
	    refCurr = refCurr->next;
	}
	i++;
    }
}

/* Calculate the pageRank of a graph */
void calculatePageRank(Graph g, urlL list, float d, float diffPR,
		       int maxIter) {
    /* Need to set-up the algorithm accordingly */
    int iteration = 0;
    int N = getNoURL(list);
    float precN = (float)N;
    float fronTerm = (1-d)/precN;
    float diff = diffPR;
    int pageUsing = 0;
    /* Start the iterations loop */
    while ((iteration < maxIter) && (diff >= diffPR)) {
	/* Need to get the pageSummation part */
	float backRef = pageSummation(g, g->connections[pageUsing]);
	float endTerm = d * backRef;
	float newPageRank = fronTerm + endTerm;
	float oldPageRank = g->connections[pageUsing]->pageRank;
	g->connections[pageUsing]->oldPageRank = oldPageRank;
	g->connections[pageUsing]->pageRank = newPageRank;
	diff = sumDiff(g, N); if (diff < diffPR) break; 
	iteration++;
	pageUsing = (pageUsing == N-1) ? 0 : (pageUsing + 1);
    }    
    return;
}

/* Sum the difference between two graph points */
static float sumDiff(Graph g, int N) {
    int i = 0;
    float sum = 0;
    while (i < N) {
	float res = fabs(g->connections[i]->pageRank -
			 g->connections[i]->oldPageRank);
	if (res < 0) {
	    res = -res;
	}
	sum += res;
	i++;	
    }
    return sum;
}

/* Helper function that assists in counting pageSummation */
static float pageSummation(Graph g, bVertex spec) {
    /* Set the reference nodes */
    vNode refCurr = spec->refs;
    /* Set a default value for the summation */
    float defaultSum = 0;
    while (refCurr != NULL) {
	float pageRankRef = g->connections[refCurr->vertex]->pageRank;
	float weightIn = weightedInLinks(g->connections[refCurr->vertex], spec,
					  g);
	float weightOut = weightedOutLinks(g->connections[refCurr->vertex],
					    spec, g);
	float mult = (pageRankRef * weightIn * weightOut);
	defaultSum += mult;
	refCurr = refCurr->next;
    }
    return defaultSum;
}

/* Helper to find the weighted inLinks */
static float weightedInLinks(bVertex j, bVertex i, Graph g) {
    /* Getting the top term to use for algorithm */
    int inLinkI = i->inComing;
    /* if (inLinkI == 0) return 0.0; */
    /* Start getting the outgoing from vertex j */
    vNode current = j->node;
    float sum = 0;
    while (current != NULL) {
	sum += g->connections[current->vertex]->inComing;
	current = current->next;
    }
    float conTop = (float)inLinkI;
    float conBot = sum;
    float res = conTop/conBot;
    return res;
}

/* Helper to find the weighted outLinks */
static float weightedOutLinks(bVertex j, bVertex i, Graph g) {
    /* Getting the top term to use for algorithm */
    int outLinkI = i->outGoing;
    /* Start getting the outgoing from vertex j */
    vNode current = j->node;
    float sum = 0;
    while (current != NULL) {
	float val = (float)g->connections[current->vertex]->outGoing;
	if (val == 0) {
	    val = 0.5;
	}
	sum += val;
	current = current->next;
    }
    float conTop = (float)outLinkI;
    if (conTop == 0) {
	conTop = 0.5;
    }
    float conBot = sum;
    if (conBot == 0) {
	conBot = 0.5;	
    }
    float res = conTop/conBot;
    return res;
}
