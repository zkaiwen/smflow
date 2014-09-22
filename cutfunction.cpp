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
	m_Xval = new unsigned long long[6];
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
	std::map<unsigned long long, std::vector<std::vector<unsigned>*> >::iterator it;
	for(it = m_PortMap.begin(); it != m_PortMap.end(); it++){
		for(unsigned int i = 0; i < it->second.size(); i++){
			delete it->second[i];
		}
	}
	
	for(it = m_PortMap_DC.begin(); it != m_PortMap_DC.end(); it++){
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
	m_PortMap_DC.clear();


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

		printf("\n--------------------------------------------------------------------\n");
		printf("FILE: %s\n", file.c_str());
		m_AIG = new AIG();

		Graph* circuit = new Graph(file);
		circuit->importGraph(file, 0);
		m_AIG->convertGraph2AIG(circuit, false);



		//Get the Inputs to the primitive
		std::vector<int> inputs;
		circuit->getInputs(inputs);
		//m_PrimInputSize[file] = inputs.size();

		/***************************************************
		 *
		 *    Preprocess library
		 *
		 *****************************************************/	

		//Make sure there is only one output in the library gate
		if(m_AIG->getOutputSize() != 1){
			printf("AIG of %s does not have an output of 1...Skipping...\n", file.c_str());
			delete m_AIG;
			delete circuit;
			continue;
		}

		//Make sure there is no more than 5 inputs(32-bit truth table)
		unsigned int inputSize = m_AIG->getInputSize();
		if(inputSize > 6){
			printf("AIG of %s has more than 6 inputs...Skipping\n", file.c_str());
			delete m_AIG;
			delete circuit;
			continue;
		}

		if(file.find("CuT") != std::string::npos){
			preProcessCut(file);
		}
		else{
			//Permutation of indexes
			unsigned int* permutation = setPermutation(inputSize);
			unsigned entriesAdded = 0;
			std::vector<unsigned>output;
			m_AIG->getOutputs(output);
			m_PrimInputSize[file] = inputSize;

			do{
				/*printf("Permutation:\n");
					for(unsigned int i = 0; i < inputSize; i++){
					printf("%d ", permutation[i]);
					}
					printf("\n");*/

				unsigned int negationLimit = 1 << inputSize;
				for(unsigned int j = 0; j < negationLimit; j++){
					int count = j;

					//Store the inputs
					m_NodeValue.clear();
					for(unsigned int i = 1; i < inputSize+1; i++){
						unsigned int permVal = permutation[i-1];
						unsigned long long input = m_Xval[permVal];

						if((count & 0x1) != 0)
							input = ~input;

						m_NodeValue[i*2] = input;
						//printf("Node %d\tVAL:%llx\n", i*2, input);
						count = count >> 1;		
					}


					calculate(output[0]);
					unsigned long long result = m_NodeValue[output[0]];
					std::map<unsigned long long, std::string>::iterator iHash = m_HashTable.find(result);

					//Make sure function is unique
					if(iHash != m_HashTable.end()){
						if(iHash->second != file){
							printf("Output currently already has a function assigned\n");
							printf("Current Function: %s\tFUNCTION: %s\n",file.c_str(),  iHash->second.c_str());
							continue;
							//exit(1);
						}
					}

					//Store function with output
					m_HashTable[result] = file;
					entriesAdded++;
					//printf("OUTPUT: %llx\n", m_NodeValue[output[i]]);
				}
			}while(std::next_permutation(permutation, permutation+inputSize));
			printf(" * %d functions added to Hashtable\n", entriesAdded);

			delete [] permutation;
		}

		delete m_AIG;
		delete circuit;
	}


	infile.close();
	m_NodeValue.clear();
}

