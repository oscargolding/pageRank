/* A file to search for aggregate 2 rank files */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "scaledFootrule.h"

typedef struct urlElement* urlNode;
typedef struct listHeader* urlListHeader;
typedef struct fileDescriptorsLL* fileDescriptorNode;
typedef struct fileDescriptorsLLHead* fileDescriptorHead;
typedef struct urlTree* urlTree;


/* Structure of the Matrix */
typedef struct phNode *Node;

typedef struct phNode {
    double foot;
    int crossedOut;
    int assigned;
    int ignored;
    int line;
    int marked;
    int rowMark;
    int colMark;
} phNode;

static int printInFix(urlTree given, int *count);
static urlTree rotateRight(urlTree n1);
static urlTree rotateLeft(urlTree n1);
static int treeDepth(urlTree given);
static int countNodeInAVLTree(urlTree given);
static void getArray(urlTree given, char **array, int *i);
// static void printLL(urlNode given);

/* Helper functions to create a Matrix */
static Node *createEmptyMatrix(int setSize);
static Node *calcFootRule(int unionSize, char **Union, urlNode *array,
			  int arraySize);
static int findPlace(urlNode given, char *word);
static void printing (Node *given, int unionSize);
static void minAndSub(Node given, int size);
static void minColSub(Node *given, int size, int col);
static void hungarian(Node *given, int size);
static void zeroAssignment(Node *given, int size);
static void markRows(Node *given, int col, int size);
static void checkRowAssign(Node *given, int row, int size);
static void markCol(Node *given, int size, int col);
static void markRowAcross(Node *given, int size, int row);
static int calcMin(Node *given, int size);
static void findMinAndSubtract(Node *given, int size);
static int *findPost(Node *given, int size, int *array);
static int checkInside(int *array, int size, int pos);
static int findSize(urlNode given);

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


typedef struct list *List;

typedef struct list {
    char *url;
    List next;
} list;

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
    printf("Num of distinct URLs is: %d\n", countNodeInAVLTree(unionOfURLs));
    /* Need to have the proper set-up */
    int count = 0;
    printInFix(unionOfURLs, &count);
    printf("Done\n");
    char **using = malloc(sizeof(char *) * count);
    count = 0;
    getArray(unionOfURLs, using, &count);
    urlNode *list = malloc((argc-1) * sizeof(urlNode));
    printf("Done\n");
    fileDescriptorNode current = fileLL->first;
    int Ocount = 0;
    while (Ocount < 5) {
	printf("%s\n", using[Ocount]);
	Ocount++;
    }
    printf("Done\n");
    int size = 0;
    while (current != NULL) {
	list[size] = current->rankOrder->first;
	current = current->next;
	size++;
    }
    printf("size %d\n", size);
    printf("count %d\n", count);
    Node *a = calcFootRule(count, using, list, size);
    printing(a, 5);
    printf("Hungarian\n");
    hungarian(a, 5);
    
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

