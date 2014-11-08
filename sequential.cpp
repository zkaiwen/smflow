/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  SEQUENTIAL.cpp
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "sequential.hpp" 


/*****************************************************************************
 * getFFList 
 *   Finds all the FF components in a circuit; 
 *
 * @PARAMS: ckt - circuit 
 *****************************************************************************/
void SEQUENTIAL::getFFList(Graph* ckt, std::list<Vertex*>& ffList){
	//Find all the flip flops in a circuit
	std::map<int, Vertex*>::iterator it;
	for(it = ckt->begin(); it != ckt->end(); it++){
		if(it->second->getType().find("FD") != std::string::npos){
			ffList.push_back(it->second);
		}
	}
}






/*****************************************************************************
 * getNumFFFeedback 
 *   FInd the number of FF with feed back loops from Q to D.  
 *
 * @PARAMS: ffList- List of flip flops 
 *****************************************************************************/
void SEQUENTIAL::getFFFeedbackList(Graph* ckt, std::list<InOut*>& ffList){
	//Find all the flip flops in a circuit
	std::map<int, Vertex*>::iterator it;
	for(it = ckt->begin(); it != ckt->end(); it++){
		if(it->second->getType().find("FD") != std::string::npos){
			Vertex* ff = it->second;

			//Check to see if Q loops back to D (Possible counter)		
			std::set<unsigned> mark;
			std::set<unsigned> ffFound;

			int dport = ff->getInputPortID("D");
			DFS_FF(mark, ffFound, ckt->getVertex(dport));

			//Check to see if current FF is in the input list (FEEDBACK)
			if(ffFound.find(ff->getID()) != ffFound.end()){
				InOut* inout = new InOut;
				inout->output = ff->getID();
				inout->input = ffFound;
				ffList.push_back(inout);
			}

		}
	}
}


void SEQUENTIAL::deleteFFFeedbackList(std::list<InOut*>& ffList){
	std::list<InOut*>::iterator it;

	for(it = ffList.begin(); it != ffList.end(); it++)
		delete *it;

}






/*****************************************************************************
 * findRegisterGroup
 *   Finds Register Group based on enable ports; 
 *
 * @PARAMS: ffList- List of flip flops 
 *****************************************************************************/
void SEQUENTIAL::findRegisterGroup(std::list<Vertex*>& ffList, std::map<unsigned, unsigned>& sizeCount){
	printf("[SEQ] -- Searching for Register groups via enable signal\n");
	//ckt->print();
	std::list<Vertex*>::iterator iList;

	//Group them by ENABLE signals
	printf(" * Grouping Enable Signals\n");
	std::map<int, std::set<Vertex*> > reg;
	for(iList = ffList.begin(); iList != ffList.end(); iList++){
		Vertex* ff = *iList;
		int enablePort= ff->getInputPortID("CE");

		if(enablePort != -1){
			reg[enablePort].insert(ff); 
		}
	}

	std::map<int, std::set<Vertex*> >::iterator iMap;
	for(iMap = reg.begin(); iMap != reg.end(); iMap++){
		if(sizeCount.find(iMap->second.size()) == sizeCount.end())
			sizeCount[iMap->second.size()] = 1;
		else
			sizeCount[iMap->second.size()]++;
	}
}






/*****************************************************************************
 * findFSM 
 *   Finds possible FSM in a circuit; 
 *
 * @PARAMS: ckt- circuit to look for counters
 *****************************************************************************/
void SEQUENTIAL::findFSM(Graph* ckt){
	printf("[SEQ] -- Finding FSM\n");
	//ckt->print();
	std::map<int, Vertex*>::iterator it;
	std::list<Vertex*> ffList;
	std::list<Vertex*>::iterator iList;
	std::set<Vertex*> possibleFF;
	std::set<Vertex*>::iterator iPos;

	//Find all the flip flops in a circuit
	for(it = ckt->begin(); it != ckt->end(); it++){
		if(it->second->getType().find("FD") != std::string::npos){
			ffList.push_back(it->second);
		}
	}


	//Finding input output loop for each FF
	//Dependence FF, FF that is connected to Dependence output
	std::map<Vertex*, Vertex*> dependence;
	std::map<Vertex*, Vertex*>::iterator iDep;
	for(iList = ffList.begin(); iList != ffList.end(); iList++){
		Vertex* ff = *iList;

		//Check to see if Q loops back to D (Possible counter)
		if(ff->getOVSize() == 0){
			printf("FFID: %d\t", ff->getID());
			printf("OUTPUT SIZE IS 0\n");
			continue;
		}

		std::set<int> mark;
		int vID = ff->getID();
		int dport = ff->getInputPortID("D");

		//Make sure FF's have a D port
		assert(dport != -1);
		Vertex* dInput = ckt->getVertex(dport);

		//Check if immediate input is a FF
		if(dInput->getType().find("FD") != std::string::npos){
			dependence[dInput] = ff;
			continue;
		}

		int found = ckt->DFS_FSM_S(mark, vID, dInput);

		if(found == 1){
			possibleFF.insert(ff);
			printf("POSSIBLE C FF: %d\n", vID);
		}
	}
	printf("\n");


	//Checking dependence on unknown FF
	for(iDep = dependence.begin(); iDep != dependence.end(); iDep++){
		//printf("DEP: %d FF %d\n", iDep->first->getID(), iDep->second->getID());
		if(possibleFF.find(iDep->first) != possibleFF.end()){
			//printf("DEPENDENCE VALIDATED\n");
			possibleFF.insert(iDep->second);
		}
	}



	printf("\n\nPOSSIBLE FFS in FSMs: ");
	for(iPos = possibleFF.begin(); iPos != possibleFF.end(); iPos++){
		printf("%d ", (*iPos)->getID());
	}
	printf("\n\n");


	//Skip ALGO 2 for now

	//Group them by ENABLE signals
	printf(" * Grouping Enable Signals\n");
	std::map<Vertex*, std::set<Vertex*> > reg;
	std::map<Vertex*, std::set<Vertex*> >::iterator iFSM;
	std::set<Vertex*> noEnableFF;
	std::set<Vertex*>::iterator iSet;

	for(iList = ffList.begin(); iList != ffList.end(); iList++){
		Vertex* ff = *iList;
		int enablePort= ff->getInputPortID("CE");
		Vertex* enable = ckt->getVertex(enablePort);

		if(enablePort == -1){
			/*
				 printf("FFID: %d does not have enable\tIN: ", ff->getID());
				 for(unsigned int i = 0; i < inports.size(); i++)
				 printf("%s ", inports[i].c_str());
				 printf("\n");
			 */

			std::vector<std::string> inports;
			ff->getInPorts(inports);
			noEnableFF.insert(ff);
			continue;
		}
		else{
			reg[enable].insert(ff); 
		}
	}

	printf("\n\nFF's grouped by enable signals:\n");
	for(iFSM = reg.begin(); iFSM != reg.end(); iFSM++){
		if(iFSM->second.size() > 1)	{
			printf("E: %d\tFF: ", iFSM->first->getID());
			for(iSet = iFSM->second.begin(); iSet != iFSM->second.end(); iSet++)
				printf("%d ", (*iSet)->getID());
			printf("\n");
		}
		else{
			printf("Single FF: %d\n", iFSM->first->getID());
		}
	}

	printf("No Enable FF: ");
	for(iSet = noEnableFF.begin(); iSet != noEnableFF.end(); iSet++){
		printf("%d ", (*iSet)->getID());
	}
	printf("\n\n");


	//Traverse backwards from the enable bit and
	//Check to see if any of the possible ff is hit
	//Those that are hit in a single traversal are grouped
	printf("REDUCED FF in FSMs based on enable signal\n");
	for(iFSM = reg.begin(); iFSM != reg.end(); iFSM++){
		if(possibleFF.find(iFSM->first) != possibleFF.end()){
			printf("%d ", iFSM->first->getID());

		}
	}
	printf("\n\n");

	std::list<std::set<Vertex*> > possibleFSMGroup;
	std::list<std::set<Vertex*> >::iterator  iPFG;
	for(iFSM = reg.begin(); iFSM != reg.end(); iFSM++){
		std::set<int> mark;
		std::set<Vertex*> candidate;

		ckt->DFS_FSM_M(mark, possibleFF, iFSM->first, candidate);
		possibleFSMGroup.push_back(candidate);
	}

	printf("Possible FSM Grouping with enable signal\n");
	std::set<Vertex*> possibleFF_Enable;
	for(iPFG = possibleFSMGroup.begin(); iPFG != possibleFSMGroup.end(); iPFG++){
		for(iSet = iPFG->begin(); iSet != iPFG->end(); iSet++){
			Vertex* ff = *iSet;
			possibleFF_Enable.insert(ff);
			printf("%d ", ff->getID());
		}
		printf("\n"); 
	}

	printf("\n\nPOSSIBLE FFS in FSMs: ");
	for(iPos = possibleFF_Enable.begin(); iPos != possibleFF_Enable.end(); iPos++){
		printf("%d ", (*iPos)->getID());
	}
	printf("\n\n");




	//Set of FF fanned out from source, list of FF with same fanout cone 
	std::map<std::set<Vertex*>, std::list<Vertex*> > fanoutMap;
	std::map<std::set<Vertex*>, std::list<Vertex*> >::iterator iFOM;
	for(iPos = possibleFF.begin(); iPos != possibleFF.end(); iPos++){
		std::set<int> mark;
		std::set<Vertex*> candidate;
		std::vector<Vertex*> out;

		Vertex* ff = *iPos;
		ff->getOutput(out);

		for(unsigned int i = 0; i < out.size(); i++)
			ckt->DFS_FSM_T(mark, out[i], candidate);

		fanoutMap[candidate].push_back(ff);

		printf("FFID: %3d\tFO-FF: ", ff->getID());
		for(iSet = candidate.begin(); iSet != candidate.end(); iSet++){
			printf("%d ", (*iSet)->getID());
		}
		printf("\n");
	}

	for(iFOM = fanoutMap.begin(); iFOM != fanoutMap.end(); iFOM++){
		printf("FANOUTKEY: ");
		for(iSet = iFOM->first.begin(); iSet != iFOM->first.end(); iSet++){
			printf("%d ", (*iSet)->getID());
		}
		printf("\nFF: ");
		for(iList = iFOM->second.begin(); iList != iFOM->second.end(); iList++){
			printf("%d ", (*iList)->getID());
		}
		printf("\n\n");

	}








}






