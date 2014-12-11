
/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
  @
  @  MAIN.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef MAIN_GUARD
#define MAIN_GUARD
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vf2_sub_graph_iso.hpp>

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "graph_b.hpp"
#include "cutenumeration.hpp"
#include "cutfunction.hpp"
#include "sequential.hpp"
#include <iostream>

void printStatement(std::string statement){
	time_t now; 
	struct tm *current;
	now = time(0);
	current = localtime(&now);
	printf("\n\n=============================================================\n");
	printf("[%02d:%02d:%02d]\t%s\n", current->tm_hour, current->tm_min, current->tm_sec, statement.c_str());
	printf("=============================================================\n");

}

//Unit Test Classes
void testGraphBoost();	
void testGraph();
void testSequential();
void testAIG();

int main(){

	//Test Graph Boost

	testGraph();
	testGraphBoost();
	//testAIG();
	testSequential();


	return 0;	
}


/**************************************************************
 * 
 * Unit Test Function for the namespace SEQUENTIAL
 *
 */
void testAIG(){
	printStatement("Testing Class:\tAIG...");
	Graph* g = new Graph("Graph");

	//TEST HANDLE FF
	g->importGraph("circuits/testbench/up_counter4.g", 0);
	SEQUENTIAL::replaceLUTs(g);
	AIG aig;

	g->print();
	aig.handleFF(1,g);
	aig.handleFF(2,g);
	aig.handleFF(3,g);
	aig.handleFF(4,g);
	g->print();
	
/*	g->importGraph("circuits/testbench/mux2.g", 0);
	g->print();
	SEQUENTIAL::replaceLUTs(g);
	g->print();
	*/


}


/**************************************************************
 * 
 * Unit Test Function for the namespace SEQUENTIAL
 *
 */
void testSequential(){
	printStatement("Testing namespace:\tSEQUENTIAL...");
	Graph* g = new Graph("Graph");
	AIG* aig = new AIG();

	//g->importGraph("circuits/iwls/SEQ_/testmuxrange.g", 0);
	g->importGraph("circuits/iwls/SEQ_/b04.g", 0);
	/*if(aig->handleFF(7, g)) g->removeVertex(7);
	if(aig->handleFF(8, g)) g->removeVertex(8);
	if(aig->handleFF(9, g)) g->removeVertex(9);
	if(aig->handleFF(10, g)) g->removeVertex(10);
	if(aig->handleFF(11, g)) g->removeVertex(11);
	*/
	printf("ORIGINAL GRAPH\n");
	g->print();
	printf("\n\n\n");

	SEQUENTIAL::replaceLUTs(g);
	printf("REPLACED LUTS\n");
	g->setLevels();
	g->print();
	printf("\n\n\n");
	VERIFICATION::verifyGraph(g);


	AIG::s_SourcePrim = "circuits/g/primitive/";

	g->resetLevels();
	aig->convertGraph2AIG(g, false);
	printf("AIG CONVERSION\n");
	aig->print();
	printf("\n\n\n");
	VERIFICATION::verifyGraph(g);
	VERIFICATION::verifyAIG(aig);

	
	CutFunction* functionCalc = new CutFunction();
	functionCalc->preProcessLibrary("database/primitives");

	CutEnumeration* cut2 = new CutEnumeration (aig);
	cut2->findKFeasibleCuts(3);
	//cut2->print();

	functionCalc->setParams(cut2, aig);
	functionCalc->processAIGCuts(false);
	//functionCalc->printLibrary();
	functionCalc->printStat();

}






/**************************************************************
 * 
 * Unit Test Function for the class: Graph
 *
 */
void testGraph(){
	printStatement("Testing Class:\tGraph...");

	printf(" - Constructor:\n");	
	std::cout<<" --- "<<"Creating Graph with 3 Vertices\n";
	Graph* g = new Graph("Graph");
	g->addVertex(11, "AND");	
	g->addVertex(22, "NAND");	
	g->addVertex(33, "NOT");	
	assert(g->getNumVertex() == 3);
	printf("\n -- PASS------------------\n");
	printf("\n\n--------------------------------------------------------------------------------\n");

	std::cout<<" --- "<<"Copy Constructor\n";
	Graph* g2 = new Graph("Graph2");
	*g2 = *g;
	assert(g2->getNumVertex() == g->getNumVertex());
	assert(g2->getVertex(11) != NULL);
	assert(g2->getVertex(22) != NULL);
	assert(g2->getVertex(33) != NULL);
	assert(g2->getVertex(44) == NULL);

	assert(g2->getLast() == g->getLast());

	delete g;
	assert(g2->getVertex(11) != NULL);
	assert(g2->getVertex(22) != NULL);
	assert(g2->getVertex(33) != NULL);
	assert(g2->getVertex(44) == NULL);

	std::vector<Vertex<std::string>*> inout;
	std::vector<Vertex<std::string>*> inout2;
	std::vector<std::string> port;
	std::vector<std::string> port2;




		

	printf("\n -- PASS------------------\n");

}

