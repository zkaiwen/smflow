/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      cutenumeration.hpp
  @      
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @			Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef CUTENUMERATION_GUARD
#define CUTENUMERATION_GUARD

#include "aig.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <list>
#include <vector>


class CutEnumeration{
	private:
		std::vector<std::list<std::set<unsigned>*>*> m_Cuts;
		AIG* m_AIGraph;
		unsigned int m_K;


	public:
		//Constructors
		CutEnumeration(AIG*);
		~CutEnumeration();


		//Algorithm methods for cut enumeration
		void findKFeasibleCuts(unsigned int);
		std::list<std::set<unsigned>*>* merge(std::list<std::set<unsigned>*>*, std::list<std::set<unsigned>*>*);


		//Getter Methods
		std::list<std::set<unsigned>*>* getCuts(unsigned);



		//Printing methods
		void print();
		void printStat();
		void printSet(std::set<unsigned>*);
		void printCutSet(unsigned);
};




#endif
