/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      print.cpp 
  @	
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @      Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#include "print.hpp"

/***************************************************************************
 *  printV
 *    Prints contents of a vector pointer of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    Vector pointer of ints
 ****************************************************************************/
void printV(std::string name, std::vector<int>* v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v->size(); i++){
		printf("%3d ", v->at(i));
	}
	printf("\n");

}



/***************************************************************************
 *  printV
 *    Prints contents of a vector of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    Vector of ints
 ****************************************************************************/
void printV(std::string name, std::vector<int> & v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v.size(); i++){
		printf("%3d ", v[i]);
	}
	printf("\n");

}



/***************************************************************************
 *  printV
 *    Prints contents of a 2D vector of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    2D Vector of ints
 ****************************************************************************/
void print2DV(std::string name, std::vector<std::vector<int> > & v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v.size(); i++){
		for(unsigned int j = 0; j < v[i].size(); j++){
			printf("%3d ", v[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}



/***************************************************************************
 *  print2DV
 *    Prints contents of a 2D vector pointer of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    2D vector pointer of ints
 ****************************************************************************/
void print2DV(std::string name, std::vector<std::vector<int>* > & v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v.size(); i++){
		for(unsigned int j = 0; j < v[i]->size(); j++){
			printf("%3d ", v[i]->at(j));
		}
		printf("\n");
	}
	printf("\n");
}
