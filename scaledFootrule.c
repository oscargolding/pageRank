/* A file to search for aggregate 2 rank files */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct urlElement* urlNode;
typedef struct listHeader* listHeader;
typedef struct fileDescriptorsLL* fileDescriptorNode;
typedef struct fileDescriptorsLLHead* fileDescriptorHead;


struct listHeader
{
    urlNode list;
    int numURLs;

};

//A linked list of URLs node
typedef struct urlNode {
    char* url;
    urlNode next;
} urlElement;

//Linked list of files (ranks) to combine, must be linked list as there is a dynamic
//number of possible file descriptors
struct fileDescriptorsLL
{
    FILE *rankFIle;
    fileDescriptorNode next;
};

struct fileDescriptorsLLHead
{
    FILE *rankFIle;
    fileDescriptorNode next;
    
};

int main (int argc, char *argv[]) {
    
    if(argc < 3) {
        printf("Usage: %s input_rank_file1, input_rank_file2... \n", argv[0]);
        return -1;
    }

    int i = 2;
    fileDescriptorHead fileLL = malloc(sizeof(struct fileDescriptorsLL));
    fileLL->rankFile = fopen(argv[1],"r");

    while(i < argc) {

    }

    FILE *rankA = fopen(argv[1], "r");
    if (rankA == NULL) {
        printf("Failed to open %s\n", argv[1]);
        return -1;
    }
    FILE *rankB = fopen(argv[2], "r");
    if (rankA == NULL) {
        printf("Failed to open %s\n", argv[2]);
        return -1;
    }



    return EXIT_SUCCESS;
}

//Reads in a file descriptor and converts the urls in that file to a linked list
//The order they appear in the linked list
listHeader createURLlist(FILE* rankFile) {
    listHeader listContainer = malloc(sizeof(struct listHeader));
    char line[10000]; //buffer to store url in

    while (fgets(line, 10000, rankFile) != NULL) {

    }
}