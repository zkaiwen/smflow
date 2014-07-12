/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      aig.cpp
  @      
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @      Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "aig.hpp"




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

//Static Variables
std::string AIG::s_SourcePrim = "";

/*******************************************************
 *  Constructor
 *    Empty
 ********************************************************/
AIG::AIG(){
	//printf("[AIG]--Constructor\n");
	m_Aiger = aiger_init();
	m_Last = 0;

}



/*******************************************************
 *  Destructor
 *    Deletes allocated space. Does not delete AIG Graph
 ********************************************************/
AIG::~AIG(){
	clearAIG();
}




/*******************************************************
 *  clearAIG 
 *    Clears the AIG graph completely
 ********************************************************/
void AIG::clearAIG(){
	m_HashTable.clear();
	m_Outputs.clear();
	m_GateMap.clear();
	aiger_reset(m_Aiger);
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *    GETTER METHODS 
####*
############################################################## 
##############################################################*/



/*******************************************************
 *  getOutputs 
 *    Gets the PO of the AIG
 *
 *    @PARAM: 
 *      * output: Reference to the output vector
 ********************************************************/
void AIG::getOutputs(std::vector<unsigned>& output){
	output = m_Outputs;
}



/*******************************************************
 *  getOutputSize
 *    Gets the output size of the AIG
 *
 *    @RETURN: Number of outputs of the AIG 
 ********************************************************/
unsigned int AIG::getOutputSize(){
	return m_Aiger->num_outputs; 
}



/*******************************************************
 *  getInputSize
 *    Gets the Input size of the AIG
 *
 *    @RETURN: Number of inputs of the AIG 
 ********************************************************/
unsigned int AIG::getInputSize(){
	return m_Aiger->num_inputs;
}



/*******************************************************
 *  getSize
 *    Gets the size of the AIG
 *
 *    @RETURN: Size of the AIG 
 ********************************************************/
unsigned int AIG::getSize(){
	return m_Aiger->num_ands; 
}



/*******************************************************
 *  getChild1 
 *    Gets the first child given a node
 *
 *    @PARAM: 
 *      * index: Node to get child 1 from
 *
 *    @RETURN: Node ID of Child1
 ********************************************************/
unsigned AIG::getChild1(unsigned index){
	//printf("index %d\n", index);
	index= (index - ((m_Aiger->num_inputs+1) *2))/2;	
	//printf("index %d\n", index);
	return m_Aiger->ands[index].rhs0; 
}



/*******************************************************
 *  getChild2
 *    Gets the first child given a node
 *
 *    @PARAM: 
 *      * index: Node to get child 2 from
 *
 *    @RETURN: Node ID of Child2
 ********************************************************/
unsigned AIG::getChild2(unsigned index){
	//printf("index %d\n", index);
	index= (index - ((m_Aiger->num_inputs+1) *2))/2;	
	//printf("index %d\n", index);
	return m_Aiger->ands[index].rhs1;
}



/*******************************************************
 *  getGNode 
 *    Gets the Graph Node ID give an AIG node ID
 *
 *    @PARAM: 
 *      * aigID: Node of the AIG ID
 *
 *    @RETURN Graph Node ID
 ********************************************************/
unsigned AIG::getGNode(unsigned aigID){
	std::map<int,unsigned >::iterator it;
	for(it = m_GateMap.begin(); it != m_GateMap.end(); it++)
		if(it->second == aigID)
			return it->first;

	printf("Cannot find Graph node for AIG ID: %d\n", aigID);
	printMap();
	exit(1);
}


/*******************************************************
 *  getAIGNode 
 *    Gets the AIG NODE ID given an GRAPH node ID
 *
 *    @PARAM: 
 *      * gID: Node of the GRAPH ID
 *
 *    @RETURN AIG Node ID
 ********************************************************/
unsigned AIG::getAIGNode(unsigned gID){
	std::map<int,unsigned >::iterator it;
	it = m_GateMap.find(gID);
	if(it != m_GateMap.end()){
		return it->second;
	}

	printf("Cannot find Graph node for AIG ID: %d\n", gID);
	printMap();
	exit(1);
}


/*******************************************************
 *  getGNode 
 *    Gets the Graph Node ID give an AIG node ID
 *
 *    @PARAM: 
 *      * aigID: Node of the AIG ID
 *
 *    @RETURN Graph Node ID
 ********************************************************/
std::vector<unsigned >  AIG::findNode(unsigned c1, unsigned c2){
	std::vector<unsigned > nodes;
	for(unsigned int i = 0; i < m_Aiger->num_ands; i++){
		unsigned child1 = m_Aiger->ands[i].rhs0;
		unsigned child2 = m_Aiger->ands[i].rhs1;

		//Negate to get the nodes
		child1 = child1 & 0xFFFFFFFE;	
		child2 = child2 & 0xFFFFFFFE;	

		if((child1 == c1 &&
					child2 == c2) ||
				(child2 == c1 &&
				 child1 == c2)){
			nodes.push_back(i);
		}
	}
	return nodes;
}




bool AIG::hasChild(unsigned child, unsigned exception){
	unsigned int ex = exception;
	child = child & 0xFFFFFFFE;	
	if(ex > m_Aiger->num_ands) return false;

	for(unsigned int i = 0; i < ex; i++){
		unsigned child1 = m_Aiger->ands[i].rhs0;
		unsigned child2 = m_Aiger->ands[i].rhs1;

		child1 = child1 & 0xFFFFFFFE;	
		child2 = child2 & 0xFFFFFFFE;	

		if(child1 == child || child2 == child)
			return true;
	}

	for(unsigned int i = ex + 1; i < m_Aiger->num_ands; i++){
		unsigned child1 = m_Aiger->ands[i].rhs0;
		unsigned child2 = m_Aiger->ands[i].rhs1;

		child1 = child1 & 0xFFFFFFFE;	
		child2 = child2 & 0xFFFFFFFE;	

		if(child1 == child || child2 == child)
			return true;
	}

	return false;

}




/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     SETTER METHODS
####*
############################################################## 
##############################################################*/
void AIG::setPrimSource(std::string source){
	s_SourcePrim = source;
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     FILE METHODS
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  importAIG 
 *    Imports an AIG given a file
 *
 *    @PARAMS: 
 *      * fileName: Name of file to read AIG from
 ********************************************************/
void AIG::importAIG(std::string fileName){
	printf("[AIG]--Reading in AIG information for %s\n", fileName.c_str());

	//Check to see if AIG is empty. If empty, clear it
	if(getSize() != 0){
		printf("AIG is not empty. Overwriting graph...\n");
		clearAIG();
	}

	FILE* file;
	file = fopen(fileName.c_str(), "r");

	const char* msg;
	msg = aiger_read_from_file(m_Aiger, file);
	if(msg != NULL)
		printf("!! ERROR READING AIGER FILE: %s\n", msg);

	printf("Import Complete***********\n\n");
}







/*******************************************************
 *  exportAIG 
 *    Exports an AIG to a given file
 *
 *    @PARAMS: 
 *      * fileName: Name of file to write AIG to
 ********************************************************/
void AIG::writeAiger(std::string filename, bool isBinary){
	//printf("[AIG]--Writing AIG to file...AIGER Format\n");
	enum aiger_mode mode;
	if(isBinary)
		mode = aiger_binary_mode;
	else
		mode = aiger_ascii_mode;

	//File
	FILE* file;
	file= fopen(filename.c_str(), "w");

	//Return code is zero if error
	int returnCode = aiger_write_to_file(m_Aiger, mode, file);
	if(returnCode == 0)
		printf("[AIG] -- Error with writing to file...\n");
}









/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *    ALGORITHM METHODS 
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  convertGraph2AIG 
 *    Converts a given graph to an AIG
 *
 *    @PARAMS: 
 *      * ckt:    Graph to convert to AIG
 ********************************************************/
void AIG::convertGraph2AIG(Graph* ckt, bool sub){
	printf("[AIG] -- Converting Graph to AIG\n");

	//Overwrite AIG if one exists
	if(getSize() != 0){
		printf("AIG is not empty. Overwriting AIG...\n");
		clearAIG();
	}

	//Primitive AIG gates
	std::string xorAIGFile_c = s_SourcePrim + "xor";
	std::string orAIGFile_c = s_SourcePrim + "or";
	std::string norAIGFile_c = s_SourcePrim + "nor";
	std::string nandAIGFile_c = s_SourcePrim + "nand";
	std::string andAIGFile_c = s_SourcePrim + "and";
	std::string aoiAIGFile_c = s_SourcePrim + "aoi";
	std::string oaiAIGFile_c = s_SourcePrim + "oai";
	std::string mxAIGFile_c = s_SourcePrim + "mux";
	std::string addAIGFile_c = s_SourcePrim + "add";

	std::map<int, Vertex*>::iterator it;
	std::vector<Vertex*> gNodeOutput;
	std::vector<int> toBeDeleted;
	std::vector<int> flipflops;




	//Go through each node in the circuit and covert it to its
	//  AND/INV representative
	for(it = ckt->begin(); it != ckt->end(); it++){
		std::string circuitType;

		//Read in the necessary primitive type
		//printf("CONVERTING V%d to AIG PRIM\n", it->first);
		//printf("GATE: %s\n", it->second->getType().c_str());
		std::string gateType = it->second->getType();

		if(gateType.find("XORCY") != std::string::npos)
			circuitType = xorAIGFile_c + "cy";
		else if(gateType.find("FF") != std::string::npos || 
				gateType.find("FD") != std::string::npos ||
				gateType.find("LD") != std::string::npos ||
				gateType.find("LAT") != std::string::npos){
			if(!sub)
				if(handleFF(it->first, ckt)){
					toBeDeleted.push_back(it->first);
				}
			continue;
		}
		else if(gateType.find("MUXCY") != std::string::npos)
			circuitType = s_SourcePrim + "muxcy";
		else if(gateType.find("VCC") != std::string::npos ||
				gateType.find("GND") != std::string::npos){
			ckt->addConstant(it->first);
			continue;
		}
		else if(gateType.find("XOR") != std::string::npos)
			circuitType = xorAIGFile_c;
		else if(gateType.find("NAND") != std::string::npos)
			circuitType = nandAIGFile_c;
		else if(gateType.find("AND") != std::string::npos ||
				gateType.find("IN") != std::string::npos){
			if(it->second->getIVSize() > 2)
				circuitType = andAIGFile_c;
			else
				continue;
		}
		else if(gateType.find("NOR") != std::string::npos)
			circuitType = norAIGFile_c;
		else if(gateType.find("OR") != std::string::npos)
			circuitType = orAIGFile_c;
		else if(gateType.find("MUXF") != std::string::npos)
			circuitType = s_SourcePrim + "muxf";
		else if(gateType.find("AOI") != std::string::npos)
			circuitType = aoiAIGFile_c;
		else if(gateType.find("OAI") != std::string::npos)
			circuitType = oaiAIGFile_c;
		else if(gateType.find("MX") != std::string::npos)
			circuitType = mxAIGFile_c;
		else if (gateType.find("RAM") != std::string::npos
				|| gateType.find("DSP") != std::string::npos){
			handleFF(it->first, ckt);	
			continue;
		}
		else if(gateType.find("ADD") != std::string::npos)
			circuitType = addAIGFile_c;
		else if(gateType.find("TBUF") != std::string::npos)
			exit(1);
		else if(gateType.find("BUF") != std::string::npos){
			toBeDeleted.push_back(it->first);
			std::vector<Vertex*> in;
			std::vector<Vertex*> out;
			it->second->getOutput(out);
			it->second->getInput(in);

			std::string outputPortName = in[0]->removeOutputValue(it->first);

			for(unsigned int i = 0; i < out.size(); i++){
				int index = out[i]->removeInputValue(it->first);
				out[i]->addInput(in[0]);

				std::vector<std::string> outPorts;
				out[i]->getInPorts(outPorts);
				out[i]->addInPort(outPorts[index]);
				out[i]->removeInPortValue(index);

				in[0]->addOutput(out[i], outputPortName);
			}

			continue;
		}
		else if(gateType.find("TINV") != std::string::npos)
			exit(1);
		else{
			printf("Unknown Gate Type\tI%sI\nPress y to resume, n to quit\n", gateType.c_str());

			char inputchar;
			std::cin>>inputchar;
			if(inputchar == 'y')
				continue;
			else
				exit(1);
		}


		int gateSizeNum = it->second->getIVSize();
		char gateSize = '0' + gateSizeNum;

		circuitType = circuitType + gateSize + ".g";
		//printf("CIRCUIT PRIM: %s\n", circuitType.c_str());

		Graph* primCkt = new Graph(circuitType);
		primCkt->importPrimitive(circuitType, ckt->getLast() +1);

		//Substitute the node with the primitive type
		ckt->substitute(it->first, primCkt);
		toBeDeleted.push_back(it->first);

		//printf("LAST INDEX: %d\n\n", aindex);
		//printf("VERTEXID: %d\n", it->first);
		//if(ckt->getLast() > 40805){
		//	ckt->printg();
		//	exit(1);
		//}	
		//ckt->printg();
		//printf("\n\n");
	}

	//printf("CONVERTED CIRCUIT\n");
	//ckt->printg();
	//printf("\n\n");


	//Delete substituted nodes from the graph
	for(unsigned int i = 0; i < toBeDeleted.size(); i++)
		ckt->removeVertex(toBeDeleted[i]);

	if(sub)
		return;
















	/******************************************************
	 ********        PERFORM AIG CALCULATION      **********
	 *******************************************************/
ckt->setLevels();

	//Order the vertices by level
	std::map<int, std::vector<Vertex*> > vLevel;
	for(it = ckt->begin(); it != ckt->end(); it++){
		//printf("ID:\t%dGate type:\t%s\n", it->first, it->second->getType().c_str());
		vLevel[it->second->getLevel()].push_back(it->second);
	}
	//exit(1);


	//Create PI for input LEVEL 0
	for(unsigned int i = 0; i < vLevel[0].size(); i++){
		if(vLevel[0][i]->getType() == "VCC")
			m_GateMap[vLevel[0][i]->getVertexID()] = 1;
		else if(vLevel[0][i]->getType() == "GND")
			m_GateMap[vLevel[0][i]->getVertexID()] = 0;
		else{
			unsigned int input= create_input();
			m_GateMap[vLevel[0][i]->getVertexID()] = input; 
			//printf("INPUT:  %d\tAIG: %u\n", vLevel[0][i]->getVertexID(),input);
		}
	}

	unsigned int vlevelSize = vLevel.size();
	unsigned int maxLevel = ckt->getMaxLevel();

	//Create the rest of the AIG Graph LEVEL 1 and UP
	for(unsigned int i = 1; i <= maxLevel; i++){
		if(vLevel.size() != vlevelSize){
			printf("Size mismatch\n");
			exit(1);
		}
		for(unsigned int j = 0; j < vLevel[i].size(); j++){
			if(vLevel[i][j]->getType().find("AND") != std::string::npos){
				std::vector<Vertex*> in;
				vLevel[i][j]->getInput(in);

				unsigned output = create_and2(m_GateMap[in[0]->getVertexID()], m_GateMap[in[1]->getVertexID()]);
				int vertexID = vLevel[i][j]->getVertexID();
				m_GateMap[vertexID] = output;
			}
			else if(vLevel[i][j]->getType().find("INV") != std::string::npos){
				std::vector<Vertex*> in;
				vLevel[i][j]->getInput(in);


				if(m_GateMap[in[0]->getVertexID()] == 0)
					m_GateMap[vLevel[i][j]->getVertexID()] = 1;
				else if(m_GateMap[in[0]->getVertexID()] == 1)
					m_GateMap[vLevel[i][j]->getVertexID()] = 0;
				else{
					if(m_GateMap[in[0]->getVertexID()]%2 == 0)
						m_GateMap[vLevel[i][j]->getVertexID()] = m_GateMap[in[0]->getVertexID()] + 1;
					else
						m_GateMap[vLevel[i][j]->getVertexID()] = m_GateMap[in[0]->getVertexID()] - 1;

				}
				if(vLevel[i][j]->getOVSize() == 0 ){
					m_Outputs.push_back(m_GateMap[in[0]->getVertexID()]);
					aiger_add_output(m_Aiger, m_GateMap[vLevel[i][j]->getVertexID()], 0);
				}
				continue;
			}
			else
			{
				printf("Unknown Gate type:\t%s\n", vLevel[i][j]->getType().c_str());
				exit(1);
			}

			//CHeck to see if node has an output
			if(vLevel[i][j]->getOVSize() == 0){
				//printf("NO OUTPUT\n");
				m_Outputs.push_back(m_GateMap[vLevel[i][j]->getVertexID()]);
				aiger_add_output(m_Aiger, m_GateMap[vLevel[i][j]->getVertexID()], 0);
			}
			//printf("\n");
		}
	}



	 /*
	std::vector<int> gInputs;
	ckt->getInputs(gInputs);
		
	std::list<unsigned int> queue;
	std::list<unsigned int>::iterator qit;
	std::set<unsigned int> marked;

	for(unsigned int i = 0; i < gInputs.size(); i++){
		Vertex* vertex = ckt->getVertex(gInputs[i]);
		std::string type  = vertex->getType();
		if(type == "VCC")
			m_GateMap[gInputs[i]] = 1;
		else if(type == "GND")
			m_GateMap[gInputs[i]] = 0;
		else{
			unsigned int input= create_input();
			m_GateMap[gInputs[i]] = input; 
		}

		std::vector<Vertex*> outputs;
		vertex->getOutput(outputs);
		for(unsigned int i = 0; i < outputs.size(); i++){
			if(marked.find(outputs[i]->getVertexID()) == marked.end()){
				marked.insert(outputs[i]->getVertexID());
				queue.push_back(outputs[i]->getVertexID());
			}
		}
	}
	


	//BFS- Create And gates/ Inverters
	while(queue.size() != 0){
		int item = queue.front();
		qit = queue.begin();
		Vertex* vertex= ckt->getVertex(item);
		std::string type  = vertex->getType();

		std::vector<Vertex*> in;
		vertex->getInput(in);

		//Check to see if inputs have been mapped. If not skipp for now
		bool valid = false;
		while(!valid){
			if(m_GateMap.find(in[0]->getVertexID()) == m_GateMap.end()){
				qit++;
				assert(qit != queue.end());
				
				item = *(qit);

				vertex = ckt->getVertex(item);
				type  = vertex->getType();
	
				in.clear();
				vertex->getInput(in);
			}
			else if(in.size() == 2){
				if(m_GateMap.find(in[1]->getVertexID()) == m_GateMap.end()){
					qit++;
					assert(qit != queue.end());
				
					item = *(qit);

					vertex = ckt->getVertex(item);
					type  = vertex->getType();
	
					in.clear();
					vertex->getInput(in);
				}
				else 
					valid = true;
			}
			else
				valid = true;
		}

		if(type.find("AND") != std::string::npos){
			unsigned output = create_and2(m_GateMap[in[0]->getVertexID()], m_GateMap[in[1]->getVertexID()]);
			m_GateMap[item] = output;
		}
		else if(type.find("INV") != std::string::npos){
			//Handle inverting constants
			if(m_GateMap[in[0]->getVertexID()] == 0)
				m_GateMap[item] = 1;
			else if(m_GateMap[in[0]->getVertexID()] == 1)
				m_GateMap[item] = 0;

			else{
				//Even is positive, odd, is negative
				if(m_GateMap[in[0]->getVertexID()]%2 == 0)
					m_GateMap[item] = m_GateMap[in[0]->getVertexID()] + 1;
				else
					m_GateMap[item] = m_GateMap[in[0]->getVertexID()] - 1;
			}


			//If node has not outputs, Node is an output nodeH
			if(vertex->getOVSize() == 0 ){
				m_Outputs.push_back(m_GateMap[in[0]->getVertexID()]);
				aiger_add_output(m_Aiger, m_GateMap[item], 0);
			}
		}
		else
		{
			printf("Unknown Gate type:\t%s\n", type.c_str());
			exit(1);
		}
		
		queue.erase(qit);
				
				
		//Add the next level of nodes	(FANOUT)
		std::vector<Vertex*> outputs;
		vertex->getOutput(outputs);
				
		for(unsigned int i = 0; i < outputs.size(); i++){
			if(marked.find(outputs[i]->getVertexID()) == marked.end()){
				marked.insert(outputs[i]->getVertexID());
				queue.push_back(outputs[i]->getVertexID());
			}
		}
	}
	*/




	//printf("[AIG]--Checking AIG...");
	const char* msg;
	msg = aiger_check(m_Aiger);
	if(msg != NULL){
		printf("[AIG] -- AIG CHECK FAIL!!\n%s\n", msg);
	}
	//printf("[AIG]--Conversion Complete\n");

	/*
	WRITING AIG TO OUTPUT AIGER FILE (.AAG)
	std::string cktname = ckt->getName();
	ckt->getName().rfind("/");
	int index = cktname.rfind("/")+1;
	cktname = cktname.substr(index, cktname.length()-index-1);
	//printf("FILE NAME: %s\n", cktname.c_str());
	writeAiger("aiger/" + cktname + "aag", true);
	printf("END\n");
	print();
	*/
}





/*******************************************************
 *  handleFF
 *    Removes FF and treates IO as PI and PO of circuit
 *
 *    @PARAMS: 
 *      * node: FF node in Graph
 *    @RETURN: True is FF node was removed, 
 *             False if converted to IN
 ********************************************************/
bool AIG::handleFF(int node, Graph* ckt){
//	printf("Removing FF...\nInputs to FF will go to PO. Gates the FF goes into will be switched for PI\n");
	std::vector<Vertex*> input;
	Vertex* ffNode = ckt->getVertex(node);
	ffNode->getInput(input);

	//Remove ff from the input nodes' output
	for(unsigned i = 0; i < input.size(); i++){
		input[i]->removeOutputValue(node);
		ffNode->removeInputValue(input[i]->getVertexID());
		
		//Delete the input if the input is isolated	
		if(input[i]->getNumInputs() == 0 && input[i]->getNumOutputs() == 0){
			ckt->removeVertex(input[i]->getVertexID());
		}
	}

	//If the FF has outputs, convert to IN
	if(ffNode->getOVSize() > 0){
		//Convert the FF to a IN Type 
		std::stringstream ss;
		ss<< "FF" << node << "_"<< ckt->getNodeName(node);
		ckt->addInput(ss.str(), node);
		ffNode->setType("IN");
		return false;
	}
	else
		return true;


}



/*******************************************************
 *  create_and2
 *    Creates an AND node in the AIG
 *
 *    @PARAMS: 
 *      * e1: AIG Node of the first input
 *      * e2: AIG Node of the second input
 *
 *    @RETURN: AND node of AIG that was created
 ********************************************************/
unsigned AIG::create_and2(unsigned e1, unsigned e2){
	std::vector<unsigned> key;
	if(e1 < e2){
		key.push_back(e1);	
		key.push_back(e2);	
	}
	else{
		key.push_back(e2);	
		key.push_back(e1);	
	}

	if(m_HashTable.find(key) != m_HashTable.end()){
		return m_HashTable[key];
	}

	m_Last+=2;
	aiger_add_and(m_Aiger, m_Last, e1, e2); 
	m_HashTable[key] = m_Last;
	return m_Last;
}




/*******************************************************
 *  create_input 
 *    Creates a new input node in the AIG
 *
 *    @RETURN: INPUT node of AIG that was created
 ********************************************************/
unsigned AIG::create_input(){
	//Create an empty vertex
	m_Last+=2;
	aiger_add_input(m_Aiger, m_Last, 0); 

	//Return location of created input
	return m_Last;
}










/*##########################################
#################################################
#####################################################
##########################################################
###############################################################
####*
#### *     PRINT METHODS
####*
############################################################## 
##############################################################*/

/*******************************************************
 *  printHash
 *    Prints hash table used by the AIG
 ********************************************************/
void AIG::printHash(){
	std::map<std::vector<unsigned>, unsigned>::iterator it;
	printf("Printing Hash Table\tSIZE: %d\n", (int) m_HashTable.size());
	for(it = m_HashTable.begin(); it!= m_HashTable.end(); it++)
		printf("KEY: %d %d\tVAL: %d\n", it->first[0], it->first[1], it->second);
}



/*******************************************************
 *  print
 *    Prints contents of the AIG 
 ********************************************************/
void AIG::print(){
	printf("[AIG] Data structure\n");
	printf("\nInputs:\n");
	for(unsigned int i = 0; i < m_Aiger->num_inputs; i++){
		printf("%u ", m_Aiger->inputs[i].lit);	
	}
	printf("\nOutputs:\n");
	for(unsigned int i = 0; i < m_Aiger->num_outputs; i++){
		printf("%u ", m_Aiger->outputs[i].lit);	
	}
	printf("\nAnd Gates:\n");
	for(unsigned int i = 0; i < m_Aiger->num_ands; i++)
		printf("%u - %u %u\n", m_Aiger->ands[i].lhs, m_Aiger->ands[i].rhs0, m_Aiger->ands[i].rhs1);	
}
		
		

/*******************************************************
 *  print
 *    Prints contents of the AIG from output to
 *    the list of inputs
 *  
 *    @PARAMS: 
 *      * output: AIG output node to start printing
 *      * input: AIG input nodes to stop at
 ********************************************************/
void AIG::printSubgraph(unsigned output, std::set<unsigned>& input){
	std::list<unsigned> queue;
	std::set<unsigned> marked;
	std::map<unsigned int, unsigned int> levelMap;
	queue.push_back(output);
	marked.insert(output);
	levelMap[output] = 0;

	bool done = false;
	unsigned doneLevel = 0;

	std::cout<<"digraph{\n";
	

	while(!queue.empty()){
		unsigned int item = queue.front();
		unsigned int itemNode = item & 0xFFFFFFFE;
		queue.pop_front();

		if(done){
			if(levelMap[itemNode] > doneLevel)
				break;
		}

		if(input.find(itemNode) != input.end()){
			input.erase(itemNode);
			if(input.size() == 0){
				done = true;
				doneLevel = levelMap[itemNode];
			}
		}
	
	
		//Check to see if it is input
		if(item <= (getInputSize()*2 + 1))
			continue;

		unsigned int c1 = getChild1(item);	
		unsigned int c2 = getChild2(item);	

		unsigned int node1 = c1 &0xFFFFFFFE;	
		unsigned int node2 = c2 &0xFFFFFFFE;	

		std::string dot1 = "";	
		std::string dot2 = "";	

		if((c1&0x1) == 0x1)
			dot1 = " [style=dotted]";
		if((c2&0x1) == 0x1)
			dot2 = " [style=dotted]";
		std::cout<<"\t"<<node1<<"->"<<itemNode<<dot1<<std::endl;
		std::cout<<"\t"<<node2<<"->"<<itemNode<<dot2<<std::endl;

		if(marked.find(node1) == marked.end()){
			marked.insert(node1);
			queue.push_back(c1); //Want to keep the sign
			levelMap[node1] = levelMap[itemNode] + 1;
		}
		if(marked.find(node2) == marked.end()){
			marked.insert(node2);
			queue.push_back(c2);
			levelMap[node2] = levelMap[itemNode] + 1;
		}
	}

	std::cout<<("}\n");
}



/*******************************************************
 *  printMap
 *    Prints circuit mapping to AIG
 ********************************************************/
void AIG::printMap(){
	printf("\n\nPrinting Circuit mapping to AIG\n");
	std::map<int,unsigned>::iterator it;
	for(it = m_GateMap.begin(); it != m_GateMap.end(); it++){
		printf("AIG NODE: %d\tCKT NODE: %d\n", it->second, it->first);
	}
}







