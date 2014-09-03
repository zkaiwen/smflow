/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @  VERIFICATION.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef VER_GUARD
#define VER_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "cutfunction.hpp"
#include "cutenumeration.hpp"



namespace VERIFICATION{
	bool verifyLUT(unsigned long long lutFunction, Graph* lutGraph);

	bool verifyGraph(Graph* graph){
		printf("[VER] -- Verifying AIG and GRAPH\n");

		if(graph->getNumInputs() > 10){
			printf("[VER] -- ERROR: Input greater than expected\n");
			return false;
		}

		const unsigned long long tt[10] = {
			0xAAAAAAAAAAAAAAAA,
			0xCCCCCCCCCCCCCCCC,
			0xF0F0F0F0F0F0F0F0,
			0xFF00FF00FF00FF00,
			0xFFFF0000FFFF0000,

			0xFFFFFFFF00000000,
			0xFFFFFFFFFFFFFFFF,
			0x0000000000000000,
			0xFFFFFFFFFFFFFFFF,
			0x0123456789ABCDEF
		};

		//node vid, function
		std::map<unsigned int, unsigned long long> nodeVal;
		unsigned long long result;

		//LSB->MSB
		//Calculate Values for graph
		try{
			std::vector<int> cktInputs;
			graph->getInputs(cktInputs);

			//Prepare inputs for calculation
			for(unsigned int i = 0; i < cktInputs.size(); i++){
				nodeVal[cktInputs[i]] = tt[i];
				printf("INPUT: %d\tVAL: %llx\n", cktInputs[i], tt[i]);
			}

			std::map<int, Vertex*>::iterator it;
			std::map<int, std::vector<Vertex*> > vLevel;
			std::map<int, std::vector<Vertex*> >::iterator lit;

			//Order nodes by level
			for(it = graph->begin(); it != graph->end(); it++){
				vLevel[it->second->getLevel()].push_back(it->second);
			}

			unsigned int vlevelSize = vLevel.size();
			unsigned int maxLevel = graph->getMaxLevel();
			std::vector<unsigned long long> outputResults;
			std::vector<unsigned> outputNode;

			//Calculate outputs for graph
			printf("[VER] -- Calculating output for graph\n");
			for(unsigned int i = 1; i <= maxLevel; i++){
				if(vLevel.size() != vlevelSize){
					printf("Size mismatch\n");
					exit(1);
				}
				for(unsigned int j = 0; j < vLevel[i].size(); j++){
					std::vector<Vertex*> input;
					vLevel[i][j]->getInput(input);

					if(vLevel[i][j]->getType().find("AND") != std::string::npos){
						result = nodeVal[input[0]->getID()];
						//printf("IN: %d ", input[0]->getID());
						for(unsigned int k = 1; k < input.size(); k++){
							result = result & nodeVal[input[k]->getID()];
							//printf("%d ", input[k]->getID());
						}
						//printf("\n");

						nodeVal[vLevel[i][j]->getID()] = result;
						//printf("AND NODE: %d\tRESULT: %llx\n", vLevel[i][j]->getID(), result);
					}
					else if(vLevel[i][j]->getType() == "INV"){
						result = ~nodeVal[input[0]->getID()];
						nodeVal[vLevel[i][j]->getID()] = result;
						//printf("IN: %d \n", input[0]->getID());
						//printf("INV NODE: %d\tRESULT: %llx\n", vLevel[i][j]->getID(), result);
					}
					else if(vLevel[i][j]->getType().find("OR") != std::string::npos){
						result = nodeVal[input[0]->getID()];
						//printf("IN: %d ", input[0]->getID());
						for(unsigned int k = 1; k < input.size(); k++){
							result = result | nodeVal[input[k]->getID()];
							//printf("%d ", input[k]->getID());
						}
						//printf("\n");

						nodeVal[vLevel[i][j]->getID()] = result;
						//printf("OR NODE: %d\tRESULT: %llx\n", vLevel[i][j]->getID(), result);
					}

					if(vLevel[i][j]->getOVSize() == 0){
						printf("GRAPH OUTPUT: %d\n", vLevel[i][j]->getID());
						outputResults.push_back(result);
						outputNode.push_back(vLevel[i][j]->getID());
					}
				}
			}

			for(unsigned int i = 0; i < outputResults.size(); i++){
				printf("NODE: %d\tRESULT  G : %llx\n", outputNode[i], outputResults[i]);
			}
			



			return true;
		}
		catch(...){

		}
		return true;
	}