void CutFunction::preProcessCut(std::string file){
	std::vector<unsigned> aigOut;
	m_AIG->getOutputs(aigOut);

	CutEnumeration* cut = new CutEnumeration (m_AIG);
	cut->findKFeasibleCuts(6);
	//cut->print();

	unsigned node = aigOut[0]; //AIG FORMAT Evens are the nodes, Odds are inv 
	//printf("################################################################################\n");
	//printf("--------------------------------\nProcessing node %d\n", node);
	std::list<std::set<unsigned> > cutList;
	std::list<std::set<unsigned> >::iterator cuts;

	cut->getCuts(node/2, cutList);	


	//Go through each cut of the node
	unsigned entriesAdded = 0;
	std::set<unsigned>::iterator cutIT;

	for(cuts = cutList.begin(); cuts != cutList.end(); cuts++){
		//Skip trivial cut
		unsigned int inputSize = cuts->size();
		if(inputSize < m_AIG->getInputSize()-1)		continue;

		//Make sure the cut has at least cutsize/2+1 inputs
		int inputLimit = 1;
		printf("CUT: ");
		int numInputPort = 0;
		for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
			if(*cutIT <= m_AIG->getInputSize()*2)
				numInputPort++;

			printf("%d ", *cutIT);
		}
		/*

		if(numInputPort < inputLimit){
			printf(".....CUT DOES NOT HAVE ENOUGH INPUTS: %d\n", numInputPort);
			continue;
		}
		*/

		std::stringstream ss;
		ss<<file<<"_"<<cuts->size();
		m_PrimInputSize[ss.str()] = inputSize;

		//Permutation of indexes
		unsigned int* permutation = setPermutation(inputSize);

		bool isFuncExist = false;
		do{
			unsigned int negationLimit = 1 << inputSize;
			for(unsigned int j = 0; j < negationLimit; j++){

				//Set the assignments for the inputs
				int pIndex= 0;	//Index for permutation
				int count = j;
				m_NodeValue.clear();
				for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
					unsigned int permVal = permutation[pIndex];
					unsigned long long input = m_Xval[permVal];

					if((count & 0x1) != 0)
						input = ~input;

					m_NodeValue[*cutIT] = input;

					pIndex++;		
					count = count >> 1;		
				}


				//Calculate the output at each node up to the current node
				calculate(node);
				unsigned long long functionVal = m_NodeValue[node];

				//Make sure function is unique
				if(m_HashTable.find(functionVal) != m_HashTable.end()){
					if(m_HashTable[functionVal] != ss.str()){
						//printf("Output currently already has a function assigned\n");
						//printf("Current Function: %s\tFUNCTION: %s\n",file.c_str(),  m_HashTable[functionVal].c_str());
						isFuncExist = true;
						break;
						//exit(1);
					}
				}

				//Store function with output
				m_HashTable[functionVal] = ss.str();
				entriesAdded++;
			}
			if(isFuncExist) break;
		}while(std::next_permutation(permutation, permutation+inputSize));

		if(isFuncExist) printf("...exisitng function found");
		printf("\n");
		delete [] permutation;
	}

	printf(" * %d functions added to Hashtable\n", entriesAdded);

	delete cut;
	m_NodeValue.clear();

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
		std::list<std::set<unsigned> > cutList;
		std::list<std::set<unsigned> >::iterator cuts;

		m_CutEnumeration->getCuts(i, cutList);	

		//Go through each cut of the node
		for(cuts = cutList.begin(); cuts != cutList.end(); cuts++){
			//Skip trivial cut
			unsigned int inputSize = cuts->size();
			if(inputSize == 1)		continue;

			//Permutation of indexes
			unsigned int* permutation = setPermutation(inputSize);
			std::set<unsigned>::iterator cutIT;
			int pIndex= 0;	//Index for permutation

			//Set the assignments for the inputs
			for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
				unsigned int permVal = permutation[pIndex];
				unsigned long long input = m_Xval[permVal];

				m_NodeValue[*cutIT] = input;
				pIndex++;		
				//printf("CUT: %d\tIV: %llx\n", *cutIT, input);
			}
			/*
				 printf("\nCUT: ");
				 for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++)
				 printf("%d ", *cutIT);
				 printf("\n");
			 */


			//Calculate the output at each node up to the current node
			calculate(node);
			unsigned long long functionVal = m_NodeValue[node];
			unsigned long long negateVal = ~(functionVal); //N-Equivalence Check the negation of the output
			//printf("FUNCTION: %llx\tNEGATE:: %llx\n",  functionVal, negateVal);

			//Check to see if the function is a primitive/in the library
			unsigned long long function;
			bool functionFound = false;

			std::map<unsigned long long, std::string>::iterator iHash;
			iHash = m_HashTable.find(functionVal);

			if(iHash == m_HashTable.end()){
				//Check the negated functionvalue
				iHash = m_HashTable.find(negateVal);
				if(iHash != m_HashTable.end()){
					functionFound = true;
					function = negateVal;
				}
			}
			else{
				functionFound = true;
				function = functionVal;
			}

			if(functionFound){
				if(m_PrimInputSize[iHash->second] != cuts->size())
					continue;

				//Create gateInputs
				std::vector<unsigned>* gateInputs = new std::vector<unsigned>();
				for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++)
					gateInputs->push_back(*cutIT);

				m_NodeFunction[function].insert(node);
				gateInputs->push_back(node);
				m_PortMap[function].push_back(gateInputs);
			}

			m_NodeValue.clear();
			delete permutation;
		}
	}
}


