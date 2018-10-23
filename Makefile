CC=gcc
CFLAGS=-Wall -std=c11 -gdwarf-2
LDFLAGS=-lm
OBJS=pagerank.o readData.o graph.o
OBJS1=inverted.o readData.o invertedIndex.o
OBJS2=searchPageRank.o readData.o
OBJS3=searchTfIdf.o tfIdf.o
OBJS4=scaledFootrule.o

pagerank : $(OBJS)
	$(CC) -o pagerank $(OBJS)

pagerank.o : pagerank.c readData.h graph.h
readData.o : readData.c readData.h 
graph.o : graph.c graph.h readData.h

inverted: $(OBJS1)
	$(CC) -o inverted $(OBJS1)

inverted.o : inverted.c readData.h invertedIndex.h
readData.o : readData.c readData.h
invertedIndex.o : invertedIndex.c invertedIndex.h

searchPageRank : $(OBJS2)
	$(CC) -o searchPageRank $(OBJS2)

searchPageRank.o : searchPageRank.c readData.h
readData.o : readData.c readData.h

searchTfIdf : $(OBJS3)
	$(CC) -o searchTfIdf $(OBJS3) $(LDFLAGS)

searchTfIdf.o : searchTfIdf.c tfIdf.h
tfIdf.o : tfIdf.c tfIdf.h

scaledFootrule : $(OBJS4)
	$(CC) -o scaledFootrule $(OBJS4) $(LDFLAGS)

scaledFootrule.o : scaledFootrule.h scaledFootrule.c 

clean :
	rm -f searchPageRank inverted searchTfIdf pagerank scaledFootrule *.o 
