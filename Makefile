CC=gcc
CFLAGS=-Wall -lm -std=c11 -gdwarf-2
OBJS=pagerank.o readData.o graph.o
OBJS1=inverted.o readData.o invertedIndex.o
OBJS2=searchPageRank.o readData.o

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

clean :
	rm -f searchPageRank inverted pagerank *.o 
