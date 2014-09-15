/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  testckt.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef testckt_GUARD
#define testckt_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "cutenumeration.hpp"
#include "cutfunction.hpp"
#include "sequential.hpp"

void printStatement(std::string statement){
	time_t now; 
	struct tm *current;
	now = time(0);
	current = localtime(&now);
	printf("\n\n=============================================================\n");
	printf("[%02d:%02d:%02d]\t%s\n", current->tm_hour, current->tm_min, current->tm_sec, statement.c_str());
	printf("=============================================================\n");

}

	
unsigned long long calculate(unsigned node, std::map<unsigned int, unsigned long long>& m_NodeValue1, AIG* aig);

int main( int argc, char *argv[] )
{

	if(argc != 4){
		printf("./testckt <cnl file 1> <cnl file 2> <database file>\n\n\n");
		return 0;
	}
	
	std::string dataBase = argv[3];          //Location of database file

	std::ifstream infile;
	infile.open(dataBase.c_str());
	if (!infile.is_open())	{
		fprintf(stderr, "[ERROR] -- Cannot open the database for import...exiting\n");
		exit(-1);
	}

	std::string aigComponentDefinition;
	getline(infile, aigComponentDefinition);
	AIG::s_SourcePrim = aigComponentDefinition;

	std::string ckname1, ckname2;
	ckname1 = argv[1];         
	ckname2 = argv[2];        

	//Import Graph
	Graph* ck1 = new Graph(ckname1);
	Graph* ck2 = new Graph(ckname2);
	ck1->importGraph(ckname1, 0);
	ck2->importGraph(ckname2, 0);
	SEQUENTIAL::replaceLUTs(ck1);
	SEQUENTIAL::replaceLUTs(ck2);

	//Convert to AIG
	AIG* aig1 = new AIG();
	AIG* aig2 = new AIG();
	aig1->convertGraph2AIG(ck1, false);
	aig2->convertGraph2AIG(ck2, false);
	
		//Make sure the inout sizes are the same for both circuit
		printf("IN1: %d IN2: %d\n", aig1->getInputSize(), aig2->getInputSize());
		assert(aig1->getInputSize() == aig2->getInputSize());

		printf("OUT1: %d OUT2: %d\n", aig1->getOutputSize(), aig2->getOutputSize());
		assert(aig1->getOutputSize() == aig2->getOutputSize());
		
		unsigned long long* m_Xval = new unsigned long long[6];
		m_Xval[0] = 0x5555555555555555;
		m_Xval[1] = 0x3333333333333333;
		m_Xval[2] = 0x0F0F0F0F0F0F0F0F;
		m_Xval[3] = 0x00FF00FF00FF00FF;
		m_Xval[4] = 0x0000FFFF0000FFFF;
		m_Xval[5] = 0x00000000FFFFFFFF;

	unsigned inputSize = aig1->getInputSize();
	
	//Name of input, index of the input truth table
	std::map<std::string, int> nameInputMap;

	//set initial input values
	std::map<unsigned int, unsigned long long> m_NodeValue1;
	for(unsigned int i = 1; i < inputSize+1; i++){
		int index = (i-1)%6;
		unsigned long long input = m_Xval[index];

		m_NodeValue1[i*2] = input;
		std::string vname = ck1->getVertex(aig1->getGNode(i*2))->getName();
		nameInputMap[vname] = index;
	}	

	std::vector<unsigned> out1;
	std::vector<unsigned> out2;
	aig1->getOutputs(out1);
	aig2->getOutputs(out2);

	printf("OUTPUT RESULTS FOR CKT1: %s\n", ckname1.c_str());
	for(unsigned int i = 0; i < out1.size(); i++){
		for(unsigned int k = 1; k < inputSize+1; k++){
			int index = (k-1)%6;
			unsigned long long input = m_Xval[index];

			m_NodeValue1[k*2] = input;
		}	

		unsigned long long result = calculate(out1[i], m_NodeValue1, aig1);
		printf(" * RESULT: %llx\n", result);
		
	}
	
	printf("OUTPUT RESULTS FOR CKT2: %s\n", ckname1.c_str());
	for(unsigned int i = 0; i < out2.size(); i++){
		for(unsigned int k = 1; k < inputSize+1; k++){
			std::string vname = ck1->getVertex(aig1->getGNode(k*2))->getName();
			unsigned long long input = m_Xval[nameInputMap[vname]];

			m_NodeValue1[k*2] = input;
		}	

		unsigned long long result = calculate(out2[i], m_NodeValue1, aig2);
		printf(" * RESULT: %llx\n", result);
		
	}

	return 0;
}


unsigned long long calculate(unsigned node, std::map<unsigned int, unsigned long long>& m_NodeValue, AIG* aig){
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
	child1 = aig->getChild1(node);
	child2 = aig->getChild2(node);

	node1 = child1 & 0xFFFFFFFE;
	node2 = child2 & 0xFFFFFFFE;

	//printf("CHILDREG %u %u\n", child1, child2);
	//printf("CHILDPOS %u %u\n", node1, node2);

	//Recurse until primary input;
	unsigned long long result1 = calculate(node1, m_NodeValue, aig);
	unsigned long long result2 = calculate(node2, m_NodeValue, aig);

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

#endif
