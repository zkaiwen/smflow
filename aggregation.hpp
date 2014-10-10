/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  AGGREGATION.cpp
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
#include "graph_b.hpp"


namespace AGGREGATION{
	CutEnumeration* ce;

	void findHA(CutFunction* , AIG* );
	int findNegInput(AIG* , unsigned , std::vector<unsigned>& );
	bool isInputNeg(AIG* , unsigned , unsigned );
	int DFS(AIG* , unsigned , unsigned , std::set<unsigned>& , std::set<unsigned>&, std::set<unsigned>&, std::set<unsigned>&);
	bool DFSearch(AIG* , unsigned , unsigned , unsigned ,  std::set<unsigned>& , std::set<unsigned>& );
	void printAddList(std::list<std::set<unsigned> >& ,	std::list<std::set<unsigned> >& );
	bool checkContainment(AIG* , unsigned ,	std::set<unsigned>&, std::set<unsigned>& );
	bool checkContainment_q(AIG* ,unsigned ,	std::set<unsigned>& ,	std::map<unsigned, std::vector<unsigned> >&, std::vector<unsigned>& );


	/*#############################################################################
	 *
	 * printIO 
	 *  prints the input output mapping for a list of specific functions 
	 *
	 *#############################################################################*/
	void printIO(std::map<unsigned long long, std::vector<InOut*> >& pmap, 
			std::vector<unsigned long long>& inOut){

		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::set<unsigned>::iterator iSet;
		int count= 0;
		for(unsigned int i = 0; i < inOut.size(); i++){
			iPMAP = pmap.find(inOut[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++){
					printf("INPUT: ");
					for(iSet = iPMAP->second[j]->input.begin(); iSet != iPMAP->second[j]->input.end(); iSet++)
						printf("%d ", *iSet);

					printf("\t\tOUTPUT:\t%d\n", iPMAP->second[j]->output);

					count++;
				}
			}
		}
		printf("COUNT: %d ************************************************************** %d\n", count, count);
	}





	/*#############################################################################
	 *
	 * parsePortMap
	 *   Parses the portMap and stores the input output for the specific functions
	 *
	 *#############################################################################*/
	void parsePortMap(std::map<unsigned long long, std::vector<InOut*> >& pmap, 
			std::vector<unsigned long long>& function ,
			std::vector<InOut*>& inout ){

		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::set<unsigned>::iterator iSet;
		for(unsigned int i = 0; i < function.size(); i++){
			iPMAP = pmap.find(function[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++)
					inout.push_back(iPMAP->second[j]);	
			}
		}
	}





	/*#############################################################################
	 *
	 * simplifyingSet 
	 *  Simplifies a set of functions such that the input is at the lowest level
	 *  possible. 
	 *
	 *#############################################################################*/
	void simplifyingSet(AIG* aig, 
			std::vector<InOut*>& functions, 
			std::set<unsigned>& sumNodes, 
			std::map<unsigned, std::set<unsigned> >& outInMap,
			std::map<unsigned, std::set<unsigned> >& containedComputeTable){

		printf("[AGG] -- SIMPLIFYING SET\n");

		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::list<std::set<unsigned> >::iterator iList1;
		std::map<unsigned, std::set<unsigned> >::iterator iMap;
		std::set<unsigned>::iterator iSet;

		//Go through input output ports of the function
		for(unsigned int j = 0; j < functions.size(); j++){
			unsigned outnode = functions[j]->output;
			/*
			printf("\n---------------------------------------------------------------------------------\n");
			printf("CHECKING PORTMAP OUTNODE: %3d\t\tINSET: ", outnode);
			for(iSet = functions[j]->input.begin(); iSet != functions[j]->input.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
			*/


			//Check to see if the output node has already been mapped
			iMap = outInMap.find(outnode);
			if(iMap == outInMap.end()){
				//If output has not been mapped, make a new outnode and push all the inputs into it
				//printf("New Output node!!!!\n");
				std::set<unsigned> inPort; 
				for(iSet = functions[j]->input.begin(); iSet != functions[j]->input.end(); iSet++)
					inPort.insert(*iSet);

				outInMap[outnode] = inPort;
			}
			else{
				//if output has been mapped, make sure to add the new nodes in and then 
				//perform a containment check
				/*
				printf("Existing Output node!!!! Checking for containment\n");
				printf("InputSet Found: ");
				std::set<unsigned>::iterator iPrint;
				for(iPrint = iMap->second.begin(); iPrint != iMap->second.end(); iPrint++)
					printf("%d ", *iPrint);
				printf("\n");
				*/

				unsigned inputSize = iMap->second.size();
				std::map<unsigned, std::set<unsigned> >::iterator iTable;
				iTable= containedComputeTable.find(outnode);

				//Check to see if containment for the outnode has already been calculated
				if(iTable != containedComputeTable.end()){
					for(iSet = functions[j]->input.begin(); iSet != functions[j]->input.end(); iSet++){
						unsigned innode = *iSet;
						if(iTable->second.find(innode) == iTable->second.end())
							iMap->second.insert(innode);
					}
				}
				else{
					for(iSet = functions[j]->input.begin(); iSet != functions[j]->input.end(); iSet++){
						unsigned innode = *iSet;
						iMap->second.insert(innode);
					}
				}

				//If no new nodes were added
				if(inputSize == iMap->second.size()) continue;

/*
				printf("Combined Set : ");
				for(iPrint = iMap->second.begin(); iPrint != iMap->second.end(); iPrint++)
					printf("%d ", *iPrint);
				printf("\n");
				*/

				//Simplify to see if the inputs are contained
				iSet = iMap->second.begin();
				unsigned prevVal = *iSet;
				iSet++; //Smallest cannot be contained since it goes by levels
				while(iSet != iMap->second.end()){
					//printf("Checking containment: %d......", *iSet);
					std::set<unsigned> marked;
					std::set<unsigned> found;
					int result = DFS(aig, *iSet, *iSet, iMap->second, sumNodes, found,  marked);

					if(result == 1){
						//Add to contained compute table
						containedComputeTable[outnode].insert(*iSet);

						//printf(" -- CONTAINED!\nErasing %d from set\n", *iSet);
						iMap->second.erase(*iSet);
						iSet = iMap->second.find(prevVal);
					}
					//jelse printf("\n");

					prevVal = *iSet;
					iSet++;
				}

				/*
				printf("FINAL Set : ");
				for(iPrint = iMap->second.begin(); iPrint != iMap->second.end(); iPrint++)
					printf("%d ", *iPrint);
				printf("\n");
				*/

			}
		}
	}



	/*#############################################################################
	 *
	 * simplify_output
	 *  Simplifies the output by seeing if an output is contained under another 
	 *
	 *#############################################################################*/
	void simplify_output(AIG* aig, 
			std::set<unsigned>& sumNodes, 
			std::map<unsigned, std::set<unsigned> >& outInMap,
			std::map<unsigned, std::set<unsigned> >& containedComputeTable){
		printf("SIMPLIFYING OUTPUT SET\n");

		std::list<std::set<unsigned> >::iterator iList1;
		std::set<unsigned>::iterator iSet;
		std::map<unsigned , std::set<unsigned> >::reverse_iterator iMap;
		std::map<unsigned , std::set<unsigned> >::reverse_iterator iMap2;
		std::set<unsigned> tobedeleted;

		//Go through each adder function
		for(iMap = outInMap.rbegin(); iMap != outInMap.rend(); iMap++){
			if(tobedeleted.find(iMap->first) != tobedeleted.end()) continue;
			unsigned outnode = iMap->first;
			iMap2 = iMap;
			iMap2++;

			for(; iMap2 != outInMap.rend(); iMap2++){

				if(tobedeleted.find(iMap->first) != tobedeleted.end()) continue;
				//printf("COMPARING OUTNODE: %3d\t%3d\n", outnode, iMap2->first);
				unsigned lb = iMap2->first;

				std::set<unsigned> markedS;
				bool isContain = DFSearch(aig, outnode, iMap2->first, lb, iMap->second,  markedS);
				if(isContain){
					//printf(" * NODE: %d is contained under %d\n", iMap2->first, outnode);

					//Combine the two inputs
					std::map<unsigned, std::set<unsigned> >::iterator iTable;
					iTable= containedComputeTable.find(outnode);
					if(iTable != containedComputeTable.end()){
						for(iSet = iMap2->second.begin(); iSet != iMap2->second.end(); iSet++){
							unsigned innode = *iSet;
							if(iTable->second.find(innode) == iTable->second.end())
								iMap->second.insert(innode);
						}
					}
					else{
						for(iSet = iMap2->second.begin(); iSet != iMap2->second.end(); iSet++){
							unsigned innode = *iSet;
							iMap->second.insert(innode);
						}
					}

/*
					printf("Combined Set : ");
					std::set<unsigned>::iterator iPrint;
					for(iPrint = iMap->second.begin(); iPrint != iMap->second.end(); iPrint++)
						printf("%d ", *iPrint);
					printf("\n");
					*/



					//Simplify to see if the inputs are contained
					iSet = iMap->second.begin();
					unsigned prevVal = *iSet;
					iSet++; //Smallest cannot be contained since it goes by levels
					while(iSet != iMap->second.end()){
						//printf("Checking containment: %d......", *iSet);
						std::set<unsigned> marked;
						std::set<unsigned> found;
						int result = DFS(aig, *iSet, *iSet, iMap->second, sumNodes, found,  marked);

						if(result == 1){
							//Add to contained compute table
							containedComputeTable[outnode].insert(*iSet);

						//	printf(" -- CONTAINED!\nErasing %d from set\n", *iSet);
							iMap->second.erase(*iSet);
							iSet = iMap->second.find(prevVal);
						}
						//else printf("\n");


						prevVal = *iSet;
						iSet++;
					}

					/*
					printf("FINAL Set : ");
					for(iPrint = iMap->second.begin(); iPrint != iMap->second.end(); iPrint++)
						printf("%d ", *iPrint);
					printf("\n");
					*/

					tobedeleted.insert(iMap2->first);
				}
			}
		}

		for(iSet = tobedeleted.begin(); iSet != tobedeleted.end(); iSet++)
			outInMap.erase(*iSet);

	}





	/*#############################################################################
	 *
	 * printAddList 
	 * 	 Prints out the aggregated adder list 
	 *
	 *#############################################################################*/
	void printAddList(std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		std::list<std::set<unsigned> >::iterator iList1;
		std::list<std::set<unsigned> >::iterator iList2;
		iList2 = addOut.begin();
		//printf("\nPRINT ADD LIST\n------------------------------------------------------------------------\n\n");
		for(iList1 = addIn.begin(); iList1 != addIn.end(); iList1++){
			std::set<unsigned>::iterator iSet;
			printf("INPUT: ");
			for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
				printf("%d ", *iSet);


			printf("\nOUTPUT %d: ",(int) iList2->size());
			for(iSet = iList2->begin(); iSet != iList2->end(); iSet++)
				printf("%d ", *iSet);
			printf("\n\n");
			iList2++;

		}
	}



	/*#############################################################################
	 *
	 * DFS 
	 * 	Given start node, make sure it's not the same as lb. 
	 *  Perform DFS to see how many of the input it hits (stored in found)
	 *  that doesn't go past sumnodes as well (boundary)
	 *
	 *  Returns 1 if all it the fanin cone hits all the nodes in found
	 *
	 *#############################################################################*/
	int DFS(AIG* aig, unsigned start, unsigned lb, std::set<unsigned>& input, std::set<unsigned>& sumnodes, std::set<unsigned>& found,  std::set<unsigned>& marked){
		start = start& 0xFFFFFFFE;
		marked.insert(start);
		//printf("Traversing start node: %d...", start);

		if(start <= aig->getInputSize()*2){
			//printf("DFS -- Start is INPUT: %d\n", start);
			return -1;
		}
		else if(sumnodes.find(start) != sumnodes.end()) {
			if(lb != start){
				//printf("SUM NODE FOUND: %d\tINITIAL SUM NODE: %d\n", start, lb);
				return -1;
			}

		}
		//else printf("\n");


		unsigned node1 = aig->getChild1(start) & 0xFFFFFFFE;
		unsigned node2 = aig->getChild2(start) & 0xFFFFFFFE;

		int result1 = 1;
		int result2 = 1;

		//printf("CHILD: %d %d\n", node1, node2);

		if(input.find(node1) != input.end()){
			//printf("HIT INPUT: %d\n", node1);
			found.insert(node1);
		}
		else if(marked.find(node1) == marked.end())
			result1 = DFS(aig, node1, lb, input, sumnodes, found, marked);

		if(input.find(node2) != input.end()){
			//printf("HIT INPUT: %d\n", node2);
			found.insert(node2);
		}
		else if(marked.find(node2) == marked.end())
			result2 = DFS(aig, node2, lb, input, sumnodes, found, marked);


		if(result1 == -1 || result2 == -1) return -1;
		else return 1;
	}

	
	
	
	
	/*#############################################################################
	 *
	 * BFS 
	 * 	Given start node, make sure it's not the same as lb. 
	 *  Perform BFS to see how many of the input it hits (stored in found)
	 *  that doesn't go past sumnodes as well (boundary)
	 *
	 *  Returns 1 if all it the fanin cone hits all the nodes in found
	 *
	 *#############################################################################*/
	unsigned BFS(AIG* aig, unsigned start, std::set<unsigned>& input){
		start = start& 0xFFFFFFFE;
		std::list<unsigned> queue;
		std::set<unsigned> marked;
		marked.insert(start);
		queue.push_back(start);


		std::set<unsigned>::iterator iSet;

		while(!queue.empty()){
			unsigned front = queue.front();
			queue.pop_front();
			iSet = input.find(front);
			if(iSet != input.end())
				if(front != start)
					return *iSet;

			if(front <= aig->getInputSize()*2)
				continue;


			unsigned node1 = aig->getChild1(front) & 0xFFFFFFFE;
			unsigned node2 = aig->getChild2(front) & 0xFFFFFFFE;


			if(marked.find(node1) == marked.end()){
				marked.insert(node1);
				queue.push_back(node1);
			}

			if(marked.find(node2) == marked.end()){
				marked.insert(node2);
				queue.push_back(node2);
			}
		}
		return 0;
	}




	/*#############################################################################
	 *
	 * DFSearch
	 *  Perform DFS to see if it can hit the end node
	 *  that doesn't go past input as well (boundary)
	 *
	 *#############################################################################*/
	bool DFSearch(AIG* aig, unsigned start, unsigned end, unsigned lb, std::set<unsigned>& input,  std::set<unsigned>& marked){
		start = start & 0xFFFFFFFE;
		end = end & 0xFFFFFFFE;
		marked.insert(start);
		//printf("Traversing start node: %d\n", start);

		if(input.find(start) != input.end()) return false;
		if(start <= aig->getInputSize()*2){
			//printf("DFS -- Start is INPUT\n");
			return false;
		}
		if(start == end){
			//printf("DFS -- Start is less than LB\n");
			return true;; //none lower than this
		}
		if(start < lb) return false;

		unsigned node1, node2, child1, child2;
		child1 = aig->getChild1(start);
		child2 = aig->getChild2(start);


		node1 = child1 & 0xFFFFFFFE;
		node2 = child2 & 0xFFFFFFFE;


		if(marked.find(node1) == marked.end()){
			bool result = DFSearch(aig, node1, end, lb, input,  marked);
			if (result) return true;

		}

		if(marked.find(node2) == marked.end()){
			bool result = DFSearch(aig, node2, end, lb, input, marked);
			if (result) return true;
		}

		return false;
	}