unsigned int SEQUENTIAL::replaceLUTs(Graph* ckt){
	printf("[SEQ] -- Replacing LUTs with combinational Logic\n");
	std::map<int, Vertex*>::iterator it;
	std::list<int> tobedeleted;
	unsigned int numLUTs = 0;
	double endIndex = ckt->end()->first;
	double percentComplete = 0.10;
	//ckt->printg();

	for(it = ckt->begin(); it != ckt->end(); it++){
		if(it->second->getType().find("LUT") != std::string::npos){
			numLUTs++;
			//printf("LUT INDEX: %d\n", it->first);
			std::vector<Vertex*> inputs;
			it->second->getInput(inputs);
			unsigned long long function = it->second->getLUT();

			if(inputs.size() == 1){
				if(function == 2){
					std::vector<Vertex*> linput;
					std::vector<Vertex*> loutput;

					it->second->getInput(linput);
					it->second->getOutput(loutput);

					assert(linput.size() == 1);
					std::string outportname = linput[0]->removeOutputValue(it->second->getID());

					for(unsigned int i = 0; i < loutput.size(); i++){
						std::string portname = loutput[i]->getInputPortName(it->second->getID());
						loutput[i]->removeInputValue(it->second->getID());
						loutput[i]->addInput(linput[0], portname);

						linput[0]->addOutput(loutput[i],outportname);
					}

					tobedeleted.push_back(it->first);
				}
				else{
					printf(" -- FUNCTION IS INVERTER\n");
					it->second->setType("INV");	
				}

				continue;
			}

			std::vector<std::string> inports;
			it->second->getInputPorts(inports);
			tobedeleted.push_back(it->first);

			std::vector<int> lutin;
			std::string pname = "I ";
			char startChar =  48;
			int bitlength= 1;

			//LSB-MSB
			for(unsigned int i = 0; i < inputs.size(); i++){
				bitlength*=2;
				pname[1] = startChar + i;
				//printf("%s:", pname.c_str());
				for(unsigned int j = 0; j < inports.size(); j++){
					if(inports[j] == pname){
						lutin.push_back(inputs[j]->getID());
						//printf("%d ", inputs[j]->getID());
					}
				}
			}
			//printf("\n");

			//printf("%llx BitLength: %d\n", function, bitlength); 

			//Prepare input file for espresso logic minimizer
			std::ofstream out("espresso.in");
			out<<".i "<<inputs.size()<<"\n";
			out<<".o 1\n";
			for(int i = 0; i < bitlength; i++){
				//On set only
				if((0x1 & function) == 1){
					int mask = 1<<(lutin.size() - 1);

					//LSB->MSB
					for(unsigned int j = 0; j < lutin.size(); j++){
						if((mask & i) > 0)    out<<"1"; //Positive Input
						else                  out<<"0"; //Negated Input
						mask = mask >> 1;
					}
					out<<" 1\n"; //On set
				}

				function= function>> 1;
			}
			out<<".e\n";
			out.close();


			if(((double)it->first)/endIndex >= percentComplete){
				printf(" -- Running Espresso...\t%0.2f Complete\n", it->first/endIndex);
				percentComplete+= 0.1;

			}
			std::system("./espresso espresso.in > espresso.out");

			//Read in output from espresso	
			std::ifstream in("espresso.out");
			if (!in.is_open())	{
				fprintf(stderr, "[ERROR] -- Cannot open the database for import\n");
				exit(-1);
			}

			//First two lines are dummy lines
			std::string dummyLines;
			getline(in, dummyLines);
			getline(in, dummyLines);
			in >> dummyLines;
			if(dummyLines == ".p"){
				in >> bitlength;
			}
			else{
				printf("[ERROR SEQ:replaceLUT] -- Espresso output error\n");
				printf("\tMake sure espresso is compiled correctly\n");
			}






			//Create subgraph for combinational logic of LUT	
			Graph* lutgraph = new Graph ("LUT");

			//Pre-Invert inputs for later use;
			//printf("ASSIGNING INPUTS\n");
			for(unsigned int i = 0; i < lutin.size(); i++){
				std::stringstream ss; 
				ss<<"I"<<i;
				Vertex* vin = lutgraph->addVertex(i, "IN");
				Vertex* vinv = lutgraph->addVertex(lutin.size()+i, "INV");

				vinv->addInput(vin, "I");
				vin->addOutput(vinv, "O");

				lutgraph->addInput(ss.str(), i);
				//printf("LUTIN: %d\tLUTGID: %d\tPORTNAME: %s\n", lutin[i], i, ss.str().c_str());
			}

			std::vector<int> clauses;
			//lutgraph->print();

			for(int i = 0; i < bitlength; i++){
				std::string inputSum;
				std::string onset;
				in>>inputSum;
				in>>onset;
				assert(onset == "1"); //Make sure it is an onset	

				//printf("INPUT SUM: %s\n", inputSum.c_str());

				//Check if clause is a literal
				int numLiteral = 0; 
				int singleInputBit;
				for(unsigned int j = 0; j < inputSum.length(); j++){
					char bit = inputSum[inputSum.length()-1-j];

					if(bit != '-'){
						numLiteral++;

						if(numLiteral > 1) break;
						else if(bit == '0')
							singleInputBit = lutin.size() + j; 
						else if (bit == '1')
							singleInputBit = j; 
					}
				}

				if(numLiteral == 1){
					clauses.push_back(singleInputBit);
					continue;
				}


				std::stringstream ss; 
				ss<<"AND";   //Set the size later. Some input may be don't care
				Vertex* aGate = new Vertex(lutgraph->getNumVertex(), ss.str());
				//printf("Making %s gate\n", ss.str().c_str());

				int numAndInput= 0;

				for(unsigned int j = 0; j < inputSum.length(); j++){
					char bit = inputSum[inputSum.length()-1-j];
					//printf("BIT: %c\n", bit);
					//Add port name
					std::stringstream portname; 
					portname<<"I"<<numAndInput;

					//Set inputs
					if(bit == '-') continue; //Don't care bit
					else if (bit == '1'){
						aGate->addInput(lutgraph->getVertex(j), portname.str());
						lutgraph->getVertex(j)->addOutput(aGate, "O");
					}
					else{
						aGate->addInput(lutgraph->getVertex(lutin.size()+j), portname.str());
						lutgraph->getVertex(lutin.size()+j)->addOutput(aGate, "O");
					}

					numAndInput++;
				}


				//Set the size of the And label
				clauses.push_back(lutgraph->getNumVertex());

				ss<<numAndInput;
				assert(numAndInput != 1);
				aGate->setType(ss.str());

				lutgraph->addVertex(aGate);
			}
			in.close();

			if(bitlength> 64){
				printf("NUMBER OF ONES IN TRUTH TABLE EXCEED 64. Please Adjust Code\n");
				assert(bitlength<= 64);
			}






			//Count how many and gates there are 
			int numAndGates = clauses.size();
			int numOrGates = (numAndGates) / 8;
			int numAndGatesLeft = (numAndGates) % 8;
			int orIndexStart = lutgraph->getNumVertex();

			//printf("and: %d  or: %d leftAnd: %d orStart %d\n", numAndGates, numOrGates, numAndGatesLeft, orIndexStart);	
			if(numAndGates != 1){
				Vertex* oGate;
				assert((numOrGates+1) <=8);


				//printf("FIRST OR\n");
				//Make first level orgate of size 8
				for(int i = 0; i < numOrGates; i++){
					oGate = lutgraph->addVertex(lutgraph->getNumVertex(), "OR8");

					for(int q = 0; q < 8; q++){
						std::stringstream portname; 
						portname<<"I"<<q;

						Vertex* gate = lutgraph->getVertex(clauses[(8*i)+q]);
						//printf("CLAUSE: %d   INDEX: %d\n", clauses[(8*i)+q], 8*i+q);
						oGate->addInput(gate, portname.str());
						gate->addOutput(oGate, "O");
					}
				}


				//Left over and gates that are encompassed above. (Make sure there is not 1 left)
				std::stringstream orgatename;
				if(numAndGatesLeft > 1){
					//printf("NOT IN HERE\n");
					orgatename<<"OR"<<numAndGatesLeft;
					oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());

					for(int q = 0; q < numAndGatesLeft; q++){
						std::stringstream portname; 
						portname<<"I"<<q;

						Vertex* gate = lutgraph->getVertex(clauses[(8*numOrGates)+q]);
						oGate->addInput(gate, portname.str());
						gate->addOutput(oGate, "O");
					}
				}


				//COmbine or gates
				int numOrGatesLeft = lutgraph->getNumVertex()-orIndexStart;
				//printf("numorgates: %d  numogateleft: %d\n", numOrGates, numOrGatesLeft);			
				if(numOrGatesLeft > 1 || numAndGatesLeft == 1){
					orgatename.str("");
					orgatename<<"OR"<< numOrGatesLeft;
					oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());

					for(int q = 0; q < numOrGatesLeft; q++){
						std::stringstream portname; 
						portname<<"I"<<q;

						Vertex* gate = lutgraph->getVertex(orIndexStart+q);
						oGate->addInput(gate, portname.str());
						gate->addOutput(oGate, "O");
					}

					//Make room for the single and gate that's left out
					if(numAndGatesLeft == 1){
						//Rename
						orgatename.str("");
						orgatename<<"OR"<< numOrGatesLeft+1;
						oGate->setType(orgatename.str());

						//Add port
						std::stringstream portname; 
						portname<<"I"<<numOrGatesLeft;

						//Get the and gate which is one before the or gate
						Vertex* gate = lutgraph->getVertex(clauses[clauses.size()-1]); 
						oGate->addInput(gate, portname.str());
						gate->addOutput(oGate, "O");
					}
				}
			}

			lutgraph->addOutput("O", lutgraph->getNumVertex()-1);

			//Remove unused inverted inputs
			for(unsigned int i = 0; i < lutin.size(); i++){

				if(lutgraph->getVertex(lutin.size() + i)->getNumOutputs() == 0){
					//printf("HERE V: %d IN: %d RM OV: %d\n", lutgraph->getVertex(lutin.size()+i)->getID(), i, lutin.size() +i );
					lutgraph->getVertex(i)->removeOutputValue(lutin.size()+i);
					lutgraph->removeVertex(lutin.size()+i);	
					//printf("DONE\n");
				}

			}

			lutgraph->renumber(ckt->getLast() + 1);
			//lutgraph->print();
			VERIFICATION::verifyLUT(it->second->getLUT(), lutgraph);
			std::vector<Vertex*> in2;
			std::vector<Vertex*> out2;
			it->second->getInput(in2);
			it->second->getOutput(out2);

			//printf("SUBBING %d\n", it->second->getID());
			ckt->substitute(it->second->getID(), lutgraph);
			//printf("HERE\n");
		}
	}

	std::list<int>::iterator it_list;
	for(it_list = tobedeleted.begin(); it_list != tobedeleted.end(); it_list++){
		ckt->removeVertex(*it_list);	
	}


	return numLUTs;
}








