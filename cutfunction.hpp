/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      cutfunction.hpp
  @      
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @      Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef CUTFUNCTION_GUARD
#define CUTFUNCTION_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include "aig.hpp"
#include "cutenumeration.hpp"
#include "graph.hpp"
#include "vertex.hpp"



//Support up to k = 5
//TRUTH TABLE, MSB=x5
#define x1 0x55555555
#define x2 0x33333333
#define x3 0x0F0F0F0F
#define x4 0x00FF00FF
#define x5 0x0000FFFF


class CutFunction{
	private:
		//Output Truth Table, Circuit Function
		std::map<unsigned long, std::string> m_HashTable;

		//Output Truth Table, count 
		std::map<unsigned long, int> m_FunctionCount;


		//Function, Nodes with the function output
		std::map<unsigned long, std::set<unsigned> > m_NodeFunction;
		//std::map<unsigned long, std::map<unsigned,unsigned> >m_CutInputIndex;


		//Function, List of inputs that have that specific output
		//Output node is stored as the last node in inner vector
		std::map<unsigned long, std::vector<std::vector<unsigned>*> >m_PortMap; 
		std::map<std::string, unsigned int> m_PrimInputSize;


		//Set of functions that define the output, list of input and output of the cut
		std::map<std::set<unsigned long>, std::vector<std::vector<unsigned> > > m_UniqueFunction;


		//AIG Node, Output Tru
		unsigned long* m_Xval;

		CutEnumeration* m_CutEnumeration;
		AIG* m_AIG;

	public:
		//AIG Node, Truth Table Result
		std::map<unsigned int, unsigned long> m_NodeValue;
		std::map<unsigned int, unsigned long> m_NodeValueIn;


		//Constructors
		CutFunction();
		~CutFunction();
		void reset();


		//Boolean Matching
		void preProcessLibrary(std::string);
		void processAIGCuts(bool);
		void processAIGCuts_Perm(bool);
		//void processLUTs(Graph*, std::map<std::string, std::set<unsigned long> >&);
		unsigned long calculate(unsigned);
		unsigned long calculate2(unsigned);
		unsigned int* setPermutation(int);


		//Aggregation
		void aggregation();
		void DFS(unsigned int, std::map<unsigned int, int>&);


		//Getters
		void getPortMap(std::map<unsigned long, std::vector<std::vector<unsigned>*> >& );
		void getHashMap(std::map<unsigned long, std::string>&);
		void setParams(CutEnumeration*, AIG*);
		void getFunctionCount(std::map<unsigned long, int>& );


		//Print Functions
		void printStat();
		void printLibrary();
		void printFunctionCount();
		void printUniqueFunction();
		void printUniqueFunctionStat();
};




#endif