	/*#############################################################################
	 *
	 * adderAggregation3_heuristics 
	 *  Attempts to combine all the sumnodes into their respective adders 
	 *  contains debug statements
	 *
	 *#############################################################################*/
	void adderAggregation3_heuristic(AIG* aig, 
			std::map<unsigned , std::set<unsigned> >& outIn, 
			std::set<unsigned> sumNodes, 
			std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		printf("\n\n-------------------------------------------------------------------------------\n");
		printf("ADDER AGGREGATION3   ------\n");

		std::list<std::set<unsigned> >::iterator iListIn;
		std::list<std::set<unsigned> >::iterator iListOut;
		std::list<std::set<unsigned> >::iterator iListInFound;
		std::list<std::set<unsigned> >::iterator iListOutFound;
		std::list<std::set<unsigned> >::iterator deleteIn;
		std::list<std::set<unsigned> >::iterator deleteOut;
		std::map<unsigned , std::set<unsigned> >::iterator iMap;
		std::set<unsigned>::iterator iSet, iSet2;	
		
		std::map<unsigned, std::vector<unsigned> > inputNodeTable;

		std::set<unsigned> curSumList;
		std::set<unsigned> curInputSet;
		iListOut = addOut.begin();
		iListIn = addIn.begin();
		while(iListOut != addOut.end()){
			for(iSet = iListOut->begin(); iSet != iListOut->end(); iSet++)
				curSumList.insert(*iSet);

				curInputSet.insert(*(iListIn->begin()));

			iListIn++;
			iListOut++;
		}
		printf("Current Sum Nodes: ");
			for(iSet = curSumList.begin(); iSet != curSumList.end(); iSet++)
				printf("%d ", *iSet);
				printf("\n");

		printf("Current In Nodes: ");
			for(iSet = curInputSet.begin(); iSet != curInputSet.end(); iSet++)
				printf("%d ", *iSet);
		printf("\n\n");


		//Go through each adder function
		for(iMap = outIn.begin(); iMap != outIn.end(); iMap++){

			unsigned outnode = iMap->first;
			printf("\n\n---------------------------------------");
			printf("[*] Aggregating Output in IMAP:  %d\n", outnode);

			//If no aggregation if found, check if the inputs lead to any of the other inputs in the adder sets
			printf("IMAP: ");
			for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
			//bool isInputContained = false;

			//Look for outnode to the inputs of addlist
			//printf("See if IMAP output can hit all the inputs in adder list\n");
			if(curSumList.find(outnode) != curSumList.end()) {
				printf("OUTPUT NODE ALREADY EXISTS IN THE LIST...SKIPPING\n");
				continue;
			}

			bool containment = false;

			printf("Searching for most likely adder set...");
			unsigned inputHit = BFS(aig, outnode, curInputSet);
			printf("%d\n", inputHit);
			assert(inputHit != 0);

			iListOut = addOut.begin();
			iListIn = addIn.begin();
			std::list<std::list<std::set<unsigned> >::iterator> iteratorListIn;
			std::list<std::list<std::set<unsigned> >::iterator> iteratorListOut;
			std::list<std::list<std::set<unsigned> >::iterator>::iterator iListInIt;
			std::list<std::list<std::set<unsigned> >::iterator>::iterator iListOutIt;

			while(iListIn != addIn.end()){
				if(iListIn->find(inputHit) != iListIn->end()){
					iteratorListIn.push_back(iListIn);
					iteratorListOut.push_back(iListOut);
				}
				iListIn++;
				iListOut++;
			}

			iListInIt = iteratorListIn.begin();
			iListOutIt = iteratorListOut.begin();

			while(iListInIt != iteratorListIn.end()){
				iListIn = *iListInIt;
				iListOut = *iListOutIt;

				//There exists an output that is the same as the input
				if(iListOut->find(outnode)!=iListOut->end()) {
					continue;
				}


				printf("[*] Begin Adder list aggregation...\n");
				printf("\nAdderLIST: ");
				for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++)
					printf("%d ", *iSet);
				printf("\n");


				//Check input containment****************************************************************
				unsigned int nummatch = 0;
				for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++){
					if(iListIn->find(*iSet) != iListIn->end())
						nummatch++;
				}
				printf("NUMMATCH: %d\tIMAPSIZE: %d\tADDERSIZE: %d\n", nummatch,(int) iMap->second.size(), (int)iListIn->size());
				if(nummatch == iMap->second.size() || nummatch == iListIn->size()){
					printf("INPUT CONTAINMENT FOUND\n");
					containment = true;
					break;

				}
				std::set<unsigned> marked;
				std::set<unsigned> found;
				int result = DFS(aig, outnode, outnode, *iListIn, sumNodes, found,  marked);

				printf("found (INPUTS that IMAP output hit from adder list): RESULT: %d\n * ", result);
				for(iSet = found.begin(); iSet != found.end(); iSet++){
					printf("%d ", *iSet);
				}
				printf("\n");


				//TEST CASE FOR THIS BLOCK IS adder4_ci
				if(found.size() != iListIn->size() && result != 1){
					//Nodes that are not found by DFS:
					printf("LOOKING FOR NODES THAT ARE NOT CVERED\n");
					for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++){
						if(found.find(*iSet) == found.end()){
							printf("NODE NOT COVERED: %d\n", *iSet);
							std::set<unsigned> marked2;
							std::set<unsigned> found2;
							int localresult = DFS(aig, *iSet, *iSet, iMap->second, sumNodes, found2,  marked2);
							if(localresult == 1){
								printf(" -- Node is contained in IMAP's inputs\n");
								result = 1;
							}
							else{
								result = -2;
								printf("NOT CONTAINED...therefore not possible...skip\n");
								break;
							}
						}
					}
				}
				if(found.size() == iListIn->size() || result == 1) {
					printf("MATCH FOUND!!!\n");
					containment = true;
					break;
				}
				iListInIt++;
				iListOutIt++;
			}


			if(containment == true){
				iListOut->insert(outnode);
				for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++)
					iListIn->insert(*iSet);
				printf("new adder list: ");
				for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++)
					printf("%d ", *iSet);
				printf("\n");

				//Simplify to see if the inputs are contained
				iSet = iListIn->begin();
				unsigned prevVal = *iSet;
				iSet++; //Smallest cannot be contained since it goes by levels
				std::vector<unsigned> inputNodes;
				printf("checking for containment\n");
				while(iSet != iListIn->end()){
					if(inputNodeTable.find(*iSet) != inputNodeTable.end()){
						printf("INPUTNODE TABLE HIT!&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
						iListIn->erase(*iSet);
						for(unsigned int i = 0; i < inputNodeTable[*iSet].size(); i++)
							iListIn->insert(inputNodeTable[*iSet][i]);
						iSet = iListIn->find(prevVal);
					}
					else if(checkContainment_q(aig, *iSet, *iListIn, inputNodeTable, inputNodes)){
						iListIn->erase(iSet);
						iSet = iListIn->find(prevVal);
						exit(1);
					}

					prevVal = *iSet;
					iSet++;
				}

				printf("sim adder list: ");
				for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++)
					printf("%d ", *iSet);
				printf("\n");
			}

			else if(containment == false ){
				printf("NEW ENTRY IN ADDER LIST\n");
				addIn.push_back(iMap->second);
				std::set<unsigned> outSet;
				//outSet.insert(-1);
				outSet.insert(outnode);
				addOut.push_back(outSet);
			}
			printf("addlist: \n");
			printAddList(addIn, addOut);
		}
	}
	
	
	
	
	
	/*#############################################################################
	 *
	 * adderAggregation3_cleanup
	 *  Attempts to combine all the sumnodes into their respective adders 
	 *  contains debug statements
	 *
	 *#############################################################################*/
	void adderAggregation3_cleanup(AIG* aig, 
			std::set<unsigned> sumNodes, 
			std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		printf("\n\n-------------------------------------------------------------------------------\n");
		printf("ADDER AGGREGATION3  CLEANUP MODE ------\n");

		std::list<std::set<unsigned> >::iterator iListIn;
		std::list<std::set<unsigned> >::iterator iListOut;
		std::list<std::set<unsigned> >::iterator iListInSearch;
		std::list<std::set<unsigned> >::iterator iListOutSearch;
		std::list<std::set<unsigned> >::iterator iListInFound;
		std::list<std::set<unsigned> >::iterator iListOutFound;
		std::list<std::set<unsigned> >::iterator deleteIn;
		std::list<std::set<unsigned> >::iterator deleteOut;
		std::set<unsigned>::iterator iSet, iSet2;	

		std::set<unsigned> curSumList;
		std::set<unsigned> curInputSet;
		iListOut = addOut.begin();
		iListIn = addIn.begin();
		while(iListOut != addOut.end()){
			curInputSet.insert(*(iListIn->begin()));
			iListIn++;
			iListOut++;
		}

		printf("Current In Nodes: ");
			for(iSet = curInputSet.begin(); iSet != curInputSet.end(); iSet++)
				printf("%d ", *iSet);
		printf("\n\n");


		//Go through each adder function
		iListInSearch = addIn.begin();
		iListOutSearch = addOut.begin();
		while(iListInSearch != addIn.end()){
			unsigned outnode = *(iListOutSearch->begin());
			printf("\n\n---------------------------------------");
			printf("[*] Aggregating Output in IMAP:  %d\n", outnode);

			//If no aggregation if found, check if the inputs lead to any of the other inputs in the adder sets
			printf("PRIMARY ADDER ");
			for(iSet = iListInSearch->begin(); iSet != iListInSearch->end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");

			bool containment = false;

			printf("Searching for most likely adder set...");
			unsigned inputHit = BFS(aig, outnode, curInputSet);
			printf("%d\n", inputHit);
			assert(inputHit != 0);

			iListOut = addOut.begin();
			iListIn = addIn.begin();
			std::list<std::list<std::set<unsigned> >::iterator> iteratorListIn;
			std::list<std::list<std::set<unsigned> >::iterator> iteratorListOut;
			std::list<std::list<std::set<unsigned> >::iterator>::iterator iListInIt;
			std::list<std::list<std::set<unsigned> >::iterator>::iterator iListOutIt;

			while(iListIn != addIn.end()){
				if(iListIn->find(inputHit) != iListIn->end()){
					iteratorListIn.push_back(iListIn);
					iteratorListOut.push_back(iListOut);
				}
				iListIn++;
				iListOut++;
			}

			iListInIt = iteratorListIn.begin();
			iListOutIt = iteratorListOut.begin();

			while(iListInIt != iteratorListIn.end()){
				iListIn = *iListInIt;
				iListOut = *iListOutIt;

				//There exists an output that is the same as the input
				if(iListOut->find(outnode)!=iListOut->end()) {
					iListInIt++;
					iListOutIt++;
					continue;
				}


				printf("[*] Begin Adder list aggregation...\n");
				printf("\nAdderLIST: ");
				for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++)
					printf("%d ", *iSet);
				printf("\n");


				//Check input containment****************************************************************
				unsigned int nummatch = 0;
				for(iSet = iListInSearch->begin(); iSet != iListInSearch->end(); iSet++){
					if(iListIn->find(*iSet) != iListIn->end())
						nummatch++;
				}
				printf("NUMMATCH: %d\tIMAPSIZE: %d\tADDERSIZE: %d\n", nummatch,(int) iListInSearch->size(), (int)iListIn->size());
				if(nummatch == iListInSearch->size() || nummatch == iListIn->size()){
					printf("INPUT CONTAINMENT FOUND\n");
					containment = true;
					break;

				}
				std::set<unsigned> marked;
				std::set<unsigned> found;
			
				int result = DFS(aig, outnode, outnode, *iListIn, sumNodes, found,  marked);

				printf("found (INPUTS that IMAP output hit from adder list): RESULT: %d\n * ", result);
				for(iSet = found.begin(); iSet != found.end(); iSet++){
					printf("%d ", *iSet);
				}
				printf("\n");


				//TEST CASE FOR THIS BLOCK IS adder4_ci
				if(found.size() != iListIn->size() && result != 1){
					//Nodes that are not found by DFS:
					//printf("LOOKING FOR NODES THAT ARE NOT CVERED\n");
					for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++){
						if(found.find(*iSet) == found.end()){
							//printf("NODE NOT COVERED: %d\n", *iSet);
							std::set<unsigned> marked2;
							std::set<unsigned> found2;
							int localresult = DFS(aig, *iSet, *iSet, *iListInSearch, sumNodes, found2,  marked2);
							if(localresult == 1){
								//printf(" -- Node is contained in IMAP's inputs\n");
								result = 1;
							}
							else{
								result = -2;
								//printf("NOT CONTAINED...therefore not possible...skip\n");
								break;
							}
						}
					}
				}
				if(found.size() == iListIn->size() || result == 1) {
					containment = true;
					break;
				}
				iListInIt++;
				iListOutIt++;
			}


			if(containment == true){
				for(iSet = iListOut->begin(); iSet != iListOut->end(); iSet++)
					iListOutSearch->insert(*iSet);

				for(iSet = iListIn->begin(); iSet != iListIn->end(); iSet++)
					iListInSearch->insert(*iSet);

				printf("new adder list: ");
				for(iSet = iListInSearch->begin(); iSet != iListInSearch->end(); iSet++)
					printf("%d ", *iSet);
				printf("\n");

				//Simplify to see if the inputs are contained
				iSet = iListInSearch->begin();
				unsigned prevVal = *iSet;
				iSet++; //Smallest cannot be contained since it goes by levels
				while(iSet != iListInSearch->end()){
					if(checkContainment(aig, *iSet, *iListInSearch, curInputSet)){ //Pass curinputSet to reduce as well
						iSet = iListInSearch->find(prevVal);
						exit(1);
					}
					prevVal = *iSet;
					iSet++;
				}

				printf("sim adder list: ");
				for(iSet = iListInSearch->begin(); iSet != iListInSearch->end(); iSet++)
					printf("%d ", *iSet);
				printf("\n");
				addOut.erase(iListOut);
				addIn.erase(iListIn);
				
			}
			else{
				printf("NO POSSIBLE AGGREGATION...\n");
			}

			printf("addlist: \n");
			printAddList(addIn, addOut);
		
			iListInSearch++;
			iListOutSearch++;
		}
	}


	/*#############################################################################
	 *
	 * adderAggregation3 
	 *  Attempts to combine all the sumnodes into their respective adders 
	 *
	 *#############################################################################*/
	void adderAggregation3(AIG* aig, 
			std::map<unsigned , std::set<unsigned> >& outIn, 
			std::set<unsigned> sumNodes, 
			std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		printf("\n\n-------------------------------------------------------------------------------\n");
		printf("ADDER AGGREGATION3   ------\n");

		std::list<std::set<unsigned> >::iterator iList1;
		std::list<std::set<unsigned> >::iterator iList2;
		std::list<std::set<unsigned> >::iterator iList1Found;
		std::list<std::set<unsigned> >::iterator iList2Found;
		std::list<std::set<unsigned> >::iterator deleteIn;
		std::list<std::set<unsigned> >::iterator deleteOut;
		std::map<unsigned , std::set<unsigned> >::iterator iMap;
		std::set<unsigned>::iterator iSet, iSet2;	

		std::set<unsigned> curSumList;
		iList1 = addOut.begin();
		while(iList1 != addOut.end()){
			for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
				curSumList.insert(*iSet);
			iList1++;
		}


		//Go through each adder function
		for(iMap = outIn.begin(); iMap != outIn.end(); iMap++){
			unsigned outnode = iMap->first;
			bool containment = false;

			//Look for outnode to the inputs of addlist
			iList2 = addOut.begin();
			iList1 = addIn.begin();


			bool outFound = false;

			//Check to see if the outnode is already part of a sum adder
			//Find and keep track of the adder it is already a part of.
			//Need to see if the sum node is part of any other input set
			if(curSumList.find(outnode) != curSumList.end()){
				outFound = true;
				iList2Found= iList2;
				iList1Found= iList1;

				//Set delete out to the index of to be deleted
				int numadder = 1;
				while(iList2Found != addOut.end()){
					if(iList2Found->find(outnode) != iList2Found->end())		break;
					iList1Found++;
					iList2Found++;
					numadder++;
				}
			}


			while(iList1 != addIn.end()){
				//There exists an output that is the same as the input
				if(iList2->find(outnode)!=iList2->end()) {
					iList1++;
					iList2++;
					continue;
				}

				//Check input containment****************************************************************
				unsigned int nummatch = 0;
				for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++){
					if(iList1->find(*iSet) != iList1->end())
						nummatch++;
				}
				if(nummatch == iMap->second.size() || nummatch == iList1->size()){
					containment = true;
					break;
				}

				std::set<unsigned> marked;
				std::set<unsigned> found;
				int result = DFS(aig, outnode, outnode, *iList1, sumNodes, found,  marked);

				//TEST CASE FOR THIS BLOCK IS adder4_ci
				if(found.size() != iList1->size() && result != 1){
					//Nodes that are not found by DFS:
					for(iSet = iList1->begin(); iSet != iList1->end(); iSet++){
						if(found.find(*iSet) == found.end()){
							std::set<unsigned> marked2;
							std::set<unsigned> found2;
							int localresult = DFS(aig, *iSet, *iSet, iMap->second, sumNodes, found2,  marked2);
							if(localresult == 1)	result = 1;
							else{
								result = -2;
								break;
							}
						}
					}
				}

				if(found.size() == iList1->size() || result == 1) {
					containment = true;
					break;	
				}

				iList2++;
				iList1++;
			}

			if(containment == true){
				if(!outFound){
					iList2->insert(outnode);
					for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++)
						iList1->insert(*iSet);
				}
				else{
					for(iSet = iList2Found->begin(); iSet != iList2Found->end(); iSet++)
						iList2->insert(*iSet);

					for(iSet = iList1Found->begin(); iSet != iList1Found->end(); iSet++)
						iList1->insert(*iSet);

					addIn.erase(iList1Found);
					addOut.erase(iList2Found);

				}

				//Simplify to see if the inputs are contained
				iSet = iList1->begin();
				unsigned prevVal = *iSet;
				iSet++; //Smallest cannot be contained since it goes by levels
				while(iSet != iList1->end()){
					if(checkContainment(aig, *iSet, *iList1, sumNodes)){
						iSet = iList1->find(prevVal);
					}
					prevVal = *iSet;
					iSet++;
				}

			}

			else if(containment == false && outFound == false){
				addIn.push_back(iMap->second);
				std::set<unsigned> outSet;
				outSet.insert(outnode);
				addOut.push_back(outSet);
			}
		}
	}




	/*#############################################################################
	 *
	 * checkContainment_q
	 * 	if there is a node in setlist that is contained, lower the upper bound 
	 *
	 *#############################################################################*/
	bool checkContainment_q(
			AIG* aig, 
			unsigned start,
			std::set<unsigned>& setList,
			std::map<unsigned, std::vector<unsigned> >& inputNodeTable,
			std::vector<unsigned> & inputNodes 
			){
		printf("Checking containment: %d......\n", start);
		if(start <= aig->getInputSize() * 2)
			return false;
		


			//printf(" -- Node hits a existing!\nErasing %d from set\n", start);

			unsigned node1, node2;
			node1 = aig->getChild1((start) & 0xFFFFFFFE) & 0xFFFFFFFE;
			node2= aig->getChild2((start) & 0xFFFFFFFE) & 0xFFFFFFFE;
			//printf(" * * Adding children nodes: %d %d\n", node1, node2);
		if(setList.find(node1) != setList.end()){
			inputNodes.push_back(node1);
			return true;
		}
		if(setList.find(node2) != setList.end()){
			inputNodes.push_back(node1);
			return true;
		}

			if(checkContainment_q(aig, node1, setList, inputNodeTable, inputNodes )){
								
				inputNodeTable[start].push_back(node2);
				std::map<unsigned, std::vector<unsigned> >::iterator iMap = inputNodeTable.find(start);
				for(unsigned int i = 0; i < inputNodes.size(); i++)
					iMap->second.push_back(inputNodes[i]);

				inputNodes.push_back(node2);
				return true;

			}
		
			else if(checkContainment_q(aig, node2, setList, inputNodeTable, inputNodes)){
				inputNodeTable[start].push_back(node1);
				std::map<unsigned, std::vector<unsigned> >::iterator iMap = inputNodeTable.find(start);
				for(unsigned int i = 0; i < inputNodes.size(); i++)
					iMap->second.push_back(inputNodes[i]);

				inputNodes.push_back(node1);
				return true;
			}

		return false;
	}





	/*#############################################################################
	 *
	 * checkContainment 
	 * 	if there is a node in setlist that is contained, lower the upper bound 
	 *
	 *#############################################################################*/
	bool checkContainment(
			AIG* aig, 
			unsigned start,
			std::set<unsigned>& setList,
			std::set<unsigned>& sumNodes 
			){

		std::set<unsigned> marked;
		std::set<unsigned> found;
		std::set<unsigned> dummy;
		//printf("Checking containment: %d......", start);
		DFS(aig, start, start, setList, dummy, found, marked);

		if(found.size() != 0){
			//printf(" -- Node hits a existing!\nErasing %d from set\n", start);
			setList.erase(start);
			sumNodes.erase(start);

			unsigned node1, node2;
			node1 = aig->getChild1((start) & 0xFFFFFFFE) & 0xFFFFFFFE;
			node2= aig->getChild2((start) & 0xFFFFFFFE) & 0xFFFFFFFE;
			//printf(" * * Adding children nodes: %d %d\n", node1, node2);

			if(!checkContainment(aig, node1, setList, sumNodes )){
				setList.insert(node1);
				sumNodes.insert(node1);

			}
			if(!checkContainment(aig, node2, setList, sumNodes)){
				setList.insert(node2);
				sumNodes.insert(node2);
			}

			return true;
		}
		//else printf("\n");
		return false;
	}







	void findFAddHeader(std::vector<InOut*>& add1,
			std::vector<InOut*>& add2, 
			std::vector<InOut*>& add3,
			std::set<unsigned>& sumNodes,
			std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		std::set<unsigned>::iterator iSet1;
		std::set<unsigned>::iterator iSet2;
		std::set<unsigned>::iterator iSet3;

		//Go through each adder function
		for(unsigned int i = 0; i < add3.size(); i++){
			for(unsigned int q = 0; q < add2.size(); q++){
				//Check to see if all the nodes in SUM2 can be found in the SUM3
				int numMatch = 0;
				for(iSet2 = add2[q]->input.begin(); iSet2 != add2[q]->input.end(); iSet2++){
					if(add3[i]->input.find(*iSet2) != add3[i]->input.end())
						numMatch++;
					else{
						numMatch = -1;
						break;
					}
				}

				if((unsigned)numMatch == add2[q]->input.size()) {
					//make sure a initial can be found
					for(unsigned int z = 0; z < add1.size(); z++){
						int numMatch2to1= 0;
						for(iSet1 = add1[z]->input.begin(); iSet1 != add1[z]->input.end(); iSet1++){
							if(add2[q]->input.find(*iSet1) != add2[q]->input.end())
								numMatch2to1++;
							else{
								numMatch2to1 = -1;
								break;
							}
						}

						if((unsigned)numMatch2to1 == add1[z]->input.size()) {
		/*					printf("\n************************************\n");
							printf("SUM3 OUT: %d MATCHES SUM2 OUT: %d MATCHES %d. COMBINE THEM\n", add3[i]->output, add2[q]->output, add1[z]->output);
							*/
							std::set<unsigned> inputCopy = add3[i]->input;
							std::set<unsigned> outputCopy;
							outputCopy.insert(add3[i]->output);
							outputCopy.insert(add2[q]->output);
							outputCopy.insert(add1[z]->output);

							sumNodes.insert(add3[i]->output);
							sumNodes.insert(add2[q]->output);
							sumNodes.insert(add1[z]->output);

							addIn.push_back(inputCopy);
							addOut.push_back(outputCopy);

							break;
						}

					}
				}
			}
		}
		//printAddList(addIn, addOut);
	}




	/*#############################################################################
	 *
	 *  findHAddHeader 
	 * 	  Searches for the possible beginnings of adders by looking for
	 *    sum, sum+1 nodes to make sure the inputs of one is contained in the other
	 *
	 *#############################################################################*/
	void findHAddHeader( AIG* aig, 
			std::vector<InOut*>& add2, 
			std::vector<InOut*>& add3,
			std::set<unsigned>& sumNodes,
			std::map<unsigned , std::set<unsigned> >& outIn, 
			std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		std::set<unsigned>::iterator iSet2;
		std::set<unsigned>::iterator iSet3;
		std::set<unsigned>::iterator iSet;
		std::map<unsigned , std::set<unsigned> >::iterator iMap;
		std::set<unsigned> ignoreOutputs;

		//Go through each adder function
		for(unsigned int i = 0; i < add3.size(); i++){
			//if(ignoreOutputs.find(add3[i]->output) != ignoreOutputs.end()) continue; 
			iMap = outIn.find(add3[i]->output);
			if(iMap == outIn.end()) continue;

			for(unsigned int q = 0; q < add2.size(); q++){
				//if(ignoreOutputs.find(add2[2]->output) != ignoreOutputs.end()) continue; 
				if(outIn.find(add2[q]->output) == outIn.end()) continue;

				//Check to see if all the nodes in SUM2 can be found in the SUM3
				int numMatch = 0;
				for(iSet2 = add2[q]->input.begin(); iSet2 != add2[q]->input.end(); iSet2++){
					if(add3[i]->input.find(*iSet2) != add3[i]->input.end())
						numMatch++;
					else{
						numMatch = -1;
						break;
					}
				}

				if((unsigned)numMatch == add2[q]->input.size()) {
					//printf("\n************************************\nSUM 3 MATCHES SUM 2\n");

					std::set<unsigned> inputCopy = add3[i]->input;
					for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++)
						inputCopy.insert(*iSet);


					//Simplify to see if the inputs are contained
					iSet = inputCopy.begin();
					unsigned prevVal = *iSet;
					iSet++; //Smallest cannot be contained since it goes by levels
					while(iSet != inputCopy.end()){
						//printf("Checking containment: %d......", *iSet);
						std::set<unsigned> marked;
						std::set<unsigned> found;
						int result = DFS(aig, *iSet, *iSet, iMap->second, sumNodes, found,  marked);

						if(result == 1){
							//Add to contained compute table
							//printf(" -- CONTAINED!\nErasing %d from set\n", *iSet);
							inputCopy.erase(*iSet);
							iSet = inputCopy.find(prevVal);
						}
						//else printf("\n");

						prevVal = *iSet;
						iSet++;
					}


					std::set<unsigned> outputCopy;
					outputCopy.insert(add3[i]->output);
					outputCopy.insert(add2[q]->output);

					sumNodes.insert(add3[i]->output);
					sumNodes.insert(add2[q]->output);

					addIn.push_back(inputCopy);
					addOut.push_back(outputCopy);

					break;
				}
			}
		}

		//printAddList(addIn, addOut);

	}






	/*#############################################################################
	 *
	 *  combineAdder 
	 * 	  Combines the adders if they share a common output node 
	 *
	 *#############################################################################*/
	void combineAdder(
			AIG* aig,
			std::list<std::set<unsigned> >& addIn,
			std::list<std::set<unsigned> >& addOut){

		std::list<std::set<unsigned> >::iterator iListIn1 ;
		std::list<std::set<unsigned> >::iterator iListOut1 ;
		std::list<std::set<unsigned> >::iterator iListIn2 ;
		std::list<std::set<unsigned> >::iterator iListOut2 ;
		std::list<std::set<unsigned> >::iterator iPrevIn;
		std::list<std::set<unsigned> >::iterator iPrevOut;
		std::set<unsigned>::iterator iSet;

		iListOut1 = addOut.begin();
		iListIn1 = addIn.begin();
		int matchLimit = iListOut1->size() - 1;


		while(iListOut1 != addOut.end()){
			iListOut2 = iListOut1;
			iListOut2++;
			iListIn2 = iListIn1;
			iListIn2++;

			iPrevIn = iListIn1;
			iPrevOut= iListOut1;

			while(iListOut2 != addOut.end()){
				int numMatch = 0;
				/*
					 printf("COMPARING OUT SET1: ");
					 for(iSet = iListOut1->begin(); iSet != iListOut1->end(); iSet++)
					 printf("%d ", *iSet);
					 printf("\t\tCOMPARING OUT SET2: ");
					 for(iSet = iListOut2->begin(); iSet != iListOut2->end(); iSet++)
					 printf("%d ", *iSet);
					 printf("\n");
				 */

				for(iSet = iListOut2->begin(); iSet != iListOut2->end(); iSet++)
					if(iListOut1->find(*iSet) != iListOut1->end())
						numMatch++;


				if(numMatch >= matchLimit){
					//printf(" * COMBINE!\n");
					for(iSet = iListIn2->begin(); iSet != iListIn2->end(); iSet++)
						iListIn1->insert(*iSet);
					for(iSet = iListOut2->begin(); iSet != iListOut2->end(); iSet++)
						iListOut1->insert(*iSet);


					//Simplify to see if the inputs are contained
					iSet = iListIn1->begin();
					unsigned prevVal = *iSet;
					iSet++; //Smallest cannot be contained since it goes by levels
					while(iSet != iListIn1->end()){
						//printf("Checking containment: %d......", *iSet);
						std::set<unsigned> marked;
						std::set<unsigned> found;
						std::set<unsigned> dummy;
						int result = DFS(aig, *iSet, *iSet, *iListIn1, dummy, found,  marked);

						if(result == 1){
							//printf(" -- CONTAINED!\nErasing %d from set\n", *iSet);
							iListIn1->erase(*iSet);
							iSet = iListIn1->find(prevVal);
						}
						//else printf("\n");

						prevVal = *iSet;
						iSet++;
					}
					/*
						 printf("FINAL Set : ");
						 for(iSet= iListIn1->begin(); iSet!= iListIn1->end(); iSet++)
						 printf("%d ", *iSet);
						 printf("\n");
					 */




					addOut.erase(iListOut2);
					addIn.erase(iListIn2);
					iListOut2 = iPrevOut;
					iListIn2 = iPrevIn;
				}
				else{
					iPrevIn = iListIn2;
					iPrevOut= iListOut2;
				}

				iListIn2++;
				iListOut2++;
			}
			iListIn1++;
			iListOut1++;

		}

	}





	/*#############################################################################
	 *
	 *  extractOutputs 
	 * 	  Stores all the outputs of a given function into a set 
	 *
	 *#############################################################################*/
	void extractOutputs(std::map<unsigned long long, std::vector<InOut*> >& pmap, 
			std::vector<unsigned long long>& inOut, 
			std::set<unsigned>& out){
		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;

		//Go through each adder function
		for(unsigned int i = 0; i < inOut.size(); i++){
			iPMAP = pmap.find(inOut[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++){
					unsigned outnode = iPMAP->second[j]->output;
					out.insert(outnode);
				}
			}
		}
	}






	/*#############################################################################
	 *
	 *  findAdder 
	 * 	  Main block to search for and see if an adder exists or not 
	 *
	 *#############################################################################*/
	void findAdder(CutFunction* cf, CutEnumeration* cut, AIG* aigraph, std::map<unsigned, unsigned>& resultA, std::map<unsigned, unsigned>& resultC){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR ADDERS -------------------------------\n");
		ce = cut;

		//Get the hashmap of circuit names to function tt
		std::map<unsigned long long, std::string>::iterator it;
		std::map<unsigned long long, std::string> hmap;
		cf->getHashMap(hmap);

		//Find all adder bitslices 
		std::vector<unsigned long long> xorFunction;
		std::vector<unsigned long long> andFunction;
		std::vector<unsigned long long> faCarry_f;
		std::vector<unsigned long long> faSum_f;
		std::vector<unsigned long long> faCarry2_f;
		std::vector<unsigned long long> faSum2_f;
		std::vector<unsigned long long> faSum3_f;
		std::vector<unsigned long long> haCarry2_f;
		std::vector<unsigned long long> haSum2_f;
		std::vector<unsigned long long> haCarry3_f;
		std::vector<unsigned long long> haSum3_f;
		std::vector<unsigned long long> cla3_f;
		printf(" * Parsing function database for half adder components...\n");

		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("xor") != std::string::npos)
				xorFunction.push_back(it->first);
			else if(it->second.find("faSum3bit") != std::string::npos){
				faSum3_f.push_back(it->first);
				//TODO: May need to check if the A3XORB3 input function is 
				//      actually an XOR function
			}
			else if(it->second.find("faSum2bit") != std::string::npos)
				faSum2_f.push_back(it->first);
			else if(it->second.find("faSum") != std::string::npos)
				faSum_f.push_back(it->first);
			else if(it->second.find("faCarry2bit") != std::string::npos)
				faCarry2_f.push_back(it->first);
			else if(it->second.find("faCarry") != std::string::npos)
				faCarry_f.push_back(it->first);
			else if(it->second.find("haSum3") != std::string::npos)
				haSum3_f.push_back(it->first);
			else if(it->second.find("haSum2") != std::string::npos)
				haSum2_f.push_back(it->first);
			else if(it->second.find("haCarry3") != std::string::npos)
				haCarry3_f.push_back(it->first);
			else if(it->second.find("haCarry2") != std::string::npos)
				haCarry2_f.push_back(it->first);
			else if(it->second.find("cla_gp3") != std::string::npos)
				cla3_f.push_back(it->first);
		}



		//Get the map for function tt and every set of input and output with that function
		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long long, std::vector<InOut*> > pmap;
		std::map<unsigned long long, std::vector<InOut*> > pmap_dc;
		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		cf->getPortMap(pmap);
		cf->getPortMap_DC(pmap_dc);
