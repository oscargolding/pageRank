/* A file to search for tf-IDF values */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tfIdf.h"

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
    IDHold used = createInverse();
    tfIdfData using = populateData(used);
    calcTfIdf(list, using, used, index);
    displayOut(using);
    return EXIT_SUCCESS;
}
