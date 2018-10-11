/* File holds the main function for implementing the pageRank Algorithm */

#include <stdio.h>
#include <stdlib.h>
#include "readData.h"


int main (int argc, char *argv[]) {
    /* Implement from here */
    if (argc < 4) {
	fprintf(stderr, "Program requires three arguments\n");
	exit(1);
    }
    float d = strtod(argv[1], NULL);
    float diffPR = strtod(argv[2], NULL);
    int maxIter = strtol(argv[3], NULL, 10);
    printf("The d:%lf, diffPR:%lf, maxIter:%d\n", d, diffPR, maxIter);
    urlL getList = getConnections();
    printf("%s\n", returnURL(0, getList));
    Graph g = getGraph(getList);
    showGraph(g);
    calculatePageRank(g, getList, d, diffPR, maxIter);
    showGraph(g);
    return EXIT_SUCCESS;
}
