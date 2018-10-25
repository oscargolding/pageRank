/* An ADT for assisting in the calculation of tf-IDF from text files 
 * Using standard tree operations and reading from files
 */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "tfIdf.h"

typedef struct IDNode *IDTree;
typedef struct tNode *Freq;

/* Using a tree data structure for inverse document calculations */
typedef struct IDNode {
    /* Key */
    char *word;
    int count;
    IDTree left;
    IDTree right;
} IDNode;

typedef struct invDoc {
    IDTree given;
    int noURLs;
} invDoc;

typedef struct tNode {
    IDTree file;
    int elements;
    char *url;
    double tfIdf;
    int found;
} tNode;

typedef struct termFreq {
    int nItems;
    Freq list;
} termFreq;

/* Collection of tree helper functions to use */
static IDTree newSet(char *url, int found);
static IDTree insertSet(IDTree given, char *word, int count);
static int treeHeight(IDTree given);
static IDTree createTree(void);
static IDTree leftRotation(IDTree n2);
static IDTree rightRotation(IDTree n1);
// static void showTree(IDTree given);
static IDTree urlTreeInsert(IDTree given, char *word);

/* File helping functions */
static int countURLs(FILE *given);
static void readingSet(char *url, Freq given);
static char *lowerString(char *given);

/* Calculation helper functions */
static int findInTree(IDTree tree, char *given);
static void calcBased(char *input, tfIdfData provided, IDHold tree);
static void copy(Freq a, int *i, Freq b, int *j);
static int more(Freq x, Freq y);
static void merge(Freq list, int lo, int mid, int hi);
static void mergeSort(Freq list, int lo, int hi);

/* When provided a double array of URLs, use this to create the inverse 
 * doucment ADT for efficient calculations of the 'corpus' body of documents */
IDHold createInverse(void) {
    IDHold new = malloc(sizeof(invDoc));
    new->noURLs = 0;
    new->given = createTree();
    FILE *using = fopen("invertedIndexTest.txt", "r");
    if (!using) {
        perror("Failed to open invertedIndex.txt\n");
        exit(EXIT_FAILURE);
    }
    FILE *collect = fopen("collection.txt", "r");
    if (!collect) {
        perror("Failed to open collection.txt\n");
        exit(EXIT_FAILURE);
    }
    /* At this point read the file to do the appropriate operations */
    char line[10000];
    /* Read until the end of the file is reached */
    while (fgets(line, 10000, using) != NULL) {
	char delimiters[3];
	delimiters[0] = '\n';
	delimiters[1] = ' ';
	delimiters[2] = '\0';
	char *final;
	char *used;
	int i = 0;
	int counter = 0;
	char *try = line;
	while ((used = strsep(&try, delimiters)) != NULL) {
	    /* Simple condition of breaking at the end */
	    if (try == NULL) {
		break;
	    }
	    if (i == 0) {
		final = strdup(used);
		i = 1;
	    } else counter++;	    
	}
	if (counter) new->given = insertSet(new->given, final, counter);
	line[0] = '\0';
    }
    new->noURLs = countURLs(collect);
    // showTree(new->given);
    // printf("The total amount of links are: %d\n", new->noURLs);
    fclose(using);
    fclose(collect);
    return new;
}

/* Populate respective file trees with data before calculation of tf-IDF */
tfIdfData populateData(IDHold provided) {
    /* Open the collections file as a starting point */
    FILE *conn = fopen("collection.txt", "r");
    if (conn == NULL) {
        perror("Couldn't open connection.txt file\n");
        exit(EXIT_FAILURE);
    }
    /* Set up the data structure to hold memory information on relevant files */
    tfIdfData using = malloc(sizeof(termFreq));
    using->nItems = provided->noURLs;
    using->list = malloc(using->nItems * sizeof(tNode));
    char foundArr[1000];
    int i = 0;
    while (fscanf(conn, "%s", foundArr) != EOF) {
	using->list[i].url = strdup(foundArr);
	using->list[i].file = createTree();
	using->list[i].elements = 0;
	using->list[i].tfIdf = 0;
	using->list[i].found = 0;
	i++;
    }
    i = 0;
    while (i < using->nItems) {
	readingSet(using->list[i].url, &using->list[i]);
	// printf("### %s ###\n", using->list[i].url);
	// showTree(using->list[i].file);
	// printf("Total elements: %d\n", using->list[i].elements);
	i++;
    }
    return using;
}
/* Given a URL and type, populate the tree structure */
static void readingSet(char *url, Freq given) {
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
    if (start == NULL) {
        perror("Can't open file\n");
        exit(EXIT_FAILURE);
    }
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
	    given->elements++;
	    given->file = urlTreeInsert(given->file, better);	    
	}
    }
}

