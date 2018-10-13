/* Reading data from file and populating to create a list */

/***
 *      ____                _       _             _       _ _   
 *     |  _ \ ___  __ _  __| | __ _| |_ __ _     / \   __| | |_ 
 *     | |_) / _ \/ _` |/ _` |/ _` | __/ _` |   / _ \ / _` | __|
 *     |  _ <  __/ (_| | (_| | (_| | || (_| |  / ___ \ (_| | |_ 
 *     |_| \_\___|\__,_|\__,_|\__,_|\__\__,_| /_/   \_\__,_|\__|
 *                                                              
 */
/* Have to define GNU compliant extra functions 
 * Create a URL List using an array implementation
 * Arrays allows for perfect mapping with a graph
 * O(1) interation with graph ADT
 * Sorting is guaranteed to be O(n*log(n)) -> done on pageRank
*/

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "readData.h"

/* Want to create a Node to hold the crucial information for urls */
typedef struct urlNode *Node;

typedef struct urlNode {
    char *url;
    double pagerank;
    int outDegree;
} urlNode;

/* Create an overview struct with information */
struct urlList {
    int nElem;
    Node list;
} urlList;

/* Helper functions for dealing with the URL List ADT */
static int countURLs(FILE *given);
static double calcInitPR(urlL given);

/* Helper functions to assist in merging */
static void mergeSort(Node list, int lo, int hi);
static void merge(Node list, int lo, int mid, int hi);
static int more(double x, double y);
static void copy(Node a, int *i, Node b, int *j);

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

/* Given a URL, find the corresponding outgoing position in list */
int findPosUrl(char *url, urlL given) {
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

/* Insert a pageRank into list and out degrees of vertex */
void insertPagerank(double pagerank, int pos, int outgoing, urlL given) {
    /* Do access and insertions */
    given->list[pos].pagerank = pagerank;
    given->list[pos].outDegree = outgoing;   
}

/* Sort the given url list */
void sort(urlL given) {
    int lo = 0;
    int hi = given->nElem - 1;
    mergeSort(given->list, lo, hi);
}

/* Performing mergeSort on buffer */
/* Mergsort and helper functions adapted for our ADT from 2521 lecture sorting
 examples */
static void mergeSort(Node list, int lo, int hi) {
    int mid = (lo+hi)/2; 
    if (hi <= lo) return;
    mergeSort(list, lo, mid);
    mergeSort(list, mid+1, hi);
    merge(list, lo, mid, hi);
}

/* Helper function for merging the two */
static void merge(Node list, int lo, int mid, int hi) {
    int  i, j, k, nitems = hi-lo+1;
    Node tmp = malloc(nitems*sizeof(urlNode));

    i = lo; j = mid+1; k = 0;
    while (i <= mid && j <= hi) {
	if (more(list[i].pagerank,list[j].pagerank))
	    copy(tmp, &k, list, &i);
	else
	    copy(tmp, &k, list, &j);
    }
    while (i <= mid) copy(tmp, &k, list, &i);
    while (j <= hi) copy(tmp, &k, list, &j);

    /* Copy back from the temp array */
    for (i = lo, k = 0; i <= hi; i++, k++) {
	list[i] = tmp[k];
    }
    free(tmp);
}

/* Helper function to simplify check on less */
static int more(double x, double y) {
    return (x > y);
}

/* Helper function to do copying */
static void copy(Node a, int *i, Node b, int *j) {
    a[*i] = b[*j];
    *i = *i + 1;
    *j = *j + 1;
}

/* Display the given urlLlist */
void displayList(urlL given) {
    int i = 0;
    while (i < given->nElem) {
	printf("Pagerank: %.7lf, Outlinks: %d\n", given->list[i].pagerank,
	       given->list[i].outDegree);
	i++;
    }
}

/* Write the pageRanks to file */
void writeToFile(urlL given) {
    FILE *fptr = fopen("pagerankList.txt", "w");
    int i = 0;
    while (i < given->nElem) {
	fprintf(fptr, "%s, %d, %.7f\n", given->list[i].url,
		given->list[i].outDegree, given->list[i].pagerank);
	i++;
    }
    fclose(fptr);
}
