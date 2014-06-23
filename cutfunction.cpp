/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      cutfunction.cpp
  @      
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @      Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "cutfunction.hpp"



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
 *    Initializes the Assignment inputs for truth tables
 ********************************************************/
CutFunction::CutFunction(){
	//Assignment inputs
	m_Xval = new unsigned long[6];
	m_Xval[0] = 0x5555555555555555;
	m_Xval[1] = 0x3333333333333333;
	m_Xval[2] = 0x0F0F0F0F0F0F0F0F;
	m_Xval[3] = 0x00FF00FF00FF00FF;
	m_Xval[4] = 0x0000FFFF0000FFFF;
	m_Xval[5] = 0x00000000FFFFFFFF;
}



/*******************************************************
 *  Destructor
 *    Deletes allocated space.
 ********************************************************/
CutFunction::~CutFunction(){
	std::map<unsigned long, std::vector<std::vector<unsigned>*> >::iterator it;
	for(it = m_PortMap.begin(); it != m_PortMap.end(); it++){
		for(unsigned int i = 0; i < it->second.size(); i++){
			delete it->second[i];
		}
	}
	delete [] m_Xval;
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     Calculation methods
####*
############################################################## 
##############################################################*/


void CutFunction::setParams(CutEnumeration* ce, AIG* aig){
	m_CutEnumeration = ce;
	m_AIG = aig;

	m_FunctionCount.clear();
	m_NodeValue.clear();
	m_NodeFunction.clear();
	m_PortMap.clear();


}


/*******************************************************
 *  preProcessLibrary 
 *    Precomputes the truth table for library components
 *    Library components expected to be in AIG format. 
 *
 *  @PARAMS:
 *    fileName: File name that contains primitives
 ********************************************************/
void CutFunction::preProcessLibrary(std::string fileName){
	printf("[CutFunction]--Preprocessing library\n");

	std::ifstream infile;
	infile.open(fileName.c_str());
	if (!infile.is_open())	{
		fprintf(stderr, "[ERROR] -- Cannot open the database for import...exiting\n");
		fprintf(stderr, "\n***************************************************\n\n");
		exit(-1);
	}

	std::string file = "";
	while(getline(infile, file)){
		if(file == "\n")
			continue;
		printf("FILE: %s\n", file.c_str());
		m_AIG = new AIG();

		Graph* circuit = new Graph(file);
		circuit->importPrimitive(file, 0);

		m_AIG->convertGraph2AIG(circuit, false);

		//Get the Inputs to the primitive
		std::vector<int> inputs;
		circuit->getInputs(inputs);
		m_PrimInputSize[file] = inputs.size();

		/***************************************************
		 *
		 *    Preprocess library
		 *
		 *****************************************************/	

		//Make sure there is only one output in the library gate
		if(m_AIG->getOutputSize() != 1){
			//printf("AIG of %s does not have an output of 1...Skipping...\n", file.c_str());
			delete m_AIG;
			delete circuit;
			continue;
		}

		//Make sure there is no more than 5 inputs(32-bit truth table)
		unsigned int inputSize = m_AIG->getInputSize();
		if(inputSize > 4){
			printf("AIG of %s has more than 5 inputs...Skipping\n", file.c_str());
			delete m_AIG;
			delete circuit;
			continue;
		}


		//Permutation of indexes
		unsigned int* permutation = setPermutation(inputSize);


		do{
			/*printf("Permutation:\n");
			  for(unsigned int i = 0; i < inputSize; i++){
			  printf("%d ", permutation[i]);
			  }
			  printf("\n");*/

			unsigned int negationLimit = 1 << inputSize;
			for(unsigned int j = 0; j < negationLimit; j++){
				int count = j;

				//printf("\n\nInputSize: %d\n", inputSize);
				//Store the inputs
				for(unsigned int i = 1; i < inputSize+1; i++){
					unsigned int permVal = permutation[i-1];
					unsigned long input = m_Xval[permVal];

					if((count & 0x1) != 0)
						input = ~input;

					m_NodeValue[i*2] = input;
					//printf("Node %d\tVAL:%lx\n", i*2, input);
					count = count >> 1;		

				}


				std::vector<unsigned>output;
				m_AIG->getOutputs(output);

				//calculate value for output
				for(unsigned int i = 0; i < output.size(); i++){
					calculate(output[i]);

					//Make sure function is unique
					if(m_HashTable.find(m_NodeValue[output[i]]) != m_HashTable.end()){
						if(m_HashTable[m_NodeValue[output[i]]] != file){
							printf("Output currently already has a function assigned\n");
							printf("Current Function: %s\tFUNCTION: %s\n",file.c_str(),  m_HashTable[m_NodeValue[output[i]]].c_str());
							exit(1);
						}
					}

					//Store function with output
					m_HashTable[m_NodeValue[output[i]]] = file;
					//printf("OUTPUT: %lx\n", m_NodeValue[output[i]]);

					m_NodeValue.clear();
				}
			}
		}while(std::next_permutation(permutation, permutation+inputSize));

		delete [] permutation;
		delete m_AIG;
		delete circuit;
	}


	infile.close();
}





/*******************************************************
 *  processAIGCuts 
 *    Process each cut of each node
 *    Compares the result with library to see if there 
 *    is a match in function
 ********************************************************/
void CutFunction::processAIGCuts(bool np){
	printf("[CutFunction] -- Calculating Function of Cuts\n");

	//Go through each node and process all the cuts
	unsigned size = m_AIG->getSize() + m_AIG->getInputSize() + 1;
	for(unsigned int i = m_AIG->getInputSize()+1; i < size; i++){
		unsigned node = i * 2; //AIG FORMAT Evens are the nodes, Odds are inv 
		//printf("################################################################################\n");
		//printf("--------------------------------\nProcessing node %d\n", node);
		std::list<std::set<unsigned>*>* cutList;
		std::list<std::set<unsigned>*>::iterator cuts;

		cutList = m_CutEnumeration->getCuts(i);	

		//Skip if input
		if(cutList == NULL) 	continue;

		//Go through each cut of the node
		for(cuts = cutList->begin(); cuts != cutList->end(); cuts++){
			//Skip trivial cut
			unsigned int inputSize = (*cuts)->size();
			if(inputSize == 1)		continue;

			//Permutation of indexes
			unsigned int* permutation = setPermutation(inputSize);
			std::set<unsigned>::iterator cutIT;

			/*
			printf("CUTS: ");
			for(cutIT = (*cuts)->begin(); cutIT != (*cuts)->end(); cutIT++)
				printf("%d ", *cutIT);
			printf("\n");
			*/

			//P-Equivalence Check

			//do{
			//printf("Permutation:\n");

			//unsigned int negationLimit = 1 << inputSize;
			//for(unsigned int j = 0; j < negationLimit; j++){
			//int count = j;  //Counter for negation
			int pIndex= 0;	//Index for permutation

			std::vector <unsigned>* gateInputs = new std::vector<unsigned>();
			gateInputs->reserve(inputSize+1);

			//Set the assignments for the inputs
			for(cutIT = (*cuts)->begin(); cutIT != (*cuts)->end(); cutIT++){
				unsigned int permVal = permutation[pIndex];
				unsigned long input = m_Xval[permVal];

				//if((count & 0x1) != 0){
				//printf("-%d ", permutation[i]);
				//		input = ~input;
				//	}

				m_NodeValue[*cutIT] = input;
				//printf("CUT: %d\tIV: %lx\n", *cutIT, input);
				gateInputs->push_back(*cutIT);

				pIndex++;		
				//	count = count >> 1;		
			}


			//Calculate the output at each node up to the current node
			calculate(node);
			unsigned long functionVal = m_NodeValue[node];
			unsigned long negateVal = ~(functionVal); //N-Equivalence Check the negation of the output
			//printf("FUNCTION: %x\tNEGATE:: %x\n",  functionVal, negateVal);



			//Count the number of occurances for each unique function
			if(inputSize  > 2){
				//Store the function that's smaller, whether negate or nodevalue
				//unsigned long functionKey = negateVal;
				//if(functionVal < negateVal)
				unsigned long functionKey = functionVal;
				//	printf("KEY CHOSEN: %lx\n", functionKey);
				//	printf("FUNCTION: %lx\n", functionVal);
				//	printf("NEGATED:  %lx\n\n", negateVal);

				std::map<unsigned long, int>::iterator fcit;
				fcit = m_FunctionCount.find(functionKey);
				if(fcit == m_FunctionCount.end())
					m_FunctionCount[functionKey] = 1;						
				else
					fcit->second++;	
			}

			//Check to see if the function is a primitive/in the library
			if(m_HashTable.find(functionVal) == m_HashTable.end()){
				if(m_HashTable.find(negateVal) != m_HashTable.end()){

					if(m_PrimInputSize[m_HashTable[negateVal]] != gateInputs->size()){
						delete gateInputs;
						continue;
					}
					
					/*printf("Gateinputs pushed back:\t");
					for(unsigned int k = 0; k < gateInputs->size(); k++){
						printf("%d ", gateInputs->at(k));
					}
					printf("\nOutput: %d\n", node);
					*/
					
					m_NodeFunction[negateVal].insert(node);
					gateInputs->push_back(node);
					m_PortMap[negateVal].push_back(gateInputs);
				}
				else
					delete gateInputs;
			}
			else{
				if(m_PrimInputSize[m_HashTable[functionVal]] != gateInputs->size()){
					delete gateInputs;
					continue;
				}
				/*printf("Gateinputs pushed back\n");
					for(unsigned int k = 0; k < gateInputs->size(); k++){
						printf("%d ", gateInputs->at(k));
					}
					printf("\nOutput: %d\n", i);
					*/
					

				m_NodeFunction[functionVal].insert(node);
				gateInputs->push_back(node);
				m_PortMap[functionVal].push_back(gateInputs);
			}

			m_NodeValue.clear();

			//}while(std::next_permutation(permutation, permutation+inputSize));
			//delete [] permutation;
		}
		//printf("***************************************\n\n\n\n");
		}

		//std::map<unsigned int, std::string>::iterator it;
		//for(it = m_HashTable.begin(); it != m_HashTable.end(); it++){
		//printf("KEY: %x\t VAL: %s\n", it->first, it->second.c_str());
		//}

		/*
		   std::vector<int>output;
		   m_AIG->getOutputs(output);
		   printf("OUTPUT NODES:\t");
		   for(unsigned int i = 0; i < output.size(); i++){
		   printf("%d ", output[i]);
		   }
		   printf("\n\n");
		 */
	}









	/*******************************************************
	 *  calculate 
	 *    Calculates the result of the node of a specific cut 
	 *
	 *  @PARAMS:
	 *    node:    Node to calculate truth table for
	 *    aigraph: AIG to be used
	 *  @RETURN:   Function value at current node
	 ********************************************************/
	unsigned long CutFunction::calculate(int node){
		node = node & 0xFFFFFFFE;	
		//printf("CURRENT NODE: %d\n", node);

		//Check to see if primary input
		std::map<unsigned int, unsigned long>::iterator nvit;

		nvit = m_NodeValue.find((unsigned int) node);
		if(nvit != m_NodeValue.end()){
			return nvit->second;
		}

		//Fix Negative edges for cuts
		unsigned node1, node2, child1, child2;
		child1 = m_AIG->getChild1(node);
		child2 = m_AIG->getChild2(node);

		node1 = child1 & 0xFFFFFFFE;
		node2 = child2 & 0xFFFFFFFE;

		//printf("CHILDREG %u %u\n", child1, child2);
		//printf("CHILDPOS %u %u\n", node1, node2);

		//Recurse until primary input;
		unsigned long result1 = calculate(node1);
		unsigned long result2 = calculate(node2);

		//Handle Inverters
		if(child1 & 0x1)
			result1 = ~result1;
		if(child2 & 0x1)
			result2 = ~result2;

		//AND Operation
		//printf("Node %d IN: %d %d\n", node, child1, child2);
		//printf("c1: %8lx\n", result1);
		//printf("c2: %8lx\n", result2);
		unsigned long result = result1 & result2;
		//printf("r : %8lx\n", result);
		m_NodeValue[node] = result;

		return result;
	}











	/*******************************************************
	 *  setPermutation 
	 *    Prepares the permutation order 
	 *
	 *  @PARAMS:
	 *    inputSize: Size of permutation
	 ********************************************************/
	unsigned int* CutFunction::setPermutation(int inputSize){
		unsigned int* permutation = new unsigned int[inputSize];
		switch(inputSize){
			case 2:
				permutation[0] =0;
				permutation[1] =1;
				return permutation;
			case 3:
				permutation[0] =0;
				permutation[1] =1;
				permutation[2] =2;
				return permutation;
			case 4:
				permutation[0] =0;
				permutation[1] =1;
				permutation[2] =2;
				permutation[3] =3;
				return permutation;
			case 5:
				permutation[0] =0;
				permutation[1] =1;
				permutation[2] =2;
				permutation[3] =3;
				permutation[4] =4;
				return permutation;
			case 6:
				permutation[0] =0;
				permutation[1] =1;
				permutation[2] =2;
				permutation[3] =3;
				permutation[4] =4;
				permutation[5] =5;
				return permutation;
			default:
				printf("Input size not between 2-6\n");
				delete [] permutation;
				return NULL;
		}
	}





	/*******************************************************
	 *  printStat
	 *    Print out function calculation statistics 
	 *******************************************************/

	void CutFunction::printLibrary(){
		printf("\n\nPrinting library functions that were preprocessed\n");
		std::map<unsigned long, std::string>::iterator it;
		for(it = m_HashTable.begin(); it != m_HashTable.end(); it++){
			printf("FUNCTION: %lx\tPRIMITIVE: %s\n", it->first, it->second.c_str());
		}

	}




	/*******************************************************
	 *  printStat
	 *    Print out function calculation statistics 
	 *******************************************************/
	void CutFunction::printStat(){

	printf("\n\n*********************************\n");
	printf("*\n");
	printf("* Printing Boolean Matches\n");
	printf("*\n");
	printf("*********************************\n\n");
	std::map<unsigned long, std::set<unsigned> >::iterator it;
	for(it = m_NodeFunction.begin(); it != m_NodeFunction.end(); it++){
		//printf("FUNCTION: %s\t%lx\nNUMBER OF MATCH: %d\n",m_HashTable[it->first].c_str(), it->first, (unsigned int)  it->second.size());
		std::set<unsigned int>::iterator sit;

		//For each outputnode
		for(sit = it->second.begin(); sit != it->second.end(); sit++){
			//printf("NODE: %d\tIN:", (*sit)*2); //AIG NODES are even, ODD is inverse

		//Finding Inputs to function
		//Iterator for m_PortMap
		std::vector<std::vector<unsigned>*>* inputSet;
		inputSet = &m_PortMap[it->first];
		unsigned int inputSetSize = inputSet->size();

		for(unsigned int i = 0; i < inputSetSize; i++){
			unsigned output = inputSet->at(i)->at(inputSet->at(i)->size()-1); 
			//output *=2; //AIG NODES ARE EVEN

			//printf("Output: %d\n", output);
			if(output== (*sit)){
				for(unsigned int j = 0; j < inputSet->at(i)->size()-1; j++){
					printf("%3d ", inputSet->at(i)->at(j));
				}
				printf("\n");
				break;
			}
		}

	
		

		}
		printf("\n\n");

	}
		/*


		   printf("Printing port information\n");
		   std::map<unsigned int, std::map<int,std::string> >::iterator ppit;
		   std::map<unsigned int, std::vector<std::vector<int>*> >::iterator mit;
		   std::map<int, std::string>::iterator pit;


		   printf("PRIMITIVE\n");
		   for (ppit = m_PrimPortMap.begin(); ppit != m_PrimPortMap.end(); ppit++){
		   printf("FUNCTION: %X %s\n", ppit->first, m_HashTable[ppit->first].c_str());
		   for (pit = ppit->second.begin(); pit != ppit->second.end(); pit++){
		   printf("ORDER: %d\tPORT: %s\n", pit->first, pit->second.c_str());		
		   }
		   printf("\n");
		   }

		   printf("\n\n\n\n\n\n\nPORT MAP\n");
		   for (mit = m_PortMap.begin(); mit != m_PortMap.end(); mit++){
		   printf("FUNCTION: %X %s\n", mit->first, m_HashTable[mit->first].c_str());
		   for(unsigned int i = 0; i < mit->second.size(); i++){
		   for(unsigned int j = 0; j < mit->second[i]->size(); j++){
		   printf("ORDER: %d\tGATENUM: %d\n", j, mit->second[i]->at(j));
		   }
		   printf("\n");
		   }
		   printf("\n");
		   }
		 */
	}



	/*******************************************************
	 *  printStat
	 *    Print out function calculation statistics 
	 *******************************************************/
	void CutFunction::printFunctionCount(){
		printf("-------------------------------------\n");
		printf(" * Printing Function Count\n");
		printf("-------------------------------------\n");
		std::map<unsigned long, int>::iterator it;
		printf("Unique Functions----------%d\n\n", (int) m_FunctionCount.size());
		for(it = m_FunctionCount.begin(); it != m_FunctionCount.end(); it++){
			printf("Function: %16lx\t\tCount: %d\n", it->first, it->second);
		}
	}




	void CutFunction::getPortMap(std::map<unsigned long, std::vector<std::vector<unsigned>*> >& pmap){
		pmap =	m_PortMap;
	}


	void CutFunction::getHashMap(std::map<unsigned long, std::string>& hmap){
		hmap = m_HashTable;
	}

	void CutFunction::getFunctionCount(std::map<unsigned long, int>& fc){
		fc = m_FunctionCount;	
	}

	
/*
	void CutFunction::processLUTs(Graph* ckt, std::map<std::string, std::set<unsigned long> >& pDatabase){
		printf("\n\n\n\nPROCESSING LUTs...\n");
		std::map<int, Vertex<std::string>*>::iterator it;
		//Loop through graph to find LUTs
		for(it = ckt->begin(); it!=ckt->end(); it++){
			//printf("Graph Node : %d\n", it->first);
			//If vertex type is a LUT
			if(it->second->getType().find("LUT") != std::string::npos){
				unsigned long function = it->second->getLUT();

				unsigned int inputSize = it->second->getNumInputs();	

				unsigned int* permutation = setPermutation(inputSize);
				if(permutation == NULL)
					continue;

				printf("LUT FUNCTION: %lx\t LUT SIZE: %d\n", function, inputSize);

				//FORM TRUTH TABLE
				std::map<std::string, unsigned int> truthTable;
				std::stringstream ss;
				unsigned int loopSize = 1 << inputSize ;
*/
				/*for(unsigned int i = 0; i < 64; i++){
				  unsigned int tmpi = i; 
				  for(unsigned int j = 0; j < 6; j++){
				  if(tmpi & 0x20) ss<<1;
				  else ss<<0;

				  tmpi = tmpi << 1;
				  }

				  if(i == loopSize){
				  function = it->second->getLUT();
				  loopSize = loopSize + (1 << inputSize);
				  }

				  truthTable[ss.str()] = function & 0x1;

				  printf("%s bit: %d\n", ss.str().c_str(), function & 0x1);
				  function = function >> 1;
				  ss.str("");
				  }*/
/*
				//Make Truth Table
				unsigned int mask = 1 << (inputSize -1);
				for(unsigned int i = 0; i < loopSize; i++){
					unsigned int tmpi = i; 
					//Make the string for input values
					for(unsigned int j = 0; j < inputSize; j++){
						if(tmpi & mask) ss<<1;
						else ss<<0;

						tmpi = tmpi << 1;
					}

					//Assign the output bit for the specific input pattern. 
					truthTable[ss.str()] = function & 0x1;

					printf("%s bit: %ld\n", ss.str().c_str(), (function & 0x1));
					function = function >> 1;
					ss.str("");
				}


				//Permute the inputs to find corresponding output bit. 
				std::set<unsigned long> LUTDatabase;
				do{
					//printf("Permutation:\n");

					unsigned int negationLimit = 1 << inputSize;
					for(unsigned int j = 0; j < negationLimit; j++){
						std::vector<unsigned long> truthTableInput;
						int count = j; 
						for(unsigned int i = 0; i < inputSize; i++){
							unsigned int permVal = permutation[i];
							unsigned long input = m_Xval[permVal];

							if((count & 0x1) != 0){
								//printf("-%d ", permutation[i]);
								input = ~input;
							}
							//else
							//printf("%d ", permutation[i]);

							truthTableInput.push_back(input);
							count = count >> 1;		
						}
						//printf("\nInput:\n");


						unsigned int mask = 0x1;
						unsigned int pFunction = 0;;
						for(unsigned int i = 0; i < loopSize; i++){
							ss.str("");
							for(unsigned int k = 0; k < inputSize; k++){
								ss<<(truthTableInput[k] & mask);
								truthTableInput[k] = truthTableInput[k] >> 1;
							}
							pFunction = pFunction << 1;
							pFunction = truthTable[ss.str()] | pFunction;
							//printf("%s VAL: %d FUNC%x\n", ss.str().c_str(), truthTable[ss.str()], pFunction);
						}

						//printf("PERMUTED FUNCTION: %x\n", pFunction);
						unsigned long pFunction64 = 0;
						for(unsigned int i = 0; i < 64/loopSize; i++){
							pFunction64 = pFunction64<<loopSize;
							pFunction64 = pFunction64 | pFunction;
						}

						pDatabase[ckt->getName()].insert(pFunction64);

					}
					//printf("\n");
				}while(std::next_permutation(permutation, permutation+inputSize));

				delete permutation;

			}
		}
		*/
		/*
		   std::map<std::string, std::set<unsigned long> >::iterator pit;
		   std::set<unsigned long>::iterator sit;
		   for(pit = pDatabase.begin(); pit != pDatabase.end(); pit++){
		   printf("CIRCUIT: %s\n", pit->first.c_str());
		   for(sit = pit->second.begin(); sit != pit->second.end(); sit++){
		   printf("%lx\n", *sit);
		   }
		   printf("\n");
		   }
	}
	*/

