/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  SEQUENTIAL.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef SEQ_GUARD
#define SEQ_GUARD
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vf2_sub_graph_iso.hpp>

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

using namespace boost;

//Counter
namespace SEQUENTIAL{
	std::string primFunction;	
	void replaceLUTs(Graph* ckt);
	int DFS(std::list<int>& mark, Vertex<std::string>* start, Graph* ckt, std::vector<std::vector<int> >& clauses);


	/*****************************************************************************
	* counterIdentification
	*   Looks for counters given a circuit
	*
	* @PARAMS: ckt- circuit to look for counters
	*****************************************************************************/
	void counterIdentification(Graph* ckt){
		printf("\n\n\n");
		printf("[SEQ] -- Counter Identification-------------------------------\n");

		std::set<int> ffset;
		std::set<int>::iterator itff;

		//map of <ff id label, vid in the graph>
		std::map<int, int> vmap;
		std::map<int, int> gb2ffmap;
		std::map<int, int>::iterator mapit;
		std::map<int, Vertex<std::string>*>::iterator it;
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
				int vID = it->second->getVertexID();
				ffsetsingle.insert(vID);
				int dport = it->second->getInputPortID("D");

				//check to see if the input is an inverter
				if(ckt->getVertex(dport)->getType() == "INV"){
					//printf("INVERTER INPUT- FF: %d\n", vID);
					std::vector<Vertex<std::string>*> inv_in;
					ckt->getVertex(dport)->getInput(inv_in);
					if(inv_in[0]->getVertexID() == vID){
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
			Vertex<std::string>* vff = ckt->getVertex(*itff);

			std::set<int> ffFound; 
			std::list<int> mark;
			ckt->DFSearchOut(mark, vff, ffFound);
			std::set<int>::iterator itffp;

			int src= vmap[vff->getVertexID()];
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
			int gFFVertex =  gb2ffmap[ff];
			//printf("\n\n##########################################\n");
			//printf("Observing FF: %d\t", gFFVertex);
			//adjChild.clear();
			
			//Get the adjacent ffs. 
			lcg->getAdjacentOut(ff, adjChild);

				/*printf("ADJ: ");
				for(iti = adjChild.begin(); iti != adjChild.end(); iti++){
					gFFVertex = gb2ffmap[*iti];
					printf("%d ", gFFVertex);
				}
				printf("\n");
				*/

			//For each adjacent node to the src check to see if what is already found connects to it
			std::vector<int> currentCount;
			std::set<int> currentCountSet; 
			gFFVertex =  gb2ffmap[ff];
			//printf("Current Counter add %d\n", gFFVertex);
			currentCount.push_back(ff);
			currentCountSet.insert(ff);

			for(iti = adjChild.begin(); iti != adjChild.end(); iti++){
				int gFFVertex =  gb2ffmap[*iti];
			//	printf("\nObserving ADJ: %d\n", gFFVertex);

				//Skip if same
				if(*iti == (int)ff){
					//printf("SAME as INITIAL FF\n");
					continue;
				}


				adjChild2.clear();
				lcg->getAdjacentIn(*iti, adjChild2);

			/*	printf("NODES INTO DEST: %d\tSRC: ", gFFVertex);
				  for(ititem = adjChild2.begin(); ititem != adjChild2.end(); ititem++){
					gFFVertex = gb2ffmap[*ititem];
					printf("%d ", gFFVertex);
				  }
				  printf("\n");
				*/

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
						gFFVertex =  gb2ffmap[*iti];
						//printf("Current Counter add %d\n", gFFVertex);
						currentCount.insert(currentCount.begin() + insertIndex, *iti);
					}
				}
				else{
					gFFVertex =  gb2ffmap[*iti];
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

		/*printf("CANDIDATE COUNTERS ******************************\n");
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
		*/
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
				/*
				   printf(" * Latches in Counter...\n");

				   for(unsigned int j = 0; j < i+1; j++){
				   gFF.insert(gb2ffmap[candidateFF[q][j]]);
				   printf("%d ", gb2ffmap[candidateFF[q][j]]);
				   }
				 */

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
			/*
			   for(unsigned int i = 0; i < it_cand->size();  i++){
			   int gFFVertex =  gb2ffmap[(*it_cand)[i]];
			//printf("%d ", gFFVertex);

			}
			 */
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







	void replaceLUTs(Graph* ckt){
		printf("[SEQ] -- Replacing LUTs with combinational Logic\n");
		std::map<int, Vertex<std::string>*>::iterator it;
		std::vector<int> tobedeleted;
		for(it = ckt->begin(); it != ckt->end(); it++){
			if(it->second->getType().find("LUT") != std::string::npos){
				tobedeleted.push_back(it->first);
				std::vector<Vertex<std::string>*> inputs;
				it->second->getInput(inputs);

				if(inputs.size() == 1){
					//printf("LUT SIZE 1\n");
					unsigned long function = it->second->getLUT();
					if(function == 2){
						std::vector<Vertex<std::string>*> linput;
						std::vector<Vertex<std::string>*> loutput;

						it->second->getInput(linput);
						it->second->getOutput(loutput);

						assert(linput.size() == 1);
						std::string outportname = linput[0]->removeOutputValue(it->second->getVertexID());

						for(unsigned int i = 0; i < loutput.size(); i++){
							std::string portname = loutput[i]->getInputPortName(it->second->getVertexID());
							int index = loutput[i]->removeInputValue(it->second->getVertexID());
							loutput[i]->removeInPortValue(index);
							loutput[i]->addInput(linput[0]);
							loutput[i]->addInPort(portname);

							linput[0]->addOutput(loutput[i],outportname);

						}


					}
					else{
						printf("FUNCTION IS INVERTER\n");
						exit(1);
					}

					continue;
				}
				std::vector<std::string> inports;
				it->second->getInputPorts(inports);

				//LSB-MSB
				std::vector<int> lutin;
				std::string pname = "I ";
				char startChar =  48;

				int numbers = 1;
				for(unsigned int i = 0; i < inputs.size(); i++){
					numbers*=2;
					pname[1] = startChar + i;
					//printf("%s:", pname.c_str());
					for(unsigned int j = 0; j < inports.size(); j++){
						if(inports[j] == pname){
							lutin.push_back(inputs[j]->getVertexID());
							//printf("%d ", inputs[j]->getVertexID());
						}
					}
				}
				//printf("\n");

				unsigned long function = it->second->getLUT();
				//printf("%lx Possible numbers: %d\n", function, numbers); 
				Graph* lutgraph = new Graph ("LUT");
				//Pre-Invert inputs for later use;
				//printf("ASSIGNING INPUTS\n");
				for(unsigned int i = 0; i < lutin.size(); i++){
					std::stringstream ss; 
					ss<<"I"<<i;
					Vertex<std::string>* vin = lutgraph->addVertex(i, "IN");
					Vertex<std::string>* vinv = lutgraph->addVertex(lutin.size()+i, "INV");

					vinv->addInput(vin);
					vinv->addInPort("I");

					vin->addOutput(vinv, "O");

					lutgraph->addInput(ss.str(), i);
					//printf("LUTIN: %d\tLUTGID: %d\tPORTNAME: %s\n", lutin[i], i, ss.str().c_str());
				}

				int andIndexStart = lutgraph->getNumVertex();
				int count = 0;

				for(int i = 0; i < numbers; i++){
					//If there is a 1 in the truth table
					if((0x1 & function) == 1){
						count++;
						std::stringstream ss; 
						ss<<"AND"<<lutin.size();
						Vertex<std::string>* aGate = lutgraph->addVertex(lutgraph->getNumVertex(), ss.str());

						int mask = 1;
						for(unsigned int j = 0; j < lutin.size(); j++){
							std::stringstream portname; 
							portname<<"I"<<j;
							aGate->addInPort(portname.str());

							if((mask & i) > 0){
								aGate->addInput(lutgraph->getVertex(j));
								lutgraph->getVertex(j)->addOutput(aGate, "O");
							}
							else{
								aGate->addInput(lutgraph->getVertex(lutin.size()+j));
								lutgraph->getVertex(lutin.size()+j)->addOutput(aGate, "O");
							}
							mask = mask << 1;

						}
					}

					function= function>> 1;
				}
				if(count> 64){
					printf("NUMBER OF ONES IN TRUTH TABLE EXCEED 64. Please Adjust Code\n");
				}


				//Count how many and gates there are 
				int numOrGates = (lutgraph->getNumVertex() - andIndexStart) / 8;
				int numAndGatesLeft = (lutgraph->getNumVertex() - andIndexStart) % 8;
				int orIndexStart = lutgraph->getNumVertex();

				for(int i = 0; i < numOrGates; i++){
					Vertex<std::string>* oGate = lutgraph->addVertex(lutgraph->getNumVertex(), "OR8");

					for(int q = 0; q < 8; q++){
						std::stringstream portname; 
						portname<<"I"<<q;
						oGate->addInPort(portname.str());

						oGate->addInput(lutgraph->getVertex(andIndexStart+(8*i)+q));
						lutgraph->getVertex(andIndexStart+(8*i)+q)->addOutput(oGate, "O");
					}
				}

				std::stringstream orgatename;
				Vertex<std::string>* oGate;
				if(numAndGatesLeft > 1){
					orgatename<<"OR"<<numAndGatesLeft;

					oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());

					for(int q = 0; q < numAndGatesLeft; q++){
						std::stringstream portname; 
						portname<<"I"<<q;
						oGate->addInPort(portname.str());

						oGate->addInput(lutgraph->getVertex(andIndexStart+(8*numOrGates)+q));
						lutgraph->getVertex(andIndexStart+(8*numOrGates)+q)->addOutput(oGate, "O");
					}

				}

				//COmbine or gates
				int numOrGatesLeft = lutgraph->getNumVertex()-orIndexStart;
				if(numOrGates > 0 && numOrGatesLeft > 1){
					orgatename.str("");
					orgatename<<"OR"<< numOrGatesLeft;
					oGate = lutgraph->addVertex(lutgraph->getNumVertex(), orgatename.str());

					for(int q = 0; q < numOrGatesLeft; q++){
						std::stringstream portname; 
						portname<<"I"<<q;
						oGate->addInPort(portname.str());

						oGate->addInput(lutgraph->getVertex(orIndexStart+q));
						lutgraph->getVertex(orIndexStart+q)->addOutput(oGate, "O");
					}

				}

				lutgraph->addOutput("O", lutgraph->getNumVertex()-1);

				//printf("PRINTING LUT GRAPH\n");
				lutgraph->renumber(ckt->getLast() + 1);
				ckt->substitute(it->second->getVertexID(), lutgraph);

			}
		}

		for(unsigned int i = 0; i < tobedeleted.size(); i++){
			ckt->removeVertex(tobedeleted[i]);	
		}

	}








	int DFS(std::list<int>& mark, Vertex<std::string>* start, Graph* ckt, std::vector<std::vector<int> >& clauses){
		mark.push_back(start->getVertexID());

		std::string type = start->getType();

		std::vector<Vertex<std::string>*> inputs;
		start->getInput(inputs);

		//EXTRACT CLAUSES 	
		if(type == "INV"){
			//printf("INV\n");
			std::vector<int> clause1;
			std::vector<int> clause2;
			clause1.push_back(-1*inputs[0]->getVertexID());
			clause1.push_back(-1 * start->getVertexID());
			//printf("CLAUSE: INV: %d %d\n", clause1[0], clause1[1]);

			clause2.push_back(inputs[0]->getVertexID());
			clause2.push_back(start->getVertexID());
			//printf("CLAUSE: INV: %d %d\n", clause2[0], clause2[1]);

			clauses.push_back(clause1);
			clauses.push_back(clause2);
		}
		else if(type.find("AND") != std::string::npos){
			//	printf("AND\n");
			std::vector<int> clause1;
			std::vector<int> clause2;
			std::vector<int> clause3;
			clause1.push_back(-1*inputs[0]->getVertexID());
			clause1.push_back(-1*inputs[1]->getVertexID());
			clause1.push_back(start->getVertexID());
			//printf("CLAUSE: AND: %d %d\n", clause1[0], clause1[1]);

			clause2.push_back(inputs[0]->getVertexID());
			clause2.push_back(-1 * start->getVertexID());
			//printf("CLAUSE: AND: %d %d\n", clause2[0], clause2[1]);

			clause3.push_back(inputs[1]->getVertexID());
			clause3.push_back(-1 * start->getVertexID());
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
			clause1.push_back(inputs[0]->getVertexID());
			clause1.push_back(inputs[1]->getVertexID());
			clause1.push_back(-1 * start->getVertexID());

			clause2.push_back(-1 * inputs[0]->getVertexID());
			clause2.push_back(-1 * inputs[1]->getVertexID());
			clause2.push_back(-1 * start->getVertexID());

			clause3.push_back(-1 * inputs[0]->getVertexID());
			clause3.push_back(inputs[1]->getVertexID());
			clause3.push_back(start->getVertexID());

			clause4.push_back(inputs[0]->getVertexID());
			clause4.push_back(-1 * inputs[1]->getVertexID());
			clause4.push_back(start->getVertexID());

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
					in0 = inputs[i]->getVertexID();
				else if(inports[i] == "CI")
					in1 = inputs[i]->getVertexID();
				else
					s = inputs[i]->getVertexID();
			}

			std::vector<int> clause1;
			std::vector<int> clause2;
			std::vector<int> clause3;
			std::vector<int> clause4;

			clause1.push_back(in0);
			clause1.push_back(in1);
			clause1.push_back(s);
			clause1.push_back(-1 * start->getVertexID());

			clause2.push_back(in0);
			clause2.push_back(-1 * in1);
			clause2.push_back(s);
			clause2.push_back(-1 * start->getVertexID());

			clause3.push_back(in0);
			clause3.push_back(in1);
			clause3.push_back(-1* s);
			clause3.push_back(-1 * start->getVertexID());

			clause4.push_back(-1 * in0);
			clause4.push_back(in1);
			clause4.push_back(-1 * s);
			clause4.push_back(-1 * start->getVertexID());

			clauses.push_back(clause1);
			clauses.push_back(clause2);
			clauses.push_back(clause3);
			clauses.push_back(clause4);
		}
		else if(type.find("VCC")!= std::string::npos){
			std::vector<int> clause;
			clause.push_back(start->getVertexID());
			clauses.push_back(clause);
		}
		else if(type.find("GND")!= std::string::npos){
			std::vector<int> clause;
			clause.push_back(-1 * start->getVertexID());
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

}





#endif