/*
		printf("XOR FUNCTIONS: \n");
		printIO(pmap, xorFunction);

		//		printf("FACARRY FUNCTIONS: \n");
		//		printIO(pmap, faCarry);
		printf("\n\nFASUM FUNCTIONS: \n");
		printIO(pmap, faSum_f);
		printf("\n\nFASUM2 FUNCTIONS: \n");
		printIO(pmap, faSum2_f);
		printf("\n\nFASUM3 FUNCTIONS: \n");
		printIO(pmap, faSum3_f);
		//		printf("\n\nFACARRY2 FUNCTIONS: \n");
		//		printIO(pmap, faCarry2);
		printf("\n\nHASUM2 FUNCTIONS: \n");
		printIO(pmap, haSum2_f);
		printf("\n\nHACARRY2 FUNCTIONS: \n");
		printIO(pmap, haCarry2_f);
		printf("\n\nHASUM3 FUNCTIONS: \n");
		printIO(pmap, haSum3_f);
		printf("\n\nHACARRY3 FUNCTIONS: \n");
		printIO(pmap, haCarry3_f);

		//printf("\n\nCLA3 FUNCTIONS: \n");
		//printIO(pmap, cla3);
		*/

		/*
			 printf("\n\nFASUM2 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, faSum2);
			 printf("\n\nFASUM3 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, faSum3);
			 printf("\n\nHASUM2 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, haSum2);
			 printf("\n\nHASUM3 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, haSum3);
		 */
		//printf("XOR FUNCTIONS___DC: \n");
		//printIO(pmap_dc, xorFunction);
		/*

			 printf("FACARRY FUNCTIONS___DC: \n");
			 printIO(pmap_dc, faCarry);
			 printf("\n\nFASUM FUNCTIONS___DC: \n");
			 printIO(pmap_dc, faSum);
			 printf("\n\nFACARRY2 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, faCarry2);
			 printf("\n\nHACARRY2 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, haCarry2);
			 printf("\n\nHACARRY3 FUNCTIONS___DC: \n");
			 printIO(pmap_dc, haCarry3);
		//printf("\n\nCLA3 FUNCTIONS___DC: \n");
		//printIO(pmap_dc, cla3);
		 */


		//Extract the output of the possible sums found
		/*
			 extractOutputs(pmap_dc, haSum3, sumNodes);
			 extractOutputs(pmap_dc, haSum2, sumNodes);
			 extractOutputs(pmap_dc, faSum3, sumNodes);
			 extractOutputs(pmap_dc, faSum2, sumNodes);
		 */

		//Aggregation
		std::list<std::set<unsigned> > faddOutputList;
		std::list<std::set<unsigned> > faddInputList;
		std::list<std::set<unsigned> > haddOutputList;
		std::list<std::set<unsigned> > haddInputList;
		/*
			 std::list<std::set<unsigned> > addOutputList;
			 std::list<std::set<unsigned> > addInputList;
			 std::list<std::string> addTypeList;
		 */
		std::list<std::set<unsigned> > dcOutputList;
		std::list<std::set<unsigned> > dcInputList;
		std::list<std::set<unsigned> >::iterator iList1;
		std::list<std::set<unsigned> >::iterator iList2;
		std::list<std::set<unsigned> >::iterator iList3;
		std::list<std::set<unsigned> >::iterator iList4;

		std::list<std::set<unsigned> >::iterator ifain;
		std::list<std::set<unsigned> >::iterator ifaout;
		std::list<std::set<unsigned> >::iterator ihain;
		std::list<std::set<unsigned> >::iterator ihaout;
		std::set<unsigned>::iterator iSet;

		std::set<unsigned>::iterator isfain;
		std::set<unsigned>::iterator isfaout;
		std::set<unsigned>::iterator ishain;
		std::set<unsigned>::iterator ishaout;

		std::set<unsigned> sumNodes;

		std::vector<InOut*> faCarry;
		std::vector<InOut*> faSum;
		std::vector<InOut*> faCarry2;
		std::vector<InOut*> faSum2;
		std::vector<InOut*> faSum3;
		std::vector<InOut*> haCarry2;
		std::vector<InOut*> haSum2;
		std::vector<InOut*> haEmpty;
		std::vector<InOut*> haSum3;

		parsePortMap(pmap, faSum_f, faSum);
		parsePortMap(pmap, faSum2_f, faSum2);
		parsePortMap(pmap, faSum3_f, faSum3);
		parsePortMap(pmap, haSum2_f, haSum2);
		parsePortMap(pmap, haSum3_f, haSum3);




		/*
			 extractOutputs(pmap, faSum, sumNodes);
			 extractOutputs(pmap, faSum2, sumNodes);
			 extractOutputs(pmap, haSum3, sumNodes);
			 extractOutputs(pmap, haSum2, sumNodes);
		 */


		std::map<unsigned, std::set<unsigned> > outIn;
		std::map<unsigned, std::set<unsigned> > containedComputeTable;
		//simplifyingSet(aigraph, pmap, faSum3, outIn, containedComputeTable);
		simplifyingSet(aigraph, faSum, sumNodes, outIn, containedComputeTable);
		simplifyingSet(aigraph, faSum2, sumNodes, outIn, containedComputeTable);
		simplifyingSet(aigraph, faSum3, sumNodes, outIn, containedComputeTable);
		simplifyingSet(aigraph, haSum3, sumNodes, outIn, containedComputeTable);
		simplifyingSet(aigraph, haSum2, sumNodes, outIn, containedComputeTable);


		std::map<unsigned, std::set<unsigned> >::iterator iOutin;
		/*
		printf("COMPRESSED IN OUT PORT FOR POSSIBLE SUM NODES:\n"); 
		for(iOutin = outIn.begin(); iOutin != outIn.end(); iOutin++){
			printf("OUTPUT NODE: %3d\t\tINPUT NODES: ", iOutin->first);
			for(iSet = iOutin->second.begin(); iSet != iOutin->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}
		*/


		simplify_output(aigraph, sumNodes, outIn, containedComputeTable);
		//printf("\n\nCOMPRESSED IN OUT PORT AFTER OUTPUT SIMPLIFICAITON\n"); 
		for(iOutin = outIn.begin(); iOutin != outIn.end(); iOutin++){
			/*
			printf("OUTPUT NODE: %3d\t\tINPUT NODES: ", iOutin->first);
			for(iSet = iOutin->second.begin(); iSet != iOutin->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
			*/
			sumNodes.insert(iOutin->first);
		}

