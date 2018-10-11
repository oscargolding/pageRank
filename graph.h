/* Interface for a graph ADT */

#ifndef GRAPH_H
#define GRAPH_H

/* Define the graph ADT type */
typedef struct graphRep *Graph;

#include "readData.h"

/* Create a new graph with given number of vertices */
Graph newGraph(int vertices);

/* Add a connection to the graph */
void addConnection(Graph g, int start, int end);

/* Debugging by showing the graph */
void showGraph(Graph g);

/* Calculate pageRank function on graph */
void calculatePageRank(Graph g, urlL list, double d, double diffPR,
		       int maxIter);
#endif 
