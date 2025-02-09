/* Interface for reading data from the command line */


#ifndef READDATA_H
#define READDATA_H

/* Define a list ADT to use to hold urLs in the file*/
typedef struct urlList *urlL;

/* Populate the URL interface with connections in PWD on contents of 
the connections.txt file */ 
urlL getConnections(void);

/* Get the current URL and increment location to NEXT, beaviour upon reaching 
the end is to return NULL */
char *getURL(urlL given);

/* Find the number of urls in connections.txt */
int getNoURL(urlL given);

/* Insert a pagerank at a given position and outgoing links */
void insertPagerank(double pagerank, int pos, int outgoing, urlL given);

/* Return the URL at a given position, return NULL if out of bounds */
char *returnURL(int pos, urlL given);

/* Sort the buffer on pageRanks */
void sort(urlL given);

/* Display list */
void displayList(urlL given);

/* Write pageranks to file */
void writeToFile(urlL given);

/* Given a URL, find the corresponding outgoing position in list */
int findPosUrl(char *url, urlL given);
#endif

/* Find matching URLs */
void findMatchedURLs(char **list, int no);


