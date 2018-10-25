/* A file to search for aggregate n number of rank files */

/* ScaledFootRule -> Implementation of algorithm to calculate a scheduling 
 * problem. 
 * BRIEF: The solution provided is non-polynomial in time complexity, i.e. it
 * scales up according to the cardinality of the union. To solve this, we have
 * implemented the Hungarian Algorithm as a method to compute rankings within
 * polynomial time. 
 * Hungarian Algorithm -> given input files that are already ranked, the scaled 
 * footrule (in the specification) can be applied to determine all possible 
 * rankings at each of the positions. This allows for the creation of a simple
 * bipartite graph with the two disjoint vertex sets being available positions
 * and the names of the ranks in file. This can be done using an adjacency 
 * matrix. From here operations are done on the matrix to reduce the problem 
 * down to find a minimum cost solution (minimum cost being least weights of 
 * edges between vertexes in the disjoint sets).
 * Total cost -> O(n^4) using the matrix method of implementation. 
 *
 * Details : https://en.wikipedia.org/wiki/Hungarian_algorithm
 * https://hal.archives-ouvertes.fr/hal-01540920/document
 */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "scaledFootrule.h"

/* Data structures to help with upkeep */
typedef struct urlElement* urlNode;
typedef struct listHeader* urlListHeader;
typedef struct fileDescriptorsLL* fileDescriptorNode;
typedef struct fileDescriptorsLLHead* fileDescriptorHead;
typedef struct urlTree* urlTree;

#define TRUE 1
#define FALSE 0

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
    int noZero;
    int zMark;
} phNode;

static void printInFix(urlTree given, int *count);
static urlTree rotateRight(urlTree n1);
static urlTree rotateLeft(urlTree n1);
static int treeDepth(urlTree given);
static int countNodeInAVLTree(urlTree given);
static void getArray(urlTree given, char **array, int *i);

/* Helper functions to create a Matrix for Hungarian and perform operations */
static Node *createEmptyMatrix(int setSize);
static Node *calcFootRule(int unionSize, char **Union, urlNode *array,
  int arraySize);
static int findPlace(urlNode given, char *word);
static void printing (Node *given, int unionSize);
static void minAndSub(Node given, int size);
static void minColSub(Node *given, int size, int col);
static void hungarian(Node *given, int size, char **Union);
static int *zeroAssignment(Node *given, int size);
static void markRows(Node *given, int col, int size);
static void checkRowAssign(Node *given, int row, int size);
static void markCol(Node *given, int size, int col);
static void markRowAcross(Node *given, int size, int row);
static int calcMin(Node *given, int size);
static void findMinAndSubtract(Node *given, int size);
static int *findPost(Node *given, int size, int *array);
static int checkInside(int *array, int size, int pos);
static int findSize(urlNode given);
static void crossDownwards(Node *given, int size, int row);
static Node *copyMatrix(Node *given, int size);
static int findSmallestCol(Node *given, int size);
static int findNoZeros(Node *given, int size, int col);
static int checkZero(double given);

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

/* Linked List for storing initial information */
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
    if(fileLL->first->rankFile == NULL) {
        perror("Cannot open a file");
        exit(EXIT_FAILURE);
    }
    fileLL->first->rankOrder = createURLlist(fileLL->first->rankFile);
    fileLL->first->next = NULL;

    int i = 2;
    while(i < argc) {
        fileLL->last->next = malloc(sizeof(struct fileDescriptorsLL));
        fileLL->last = fileLL->last->next;
        fileLL->last->rankFile = fopen(argv[i],"r");
        if(fileLL->last->rankFile == NULL) {
            perror("Cannot open a file");
            exit(EXIT_FAILURE);
        }
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
    while (Ocount < count) {
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
    printing(a, count);
    printf("Hungarian\n");
    hungarian(a, count, using);
    
    return EXIT_SUCCESS;
}

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

static void printInFix(urlTree given, int *count) {
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

/* Calculate the footRule and return it organised, this is the set-up of the 
 * matrix with no operations having been performed on it yet  */
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
            creation[row][i].marked = 0;
            row++;
        }
        i++;
    }
    return creation;
}

