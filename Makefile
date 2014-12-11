CXX = g++

CFLAGS = \
		-Wall \
		-g \
		-Wno-unused-function \
		-Wno-write-strings \
		-Wno-sign-compare \
		-Wno-unused-but-set-variable 
		

OBJS= \
	@src/aiger/ObjectFileList \
	@src/dflow/ObjectFileList \
	$

	
	
	

.PHONY: all src dflow espresso

all: espresso smflow_db 

smflow_db: src 
	@echo ""
	@echo "****************************************************"
	@echo "***   Compiling smflow_db"
	make -C src/main
	$(CXX) $(CFLAGS) $(OBJS) src/main/mainDB.o -o smflow_db  
	@echo ""
	@echo "********         Make Complete!        *************"
	@echo ""


src:
	@echo ""
	@echo "****************************************************"
	@echo "***   Compiling Aiger"
	make -C src/aiger -f Makefile_simple
	
	@echo ""
	@echo "****************************************************"
	@echo "***   Compiling dFlow"
	make -C src/dflow

espresso: 
	@echo ""
	@echo "****************************************************"
	@echo "***   Compiling Espresso"
	make -C src/espresso
	@cp -vf src/espresso/src/espresso .

test: 
	g++ -o testProgram $(OBJ) test.cpp -lboost_graph

testckt: testckt.cpp $(OBJ)
	g++ -o testckt $(OBJ) testckt.cpp 

aigex: aigex.cpp aig.o aiger.o graph.o vertex.o
	g++ -o aigex cutenumeration.o aig.o aiger.o graph.o vertex.o aigex.cpp 

graphex: graphex.cpp graph.o vertex.o
	g++ -o graphex graph.o vertex.o graphex.cpp

ABC: mainABC.cpp  graph.o vertex.o aig.o
	g++ -I../abc/src/ -c mainABC.cpp -o mainABC.o
	g++ -o aigABC graph.o vertex.o aig.o aiger.o mainABC.o -L. -labc -lm -ldl -rdynamic -lreadline -lpthread -lncurses -ltermcap -lrt

aigABC: mainABCaig.cpp  graph.o vertex.o aig.o
	g++ -I../programs/abc/src/ -c mainABCaig.cpp -o mainABCaig.o
	g++ -o aigABC graph.o vertex.o aig.o aiger.o mainABCaig.o -L. -laig -lm -ldl -rdynamic -lreadline -lpthread -lncurses -ltermcap -lrt

clean:
	@echo ""
	@echo "****************************************************"
	@echo "***   Cleaning files"
	@echo "****************************************************"
	make -C src/dflow clean
	make -C src/main clean
	make -C src/aiger -f Makefile_simple clean
	make -C src/espresso clean
	make -C src/espresso distclean 
	rm -f *.o *smflow* *.out* *.in*  cnf testProgram cDatabase aigex testckt espresso

