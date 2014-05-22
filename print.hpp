/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
 @
 @      print.hpp
 @	
 @      @AUTHOR:Kevin Zeng
 @      Copyright 2012 – 2013 
 @      Virginia Polytechnic Institute and State University
 @
 @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <list>

void printV(std::string, std::vector<int>&);
void printV(std::string, std::vector<int>*);
void print2DV(std::string, std::vector<std::vector<int> >&);
void print2DV(std::string, std::vector<std::vector<int>* >&);