static int countNodeInAVLTree(urlTree given) {
    if (given == NULL) return 0;
    return countNodeInAVLTree(given->left) + countNodeInAVLTree(given->right) + 1;
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

static int printInFix(urlTree given, int *count) {
    if(given == NULL) return;
    printInFix(given->right, count);
    printf("%s\n", given->url);
    *count += 1;
    printInFix(given->left, count);
}

static void getArray(urlTree given, char **array, int *i) {
    if(given == NULL) return;
    getArray(given->right, array, i);
    array[*i] = strdup(given->url);
    printf("%s\n", array[*i]);
    *i += 1;
    getArray(given->left, array, i);
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


/* Create an empty matrix and return it */
static Node *createEmptyMatrix(int setSize) {
    Node *a = malloc(sizeof(Node *) * (setSize * setSize));
    int i = 0;
    for(i=0;i < setSize;i++) {
	a[i] = malloc(sizeof(phNode) * setSize);
    }
    return a;
}

/* Calculate the footRule and return it organised */
static Node *calcFootRule(int unionSize, char **Union, urlNode *array,
			  int arraySize) {
    Node *creation = createEmptyMatrix(unionSize);
    int i = 0;
    while (i < unionSize) {
	int row = 0;
	while (row < unionSize) {
	    double sum = 0;
	    int nodes = 0;
	    while (nodes < arraySize) {
		int place = findPlace(array[nodes], Union[i]);
		if (place == -1) {
		    nodes++;
		    continue;
		}
		else {
		    double lead = (double)(place + 1)/(double)findSize(array[nodes]);
		    double back = (double)(row + 1)/(double)unionSize;
		    sum += fabs(lead - back); 
		}
		nodes++;
	    }
	    creation[row][i].foot = sum;
	    creation[row][i].assigned = 0;
	    creation[row][i].rowMark = 0;
	    creation[row][i].colMark = 0;
	    row++;
	}
	i++;
    }
    return creation;
}

/* Find the place of a word in a given list */
static int findPlace(urlNode given, char *word) {
    urlNode current = given;
    int i = 0;
    while (current != NULL) {
	if (strcmp(current->url, word) == 0) return i;
	i++;
	current = current->next;
    }
    /* In this instance want to return -1 indicating nothing found */
    return -1;
}

/* Simple -> find the size of a Linked List */
static int findSize(urlNode given) {
    int i = 0;
    urlNode current = given;
    while (current != NULL) {
	i++;
	current = current->next;
    }
    return i;
}

/* Print out the given footRules */
static void printing (Node *given, int unionSize) {
    int row = 0;
    while (row < unionSize) {
	int col = 0;
	while (col < unionSize) {
	    printf("(%d, %d) foot: %lf ", row, col, given[row][col].foot);
	    col++;
	}
	row++;
	printf("\n");
    }
}

/* The Hungarian Algorithm basic */
static void hungarian(Node *given, int size) {
    int row = 0;
    while (row < size) {
	minAndSub(&given[row][0], size);
	row++;
    }
    int col = 0;
    while (col < size) {
	minColSub(given, size, col);
	col++;
    }
    printing(given, size);
    zeroAssignment(given, 5);
}

/* Find the row min and subtract off matrix */
static void minAndSub(Node given, int size) {
    int i = 0;
    double min = given[i].foot;
    while (i < size) {
	if (given[i].foot < min) min = given[i].foot;
	i++;
    }
    i = 0;
    while (i < size) {
	given[i].foot -= min;
	i++;
    }
}

/* Find the column min and subtract off matrix */
static void minColSub(Node *given, int size, int col) {
    int i = 0;
    double min = given[i][col].foot;
    while (i < size) {
	if (given[i][col].foot < min) min = given[i][col].foot;
	i++;
    }
    i = 0;
    while (i < size) {
	given[i][col].foot -= min;
	i++;
    }
}

/* Assign zeros to parts of the matrix */
static void zeroAssignment(Node *given, int size) {
    int row = 0;
    int found = 0;
    while (row < size) {
	int col = 0;
	int hit = 0;
	while (col < size) {
	    if (given[row][col].foot == 0 && hit == 0 &&
		given[row][col].crossedOut == 0) {
		found++;
		given[row][col].assigned = 1;
		hit = 1;
	    } else if (given[row][col].foot == 0 && hit != 0) {
		given[row][col].crossedOut = 1;
	    }
	    col++;
	}
	row++;
    }
    markRows(given, 0, size);
    printf("There are: %d\n", calcMin(given, size));
    int min = calcMin(given, size);
    if (min < size) {
	findMinAndSubtract(given, size);
	markRows(given, 0, size);
	min = calcMin(given, size);
    }
    int array[size];
    memset(array, -1, size * sizeof(int));
    int i = 0;
    int *hold = findPost(given, size, array);
    printing(given, size);
    while (i < size) {
	printf("%d\n", hold[i]);
	i++;
    }    
}

/* Find the correct parts of matrix to allocate inside a list */
static int *findPost(Node *given, int size, int *array) {
    int col = 0;
    int *hold = malloc(size * sizeof(int));
    while (col < size) {
	int row = 0;
	while (row < size) {
	    if (given[row][col].foot == 0) {
		if (checkInside(array, size, row) == 1) {
		    array[row] = row;
		    hold[col] = row + 1;
		    break;
		}
				
	    }
	    row++;
	}
	col++;
    }
    return hold;
}

/* Check inside a list to see whether present */
static int checkInside(int *array, int size, int pos) {
    int i = 0;
    while (i < size) {
	if (array[i] == pos) {
	    return 0;
	}
	i++;
    }
    return 1;
}

/* Check downwards and mark */
static void crossDownwards(Node *given, int size, int row) {
    int i = 0;
    while (i < size) {
	if (given[row][i].foot == 0) {
	    given[row][i].crossedOut = 1;
	}
	i++;
    }
}

/* Mark all rows in the matrix as per conditions needed */
static void markRows(Node *given, int col, int size) {
    int row = 0;
    while (row < size) {
	checkRowAssign(given, row, size);
	row++;
    }
}

/* Check whether the row has been assigned or not */
static void checkRowAssign(Node *given, int row, int size) {
    int i = 0;
    while (i < size) {
	if (given[row][i].assigned == 0) {
	    i++;
	} else {
	    return;
	}
    }
    i = 0;
    given[row][0].rowMark = 1;
    while (i < size) {
	if (given[row][i].foot == 0) {
	    markCol(given, size, i);
	}
	given[row][i].marked = 1;
	i++;
    }
}

/* Check the column for any assigned rows */
static void markCol(Node *given, int size, int col) {
    int i = 0;
    int count = 0;
    if (given[0][col].colMark == 1) {
	return;
    }
    given[0][col].colMark = 1;
    i = 0;
    while (i < size) {
	given[i][col].marked = 1;
	i++;
    }    
}

/* Marks rows going across */
static void markRowAcross(Node *given, int size, int row) {
    int i = 0;
    if (given[row][0].rowMark == 1) {
	return;
    }
    while (i < size) {
	given[row][i].marked = 1;
	i++;
    }
}

/* Find the minimum number of lines drawn over matrix */
static int calcMin(Node *given, int size) {
    int col = 0;
    int min = 0;
    while (col < size) {
	if (given[0][col].colMark == 1) min +=1;
	col++;
    }
    int row = 0;
    while (row < size) {
	if (given[row][0].rowMark != 1) min +=1;
	row++;
    }
    return min;
}

/* Find the minimum of the matrix and then subtract, also want to clear the 
 * matrix for use in subsequent iterations*/
static void findMinAndSubtract(Node *given, int size) {
    int row = 0;
    int hit = -1;
    while (row < size) {
	int col = 0;
	while (col < size) {
	    if (given[row][col].marked == 0) {
		if (hit == -1) {
		    hit = given[row][col].foot;		    
		}
		else if (given[row][col].foot < hit) {
		    hit = given[row][col].foot;
		}
	    }
	    col++;
	}
	row++;
    }
    row = 0;
    while (row < size) {
	int col = 0;
	while (col < size) {
	    if (given[row][col].marked == 0) {
		given[row][col].foot -= hit;
	    } else if (given[row][col].marked == 2) {
		given[row][col].foot += hit;
	    }
	}
    }
    row = 0;
    while (row < size) {
	int col = 0;
	while (col < size) {
	    given[row][col].marked = 0;
	    given[row][col].colMark = 0;
	    given[row][col].rowMark = 0;
	    given[row][col].assigned = 0;
	    given[row][col].crossedOut = 0;
	    col++;
	}
	row++;
    }
    
}