/*******************************************************
 *  processAIGCuts 
 *    Process each cut of each node
 *    Compares the result with library to see if there 
 *    is a match in function
 ********************************************************/
void CutFunction::processAIGCutsX(bool np){
	printf("[CutFunction] -- Calculating Function of Cuts..K=%d\n", m_CutEnumeration->getK() );

	//Go through each node and process all the cuts
	unsigned size = m_AIG->getSize() + m_AIG->getInputSize() + 1;
	for(unsigned int i = m_AIG->getInputSize()+1; i < size; i++){
		unsigned node = i * 2; //AIG FORMAT Evens are the nodes, Odds are inv 
		//printf("#########################################################################################################\n");
		//printf("#########################################################################################################\n");
		//printf("--------------------------------\nProcessing node %d\n", node);
		std::list<std::set<unsigned> > cutList;
		std::list<std::set<unsigned> >::iterator cuts;

		m_CutEnumeration->getCuts(i, cutList);	

		//Go through each cut of the node
		for(cuts = cutList.begin(); cuts != cutList.end(); cuts++){
			
			//Skip trivial cut
			unsigned int inputSize = cuts->size();
			if(inputSize == 1)		continue;

			//Permutation of indexes
			unsigned int* permutation = setPermutation(inputSize);
			std::set<unsigned>::iterator cutIT;
			unsigned int pIndex= 0;	//Index for permutation

			//Set the assignments for the inputs
			m_NodeValue.clear();
			for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
				unsigned int permVal = permutation[pIndex];
				unsigned long long input = m_Xval[permVal];

				m_NodeValue[*cutIT] = input;
				pIndex++;		
				//printf("CUT: %d\tIV: %llx\n", *cutIT, input);
			}

			//Calculate the output at each node up to the current node
			calculate(node);
			unsigned long long functionVal = m_NodeValue[node];
			unsigned long long negateVal = ~(functionVal); //N-Equivalence Check the negation of the output
			//printf("FUNCTION: %llx\tNEGATE:: %llx\n",  functionVal, negateVal);

			std::set<unsigned long long> existingFunctionFound; 
			existingFunctionFound.insert(functionVal);
			existingFunctionFound.insert(negateVal);
			std::set<std::string> existingFunctionName;

			//Check to see if the function is a primitive/in the library
			unsigned long long function;
			bool functionFound = false;

			std::map<unsigned long long, std::string>::iterator iHash;
			iHash = m_HashTable.find(functionVal);

			if(iHash == m_HashTable.end()){
				//Check the negated functionvalue
				iHash = m_HashTable.find(negateVal);
				if(iHash != m_HashTable.end()){
					functionFound = true;
					function = negateVal;
				}
			}
			else{
				functionFound = true;
				function = functionVal;
			}

			if(functionFound){
				if(m_PrimInputSize[iHash->second] != cuts->size())
					continue;
				existingFunctionName.insert(iHash->second);

				//Create gateInputs
				std::vector<unsigned>* gateInputs = new std::vector<unsigned>();
				for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++)
					gateInputs->push_back(*cutIT);

				m_NodeFunction[function].insert(node);
				gateInputs->push_back(node);
				m_PortMap[function].push_back(gateInputs);
			}

			delete permutation;


			//***************************************************************************
			//***************************************************************************
			//Don't care test
			//Permutation of indexes
			inputSize = cuts->size() - 1;
			if(inputSize == 1)		continue;

			permutation = setPermutation(inputSize);
			unsigned long long dc[2] = {
				0x0000000000000000, 
				0xFFFFFFFFFFFFFFFF
			};

			//printf("\nDONT CARE!-------------------------------------------------\n");
			for(unsigned dcIndex = 0; dcIndex < cuts->size(); dcIndex++){
				//printf("DC INDEX: %d\n", dcIndex);
				for(unsigned w = 0; w < 2; w++){
					pIndex= 0;	//Index for permutation
					unsigned int index = 0; 


					//Set the assignments for the inputs
					m_NodeValue.clear();
					for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
						unsigned long long input; 
						if(index == dcIndex)
							input = dc[w];
						else{
							unsigned int permVal = permutation[pIndex];
							input = m_Xval[permVal];
							pIndex++;		
						}

						index++;
						m_NodeValue[*cutIT] = input;
						//printf("CUT: %d\tIV: %llx\n", *cutIT, input);
					}

					//Calculate the output at each node up to the current node
					calculate(node);
					unsigned long long functionVal = m_NodeValue[node];
					unsigned long long negateVal = ~(functionVal); //N-Equivalence Check the negation of the output
					//printf("FUNCTION: %llx\tNEGATE:: %llx\n",  functionVal, negateVal);

					if(existingFunctionFound.find(functionVal) != existingFunctionFound.end()){
						//printf("SKIPPED-----------\n");
						continue;
					}
					if(existingFunctionFound.find(negateVal) != existingFunctionFound.end()){
						//printf("SKIPPED-----------\n");
						continue;
					}

					existingFunctionFound.insert(functionVal);
					existingFunctionFound.insert(negateVal);


					//Check to see if the function is a primitive/in the library
					unsigned long long function;
					bool functionFound = false;

					std::map<unsigned long long, std::string>::iterator iHash;
					iHash = m_HashTable.find(functionVal);


					if(iHash == m_HashTable.end()){
						//Check the negated functionvalue
						iHash = m_HashTable.find(negateVal);
						if(iHash != m_HashTable.end()){
							functionFound = true;
							function = negateVal;
						}
					}
					else{
						functionFound = true;
						function = functionVal;
					}

					if(functionFound){
						if(existingFunctionName.find(iHash->second) != existingFunctionName.end())
							continue;

						if(m_PrimInputSize[iHash->second] != cuts->size()-1)
							continue;
						//printf("EXISTING FUNCTION FOUND: %s\n", iHash->second.c_str());

						existingFunctionName.insert(iHash->second);

						//Create gateInputs
						std::vector<unsigned>* gateInputs = new std::vector<unsigned>();
						pIndex = 0; 
						for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
							//TODO:if(pIndex != dcIndex)
								gateInputs->push_back(*cutIT);
							pIndex++;
						}

						m_NodeFunction[function].insert(node);
						gateInputs->push_back(node);
						m_PortMap_DC[function].push_back(gateInputs);
						//printf("PORT MAP STORED****************************\n");
					}

				}

			}
			//printf("END DONT CARE COMPUTATION\n\n");
			delete permutation;
		}
	}

	m_NodeValue.clear();
}

