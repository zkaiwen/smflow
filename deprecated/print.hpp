/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
 @
 @      print.hpp
 @	
 @      @AUTHOR:Kevin Zeng
 @      Copyright 2012 – 2013 
 @      Virginia Polytechnic Institute and State University
 @
 @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/



#ifndef PRINT_GUARD
#define PRINT_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <set>
#include <list>
#include <map>
#include "cutfunction.hpp"

void printV(std::string, std::vector<int>&);
void printV(std::string, std::vector<int>*);
void print2DV(std::string, std::vector<std::vector<int> >&);
void print2DV(std::string, std::vector<std::vector<int>* >&);

void printIO(std::map<unsigned long long, std::vector<InOut*> >& pmap, std::vector<unsigned long long>& inOut);

void printAddList(std::list<std::set<unsigned> >& addIn, std::list<std::set<unsigned> >& addOut);

void printListSetU(std::list<std::set<unsigned> >&);

void printSetU(std::set<unsigned>&);



#endif
