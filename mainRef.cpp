/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @  MAINREF.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef MAIN_GUARD
#define MAIN_GUARD
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vf2_sub_graph_iso.hpp>

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
#include "cutenumeration.hpp"
#include "cutfunction.hpp"
#include "graph_b.hpp"
#include "sequential.hpp"
#include "topoDescriptors.hpp"
#include "aggregation.hpp"

using namespace boost;

//Similarity
double Jaccard(std::vector<unsigned long>& set1, std::vector<unsigned long>& set2, bool overRef);
double Dice(std::vector<unsigned long>& set1, std::vector<unsigned long>& set2);

//Combinational Cloud
void getCutGraph(AIG* aigraph, unsigned out, std::vector<unsigned>& in);


//Flags
bool verbose;

//Special Print Function
void printStatement(std::string statement){
	time_t now; 
	struct tm *current;
	now = time(0);
	current = localtime(&now);
	printf("\n\n================================================================================\n");
	printf("[%02d:%02d:%02d]\t%s\n", current->tm_hour, current->tm_min, current->tm_sec, statement.c_str());
	printf("================================================================================\n");
}




int main( int argc, char *argv[] )
{
	printf("\n\n********************************************************************************\n");
	printf("********************************************************************************\n");
	printf("*\n");
	printf("*	BOOLEAN MATCHING USING CUT ENUMERATION\n");
	printf("* \n");
	printf("*\tVirginia Polytechnic Institute and State University\n");
	printf("*\tAUTHOR:\tKevin Zeng\n");
	printf("*\tCopyright 2012–2014\n");
	printf("* \n");
	printf("********************************************************************************\n");
	printf("********************************************************************************\n\n");

	if(argc < 4){
		printf("./xfpgeniusRef <input G file> <primitive file> <database file>\n\n\n");
		printf("\nOPTIONS:\n");
		return 0;
	}





	//**************************************************************************
	//*  Declarations
	//**************************************************************************
	std::string referenceCircuit = argv[1];    //Name of reference circuit
	std::string primBase = argv[2];            //Location of primitive circuits
	std::string database = argv[3];            //Location of database file
	std::string option= "";                    //Command line option

	//Time tracking
	timeval aig_b, aig_e;
	timeval ce_b, ce_e;
	timeval lib_b, lib_e;
	timeval func_b, func_e;
	timeval agg_b, agg_e;
	timeval cnt_b, cnt_e;
	//timeval cnt_b, cnt_e;
	double libt = 0.0;
	double elapsedTime;

	int k = 3;                              //k-Cut enumeration value 

	std::ifstream indb;                     //Input stream for reading data base file
	std::map<std::string, std::set<unsigned long> > pDatabase;
	std::map<std::string, double> similarity;

	//database file name, function count
	std::map<std::string, std::map<unsigned long, int> >functionCountMap;
	std::map<std::string, std::map<unsigned long, int> >::iterator fcmit;

	std::map<unsigned long, int>::iterator fcit;


	//Option flags
	bool counterFlag = false;
	bool topoFlag = false;





	//**************************************************************************
	//* Get options 
	//**************************************************************************
	if(argc == 5){ 
		option = argv[4];
		printf("OPTIONS: %s\n", argv[4]);
	}

	verbose = false;
	if(option.find("v") != std::string::npos)
		verbose = true;
	if(option.find("c") != std::string::npos)
		counterFlag = true;
	if(option.find("t") != std::string::npos)
		topoFlag = true;





	//**************************************************************************
	//* Import Circuit Function Database 
	//**************************************************************************
	printStatement("Preprocessing circuit database: " + database);
	indb.open(database.c_str());
	std::string circuitName = "";

	std::string aigComponentDefinition;
	indb >> aigComponentDefinition;
	AIG::s_SourcePrim = aigComponentDefinition;

	while(!indb.eof()){
		indb >> circuitName;
		if(circuitName == "END")
			break;

		std::map<unsigned long, int> functionCount;

		unsigned int size;
		indb >> size;

		std::string name = circuitName;
		int lastSlashIndex = name.find_last_of("/") + 1;
		printf("%-15s\t", name.substr(lastSlashIndex, name.length()-lastSlashIndex-2).c_str());
		printf("SIZE: %d\n", size);

		for(unsigned int i = 0; i < size; i++){
			unsigned long function;
			int count;
			indb >>function;
			indb >>count;
			functionCount[function] = count;
		}

		functionCountMap[circuitName] = functionCount;
	}





	//**************************************************************************
	//* Preprocess library for boolean matching 
	//**************************************************************************
	printStatement("Performing Library preprocessing");
	CutFunction* functionCalc = new CutFunction();
	gettimeofday(&lib_b, NULL);
	functionCalc->preProcessLibrary(primBase);
	gettimeofday(&lib_e, NULL);

	elapsedTime = (lib_e.tv_sec - lib_b.tv_sec) * 1000.0;
	elapsedTime += (lib_e.tv_usec - lib_b.tv_usec) / 1000.0;
	libt+=elapsedTime;





	//**************************************************************************
	//* Processing Reference circuit for comparison 
	//**************************************************************************
	//printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	std::map<std::string, std::set<unsigned long> > rDatabase;

	printStatement("Processing Reference Circuit:\t" + referenceCircuit);
	Graph* ckt2 = new Graph(referenceCircuit);
	ckt2->importGraph(referenceCircuit, 0);

	if(counterFlag){
		gettimeofday(&cnt_b, NULL);
		Graph* copy = new Graph("copy");
		*copy = *ckt2;
		SEQUENTIAL::counterIdentification(copy);
		gettimeofday(&cnt_e, NULL);
		delete copy;
	}
	

	SEQUENTIAL::replaceLUTs(ckt2);

	if(topoFlag){
		TOPOLOGY::weinerIndex(ckt2);
		delete ckt2;
		delete functionCalc;
		return 0;
	}
	//ckt2->print();

	//Converting circuit to AIG
	gettimeofday(&aig_b, NULL);
	Graph* originalCkt = new Graph(referenceCircuit);
	*originalCkt = *ckt2;

	AIG* aigraph2 = new AIG();
	aigraph2->convertGraph2AIG(ckt2, false);
	aigraph2->print();
	gettimeofday(&aig_e, NULL);

	//PERFORM K CUT ENUMERATION
	gettimeofday(&ce_b, NULL);
	CutEnumeration* cut2 = new CutEnumeration (aigraph2);
	cut2->findKFeasibleCuts(k);
	gettimeofday(&ce_e, NULL);

	//PERFORM BOOLEAN MATCHING
	gettimeofday(&func_b, NULL);
	functionCalc->setParams(cut2, aigraph2);
	functionCalc->processAIGCuts(false);
	gettimeofday(&func_e, NULL);







	//**************************************************************************
	//* Calculate similarity of circuits based on function type 
	//**************************************************************************
	//TODO compare functions to database
	//Function, number of nodes with that function
	std::map<unsigned long, int> functionCount;          //Counter of specific function
	std::map<unsigned long, int>::iterator fcrefit;

	//database circuit name, sim
	std::map<std::string, double> functionSimilarity;    //Keeps track of how similar two functions are
	functionCalc->getFunctionCount(functionCount);

	printStatement("Comparing Reference circuit to database");
	printf("-- Calculation is performed by comparing counting the number\n");
	printf("-- of specific function a node performs with cut k =  %d\n", k);

	//For each circuit in teh database
	for(fcmit = functionCountMap.begin(); fcmit != functionCountMap.end(); fcmit++){
		std::string name = fcmit->first;
		int lastSlashIndex = name.find_last_of("/") + 1;
		printf("%-15s\t", name.substr(lastSlashIndex, name.length()-lastSlashIndex-2).c_str());

		//printf("Total Match\tDB: %d\tREF:%d\n", (int) fcmit->second.size(), (int)functionCount.size());
		double numFMatch = 0.0;
		double numFMatchCount = 0.0;

		//double refTotal = (double) functionCount.size();
		//double dbTotal = (double) fcmit->second.size();
		double refTotalCount = (double) functionCount.size();
		double refTotal = (double) functionCount.size();

		std::vector<unsigned long> dbvector;
		std::vector<unsigned long> refvector;

		//For each function in the circuit
		for(fcit = fcmit->second.begin(); fcit != fcmit->second.end(); fcit++){

			//For each function in the reference circuit
			if(functionCount.find(fcit->first) != functionCount.end()){
				int count = 0;
				if(fcit->second < functionCount[fcit->first]){
					count = fcit->second;
				}
				else
					count = functionCount[fcit->first];

				numFMatchCount += count;
				refTotalCount += (count-1);
				numFMatch++;
			}
			dbvector.push_back(fcit->first);
		}

		for(fcit = functionCount.begin(); fcit != functionCount.end(); fcit++)
			refvector.push_back(fcit->first);

		//double jaccSimRef = Jaccard(refvector, dbvector, true);
		//double jaccSim = Jaccard(refvector, dbvector, false);
		//double diceSim = Dice(refvector, dbvector);

		//printf("Number of function matches: %f\n", numFMatch);
		//printf("Number of function counts : %f / %f \n", numFMatchCount, refTotalCount );
		//printf("SIM over db : %f\n", numFMatch/dbTotal); 
		printf("%-5f\t", numFMatch/refTotal); 
		printf("%-5f\n", numFMatchCount/refTotalCount); 
		//printf("SIM over     JACCARD : %f\n", jaccSimRef); 
		//printf("SIM over ref JACCARD : %f\n", jaccSim); 
		//printf("SIM over     DICE    : %f\n", diceSim);
		//printf("\n\n");

	}	

aigraph2->print();
//aigraph2->printMap();
//printf("\n\n");
//originalCkt->print();
functionCalc->printStat();
functionCalc->printLibrary();
	AGGREGATION::findMux(functionCalc, aigraph2);

	/*
	//Aggregating reference circuit data
	findHA(functionCalc, aigraph2);
	findDecoder(functionCalc, aigraph2);
	printf("\n\n\n");
	printStatement("RESULTS");
	gettimeofday(&agg_b, NULL);
	findHA(functionCalc, aigraph2);
	findMux(functionCalc, aigraph2);
	findDecoder(functionCalc, aigraph2);
	gettimeofday(&agg_e, NULL);
	 */
	//	printf("\nNUMBER OF POSSIBLE COMBINATIONAL CLOUDS:\t%d\n", nCloud);



	//Calculate Elapsed time
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	printStatement("Execution Timing Statistics");
	printf("LIBRARY PREPROCEES:  %10.4f s\n", libt/1000.0);

	elapsedTime = (aig_e.tv_sec - aig_b.tv_sec) * 1000.0;   
	elapsedTime += (aig_e.tv_usec - aig_b.tv_usec) / 1000.0;
	printf("AIG IMPORT:          %10.4f s\n", elapsedTime/1000.0);
	
	elapsedTime = (ce_e.tv_sec - ce_b.tv_sec) * 1000.0;     
	elapsedTime += (ce_e.tv_usec - ce_b.tv_usec) / 1000.0;  
	printf("CUT ENUMERATION:     %10.4f s\n", elapsedTime/1000.0);
	
	elapsedTime = (func_e.tv_sec - func_b.tv_sec) * 1000.0;
	elapsedTime += (func_e.tv_usec - func_b.tv_usec) / 1000.0;
	printf("BOOLEAN MATCHING:    %10.4f s\n", elapsedTime/1000.0);
	
	elapsedTime = (agg_e.tv_sec - agg_b.tv_sec) * 1000.0;   
	elapsedTime += (agg_e.tv_usec - agg_b.tv_usec) / 1000.0;
	printf("AGGREGATION:         %10.4f s\n", elapsedTime/1000.0);


	if(counterFlag)	{
		elapsedTime = (cnt_e.tv_sec - cnt_b.tv_sec) * 1000.0;   
		elapsedTime += (cnt_e.tv_usec - cnt_b.tv_usec) / 1000.0;
		printf("COUNTER ENUMERATION: %10.4f s\n", elapsedTime/1000.0);

	}

	printf("\n\n[ --------***--------      END      --------***-------- ]\n\n");

	delete aigraph2;
	delete cut2;
	delete functionCalc;
	delete ckt2;

	if(option != "")
		return 0;

	//Similarity Calculation	
	/*
	   std::map<std::string, std::vector<unsigned int> >::iterator nit;
	   for(nit = nbitMap.begin(); nit != nbitMap.end(); nit++){
	   printf("File: %s, Size: %d\n", nit->first.c_str(), (int) nit->second.size());
	   double sim = Jaccard(nit->second, nbit14);
	   similarity[nit->first] = sim;
	   }

	   std::map<std::string, double>::iterator simit;
	   for(simit = similarity.begin(); simit != similarity.end(); simit++){
	   printf("REF CIRCUIT: %s  == CIRCUIT %s\tSIM: %f\n", referenceCircuit.c_str(), simit->first.c_str(), simit->second);
	   }
	 */

	return 0;

}





