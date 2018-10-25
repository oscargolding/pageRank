/* File holds the main function for implementing the pageRank Algorithm */

#include <stdio.h>
#include <stdlib.h>
#include "readData.h"
#include "graph.h"
#include <string.h>

static void readingVert(Graph g, char *url, urlL given, int pos);
static Graph getGraph(urlL given);

int main (int argc, char *argv[]) {
    /* Implement from here */
    if (argc < 4) {
        fprintf(stderr, "Program requires three arguments\n");
        exit(EXIT_FAILURE);
    }
    double d = strtod(argv[1], NULL);
    double diffPR = strtod(argv[2], NULL);
    int maxIter = strtol(argv[3], NULL, 10);
    printf("The d:%lf, diffPR:%lf, maxIter:%d\n", d, diffPR, maxIter);
    urlL getList = getConnections();
    printf("%s\n", returnURL(0, getList));
    Graph g = getGraph(getList);
    showGraph(g);
    calculatePageRank(g, getList, d, diffPR, maxIter);
    showGraph(g);
    sort(getList);
    displayList(getList);
    writeToFile(getList);
    return EXIT_SUCCESS;
}

static Graph getGraph(urlL given) {
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
    if (start == NULL) {
        perror("Can't open file\n");
        exit(EXIT_FAILURE);
    }
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
