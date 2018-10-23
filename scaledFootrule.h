/* Interface for creating an inverted index */

#ifndef SCALEDFOOTRULE_H
#define SCALEDFOOTRULE_H

typedef struct urlElement* urlNode;
typedef struct listHeader* urlListHeader;
typedef struct fileDescriptorsLL* fileDescriptorNode;
typedef struct fileDescriptorsLLHead* fileDescriptorHead;
typedef struct urlTree* urlTree;

urlListHeader createURLlist(FILE* rankFile);

urlTree insertURL (char* url, urlTree given);

#endif