/**************************************************************
 * 
 * Unit Test Function for the class: GraphBoost
 *
 */
void testGraphBoost(){
	printStatement("Testing Class:\tGraphBoost...");

	printf(" - Constructor:\n");	
	std::cout<<" --- "<<"Creating Graph with 10 Vertices\n";
	GraphBoost* gb = new GraphBoost(10);
	assert(gb->getNumVertices() == 10);
	delete gb;

	std::cout<<" --- "<<"Creating Graph with -1 Vertices\n";
	gb = new GraphBoost(-1);
	assert(gb->getNumVertices() == 0);
	delete gb;
	printf("\n -- PASS------------------\n");

	printf("\n\n----------------------------------------------------------------------\n");
	printf("\n - Function: addVertex:\n");	
	std::cout<<" --- "<<"Creating Graph with 10 Vertices\n";
	gb = new GraphBoost(10);
	assert(gb->getNumVertices() == 10);

	std::cout<<" --- "<<"Adding 1 vertex to the graph\n";
	int v = gb->addVertex();
	assert(gb->getNumVertices() == 11);
	assert(v == 10);

	std::cout<<" --- "<<"Adding 5 vertex to the graph\n";
	v = gb->addVertex();
	assert(v == 11);
	v = gb->addVertex();
	assert(v == 12);
	v = gb->addVertex();
	assert(v == 13);
	v = gb->addVertex();
	assert(v == 14);
	v = gb->addVertex();
	assert(v == 15);
	assert(gb->getNumVertices() == 16);
	printf("\n -- PASS------------------\n");

	/*
	   printf("----------------------------------------------------------------------\n");
	   printf("\n - Function: setVID and getVID:\n");	
	   std::cout<<" --- "<<"Adding 1-6 vertex labels to label squared\n";
	   gb->setVID(1, 1);
	   gb->setVID(2, 4);
	   gb->setVID(3, 9);
	   gb->setVID(4, 16);
	   gb->setVID(5, 25);
	   gb->setVID(6, 36);

	   std::cout<<" --- "<<"Checking labels to see if they are label squared\n";
	   assert(gb->getVID(1) == 1);
	   assert(gb->getVID(2) == 4);
	   assert(gb->getVID(3) == 9);
	   assert(gb->getVID(4) == 16);
	   assert(gb->getVID(5) == 25);
	   assert(gb->getVID(6) == 36);
	   std::cout<<" --- "<<"Checking unset labels\n";
	   assert(gb->getVID(7) == 0);
	   assert(gb->getVID(8) == 0);
	   assert(gb->getVID(9) == 0);
	   std::cout<<" --- "<<"Checking Out of bounds vertex reference\n";
	   assert(gb->getVID(200) == 0);

	   printf("\n -- PASS------------------\n");
	 */


	printf("\n\n----------------------------------------------------------------------\n");
	printf("\n - Function: addVertex:\n");	
	std::cout<<" --- "<<"Removing vertices \n";
	gb->removeVertex(0);
	assert(gb->getNumVertices() == 15);  
	std::cout<<" --- "<<"Removing invalid vertex \n";
	gb->removeVertex(100);
	std::cout<<"v: "<<gb->getNumVertices()<<"\n";
	assert(gb->getNumVertices() == 15);
	std::cout<<" --- "<<"Removing a removed vertex \n";
	gb->removeVertex(0);
	assert(gb->getNumVertices() == 14);
	printf("\n -- PASS------------------\n");



	printf("\n\n----------------------------------------------------------------------\n");
	printf("\n - Function: addEdge, getAdjacentOut:\n");	
	std::cout<<" --- "<<"Adding Edges\n";
	gb->addEdge(0,1);
	gb->addEdge(0,8);
	gb->addEdge(0,6);
	gb->addEdge(1,2);
	gb->addEdge(2,3);
	gb->addEdge(3,4);
	gb->addEdge(4,5);
	gb->addEdge(5,6);
	gb->addEdge(6,7);
	gb->addEdge(6,0);
	gb->addEdge(7,8);

	std::cout<<" --- "<<"Checking Adjacencies\n";
	std::set<int> adj; 
	gb->getAdjacentOut(0, adj);
	assert(adj.size() == 3);
	adj.clear();
	gb->getAdjacentOut(6, adj);
	assert(adj.size() == 2);
	adj.clear();
	gb->getAdjacentOut(2, adj);
	assert(adj.size() == 1);
	adj.clear();
	gb->getAdjacentOut(8, adj);
	assert(adj.size() == 0);
	adj.clear();
	printf("\n -- PASS------------------\n");
}

#endif