/*
		printf(" *-* SUMNODES: ");
		for(iSet = sumNodes.begin(); iSet != sumNodes.end(); iSet++)
			printf("%d ", *iSet);
		printf("\n");
		*/







		//printf("\n\nFINDING ADD HEADER FOR FULL ADDER\n");
		findHAddHeader(aigraph, faSum, faSum2, sumNodes, outIn, faddInputList, faddOutputList);
		printf("\n\nFINDING ADD HEADER FOR HALF ADDER\n");
		findHAddHeader(aigraph, haSum2, haSum3, sumNodes, outIn, haddInputList, haddOutputList);

/*
		printf("FADD CARRY Aggregation\n");
		printAddList(faddInputList, faddOutputList);
		printf("HADD CARRY Aggregation\n");
		printAddList(haddInputList, haddOutputList);
		*/


		combineAdder(aigraph, haddInputList, haddOutputList);
		//combineAdder(aigraph, faddInputList, faddOutputList);

		printf("HADD CARRY AggregationAFTER COMBING #################################\n");
		printAddList(haddInputList, haddOutputList);



		/*
			 printf("FAsum3 AGG\n");
			 adderAggregation2(aigraph, pmap, faSum3, sumNodes, addInputList, addOutputList);

			 printf("HAsum3 AGG\n");
			 adderAggregation2(aigraph, pmap, haSum3, sumNodes, addInputList, addOutputList);

			 printf("FAsum2 AGG\n");
			 adderAggregation2(aigraph, pmap, faSum2, sumNodes, addInputList, addOutputList);

			 printf("HAsum2 AGG\n");
			 adderAggregation2(aigraph, pmap, haSum2, sumNodes, addInputList, addOutputList);
			 std::set<unsigned> inputNodes;
			 ihain= haddInputList.begin(); 
			 while(ihain!= haddInputList.end()){
			 for(ishain = ihain->begin(); ishain!= ihain->end(); ishain++)
			 sumNodes.insert(*ishain);
			 ihain++;
			 }
		 */


/*
		printf(" *-* SUMNODES: ");
		for(iSet = sumNodes.begin(); iSet != sumNodes.end(); iSet++)
			printf("%d ", *iSet);
		printf("\n");
		*/
		adderAggregation3_heuristic(aigraph, outIn, sumNodes,  haddInputList, haddOutputList);
		adderAggregation3_cleanup(aigraph, sumNodes, haddInputList, haddOutputList);
		/*
			 ifain= faddInputList.begin(); 
			 while(ifain!= faddInputList.end()){
			 for(isfain = ifain->begin(); isfain!= ifain->end(); isfain++)
			 sumNodes.insert(*isfain);
			 ifain++;
			 }
		 */


/*
		printf("FADD CARRY AggregationAFTER COMBING #################################\n");
		printAddList(faddInputList, faddOutputList);
*/	

