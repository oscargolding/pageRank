/* Reading data from file and populating to create a list */
/***
 *      ____                _ ____        _             _    ____ _____ 
 *     |  _ \ ___  __ _  __| |  _ \  __ _| |_ __ _     / \  |  _ \_   _|
 *     | |_) / _ \/ _` |/ _` | | | |/ _` | __/ _` |   / _ \ | | | || |  
 *     |  _ <  __/ (_| | (_| | |_| | (_| | || (_| |  / ___ \| |_| || |  
 *     |_| \_\___|\__,_|\__,_|____/ \__,_|\__\__,_| /_/   \_\____/ |_|  
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
#include <assert.h>
#include "invertedIndex.h"
#include "readData.h"

#define TRUE 1
#define FALSE 0

/* Want to create a Node to hold the crucial information for urls */
typedef struct urlNode *Node;

typedef struct urlNode {
    char *url;
    double pagerank;
    int outDegree;
    int nItems;    
} urlNode;

/* Create an overview struct with information */
struct urlList {
    int nElem;
    int used;
    int index;
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

/* Helper other reading */
static int checkIfIn(char *given, char **list, int no);
static int checkURL(char *given);
// static void checkInside(char *url, urlL handle);
static void clean(urlL given);
// static void printMatched(urlL handle);
// static int checkInsideURL(char *url, urlL given);
// static void insertInside(double pageRank, urlL given, char *url);
// static void printMatchedPage(urlL handle);
// static void sortOnPageSub(urlL given);
// static int cmpfunc1 (const void * a, const void * b);
// static int cmpfunc (const void * a, const void * b);

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

/* Check if it's inside */
static int checkIfIn(char *given, char **list, int no) {
    int i = 0;
    while (i < no) {
	if (strcmp(given, list[i]) == 0) return TRUE;
	i++;
    }
    return FALSE;
}

/* Check the URL */
static int checkURL(char *given) {
    if (strlen(given) < 3) return FALSE;
    if (given[0] == 'u' && given[1] == 'r' && given[2] == 'l') return TRUE;
    return FALSE;
}

/* Check if the given is inside the list (if it is increment))*/
/* If it's not, add to the end */
// static void checkInside(char *url, urlL handle) {
    
//     int i = 0;
//     while (i < handle->index) {
// 	if (strcmp(handle->list[i].url, url) == 0) {
// 	    handle->list[i].nItems++;
// 	    return;
// 	}
// 	i++;
//     }
//     handle->list[handle->index].url = strdup(url);
//     handle->list[handle->index].nItems = 1;
//     handle->index++;
// }

static void clean(urlL given) {
    int i = 0;
    while (i < given->nElem) {
	given->list[i].url = NULL;
	i++;
    }
    given->index = 0;
    given->used = 0;
}
// static int cmpfunc (const void * a, const void * b) {
//     return (((Node)b)->nItems - ((Node)a)->nItems);
// }

static Tree addURL(Tree given, char* url) {
    //Adds the url into an AVL tree, if it is already there it increments
    //the number of occurances, a string must be passed in to satisfy ADT requirements
    Tree newTree = insertSet(given, url, "emptyString");
    return newTree;
}

/* Find macthing URLs 
 * This isn't an O(n^2) loop, as it's performing the same operation of reading 
 * from a file with more conditions.
*/
void findMatchedURLs(char **list, int no) {
    FILE *retrieve = fopen("invertedIndex.txt", "r");
    if (retrieve == NULL) perror("Couldn't open invertedIndex.txt");
    int tot = countURLs(retrieve);
    urlL handle = malloc(sizeof(urlList));
    handle->nElem = tot;
    handle->list = malloc(tot * sizeof(urlNode));
    clean(handle);	
    char foundArr[1000];
    Tree urlTree = createSet();
    while (fscanf(retrieve, "%s", foundArr) != EOF) {
    	if(checkIfIn(foundArr, list, no)) {
    	    while (fscanf(retrieve, "%s", foundArr) != EOF) {
        		if (checkURL(foundArr)) {
                    // printf("Adding: %s\n", foundArr);
                    urlTree = addURL(urlTree, foundArr);
        		    // checkInside(foundArr, handle);
        		} else {
        		    if(checkIfIn(foundArr, list, no)) continue;
        		    else break; 
        		}
    	    }
    	}
    }

    //qsort(handle->list, handle->index, sizeof(urlNode), cmpfunc);
    //printMatched(handle);
    //printf("Done\n");
    // addPageRanks(handle);
    addPageRanks(urlTree);
    
}

// static void printMatched(urlL handle) {
//     int i = 0;
//     while (i < handle->index) {
// 	printf("%s\n", handle->list[i].url);
// 	printf("%d\n", handle->list[i].nItems);
// 	i++;
//     }
// }

/*Function to add pageRanks from file to AVL tree */
void addPageRanks(Tree given) {
    FILE *gotten = fopen("pagerankList.txt", "r");
    assert(gotten != NULL);
    char foundArr[1000];
    char holding[1000];
    holding[0] = '\0';
    int i = 0;
    int hit = 0;

    while (fscanf(gotten, "%s", foundArr) != EOF) {
        if(i % 3 == 0) {
            int len = strlen(foundArr);
            foundArr[len-1] = '\0';
            if(isInTree(given,foundArr) == 1) {
                // printf("foundArr is: %s\n", foundArr);
                hit = 1;
                strcpy(holding, foundArr);
            }
        }
        if(i % 3 == 2 && hit == 1) {
            int len = strlen(foundArr);
            foundArr[len-1] = '\0';
            double pageRank = strtod(foundArr, NULL);
            if(updatePageRank(given, holding, pageRank) == 0) {
                printf("Failed to update page rank (readData.c:343)\n");
            }
            hit = 0;
            holding[0] = '\0';
        } 
        i++;
    }
    printPageRank(given);
}


/* Function to add pageRanks from file */
// void addPageRanks(urlL given) {
//     FILE *gotten = fopen("pagerankList.txt", "r");
//     assert(gotten != NULL);
//     char foundArr[1000];
//     char holding[1000];
//     holding[0] = '\0';
//     int i = 0;
//     int hit = 0;
//     while (fscanf(gotten, "%s", foundArr) != EOF) {
// 	if (i % 3 == 0) {
// 	    int len = strlen(foundArr);
// 	    foundArr[len-1] = '\0';
// 	    if (checkInsideURL(foundArr, given)) {
// 		hit = 1;
// 		strcpy(holding, foundArr);
// 	    }
// 	}
// 	if ((i % 3 == 2) && hit == 1) {
// 	    int len = strlen(foundArr);
// 	    foundArr[len-1] = '\0';
// 	    double pageRank = strtod(foundArr, NULL);
// 	    insertInside(pageRank, given, holding);
// 	    holding[0] = '\0';
// 	    hit = 0;
// 	}
// 	i++;
//     }
//     printMatchedPage(given);
//     if (given->index > 0) sortOnPageSub(given);
//     printf("### Sorted values for results ###\n");
//     printMatchedPage(given);
// }

// static int checkInsideURL(char *url, urlL given) {
//     int i = 0;
//     while (i < given->index) {
// 	if (strcmp(given->list[i].url, url) == 0) return TRUE;
// 	i++;
//     }
//     return FALSE;
// }

/* Add the pageRank */
// static void insertInside(double pageRank, urlL given, char *url) {
//     int i = 0;
//     while (i < given->index) {
// 	if (strcmp(given->list[i].url, url) == 0) {
// 	    given->list[i].pagerank = pageRank;
// 	    return;
// 	}
// 	i++;
//     }
// }
/* Better */
// static void printMatchedPage(urlL handle) {
//     int i = 0;
//     while (i < handle->index) {
// 	printf("# Block #\n");
// 	printf("%s\n", handle->list[i].url);
// 	printf("%d\n", handle->list[i].nItems);
// 	printf("%lf\n", handle->list[i].pagerank);
// 	printf("# End Block #\n");
// 	i++;
//     }
// }

// static int cmpfunc1 (const void * a, const void * b) {
//     if (((Node)b)->pagerank > ((Node)a)->pagerank) {
// 	return 1;
//     }
//     else if (((Node)a)->pagerank > ((Node)b)->pagerank) {
// 	return -1;
//     }
//     else return 0;
// }

// static void sortOnPageSub(urlL given) {
//     int i = 0;
//     Node first = &(given->list[0]);
//     int set = given->list[0].nItems;
//     int counter = 1;
//     int seaT = 0;
//     while (i < given->index) {
// 	if (given->list[i].nItems != set) {
// 	    qsort(first, counter, sizeof(urlNode), cmpfunc1);
// 	    set = given->list[i].nItems;
// 	    first = &(given->list[i]);
// 	    counter = 1;
// 	    seaT = 0;
// 	}
// 	if (seaT != 0) counter++;
// 	seaT++;
// 	i++;
//     }
//     qsort(first, counter, sizeof(urlNode), cmpfunc1);
// }
