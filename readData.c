/* Reading data from file and populating to create a list */

/* Have to define GNU compliant extra functions */
#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readData.h"

/* Want to create a Node to hold the crucial information for urls */
typedef struct urlNode *Node;

typedef struct urlNode {
    char *url;
    float pagerank;
    float diff;
} urlNode;

/* Create an overview struct with information */
struct urlList {
    int nElem;
    Node list;
} urlList;

static int countURLs(FILE *given);
static void readingVert(Graph g, char *url, urlL given, int pos);
static int findPosUrl(char *url, urlL given);
static double calcInitPR(urlL given);

/* Get the connections for a given connections.txt */
urlL getConnections(void) {
    /* Open for reading */
    FILE *conn = fopen("collection.txt", "r");
    if (conn == NULL) {
	fprintf(stderr, "Collections not found in present working directoy.\n");
    }
    int found = countURLs(conn);
    if (found == 0) {
	fprintf(stderr, "The collections file was empty.\n");
    }
    /* At this point want to create a handle on the list */
    urlL handle = malloc(sizeof(urlList));
    handle->nElem = found;
    /* Create an array of memory associated */
    handle->list = malloc(found * sizeof(urlNode));
    /* It's assumed no line is larger than 1000 characters */
    char foundArr[1000];
    int i = 0;
    double setPR = calcInitPR(handle);
    while (fscanf(conn, "%s", foundArr) != EOF) {
	handle->list[i].pagerank = setPR;
	handle->list[i].url = strdup(foundArr);
	i++;
    }
    return handle;
}

static int countURLs(FILE *given) {
    char finding[100];
    int found = 0;
    while (fscanf(given, "%s", finding) != EOF) {
	found++;       
    }
    /* Reset to the start of the file */
    fseek(given, 0, SEEK_SET);
    return found;
}

char *returnURL(int pos, urlL given) {
    if (pos < 0 || pos > given->nElem) {
	return NULL;
    }
    return given->list[pos].url;
}

/* Return the number of URLs in given structure */
int getNoURL(urlL given) {
    return given->nElem;
}

/* This returns a graph structure needed */
Graph getGraph(urlL given) {
    int nVert = getNoURL(given);
    /* Generate the graph */
    Graph g = newGraph(nVert);
    int i = 0;
    while (i < nVert) {
	char *url = returnURL(i, given);
	readingVert(g, url, given, i);
	i++;
    }
    return g;
}

/* Function to assist in graph population */
static void readingVert(Graph g, char *url, urlL given, int pos) {
    /* Create needed file extension */
    char txt[5] = ".txt";
    char reading[1000];
    reading[0] = '\0';
    strcat(reading, url);
    strcat(reading, txt);
    /* Don't want some lines to do with formatting */
    char *ignore0 = "#start";
    char *ignore1 = "Section-1";
    char *ignore2 = "#end";
    char result[1000];
    FILE *start = fopen(reading, "r");
    if (start == NULL) perror("Can't open file\n");
    fseek(start, 0, SEEK_SET);
    while (fscanf(start, "%s", result) != EOF) {
	if ((strcmp(result, ignore0) == 0) || strcmp(result, ignore1) == 0) {
	    /* Ignore the headers */
	    continue;
	} else if (strcmp(result, ignore2) == 0) {
	    /* This is the end of outgoing links */
	    break;
	}
	int loc = findPosUrl(result, given);
	addConnection(g, pos, loc);
    }
}

/* Given a URL, find the corresponding outgoing position in list */
static int findPosUrl(char *url, urlL given) {
    int end = getNoURL(given);
    int i = 0;
    while (i < end) {
	if (strcmp(url, returnURL(i, given)) == 0) {
	    return i;
	}
	i++;
    }
    return i;
}

/* Generate pagerank for given initial conditions */
static double calcInitPR(urlL given) {
    double inPR = 1/(given->nElem);
    return inPR;
}
