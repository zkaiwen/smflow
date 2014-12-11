THIS_DIR = src/main
CXX = g++

CFLAGS = \
		-Wall \
		-g \
		-Wno-unused-function \
		-Wno-write-strings \
		-Wno-sign-compare \
		-Wno-unused-but-set-variable \
		-I../dflow \
		-I../


OBJS =  mainDB.o \
				mainRef.o \
				$

.PHONY: all main clean


#Build the object list
ObjectFileList:$(OBJS)
	@echo $(OBJS:%=$(THIS_DIR)/%) > ObjectFileList

#Make
all: ObjectFileList

%.o: %.cpp 
	$(CXX) $(CFLAGS) -o $@ -c $<


clean:
	rm -f *.o ObjectFileList