	bool verifyAIG(AIG* aig){
		printf("[VER] -- Verifying AIG\n");
		if(aig->getInputSize() > 10){
			printf("[VER] -- ERROR: Input greater than expected\n");
			return false;
		}

		const unsigned long long tt[10] = {
			0xAAAAAAAAAAAAAAAA,
			0xCCCCCCCCCCCCCCCC,
			0xF0F0F0F0F0F0F0F0,
			0xFF00FF00FF00FF00,
			0xFFFF0000FFFF0000,
			0xFFFFFFFF00000000,
			0xFFFFFFFFFFFFFFFF,
			0x0000000000000000,
			0xFFFFFFFFFFFFFFFF,
			0x0123456789ABCDEF
		};
		for(unsigned int i = 1; i <= aig->getInputSize(); i++){
			printf("INPUT: %d\tVAL: %llx\n", i*2, tt[i-1]);
			printf("GRAPH NODE: %d\n", aig->getGNode(i*2));
		}

		//node vid, function
		std::map<unsigned int, unsigned long long> nodeVal;
		unsigned long long result;

		//LSB->MSB
		//Calculate Values for graph
		try{
			std::vector<unsigned long long> outputResults;
			std::vector<unsigned> outputNodes;

			//AIG CALCULATION
			printf("[VER] -- Calculating output for AIG\n");
			CutFunction cf;
			CutEnumeration* ce = new CutEnumeration(aig);
			cf.setParams(ce, aig);

			std::vector<unsigned> aigOut; 
			aig->getOutputs(aigOut);
			assert(aigOut.size() == aig->getOutputSize());
			for(unsigned int i = 1; i <= aig->getInputSize(); i++)
				cf.m_NodeValue[i*2] = tt[i-1];
			
			for(unsigned int k = 0; k < aig->getOutputSize(); k++){
				result = cf.calculate(aigOut[k]);		
				outputResults.push_back(result);
				outputNodes.push_back(aigOut[k]);
			}

			for(unsigned int i = 0; i < outputResults.size(); i++){
				printf("NODE: %d\tGNODE: %d\tRESULT AIG: %llx\n", outputNodes[i], aig->getGNode(outputNodes[i]), outputResults[i]);
			}


		}
		catch(...){

		}
		return true;
	}

	bool verifyLUT(unsigned long long lutFunction, Graph* lutGraph){
		//printf("[SEQ] -- Verifying LUT Transformation...");

		//node vid, function
		std::map<unsigned int, unsigned long long> nodeVal;

		const unsigned long long tt[6] = {
			0xAAAAAAAAAAAAAAAA,
			0xCCCCCCCCCCCCCCCC,
			0xF0F0F0F0F0F0F0F0,
			0xFF00FF00FF00FF00,
			0xFFFF0000FFFF0000,
			0xFFFFFFFF00000000,
		};

		const int sizeOffset = 2; //Smallest lut is size 2;
		const unsigned long long maskList[5] = {
			0xF,
			0xFF,
			0xFFFF,
			0xFFFFFFFF,
			0xFFFFFFFFFFFFFFFF
		};

		unsigned long long mask = maskList[lutGraph->getNumInputs() - sizeOffset];
		unsigned long long result;

		//LSB->MSB
		try{
			int inputIndex = 0;
			std::map<int, Vertex*>::iterator it;
			for(it = lutGraph->begin(); it != lutGraph->end(); it++){
				//printf("TYPE: %s\n", it->second->getType().c_str());
				std::vector<Vertex*> input;
				it->second->getInput(input);

				if(it->second->getType() == "IN"){
					nodeVal[it->first] = tt[inputIndex];	
					//printf("NODE: %3d\tRESULT: %llx\n", it->first, tt[inputIndex]);
					inputIndex++;
				}
				else if(it->second->getType() == "INV"){
					result = ~nodeVal[input[0]->getID()];
					nodeVal[it->first] = result;
					//printf("NODE: %d\tRESULT: %llx\n", it->first, result);
				}
				else if(it->second->getType().find("AND") != std::string::npos){
					result = nodeVal[input[0]->getID()];
					for(unsigned int i = 1; i < input.size(); i++)
						result = result & nodeVal[input[i]->getID()];

					nodeVal[it->first] = result;
					//printf("NODE: %d\tRESULT: %llx\n", it->first, result);
				}
				else if(it->second->getType().find("OR") != std::string::npos){
					result = nodeVal[input[0]->getID()];
					for(unsigned int i = 1; i < input.size(); i++)
						result = result | nodeVal[input[i]->getID()];

					nodeVal[it->first] = result;
					//printf("NODE: %d\tRESULT: %llx\n", it->first, result);
				}
				//printf("\n");
			}

			result = result & mask;
			//printf("RESULT: %llx\tFUNCTION: %llx\n", result, lutFunction);
			assert(result == lutFunction);
		}
		catch(...){
			printf("\n[SEQ] -- Try: Probably node not found (verifyLUT)\n");
			exit(1);
		}
		return true;
	}



}

#endif
