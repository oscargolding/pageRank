/* File for searching for a pagerank */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readData.h"

#define TRUE 1
#define FALSE 0

int main (int argc, char *argv[]) {
    /* Need to read in from STDIN */
    char **list = malloc((argc-1)*sizeof(char *));
    int i = 1;
    int index = 0;
    while (i < argc) {
    	list[index] = strdup(argv[i]);
    	i++;
    	index++;
    }
    /* This will call a fetch function on getting the URLs */
    findMatchedURLs(list, index);
    return EXIT_SUCCESS;
}

