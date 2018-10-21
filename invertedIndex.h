/* Interface for creating an inverted index */

#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

typedef struct Node *Tree;

/* A function to insert a given URL inside a set */
Tree insertSet(Tree given, char *word, char *url);

/* Create an empty tree to use */
Tree createSet();

/* Write a tree out to a file */
void writeInverted(Tree given);

/*checks whether a word is in a tree*/
int isInTree(Tree given, char* word);

/*Updates the page rank of a URL into tree*/
int updatePageRank(Tree given, char* word, double pagerank);

/* prints the page rank from the AVL tree, just for testing should be
removed for release */
void printPageRank(Tree given);

/* Add pageRanks from file */
void addPageRanks(Tree given);

/* Sanity check to ensure AVL properties of set */
void sanityCheck(Tree given);

#endif
