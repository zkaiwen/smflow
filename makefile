CFLAGS = \
		-Wall \
		-g 
		

OBJ= \
		graph.o \
		graph_b.o \
		aig.o \
		cutenumeration.o \
		cutfunction.o \
		vertex.o \
		aiger.o

PROF=

all: mainRef mainDB

mainRef: $(OBJ) mainRef.o 
	g++ $(CFLAGS) -o xfpgeniusRef $(OBJ) mainRef.o -lboost_graph

mainDB: $(OBJ) mainDB.o 
	g++ $(CFLAGS) -o xfpgeniusDB $(OBJ) mainDB.o $(PROF) -lboost_graph

graph.o: graph.cpp graph.hpp vertex.o
	g++ $(CFLAGS) -c graph.cpp $(PROF)

graph_b.o: graph_b.cpp graph_b.hpp 
	g++ $(CFLAGS) -c graph_b.cpp $(PROF)

mainRef.o: mainRef.cpp sequential.hpp aggregation.hpp topoDescriptors.hpp verification.hpp $(OBJ)
	g++ $(CFLAGS) -c mainRef.cpp $(PROF) 

mainDB.o: mainDB.cpp sequential.hpp aggregation.hpp topoDescriptors.hpp verification.hpp fingerprint.hpp $(OBJ)
	g++ $(CFLAGS) -c mainDB.cpp $(PROF)

aig.o: aig.cpp aig.hpp graph.o
	g++ $(CFLAGS) -c aig.cpp $(PROF)

vertex.o: vertex.cpp vertex.hpp 
	g++ $(CFLAGS) -c vertex.cpp $(PROF)

aiger.o: aiger.c aiger.h 
	gcc $(CFLAGS) -c aiger.c 

cutenumeration.o: cutenumeration.cpp cutenumeration.hpp aig.o
	g++ $(CFLAGS) -c cutenumeration.cpp $(PROF)

cutfunction.o: cutfunction.cpp cutfunction.hpp aig.o
	g++ $(CFLAGS) -c cutfunction.cpp $(PROF)

test: 
	g++ -o testProgram $(OBJ) test.cpp -lboost_graph

clean:
	rm -f *.o *fpgenius* *.out* *.in*  cnf testProgram cDatabase

