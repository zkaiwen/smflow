THIS_DIR = src/dflow
CXX = g++

CFLAGS = \
		-Wall \
		-g \
		-Wno-unused-function \
		-Wno-write-strings \
		-Wno-sign-compare \
		-Wno-unused-but-set-variable


OBJS =  graph.o \
				aig.o \
				cutenumeration.o \
				cutfunction.o \
				vertex.o \
				aggregation.o \
				similarity.o \
				sequential.o \
				verification.o \
				server.o \
				database.o \
				$

.PHONY: all dflow clean

#Build the object list
ObjectFileList:$(OBJS)
	@echo $(OBJS:%=$(THIS_DIR)/%) > ObjectFileList

#Make
all: ObjectFileList

%.o: %.cpp %.hpp
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o ObjectFileList
