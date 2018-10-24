/* Main program for running inverted on a url */

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include "invertedIndex.h"
#include"readData.h"

static Tree populate(urlL given);
static Tree  readingSet(char *url, urlL given, Tree first);
static char *lowerString(char *given);

int main (int argc, char *argv[]) {
    urlL getList = getConnections();
    Tree new = populate(getList);
    writeInverted(new);
    return EXIT_SUCCESS;
}

/* Populate a tree with the given URLs */
Tree populate(urlL given) {
    int nVert = getNoURL(given);
    /* Generate a new tree */
    Tree new = createSet();
    int i = 0;
    while (i < nVert) {
	char *url = returnURL(i, given);
	new = readingSet(url, given, new);
	i++;
    }
    return new;    
}

/* Helper to allow reading from a url to get contents and text processing */
static Tree readingSet(char *url, urlL given, Tree first) {
    /* Create the file extension */
    char txt[5] = ".txt";
    char reading[1000];
    reading[0] = '\0';
    strcat(reading, url);
    strcat(reading, txt);
    /* Need to get to right reading position in the file */
    char result[1000];
    char *want = "Section-2";
    FILE *start = fopen(reading, "r");
    if (start == NULL) perror("Can't open file\n");
    fseek(start, 0, SEEK_SET);
    /* Want to reach the desired position in file to start reading */
    while (fscanf(start, "%s", result) != EOF) {
	if (strcmp(result, want) == 0) break;
    }
    /* At this point can do the required reading of files */
    char *bad = "#end";
    while (fscanf(start, "%s", result) != EOF) {
	if (strcmp(result, bad) == 0) break;
	int len = strlen(result);
	if (len > 0) {
	    /* Get the last character */
	    char cha = result[len-1];
	    switch(cha) {
	    case '.':
		result[len-1] = '\0';
		break;
	    case ',':
		result[len-1] = '\0';
		break;
	    case ';':
		result[len-1] = '\0';
		break;
	    case '?':
		result[len-1] = '\0';
		break;
	    default:
		break;
	    }
	    char *better = lowerString(result);	    
	    first = insertSet(first, better, url);	    
	}
    }
    return first;
}

/* Helper function to lower a string */
static char *lowerString(char *given) {
    int len = strlen(given);
    int i = 0;
    while (i < (len)) {
	given[i] = tolower(given[i]);
	i++;
    }
    return given;
}