int SEQUENTIAL::DFS(std::list<int>& mark, Vertex* start, Graph* ckt, std::vector<std::vector<int> >& clauses){
	mark.push_back(start->getID());

	std::string type = start->getType();

	std::vector<Vertex*> inputs;
	start->getInput(inputs);

	//EXTRACT CLAUSES 	
	if(type == "INV"){
		//printf("INV\n");
		std::vector<int> clause1;
		std::vector<int> clause2;
		clause1.push_back(-1*inputs[0]->getID());
		clause1.push_back(-1 * start->getID());
		//printf("CLAUSE: INV: %d %d\n", clause1[0], clause1[1]);

		clause2.push_back(inputs[0]->getID());
		clause2.push_back(start->getID());
		//printf("CLAUSE: INV: %d %d\n", clause2[0], clause2[1]);

		clauses.push_back(clause1);
		clauses.push_back(clause2);
	}
	else if(type.find("AND") != std::string::npos){
		//	printf("AND\n");
		std::vector<int> clause1;
		std::vector<int> clause2;
		std::vector<int> clause3;
		clause1.push_back(-1*inputs[0]->getID());
		clause1.push_back(-1*inputs[1]->getID());
		clause1.push_back(start->getID());
		//printf("CLAUSE: AND: %d %d\n", clause1[0], clause1[1]);

		clause2.push_back(inputs[0]->getID());
		clause2.push_back(-1 * start->getID());
		//printf("CLAUSE: AND: %d %d\n", clause2[0], clause2[1]);

		clause3.push_back(inputs[1]->getID());
		clause3.push_back(-1 * start->getID());
		//	printf("CLAUSE: AND: %d %d\n", clause3[0], clause3[1]);

		clauses.push_back(clause1);
		clauses.push_back(clause2);
		clauses.push_back(clause3);

	}
	else if (type.find("XOR") != std::string::npos){
		printf("XOR\n");
		std::vector<int> clause1;
		std::vector<int> clause2;
		std::vector<int> clause3;
		std::vector<int> clause4;
		clause1.push_back(inputs[0]->getID());
		clause1.push_back(inputs[1]->getID());
		clause1.push_back(-1 * start->getID());

		clause2.push_back(-1 * inputs[0]->getID());
		clause2.push_back(-1 * inputs[1]->getID());
		clause2.push_back(-1 * start->getID());

		clause3.push_back(-1 * inputs[0]->getID());
		clause3.push_back(inputs[1]->getID());
		clause3.push_back(start->getID());

		clause4.push_back(inputs[0]->getID());
		clause4.push_back(-1 * inputs[1]->getID());
		clause4.push_back(start->getID());

		clauses.push_back(clause1);
		clauses.push_back(clause2);
		clauses.push_back(clause3);
		clauses.push_back(clause4);
	}

	else if (type.find("MUX") != std::string::npos){
		printf("MUX\n");
		std::vector<std::string> inports;
		start->getInputPorts(inports);
		int s, in0, in1;

		for(unsigned int i = 0; i < inports.size(); i++){
			if(inports[i] == "DI")
				in0 = inputs[i]->getID();
			else if(inports[i] == "CI")
				in1 = inputs[i]->getID();
			else
				s = inputs[i]->getID();
		}

		std::vector<int> clause1;
		std::vector<int> clause2;
		std::vector<int> clause3;
		std::vector<int> clause4;

		clause1.push_back(in0);
		clause1.push_back(in1);
		clause1.push_back(s);
		clause1.push_back(-1 * start->getID());

		clause2.push_back(in0);
		clause2.push_back(-1 * in1);
		clause2.push_back(s);
		clause2.push_back(-1 * start->getID());

		clause3.push_back(in0);
		clause3.push_back(in1);
		clause3.push_back(-1* s);
		clause3.push_back(-1 * start->getID());

		clause4.push_back(-1 * in0);
		clause4.push_back(in1);
		clause4.push_back(-1 * s);
		clause4.push_back(-1 * start->getID());

		clauses.push_back(clause1);
		clauses.push_back(clause2);
		clauses.push_back(clause3);
		clauses.push_back(clause4);
	}
	else if(type.find("VCC")!= std::string::npos){
		std::vector<int> clause;
		clause.push_back(start->getID());
		clauses.push_back(clause);
	}
	else if(type.find("GND")!= std::string::npos){
		std::vector<int> clause;
		clause.push_back(-1 * start->getID());
		clauses.push_back(clause);
	}
	else if (type.find("IN")!= std::string::npos){

	}
	else if (type.find("FD") != std::string::npos){

	}
	else{
		printf("TYPE UNKNOWN: %s\n", type.c_str());
		exit(1);
	}


	//SEARCH FOR NEXT
	int combo = 0; 
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(inputs[i]->getType().find("FD") == std::string::npos){
			combo+= DFS(mark,inputs[i], ckt, clauses);
		}
	}

	return combo;
}

