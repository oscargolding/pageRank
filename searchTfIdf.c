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
    printf("### Words provided ###\n");
    while (i < argc) {
	list[index] = strdup(argv[i]);
	i++;
	printf("%s\n", list[index]);
	index++;
    }
    printf("### Finished Words Provided ###\n");
    printf("### Creating Inverse Storage ###\n");
    IDHold used = createInverse();
    printf("### Finished Inverted Storage ###\n");
    printf("### Populating memory with text ###\n");
    tfIdfData using = populateData(used);
    printf("### Finished populating memory ###\n");
    printf("### Doing cacls ###\n");
    calcTfIdf(list, using, used, index);
    printf("### Finished calcs ###\n");
    printf("### Displaying to STDOUT ###\n");
    displayOut(using);
    printf("### Finished displaying out ###\n");
    return EXIT_SUCCESS;
}