/*******************************************************
 *  processAIGCuts_Perm
 *    Process each cut of each node
 *    Compares the result with library to see if there
 *    is a match in function
 *
 *    Takes into account permutation at this level
 ********************************************************/
void CutFunction::processAIGCuts_Perm(bool np){
	printf("[CutFunction] -- Calculating Function of Cuts Permutation Mode\n");

	//Go through each node and process all the cuts
	unsigned size = m_AIG->getSize() + m_AIG->getInputSize() + 1;
	for(unsigned int i = m_AIG->getInputSize()+1; i < size; i++){
		unsigned node = i * 2;
		//printf("############################################################################################################\n");
		//printf("--------------------------------\nProcessing node %d\n", node);
		std::list<std::set<unsigned> > cutList;
		std::list<std::set<unsigned> >::iterator cuts;

		m_CutEnumeration->getCuts(i, cutList); 

		//Go through each cut of the node
		for(cuts = cutList.begin(); cuts != cutList.end(); cuts++){

			//Skip trivial cut
			unsigned int inputSize = cuts->size();
			if(inputSize < 5)
				continue;

			//Permutation of indexes
			unsigned int* permutation = setPermutation(inputSize);

			std::set<unsigned>::iterator cutIT;
			//printf("NODE %d\t", i);

			//printf("CUT: ");
			std::vector<unsigned> gateInput;
			for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++)
			{
				//printf("%d ", *cutIT);
				gateInput.push_back(*cutIT);
			}

			gateInput.push_back(node);
			//printf("\n");

			//P-Equivalence Check
			//Permutation of indexes

			std::set<unsigned long long> keyVals;

			do{
				/*printf("Permutation:\n");
					for(unsigned int i = 0; i < inputSize; i++){
					printf("%d ", permutation[i]);
					}
					printf("\n");
				 */

				int pIndex= 0;  //Index for permutation

				//printf("\n\nInputSize: %d\n", inputSize);
				//Set the assignments for the inputs
				for(cutIT = cuts->begin(); cutIT != cuts->end(); cutIT++){
					unsigned int permVal = permutation[pIndex];
					unsigned long long input = m_Xval[permVal];

					m_NodeValue[*cutIT] = input;
					m_NodeValueIn[*cutIT] = input;
					//printf("CUT: %d\tIV: %llx\n", *cutIT, input);

					pIndex++;      
				}

				//Calculate the output at each node up to the current node
				calculate2(node);
				unsigned long long functionVal = m_NodeValue[node];
				unsigned long long negateVal = ~(functionVal);
				//printf("OUTPUT: %x\n", negate);

				//N-Equivalence Check the negation of the output
				//Count the number of occurances for each unique function
				//Store the function that's smaller, whether negate or nodevalue
				unsigned long long functionKey = functionVal;
				if(negateVal < functionVal)
					functionKey = negateVal;

				//printf("KEY CHOSEN: %llx\n", functionKey);
				keyVals.insert(functionKey);
				//  printf("FUNCTION: %llx\n", functionVal);
				//  printf("NEGATED:  %llx\n\n", negateVal);

				std::map<unsigned long long, int>::iterator fcit;
				fcit = m_FunctionCount.find(functionKey);
				if(fcit == m_FunctionCount.end())
					m_FunctionCount[functionKey] = 1;                      
				else
					fcit->second++; 

				m_NodeValue.clear();
				m_NodeValueIn.clear();

			}while(std::next_permutation(permutation, permutation+inputSize));
			delete [] permutation;

			m_UniqueFunction[keyVals].push_back(gateInput);
		}

	}
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
unsigned long long CutFunction::calculate(unsigned node){
	node = node & 0xFFFFFFFE;	
	//printf("CURRENT NODE: %d\n", node);

	//Check to see if primary input
	std::map<unsigned int, unsigned long long>::iterator nvit;

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
	unsigned long long result1 = calculate(node1);
	unsigned long long result2 = calculate(node2);

	//Handle Inverters
	if(child1 & 0x1)
		result1 = ~result1;
	if(child2 & 0x1)
		result2 = ~result2;

	//AND Operation
	//printf("Node %d IN: %d %d\n", node, child1, child2);
	//printf("c1: %8llx\n", result1);
	//printf("c2: %8llx\n", result2);
	unsigned long long result = result1 & result2;
	//printf("r : %8llx\n\n", result);
	m_NodeValue[node] = result;

	return result;
}