void SEQUENTIAL::DFS_FF(std::set<unsigned>& mark, std::set<unsigned>& ffFound, Vertex* ff){
	unsigned ID = ff->getID();
	mark.insert(ID);
	std::vector<Vertex*> in;
	ff->getInput(in);

	for(unsigned int i = 0; i < in.size(); i++){
		std::string type = in[i]->getType();

		if(type == "IN") continue;

		else if(type.find("FD") != std::string::npos)
			ffFound.insert(in[i]->getID());

		else if(mark.find(in[i]->getID()) == mark.end())
			DFS_FF(mark, ffFound, in[i]);
	}
}



bool SEQUENTIAL::checkContainment(std::set<unsigned>& small, std::set<unsigned>& large){
	std::set<unsigned>::iterator iSet;
	for(iSet = small.begin(); iSet != small.end(); iSet++){
		if(large.find(*iSet) == large.end())
			return false;
	}
	return true;
}


unsigned SEQUENTIAL::findNumMismatch(std::set<unsigned>& small, std::set<unsigned>& large, unsigned maxMismatch){
	assert(small.size() == large.size());
	std::set<unsigned>::iterator iSet;
	unsigned mismatch = 0; 
	for(iSet = small.begin(); iSet != small.end(); iSet++){
		if(large.find(*iSet) == large.end()){
			mismatch++;
			if(mismatch > maxMismatch) return mismatch;
		}
	}
	return mismatch;
}







/*#############################################################################
 *
 *  cascadingFF 
 *    Looks for possible cascading input dependence
 *
 *#############################################################################*/
bool SEQUENTIAL::cascade(std::map<unsigned, std::list<InOut* > >::iterator iMap, 
		std::list<InOut*>::iterator  small, 
		unsigned cascadeGap, 
		std::set<unsigned>& ffs){

	std::set<unsigned>::iterator iSet;
	/*
		 printf("Checking set: \n");
		 for(iSet = small->begin(); iSet != small->end(); iSet++)
		 printf("%d ", *iSet);
		 printf("\n");
		 */


	std::map<unsigned, std::list<InOut* > >::iterator iMap2;
	std::list<InOut*>::iterator iList;
	iMap2 = iMap;
	iMap2++;

	//Check to see if there is a next size up
	if(iMap->first == iMap2->first - cascadeGap){

		//Go through each one of the sizes and see 
		//if the smaller is fully contained in the larger
		iList = iMap2->second.begin();
		while(iList != iMap2->second.end()){

/*
			for(iSet = iList->begin(); iSet != iList->end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
			*/

			if(checkContainment((*small)->input, (*iList)->input)){
				//printf("CONTAINED!\n");
				if(cascade(iMap2, iList, cascadeGap, ffs)){
					ffs.insert((*iList)->output);
					iMap->second.erase(iList);
				}
				return true;
			}
			/*
			else
				printf("NOT CONTAINED\n");
				*/
			iList++;
		}
	}
	/*
	else
		printf("NONE HIGHER\n");

	printf("DONE!!!!!!!!!\n\n");
	*/
	return false;
}
















/*#############################################################################
 *
 *  cascadingFF 
 *    Looks for possible cascading input dependence
 *
 *#############################################################################*/
void SEQUENTIAL::cascadingFF(Graph* ckt, std::list<InOut*>& ffList,  unsigned cascadeGap, std::map<unsigned, unsigned>& result){
	printf("[SEQ] -- Searching for cascading FF structure...GAP: %d\n", cascadeGap);

	//size, set of ff input dependence
	std::map<unsigned, std::list<InOut*> > orderedFF;
	std::map<unsigned, std::list<InOut*> >::iterator iMap;
	std::map<unsigned, std::list<InOut*> >::iterator iMap2;

	std::list<InOut*>::iterator iListFF;
	for(iListFF = ffList.begin(); iListFF != ffList.end(); iListFF++){
			orderedFF[(*iListFF)->input.size()].push_back(*iListFF);
	}

	std::set<unsigned>::iterator iSet;
	std::set<unsigned>::iterator iSet2;
	std::list<InOut*>::iterator iList;


/*
	for(iMap = orderedFF.begin(); iMap != orderedFF.end(); iMap++){
		printf("SIZE: %d\n", iMap->first);
		for(iList = iMap->second.begin(); iList != iMap->second.end(); iList++){
			for(iSet = iList->begin(); iSet != iList->end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}
	}
	*/




	//printf("Cascading structure:\n");
	//cascade size (Depth), count;
	std::map<unsigned, unsigned>::iterator iRes;

	//Go through each FF based on the size of the input dependence
	for(iMap = orderedFF.begin(); iMap != orderedFF.end(); iMap++){
		iMap2 = iMap;
		iMap2++;

		//Check to see if there is a next size up
		if(iMap->first == iMap2->first - cascadeGap){

			//Go through each one of the sizes and see 
			//if the smaller is fully contained in the larger
			iList = iMap->second.begin();
			while(iList != iMap->second.end()){
				std::set<unsigned> ffs;
				if(cascade(iMap, iList, cascadeGap, ffs)){
					ffs.insert((*iList)->output);
					iList = iMap->second.erase(iList);

					//printf("Cascade size: %d\n", (int)ffs.size());
					if(result.find(ffs.size()) == result.end()) result[ffs.size()] = 1;
					else result[ffs.size()]++;

					if(iMap->second.size() == 0) break;
				}
				else
					iList++;
			}

		}
	}


	for(iRes = result.begin(); iRes != result.end(); iRes++)
		printf("Complete block size M0 SIZE(L): %3d\t\tblock size(H): %3d\n", (int)iRes->first, iRes->second);

}






