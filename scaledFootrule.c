/* A file to search for aggregate 2 rank files */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scaledFootrule.h"

typedef struct urlElement* urlNode;
typedef struct listHeader* urlListHeader;
typedef struct fileDescriptorsLL* fileDescriptorNode;
typedef struct fileDescriptorsLLHead* fileDescriptorHead;
typedef struct urlTree* urlTree;


static void printInFix(urlTree given);
static urlTree rotateRight(urlTree n1);
static urlTree rotateLeft(urlTree n1);
static int treeDepth(urlTree given);
// static void printLL(urlNode given);

struct listHeader {
    urlNode first;
    urlNode last;
    int numURLs;
};

//A linked list of URLs node
struct urlElement {
    char* url;
    urlNode next;
};

//Linked list of files (ranks) to combine, must be linked list as there is a dynamic
//number of possible file descriptors
struct fileDescriptorsLL
{
    FILE *rankFile;
    urlListHeader rankOrder;
    fileDescriptorNode next;
};

//Header for file descriptor LL allows appending to list in O(1) time
struct fileDescriptorsLLHead
{
    fileDescriptorNode first;
    fileDescriptorNode last;
    
};


//AVL tree of URLs, used when finding the union as it will work out in O(log(N)) time whether the URL is
//Already present and add it or not

struct urlTree
{
    char* url;
    urlTree left;
    urlTree right;
};



int main (int argc, char *argv[]) {
    
    if(argc < 3) {
        printf("Usage: %s input_rank_file1, input_rank_file2... \n", argv[0]);
        return -1;
    }


    //Populates the LL with the file descriptors and order of URLs
    fileDescriptorHead fileLL = malloc(sizeof(struct fileDescriptorsLLHead));
    fileLL->first = malloc(sizeof(struct fileDescriptorsLL));
    fileLL->last = fileLL->first;
    fileLL->first->rankFile = fopen(argv[1],"r");
    fileLL->first->rankOrder = createURLlist(fileLL->first->rankFile);
    fileLL->first->next = NULL;

    int i = 2;
    while(i < argc) {
        fileLL->last->next = malloc(sizeof(struct fileDescriptorsLL));
        fileLL->last = fileLL->last->next;
        fileLL->last->rankFile = fopen(argv[i],"r");
        fileLL->last->next = NULL;
        fileLL->last->rankOrder = createURLlist(fileLL->last->rankFile);
        i++;
    }
    //Now create the union of all sets using a AVL tree to do it in log(n) time
    fileDescriptorNode currFile = fileLL->first;
    urlNode currURL = NULL;
    urlTree unionOfURLs = NULL;
    while (currFile != NULL) {
        currURL = currFile->rankOrder->first;
        while(currURL != NULL) {
            unionOfURLs = insertURL(currURL->url, unionOfURLs);
            currURL = currURL->next;
        }
        currFile = currFile->next;
    }
    printInFix(unionOfURLs);
    return EXIT_SUCCESS;
}

// static void printLL(urlNode given) {
//     urlNode curr = given;
//     while(curr != NULL) {
//         printf("%s\n", curr->url);
//         curr = curr->next;
//     }
// }

//Reads in a file descriptor and converts the urls in that file to a linked list
//The order they appear in the linked list is the relative rank from the original file
urlListHeader createURLlist(FILE* rankFile) {
    char line[10000]; //buffer to store url in
    urlListHeader urlList = malloc(sizeof(struct listHeader));
    urlList->first = NULL;
    urlList->last = NULL;
    urlList->numURLs = 0;

    while (fscanf(rankFile, "%s", line) != EOF) {
        if(urlList->first == NULL) {
            urlList->first = malloc(sizeof(struct urlElement));
            urlList->last = urlList->first;
            urlList->numURLs = 1;
            urlList->first->url = strdup(line);
            urlList->first->next = NULL;
        }
        else {
            urlList->last->next = malloc(sizeof(struct urlElement));
            urlList->last = urlList->last->next;
            urlList->last->url = strdup(line);
            urlList->last->next = NULL;
            urlList->numURLs++;
        }
    }
    return urlList;
}


//Insert into tree
urlTree insertURL (char* url, urlTree given) {
    if(given == NULL) {
        urlTree tree = malloc(sizeof(struct urlTree));
        tree->url = strdup(url);
        tree->left = NULL;
        tree->right = NULL;
        return tree;
    }
    if(strcmp(given->url, url) < 0) {
         given->left = insertURL(url, given->left);
    }
    else if(strcmp(given->url, url) > 0) {
        given->right = insertURL(url, given->right);
    }
    else {
        //Aready exists in tree
        return given;
    }
    if(treeDepth(given->left)-treeDepth(given->right) > 1) {
        urlTree tmp = rotateRight(given);
        return tmp;
    }
    else if(treeDepth(given->right)-treeDepth(given->left) > 1) {
        urlTree tmp = rotateLeft(given);
         
        return tmp;
    }
    return given;

}

static void printInFix(urlTree given) {
    if(given == NULL) return;
    printInFix(given->left);
    printf("%s\n", given->url);
    printInFix(given->right);
}

static int treeDepth(urlTree given) {
    if(given == NULL) return 0;
    int leftDepth = treeDepth(given->left);
    int rightDepth = treeDepth(given->right);
    if(leftDepth > rightDepth) return 1 + leftDepth;
    else return rightDepth + 1;
}

static urlTree rotateRight(urlTree n1) {
    if(n1 == NULL || n1->left == NULL) return n1;
    urlTree tmp = n1->left;

    n1->left = tmp->right;
    tmp->right = n1;
    return tmp;
}

static urlTree rotateLeft(urlTree n2) {
    if(n2 == NULL || n2->right == NULL) return n2;
    urlTree tmp = n2->right;
    n2->right = tmp->left;
    tmp->left = n2;
    return tmp; 
}