/*
		printf(" *-* SUMNODES: ");
		for(iSet = sumNodes.begin(); iSet != sumNodes.end(); iSet++)
			printf("%d ", *iSet);
		printf("\n");
		*/

		//quickAdderAggregation(outIn, sumNodes, faddInputList, faddOutputList);
		//adderAggregation3_heuristic(aigraph, outIn, sumNodes,  faddInputList, faddOutputList);
		//adderAggregation3(aigraph, outIn, sumNodes, addInputList2, addOutputList2);

		/*
			 printf("FAsum3 AGG\n");
			 adderAggregation2(aigraph, pmap_dc, faSum3, sumNodes, dcInputList, dcOutputList);

			 printf("HAsum3 AGG\n");
			 adderAggregation2(aigraph, pmap_dc, haSum3, sumNodes, dcInputList, dcOutputList);

			 printf("FAsum2 AGG\n");
			 adderAggregation2(aigraph, pmap_dc, faSum2, sumNodes, dcInputList, dcOutputList);

			 printf("HAsum2 AGG\n");
			 adderAggregation2(aigraph, pmap_dc, haSum2, sumNodes, dcInputList, dcOutputList);
		 */

		/*


		//Chcek to see if any of the sets are contained within each other
		iList1 = addInputList.begin();
		while(iList1 != addInputList.end()){
		iList2 = iList1;
		iList2++;
		while(iList2 != addInputList.end()){
		std::set<unsigned>::iterator iSet;
		bool similarSet = false;
		for(iSet = iList2->begin(); iSet != iList2->end(); iSet++){
		if(iList1->find(*iSet) != iList1->end()){
		similarSet = true;
		break;
		}
		}

		if(similarSet){
		for(iSet = iList2->begin(); iSet != iList2->end(); iSet++)
		iList1->insert(*iSet);

		iList2 = addInputList.erase(iList2);
		addOutputList.pop_back();
		}
		else iList2++;
		}
		iList1++;
		}
		 */


		printf("\n####################################################################\n");
		//printf("FADD CARRY Aggregation\n");
		///printAddList(faddInputList, faddOutputList);
		printf("HADD CARRY Aggregation\n");
		printAddList(haddInputList, haddOutputList);
		printf("####################################################################\n");


		/*
			 printf("\n\nDETERMINING WHICH ADDER IS HALF AND FULL\n");
			 ifain= faddInputList.begin(); 
			 ifaout= faddOutputList.begin(); 
			 while(ifain!= faddInputList.end()){
			 ihain= haddInputList.begin(); 
			 ihaout= haddOutputList.begin(); 
			 while(ihain!= haddInputList.end()){
			 for(ishaout = ihaout->begin(); ishaout!= ihaout->end(); ishaout++){
		//Check to see if there are same output adders
		if(ifaout->find(*ishaout) != ifaout->end()){
		printf("FOUND SIMILAR OUTPUT PORT\n");
		//CHeck to see if there are 4 input nodes that match
		//input of full adder
		int nummatch = 0; 
		for(isfain = ifain->begin(); isfain != ifain->end(); isfain++){
		if(ihain->find(*isfain) != ihain->end())
		nummatch++;
		}

		//if less than 4, then the adder is full adder
		if(nummatch < 4){
		printf("FA ADDER\n");
		addOutputList.push_back(*ifaout);
		addInputList.push_back(*ifain);
		addTypeList.push_back("FA");
		}
		else{
		printf("HA ADDER\n");
		addOutputList.push_back(*ihaout);
		addInputList.push_back(*ihain);
		addTypeList.push_back("HA");
		}
		break;
		}
		}
		ihain++;
		ihaout++;
		}
		ifain++;
		ifaout++;
		}
		 */

		/*

			 printf("HASUM3 AGG____DC\n");
			 adderAggregation(pmap_dc, haSum3, addInputList, addOutputList);

			 printf("HASUM2 AGG____DC\n");
			 adderAggregation(pmap_dc, haSum2, addInputList, addOutputList);

			 printf("FASum3 AGG____DC\n");
			 adderAggregation(pmap_dc, faSum3, addInputList, addOutputList);

			 printf("FASum2 AGG____DC\n");
			 adderAggregation(pmap_dc, faSum2, addInputList, addOutputList);
		 */


		printf("%d adders found\n", (int)faddInputList.size() + 1);
		iList2 = faddOutputList.begin();
		for(iList1 = faddInputList.begin(); iList1 != faddInputList.end(); iList1++){
			printf("ADDER SIZE: %d\t\t", (int)iList2->size());
			if(iList2->size() <= 1){
				iList2++;
				continue;
			}

			std::set<unsigned>::iterator iSet;
			printf("INPUT: ");
			for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
				printf("%d ", *iSet);

			printf("\n\nOUTPUT: ");
			for(iSet = iList2->begin(); iSet != iList2->end(); iSet++)
				printf("%d ", *iSet);
			printf("\n------------------------------------------------------------------------\n\n");

			//Check for output containment
			std::vector<unsigned> toBeDeleted;
			std::set<unsigned>::iterator iSet2;

			//TODO: OPTIMIZE AND STORE THE TOBEDELETED
			/*
				 printf(" * Checking for containment...\n");
				 std::set<unsigned> intermediate;
				 for(iSet = iList2->begin(); iSet != iList2->end(); iSet++){
				 for(iSet2 = iList2->begin(); iSet2 != iList2->end(); iSet2++){
				 if(*iSet == 0xFFFFFFFF || *iSet2 == 0xFFFFFFFF) continue;
				 else if(*iSet == *iSet2) continue;
				 std::set<unsigned> marked;
				 int isContained = DFS(aigraph, *iSet, *iSet2, *iList1, intermediate,  marked);
			//printf("OUTPUT %d is contained under %d: RESULT: %d\n", *iSet2, *iSet, isContained);
			if(isContained == 1) toBeDeleted.push_back(*iSet2);

			}
			}

			//Delete contained output nodes from output list
			for(unsigned int i = 0; i < toBeDeleted.size(); i++){
			iList2->erase(toBeDeleted[i]);
			}
			 */

			if(resultA.find(iList2->size()) == resultA.end())
				resultA[iList2->size()] = 1;
			else
				resultA[iList2->size()]++;

			iList2++;
		}

		/*

			 printf("%d DC ADDERS found\n", (int)dcInputList.size() + 1);
			 iList2 = dcOutputList.begin();
			 for(iList1 = dcInputList.begin(); iList1 != dcInputList.end(); iList1++){

			 printf("DC SIZE: %d\t\t", (int)iList2->size());
			 if(iList2->size() <= 1){
			 iList2++;
			 continue;
			 }

			 std::set<unsigned>::iterator iSet;
			 printf("INPUT: ");
			 for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
			 printf("%d ", *iSet);


			 printf("\t\tOUTPUT: ");
			 for(iSet = iList2->begin(); iSet != iList2->end(); iSet++)
			 printf("%d ", *iSet);
			 printf("\n");

			 if(resultC.find(iList2->size()) == resultC.end()){
			 resultC[iList2->size()] = 1;
			 }
			 else
			 resultC[iList2->size()]++;

			 iList2++;

			 }
		 */

		printf("%d DC ADDERS found\n", (int)haddInputList.size() + 1);
		iList2 = haddOutputList.begin();
		for(iList1 = haddInputList.begin(); iList1 != haddInputList.end(); iList1++){

			printf("DC SIZE: %d\t\t", (int)iList2->size());
			if(iList2->size() <= 1){
				iList2++;
				continue;
			}

			std::set<unsigned>::iterator iSet;
			printf("INPUT: ");
			for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
				printf("%d ", *iSet);


			printf("\t\tOUTPUT: ");
			for(iSet = iList2->begin(); iSet != iList2->end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");

			if(resultC.find(iList2->size()) == resultC.end()){
				resultC[iList2->size()] = 1;
			}
			else
				resultC[iList2->size()]++;

			iList2++;

		}

		ce = NULL;
	}















	void findDecoder(CutFunction* cf, AIG* aigraph, std::map<unsigned, unsigned>& result){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR DECODERS-------------------------------");

		//Get the hashmap of circuit names to function tt
		std::map<unsigned long long, std::string>::iterator it;
		std::map<unsigned long long, std::string> hmap;
		cf->getHashMap(hmap);

		//Get the map for function tt and every set of input and output with that function
		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long long, std::vector<InOut*> > pmap;
		cf->getPortMap(pmap);

		std::vector<unsigned long long> andInput;
		//possible inputs for decoder, possible outputs for decoder 
		std::map<std::vector<unsigned>, std::vector<unsigned> > agg;
		std::map<std::vector<unsigned>, std::vector<unsigned> >::iterator iAgg;

		//Final Result of Aggregation
		std::list<std::vector<unsigned> > possibleDecoderIn;
		std::list<std::vector<unsigned> > possibleDecoderOut;

		//Find instances of or gates
		printf(" * Parsing function database for decoder blocks...\n");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			//or and and have the same basic layout. Or is just inverted at in and out
			if(it->second.find("or") != std::string::npos && it->second.find("xor") == std::string::npos){
				andInput.push_back(it->first);
			}
		}

		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::set<unsigned>::iterator iSet;

		//Aggregate based on inputs of the and gates
		for(unsigned int i = 0; i < andInput.size(); i++){
			iPMAP = pmap.find(andInput[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++){
					std::vector<unsigned> ports;

					for(iSet = iPMAP->second[j]->input.begin(); iSet != iPMAP->second[j]->input.end(); iSet++){
						unsigned in = *iSet;
						ports.push_back(in);
					}

					agg[ports].push_back(iPMAP->second[j]->output);
				}
			}
		}




		for(iAgg = agg.begin(); iAgg!=agg.end(); iAgg++){
			bool isDecoder = false;
			int outputSize = iAgg->second.size();
			int inputSize= iAgg->first.size();
			int expectedOutputSize = 1 << inputSize;
			int expectedOutputSizeWithEnable = 1 << (inputSize -1);


			std::list<int>::iterator iList; 

			//If the output is a power of two to the inputsize, then it is a decoder
			if(outputSize == expectedOutputSize)
				isDecoder = true;

			//Check to see if a possible enable bit is introduced
			else if(expectedOutputSizeWithEnable == outputSize){
				//Skip 1-bit decoder with enable
				if(outputSize == 2) continue;

				//Check to see if at least one of the inputs is never negated
				for(unsigned int i = 0; i < iAgg->first.size(); i++){
					bool isAInputNeg = false;
					for(unsigned int k = 0; k < iAgg->second.size(); k++){
						if(isInputNeg(aigraph, iAgg->second[k], iAgg->first[i])){
							isAInputNeg = true;;
							break;
						}
					}

					if(isAInputNeg){
						isDecoder = true;
						break;
					}
				}
			}

			if(isDecoder){
				possibleDecoderIn.push_back(iAgg->first);
				possibleDecoderOut.push_back(iAgg->second);
			}
		}

		agg.clear();


		std::list<std::vector<unsigned> >::iterator iPDI;
		std::list<std::vector<unsigned> >::iterator iPDI2;
		std::list<std::vector<unsigned> >::iterator iPDO;
		std::list<std::vector<unsigned> >::iterator iPDO2;

		/*
			 printf("\n\n * POSSIBLE DECODERS: \n");
			 printf("IN: \n");
			 for(iPDI= possibleDecoderIn.begin(); iPDI!=possibleDecoderIn.end(); iPDI++){
			 for(unsigned int i = 0; i < iPDI->size(); i++)
			 printf("%d ", iPDI->at(i));
			 printf("\n");

			 }
			 printf("OUT: \n");
			 for(iPDO= possibleDecoderOut.begin(); iPDO!=possibleDecoderOut.end(); iPDO++){
			 for(unsigned int i = 0; i < iPDO->size(); i++)
			 printf("%d ", iPDO->at(i));
			 printf("\n");
			 }
			 printf("\n");
		 */


		printf(" * REMOVING REDUNDANCIES\n");
		iPDI = possibleDecoderIn.begin();
		iPDO = possibleDecoderOut.begin();

		while(iPDI != possibleDecoderIn.end()){
			iPDI2 = iPDI;
			iPDI2++;
			iPDO2 = iPDO;
			iPDO2++;

			bool next = true;
			while(iPDI2 != possibleDecoderIn.end()){
				//TODO: Assume if same size, cannot be contained
				if(iPDI2->size() == iPDI->size()){
					iPDI2++;
					iPDO2++;
					continue;
				}

				unsigned int numMatch = 0; 
				for(unsigned int i = 0; i < iPDI->size(); i++){
					for(unsigned int k = 0; k < iPDI2->size(); k++){
						if(iPDI->at(i) == iPDI2->at(k)){
							numMatch++;	
							break;
						}
					}
				}

				if(numMatch == iPDI->size()){
					iPDI = possibleDecoderIn.erase(iPDI);
					iPDO = possibleDecoderOut.erase(iPDO);
					next = false;
					break;
				}
				else if(numMatch == iPDI2->size()){
					iPDI2= possibleDecoderIn.erase(iPDI2);
					iPDO2 = possibleDecoderOut.erase(iPDO2);
				}
				else{
					iPDI2++;
					iPDO2++;
				}
			}
			if(next){
				iPDI++;
				iPDO++;
			}
		}


		//	printf("\n\n * POSSIBLE DECODERS: \n");
		iPDI = possibleDecoderIn.begin();
		iPDO = possibleDecoderOut.begin();

		while(possibleDecoderIn.size() > 0){
			//size, count
			if(result.find(iPDO->size()) == result.end()){
				result[iPDO->size()] = 1;
			}
			else
				result[iPDO->size()]++;
			/*
				 printf("IN: ");
				 for(unsigned int i = 0; i < iPDI->size(); i++)
				 printf("%d ", iPDI->at(i));
				 printf("\t\tOUT:");
				 for(unsigned int i = 0; i < iPDO->size(); i++)
				 printf("%d ", iPDO->at(i));
				 printf("\n");
			 */

			iPDI = possibleDecoderIn.erase(iPDI);
			iPDO = possibleDecoderOut.erase(iPDO);
		}







		/*
			 printf("\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			 printf("\n----------------------------------------------------------------\n");
			 printf("Result Search:\n");
			 printf("----------------------------------------------------------------\n");
			 printf("----------------------------------------------------------------\n");
			 std::list<unsigned> queue;
			 queue.push_back(2496);
			 queue.push_back(2480);
			 queue.push_back(2488);
			 queue.push_back(2472);
			 queue.push_back(2492);
			 queue.push_back(2476);
			 queue.push_back(2484);
			 queue.push_back(2468);
			 queue.push_back(2400);
			 queue.push_back(2392);
			 queue.push_back(2388);
			 queue.push_back(2398);
			 queue.push_back(2390);
			 queue.push_back(2394);
			 queue.push_back(2384);
			 queue.push_back(2396);
			 std::set<unsigned> input;
			 input.insert(254);
			 input.insert(256);
			 input.insert(258);
			 input.insert(2196);
			 aigraph->printSubgraph(queue, input);
		 */

	}





















	/*
		 Works for traditional 4-1 amd 2-1 muxes
		 Needs more work for 3-1 and those abve 4-1
	 */

	void findMux2(CutFunction* cf, AIG* aigraph, std::map<unsigned,std::map<unsigned, unsigned> >& sizeCountMap){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING MUXES METHOD 2-------------------------------\n");

		//Get the hashmap of circuit names to function tt
		std::map<unsigned long long, std::string>::iterator it;
		std::map<unsigned long long, std::string> hmap;
		cf->getHashMap(hmap);

		//Get the map for function tt and every set of input and output with that function
		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long long, std::vector<InOut*> > pmap;
		cf->getPortMap(pmap);

		//Find all or gate function tt
		std::vector<unsigned long long> orFunction;
		printf(" * Parsing function database for mux components...");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("or") != std::string::npos && it->second.find("xor") == std::string::npos){
				orFunction.push_back(it->first);
			}
		}

		//Store input and output of orgate functions found
		std::list<std::vector<unsigned> > orIn;
		std::list<std::vector<unsigned> >::iterator orinit;
		std::list<std::vector<unsigned> >::iterator orinit2;
		std::list<unsigned> orOut;
		std::list<unsigned>::iterator  oroutit;
		std::list<unsigned>::iterator  oroutit2;
		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::set<unsigned>::iterator iSet;
		for(unsigned int i = 0; i < orFunction.size(); i++){
			iPMAP = pmap.find(orFunction[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++){
					//printf("\nINPUT: ");
					std::vector<unsigned> ports;
					for(iSet = iPMAP->second[j]->input.begin(); iSet != iPMAP->second[j]->input.end(); iSet++){
						//	printf("%d ", pmap[orFunction[i]][j]->at(k));
						ports.push_back(*iSet);
					}
					//printf("\nOUTPUT:\t%d", pmap[orFunction[i]][j]->at(k));

					//If input is not contained in an already found, store. 
					unsigned outnode = iPMAP->second[j]->output;
					orIn.push_back(ports);
					orOut.push_back(outnode);
				}
			}
		}
		printf("done\n");




		//Verify inputs are negated
		printf(" * Verifying negated inputs of OR...");
		orinit = orIn.begin();
		oroutit = orOut.begin();

		while(orinit != orIn.end()){
			unsigned int numNeg = findNegInput(aigraph, *oroutit, *orinit);
			//printf("NUM NEGATIVE INPUT: %d\t insize: %d\n", numNeg, (*orinit).size());
			//printf("IN %d OUT %d\n", orIn.size(), orOut.size());
			if((*orinit).size() != numNeg){
				orinit = orIn.erase(orinit);
				oroutit = orOut.erase(oroutit);
			}
			else{
				orinit++;
				oroutit++;
			}
		}
		printf("done\n");

		/*
			 printf("\n\nPossible Mux or gates\n");	
			 orinit = orIn.begin();
			 oroutit = orOut.begin();

			 while(orinit != orIn.end()){
			 printf("INPUT: ");
			 for(unsigned int i = 0; i < orinit->size(); i++){
			 printf("%d ", orinit->at(i));
			 }
			 printf("\t\tOutput: %d\n", *oroutit);

			 orinit++;
			 oroutit++;
			 }
		 */



		//Remove or gates contained within another. 
		printf(" * Searching for common signals\n");

		//Mux size, selectbits, count
		std::map<int, std::map<std::set<int>, int> >stats;
		std::map<int, std::map<std::set<int>, int> >::iterator iStats;

		//Orinit iterator, vector of select bits
		std::map<std::vector<unsigned> , std::set<int> > selectBits;
		unsigned int aigInputLimit = (2 * aigraph->getInputSize()) + 1;
		orinit = orIn.begin();
		oroutit = orOut.begin();


		//BFS
		while(orinit != orIn.end()){
			std::vector<std::list<unsigned int> > queue;
			queue.reserve(orinit->size());

			//Node ID, Level of node
			std::map<unsigned int, unsigned int> levelMap;

			//Prepare a queue for each input
			bool conflict = false;
			for(unsigned int i = 0; i < orinit->size(); i++){
				unsigned int signal = orinit->at(i);

				/*
					 Makes sure input to or gate does not go into 
					 other inputs of the or gate
				 */
				if(signal > aigInputLimit){

					//Check if other inputs are connected to it
					unsigned int c1 = aigraph->getChild1(signal);
					unsigned int c2 = aigraph->getChild2(signal);
					unsigned int node1 = c1 & 0xFFFFFFFE;
					unsigned int node2 = c2 & 0xFFFFFFFE;


					for(unsigned int k = 0; k < orinit->size(); k++){
						if(orinit->at(k) == node1 || orinit->at(k) == node2){
							//printf("CONFLICT\n");
							conflict = true;
							break;
						}
					}
					if(conflict) break;
				}

				std::list<unsigned int> startqueue;
				startqueue.push_back(signal);
				//printf("START: %d INDEX: %d\n", orinit->at(i), i);

				queue.push_back(startqueue);
				levelMap[orinit->at(i)] = 0;
			}

			if(conflict){
				/*printf("CONFLICT REMOVAL\n");
					for(unsigned int i = 0; i < orinit->size(); i++)
					printf("%d ", orinit->at(i));
					printf("\t\tOutput: %d\n", *oroutit);
				 */

				orinit = orIn.erase(orinit);
				oroutit = orOut.erase(oroutit);
				continue;
			}


			//Check to see if there are neg and pos select bit signals
			//Make sure the selectbits have pos and neg signals
			//BB - LSB is 1 if there is a pos sig, MSB is 1 if ther eis a neg sig
			//signal, BB
			std::map<unsigned int, std::set<int> > selectNegMap;

			//Node id, count
			std::map<unsigned int, std::set<unsigned int> >sameSignalCount;
			bool isQueueEmpty= false;
			unsigned int numSameSignal = 0;
			unsigned int mask = 1;
			while(mask < orinit->size()){
				numSameSignal++;
				mask = mask << 1;
			}

			//printf("\nAIG SAME SIGNAL: %d\n", numSameSignal);
			//printf("Number of Inputs: %d\n", orinit->size());
			std::set<int> selectBit;

			//Go at most 3 or 2 levels deep depending on size of or gate;
			bool isLevelLimit = false;


			while(!isQueueEmpty && !isLevelLimit){
				/*
					 if(orinit->size() != 4) break;
					 printf("\n\nINPUT: ");
					 for(unsigned int i = 0; i < orinit->size(); i++){
					 printf("%d ", orinit->at(i));
					 }
					 printf("\t\tOutput: %d\tNUMSAMESIGNAL: %d\n", *oroutit, numSameSignal);
				 */

				isQueueEmpty = true;

				//Loop through each BFS
				for(unsigned int i = 0; i < orinit->size(); i++){

					//Check to see if queue is empty
					if(queue[i].empty())	continue;
					isQueueEmpty = false;


					//Pop the queue	
					unsigned int origSignal = queue[i].front();
					unsigned int signal = origSignal & 0xFFFFFFFE;
					queue[i].pop_front();


					//Check to see if the levelLimit is reached
					//printf("SIGNAL POPPED: %d INDEX %d LEVEL: %d\n", signal, i, levelMap[signal]);
					if(levelMap[signal] > numSameSignal){
						isLevelLimit = true;
						break;
					}


					//Check selectNegList
					if(origSignal & 0x1)  selectNegMap[signal].insert(1);
					else	                selectNegMap[signal].insert(0);


					//Increase signal count
					if(sameSignalCount.find(signal) != sameSignalCount.end()){
						sameSignalCount[signal].insert(i);

						//Check to see if the number of same signals are as expected
						if(sameSignalCount[signal].size() == orinit->size()){
							//printf("Select Bit: Push: %d\n", signal);
							selectBit.insert(signal);

							//Check to see if the number of select bit matches the orgatesize
							if(selectBit.size() == numSameSignal)
								isQueueEmpty = true;
						}

						continue;
					}

					sameSignalCount[signal].insert(i);



					//Check to see if it is an input node
					if(signal <= aigInputLimit)	continue;

					//Search the childrens
					unsigned int c1 = aigraph->getChild1(signal);
					unsigned int c2 = aigraph->getChild2(signal);
					unsigned int node1 = c1 & 0xFFFFFFFE;
					unsigned int node2 = c2 & 0xFFFFFFFE;

					queue[i].push_back(c1);
					levelMap[node1] = levelMap[signal]+1;
					queue[i].push_back(c2);
					levelMap[node2] = levelMap[signal]+1;

					/*printf("    * Signal Pushed: ");
						printf("%d ", c1);
						printf("%d ", c2);
						printf("\n");
					 */
				}
			}


			//Check the selectNegMap
			std::set<int>::iterator iSet;
			bool isSelectNeg = false;
			for(iSet = selectBit.begin(); iSet != selectBit.end(); iSet++){
				if(selectNegMap[*iSet].size() != 2){
					isSelectNeg = true;
					break;
				}
			}

			if(selectBit.size() != numSameSignal || 
					selectBit.size() == 0 ||
					isSelectNeg ){

				/*
					 printf("Not possible...Deleting from list...\n");
					 for(unsigned int i = 0; i < orinit->size(); i++)
					 printf("%d ", orinit->at(i));
					 printf("\t\tOutput: %d\n", *oroutit);
				 */

				orinit = orIn.erase(orinit);
				oroutit = orOut.erase(oroutit);
			}
			else{
				selectBits[*orinit] = selectBit;

				orinit++;
				oroutit++;
			}

			//printf("\n\n");
		}


		//removing gates contained within one another
		orinit = orIn.begin();
		oroutit = orOut.begin();
		printf(" * Removing Redundancies that are contained...");
		while(orinit != orIn.end()){
			orinit2 = orinit;
			orinit2++;
			oroutit2 = oroutit;
			oroutit2++;

			bool next = true;
			while(orinit2 != orIn.end()){

				unsigned int numMatch = 0;

				for(unsigned int i = 0; i < orinit->size(); i++){
					for(unsigned int j = 0; j < orinit2->size(); j++){
						if(orinit->at(i) == orinit2->at(j)){
							numMatch++;
							break;
						}
					}
				}

				if(numMatch == orinit->size()){
					selectBits.erase(*orinit);
					orinit = orIn.erase(orinit);
					oroutit = orOut.erase(oroutit);
					next = false;
					break;
				}
				else if(numMatch == orinit2->size()){
					selectBits.erase(*orinit2);
					orinit2 = orIn.erase(orinit2);
					oroutit2 = orOut.erase(oroutit2);

				}
				else{
					oroutit2++;
					orinit2++;
				}
			}

			if(next){
				orinit++;
				oroutit++;
			}
		}
		printf("done\n");







		//Prepare output
		orinit = orIn.begin();
		oroutit = orOut.begin();

		while(orinit != orIn.end()){
			if(stats.find(orinit->size()) == stats.end()){
				std::map<std::set<int>, int> count;
				count[selectBits[*orinit]] = 1;
				stats[orinit->size()] = count;
			}
			else{
				if(stats[orinit->size()].find(selectBits[*orinit]) == stats[orinit->size()].end()){
					stats[orinit->size()][selectBits[*orinit]] = 1;
				}
				else
					stats[orinit->size()][selectBits[*orinit]] ++;
			}

			orinit++;
			oroutit++;
		}



		/*
			 printf("\n\nPossible Mux or gates\n");	
			 orinit = orIn.begin();
			 oroutit = orOut.begin();

			 unsigned int index = 0;
			 while(orinit != orIn.end()){
			 printf("INPUT: ");
			 for(unsigned int i = 0; i < orinit->size(); i++){
			 printf("%d ", orinit->at(i));
			 }
			 printf("\t\tOutput: %d\t\tSB: ", *oroutit);
			 std::set<int>::iterator iSB;
			 for(iSB = selectBits[*orinit].begin(); iSB != selectBits[*orinit].end(); iSB++){
			 printf("%d ", *iSB);
			 }
			 printf("\n");

			 orinit++;
			 oroutit++;
			 index++;
			 }
			 printf("\n\n");
		 */


		for(iStats = stats.begin(); iStats!=stats.end(); iStats++){
			std::map<std::set<int>, int>::iterator iCount;
			//			printf("%d-1 Mux:\n", iStats->first);

			std::map<unsigned, unsigned> sizeCount;
			sizeCountMap[iStats->first] = sizeCount;

			for(iCount = iStats->second.begin(); iCount!=iStats->second.end(); iCount++){
				/*
					 printf("\t* %d-Bit Mux\t\tSEL: ", iCount->second);
					 std::set<int>::iterator iSB;
					 for(iSB = iCount->first.begin(); iSB != iCount->first.end(); iSB++)
					 printf("%d:%d ", *iSB, aigraph->getGNode(*iSB));
					 printf("\n");
				 */

				if(sizeCountMap[iStats->first].find(iCount->second) == sizeCountMap[iStats->first].end())
					sizeCountMap[iStats->first][iCount->second] = 1;
				else
					sizeCountMap[iStats->first][iCount->second]++;
			}
		}

		std::map<unsigned,std::map<unsigned, unsigned> >::iterator iMapM;
		std::map<unsigned,unsigned>::iterator iMap;
		/*
			 for(iMapM = sizeCountMap.begin(); iMapM != sizeCountMap.end(); iMapM++){

			 printf("\t%d-1 MUX:\n", iMapM->first);
			 for(iMap = iMapM->second.begin(); iMap != iMapM->second.end(); iMap++)
			 printf("\t\t%4d-Bit Mux %7d\n", iMap->first, iMap->second);
			 }
		 */


	}





	unsigned int findMux_Orig(CutFunction* cf, AIG* aigraph, std::vector<unsigned int>& returnlist){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR MUXES original -------------------------------\n");

		//Functions in hashmap that are mux operations
		std::map<unsigned long long, std::string>::iterator it;
		std::map<unsigned long long, std::string> hmap;
		std::vector<unsigned long long> muxes;
		cf->getHashMap(hmap);
		printf(" * Parsing function database for multiplexor blocks...\n");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("mux") != std::string::npos)
				muxes.push_back(it->first);
		}

		printf("Multiplexor Bitslices found in hashmap database: %d\n", (int)muxes.size());

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long long, std::vector<InOut*> > pmap;
		cf->getPortMap(pmap);

		//List of inputs of mux components. Last index is the output
		std::vector<std::vector<unsigned> > muxlist;
		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::set<unsigned>::iterator iSet;

		//Get the Inputs and outputs of all the mux components found in the circuit 
		for(unsigned int i = 0; i < muxes.size(); i++){
			iPMAP = pmap.find(muxes[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++){
					//printf("INPUT: ");
					std::vector<unsigned> ports;
					for(iSet = iPMAP->second[j]->input.begin(); iSet != iPMAP->second[j]->input.end(); iSet++){
						//printf("%d ", pmap[muxes[i]][j]->at(k));
						ports.push_back(*iSet);
					}
					//printf("\nOUTPUT:\t%d\n", pmap[muxes[i]][j]->at(k));
					unsigned outnode = iPMAP->second[j]->output;
					ports.push_back(outnode);
					muxlist.push_back(ports);
				}
			}
		}
		printf("done\n");
		printf("Number of mux bitslice: %d\n", (int)muxlist.size());

		//simlar input, set of output
		printf(" * Looking for blocks with similar inputs...");
		std::map<unsigned, std::set<unsigned> > muxSet;
		std::map<unsigned, std::set<unsigned> > ::iterator mit;
		std::map<unsigned, std::set<unsigned> > ::iterator mit2;
		int skippedmux = 0; 
		//std::vector<std::vector<unsigned> > muxlist;
		for(unsigned int i = 0; i < muxlist.size(); i++){
			std::map<unsigned int, int> inputCount;
			std::map<unsigned int, int>::iterator iCount;
			for(unsigned int k = 0; k < muxlist[i].size()-1; k++)
				inputCount[muxlist[i][k]] = 0;

			//Assert mux 2-1 have 3 inputs;
			//printf("MUX INPUT SIZE: %d. INPUT SIZE COUNT: %d\n", muxlist[i].size()-1, inputCount.size());
			assert(inputCount.size() == 3);

			unsigned int outputNode = muxlist[i][muxlist[i].size()-1];
			cf->DFS(outputNode, inputCount);
			unsigned int num1 = 0;
			unsigned int num2 = 0;
			unsigned int other = 0; 
			unsigned select;
			for(iCount = inputCount.begin(); iCount != inputCount.end(); iCount++){
				//printf("INPUT: %d COUNT: %d\n", iCount->first, iCount->second);
				if(iCount->second == 1) num1++;
				else if(iCount->second == 2){
					num2++;
					select = iCount->first;
				}
				else other++;
			}

			/*
				 printf("\n");
				 std::list<unsigned> out; 
				 out.push_back(outputNode)	;

				 std::set<unsigned> in;
				 for(unsigned int k = 0; k < muxlist[i].size()-1; k++)
				 in.insert(muxlist[i][k]);

				 aigraph->printSubgraph(out, in);

			 */

			if(other != 0 || num1 != 2 || num2 != 1){
				skippedmux++;
				continue;
			}

			muxSet[select].insert(outputNode);
		}

		//printf("skipped mux: %d out of %d\ndone\n", skippedmux, muxlist.size());





		printf("\n\n\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("\n----------------------------------------------------------------\n");
		printf("Result Search:\n");
		printf("----------------------------------------------------------------\n");
		printf("Number of bitslice mux:\t%d\n", (int)muxlist.size()); 
		//mux size, list of a list of outputs
		std::map<int, int> muxCount;
		std::map<int, int>::iterator cnt;
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){

			if(muxCount.find(mit->second.size()) == muxCount.end()){
				muxCount[mit->second.size()] = 1;
			}
			else{
				muxCount[mit->second.size()]++;
			}
		}


		printf("2-1 Mux Found:\t%d\n", (int) muxCount.size());
		for(cnt = muxCount.begin(); cnt != muxCount.end(); cnt++){
			printf("   %2d-bit mux.....%d\n", cnt->first, (int) cnt->second);
			returnlist.push_back(cnt->first);
		}



		return muxlist.size();
	}

	unsigned int findMux(CutFunction* cf, AIG* aigraph, std::vector<unsigned int>& returnlist){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR MUXES original -------------------------------\n");

		//Functions in hashmap that are mux operations
		std::map<unsigned long long, std::string>::iterator it;
		std::map<unsigned long long, std::string> hmap;
		std::vector<unsigned long long> muxes;
		cf->getHashMap(hmap);
		printf(" * Parsing function database for multiplexor blocks...\n");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("mux") != std::string::npos)
				muxes.push_back(it->first);
		}

		printf("Multiplexor Bitslices found in hashmap database: %d\n", (int)muxes.size());

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long long, std::vector<InOut*> > pmap;
		cf->getPortMap(pmap);

		//List of inputs of mux components. Last index is the output
		std::vector<std::vector<unsigned> > muxlist;
		std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
		std::set<unsigned>::iterator iSet;

		//Get the Inputs and outputs of all the mux components found in the circuit 
		for(unsigned int i = 0; i < muxes.size(); i++){
			iPMAP = pmap.find(muxes[i]);
			if(iPMAP != pmap.end()){
				for(unsigned int j = 0; j < iPMAP->second.size(); j++){
					//printf("INPUT: ");
					std::vector<unsigned> ports;
					for(iSet = iPMAP->second[j]->input.begin(); iSet != iPMAP->second[j]->input.end(); iSet++){
						//printf("%d ", pmap[muxes[i]][j]->at(k));
						ports.push_back(*iSet);
					}
					//printf("\nOUTPUT:\t%d\n", pmap[muxes[i]][j]->at(k));
					unsigned outnode = iPMAP->second[j]->output;
					ports.push_back(outnode);
					muxlist.push_back(ports);
				}
			}
		}
		printf("done\n");
		printf("Number of mux bitslice: %d\n", (int)muxlist.size());

		//simlar input, set of output
		printf(" * Looking for blocks with similar inputs...");
		std::map<unsigned, std::set<unsigned> > muxSet;
		std::map<unsigned, std::set<unsigned> > ::iterator mit;
		std::map<unsigned, std::set<unsigned> > ::iterator mit2;
		for(unsigned int i = 0; i < muxlist.size(); i++){
			for(unsigned int k = i+1; k < muxlist.size(); k++){
				//if(k == i) continue;

				for(unsigned int j = 0; j < muxlist[i].size()-1; j++){
					for(unsigned int l = 0; l < muxlist[k].size()-1; l++){
						//printf("Comparing %d %d\n", muxlist[i][j] , muxlist[k][l]);
						if(muxlist[i][j] == muxlist[k][l]){
							if(muxSet.find(muxlist[i][j]) != muxSet.end())
								muxSet[muxlist[i][j]].insert(muxlist[k][muxlist[k].size()-1]);
							else{
								std::set<unsigned> outset;
								outset.insert(muxlist[i][muxlist[i].size()-1]);
								outset.insert(muxlist[k][muxlist[k].size()-1]);

								muxSet[muxlist[i][j]] = outset;
							}

							break;
						}
					}
				}
			}
		}
		printf("done\n");



		std::set<unsigned>::iterator setit;

		printf(" * Aggregating Results...");
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
			for(mit2 = muxSet.begin(); mit2 != muxSet.end(); mit2++){
				if(mit == mit2) continue;

				unsigned int count = 0;
				for(setit = mit2->second.begin(); setit != mit2->second.end(); setit++){
					if(mit->second.find(*setit) != mit->second.end())
						count++;
				}

				if(count == mit2->second.size()){
					muxSet.erase(mit2);
					mit2 = mit;
				}
			}
		}

		printf("done\n");




		printf("\n\n\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("\n----------------------------------------------------------------\n");
		printf("Result Search:\n");
		printf("----------------------------------------------------------------\n");
		printf("Number of bitslice mux:\t%d\n", (int)muxlist.size()); 
		//mux size, list of a list of outputs
		std::map<int, std::vector<std::vector<unsigned> > > muxCount;
		std::map<int, std::vector<std::vector<unsigned> > >::iterator cnt;
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
			//printf("%2d-bit mux\n", (int) mit->second.size());
			//printf("Inputs: %d\tOutputs: ", mit->first);
			for(setit = mit->second.begin(); setit != mit->second.end(); setit++){
				//printf("%d ", *setit);

			}
			//printf("\n\n");



			if(muxCount.find(mit->second.size()) == muxCount.end()){
				std::vector<std::vector<unsigned> > listOutList;
				std::vector<unsigned> outlist;
				for(setit = mit->second.begin(); setit != mit->second.end(); setit++)
					outlist.push_back(*setit);

				listOutList.push_back(outlist);
				muxCount[mit->second.size()] = listOutList;

			}
			else{
				std::vector<unsigned> outlist;
				for(setit = mit->second.begin(); setit != mit->second.end(); setit++)
					outlist.push_back(*setit);

				muxCount[mit->second.size()].push_back(outlist);
			}
		}


		printf("2-1 Mux Found:\t%d\n", (int) muxCount.size());
		for(cnt = muxCount.begin(); cnt != muxCount.end(); cnt++){
			printf("   %2d-bit mux.....%d\n", cnt->first, (int) cnt->second.size());
			returnlist.push_back(cnt->first);
			/*
				 for(unsigned int i = 0; i < cnt->second.size(); i++){
				 printf("    *   ");
				 for(unsigned int j = 0; j < cnt->second[i].size(); j++){
				 printf("%d ", cnt->second[i][j]);
				 }
				 printf("\n");
				 }
				 printf("\n\n");
			 */
		}



		/*
			 printf("4-1 Mux Found:\t%d\n", (int)muxset4.size());
			 for(muxset4it = muxset4.begin(); muxset4it != muxset4.end(); muxset4it++){
			 printf("    %2d-bit mux\n", (int) muxset4it->second.size());
			 printf("      OR4: ");
			 for(unsigned int i = 0; i < muxset4it->second.size(); i++){
			 printf("%d ", muxset4it->second[i]);
			 }
			 printf("\n");

			 }
		 */




		return muxlist.size();
	}



	int findNegInput(AIG* aigraph, unsigned out, std::vector<unsigned>& in){
		std::list<unsigned> queue;
		std::set<unsigned> mark;
		std::vector<unsigned*> nChilds;


		//Enqueue
		queue.push_back(out);
		mark.insert(out);
		int numNegative = 0;

		/*
			 printf("NEG: OUT:%d IN: ", out);
			 for(unsigned int i = 0; i < in.size(); i++){
			 printf("%d ", in[i]);
			 }
			 printf("\n");
		 */

		while(queue.size() != 0){
			unsigned qitem = queue.front();
			queue.pop_front();


			//If reached an input, return
			bool foundInput = false;
			bool isNeg = false;
			if(qitem & 0x1){
				qitem = qitem & 0xFFFFFFFE;
				isNeg = true;
			}

			//Check to see if node is at the input
			for(unsigned int i = 0; i < in.size(); i++){
				unsigned input = in[i];
				input = input & 0xFFFFFFFE;

				if(qitem == in[i]){
					if(isNeg)
						numNegative++;

					foundInput = true;
					break;
				}
			}

			if(foundInput) continue;

			//Enqueue childs
			unsigned c1node = aigraph->getChild1(qitem);
			unsigned c2node = aigraph->getChild2(qitem);
			unsigned c1 = c1node & 0xFFFFFFFE;
			unsigned c2 = c2node & 0xFFFFFFFE;

			if(mark.find(c1) == mark.end()){
				queue.push_back(c1node);
				mark.insert(c1);
			}
			if(mark.find(c2) == mark.end()){
				queue.push_back(c2node);
				mark.insert(c2);
			}
		}

		return numNegative;

	}


	bool isInputNeg(AIG* aigraph, unsigned out, unsigned in){
		std::list<unsigned> queue;
		std::set<unsigned> mark;
		std::vector<unsigned*> nChilds;


		//Enqueue
		queue.push_back(out);
		mark.insert(out);

		while(queue.size() != 0){
			unsigned qitem = queue.front();
			unsigned qNode = qitem & 0xFFFFFFFE;
			queue.pop_front();

			unsigned input =  in & 0xFFFFFFFE;

			//If node is the node we're looking for, check to see if it is neg
			if(qNode == input){
				if(qitem & 0x1)
					return true;
			}

			//Continue because node is an input, no child
			if(qNode <= aigraph->getInputSize()*2) continue;

			//Enqueue childs
			unsigned c1node = aigraph->getChild1(qNode);
			unsigned c2node = aigraph->getChild2(qNode);
			unsigned c1 = c1node & 0xFFFFFFFE;
			unsigned c2 = c2node & 0xFFFFFFFE;

			if(mark.find(c1) == mark.end()){
				queue.push_back(c1node);
				mark.insert(c1);
			}
			if(mark.find(c2) == mark.end()){
				queue.push_back(c2node);
				mark.insert(c2);
			}
		}

		return false;
	}


























	// EXTRAS THAT ARE CURRENTLY NOT USED

	/*#############################################################################
	 *
	 * DFS_End_Set
	 *  Perform DFS to see if it can hit the end node
	 *  that doesn't go past input as well (boundary)
	 *
#############################################################################*/
	int DFS_End_Set(AIG* aig, unsigned start, unsigned lb, std::set<unsigned>& input,  std::set<unsigned>& marked){
		start = start & 0xFFFFFFFE;
		marked.insert(start);
		//printf("Traversing start node: %d\n", start);

		if(input.find(start) != input.end()) return start;
		//if(start < lb) return -1;
		if(start <= aig->getInputSize()*2){
			//printf("DFS -- Start is INPUT\n");
			return -1;
		}

		unsigned node1, node2, child1, child2;
		child1 = aig->getChild1(start);
		child2 = aig->getChild2(start);

		node1 = child1 & 0xFFFFFFFE;
		node2 = child2 & 0xFFFFFFFE;

		if(marked.find(node1) == marked.end()){
			int result = DFS_End_Set(aig, node1, lb, input,  marked);
			if (result != -1) return result;

		}

		if(marked.find(node2) == marked.end()){
			int result = DFS_End_Set(aig, node2, lb, input,  marked);
			if (result) return result;
		}

		return -1;
	}


}