/*#############################################################################
 *
 *  blockFF 
 *    Looks for possible blocks of FF with input dependence
 *
 *#############################################################################*/
void SEQUENTIAL::blockFF(Graph* ckt, std::list<InOut*>& ffList,  unsigned maxMismatch, std::map<unsigned, unsigned>& result){
	printf("[SEQ] -- Searching for Block FF structure\n");
	printf(" * Mismatch set: %d\n", maxMismatch);

	//size, set of ff input dependence
	std::map<unsigned, std::list<InOut*> > orderedFF;
	std::map<unsigned, std::list<InOut*> >::iterator iMap;
	std::map<unsigned, std::list<InOut*> >::iterator iMap2;


	//printf("[SEQ] -- Finding possible counter FF\n");
	std::list<InOut*>::iterator iListFF;
	for(iListFF = ffList.begin(); iListFF != ffList.end(); iListFF++){
			orderedFF[(*iListFF)->input.size()].push_back(*iListFF);
	}

	/*
		 for(iMap = orderedFF.begin(); iMap != orderedFF.end(); iMap++){
		 printf("SIZE: %d\n", iMap->first);
		 for(iList = iMap->second.begin(); iList != iMap->second.end(); iList++){
		 for(iSet = iList->begin(); iSet != iList->end(); iSet++)
		 printf("%d ", *iSet);
		 printf("\n");
		 }
		 }
	 */


	//printf("Cascading structure:\n");
	//cascade size, count;
	std::map<std::set<unsigned>, unsigned> resultTemp;
	std::map<std::set<unsigned>, unsigned>::iterator iTemp;
	std::set<unsigned>::iterator iSet;
	std::list<InOut*>::iterator iList;
	std::list<InOut*>::iterator iList2;

	for(iMap = orderedFF.begin(); iMap != orderedFF.end(); iMap++){
		if(iMap->second.size() == 1) continue;

		//Look for exact matches with sets that are the same size
		iList = iMap->second.begin();
		while(iList != iMap->second.end()){
			iList2 = iList;
			iList2++;
			bool found = false;

			/*
				 printf("Checking set: \n");
				 for(iSet = iList->begin(); iSet != iList->end(); iSet++)
				 printf("%d ", *iSet);
				 printf("\n");
			 */



			while(iList2 != iMap->second.end()){
				unsigned mismatch = findNumMismatch((*iList)->input, (*iList2)->input, maxMismatch);

				//Makes sure the mismatch of a specific block remains the same throughout
				//Don't mix mismatches

				//Handle mismatches in the FF blocks
				if(mismatch == maxMismatch){
					if(resultTemp.find((*iList)->input) == resultTemp.end()) resultTemp[(*iList)->input] = 1;
					else resultTemp[(*iList)->input]++;

					found = true;
					iList2 = iMap->second.erase(iList2);
				}
				else iList2++;
			}

			/*
				 unsigned mismatchFound = 10;
				 while(iList2 != iMap->second.end()){
				 unsigned mismatch = findNumMismatch(*iList, *iList2);

			//Makes sure the mismatch of a specific block remains the same throughout
			//Don't mix mismatches
			if(mismatchFound == 10) 
			if(mismatch < 3)
			mismatchFound = mismatch;

			if(mismatch != mismatchFound){
			iList2++;
			continue;
			}

			//Handle mismatches in the FF blocks
			if(mismatch < 3){
			if(mismatch == 0){
			if(result.find(*iList) == result.end()) result[*iList] = 1;
			else result[*iList]++;
			//printf("CONTAINED----COUNT: %d\n", result[*iList]);
			}
			else if(mismatch == 1){
			if(result1.find(*iList) == result1.end()) result1[*iList] = 1;
			else result1[*iList]++;
			}
			else if(mismatch == 2){
			if(result2.find(*iList) == result2.end()) result2[*iList] = 1;
			else result2[*iList]++;
			}

			found = true;
			iList2 = iMap->second.erase(iList2);
			}
			else iList2++;
			}
			 */

			if(found) iList = iMap->second.erase(iList);
			else	iList++;
		}
	}

	std::map<unsigned, unsigned>::iterator iRes;
	for(iTemp = resultTemp.begin(); iTemp != resultTemp.end(); iTemp++){
		iRes= result.find(iTemp->first.size());
		if(iRes== result.end()){
			result[iTemp->first.size()] = iTemp->second;
		}
		else{
			if(iRes->second < iTemp->second)
				iRes->second = iTemp->second;
		}
	}


	for(iRes = result.begin(); iRes != result.end(); iRes++)
		printf("Complete block size M0 SIZE(L): %3d\t\tblock size(H): %3d\n", (int)iRes->first, iRes->second);
	

}










/*#############################################################################
 *
 *  counterIdentification 
 *    Looks for possible counters 	
 *
 *#############################################################################*/
void SEQUENTIAL::counterIdentification(Graph* ckt, std::list<InOut*>& ffList,  std::map<unsigned, unsigned>& result){
	printf("[SEQ] -- Counter Identification\n");

	//FF, set of ff input dependence
	std::map<unsigned, std::set<unsigned> > ffDependence;

	//Possible FF start, possible counter aggregation
	std::map<unsigned, std::set<unsigned> > possibleStart;

	std::list<InOut*>::iterator iListFF;
	for(iListFF = ffList.begin(); iListFF != ffList.end(); iListFF++){
		ffDependence[(*iListFF)->output] = (*iListFF)->input;
		if((*iListFF)->input.size() == 1)
			possibleStart[(*iListFF)->output];
	}

	std::map<unsigned, std::set<unsigned> >::iterator iMap;
	std::map<unsigned, std::set<unsigned> >::iterator iMap2;
	std::set<unsigned>::iterator iSet;
	std::set<unsigned>::iterator iSet2;
	/*
		 for(iMap = ffDependence.begin(); iMap != ffDependence.end(); iMap++){
		 printf("FF %3d SZ: %3d\t", iMap->first, (int)iMap->second.size()) ;
		 printf(" * ");
		 for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++){
		 printf("%d ", *iSet);
		 }
		 printf("\n");

		 }


		 printf("POSSIBLE STARTING NODES FOR COUNTER: "); 
		 for(iMap2 = possibleStart.begin(); iMap2 != possibleStart.end(); iMap2++){
		 printf("%d ", iMap2->first);
		 }
		 printf("\n");
	 */


	for(iMap2 = possibleStart.begin(); iMap2 != possibleStart.end(); iMap2++){

		unsigned start = iMap2->first;
		for(iMap = ffDependence.begin(); iMap != ffDependence.end(); iMap++){
			if(iMap->second.find(start) != iMap->second.end())
				iMap2->second.insert(iMap->first);
		}

	}


	/*
		 for(iMap2 = possibleStart.begin(); iMap2 != possibleStart.end(); iMap2++){
		 printf("START NODE: %d\t\t", iMap2->first);
		 for(iSet = iMap2->second.begin(); iSet != iMap2->second.end(); iSet++){
		 printf("%d ", *iSet);
		 }
		 printf("\n");
		 }
	 */


	//Go through each of the possible FF counter
	std::set<unsigned> marked;
	for(iMap2 = possibleStart.begin(); iMap2 != possibleStart.end(); iMap2++){
		//printf("START NODE: %d\t\t", iMap2->first);

		//Go through the nodes that contain the start node
		std::vector<unsigned> tobedeleted;
		for(iSet = iMap2->second.begin(); iSet != iMap2->second.end(); iSet++){
			iMap = ffDependence.find(*iSet);	

			//Go through the dependence of that node to see if it is all contained
			bool contained = true;
			for(iSet2 = iMap->second.begin(); iSet2 != iMap->second.end(); iSet2++){
				if(iMap2->second.find(*iSet2) == iMap2->second.end()){
					contained = false;
					break;
				}
			}

			if(!contained) 
				tobedeleted.push_back(*iSet);
			else
				marked.insert(*iSet);


			//printf("%d ", *iSet);
		}

		for(unsigned int i= 0; i < tobedeleted.size(); i++){
			iMap2->second.erase(tobedeleted[i]);
		}


		//printf("\n");
	}


	//Look for similar sets

	for(iMap = ffDependence.begin(); iMap != ffDependence.end(); iMap++){
		if(marked.find(iMap->first) != marked.end()) continue;
		iMap2=iMap;
		iMap2++;

		for(; iMap2 != ffDependence.end(); iMap2++){
			//If not the same set size, Go to the next one
			if(iMap->second.size() != iMap2->second.size()) continue;
			if(marked.find(iMap2->first) != marked.end()) continue;


			bool match = true;
			for(iSet = iMap->second.begin(); iSet != iMap->second.end(); iSet++){
				if(iMap2->second.find(*iSet) == iMap2->second.end()){
					match = false;
					break;
				}
			}

			//If the sets are not the same, check next
			if(match){
				possibleStart[iMap->first].insert(iMap->first);
				possibleStart[iMap->first].insert(iMap2->first);
				marked.insert(iMap->first);
				marked.insert(iMap2->first);
			}
		}

		if(possibleStart[iMap->first].size() < iMap->second.size()) possibleStart.erase(iMap->first);
	}

	printf("Possible counters\n");
	std::map<unsigned, unsigned>::iterator iResult;
	for(iMap2 = possibleStart.begin(); iMap2 != possibleStart.end(); iMap2++){
		std::set<unsigned>::iterator iSet;
		if(iMap2->second.size() > 1){
			printf("COUNTER SIZE: %d\t\t", (int)iMap2->second.size());
			printf("START NODE: %d\t\t", iMap2->first);
			for(iSet = iMap2->second.begin(); iSet != iMap2->second.end(); iSet++){
				printf("%d ", *iSet);
			}
			printf("\n");

			iResult = result.find(iMap2->second.size());
			if(iResult == result.end()) result[iMap2->second.size()] = 1;
			else iResult->second++;
		}
	}

}