/*********************************************************************************
 *
 *
 *
 *
 *                      Similarity
 *
 *
 *
 *********************************************************************************/
double Jaccard(std::vector<unsigned long>& set1, std::vector<unsigned long>& set2, bool overRef){
	//printStatement("CALCULATING JACCARD");
	double unionVal = 0.0;
	double intersectVal = 0.0;

	std::set<unsigned long> s1, s2, tmp; 
	std::set<unsigned long>::iterator si1, si2; 

	//printf("Size s1: %d\n", set2.size());
	//printf("Size s2: %d\n", set1.size());

	for(unsigned int i = 0; i < set1.size(); i++){
		s1.insert(set1[i]);
	}

	for(unsigned int j = 0; j < set2.size(); j++){
		s2.insert(set2[j]);
	}

	for(si1 = s1.begin(); si1 != s1.end(); si1++){
		for(si2 = s2.begin(); si2 != s2.end(); si2++){
			if(*si1 == *si2){
				intersectVal ++;
				break;
			}
		}
	}


	if(overRef)
		unionVal = set1.size();
	else{
		for(si1 = s1.begin(); si1 != s1.end(); si1++)
			tmp.insert(*si1);
		for(si2 = s2.begin(); si2 != s2.end(); si2++)
			tmp.insert(*si2);
		unionVal = tmp.size();
	}
	//printf("INTERSECT: %f\n", intersectVal);
	//printf("UNION: %f\n", unionVal);
	//printf("JACCARD: %f\n", intersectVal/unionVal);
	//printf("\n\n");

	return intersectVal / unionVal;
};



