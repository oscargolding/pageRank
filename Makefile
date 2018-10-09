CC=gcc
CFLAGS=-Wall -lm -std=c11 -gdwarf-2
OBJS=pagerank.o readData.o graph.o

pagerank : $(OBJS)
	$(CC) -o pagerank $(OBJS)

pagerank.o : pagerank.c readData.h 
readData.o : readData.c readData.h graph.h
graph.o : graph.c graph.h readData.h

clean :
	rm -f pagerank *.o 