/*****************************************************************************
 * counterIdentification
 *   Looks for counters given a circuit
 *
 * @PARAMS: ckt- circuit to look for counters
 *****************************************************************************/
/*
	 void counterIdentification(Graph* ckt){
	 printf("[SEQ] -- Counter Identification\n");

	 std::set<int> ffset;
	 std::set<int>::iterator itff;

//map of <ff id label, vid in the graph>
std::map<int, int> vmap;
std::map<int, int> gb2ffmap;
std::map<int, int>::iterator mapit;
std::map<int, Vertex*>::iterator it;
std::vector<int> lsb;
GraphBoost* lcg = new GraphBoost();
int ffcount= 0;

//Find all the flip flops in the circuit
printf("[SEQ] -- Finding possible counter FF\n");
for(it = ckt->begin(); it != ckt->end(); it++){
if(it->second->getType().find("FD") != std::string::npos){
ffcount++;

//Check to see if Q loops back to D (Possible counter)		
std::list<int> mark;
std::set<int> ffsetsingle;
std::set<int> ffFound;
int vID = it->second->getID();
ffsetsingle.insert(vID);
int dport = it->second->getInputPortID("D");

//check to see if the input is an inverter
if(ckt->getVertex(dport)->getType() == "INV"){
//printf("INVERTER INPUT- FF: %d\n", vID);
std::vector<Vertex*> inv_in;
ckt->getVertex(dport)->getInput(inv_in);
if(inv_in[0]->getID() == vID){
//printf("LSB OF COUNTER\n");
lsb.push_back(vID);
}
}

ckt->DFSearchIn(mark, ffsetsingle, ckt->getVertex(dport), ffFound);
if(ffFound.size() > 0){
ffset.insert(vID);
//printf("POSSIBLE C FF: %d\n", vID);
int lcgVID = lcg->addVertex(vID);
vmap[vID] = lcgVID;
gb2ffmap[lcgVID] = vID;
}

}
}

assert(vmap.size() == ffset.size());

printf("[SEQ] -- Build LCG of Reference Circuit\n");
//list of map<FF, set of FF that leads into FF
for(itff = ffset.begin(); itff != ffset.end(); itff++){
Vertex* vff = ckt->getVertex(*itff);

std::set<int> ffFound; 
std::list<int> mark;
ckt->DFSearchOut(mark, vff, ffFound);
std::set<int>::iterator itffp;

int src= vmap[vff->getID()];
for(itffp = ffFound.begin(); itffp != ffFound.end(); itffp++){
std::map<int,int>::iterator vmapit;
vmapit = vmap.find(*itffp);
if(vmapit != vmap.end())
lcg->addEdge(src, vmapit->second);
}
}

//Build counter model starting with AT MOST 64 bit counter. 
//unsigned int initSize = lcg->getNumVertices();

std::set<int> adjChild;
std::set<int> adjChild2;
std::set<int>::iterator ititem;
std::set<int>::iterator iti;
std::list<std::vector<int> > candidateFF;
std::list<std::vector<int> >::iterator it_cand;

replaceLUTs(ckt);
printf("[SEQ] -- Converting Circuit to AIG Format\n");
AIG* aigraph2 = new AIG();	
aigraph2->convertGraph2AIG(ckt, true);

//TODO: This section has some errors
printf("[SEQ] -- Searching for Candidate Counters in LCG\n");

//For each flip flop in the LCG
for(unsigned int ff = 0; ff < lcg->getNumVertices(); ff++){
	//int gFFVertex =  gb2ffmap[ff];
	//printf("\n\n##########################################\n");
	//printf("Observing FF: %d\t", gFFVertex);
	//adjChild.clear();

	//Get the adjacent ffs. 
	lcg->getAdjacentOut(ff, adjChild);

	//printf("ADJ: ");
	//for(iti = adjChild.begin(); iti != adjChild.end(); iti++){
	//gFFVertex = gb2ffmap[*iti];
	//printf("%d ", gFFVertex);
	//}
	//printf("\n");

	//For each adjacent node to the src check to see if what is already found connects to it
	std::vector<int> currentCount;
	std::set<int> currentCountSet; 
	//gFFVertex =  gb2ffmap[ff];
	//printf("Current Counter add %d\n", gFFVertex);
	currentCount.push_back(ff);
	currentCountSet.insert(ff);

	for(iti = adjChild.begin(); iti != adjChild.end(); iti++){
		//int gFFVertex =  gb2ffmap[*iti];
		//	printf("\nObserving ADJ: %d\n", gFFVertex);

		//Skip if same
		if(*iti == (int)ff){
			//printf("SAME as INITIAL FF\n");
			continue;
		}


		adjChild2.clear();
		lcg->getAdjacentIn(*iti, adjChild2);

		//printf("NODES INTO DEST: %d\tSRC: ", gFFVertex);
		//for(ititem = adjChild2.begin(); ititem != adjChild2.end(); ititem++){
		//gFFVertex = gb2ffmap[*ititem];
		//printf("%d ", gFFVertex);
		//}
		//printf("\n");

		bool fail = false;
		unsigned int insertIndex; 
		//Check to see if previously found all go into the input of the vertex
		for(unsigned int i = 0; i < currentCount.size(); i++){
			if(adjChild2.find(currentCount[i]) == adjChild2.end()){
				//int gFFVertex2 = gb2ffmap[currentCount[i]];
				//gFFVertex = gb2ffmap[gFFVertex];
				//printf("%d-%d is not adjacent\n", gFFVertex2, gFFVertex);
				fail = true;
				insertIndex = i;
				break;
			}
		}

		if(fail){
			fail = false;
			//printf("Checking to see if node connects to rest of the current count\n");
			for(unsigned int i = insertIndex; i < currentCount.size() ; i++){
				if(!lcg->isAdjacent(*iti, currentCount[i])){
					//gFFVertex = gb2ffmap[*ititem];
					//int gFFVertex2 = gb2ffmap[currentCount[i]];
					//printf("%d-%d is not adjacentback\n", gFFVertex, gFFVertex2);
					fail = true;
					break;
				}
			}

			if(!fail){
				//gFFVertex =  gb2ffmap[*iti];
				//printf("Current Counter add %d\n", gFFVertex);
				currentCount.insert(currentCount.begin() + insertIndex, *iti);
			}
		}
		else{
			//gFFVertex =  gb2ffmap[*iti];
			//printf("Current Counter add %d\n", gFFVertex);
			currentCount.push_back(*iti);

		}

	}

	//Observe counters of size 3 and greater
	if(currentCount.size() < 3)
		continue;

	//Sort by size
	for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
		if(it_cand->size()< currentCount.size()){
			break;
		}
	}
	candidateFF.insert(it_cand, currentCount);





}

printf("CANDIDATE COUNTERS ******************************\n");
for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
	for(unsigned int i = 0; i < it_cand->size() ; i++){
		int gFFVertex =  gb2ffmap[(*it_cand)[i]];
		printf("%d ", gFFVertex);
	}
	printf("\n");

}
lcg->print();
printf("Early Termination\n");
//exit(1);
//Get Combinational fanin for each
std::set<int> gFF;
printf("\n * Searching for clauses...\n");
int combo = 0;
std::vector<int> satisfiable;
std::set<int> foundLatches;


for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
	std::vector<std::vector<int> > clauses;	

	if(it_cand->size() < 3){
		candidateFF.erase(it_cand);
		if(candidateFF.size() == 0)
			break;
		it_cand--;
		continue;

	}

	bool alreadyFoundLatch = false;
	for(unsigned int i = 0; i < it_cand->size(); i++){
		if(foundLatches.find((*it_cand)[i]) != foundLatches.end()){
			alreadyFoundLatch = true;
			break;
		}
	}
	if(alreadyFoundLatch){
		candidateFF.erase(it_cand);
		if(candidateFF.size() == 0)
			break;
		it_cand--;
		continue;
	}

	for(unsigned int i = 0; i < it_cand->size(); i++){
		\*
			printf(" * Latches in Counter...\n");

		for(unsigned int j = 0; j < i+1; j++){
			gFF.insert(gb2ffmap[candidateFF[q][j]]);
			printf("%d ", gb2ffmap[candidateFF[q][j]]);
		}
		*\

			int gFFVertex =  gb2ffmap[(*it_cand)[i]];
		//printf("\nFF: %d\t", gFFVertex);
		std::list<int> markx;
		int in2FF = ckt->getVertex(gFFVertex)->getInputPortID("D");
		//printf("Input ID: %d\n", in2FF);

		combo+= DFS(markx,ckt->getVertex(in2FF), ckt, clauses);
		//printf("\n\n");
	}


	printf("\n\nExporting all clauses...\n");
	std::stringstream ss;
	std::stringstream cc1;
	std::stringstream cc2;

	ss << "p cnf " << it_cand->size()+combo << " " << clauses.size()<< "\n";
	for(unsigned int i = 0; i < clauses.size(); i++){
		for(unsigned int j = 0; j < clauses[i].size(); j++){
			ss << clauses[i][j] << " " ;
		}
		ss << "0\n";
	}

	//When the bit changes to 1
	printf("creating clause for when counter bit changes to 1\n");

	std::vector<int> inputCandidate;
	inputCandidate.reserve(it_cand->size());

	for(unsigned int i = 0; i < it_cand->size();  i++){
		int gFFVertex =  gb2ffmap[(*it_cand)[i]];
		int in2FF = ckt->getVertex(gFFVertex)->getInputPortID("D");
		inputCandidate.push_back(in2FF);
		//printf("Input ID: %d\n", in2FF);
	}


	int isSat = 1;
	//Check when bit goes low to high
	printf("Performing Low to High Bit transition test\n");
	\*
		for(unsigned int i = 0; i < it_cand->size();  i++){
			int gFFVertex =  gb2ffmap[(*it_cand)[i]];
			//printf("%d ", gFFVertex);

		}
	*\
		//printf("\n");
		for(unsigned int i = 0; i < it_cand->size();  i++){

			int gFFVertex =  gb2ffmap[(*it_cand)[i]];
			cc2.str("");
			cc2<<-1*gFFVertex<<" 0\n";
			cc2<<inputCandidate[i]<<" 0\n";
			//for(int j = i+1; j < candidateFF[q].size(); j++)
			for(unsigned int j = 0; j < i; j++)
			{
				int gFFVertex2 =  gb2ffmap[(*it_cand)[j]];
				cc2<<gFFVertex2<<" 0\n";
				cc2<<-1 * inputCandidate[j]<<" 0\n";
			}
			//printf("%s", ss.str().c_str());
			//printf("%s\n\n", cc2.str().c_str());
			std::ofstream fs("cnf");
			fs<<ss.str()<<cc2.str();
			fs.close();


			//Run Minisat
			std::system("./minisat cnf out > /dev/null");
			std::ifstream is("out");
			std::string sat;
			is>>sat;
			is.close();
			if(sat == "UNSAT"){
				isSat = 0;
				break;
			}
			else if(sat != "SAT")
				printf("UNKNOWN LINE IN MINISAT OUTPUT FILE\n");
		}

	//Check when bit stays high
	if(isSat){
		printf("Performing High stay High test\n");
		//Omit the first bit since the first bit always toggles
		for(unsigned int i = 1; i < it_cand->size();  i++){
			int gFFVertex =  gb2ffmap[(*it_cand)[i]];
			cc2.str("");
			cc2<<gFFVertex<<" 0\n";
			cc2<<inputCandidate[i]<<" 0\n";
			bool into = false;
			for(unsigned int j = 0; j < i; j++)
			{
				int gFFVertex2 =  gb2ffmap[(*it_cand)[j]];
				cc2<<-1* gFFVertex2<<" ";
				into = true;
			}
			if(into)
				cc2<<"0\n";
			std::ofstream fs("cnf");
			fs<<ss.str()<<cc2.str();
			fs.close();
			//printf("%s\n\n", cc2.str().c_str());


			//Run Minisat
			std::system("./minisat cnf out");
			std::ifstream is("out");
			std::string sat;
			is>>sat;
			is.close();
			if(sat == "UNSAT"){
				isSat = 0;
				break;
			}
			else if(sat != "SAT")
				printf("UNKNOWN LINE IN MINISAT OUTPUT FILE\n");


		}
	}

	//CHeck to see if counter is satisfiable
	if(isSat){
		for(unsigned int i = 0; i < it_cand->size();  i++){
			foundLatches.insert((*it_cand)[i]);
		}
	}

	satisfiable.push_back(isSat);
}

int numCounter = 0;
int index = 0; 
printf("RESULTS------------------------------\n");
for(it_cand = candidateFF.begin(); it_cand != candidateFF.end(); it_cand++){
	for(unsigned int j = 0; j < it_cand->size(); j++){
		int gFFVertex =  gb2ffmap[(*it_cand)[j]];
		printf("%d ", gFFVertex);
	}
	printf(" ---------------\t");
	if(satisfiable[index] == 1){
		printf("SAT\n");
		numCounter++;
	}
	else
		printf("UNSAT\n");
	index++;
}
printf("NUM OF COUNTERS: %d\n", numCounter);
printf("NUM OF LATCHES:  %d\n", ffcount);
}
*/

