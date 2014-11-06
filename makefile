CFLAGS = \
		-Wall \
		-g 
		

OBJ= \
		graph.o \
		aig.o \
		cutenumeration.o \
		cutfunction.o \
		vertex.o \
		print.o \
		aggregation.o \
		similarity.o \
		sequential.o \
		verification.o \
		aiger.o

PROF=

all: mainDB

mainRef: $(OBJ) mainRef.o 
	g++ $(CFLAGS) -o xfpgeniusRef $(OBJ) mainRef.o 

mainDB: $(OBJ) mainDB.o 
	g++ $(CFLAGS) -o xfpgeniusDB $(OBJ) mainDB.o $(PROF) #-lboost_graph

graph.o: graph.cpp graph.hpp vertex.o
	g++ $(CFLAGS) -c graph.cpp $(PROF)

graph_b.o: graph_b.cpp graph_b.hpp 
	g++ $(CFLAGS) -c graph_b.cpp $(PROF)

mainRef.o: mainRef.cpp topoDescriptors.hpp verification.hpp $(OBJ)
	g++ $(CFLAGS) -c mainRef.cpp $(PROF) 

mainDB.o: mainDB.cpp topoDescriptors.hpp verification.hpp $(OBJ)
	g++ $(CFLAGS) -c mainDB.cpp $(PROF)

aig.o: aig.cpp aig.hpp graph.o aiger.o
	g++ $(CFLAGS) -c aig.cpp $(PROF)

vertex.o: vertex.cpp vertex.hpp 
	g++ $(CFLAGS) -c vertex.cpp $(PROF)

aggregation.o: aggregation.cpp aggregation.hpp 
	g++ $(CFLAGS) -c aggregation.cpp $(PROF)

similarity.o: similarity.cpp similarity.hpp 
	g++ $(CFLAGS) -c similarity.cpp $(PROF)

verification.o: verification.cpp verification.hpp 
	g++ $(CFLAGS) -c verification.cpp $(PROF)

sequential.o: sequential.cpp sequential.hpp verification.o
	g++ $(CFLAGS) -c sequential.cpp $(PROF)

aiger.o: aiger.c aiger.h 
	gcc $(CFLAGS) -c aiger.c 

print.o: print.cpp print.hpp 
	g++ $(CFLAGS) -c print.cpp

cutenumeration.o: cutenumeration.cpp cutenumeration.hpp aig.o
	g++ $(CFLAGS) -c cutenumeration.cpp $(PROF)

cutfunction.o: cutfunction.cpp cutfunction.hpp aig.o
	g++ $(CFLAGS) -c cutfunction.cpp $(PROF)

test: 
	g++ -o testProgram $(OBJ) test.cpp -lboost_graph

testckt: testckt.cpp $(OBJ)
	g++ -o testckt $(OBJ) testckt.cpp 

aigex: aigex.cpp aig.o aiger.o graph.o vertex.o
	g++ -o aigex cutenumeration.o aig.o aiger.o graph.o vertex.o aigex.cpp 

graphex: graphex.cpp graph.o vertex.o
	g++ -o graphex graph.o vertex.o graphex.cpp

aigABC: mainABC.cpp  graph.o vertex.o aig.o
	g++ -I../abc/src/ -c mainABC.cpp -o mainABC.o
	g++ -o aigABC graph.o vertex.o aig.o aiger.o mainABC.o -L. -labc -lm -ldl -rdynamic -lreadline -lpthread -lncurses -ltermcap -lrt

clean:
	rm -f *.o *fpgenius* *.out* *.in*  cnf testProgram cDatabase aigex testckt