/*******************************************************
 *  calculate 
 *    Calculates the result of the node of a specific cut 
 *    This calculate ignores negation at inputs
 *
 *  @PARAMS:
 *    node:    Node to calculate truth table for
 *    aigraph: AIG to be used
 *  @RETURN:   Function value at current node
 ********************************************************/
unsigned long long CutFunction::calculate2(unsigned node){
	node = node & 0xFFFFFFFE;	
	//printf("CURRENT NODE: %d\n", node);

	//Check to see if primary input
	std::map<unsigned int, unsigned long long>::iterator nvit;

	nvit = m_NodeValue.find((unsigned int) node);
	if(nvit != m_NodeValue.end()){
		//printf("VAL FOUND: %8llx\n", nvit->second);
		return nvit->second;
	}

	//Fix Negative edges for cuts
	unsigned node1, node2, child1, child2;
	child1 = m_AIG->getChild1(node);
	child2 = m_AIG->getChild2(node);

	node1 = child1 & 0xFFFFFFFE;
	node2 = child2 & 0xFFFFFFFE;


	//Recurse until primary input;
	unsigned long long result1 = calculate2(node1);
	unsigned long long result2 = calculate2(node2);

	//Handle Inverters only at points not at the input
	//printf("CHILDREG %u %u\n", child1, child2);
	if(child1 & 0x1){
		nvit = m_NodeValueIn.find((unsigned int) node1);
		if(nvit == m_NodeValueIn.end()){
			result1 = ~result1;
			//printf("CHILD INVERTED: %d\n", node1);
		}
		else{
			//printf("CHILD NOT INVERTED: %d\n", node1);

		}
	}

	if(child2 & 0x1){
		nvit = m_NodeValueIn.find((unsigned int) node2);
		if(nvit == m_NodeValueIn.end()){
			result2 = ~result2;
			//printf("CHILD INVERTED: %d\n", node2);
		}
		else{
			//printf("CHILD NOT INVERTED: %d\n", node2);

		}
	}

	//AND Operation
	//printf("Node %d IN: %d %d\n", node, child1, child2);
	//printf("c1: %8llx\n", result1);
	//printf("c2: %8llx\n", result2);
	unsigned long long result = result1 & result2;
	//printf("r : %8llx\n\n", result);
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




void CutFunction::DFS(unsigned int node, std::map<unsigned int, int>& stop){
	node = node & 0xFFFFFFFE;	
	//printf("CURRENT NODE: %d\n", node);

	if(stop.find(node) != stop.end()){
		stop[node]++;
		return;
	}
	else if(node > m_AIG->getInputSize()*2+1){

		//Fix Negative edges for cuts
		unsigned node1, node2, child1, child2;
		child1 = m_AIG->getChild1(node);
		child2 = m_AIG->getChild2(node);

		node1 = child1 & 0xFFFFFFFE;
		node2 = child2 & 0xFFFFFFFE;

		//printf("CHILDREG %u %u\n", child1, child2);
		//printf("CHILDPOS %u %u\n", node1, node2);

		//Recurse until primary input;
		DFS(node1, stop);
		DFS(node2, stop);
	}
}




/*******************************************************
 *  printLibrary
 *    Print out function Hash Table 
 *******************************************************/

void CutFunction::printLibrary(){
	printf("\n\nPrinting library functions that were preprocessed\n");
	std::map<unsigned long long, std::string>::iterator it;
	for(it = m_HashTable.begin(); it != m_HashTable.end(); it++){
		printf("FUNCTION: %llx\tPRIMITIVE: %s\n", it->first, it->second.c_str());
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
	std::map<unsigned long long, std::set<unsigned> >::iterator it;
	for(it = m_NodeFunction.begin(); it != m_NodeFunction.end(); it++){
		printf("FUNCTION: %s\t%llx\nNUMBER OF MATCH: %d\n",m_HashTable[it->first].c_str(), it->first, (unsigned int)  it->second.size());
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

				if(output== (*sit)){
					printf("Output: %d\t\t", output);
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
	std::map<unsigned long long, int>::iterator it;
	printf("Unique Functions----------%d\n\n", (int) m_FunctionCount.size());
	for(it = m_FunctionCount.begin(); it != m_FunctionCount.end(); it++){
		printf("Function: %16llx\t\tCount: %d\n", it->first, it->second);
	}
}



/*******************************************************
 *  printStat
 *    Print out function calculation statistics 
 *******************************************************/
void CutFunction::printUniqueFunction(){
	printf("-------------------------------------\n");
	printf(" * Printing Unique Functions\n");
	printf("-------------------------------------\n");
	std::map<std::set<unsigned long long>, std::vector<std::vector<unsigned> > >::iterator iMap;
	std::set<unsigned long long>::iterator iSet;

	for(iMap = m_UniqueFunction.begin(); iMap != m_UniqueFunction.end(); iMap++){
		printf("FUNCTION KEY: \n");
		for(iSet = iMap->first.begin(); iSet != iMap->first.end(); iSet++){
			printf(" %llx\n", *iSet);
		}
		printf(" *  COUNT: %d\n", (int)iMap->second.size());

		for(unsigned int i = 0; i < iMap->second.size(); i++){
			printf("        Input: ");
			for(unsigned int k = 0; k < iMap->second[i].size()-1; k++){
				printf("%d ", iMap->second[i][k]);
			}
			printf("\tOutput: %d\n", iMap->second[i][iMap->second[i].size() -1]);
		}
		printf("\n");
	}
}




/*******************************************************
 *  printStat
 *    Print out function calculation statistics 
 *******************************************************/
void CutFunction::printUniqueFunctionStat(){
	printf("-------------------------------------\n");
	printf(" * Printing Unique Functions Statistics\n");
	printf("-------------------------------------\n");
	std::map<std::set<unsigned long long>, std::vector<std::vector<unsigned> > >::iterator iMap;
	std::set<unsigned long long>::iterator iSet;
	std::vector< std::map<std::set<unsigned long long>, std::vector<std::vector<unsigned> > >::iterator > mostUnique;

	unsigned int min = 0xFFFFFFFF;

	for(iMap = m_UniqueFunction.begin(); iMap != m_UniqueFunction.end(); iMap++){
		if(iMap->second.size() < min){
			min = iMap->second.size();
		}
	}

	for(iMap = m_UniqueFunction.begin(); iMap != m_UniqueFunction.end(); iMap++){
		if(iMap->second.size() == min)
			mostUnique.push_back(iMap);
	}


	for(unsigned int a = 0; a < mostUnique.size(); a++){
		printf("FUNCTION KEY: \n");
		for(iSet = mostUnique[a]->first.begin(); iSet != mostUnique[a]->first.end(); iSet++){
			printf(" %llx\n", *iSet);
		}
		printf(" *  COUNT: %d\n", (int)mostUnique[a]->second.size());

		for(unsigned int i = 0; i < mostUnique[a]->second.size(); i++){
			printf("        Input: ");
			for(unsigned int k = 0; k < mostUnique[a]->second[i].size()-1; k++){
				printf("%d ", mostUnique[a]->second[i][k]);
			}
			printf("\tOutput: %d\n", mostUnique[a]->second[i][mostUnique[a]->second[i].size() -1]);
		}
		printf("\n");

	}

}



void CutFunction::getPortMap(std::map<unsigned long long, std::vector<std::vector<unsigned>*> >& pmap){
	pmap =	m_PortMap;
}

void CutFunction::getPortMap_DC(std::map<unsigned long long, std::vector<std::vector<unsigned>*> >& pmap){
	pmap =	m_PortMap_DC;
}


void CutFunction::getHashMap(std::map<unsigned long long, std::string>& hmap){
	hmap = m_HashTable;
}

void CutFunction::getFunctionCount(std::map<unsigned long long, int>& fc){
	fc = m_FunctionCount;	
}


void CutFunction::reset(){
	m_NodeValue.clear();
	m_NodeValueIn.clear();
	m_FunctionCount.clear();
	m_NodeFunction.clear();
	m_PortMap.clear();
	m_PortMap_DC.clear();
	m_UniqueFunction.clear();


}



/*
	 void CutFunction::processLUTs(Graph* ckt, std::map<std::string, std::set<unsigned long long> >& pDatabase){
	 printf("\n\n\n\nPROCESSING LUTs...\n");
	 std::map<int, Vertex<std::string>*>::iterator it;
//Loop through graph to find LUTs
for(it = ckt->begin(); it!=ckt->end(); it++){
//printf("Graph Node : %d\n", it->first);
//If vertex type is a LUT
if(it->second->getType().find("LUT") != std::string::npos){
unsigned long long function = it->second->getLUT();

unsigned int inputSize = it->second->getNumInputs();	

unsigned int* permutation = setPermutation(inputSize);
if(permutation == NULL)
continue;

printf("LUT FUNCTION: %llx\t LUT SIZE: %d\n", function, inputSize);

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
std::set<unsigned long long> LUTDatabase;
do{
//printf("Permutation:\n");

unsigned int negationLimit = 1 << inputSize;
for(unsigned int j = 0; j < negationLimit; j++){
std::vector<unsigned long long> truthTableInput;
int count = j; 
for(unsigned int i = 0; i < inputSize; i++){
unsigned int permVal = permutation[i];
unsigned long long input = m_Xval[permVal];

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
unsigned long long pFunction64 = 0;
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
	 std::map<std::string, std::set<unsigned long long> >::iterator pit;
	 std::set<unsigned long long>::iterator sit;
	 for(pit = pDatabase.begin(); pit != pDatabase.end(); pit++){
	 printf("CIRCUIT: %s\n", pit->first.c_str());
	 for(sit = pit->second.begin(); sit != pit->second.end(); sit++){
	 printf("%llx\n", *sit);
	 }
	 printf("\n");
	 }
	 }
 */