//Unsimplified replacement
/*
	 unsigned int replaceLUT2(Graph* ckt){
	 printf("[SEQ] -- Replacing LUTs with combinational Logic  [2]\n");
	 std::map<int, Vertex*>::iterator it;
	 std::list<int> tobedeleted;
	 unsigned int numLUTs = 0;

	 for(it = ckt->begin(); it != ckt->end(); it++){
	 if(it->second->getType().find("LUT") != std::string::npos){
	 numLUTs++;
	 std::vector<Vertex*> inputs;
	 it->second->getInput(inputs);

	 if(inputs.size() == 1){
//printf("LUT SIZE 1\n");
unsigned long long function = it->second->getLUT();
if(function == 2){
std::vector<Vertex*> linput;
std::vector<Vertex*> loutput;

it->second->getInput(linput);
it->second->getOutput(loutput);

assert(linput.size() == 1);
std::string outportname = linput[0]->removeOutputValue(it->second->getID());

for(unsigned int i = 0; i < loutput.size(); i++){
std::string portname = loutput[i]->getInputPortName(it->second->getID());
int index = loutput[i]->removeInputValue(it->second->getID());
loutput[i]->removeInPortValue(index);
loutput[i]->addInput(linput[0]);
loutput[i]->addInPort(portname);

linput[0]->addOutput(loutput[i],outportname);
}

tobedeleted.push_back(it->first);
}
else{
printf(" * FUNCTION IS INVERTER\n");
it->second->setType("INV");	
}

continue;
}

std::vector<std::string> inports;
it->second->getInputPorts(inports);
tobedeleted.push_back(it->first);

//LSB-MSB
std::vector<int> lutin;
std::string pname = "I ";
char startChar =  48;

int bitlength= 1;
//Handle input ordering
for(unsigned int i = 0; i < inputs.size(); i++){
bitlength*=2;
pname[1] = startChar + i;
//printf("%s:", pname.c_str());
for(unsigned int j = 0; j < inports.size(); j++){
if(inports[j] == pname){
lutin.push_back(inputs[j]->getID());
//printf("%d ", inputs[j]->getID());
}
}
}
//printf("\n");

unsigned long long function = it->second->getLUT();
//printf("%llx BitLength: %d\n", function, bitlength); 

//Prepare input file for espresso logic minimizer
std::ofstream out("espresso.in");
out<<".i "<<inputs.size()<<"\n";
out<<".o 1\n";


//Create Subgraph
Graph* lutgraph = new Graph ("LUT");


//Prepare inputs and prenegate
for(unsigned int i = 0; i < lutin.size(); i++){
	std::stringstream ss; 
	ss<<"I"<<i;
	Vertex* vin = lutgraph->addVertex(i, "IN");
	Vertex* vinv = lutgraph->addVertex(lutin.size()+i, "INV");

	vinv->addInput(vin);
	vinv->addInPort("I");
	vin->addOutput(vinv, "O");

	lutgraph->addInput(ss.str(), i);
	//printf("LUTIN: %d\tLUTGID: %d\tPORTNAME: %s\n", lutin[i], i, ss.str().c_str());
}


//Calculate onset
std::vector<int> clauses;
for(int i = 0; i < bitlength; i++){
	//On set only
	if((0x1 & function) == 1){
		int mask = 0x1;
		std::stringstream ss; 
		ss<<"AND";   //Set the size later. Some input may be don't care
		Vertex* aGate = new Vertex(lutgraph->getNumVertex(), ss.str());

		//LSB->MSB
		int numAndInput = 0;
		int inputPortIndex = 0;
		for(unsigned int j = 0; j < lutin.size(); j++){
			if((mask & i) > 0) { //Positive input
				aGate->addInput(lutgraph->getVertex(j));
				lutgraph->getVertex(j)->addOutput(aGate, "O");
				numAndInput++;
			}
			else {    //Negated Input
				aGate->addInput(lutgraph->getVertex(lutin.size()+j));
				lutgraph->getVertex(lutin.size()+j)->addOutput(aGate, "O");
				numAndInput++;
			}		
			std::stringstream portname; 
			portname<<"I"<<inputPortIndex;
			inputPortIndex++;
			aGate->addInPort(portname.str());

			mask = mask << 1;
		}
		ss<<numAndInput;
		assert(numAndInput != 1);
		aGate->setType(ss.str());
		lutgraph->addVertex(aGate);
		clauses.push_back(aGate->getID());
	}

	function= function>> 1;
}





if(bitlength> 64){
	printf("NUMBER OF ONES IN TRUTH TABLE EXCEED 64. Please Adjust Code\n");
	assert(bitlength<= 64);
}






//Count how many and gates there are 
int numAndGates = clauses.size();
int numOrGates = (numAndGates) / 8;
int numAndGatesLeft = (numAndGates) % 8;
int orIndexStart = lutgraph->getNumVertex();

//printf("and: %d  or: %d leftAnd: %d orStart %d\n", numAndGates, numOrGates, numAndGatesLeft, orIndexStart);	
if(numAndGates != 1){
	Vertex* oGate;
	assert((numOrGates+1) <=8);


	//printf("ANDG: %d\tOR8: %d\tANDLEFT:%d\n", lutgraph->getNumVertex() - andIndexStart, numOrGates, numAndGatesLeft);
	//Make first level orgate of size 8
	for(int i = 0; i < numOrGates; i++){
		oGate = lutgraph->addVertex(lutgraph->getNumVertex(), "OR8");

		for(int q = 0; q < 8; q++){
			std::stringstream portname; 
			portname<<"I"<<q;
			oGate->addInPort(portname.str());

			Vertex* gate = lutgraph->getVertex(clauses[(8*i)+q]);
			oGate->addInput(gate);
			gate->addOutput(oGate, "O");
		}
	}


	//Left over and gates that are encompassed above. (Make sure there is not 1 left)
	std::stringstream orgatename;
	if(numAndGatesLeft > 1){
		orgatename<<"OR"<<numAndGatesLeft;
		oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());

		for(int q = 0; q < numAndGatesLeft; q++){
			std::stringstream portname; 
			portname<<"I"<<q;
			oGate->addInPort(portname.str());

			Vertex* gate = lutgraph->getVertex(clauses[(8*numOrGates)+q]);
			oGate->addInput(gate);
			gate->addOutput(oGate, "O");
		}
	}


	//COmbine or gates
	int numOrGatesLeft = lutgraph->getNumVertex()-orIndexStart;
	//printf("numorgates: %d  numogateleft: %d\n", numOrGates, numOrGatesLeft);			
	if(numOrGatesLeft > 1 || numAndGatesLeft == 1){
		orgatename.str("");
		orgatename<<"OR"<< numOrGatesLeft;
		oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());

		for(int q = 0; q < numOrGatesLeft; q++){
			std::stringstream portname; 
			portname<<"I"<<q;
			oGate->addInPort(portname.str());

			Vertex* gate = lutgraph->getVertex(orIndexStart+q);
			oGate->addInput(gate);
			gate->addOutput(oGate, "O");
		}

		//Make room for the single and gate that's left out
		if(numAndGatesLeft == 1){
			//Rename
			orgatename.str("");
			orgatename<<"OR"<< numOrGatesLeft+1;
			oGate->setType(orgatename.str());

			//Add port
			std::stringstream portname; 
			portname<<"I"<<numOrGatesLeft;
			oGate->addInPort(portname.str());

			//Get the and gate which is one before the or gate
			Vertex* gate = lutgraph->getVertex(orIndexStart-1); 
			oGate->addInput(gate);
			gate->addOutput(oGate, "O");
		}
	}
}

lutgraph->addOutput("O", lutgraph->getNumVertex()-1);

//Remove unused inverted inputs
for(unsigned int i = 0; i < lutin.size(); i++){
	if(lutgraph->getVertex(lutin.size() + i)->getNumOutputs() == 0){
		lutgraph->getVertex(i)->removeOutputValue(lutin.size()+i);
		lutgraph->removeVertex(lutin.size()+i);	
	}
}

lutgraph->renumber(ckt->getLast() + 1);
//lutgraph->print();
VERIFICATION::verifyLUT(it->second->getLUT(), lutgraph);
ckt->substitute(it->second->getID(), lutgraph);
}
}

std::list<int>::iterator it_list;
for(it_list = tobedeleted.begin(); it_list != tobedeleted.end(); it_list++){
	ckt->removeVertex(*it_list);	
}


return numLUTs;
}

*/
