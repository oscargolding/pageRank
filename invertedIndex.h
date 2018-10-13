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

/* Sanity check to ensure AVL properties of set */
void sanityCheck(Tree given);

#endif
