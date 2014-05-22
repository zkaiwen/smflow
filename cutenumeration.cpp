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


	//Pre-allocate space for the cutset for each node
	m_Cuts.resize(m_AIGraph->getSize()+m_AIGraph->getInputSize()+1, NULL);

	for(unsigned int i = 0; i < 2; i++){
		std::set<unsigned>* singleCut = new std::set<unsigned>();
		singleCut->insert(i);
		std::list<std::set<unsigned>*>* singleCutSet;
		singleCutSet = new std::list<std::set<unsigned>*>();
		singleCutSet->push_back(singleCut);
		m_Cuts[i] = singleCutSet;
	}

	//printf("CUT RESIZE: %d\n", m_AIGraph->getSize()+m_AIGraph->getInputSize()+1);
}



/*******************************************************
 *  Destructor
 *    Deletes allocated space. Does not delete AIG Graph
 ********************************************************/
CutEnumeration::~CutEnumeration(){
	std::list<std::set<unsigned>*>::iterator r;
	for(unsigned int i = 0; i < m_Cuts.size(); i++){ 
		if(m_Cuts[i] == NULL)
			continue;

		for(r = m_Cuts[i]->begin(); r != m_Cuts[i]->end(); r++)
			delete (*r);

		delete m_Cuts[i];
	}
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

	//Process trivial cuts for inputs
	//m_AIGraph->print();
	for(unsigned int i = 1; i < m_AIGraph->getInputSize()+1; i++){
		unsigned node = i*2;
		//printf("CURRENT IN NODE: %d\n", node);
		std::list<std::set<unsigned>*>* singleCutSet;
		std::set<unsigned>* singleCut = new std::set<unsigned>();
		singleCut->insert(node);

		//printf("SINGLE CUT LIST: %d\n", node);
		singleCutSet = new std::list<std::set<unsigned>*>();
		singleCutSet->push_back(singleCut);
		m_Cuts[i] = singleCutSet;
	}

	unsigned int size = m_AIGraph->getSize()+m_AIGraph->getInputSize()+1;
	for(unsigned int i = (m_AIGraph->getInputSize()+1); i < size; i++){
		unsigned node = 2*i;
		std::set<unsigned>* singleCut = new std::set<unsigned>();
		singleCut->insert(node);
		//printf("CURRENT AND NODE: %d\n", node);

		//Fix Negative edges for cuts
		int node1, node2;
		node1 = m_AIGraph->getChild1(node);
		node2 = m_AIGraph->getChild2(node);
		node1 = node1&0xFFFFFFFE;
		node2 = node2&0xFFFFFFFE;

		//printf("cnodes: %d %d\n", node1, node2);
		//printCutSet(node1);
		//printCutSet(node2);

		std::list<std::set<unsigned>*>* result;
		result = merge(m_Cuts[node1/2], m_Cuts[node2/2]);
		result->push_back(singleCut);
		m_Cuts[i] = result;
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
std::list<std::set<unsigned>*>* CutEnumeration::merge(std::list<std::set<unsigned>*>* cutSet1, std::list<std::set<unsigned>*>* cutSet2){

	std::list<std::set<unsigned>*>* result;
	result = new std::list<std::set<unsigned>*>();

	std::list<std::set<unsigned>*>::iterator cut1;
	std::list<std::set<unsigned>*>::iterator cut2;
	std::list<std::set<unsigned>*>::iterator r;
	std::set<unsigned>::iterator setit1;
	std::set<unsigned>::iterator setit2;

	//Merge the cuts from the two sets together (has to be unique)
	//printf("Traversing Cutset from both childs\n");
	for(cut1 = cutSet1->begin(); cut1 != cutSet1->end(); cut1++){
		for(cut2 = cutSet2->begin(); cut2 != cutSet2->end(); cut2++){
			//CUT1 OR CUT2
			std::set<unsigned>* currentSet = new std::set<unsigned>();

			currentSet->insert((*cut1)->begin(), (*cut1)->end());
			currentSet->insert((*cut2)->begin(), (*cut2)->end());

			//printf("CurrentSet\n");
			//printSet(currentSet);

			//Check to make sure the number of cuts is less than k
			if(currentSet->size() <= m_K){
				bool isExists = false;

				//Add the cut to result if the cut does not already exist
				for(r = result->begin(); r != result->end(); r++){
					//If the two sets are not the same size
					//They cannot be equal
					if((*r)->size() != currentSet->size())
						continue;


					bool mismatch = false;
					setit1 = (*r)->begin();
					setit2 = currentSet->begin();
					while(setit1 != (*r)->end()){
						if(*setit1 != *setit2){
							mismatch = true;
							break;
						}
						setit1++;
						setit2++;
					}

					//Set in result already exists. Go to next set
					if(!mismatch){
						delete currentSet;
						isExists = true;
						break;
						//printf("CUT ALREADY EXISTS\n");
					}
				}

				//If set does not exist, put it result
				if(!isExists)
					result->push_back(currentSet);

			}//end if
			else
				delete currentSet;

			//All data in cut is merged.
		}//end for cutset2

	}//end for cutset1


	return result;
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
 *    
 *    @PARAMS
 *       node: The node to get the cuts from
 *
 *    @RETURN: Pointer to the list of cuts
 ********************************************************/

std::list<std::set<unsigned>*>* CutEnumeration::getCuts(unsigned node){
	return m_Cuts[node];
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

	std::list<std::set<unsigned>*>::iterator r;
	std::set<unsigned>::iterator n;
	for(unsigned int i = 1; i < m_Cuts.size(); i++){ 
		printf("NODE %d:\n", i*2);
		for(r = m_Cuts[i]->begin(); r != m_Cuts[i]->end(); r++){
			n = (*r)->begin();
			printf("{ %d", *n);
			n++;

			for(; n !=(*r)->end(); n++)
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
		if(m_Cuts[i] == NULL){
			//printf("NODE %d\t\tNo Cuts, INPUT\n", i);
			continue;
		}
		//printf("NODE %d\t\t%d Cuts\n", i, (int)m_Cuts[i]->size());
		numCuts+=m_Cuts[i]->size();
	}
	printf("Number of Cuts:\t\t%d\n\n", numCuts);	
}


void CutEnumeration::printSet(std::set<unsigned>* s){
	std::set<unsigned>::iterator n;
	for(n = s->begin(); n !=s->end(); n++)
		printf("%d ", *n);

	printf("\n");
}





















void CutEnumeration::printCutSet(unsigned node){
	std::list<std::set<unsigned>*>::iterator r;
	std::set<unsigned>::iterator n;
	unsigned i = node/2;
	if(m_Cuts[i] == NULL){
		printf("NODE %d\tINPUT\n", i);
		return;
	}

	printf("NODE %d IN: %d:\n", node, i);
	for(r = m_Cuts[i]->begin(); r != m_Cuts[i]->end(); r++){
		n = (*r)->begin();
		printf("{ %d", *n);
		n++;

		for(; n !=(*r)->end(); n++)
			printf(", %d", *n); 

		printf(" }\n");
	}
	printf("\n");
}
