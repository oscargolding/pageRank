/* ADT for creating an inverted index set */

/***
 *      ___                     _           _   ___           _           
 *     |_ _|_ ____   _____ _ __| |_ ___  __| | |_ _|_ __   __| | _____  __
 *      | || '_ \ \ / / _ \ '__| __/ _ \/ _` |  | || '_ \ / _` |/ _ \ \/ /
 *      | || | | \ V /  __/ |  | ||  __/ (_| |  | || | | | (_| |  __/>  < 
 *     |___|_| |_|\_/ \___|_|   \__\___|\__,_| |___|_| |_|\__,_|\___/_/\_\
 *                                                                        
 */
/* Using an AVL Tree to run in O(log(n)) time 
 * Bookeeping functions and logic adapted from 2521 lecture examples for AVL 
 * upkeep.  
 */

#define _GNU_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"invertedIndex.h"

typedef struct listNode *list;

/* Simple Node to create a linked list with */
struct listNode {    
    char *url;
    list next;
};

typedef struct Node {
    /* Key */
    char *word;
    /* List with all urls holding key */
    int frequency; //Used for findMatchedURLs instad of storing data in array
    double pagerank; //Used for findMatchedURLs when adding pageRank
    list colUrls;
    Tree left;
    Tree right;
} Node;

/* Helper functions */
static Tree newSet(char *url, char *word);
static list createNode(char *url, list give);
static list appendOrdered(list given, char *url);
static int treeHeight(Tree given);
static Tree leftRotation(Tree n2);
static Tree rightRotation(Tree n1);
static list createNode(char *url, list give);
static void infixOrder(Tree given, FILE *fptr);
static void writeToFile(Tree given, FILE *fptr);
static void printUrls(list given, FILE *fptr);

/* Create an empty set with just NULL as base */
Tree createSet() {
    return NULL;
}

/* Create a new set and return it to caller */
static Tree newSet(char *url, char *word) {
    /* Want to persist the url */
    char *holding = strdup(word);
    Tree handle = malloc(sizeof(Node));
    handle->word = holding;
    handle->colUrls = createNode(url, NULL);
    handle->left = NULL;
    handle->right = NULL;
    handle->frequency = 1;    
    return handle;
}

/* Create a url node */
static list createNode(char *url, list give) {
    list new = malloc(sizeof(struct listNode));
    char *bet = strdup(url);
    new->url = bet;
    new->next = give;
    return new;
}

/* Simple operation allowing for rotation of trees */
/* Rotation happens in the right direction */
static Tree rightRotation(Tree n1) {
    if (n1 == NULL || n1->left == NULL) {
	return n1;
    }
    Tree n2 = n1->left;
    n1->left = n2->right;
    n2->right = n1;
    return n2;
}

/* Simple operation allowing for rotation of trees */
/* Rotation happens in the left direction */
static Tree leftRotation(Tree n2) {
    if (n2 == NULL || n2->right == NULL) {
	return n2;
    }
    Tree n1 = n2->right;
    n2->right = n1->left;
    n1->left = n2;
    return n1;
}

/* Calculate the height of the given tree */
static int treeHeight(Tree given) {
    if (given == NULL) return 0;
    else {
	int leftHeight = 1 + treeHeight(given->left);
	int rightHeight = 1 + treeHeight(given->right);
	if (leftHeight > rightHeight) return leftHeight;
	else return rightHeight;					  
    }
}

/* Maintain the order of the lists */
static list appendOrdered(list given, char *url) {
    if ((given == NULL) || (strcmp(url, given->url) < 0)) {
	return createNode(url, given);
    }
    if (strcmp(url, given->url) == 0) {
	return given;
    }
    given->next = appendOrdered(given->next, url);
    return given;    
}

/* Perform an insert on the tree */
/* The tree will be AVL -> allowing for O(log(n)) searching */
Tree insertSet(Tree given, char *word, char *url) {
    /* Base case for insertion, when it's NULL */
    if (given == NULL) {
	return newSet(url, word);
    }
    /* Checking that we've found the word inside the tree*/
    if (strcmp(word, given->word) == 0) {
	/* Want to append to the list (assume ordered) */
	given->colUrls = appendOrdered(given->colUrls, url);
    given->frequency++;
	return given;
    }
    if (strcmp(word, given->word) < 0) {
	given->left = insertSet(given->left, word, url);
    } else {
	given->right = insertSet(given->right, word, url);
    }
    int leftHeight = treeHeight(given->left);
    int rightHeight = treeHeight(given->right);
    if ((leftHeight - rightHeight) > 1) {
	if (strcmp(word, given->word) > 0) {
	    given->left = leftRotation(given->left);
	}
	given = rightRotation(given);
    } else if ((rightHeight - leftHeight) > 1) {
	if (strcmp(word, given->word) < 0) {
	    given->right = rightRotation(given->right);
	}
	given = leftRotation(given);
    }
    return given;					 
}

/* Function to write a tree out to a file */
void writeInverted(Tree given) {
    FILE *fptr = fopen("invertedIndex.txt", "w");
    assert(given != NULL);
    infixOrder(given, fptr);
    fclose(fptr);
}

/* Traveral writer helper  */
static void infixOrder(Tree given, FILE *fptr) {
    /* Base case */
    if (given == NULL) return;
    infixOrder(given->left, fptr);
    writeToFile(given, fptr);
    infixOrder(given->right, fptr);
}

/* Write the word to the file */
static void writeToFile(Tree given, FILE *fptr) {
    fprintf(fptr, "%s ", given->word);
    printUrls(given->colUrls, fptr);
}

/* Print out the file recursively with URLs */
static void printUrls(list given, FILE *fptr) {
    if (given != NULL) {
	if (given->next != NULL) {
	    fprintf(fptr, "%s ", given->url);
	} else {
	    fprintf(fptr, "%s\n", given->url);
	}
	printUrls(given->next, fptr);
    }
}

int isInTree(Tree given, char* word) {
    if(given == NULL) return 0;
    // printf("Comparing: %s and %s\n", given->word, word);
    if (strcmp(given->word, word) == 0) {
        // printf("Found a match\n");
        return 1;
    }
    int sum = isInTree(given->left, word) + isInTree(given->right, word);
    return sum;
}

int updatePageRank(Tree given, char* word, double pagerank) {
    if(given == NULL) return 0;
    if (strcmp(given->word, word) == 0) {
        given->pagerank = pagerank;
        return 1;
    }
    int sum = updatePageRank(given->left, word, pagerank) + updatePageRank(given->right, word, pagerank);
    return sum;
}

void printPageRank(Tree given) {
    if(given == NULL) return;
    printPageRank(given->left);
    printf("%s: %lf\n", given->word, given->pagerank);
    printPageRank(given->right);
    return;
}

/* Want to ensure that the Tree is height balanced */
void sanityCheck(Tree given) {
    printf("The RHS: %d\n", treeHeight(given->right));
    printf("The LHS: %d\n", treeHeight(given->left));
}