/* Copy the matrix to we can retain original values when calculating the 
 * minimal cost of the problem */
static Node *copyMatrix(Node *given, int size) {
    Node *creation = createEmptyMatrix(size);
    int i = 0;
    while (i < size) {
        int row = 0;
        while (row < size) {
            creation[row][i].foot = given[row][i].foot;
            creation[row][i].assigned = 0;
            creation[row][i].rowMark = 0;
            creation[row][i].colMark = 0;
            creation[row][i].marked = 0;
            row++;
        }
        i++;
    }
    printf("Printing copied matrix\n");
    printing(given, size);
    printf("Finished copying\n");
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

/* Print out the given footRules, this is a helper function */
static void printing (Node *given, int unionSize) {
    int row = 0;
    while (row < unionSize) {
        int col = 0;
        while (col < unionSize) {
            printf("(%d, %d) foot: %lf cross: %d assign: %d", row, col,
             given[row][col].foot, given[row][col].crossedOut,
             given[row][col].assigned);
            col++;
        }
        row++;
        printf("\n");
    }
}

/* The Hungarian Algorithm basic, giving a broad level overview of how the 
 * algorithm performs */
static void hungarian(Node *given, int size, char **Union) {
    Node *retained = copyMatrix(given, size);
    int row = 0;
    while (row < size) {
        minAndSub(&given[row][0], size);
        row++;
    }
    printf("Min ROW\n");
    printing(given, size);
    int col = 0;
    while (col < size) {
        minColSub(given, size, col);
        col++;
    }
    printf("Min COL\n");
    printing(given, size);
    int *hold = zeroAssignment(given, size);
    int counter = 0;
    while (counter < size) {
        printf("%d\n", hold[counter]);
        counter++;
    }
    char *temp[size];
    int i = 0;
    double tot = 0;
    while (i < size) {
        tot += retained[hold[i]][i].foot;
        temp[hold[i]] = strdup(Union[i]);
        i++;    
    }
    printf("%.6f\n", tot);
    i = 0;
    while (i < size) {
        printf("%s\n", temp[i]);
        i++;
    }    
}

/* Find the row min and subtract off the matrix */
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
        if (checkZero(given[i].foot)) given[i].foot = 0;
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
        if (checkZero(given[i][col].foot)) given[i][col].foot = 0;
        i++;
    }
}

/* NB this is an important part, since after performing operations on a matrix
 * we want to check zero values, due to IEEE float standards we may be close 
 * to zero but not precisely there. Hence,  if a double value is within our 
 * below define 'interval of faith' of 0.000000005 then we classify this
 * number as being 0.0 */
static int checkZero(double given) {
    if (fabs(given) < 0.000000005) return TRUE;
    else return FALSE;
}

/* Assign zeros to parts of the matrix. This marks parts of the matrix and 
 * allows for the algorithm to continue onwards using new marked values */
static int *zeroAssignment(Node *given, int size) {
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
                crossDownwards(given, size, col);
                hit = 1;
            } else if (given[row][col].foot == 0 && hit != 0) {
                given[row][col].crossedOut = 1;
            }
            col++;
        }
        row++;
    }
    printf("Found: %d\n", found);
    markRows(given, 0, size);
    int min = calcMin(given, size);
    printf("There are %d\n", min);
    if (min < size) {
        findMinAndSubtract(given, size);
        markRows(given, 0, size);
        min = calcMin(given, size);
        printf("New min %d\n", min);
        printing(given, size);
    }
    int array[size];
    memset(array, -1, size * sizeof(int));
    int *hold = findPost(given, size, array);
    return hold;
}

/* Find the correct parts of matrix to allocate inside a list. This finds 
 * the positions within the original matrix that provided the minimum cost */
static int *findPost(Node *given, int size, int *array) {
    int newCol = 0;
    while (newCol < size) {
        given[0][newCol].noZero = findNoZeros(given, size, newCol);
        given[0][newCol].zMark = 1;
        newCol++;
    }
    int col = findSmallestCol(given, size);
    int *hold = malloc(size * sizeof(int));
    while (col != -1) {
        int row = 0;
        while (row < size) {
            if (given[row][col].foot == 0) {
                if (checkInside(array, size, row) == 1) {
                    array[row] = row;
                    hold[col] = row;
                    break;
                }
                
            }
            row++;
        }
        col = findSmallestCol(given, size);
    }
    return hold;
}

