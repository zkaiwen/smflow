/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  SEQUENTIAL.hpp
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef SEQ_GUARD
#define SEQ_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "verification.hpp"

namespace SEQUENTIAL{

	void getFFList(Graph* , std::list<Vertex*>& );
	void getFFFeedbackList(Graph* , std::list<InOut*>& );
	void deleteFFFeedbackList(std::list<InOut*>& );

	void findRegisterGroup(std::list<Vertex*>& , std::map<unsigned, unsigned>& );

	void findFSM(Graph* );
	unsigned int replaceLUTs(Graph* );

	int DFS(std::list<int>& , Vertex* , Graph* , std::vector<std::vector<int> >& );
	void DFS_FF(std::set<unsigned>& , std::set<unsigned>& , Vertex* );

	//FF Aggregation
	bool checkContainment(std::set<unsigned>& , std::set<unsigned>& );
	unsigned findNumMismatch(std::set<unsigned>& , std::set<unsigned>& , unsigned );

	bool cascade(std::map<unsigned, std::list<InOut*> >::iterator , 
			std::list<InOut* >::iterator, 
			unsigned , std::set<unsigned>&);

	void cascadingFF(Graph*, std::list<InOut*>&, unsigned, std::map<unsigned, unsigned>& );
	void blockFF(Graph*, std::list<InOut*>&,  unsigned, std::map<unsigned, unsigned>& );

	void counterIdentification(Graph*, std::list<InOut*>&,  std::map<unsigned, unsigned>&);

}





#endif





