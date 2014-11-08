/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  AGGREGATION.hpp
	@		--Aggregates Common components into higher structures
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef AGG_GUARD
#define AGG_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include <map>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "cutenumeration.hpp"
#include "cutfunction.hpp"


namespace AGGREGATION{

	/*#############################################################################
	 *
	 * 	 ADDER AGGREGATION 
	 *
	 *#############################################################################*/

	//Searches
	int DFS(AIG* , unsigned , unsigned , std::set<unsigned>& , std::set<unsigned>&, std::set<unsigned>&, std::set<unsigned>&);
	bool DFSearch(AIG* , unsigned , unsigned , unsigned ,  std::set<unsigned>& , std::set<unsigned>& );

	void BFS(AIG* , unsigned , std::set<unsigned>& , std::vector<unsigned>& );


	bool checkContainment_q(AIG* ,unsigned ,unsigned, std::set<unsigned>& ,	std::map<unsigned, std::set<unsigned> >&, std::set<unsigned>& );
	void reduceInputContainment(AIG*, std::set<unsigned>&, std::map<unsigned, std::set<unsigned> >& );

	//Verifications
	bool verify_adder(AIG* , std::set<unsigned>& ,std::set<unsigned>& ,	unsigned );
	bool verify_adder_set(AIG* , 
			std::set<unsigned>& ,
			std::set<unsigned>& ,
			std::set<unsigned>& );
	bool adderAggregation3_verify(AIG* , 
			std::set<unsigned>& ,
			std::list<std::set<unsigned> >& ,
			std::list<std::set<unsigned> >& );


	void simplifyingSet(AIG* , 
			std::vector<InOut*>& , 
			std::set<unsigned>& , 
			std::map<unsigned, std::set<unsigned> >& ,
			std::map<unsigned, std::set<unsigned> >& );

	void simplify_output(AIG* , 
			std::set<unsigned>& , 
			std::map<unsigned, std::set<unsigned> >& ,
			std::map<unsigned, std::set<unsigned> >& );

	void printAddList(std::list<std::set<unsigned> >& ,	std::list<std::set<unsigned> >& );


	void printIO(std::map<unsigned long long, std::vector<InOut*> >& , 
			std::vector<unsigned long long>& );

	void parsePortMap(std::map<unsigned long long, std::vector<InOut*> >& , 
			std::vector<unsigned long long>& ,
			std::vector<InOut*>& );

	void adderAggregation3_heuristic(AIG* , 
			std::map<unsigned , std::set<unsigned> >& outIn, 
			std::set<unsigned> , 
			std::list<std::set<unsigned> >& ,
			std::list<std::set<unsigned> >& );
	void adderAggregation3_cleanup(AIG* , 
			std::set<unsigned> , 
			std::list<std::set<unsigned> >& ,
			std::list<std::set<unsigned> >& );

	void findHAddHeader( AIG* , 
			std::vector<InOut*>& , 
			std::vector<InOut*>& ,
			std::set<unsigned>& ,
			std::map<unsigned , std::set<unsigned> >& , 
			std::list<std::set<unsigned> >& ,
			std::list<std::set<unsigned> >& );
	void combineAdder(
			AIG* ,
			std::list<std::set<unsigned> >& ,
			std::list<std::set<unsigned> >& );
	void findAdder(CutFunction* , CutEnumeration* , AIG* , std::map<unsigned, unsigned>& , std::map<unsigned, unsigned>&, std::map<unsigned, std::set<unsigned> > & );
	void findCarry(CutFunction* , CutEnumeration* , AIG* , std::map<unsigned, unsigned>& , std::map<unsigned, unsigned>&, std::map<unsigned, std::set<unsigned> >& ); 


	/*#############################################################################
	 *
	 * 	 DECODER AGGREGATION 
	 *
	 *#############################################################################*/
	void findDecoder(CutFunction* , AIG* , std::map<unsigned, unsigned>&, std::map<unsigned, std::set<unsigned> > & );


	/*#############################################################################
	 *
	 * 	 MUX AGGREGATION 
	 *
	 *#############################################################################*/
	void findMux2(CutFunction* cf, 
			AIG* aigraph, 
			std::map<unsigned, unsigned>& result2, 
			std::map<unsigned, unsigned>& result3, 
			std::map<unsigned, unsigned>& result4, 
			std::map<unsigned, std::set<unsigned> > &);
	unsigned int findMux_Orig(CutFunction* , AIG* , std::vector<unsigned int>& );
	unsigned int findMux(CutFunction* , AIG* , std::vector<unsigned int>& );

	int findNegInput(AIG* , unsigned , std::set<unsigned>& );
	bool isInputNeg(AIG* , unsigned , unsigned );

	/*#############################################################################
	 *
	 * 	 PARITY AGGREGATION 
	 *
	 *#############################################################################*/
	void findParityTree(CutFunction*, AIG*, std::map<unsigned, unsigned>&, std::map<unsigned, std::set<unsigned> > &);
	void DFS_agg(std::map<unsigned, std::set<unsigned> >& , unsigned , std::set<unsigned>& );

	/*#############################################################################
	 *
	 * 	 GATE FUNCTION AGGREGATION 
	 *
	 *#############################################################################*/
	void findGateFunction(CutFunction* , AIG* ,  std::map<unsigned, unsigned>&, std::map<unsigned, std::set<unsigned> > & );
	void findInputSign(AIG* , unsigned , std::set<unsigned>&, std::list<unsigned>&, std::list<unsigned>& );
	void DFS_gate(AIG*, std::map<unsigned, std::set<unsigned> >& , unsigned , std::set<unsigned>& );
	bool verify_gate(AIG*, std::set<unsigned>&,	unsigned );


	/*#############################################################################
	 *
	 * 	 EQUALITY FUNCTION AGGREGATION 
	 *
	 *#############################################################################*/
	void findEquality(CutFunction* , AIG* ,  std::map<unsigned, unsigned>&, std::map<unsigned, std::set<unsigned> >& );



	bool verifyContainment_DFS(AIG* , unsigned , std::set<unsigned>& , std::set<unsigned>& );
	bool verifyContainment(AIG*, std::set<unsigned>&,	unsigned );
	bool verifyContainment(AIG*, std::set<unsigned>&,	std::set<unsigned>& );

	}


#endif