/* Find the smallest column, and return it to the parent function. When 
 * there are no cols left return -1 */
static int findSmallestCol(Node *given, int size) {
    int col = 0;
    int hit = -1;
    int sizeR = -1;
    int smallestZero;
    while (col < size) {
        if ((hit == -1) && given[0][col].zMark == 1) {
            hit = 1;
            sizeR = col;
            smallestZero = given[0][col].noZero;
        } else if ((hit == 1) && given[0][col].zMark == 1) {
            if (given[0][col].noZero < smallestZero) {
                sizeR = col;
            }
        }
        col++;
    }
    if (hit != -1) {
        given[0][sizeR].zMark = 0;
    }
    return sizeR;
}

/* Calculate the number of zeros that are present in a single column of a 
 * matrix */
static int findNoZeros(Node *given, int size, int col) {
    int num = 0;
    int row = 0;
    while (row < size) {
        if (given[row][col].foot == 0) num++;
        row++;
    }
    return num;
}

/* Check inside a list to see whether position is present or not */
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

/* Mark any zeros that appear downwards to assist in algorithm calculations */
static void crossDownwards(Node *given, int size, int col) {
    int i = 0;
    while (i < size) {
        if (given[i][col].foot == 0 && given[i][col].assigned != 1) {
            given[i][col].crossedOut = 1;
        }
        i++;
    }
}

/* Mark all rows in the matrix as per conditions needed for the algorithm*/
static void markRows(Node *given, int col, int size) {
    int row = 0;
    while (row < size) {
        checkRowAssign(given, row, size);
        row++;
    }
}

/* Check whether the row has been assigned or not, and the forwards assign 
 * columns as necessary so long as they haven't been assigned */
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
        i++;
    }
}

/* Check the column for any assigned rows, and then assign columns necessary */
static void markCol(Node *given, int size, int col) {
    int i = 0;
    if (given[0][col].colMark == 1) {
        return;
    }
    given[0][col].colMark = 1;
    i = 0;
    while (i < size) {
        if (given[i][col].assigned == 1) {
            markRowAcross(given, size, i);
        }
        i++;
    }    
}

/* Marks rows going across on the condition element contains 0 */
static void markRowAcross(Node *given, int size, int row) {
    int i = 0;
    if (given[row][0].rowMark == 1) {
        return;
    }
    given[row][0].rowMark = 1;
    while (i < size) {
        if (given[row][i].foot == 0) {
            markCol(given, size, i);
        }
        i++;
    }
}

/* Find the minimum number of lines drawn over matrix using the conditions
 * provided in the algorithm -> min being all columns marks and rows not 
 * having been marked */
static int calcMin(Node *given, int size) {
    int col = 0;
    int min = 0;
    while (col < size) {
        if (given[0][col].colMark == 1) {
            int row = 0;
            while (row < size) {
                given[row][col].marked++;
                row++;
            }
            min +=1;
        }
        col++;
    }
    /* After col Mark */
    int row = 0;
    while (row < size) {
        if (given[row][0].rowMark != 1) {
            int col = 0;
            while (col < size) {
                given[row][col].marked++;
                col++;
            }
            min +=1;
        }
        row++;
    }
    return min;
}

/* Find the minimum of the matrix and then subtract, also want to clear the 
 * matrix for use in subsequent iterations. This section of the algorithm is 
 * rarely ever reached (in most cases). But in some situations is required. */
static void findMinAndSubtract(Node *given, int size) {
    printf("Finding the minimum\n");
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
    printf("Min is %d\n", hit);
    row = 0;
    while (row < size) {
        int col = 0;
        while (col < size) {
            if (given[row][col].marked == 0) {
                given[row][col].foot -= hit;
                if (checkZero(given[row][col].foot)) given[row][col].foot = 0;
            } else if (given[row][col].marked == 2) {
                given[row][col].foot += hit;
            }
            col++;
        }
        row++;
    }
    /* This process resets the matrix to correct values */
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
