/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  topoDescriptor.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef TOPO_GUARD
#define TOPO_GUARD
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "graph_b.hpp"


//Topology Descriptor Functions
namespace TOPOLOGY{

	/*****************************************************************************
	* Weiner Index 
	*  Sum of shortest paths between all pair of vertices 
	*
	* @PARAMS: ckt- circuit to calculate Weiner Index 
	*****************************************************************************/
	unsigned int weinerIndex(Graph* ckt){
		printf("[TOP] -- Calculating Weiner Index\n");
		
		//count
		std::vector<int> pathLengthCount((ckt->getNumVertex()/2)+2, 0);
		std::set<int> startingMarked;

		std::map<int, Vertex*>::iterator it;
		unsigned int maxPath = 0;

		for(it = ckt->begin(); it != ckt->end(); it++){
			
			unsigned int pathLength = 1; 
			startingMarked.insert(it->first);

			//BFS
			std::list<unsigned int> queue;
			std::set<unsigned int> marked;
			queue.push_back(it->first);
			marked.insert(it->first);
			queue.push_back(-1);           //-1 marks where the path length changes

			while(queue.size() != 0){
				int item = queue.front();
				queue.pop_front();

				if(item == -1){
					pathLength++;
					if(queue.size() > 0)
						queue.push_back(-1);
					continue;
				}

				std::vector<Vertex*> outputs;
				ckt->getVertex(item)->getOutput(outputs);
				
				for(unsigned int i = 0; i < outputs.size(); i++){
					if(marked.find(outputs[i]->getID()) == marked.end()){
						marked.insert(outputs[i]->getID());

						//Make sure the BFS doesn't count paths that contain previous starting vertices
						if(startingMarked.find(outputs[i]->getID()) == startingMarked.end()){
							queue.push_back(outputs[i]->getID());
							pathLengthCount[pathLength]++;
						}

					}
				}

				if(pathLength > maxPath) maxPath = pathLength;
			}
			//END BFS
		}


		unsigned int wIndex = 0; 
		for(unsigned int i = 1; i < maxPath+1; i++){
			wIndex += (i*pathLengthCount[i]);		
		}
		printf("[TOP] -- Weiner Index: %d\n", wIndex);
		return wIndex;
	}





}


#endif
