/* Interface for standard tf-IDF operations to be performed */

#ifndef TFIDF_H
#define TFIDF_H

typedef struct invDoc *IDHold;
typedef struct termFreq *tfIdfData;

/* Data structure for inverse document */
IDHold createInverse(void);

/* Populate memory from provided files */
tfIdfData populateData(IDHold provided);

/* With populated data, calculate the tf-IDF for a given set */
void calcTfIdf(char **array, tfIdfData given, IDHold tree, int elem);

/* Display search engine resorts on tf-IDF in sorted manner */
void displayOut(tfIdfData given);
#endif
