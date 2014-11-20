/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
	@
	@      cutenumeration.cpp
	@      
	@      @AUTHOR:Kevin Zeng
	@      Copyright 2012 – 2013 
	@      Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "cutenumeration.hpp"



/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     Constructors
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  Constructor
 *    Initializes the Cut Enumeration with an AIG Graph
 *
 *    @PARAMS: 
 *      * graph: AIG CE is suppose to be performed on
 ********************************************************/
CutEnumeration::CutEnumeration(AIG* graph){
	m_AIGraph = graph;



	//printf("CUT RESIZE: %d\n", m_AIGraph->getSize()+m_AIGraph->getInputSize()+1);
}



/*******************************************************
 *  Destructor
 *    Deletes allocated space. Does not delete AIG Graph
 ********************************************************/
CutEnumeration::~CutEnumeration(){
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     Algorithm Methods
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  findKFeasibleCuts
 *    Initial Method that performs the cut enumeration
 *
 *    @PARAMS: 
 *      * k: Upper limit for the set size of cuts
 ********************************************************/
void CutEnumeration::findKFeasibleCuts(unsigned int k ){
	printf("[CutEnumeration]--Find K Feasible Cuts: %d\n", k);
	//printf("Finding K Feasible Cuts\nK = %d\n", k);
	m_K = k;

	//Pre-allocate space for the cutset for each node
	m_Cuts.resize(m_AIGraph->getSize()+m_AIGraph->getInputSize()+1);
	//m_Signatures.resize(m_AIGraph->getSize()+m_AIGraph->getInputSize()+1);

	//Prepare constant nodes
	/*
		 for(unsigned int i = 0; i < 2; i++){
		 std::set<unsigned> singleCut;
		 singleCut.insert(i);
		 std::list<std::set<unsigned> > singleCutSet;
		 singleCutSet.push_back(singleCut);
		 m_Cuts[i] = singleCutSet;

		 std::list<unsigned> singleCutSet;
		 }
	 */



	//Process trivial cuts for inputs
	//m_AIGraph->print();
	for(unsigned int i = 1; i < m_AIGraph->getInputSize()+1; i++){
		unsigned node = i*2;
		//printf("CURRENT IN NODE: %d\n", node);
		std::list<std::set<unsigned> > singleCutSet;
		std::set<unsigned> singleCut;
		singleCut.insert(node);

		//printf("SINGLE CUT LIST: %d\n", node);
		singleCutSet.push_back(singleCut);
		m_Cuts[i] = singleCutSet;

		//Handle Signatures 
		//std::list<unsigned> signature;
		//signature.push_back(calculateSignature(singleCut));
		//m_Signatures[i] = signature;
	}

	unsigned int size = m_AIGraph->getSize()+m_AIGraph->getInputSize()+1;
	float percentC = 0.1;
	unsigned int start = m_AIGraph->getInputSize() + 1;

	for(unsigned int i = start; i < size; i++){
		if((double)i/size >= percentC){
			printf(" -- CutEnumeration is %0.2f Complete\n", ((double)i/size));
			percentC+=0.1;
		}
		unsigned node = 2*i;
		std::set<unsigned> singleCut;
		singleCut.insert(node);
		//printf("\n\nCURRENT AND NODE: %d ####################################### %d\n", node, node);

		//Fix Negative edges for cuts
		int node1, node2;
		node1 = m_AIGraph->getChild1(node);
		node2 = m_AIGraph->getChild2(node);
		node1 = node1&0xFFFFFFFE;
		node2 = node2&0xFFFFFFFE;

		/*
			 printf("cnodes: %d %d\n", node1, node2);
			 printCutSet(node1);
			 printCutSet(node2);
		 */

		std::list<std::set<unsigned> > result;
		merge(m_Cuts[node1/2], m_Cuts[node2/2], result);
		result.push_back(singleCut);
		m_Cuts[i] = result;
	}
	//printf("SIZE: %d\n", m_Cuts.size());
}




/*******************************************************
 *  findKFeasibleCuts
 *    Initial Method that performs the cut enumeration
 *
 *    @PARAMS: 
 *      * k: Upper limit for the set size of cuts
 ********************************************************/
void CutEnumeration::findKFeasibleCuts_In(unsigned int k, std::vector<unsigned int>& interNodes ){
	printf("[CutEnumeration]--Find K Feasible Cuts for subset: %d\n", k);

	//printf("Finding K Feasible Cuts\nK = %d\n", k);
	m_K = k;

	//Process trivial cuts for inputs
	//m_AIGraph->print();

	for(unsigned int i = 0; i < interNodes.size(); i++){
		unsigned node = interNodes[i];

		std::list<std::set<unsigned> > result;
		std::set<unsigned> singleCut;
		singleCut.insert(node);

		//printf("\nCURRENT AND NODE: %d\n", node);

		if(node > m_AIGraph->getInputSize()*2+1){
			//Fix Negative edges for cuts
			int node1, node2;
			node1 = m_AIGraph->getChild1(node);
			node2 = m_AIGraph->getChild2(node);
			node1 = node1&0xFFFFFFFE;
			node2 = node2&0xFFFFFFFE;

			//printf("cnodes: %d %d\n", node1, node2);
			//printCutSet_In(node1);
			//printCutSet_In(node2);

			merge(m_SubCut[node1], m_SubCut[node2], result);
		}

		result.push_back(singleCut);
		m_SubCut[node] = result;
		//printCutSet_In(node);

	}
	//printf("SIZE: %d\n", m_Cuts.size());
	//print();
}





/*******************************************************
 *  merge 
 *    Merges the cuts of the childs
 *
 *    @PARAMS: 
 *      * cutSet1: list of cuts of child1
 *      * cutSet2: list of cuts of child2
 *
 *    @RETURN: The merged list of cuts of the node
 ********************************************************/
void CutEnumeration::merge(std::list<std::set<unsigned> >& cutSet1, std::list<std::set<unsigned> >& cutSet2, std::list<std::set<unsigned> >& result){

	std::list<std::set<unsigned> >::iterator cut1;
	std::list<std::set<unsigned> >::iterator cut2;
	std::list<std::set<unsigned> >::iterator r;
	std::set<unsigned>::iterator iSet;
	std::set<unsigned>::iterator iSet2;

	std::map<unsigned, std::list<std::list<std::set<unsigned> >::iterator> > resultSigMap;

	//Merge the cuts from the two sets together (has to be unique)
	//printf("Traversing Cutset from both childs\n");
	for(cut1 = cutSet1.begin(); cut1 != cutSet1.end(); cut1++){
		unsigned signature1 = calculateSignature(*cut1); 

		for(cut2 = cutSet2.begin(); cut2 != cutSet2.end(); cut2++){
			//Calculate signature of cut
			unsigned signature2 = calculateSignature(*cut2); 

			//printf("Checking if sig is same...");



			//Check if it is K-Feasible, Not guaranteed. Need to check once more
			//printf("checking kfeasibilities...");
			unsigned sigOr= signature1 | signature2;
			unsigned kFeasibility = 0;
			unsigned mask = 1;
			for(unsigned int i = 0; i < 32; i++){
				kFeasibility+= ((sigOr & mask) > 0);
				mask = mask << 1;
			}
			//printf(" %d\n", kFeasibility);



			//Check to make sure the number of cuts is less than k
			if(kFeasibility <= m_K){
				//CUT1 OR CUT2
				std::set<unsigned> currentSet;
				currentSet.insert(cut1->begin(), cut1->end());
				currentSet.insert(cut2->begin(), cut2->end());
				//printf("Verifying Feasibility...");
				if(currentSet.size() > m_K) continue;
				//printf("verified\n");
			
			/*
			printf("\nCUT1:       ");
			printSet(*cut1);
			printf("CUT2:       ");
			printSet(*cut2);
			printf("CurrentSet: ");
			printSet(currentSet);
			*/

				bool isExists = false;
				unsigned resultSig = calculateSignature(currentSet);
			//printf("SIG1 : %8x\n", signature1);
			//printf("SIG2 : %8x\n", signature2);

				//See if set is already in result
				std::map<unsigned, std::list<std::list<std::set<unsigned> >::iterator> >::iterator iMap;
				std::list<std::list<std::set<unsigned> >::iterator>::iterator iListS;
				iMap = resultSigMap.find(resultSig);
				if(iMap != resultSigMap.end()){
					//Set in result already exists. Go to next set
					for(iListS = iMap->second.begin(); iListS != iMap->second.end(); iListS++){

						if(*(*iListS) == currentSet){
							isExists = true;
							break;
						}
					}
				}



				//Check for dominance and existing
					for(iMap = resultSigMap.begin(); iMap != resultSigMap.end(); iMap++){
						//Check for dominance
						unsigned sigAnd = resultSig & iMap->first;
						//printf("SIGCS: %8x\n", resultSig);
						//printf("SIGRS: %8x\n", iMap->first);
						
						//printf("Checking Dominance\n");
						if(sigAnd == resultSig && sigAnd == iMap->first){
							int foundDom = false;
							for(iListS = iMap->second.begin(); iListS != iMap->second.end(); iListS++){
								if((*iListS)->size() < currentSet.size()){
								unsigned count = 0; 
								for(iSet = currentSet.begin(); iSet != currentSet.end(); iSet++){
									if((*iListS)->find(*iSet) != (*iListS)->end())
										count++;
								}
								if(count == (*iListS)->size()){
									//printf("SIG2 DOM SIG1: Not storing cut in result\n");

									isExists = true;
									foundDom = true;
									break;
								}

								}
								else{
								unsigned count = 0; 
								for(iSet = (*iListS)->begin(); iSet != (*iListS)->end(); iSet++){
									if(currentSet.find(*iSet) != currentSet.end())
										count++;
								}

								if(count == (*iListS)->size()){
									//printf("SIG1 DOM SIG2\n");
									//printf("REMOVING SET FROM RESULT: ");
									printSet(*(*iListS));
									result.erase((*iListS));
									iMap->second.erase(iListS);
									break;
								}

								}
							}
							if(foundDom) break;

						}
						if(sigAnd == resultSig){
							//If contained within sig2 or sig1, check for dominance
							//printf("Keep currentSet...find dominating\n");
							for(iListS = iMap->second.begin(); iListS != iMap->second.end(); iListS++){
								unsigned count = 0; 
								for(iSet = (*iListS)->begin(); iSet != (*iListS)->end(); iSet++){
									if(currentSet.find(*iSet) != currentSet.end())
										count++;
								}

								if(count == (*iListS)->size()){
									//printf("SIG1 DOM SIG2\n");
									//printf("REMOVING SET FROM RESULT: ");
									printSet(*(*iListS));
									result.erase((*iListS));
									iMap->second.erase(iListS);
									break;
								}
							}

							if(isExists) break;
						}

						else if (sigAnd == iMap->first){
							//printf("See if existing set in result is dominating\n");
							int foundDom = false;
							for(iListS = iMap->second.begin(); iListS != iMap->second.end(); iListS++){
								unsigned count = 0; 
								for(iSet = currentSet.begin(); iSet != currentSet.end(); iSet++){
									if((*iListS)->find(*iSet) != (*iListS)->end())
										count++;
								}
								if(count == (*iListS)->size()){
									//printf("SIG2 DOM SIG1: Not storing cut in result\n");

									isExists = true;
									foundDom = true;
									break;
								}
							}
							if(foundDom) break;
						}
					}


				//If set does not exist, put it result
				if(!isExists){
					//printf("STORING CURRENT SET IN RESULTS\n");
					result.push_back(currentSet);
					r = result.end();
					r--;
					resultSigMap[resultSig].push_back(r);
				}
			}//end if

			//All data in cut is merged.
		}//end for cutset2
	}//end for cutset1
}

unsigned CutEnumeration::calculateSignature(std::set<unsigned>& cut){
	std::set<unsigned>::iterator iSet;
	unsigned signature = (unsigned) 0 ;
	unsigned const m = (unsigned) 32;
	for(iSet = cut.begin(); iSet != cut.end(); iSet++)
		signature = signature | ((unsigned)1<<((*iSet) % m));
	return signature;
}

void CutEnumeration::findInputCut(std::vector<unsigned>& nodes){
	printf("[CutEnumeration]--Find Input Based Cut\n");
	m_Cuts2.clear();

	for(unsigned int i = 0; i < nodes.size(); i++){
		//BFS
		std::list<unsigned int> queue;
		std::set<unsigned int> marked;
		queue.push_back(nodes[i]);
		marked.insert(nodes[i]);
		std::set<unsigned> inputs;

		while(queue.size() != 0){
			int item = queue.front();
			int node= item & 0xFFFFFFFE;
			queue.pop_front();

			//printf("NODE %d ITEM: %d\n", node, item);
			if(item <= ((int)m_AIGraph->getInputSize()*2+1)){
				//printf("INPUT!\n");
				inputs.insert(node);
				continue;
			}



			//Fix Negative edges for cuts
			unsigned node1, node2, child1, child2;
			child1 = m_AIGraph->getChild1(node);
			child2 = m_AIGraph->getChild2(node);

			node1 = child1 & 0xFFFFFFFE;
			node2 = child2 & 0xFFFFFFFE;


			if(marked.find(node1) == marked.end()){
				queue.push_back(child1);
				marked.insert(node1);
			}
			if(marked.find(node2) == marked.end()){
				queue.push_back(child2);
				marked.insert(node2);
			}

		}

		if(inputs.size() > 1)	
			m_Cuts2[nodes[i]] = inputs;
		//END BFS
	}

	std::map<unsigned, std::set<unsigned> >::iterator iMap;
	std::set<unsigned>::iterator iList;
/*
	for(iMap = m_Cuts2.begin(); iMap != m_Cuts2.end(); iMap++){
		printf("FFOUT: %5d Size: %3d\t", iMap->first, (int)iMap->second.size());
		for(iList = iMap->second.begin(); iList != iMap->second.end(); iList++){
			printf("%d ", *iList);
		}
		printf("\n");
	}
	*/
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     Getter Methods
####*
############################################################## 
##############################################################*/


/*******************************************************
 *  getCuts
 *    Gets the list of cuts for a given node
 *    Node is the Node ID/2
 *    
 *    @PARAMS
 *       node: The node to get the cuts from 
 *             MAKE SURE TO DIVIDE BY 2 to get the index
 *
 *    @RETURN: Pointer to the list of cuts
 ********************************************************/

void CutEnumeration::getCuts(unsigned node, std::list<std::set<unsigned> >& returnVal){
	returnVal = m_Cuts[node];
}


/*******************************************************
 *  getCuts
 *    Gets the list of cuts for a given node
 *    
 *    @PARAMS
 *       node: The node to get the cuts from
 *
 *    @RETURN: Pointer to the list of cuts
 ********************************************************/

void CutEnumeration::getCuts_In(unsigned node, std::list<std::set<unsigned> >& returnVal){
	returnVal = m_SubCut[node];
}



void CutEnumeration::getCut2(std::map<unsigned, std::set<unsigned> >& cut){
	cut = m_Cuts2;	
}


unsigned int CutEnumeration::getK(){
	return m_K;
}





/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     Printing Methods
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  print
 *    Prints the cut set for each node in the AIG
 ********************************************************/
void CutEnumeration::print(){
	printf("\n\n\n\n*************************************\n*\n");
	printf("* Printing Cuts\n*\n");
	printf("*************************************\n\n");

	std::list<std::set<unsigned> >::iterator r;
	std::set<unsigned>::iterator n;
	for(unsigned int i = 1; i < m_Cuts.size(); i++){ 
		printf("NODE %d:\tSIZE %d\n", i*2, (int)m_Cuts[i].size());
		for(r = m_Cuts[i].begin(); r != m_Cuts[i].end(); r++){
			n = r->begin();
			printf("{ %d", *n);
			n++;

			for(; n !=r->end(); n++)
				printf(", %d", *n); 

			printf(" }\n");
		}
		printf("\n");
	}
}



/*******************************************************
 *  printStat 
 *    Prints statistical information on the CE performed
 ********************************************************/
void CutEnumeration::printStat(){
	printf("\n\n\n\n*************************************\n*\n");
	printf("* Printing Cut Enumeration Statistics\n*\n");
	printf("*************************************\n\n");
	printf("K:\t\t\t%d\n", m_K);
	printf("Number of Nodes:\t%d\n", (int) m_Cuts.size());
	unsigned int numCuts = 0;
	for(unsigned int i = 2; i < m_Cuts.size(); i++){ 
		//printf("NODE %d\t\t%d Cuts\n", i, (int)m_Cuts[i]->size());
		numCuts+=m_Cuts[i].size();
	}
	printf("Number of Cuts:\t\t%d\n\n", numCuts);	
	printf("--------------------------------------------\n");
	printf("Number of AND Gates: %d\n", m_AIGraph->getSize());
	printf("Number of PI       : %d\n", m_AIGraph->getInputSize());
	printf("Number of PO       : %d\n", m_AIGraph->getOutputSize());
}


void CutEnumeration::printSet(std::set<unsigned>& s){
	std::set<unsigned>::iterator n;
	for(n = s.begin(); n !=s.end(); n++)
		printf("%d ", *n);

	printf("\n");
}









void CutEnumeration::printCutSet_In(unsigned node){
	std::list<std::set<unsigned> >::iterator r;
	std::set<unsigned>::iterator n;

	printf("NODE %d SIZE: %d:\n", node, (int)m_SubCut[node].size());

	for(r = m_SubCut[node].begin(); r != m_SubCut[node].end(); r++){
		n = r->begin();
		printf("{ %d", *n);
		n++;

		for(; n !=r->end(); n++)
			printf(", %d", *n); 

		printf(" }\n");
	}
	printf("\n");
}











/*
 * 
 * Prints the cut set given the ACTUAL AIG NODE
 *
 */
void CutEnumeration::printCutSet(unsigned node){
	std::list<std::set<unsigned> >::iterator r;
	std::set<unsigned>::iterator n;
	unsigned i = node/2;

	printf("NODE %d IN: %d:\n", node, i);
	for(r = m_Cuts[i].begin(); r != m_Cuts[i].end(); r++){
		n = r->begin();
		printf("{ %d", *n);
		n++;

		for(; n !=r->end(); n++)
			printf(", %d", *n); 

		printf(" }\n");
	}
	printf("\n");
}
