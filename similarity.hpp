/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  SIMILARITY.hpp
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef SIM_GUARD 
#define SIM_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"


namespace SIMILARITY{

	int findMinDistance(std::map<unsigned, unsigned>& , std::set<unsigned>& , unsigned , std::map<unsigned, unsigned>::iterator& );
	double tanimotoWindow(std::map<unsigned, unsigned>& , std::map<unsigned,unsigned>& );
	double tanimotoWindow_size(std::map<unsigned, unsigned>& , std::map<unsigned,unsigned>& );




	double euclideanDistanceWNorm(std::vector<double>& , std::vector<double>&);
	double euclideanDistance(std::vector<double>& , std::vector<double>&);
	unsigned hammingDistance(unsigned long , unsigned long );
}


#endif