/*j
	void adderAggregation( AIG* aig, 
	std::map<unsigned long long, std::vector<InOut*> >& pmap, 
	std::vector<unsigned long long>& inOut, 
	std::set<unsigned> sumNodes, 
	std::list<std::set<unsigned> >& addIn,
	std::list<std::set<unsigned> >& addOut){
	printf("\n\n-------------------------------------------------------------------------------\n");
	printf("ADDER AGGREGATING UNFCIONT------\n");

	std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
	std::list<std::set<unsigned> >::iterator iList1;
	std::list<std::set<unsigned> >::iterator iList2;

//Go through each adder function
for(unsigned int i = 0; i < inOut.size(); i++){
iPMAP = pmap.find(inOut[i]);
if(iPMAP != pmap.end()){
for(unsigned int j = 0; j < iPMAP->second.size(); j++){

std::set<unsigned> inSet;
unsigned outnode = iPMAP->second[j]->at(iPMAP->second[j]->size()-1);
for(unsigned int k = 0; k < iPMAP->second[j]->size()-1; k++){
inSet.insert(iPMAP->second[j]->at(k));
}

//If no aggregation if found, check if the inputs lead to any of the other inputs in the adder sets
std::set<unsigned>::iterator iSet, iASet;	
printf("\n\n. Checking containment\n");
printf("INSET: ");
for(iSet = inSet.begin(); iSet != inSet.end(); iSet++)
printf("%d ", *iSet);
printf("\n");
printf("CURRENT ADDLIST: \n");
printAddList(addIn, addOut);

bool containment = false;
bool isInputContained = false;
iList2 = addOut.begin();
for(iList1 = addIn.begin(); iList1 != addIn.end(); iList1++){

isInputContained = false;
int containedCount = 0; 
for(iSet = inSet.begin(); iSet != inSet.end(); iSet++){
unsigned iVal = *iSet;
//If the item already exists...no need to check for containment


std::vector<unsigned> tobedeleted;
bool deleteInSet = false;
for(iASet= iList1->begin(); iASet != iList1->end(); iASet++){
if(iVal == *iASet){
isInputContained = true;
containedCount++;
continue;
}

bool inSetInput = false;
bool addlistinput = false;
if(iVal <= aig->getInputSize() * 2) inSetInput = true;
if(*iASet <= aig->getInputSize() * 2) addlistinput = true;;

if(inSetInput && addlistinput) continue;

printf("Checking containment between %d and %d\n", iVal, *iASet);

std::set<unsigned> marked;
int isContained; 
unsigned root, node;
if(iVal > *iASet){
root = iVal;
node = *iASet;
}
else{
	root = *iASet ;
	node = iVal;
}

unsigned node1, node2;
node1 = aig->getChild1(root) & 0xFFFFFFFE;
node2 = aig->getChild2(root) & 0xFFFFFFFE;

printf("ROOT: %d\tNODE: %d\n", root, node);


isContained = DFS(aig, node1, node, *iList1, sumNodes, marked);

if(isContained == 1){
	containment = true;
	printf("SIDE2 OUTPUT %d is contained under %d: RESULT: %d\n", node, root, isContained);
	tobedeleted.push_back(root);
	if(iVal > *iASet){
		deleteInSet = true;
		isInputContained = true;
		containedCount++;
		break;
	}

	continue;
}
else if(isContained == 2) {
	printf("INTERMEDIATE OUTPUT SUM NODE HIT!\n");
	exit(1);
}

isContained = DFS(aig, node2, node, *iList1, sumNodes, marked);

if(isContained == 1){
	containment = true;
	printf("SIDE1 OUTPUT %d is contained under %d: RESULT: %d\n", node, root, isContained);
	tobedeleted.push_back(root);
	if(iVal > *iASet){
		deleteInSet = true;
		isInputContained = true;
		containedCount++;
		break;
	}
}
else if(isContained == 2) {
	printf("INTERMEDIATE OUTPUT SUM NODE HIT!\n");
	exit(1);
}
}

//Make sure all the inputs are contained
if(!isInputContained) break;

if(deleteInSet) 
	for(unsigned int k = 0; k < tobedeleted.size(); k++)
	inSet.erase(tobedeleted[k]);
	else	
	for(unsigned int k = 0; k < tobedeleted.size(); k++)
	iList1->erase(tobedeleted[k]);
	}

//Combine the two that are similar
if(containment && isInputContained){
	for(iSet = inSet.begin(); iSet != inSet.end(); iSet++){
		iList1->insert(*iSet);
	}
	iList2->insert(outnode);
	break;
}
iList2++;
}

if(!containment || !isInputContained){
	addIn.push_back(inSet);
	std::set<unsigned> outSet;
	outSet.insert(-1);
	outSet.insert(outnode);
	addOut.push_back(outSet);
}

}
printf("CURRENT ADDLIST: \n");
printAddList(addIn, addOut);
printf("\n\n\n");
}
}

}

void carryAggregation(std::map<unsigned long long, std::vector<InOut*> >& pmap, 
		std::vector<unsigned long long>& inOut, 
		std::set<unsigned> sumNodes, 
		std::list<std::set<unsigned> >& addIn,
		std::list<std::set<unsigned> >& addOut){

	std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
	std::list<std::set<unsigned> >::iterator iList1;

	for(unsigned int i = 0; i < inOut.size(); i++){
		iPMAP = pmap.find(inOut[i]);
		if(iPMAP != pmap.end()){
			for(unsigned int j = 0; j < iPMAP->second.size(); j++){

				std::set<unsigned> inSet;
				bool isAgg = false;
				bool sumOutput = false;
				for(unsigned int k = 0; k < iPMAP->second[j]->size()-1; k++){
					unsigned outnode = iPMAP->second[j]->at(iPMAP->second[j]->size()-1);
					if(sumNodes.find(outnode) != sumNodes.end()){
						sumOutput = true;
						continue;
					}

					inSet.insert(iPMAP->second[j]->at(k));

					for(iList1 = addIn.begin(); iList1 != addIn.end(); iList1++){
						if(iList1->find(iPMAP->second[j]->at(k)) !=  iList1->end()){
							isAgg = true;
							break;
						}
						else if(iList1->find(outnode) != iList1->end()){
							isAgg = true;
							break;
						}
					}

					if(isAgg) {
						for(unsigned int w = 0; w < iPMAP->second[j]->size()-1; w++)
							iList1->insert(iPMAP->second[j]->at(w));
						iList1->insert(outnode);
						break;
					}
				}

				if(!isAgg && !sumOutput){
					addIn.push_back(inSet);
					std::set<unsigned> outSet;
					outSet.insert(-1);
					addOut.push_back(outSet);
				}
			}
		}
	}

}
void adderAggregation2( AIG* aig, 
		std::map<unsigned , std::set<unsigned> >& outIn, 
		std::set<unsigned> sumNodes, 
		std::list<std::set<unsigned> >& addIn,
		std::list<std::set<unsigned> >& addOut){
	printf("\n\n-------------------------------------------------------------------------------\n");
	printf("ADDER AGGREGATION   ------\n");

	std::list<std::set<unsigned> >::iterator iList1;
	std::list<std::set<unsigned> >::iterator iList2;
	std::map<unsigned , std::set<unsigned> >::iterator iMap;

	//Go through each adder function
	for(iMap = outIn.begin(); iMap != outIn.end(); iMap++){

		unsigned outnode = iMap->first;

		//If no aggregation if found, check if the inputs lead to any of the other inputs in the adder sets
		std::set<unsigned>::iterator iSet, iASet;	
		printf("\n\n. Checking containment##################################################################\n");
		printf("INSET: ");
		for(iSet = inSet.begin(); iSet != inSet.end(); iSet++)
			printf("%d ", *iSet);
		printf("\t\tOUTNODE: %d\n", outnode);
		printf("CURRENT ADDLIST: \n");
		printAddList(addIn, addOut);

		bool containment = false;
		//bool isInputContained = false;

		//Look for outnode to the inputs of addlist
		iList2 = addOut.begin();
		for(iList1 = addIn.begin(); iList1 != addIn.end(); iList1++){

			//replace items with furthest cut
			std::set<unsigned> inputCopySet;
			unsigned lb = 0xFFFFFFFF;
			for(iASet= iList1->begin(); iASet != iList1->end(); iASet++){
				std::list<std::set<unsigned> > cuts;	
				ce->getCuts((*iASet)/2, cuts);
				std::set<unsigned>::iterator iCut;

				//first cut is usually the furthest
				//printf("--------\n");

				for(iCut = cuts.front().begin(); iCut != cuts.front().end(); iCut++){
					//printf("CUT: %d\n", *iCut);
					if(inputCopySet.find(*iCut) == inputCopySet.end()){

						if(*iCut < lb){
							lb = *iCut;
							//printf("INPUTING INTO COPY CUT LB SET: %d\n", *iCut);
							inputCopySet.insert(*iCut);
							continue;

						}

						std::set<unsigned> marked;
						std::set<unsigned> found;
						std::list<unsigned> endpoints;
						int result = DFS(aig, *iCut, lb, inputCopySet, endpoints, found,  marked);

						if(result == -1){
							//printf("NOT FULLY CONTAINED\n");
							//printf("INPUTING INTO COPY CUT: %d\n", *iCut);
							inputCopySet.insert(*iCut);
						}
						else{
							//printf("CONTAINED!!!!\n");
							continue;
						}

					}
				}
			}

			printf("CopyInputSet: ");
			for(iSet = inputCopySet.begin(); iSet != inputCopySet.end(); iSet++){
				printf("%d ", *iSet);
			}
			printf("\n");


			std::set<unsigned> marked;
			std::set<unsigned> found;
			std::list<unsigned> endpoints;
			//printf("Performing DFS Search using INSET OUTPUT: %d LB: %d\n", outnode, lb);
			int result = DFS(aig, outnode, lb, inputCopySet, endpoints, found,  marked);

			printf("found (INPUTS HIT): ");
			for(iSet = found.begin(); iSet != found.end(); iSet++){
				printf("%d ", *iSet);
			}
			printf("\n");

			if(found.size() == inputCopySet.size() || result == 1){
				//printf("INPUT CUT MATCH!!!\n");
				containment = true;
				iList2->insert(outnode);
				for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++)
					iList1->insert(*iSet);

				//Simplify to see if the inputs are contained
				iSet = iList1->begin();
				lb = *iSet;
				iSet++; //Smallest cannot be contained since it goes by levels
				unsigned prevVal = lb;
				while(iSet != iList1->end()){
					printf("Checking containment: %d......", *iSet);
					marked.clear();
					found.clear();
					endpoints.clear();
					result = DFS(aig, *iSet, lb, *iList1, endpoints, found,  marked);

					if(result == 1){
						printf(" -- CONTAINED!\nErasing %d from set\n", *iSet);
						iList1->erase(*iSet);
						iSet = iList1->find(prevVal);
					}
					else printf("\n");
					prevVal = *iSet;
					iSet++;
				}

				break;
			}

			iList2++;

		}

		if(containment == false){
			addIn.push_back(iMap->second);
			std::set<unsigned> outSet;
			outSet.insert(-1);
			outSet.insert(outnode);
			addOut.push_back(outSet);
		}
	}
}
*/

