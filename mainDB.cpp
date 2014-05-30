/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @  MAINDB.cpp
  @  
  @  @AUTHOR:Kevin Zeng
  @  Copyright 2012 – 2013 
  @  Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#ifndef MAINDB_GUARD
#define MAINDB_GUARD
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
#include "cutenumeration.hpp"
#include "cutfunction.hpp"
#include "sequential.hpp"
#include "topoDescriptors.hpp"
#include "aggregation.hpp"

using namespace boost;




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
	printf("\n\n*************************************************************\n");
	printf("*************************************************************\n");
	printf("*\n");
	printf("*   BOOLEAN MATCHING USING CUT ENUMERATION\n");
	printf("*      Database Preprocessing\n");
	printf("* \n");
	printf("*\tVirginia Polytechnic Institute and State University\n");
	printf("*\tAUTHOR:\tKevin Zeng\n");
	printf("*\tCopyright 2012–2014\n");
	printf("* \n");
	printf("*************************************************************\n");
	printf("*************************************************************\n\n");
	if(argc < 4)
	{

		printf("./xfpgeniusDB <primitive file> <database file> <location of prim> <output database>\n\n\n");
		return 0;
	}

	//**************************************************************************
	//*  Declarations
	//**************************************************************************
	std::string primBase = argv[1];          //Location of primitive circuits
	std::string dataBase = argv[2];          //Location of database file
	std::string outDatabase = argv[3];       //Output database file
	std::string option= "";                  //Command line options

	//Time tracking
	timeval aig_b, aig_e;
	timeval ce_b, ce_e;
	timeval lib_b, lib_e;
	timeval func_b, func_e;
	timeval agg_b, agg_e;

	double aigt = 0.0;
	double cet = 0.0;
	double libt = 0.0;
	double funct = 0.0;
	double aggt = 0.0;
	double elapsedTime;
	int k = 3;                              //k-Cut enumeration value 

	std::ofstream outdb;                    //Database output file stream
	std::map<std::string, std::set<unsigned long> > pDatabase;
	std::map<std::string, double> similarity;

	//function, count
	std::map<unsigned long, int>::iterator fcit;

	//Circuit statistic
	std::vector<std::map<std::string, int> > stat_compCount;   //Component, count
	std::vector<unsigned int> stat_wienerIndex;
	std::vector<unsigned int> stat_aigSize;
	std::vector<unsigned int> stat_ffSize;
	std::vector<unsigned int> stat_numInput;
	std::vector<unsigned int> stat_numOutput;





	//**************************************************************************
	//* Get options 
	//**************************************************************************
	if(argc == 5) 
		option = argv[4];

	verbose = false;
	if(option.find("v") != std::string::npos)
		verbose = true;



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
	//* Preprocess the circuits in the database 
	//**************************************************************************
	//Make sure database file is okay
	std::ifstream infile;
	infile.open(dataBase.c_str());
	if (!infile.is_open())	{
		fprintf(stderr, "[ERROR] -- Cannot open the database for import...exiting\n");
		fprintf(stderr, "\n***************************************************\n\n");
		exit(-1);
	}

	std::string aigComponentDefinition;
	getline(infile, aigComponentDefinition);
	AIG::s_SourcePrim = aigComponentDefinition;
	
	outdb.open(outDatabase.c_str());
	outdb<<aigComponentDefinition<<"\n";

	std::string file = "";
	std::vector<int> count; 
	std::vector<std::string> name;
	//Go through each circuit in the database and preprocess data/signature 
	while(getline(infile, file)){
		//Handles empty lines in files
		if(file == "\n")
			continue;

		printStatement("Processing File: " +  file);
		Graph* ckt = new Graph(file);

		//Import circuit and convert to AIG
		gettimeofday(&aig_b, NULL);
		ckt->importGraph(file, 0);

		stat_numInput.push_back(ckt->getNumInputs());
		stat_numOutput.push_back(ckt->getNumOutputs());
		SEQUENTIAL::replaceLUTs(ckt);
		
		//Count the number of specific components
		std::map<int, Vertex<std::string>*>::iterator cktit;
		std::map<std::string, int> compCount;   //Component, count
		unsigned int ffcount = 0; 
		for(cktit = ckt->begin(); cktit != ckt->end(); cktit++){
			if(compCount.find(cktit->second->getType()) != compCount.end())
				compCount[cktit->second->getType()]++;
			else
				compCount[cktit->second->getType()] = 1; 

			if(cktit->second->getType().find("FD") != std::string::npos){
				ffcount++;
			}
		}
		stat_compCount.push_back(compCount);
		stat_ffSize.push_back(ffcount);

		unsigned int wIndex = TOPOLOGY::weinerIndex(ckt);
		stat_wienerIndex.push_back(wIndex);


		AIG* aigraph = new AIG();
		aigraph->convertGraph2AIG(ckt, false);
		stat_aigSize.push_back(aigraph->getSize());
		gettimeofday(&aig_e, NULL);

		//PERFORM K CUT ENUMERATION
		gettimeofday(&ce_b, NULL);
		CutEnumeration* cut = new CutEnumeration (aigraph);
		cut->findKFeasibleCuts(k);
		gettimeofday(&ce_e, NULL);

		//PERFORM BOOLEAN MATCHING
		gettimeofday(&func_b, NULL);
		functionCalc->setParams(cut, aigraph);
		functionCalc->processAIGCuts(true);
		gettimeofday(&func_e, NULL);

		//Aggregation
		/*
		   printStatement("RESULTS");
		   gettimeofday(&agg_b, NULL);
		   functionCalc->printFunctionCount();
		   findHA(functionCalc, aigraph);
		   findDecoder(functionCalc, aigraph);
		   findMux(functionCalc, aigraph);
		   findMux2(functionCalc, aigraph);
		   gettimeofday(&agg_e, NULL);
		 */

		std::map<unsigned long, int> functionCount;
		functionCalc->getFunctionCount(functionCount);
		outdb<< file << "\n";
		outdb<< functionCount.size() << "\n";
		name.push_back(file);
		count.push_back(functionCount.size());
		for(fcit = functionCount.begin(); fcit != functionCount.end(); fcit++){
			outdb<< fcit->first << " " << fcit->second << "\n";
		}


		//Calculate elapsed time
		elapsedTime = (aig_e.tv_sec - aig_b.tv_sec) * 1000.0;   
		elapsedTime += (aig_e.tv_usec - aig_b.tv_usec) / 1000.0;
		aigt+=elapsedTime;

		elapsedTime = (ce_e.tv_sec - ce_b.tv_sec) * 1000.0;     
		elapsedTime += (ce_e.tv_usec - ce_b.tv_usec) / 1000.0;  
		cet+=elapsedTime;

		elapsedTime = (func_e.tv_sec - func_b.tv_sec) * 1000.0;
		elapsedTime += (func_e.tv_usec - func_b.tv_usec) / 1000.0;
		funct+=elapsedTime;

		elapsedTime = (agg_e.tv_sec - agg_b.tv_sec) * 1000.0;
		elapsedTime += (agg_e.tv_usec - agg_b.tv_usec) / 1000.0;
		aggt+=elapsedTime;

		delete aigraph;
		delete cut;
		delete ckt;
		printf("\n");
	}

	outdb << "END\n";
	outdb.close();
	printStatement("Build Database Complete");
	printf("[mainDB] -- Database Output File: %s\n\n", outDatabase.c_str() );

		printf("%-15s", "Circuit");
		printf("%5s", "In");
		printf("%5s", "Out");
		printf("%8s", "Func");
		printf("%8s", "Wiener");
		printf("%8s", "|AIG|");
		printf("%8s", "|FF|");
		printf("\n");
		printf("--------------------------------------------------------------------------------\n");
	for(unsigned int i = 0; i < count.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-15s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
		printf("%5d", stat_numInput[i]);
		printf("%5d", stat_numOutput[i]);
		printf("%8d", count[i]);
		printf("%8d", stat_wienerIndex[i]);
		printf("%8d", stat_aigSize[i]);
		printf("%8d", stat_ffSize[i]);
		printf("\n");
	}
	printf("\n");


	//Calculate Elapsed time
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	printStatement("Execution Timing Statistics");
	printf("LIBRARY PREPROCEES:  %10.4f s\n", libt/1000.0);
	printf("AIG IMPORT:          %10.4f s\n", aigt/1000.0);
	printf("CUT ENUMERATION:     %10.4f s\n", cet/1000.0);
	printf("BOOLEAN MATCHING:    %10.4f s\n", funct/1000.0);
	printf("AGGREGATION:         %10.4f s\n", aggt/1000.0);
	printf("\n\n[ --------***--------      END      --------***-------- ]\n\n");

	return 0;

}







#endif
