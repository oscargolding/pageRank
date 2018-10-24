/* A file to search for tf-IDF values */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

int isInUnion(char* string, char*unionOfURLs[]);
char** populateArray(FILE* file);
void swap(char* a, char* b);
void permute(char **array,int i,int length, char** urlsList[]);
void printArray(char ** array, int length);
char** copyOrder(char** anOrder);
double calculateFootRule(char ** anOrder, int lengthOfAnOrder, char** originalOrder[]);
int numOfUrls(char ** array);
int findIndexof(char* string, char *array[]);

double currentBestFootRule = 1000000;
char ** currentBestOrder = NULL;

int main (int argc, char *argv[]) {
    /* Need to read in from STDIN */
    int fileNum = 1;
    char **urlsArray[argc];
    while(fileNum < argc) {
        FILE *fp = fopen(argv[fileNum], "r");
        urlsArray[fileNum-1] = populateArray(fp);
        fileNum++;
    }
    urlsArray[argc-1] = NULL;
    char** unionOfURLs = NULL;
    int numOfDistinctURLs = 0;
    int i = 0;
    while(urlsArray[i] != NULL) {
        int j = 0;
        while(urlsArray[i][j] != NULL) {
            if (unionOfURLs == NULL || isInUnion(urlsArray[i][j], unionOfURLs) == FALSE) {
                unionOfURLs = realloc(unionOfURLs, (numOfDistinctURLs+2)*sizeof(char*));
                unionOfURLs[numOfDistinctURLs] = strdup(urlsArray[i][j]);
                numOfDistinctURLs++;
                unionOfURLs[numOfDistinctURLs] = NULL;
            }
            j++;
        }
        i++;
    }
    unionOfURLs = realloc(unionOfURLs, (numOfDistinctURLs+1)*sizeof(char*));
    unionOfURLs[numOfDistinctURLs+1] = NULL;
    i = 0;
    while(unionOfURLs[i] != NULL) {
        printf("URL is: %s\n", unionOfURLs[i]);
        i++;
    }
    permute(unionOfURLs, 0, numOfDistinctURLs, urlsArray);

    printf("Important answers from here ---------------------------------\n");
    printf("Best footrule is %f\n", currentBestFootRule);
    printf("In the order: \n");
    printArray(currentBestOrder, numOfDistinctURLs);



    return EXIT_SUCCESS;
}

void permute(char **array,int i,int length, char** urlsList[]) { 
  if (length == i){
     printArray(array,length);
     double aFootRule = calculateFootRule(array, length, urlsList);
     if(aFootRule < currentBestFootRule) {
        currentBestFootRule = aFootRule;
        currentBestOrder = copyOrder(array);
     }
     return;
  }
  int j = i;
  for (j = i; j < length; j++) { 
     swap(array[i],array[j]);
     permute(array,i+1,length, urlsList);
     swap(array[i],array[j]);
  }
  return;
}

char** copyOrder(char** anOrder) {
    char** savedOrder = NULL;
    int i = 0;
    while(anOrder[i] != NULL) {
        savedOrder = realloc(savedOrder, (i+1)*sizeof(char*));
        savedOrder[i] = strdup(anOrder[i]);
        i++;
    }
    return savedOrder;
}

void printArray(char ** array, int length) {
    int i = 0;
    printf("printArray: \n");
    while (i < length) {
        printf("%s\n", array[i]);
        i++;

    }
    printf("end printArray\n");
}

void swap(char* a, char* b) {
    char tmp[10000];
    strcpy(tmp, a);
    strcpy(a, b);
    strcpy(b, tmp);
    return;
}

double setToPositive(double number) {
    if (number < 0) return -number;
    return number;
}

double calculateFootRule(char ** anOrder, int lengthOfAnOrder, char** originalOrder[]) {
    double footrule = 0;
    int i = 0;
    while(originalOrder[i] != 0) {
        int j = 0;
        double numOfUrlsInOriginalOrder = numOfUrls(originalOrder[i]);
        while(originalOrder[i][j] != NULL) {
            printf("%f - %f", (double)(j+1)/numOfUrlsInOriginalOrder, (double)(findIndexof(originalOrder[i][j], anOrder)+1)/(double)lengthOfAnOrder);
            printf(" = %f\n", setToPositive( (double)(j+1)/numOfUrlsInOriginalOrder - (double)(findIndexof(originalOrder[i][j], anOrder)+1)/(double)lengthOfAnOrder));
            footrule += setToPositive((double)(j+1)/numOfUrlsInOriginalOrder - (double)(findIndexof(originalOrder[i][j], anOrder)+1)/(double)lengthOfAnOrder);
            printf("current footrule is %f\n", footrule);
            j++;
        }
        i++;
    }
    return footrule;
}

int numOfUrls(char ** array) {
    int i = 0;
    while(array[i] != NULL) i++;
    return i;
}

int findIndexof(char* string, char *array[]) {
    int i = 0;
    while (array[i] != NULL) {
        if(strcmp(array[i], string) == 0) return i;
        i++;
    }  
    return -1;
}

int isInUnion(char* string, char*unionOfURLs[]) {
    int i = 0;
    if (unionOfURLs == NULL) return FALSE;
    while(unionOfURLs[i] != NULL) {
        if(strcmp(unionOfURLs[i], string) == 0) return TRUE;
        i++;
    }
    return FALSE;
}


char** populateArray(FILE* file) {
    char line[1000];
    int i = 0;
    char **urlsList = NULL;
    while(fscanf(file, "%s", line) != EOF) {
        urlsList = realloc(urlsList, (i+1)*sizeof(char*));
        urlsList[i] = strdup(line);
        i++;
    }
    urlsList = realloc(urlsList, (i+1)*sizeof(char*));
    urlsList[i] = NULL;
    return urlsList;
}