#endif




/*
	 void findHA(CutFunction* cf, AIG* aigraph){
	 printf("\n\n\n");
	 printf("[AGG] -- SEARCHING FOR ADDERS-------------------------------");
	 const unsigned long long xor1 = 0x6666666666666666;
	 const	unsigned long long xor2 = 0x9999999999999999;

//Function, Vector of every set of inputs with that function. Last item is the output node
std::map<unsigned long long, std::vector<InOut*> > pmap;
cf->getPortMap(pmap);

printf(" * Parsing function database for possible Half Adders...");
std::map<std::vector<unsigned>, std::vector<unsigned> > faOutput;
if(pmap.find(xor1) != pmap.end()){
for(unsigned int i = 0; i < pmap[xor1].size(); i++){
unsigned int j = 0;
//printf("INPUT: ");
//for(j = 0; j < pmap[xor1][i]->size()-1; j++){
//	printf("%d ", pmap[xor1][i]->at(j));
//}
//printf("\nOUTPUT:\t%d\n", pmap[xor1][i]->at(j));
unsigned outnode = pmap[xor1][i]->at(j);

//Is there an and gate that has the same inputs as the XOR gate
std::vector<unsigned> andindex;
andindex = aigraph->findNode(pmap[xor1][i]->at(0), pmap[xor1][i]->at(1));
for(unsigned int j = 0; j < andindex.size(); j++){
if(aigraph->hasChild(andindex[j], outnode)){
//printf("THIS XOR IS A HA- S:%d  CO:%d\n\n", outnode, andindex[j]);
//0- Sum, 1- CO
std::vector<unsigned > sumCO;
sumCO.push_back(outnode);
sumCO.push_back(andindex[j]);

//Inputs
std::vector<unsigned> inputs;
for(unsigned int k = 0; k < pmap[xor1][i]->size()-1; k++)
inputs.push_back(pmap[xor1][i]->at(k));

faOutput[inputs] = sumCO;
}
}
}
}


if(pmap.find(xor2) != pmap.end()){
for(unsigned int i = 0; i < pmap[xor2].size(); i++){
unsigned int j = 0;
//printf("INPUT: ");
//for(j = 0; j < pmap[xor2][i]->size()-1; j++){
//	printf("%d ", pmap[xor2][i]->at(j));
//	}

//printf("\nOUTPUT:\t%d\n", pmap[xor2][i]->at(j));
unsigned outnode = pmap[xor2][i]->at(j);

//Is there an and gate that has the same inputs as the XOR gate
std::vector<unsigned> andindex;
andindex = aigraph->findNode(pmap[xor2][i]->at(0), pmap[xor2][i]->at(1));
for(unsigned int j = 0; j < andindex.size(); j++){
if(aigraph->hasChild(andindex[j], outnode)){
//printf("THIS XOR IS A HA- S:%d  CO:%d\n\n", outnode, andindex[j]);

//0- Sum, 1- CO
std::vector<unsigned> sumCO;
sumCO.push_back(outnode);
sumCO.push_back(andindex[j]);

//Inputs
std::vector<unsigned> inputs;
for(unsigned int k = 0; k < pmap[xor2][i]->size()-1; k++)
inputs.push_back(pmap[xor2][i]->at(k));

faOutput[inputs] = sumCO;
}
}
}
}
printf("done\n");



std::map<std::vector<unsigned>, std::vector<unsigned> >::iterator outit;
//printf("\n\nHALF ADDERS\n");
// for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
// printf("INPUTS: ");
// for(unsigned int i = 0; i < outit->first.size(); i++)
// printf("%d ", outit->first[i]);
// printf("\nSUM:%d\tCO:%d\n", outit->second[0], outit->second[1]);
//}

printf("\n----------------------------------------------------------------\n");
printf("Number of Possible Half Adders\t\t%d\n", (int) faOutput.size());
printf("----------------------------------------------------------------\n\n");




// SEARCHING GREY BLOCKS




printf(" * Parsing function database for Grey Blocks...");
const	unsigned long long grey= 0x8c8c8c8c8c8c8c8c;
const	unsigned long long grey2= 0x8a8a8a8a8a8a8a8a;
const	unsigned long long grey3= 0x0e0e0e0e0e0e0e0e;

//Function, Vector of every set of inputs with that function. Last item is the output node
cf->getPortMap(pmap);

//List of inputs of sum components. Last index is the output
std::vector<std::vector<unsigned> > gPorts;

if(pmap.find(grey) != pmap.end()){
	for(unsigned int i = 0; i < pmap[grey].size(); i++){
		unsigned int j = 0;
		//printf("INPUT: ");
		std::vector<unsigned> ports;
		for(j = 0; j < pmap[grey][i]->size()-1; j++){
			//printf("%d ", pmap[grey][i]->at(j));
			ports.push_back(pmap[grey][i]->at(j));
		}
		//printf("\nOUTPUT:\t%d\n", pmap[grey][i]->at(j));
		unsigned outnode = pmap[grey][i]->at(j);
		ports.push_back(outnode);
		gPorts.push_back(ports);
	}
}

if(pmap.find(grey2) != pmap.end()){
	for(unsigned int i = 0; i < pmap[grey2].size(); i++){
		unsigned int j = 0;
		//printf("INPUT: ");
		std::vector<unsigned> ports;
		for(j = 0; j < pmap[grey2][i]->size()-1; j++){
			//printf("%d ", pmap[grey2][i]->at(j));
			ports.push_back(pmap[grey2][i]->at(j));
		}
		//printf("\nOUTPUT:\t%d\n", pmap[grey2][i]->at(j));
		unsigned outnode = pmap[grey2][i]->at(j);
		ports.push_back(outnode);
		gPorts.push_back(ports);
	}
}

if(pmap.find(grey3) != pmap.end()){
	for(unsigned int i = 0; i < pmap[grey3].size(); i++){
		unsigned int j = 0;
		//printf("INPUT: ");
		std::vector<unsigned> ports;
		for(j = 0; j < pmap[grey3][i]->size()-1; j++){
			//printf("%d ", pmap[grey3][i]->at(j));
			ports.push_back(pmap[grey3][i]->at(j));
		}
		//printf("\nOUTPUT:\t%d\n", pmap[grey3][i]->at(j));
		unsigned outnode = pmap[grey3][i]->at(j);
		ports.push_back(outnode);
		gPorts.push_back(ports);
	}
}
printf("done\n");

//printf("PRINTING GBLOCKS\n");
//for(unsigned int j = 0; j < gPorts.size(); j++){
//printf("INPUT: ");
//unsigned int k = 0;
//for(k = 0; k < gPorts[j].size()-1; k++){
//printf("%d ", gPorts[j][k]);

//}
//printf("\nOUTPUT:\t%d\n", gPorts[j][k]);
//	}
printf(" * Aggregating Results...");	
//Vector of vector of iterators. (Aggregated adders)
std::map<unsigned long long, std::string>::iterator it;
std::vector<std::set<unsigned> > marked;
int numHA = 0;
int numGB = 0;


//For each ha, see if there is a connecting gb
for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
	//printf("Checking CO: %d\n", outit->second[1]);

	//Check to see which grey the co goes out to. 
	for(unsigned int z = 0; z < gPorts.size(); z++){
		for(unsigned int i = 0; i < gPorts[z].size()-1; i++){

			//If carry out is the same as input to grey block, mark
			bool foundGB = false;
			//printf("Comparing %d - %d\n", outit->second[1], gPorts[z][i]);
			if(outit->second[1] == gPorts[z][i]){
				for(unsigned int j = 0; j < marked.size(); j++){
					if(marked[j].find(outit->second[1]) != marked[j].end()){
						//printf("Cout found in set %d...inserting cout\n\n", j);
						marked[j].insert(gPorts[z][gPorts[z].size()-1]);
						foundGB = true;
						numGB++;
						break;
					}
					else if(marked[j].find(gPorts[z][gPorts[z].size()-1]) != marked[j].end()){
						//printf("GB not found in set %d...inserting gb output\n\n", j);
						marked[j].insert(outit->second[1]);
						foundGB = true;
						numHA++;
						break;
					}
				}
				if(!foundGB){
					//printf("No prev set found, creating new set\n\n");
					std::set<unsigned> n;
					n.insert(outit->second[1]);
					n.insert(gPorts[z][gPorts[z].size()-1]);
					marked.push_back(n);
					numHA++;
					numGB++;
					break;
				}
			}

		}
	}
}
printf("done\n");


//This is mainly for carry ripple
printf(" * Aggregating carry ripple architecture...");
std::set<unsigned>::iterator sit;
std::set<unsigned>::iterator sit2;
for(unsigned int z = 0; z < gPorts.size(); z++){
	unsigned gout = gPorts[z][gPorts[z].size()-1];
	for(unsigned int i = 0; i < gPorts.size(); i++){
		if(z == i) continue;

		for(unsigned int j = 0; j < gPorts[i].size()-1; j++){
			//printf("Checking %d to %d\n", gPorts[i][j], gout);
			if(gout == gPorts[i][j])
			{
				//printf("FOUND %d to %d\n", gout, gPorts[i][gPorts[i].size()-1]);
				int index1 = -1;
				int index2 = -1;
				for(unsigned int k = 0; k < marked.size(); k++){
					if(marked[k].find(gout) != marked[k].end())
						index1 = k;
					else if(marked[k].find(gPorts[i][gPorts[i].size()-1]) != marked[k].end())
						index2 = k;
				}

				//printf("index1 %d, index2 %d\n", index1, index2);
				if(index1 != -1 && index2 != -1){
					//printf("ERASING INDEX %d\n", index1);
					for(sit = marked[index1].begin(); sit != marked[index1].end(); sit++){
						marked[index2].insert(*sit);
					}
					marked.erase(marked.begin() + index1);
				}
			}
		}
	}
}
printf("done\n");





printf("\n\n");
printf("[AGG] -- SEARCHING FOR ADDERS USING halfadder_gp BLOCKS-------------------------------\n");
std::vector<unsigned long long> hagp;
std::map<unsigned long long, std::string> hmap;
cf->getHashMap(hmap);

//find halfadder_gp 
printf(" * Parsing function database for possible halfadder_gp Blocks...");
for(it = hmap.begin(); it!=hmap.end(); it++){
	if(it->second.find("halfadder_gp") != std::string::npos)
		hagp.push_back(it->first);
}

std::vector<std::vector<unsigned> > hagpInSet;
for(unsigned int i = 0; i < hagp.size(); i++){
	if(pmap.find(hagp[i]) != pmap.end()){
		for(unsigned int j = 0; j < pmap[hagp[i]].size(); j++){
			unsigned int k = 0;
			std::vector<unsigned> ports;

			//sort positive inputs (used as key)
			for(k = 0; k < pmap[hagp[i]][j]->size()-1; k++){
				unsigned in = pmap[hagp[i]][j]->at(k);
				ports.push_back(in);
			}
			ports.push_back(pmap[hagp[i]][j]->at(k));
			hagpInSet.push_back(ports);
		}
	}
}
printf("done\n");


std::vector<std::vector<unsigned> > addbitIn;
std::vector<std::vector<unsigned> > addbitOut;

//For each ha, see if there is a halfadder_gp with same input 
printf(" * Looking for blocks with similar inputs...");
for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
	int inputCount = 0; 
	for(unsigned int i = 0; i < hagpInSet.size(); i++){
		//For each input of HAGP, compare INPUT of HA to see if HAGP matches
		for(unsigned int j = 0; j < hagpInSet[i].size()-1; j++){
			//for each input of HA
			for(unsigned int k = 0; k < outit->first.size(); k++){
				//printf("Comparing %d - %d\n", outit->first[k], hagpInSet[i][j]);
				if(outit->first[k] == hagpInSet[i][j]){
					inputCount++;
					break;
				}
			}

			if(inputCount == 2)	break;	
		}

		if(inputCount == 2){
			std::vector<unsigned> input;
			std::vector<unsigned> output;
			input.push_back(hagpInSet[i][0]);
			input.push_back(hagpInSet[i][1]);
			input.push_back(hagpInSet[i][2]);

			output.push_back(outit->second[1]);	
			output.push_back(hagpInSet[i][3]);

			addbitIn.push_back(input);
			addbitOut.push_back(output);

			break;	
		}

	}

	if(inputCount != 2){
		std::vector<unsigned> input;
		std::vector<unsigned> output;
		input.push_back(outit->first[0]);
		input.push_back(outit->first[1]);

		output.push_back(outit->second[1]);
		output.push_back(outit->second[0]);

		addbitIn.push_back(input);
		addbitOut.push_back(output);
	}
}
printf("done\n");

//printf("Printing ha and hagp set\n");
//for(unsigned int i = 0; i < addbitIn.size(); i++){
//printf("Input:\t");
//for(unsigned int j = 0; j < addbitIn[i].size(); j++){
//printf("%d ", addbitIn[i][j]);
//}

//printf("\nOutput:\t");
//for(unsigned int j = 0; j < addbitOut[i].size(); j++){
//printf("%d ", addbitOut[i][j]);
//}
//printf("\n\n");

//}
//output index 0
std::vector<std::set<unsigned> > marked2; 

printf(" * Aggregating results...\n");
for(unsigned int i = 0; i < addbitOut.size(); i++){
	for(unsigned int j = 0; j < addbitIn.size(); j++){
		if(i == j) continue;
		else if(addbitIn[j].size() == 2) continue;

		for(unsigned int k = 0; k < addbitIn[j].size(); k++){
			bool found = false;
			//printf("comparing %d %d\n", addbitOut[i][0], addbitIn[j][k]);
			if(addbitOut[i][0] == addbitIn[j][k] ||
					addbitOut[i][1] == addbitIn[j][k]){
				for(unsigned int a = 0; a < marked2.size(); a++){
					if(marked2[a].find(addbitOut[i][0]) != marked2[a].end()){
						//printf("found1\n");
						marked2[a].insert(addbitOut[j][0]);
						found = true;
						break;
					}
					else if(marked2[a].find(addbitOut[j][0]) != marked2[a].end()){
						//printf("found2\n");
						marked2[a].insert(addbitOut[i][0]);
						found = true;
						break;
					}
				}

				if(!found){
					//printf("not found...making new set\n");
					std::set<unsigned> mset;
					mset.insert(addbitOut[j][0]);
					mset.insert(addbitOut[i][0]);

					marked2.push_back(mset);
					break;
				}

			}
		}
	}
}
printf("done\n");




printf("\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
printf("\n----------------------------------------------------------------\n");
printf("Result Search 1:\n");
printf("Number of HA: %d, Number of GB: %d\n", numHA, numGB);
printf("Number of possible adders:\t %d\n\n", (int) marked.size());
for(unsigned int i = 0; i < marked.size(); i++){
	int nbit = 0; 
	for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
		if(marked[i].find(outit->second[1]) != marked[i].end())
			nbit++;
	}
	printf("   %d-bit adder\n", nbit);
}

printf("----------------------------------------------------------------\n");
printf("----------------------------------------------------------------\n");
printf("Result Search 2:\n");
printf("----------------------------------------------------------------\n");

for(unsigned int i = 0; i < marked2.size(); i++){
	printf("   %d-bit adder\n", (int) marked2[i].size());

}
printf("----------------------------------------------------------------\n");




}
*/