double Dice(std::vector<unsigned long>& set1, std::vector<unsigned long>& set2){
	//printStatement("CALCULATING JACCARD");
	//double unionVal = 0.0;
	double intersectVal = 0.0;

	std::set<unsigned long> s1, s2, tmp; 
	std::set<unsigned long>::iterator si1, si2; 

	//printf("Size s1: %d\n", set2.size());
	//printf("Size s2: %d\n", set1.size());

	for(unsigned int i = 0; i < set1.size(); i++){
		s1.insert(set1[i]);
	}

	for(unsigned int j = 0; j < set2.size(); j++){
		s2.insert(set2[j]);
	}

	for(si1 = s1.begin(); si1 != s1.end(); si1++){
		for(si2 = s2.begin(); si2 != s2.end(); si2++){
			if(*si1 == *si2){
				intersectVal ++;
				break;
			}
		}
	}


	return ( 2*intersectVal) / (set1.size() + set2.size());
};









/*

void getCutGraph(AIG* aigraph, unsigned out, std::vector<unsigned> in){
	std::list<unsigned> queue;
	std::set<unsigned> mark;
	std::vector<unsigned> nodes;
	std::vector<std::vector<unsigned> > nChilds;


	//Enqueue
	queue.push_back(out);
	mark.insert(out);

	while(queue.size() != 0){
		unsigned qitem = queue.front();
		queue.pop_front();


		//If reached an input, return
		bool foundInput = false;
		for(unsigned int i = 0; i < in.size(); i++){
			qitem = qitem & 0xFFFFFFFE;
			unsigned input = in[i];
			input = input & 0xFFFFFFFE;

			if(qitem == in[i]){
				foundInput = true;
				break;
			}
		}

		if(foundInput) continue;

		nodes.push_back(qitem);


		//Enqueue childs
		unsigned c1node = aigraph->getChild1(qitem);
		unsigned c2node = aigraph->getChild2(qitem);
		unsigned c1 = c1node & 0xFFFFFFFE;
		unsigned c2 = c2node & 0xFFFFFFFE;

		std::vector<unsigned> nChildsIndices;
		nChildsIndices.push_back(c1);
		nChildsIndices.push_back(c2);
		nChilds.push_back(nChildsIndices);

		if(mark.find(c1) == mark.end()){
			queue.push_back(c1node);
			mark.insert(c1);
		}
		if(mark.find(c2) == mark.end()){
			queue.push_back(c2node);
			mark.insert(c2);
		}
	}


	printf("\n\nSUBGRAPH NODES\n");
	printf("INPUTS: ");
	for(unsigned int i = 0; i < in.size(); i++)
		printf("%d ", in[i]);
	printf("\nOUTPUT: %d\n\n", out);

	for(unsigned int i = 0; i < nodes.size(); i++){
		printf("%d: %d %d\n", nodes[i], nChilds[i][0], nChilds[i][1]); 
	}



}
















   void findFA(CutFunction* cf, AIG* aigraph){
   printf("\n\nSEARCHING FULL ADDERS\n");

   std::map<unsigned long, std::string>::iterator it;
   std::map<unsigned long, std::string> hmap;
   cf->getHashMap(hmap);

//Function, Vector of every set of inputs with that function. Last item is the output node
std::map<unsigned long, std::vector<std::vector<int>*> > pmap;
cf->getPortMap(pmap);

//Functions that are carry and sum operations
std::vector<unsigned long> carryNodes;
std::vector<unsigned long> sumNodes;

for(it = hmap.begin(); it!=hmap.end(); it++){
if(it->second.find("fulladder_carry") != std::string::npos)
carryNodes.push_back(it->first);
if(it->second.find("fulladder_sum") != std::string::npos)
sumNodes.push_back(it->first);
}


//List of inputs of sum components. Last index is the output
std::list<std::vector<int> > sumPorts;
std::list<std::vector<int> > carryPorts;


//Get the Inputs and outputs of all the sum and carry componenets
printf("Sum\n");
for(unsigned int i = 0; i < sumNodes.size(); i++){
if(pmap.find(sumNodes[i]) != pmap.end()){
for(unsigned int j = 0; j < pmap[sumNodes[i]].size(); j++){
unsigned int k = 0;
printf("INPUT: ");
std::vector<int> ports;
for(k = 0; k < pmap[sumNodes[i]][j]->size()-1; k++){
printf("%d ", pmap[sumNodes[i]][j]->at(k));
ports.push_back(pmap[sumNodes[i]][j]->at(k));
}
printf("\nOUTPUT:\t%d\n", pmap[sumNodes[i]][j]->at(k));
int outnode = pmap[sumNodes[i]][j]->at(k);
ports.push_back(outnode);
sumPorts.push_back(ports);
}
}
}

printf("Carry\n");
for(unsigned int i = 0; i < carryNodes.size(); i++){
if(pmap.find(carryNodes[i]) != pmap.end()){
for(unsigned int j = 0; j < pmap[carryNodes[i]].size(); j++){
unsigned int k = 0;
printf("INPUT: ");
std::vector<int> ports;
for(k = 0; k < pmap[carryNodes[i]][j]->size()-1; k++){
printf("%d ", pmap[carryNodes[i]][j]->at(k));
ports.push_back(pmap[carryNodes[i]][j]->at(k));
}
printf("\nOUTPUT:\t%d\n", pmap[carryNodes[i]][j]->at(k));
int outnode = pmap[carryNodes[i]][j]->at(k);
ports.push_back(outnode);
carryPorts.push_back(ports);
}
}
}

//Inputs, sumco
std::map<std::vector<int>, std::vector<int> > faOutput;
std::list<std::vector<int> >::iterator itsum;
std::list<std::vector<int> >::iterator itcarry;

//See if the sum and carry components match (INPUT MATCH)
for(itsum = sumPorts.begin(); itsum != sumPorts.end(); itsum++){
	for(itcarry = carryPorts.begin(); itcarry != carryPorts.end(); ){
		int match = 0; 

		for(unsigned int j = 0; j < itsum->size()-1; j++){
			for(unsigned int l = 0; l < itcarry->size()-1; l++){
				if(itcarry->at(l) == itsum->at(j)){
					match++;
					break;
				}
			}
		}

		if(match == 3){
			//0- Sum, 1- CO
			std::vector<int> sumCO;
			sumCO.push_back(itsum->at(itsum->size()-1));
			sumCO.push_back(itcarry->at(itcarry->size()-1));

			//Inputs
			std::vector<int> inputs;
			for(unsigned int j = 0; j < itsum->size()-1; j++)
				inputs.push_back(itsum->at(j));

			faOutput[inputs] = sumCO;
			carryPorts.erase(itcarry);	
			itcarry = carryPorts.begin();
		}
		else{
			itcarry++;
		}
	}
}

std::map<std::vector<int>, std::vector<int> >::iterator outit;
printf("\n\nFULL ADDERS\n");
for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
	printf("INPUTS: ");
	for(unsigned int i = 0; i < outit->first.size(); i++)
		printf("%d ", outit->first[i]);
	printf("\nSUM:%d\tCO:%d\n", outit->second[0], outit->second[1]);
	//getCutGraph(aigraph, outit->second[0], outit->first);
	//getCutGraph(aigraph, outit->second[1], outit->first);
}


//CHeck for carry ripple
printf("\nAGGREGATION\n");
std::map<std::vector<int>, std::vector<int> >::iterator outit2;

//Vector of vector of iterators. (Aggregated adders)
std::vector<std::vector<std::map<std::vector<int>, std::vector<int> >::iterator> > aggAdder;

for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
	for(outit2 = faOutput.begin(); outit2 != faOutput.end(); outit2++){
		for(unsigned int i = 0; i < outit2->first.size(); i++){

			//IF CO is found as input to FA
			if(outit->second[1] == outit2->first[i]){
				printf("CO:%d GOES INTO INPUT\n", outit->second[1]);

				//Check to see if adders have been aggregated before
				bool found = false;
				for(unsigned int j = 0; j < aggAdder.size(); j++){
					for(unsigned int k = 0; k < aggAdder[j].size(); k++){

						if(aggAdder[j][k]->second[1] == outit->second[1]){
							aggAdder[j].push_back(outit2);
							found = true;
							break;
						}
						else if(aggAdder[j][k]->second[1] == outit2->second[1]){
							aggAdder[j].push_back(outit);
							found = true;
							break;
						}
					}
					if(found) break;
				}

				if(!found){
					std::vector<std::map<std::vector<int>, std::vector<int> >::iterator> aggregation;
					aggregation.push_back(outit);
					aggregation.push_back(outit2);
					aggAdder.push_back(aggregation);
				}
			}
		}
	}
}


//Print final results
printf("\n\nResults\n");
std::vector<int> adders;
for(unsigned int j = 0; j < aggAdder.size(); j++){
	adders.push_back(aggAdder[j].size());
	for(unsigned int k = 0; k < aggAdder[j].size(); k++){
		printf("INPUTS: ");
		for(unsigned int i = 0; i < aggAdder[j][k]->first.size(); i++)
			printf("%d ", aggAdder[j][k]->first[i]);
		printf("\nSUM:%d\tCO:%d\n", aggAdder[j][k]->second[0], aggAdder[j][k]->second[1]);


	}
	printf("\n");
}

printf("\nAdders\n");
for(unsigned int i = 0; i < adders.size(); i++)
printf("%d-Bit Adder\n", adders[i]);



}
*/

#endif
