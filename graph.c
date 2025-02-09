/***
 *       ____                 _          _       _ _   
 *      / ___|_ __ __ _ _ __ | |__      / \   __| | |_ 
 *     | |  _| '__/ _` | '_ \| '_ \    / _ \ / _` | __|
 *     | |_| | | | (_| | |_) | | | |  / ___ \ (_| | |_ 
 *      \____|_|  \__,_| .__/|_| |_| /_/   \_\__,_|\__|
 *                     |_|                             
 */
/* Implementation of the Graph ADT 
 * Requires a clear mapping between a list and graph
 * Functionality coupled with readdata.h ADT to ensure clean transferal
 of urls to nodes.
*/


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

/* Structure that persists aggregate information on vertices in graph */
struct baseVertex {
    int outGoing;
    int inComing;
    int length;
    double oldPageRank;
    double pageRank;
    vNode node;
    vNode refs;
};

/* Helper function */
static int inVertexList(bVertex given, int vertex);
static vNode insertInside(bVertex list, int vertex);
static vNode insertReference(bVertex list, int vertex);

/* pageRank helper functions */
static double pageSummation(Graph g, bVertex spec, int pageUsing);
static double weightedInLinks(bVertex j, bVertex i, Graph g);
static double weightedOutLinks(bVertex j, bVertex i, Graph g);
static double sumDiff(Graph g, int N, double *arr);
static double *updateAll(Graph g, double d, double diffPR, int N);
static void persistRanks(urlL list, Graph g);

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
void calculatePageRank(Graph g, urlL list, double d, double diffPR,
		       int maxIter) {
    /* Need to set-up the algorithm accordingly */
    int iteration = 0;
    int N = getNoURL(list);    
    double diff = diffPR;
    /* start the iterations loop */
    while ((iteration < maxIter) && (diff >= diffPR)) {
	/* Need to get the pageSummation part */
	double *ret = updateAll(g, d, diffPR, N);
	diff = sumDiff(g, N, ret); 
	iteration++;
    }
    persistRanks(list, g);
    return;
}

/* Function that computes simulatenous updating across all vertices that are
present. This is required since the algorithm uses a general p(i) index, 
indicating all page ranks must be updated in the same iteration. */
static double *updateAll(Graph g, double d, double diffPR, int N) {
    int i = 0;
    double precN = (double)N;
    double fronTerm = (1-d)/precN;
    double arr[N];
    double *ret = malloc(N*sizeof(double));
    while (i < N) {
	ret[i] = g->connections[i]->pageRank;
	i++;
    }
    i = 0;
    /* Can't change the pageRank immediately, must wait for all pages */
    while (i < N) {	
	double backRef = pageSummation(g, g->connections[i], i);
	double endTerm = d*backRef;
	double newPageRank = fronTerm + endTerm;
	arr[i] = newPageRank;
	i++;
    }
    i = 0;
    while (i < N) {
	g->connections[i]->pageRank = arr[i];
	i++;
    }
    return ret;
}

/* Sum the difference between two graph points */
static double sumDiff(Graph g, int N, double *arr) {
    int i = 0;
    double sum = 0;
    while (i < N) {
	double res = fabs(g->connections[i]->pageRank -
			 arr[i]);
	sum += res;
	i++;	
    }
    return sum;
}

/* Helper function that assists in counting pageSummation */
static double pageSummation(Graph g, bVertex spec, int pageUsing) {
    /* Set the reference nodes */
    vNode refCurr = spec->refs;
    /* Set a default value for the summation */
    double defaultSum = 0;
    while (refCurr != NULL) {
	double pageRankRef = g->connections[refCurr->vertex]->pageRank;
	double weightIn = weightedInLinks(g->connections[refCurr->vertex], spec,
					  g);
	double weightOut = weightedOutLinks(g->connections[refCurr->vertex],
					    spec, g);
	double mult = (pageRankRef * weightIn * weightOut);
	defaultSum += mult;
	refCurr = refCurr->next;
    }
    return defaultSum;
}

/* Helper to find the weighted inLinks */
static double weightedInLinks(bVertex j, bVertex i, Graph g) {
    /* Getting the top term to use for algorithm */
    int inLinkI = i->inComing;
    /* Start getting the outgoing from vertex j */
    vNode current = j->node;
    double sum = 0;
    while (current != NULL) {
	sum += g->connections[current->vertex]->inComing;
	current = current->next;
    }
    double conTop = (double)inLinkI;
    double conBot = sum;
    double res = conTop/conBot;
    return res;
}

/* Helper to find the weighted outLinks */
static double weightedOutLinks(bVertex j, bVertex i, Graph g) {
    /* Getting the top term to use for algorithm */
    int outLinkI = i->outGoing;
    /* Start getting the outgoing from vertex j */
    vNode current = j->node;
    double sum = 0;
    while (current != NULL) {
	double val = (double)g->connections[current->vertex]->outGoing;
	if (val == 0) {
	    val = 0.5;
	}
	sum += val;
	current = current->next;
    }
    double conTop = (double)outLinkI;
    if (conTop == 0) {
	conTop = 0.5;
    }
    double conBot = sum;
    if (conBot == 0) {
	conBot = 0.5;	
    }
    double res = conTop/conBot;
    return res;
}

/* Helper function to persist calculate pageRanks */
static void persistRanks(urlL list, Graph g) {
    int i = 0;
    while (i < g->nV) {
	insertPagerank(g->connections[i]->pageRank, i,
		       g->connections[i]->outGoing, list);
	i++;
    }
    return;
}
