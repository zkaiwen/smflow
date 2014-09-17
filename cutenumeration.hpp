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
		std::vector<std::list<std::set<unsigned> > > m_Cuts;
		std::vector<std::list<unsigned> > m_Signatures;
		std::map<unsigned, std::set<unsigned> > m_Cuts2;

		std::map<unsigned, std::list<std::set<unsigned> > > m_SubCut;
		AIG* m_AIGraph;
		unsigned int m_K;


	public:
		//Constructors
		CutEnumeration(AIG*);
		~CutEnumeration();


		//Algorithm methods for cut enumeration
		void findKFeasibleCuts(unsigned int);
		void findKFeasibleCuts_In(unsigned int, std::vector<unsigned int>&);
		void findInputCut(std::vector<unsigned>&);
		void merge(std::list<std::set<unsigned> >&, std::list<std::set<unsigned> >&, std::list<std::set<unsigned> >&);
		unsigned calculateSignature(std::set<unsigned>&);


		//Getter Methods
		void getCuts(unsigned, std::list<std::set<unsigned> >& );
		void getCuts_In(unsigned, std::list<std::set<unsigned> >& );
		void getCut2(std::map<unsigned, std::set<unsigned> >&);
		unsigned int getK();



		//Printing methods
		void print();
		void printStat();
		void printSet(std::set<unsigned>&);
		void printCutSet(unsigned);
		void printCutSet_In(unsigned);
};




#endif