/* Count the number of URLs present in a file */
static int countURLs(FILE *given) {
    fseek(given, 0, SEEK_SET);
    char finding[100];
    int found = 0;
    while (fscanf(given, "%s", finding) != EOF) {
	found++;       
    }
    /* Reset to the start of the file */
    fseek(given, 0, SEEK_SET);
    return found;
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

/* Create an empty tree */
static IDTree createTree(void) {
    return NULL;
}

/* Create a new set and return it to caller */
static IDTree newSet(char *url, int found) {
    /* Simple way to make a new Tree Node for usage */
    char *holding = strdup(url);
    IDTree handle = malloc(sizeof(IDNode));
    handle->word = holding;
    handle->count = found;
    handle->left = NULL;
    handle->right = NULL;    
    return handle;
}

/* Simple operation allowing for rotation of trees */
/* Rotation happens in the right direction */
static IDTree rightRotation(IDTree n1) {
    if (n1 == NULL || n1->left == NULL) {
	return n1;
    }
    IDTree n2 = n1->left;
    n1->left = n2->right;
    n2->right = n1;
    return n2;
}

/* Simple operation allowing for rotation of trees */
/* Rotation happens in the left direction */
static IDTree leftRotation(IDTree n2) {
    if (n2 == NULL || n2->right == NULL) {
	return n2;
    }
    IDTree n1 = n2->right;
    n2->right = n1->left;
    n1->left = n2;
    return n1;
}

/* Calculate the height of the given tree based on premise of bottom level
 * being a height of 1 -> this is mainly a helper function for AVL Trees
 */
static int treeHeight(IDTree given) {
    if (given == NULL) return 0;
    else {
	int leftHeight = 1 + treeHeight(given->left);
	int rightHeight = 1 + treeHeight(given->right);
	if (leftHeight > rightHeight) return leftHeight;
	else return rightHeight;					  
    }
}

/* Perform an insert on the tree -> using an AVL Tree for height balancing
 * Creating a height Balanced Tree for simple Inverse Document Usage
*/
static IDTree insertSet(IDTree given, char *word, int count) {
    /* Base case for insertion, when it's NULL */
    if (given == NULL) {
	return newSet(word, count);
    }
    /* Checking that we've found the word inside the tree*/
    if (strcmp(word, given->word) == 0) {
	/* When finding the same just return it as per normal */
	return given;
    }
    if (strcmp(word, given->word) < 0) {
	given->left = insertSet(given->left, word, count);
    } else {
	given->right = insertSet(given->right, word, count);
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

// /* Show the Tree using inorder */
// static void showTree(IDTree given) {
//     if (given == NULL) return;
//     showTree(given->left);
//     printf("%s : %d\n", given->word, given->count);
//     showTree(given->right);
// }

/* Another AVL insertion function, except this can respond dynamically to 
 * the presence of there already being a line of text in the tree */
static IDTree urlTreeInsert(IDTree given, char *word) {
    /* Base case for insertion, when it's NULL */
    if (given == NULL) {
	return newSet(word, 1);
    }
    /* Checking that we've found the word inside the tree*/
    if (strcmp(word, given->word) == 0) {
	/* When finding the same just return it as per normal */
	given->count++;
	return given;
    }
    if (strcmp(word, given->word) < 0) {
	given->left = urlTreeInsert(given->left, word);
    } else {
	given->right = urlTreeInsert(given->right, word);
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

/* Given a populated data set, calculate the tf-IDF values for words provided
 * from STDIN. The results must be printed out to STDOUT */
void calcTfIdf(char **array, tfIdfData given, IDHold tree, int elem) {
    int i = 0;
    while (i < elem) {
	calcBased(array[i], given, tree);
	i++;
    }
    i = 0;
 //    printf("Showing results\n");
 //    while (i < given->nItems) {
	// printf("%s %d %lf\n", given->list[i].url, given->list[i].found,
	//        given->list[i].tfIdf);
	// i++;
 //    }
    mergeSort(given->list, 0, given->nItems-1);
    i = 0;
    // printf("Showing results\n");
 //    while (i < given->nItems) {
	// printf("%s %d %lf\n", given->list[i].url, given->list[i].found,
	//        given->list[i].tfIdf);
	// i++;
 //    }
}

/* Find a given value in a binary search tree */
static int findInTree(IDTree tree, char *given) {
    if (tree == NULL) return 0;
    if (strcmp(tree->word, given) == 0)
	return tree->count;
    else if (strcmp(tree->word, given) > 0)
	return findInTree(tree->left, given);
    else if (strcmp(tree->word, given) < 0)
	return findInTree(tree->right, given);
    return 0;
}

/* A helper function to perform calculations of tf-IDF values that are needed */
static void calcBased(char *input, tfIdfData provided, IDHold tree) {
    int found = findInTree(tree->given, input);
    // printf("%d found\n", found);
    // showTree(tree->given);
    int totN = tree->noURLs;
    if (found == 0 || totN == 0) return;
    else {
	double frac = (double)totN/(double)found;
	double idf = log(frac);
	int i = 0;
	while (i < provided->nItems) {
	    int termFreq = findInTree(provided->list[i].file, input);
	    int docFreq = provided->list[i].elements;
	    if (termFreq > 0 && docFreq > 0) {
		double tf = (double)termFreq/(double)docFreq;
		double tfIdf = tf * idf;
		provided->list[i].found++;
		provided->list[i].tfIdf += tfIdf;
	    }
	    i++;
	}
    }
}

/* Performing mergeSort on buffer */
/* Mergsort and helper functions adapted for our ADT from 2521 lecture sorting
 examples */
static void mergeSort(Freq list, int lo, int hi) {
    int mid = (lo+hi)/2; 
    if (hi <= lo) return;
    mergeSort(list, lo, mid);
    mergeSort(list, mid+1, hi);
    merge(list, lo, mid, hi);
}

/* Helper function for merging the two */
static void merge(Freq list, int lo, int mid, int hi) {
    int  i, j, k, nitems = hi-lo+1;
    Freq tmp = malloc(nitems*sizeof(tNode));
    i = lo; j = mid+1; k = 0;
    while (i <= mid && j <= hi) {
	if (more(&list[i],&list[j]))
	    copy(tmp, &k, list, &i);
	else
	    copy(tmp, &k, list, &j);
    }
    while (i <= mid) copy(tmp, &k, list, &i);
    while (j <= hi) copy(tmp, &k, list, &j);

    /* Copy back from the temp array */
    for (i = lo, k = 0; i <= hi; i++, k++) {
	list[i] = tmp[k];
    }
    free(tmp);
}

/* Helper function to simplify check on less */
static int more(Freq x, Freq y) {
    if (x->found > y->found) return 1;
    if (x->found == y->found) {
	if (x->tfIdf > y->tfIdf) return 1;
	else return 0;
    } else return 0;
}

/* Helper function to do copying */
static void copy(Freq a, int *i, Freq b, int *j) {
    a[*i] = b[*j];
    *i = *i + 1;
    *j = *j + 1;
}

void displayOut(tfIdfData given) {
    if (given->nItems == 0) return;
    else {
	mergeSort(given->list, 0, given->nItems-1);
	int i = 0;
	while (i < given->nItems && i < 30) {
	    if (given->list[i].found == 0) break;
	    printf("%s %.6f\n", given->list[i].url, given->list[i].tfIdf);
	    i++;
	}
    }

}
