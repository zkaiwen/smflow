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


//System Includes
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <fstream>

//smflow Includes
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "cutenumeration.hpp"
#include "cutfunction.hpp"
#include "sequential.hpp"
#include "aggregation.hpp"
#include "similarity.hpp"
#include "server.hpp"

//XML Includes
#include "rapidxml/rapidxml.hpp"



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

void calculateSimilarity(std::vector<std::string>& name, 
		std::vector<std::map<unsigned, unsigned> >& fingerprint,
		std::vector<std::vector< std::vector<double> > >& simTable);

void calculateSimilarity_size(std::vector<std::string>& name, 
		std::vector<std::map<unsigned, unsigned> >& fingerprint,
		std::vector<std::vector< std::vector<double> > >& simTable);

std::string receiveAllData(int newsockfd, int bufferLength);

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
	if(argc != 4){
		printf("./xfpgeniusDB <primitive file> <database file> <xml database file>\n\n\n");
		return 0;
	}

	//**************************************************************************
	//* MKR-   Declarations
	//**************************************************************************
	std::string primBase = argv[1];          //Location of primitive circuits
	std::string dataBase = argv[2];          //Location of database file
	std::string xmlFileName= argv[3];       //Output database file
	std::string option= "";                  //Command line options

	//Time tracking
	timeval iaig_b, iaig_e;
	timeval ce_b, ce_e;
	timeval func_b, func_e;
	timeval rlut_b, rlut_e ;
	timeval igraph_b, igraph_e;
	timeval fb_b, fb_e;
	timeval cas_b, cas_e;
	timeval mux_b, mux_e;
	timeval blk_b, blk_e;
	timeval outc_b, outc_e;
	timeval ffc_b, ffc_e;
	timeval dec_b, dec_e;
	timeval lib_b, lib_e;
	timeval cnt_b, cnt_e;
	timeval eq_b, eq_e;
	timeval gate_b, gate_e;
	timeval par_b, par_e;
	timeval add_b, add_e;

	float elapsedTime;
	int k = 6;                              //k-Cut enumeration value 

	//std::ofstream outdb;                    //Database output file stream
	std::map<std::string, std::set<unsigned long long> > pDatabase;
	std::map<std::string, double> similarity;

	//function, count
	std::map<unsigned long long, int>::iterator fcit;

	//Circuit statistic
	std::vector<std::map<std::string, int> > stat_compCount;  //Component, count
	std::vector<unsigned int> stat_wienerIndex;
	std::vector<unsigned int> stat_aigSize;
	std::vector<unsigned int> stat_ffSize;
	std::vector<unsigned int> stat_numInput;
	std::vector<unsigned int> stat_numCktInput;
	std::vector<unsigned int> stat_numCktOutput;
	std::vector<unsigned int> stat_numOutput;
	std::vector<unsigned int> stat_numMux;
	std::vector<unsigned int> stat_numLUTs;
	std::vector<unsigned int> stat_numFFFeedback;

	//Mux Type (2-1, 4-1), mux size (2bit, 4bit), count
	std::vector<std::map<unsigned, unsigned> >  stat_decAgg;
	std::vector<std::vector<unsigned> >  stat_addAgg;
	std::vector< std::vector<float> > stat_time;

	//size/count
	std::vector<std::map<unsigned, unsigned> > stat_mux2;
	std::vector<std::map<unsigned, unsigned> > stat_mux3;
	std::vector<std::map<unsigned, unsigned> > stat_mux4;

	std::vector<std::map<unsigned, unsigned> > stat_reg;
	std::vector<std::map<unsigned, unsigned> > stat_counter;
	std::vector<std::map<unsigned, unsigned> > stat_spCutCountFF; 
	std::vector<std::map<unsigned, unsigned> > stat_spCutCountOut;
	std::vector<std::map<unsigned, unsigned> > stat_adder;
	std::vector<std::map<unsigned, unsigned> > stat_adderAgg;
	std::vector<std::map<unsigned, unsigned> > stat_carry;
	std::vector<std::map<unsigned, unsigned> > stat_carryAgg;

	std::vector<std::map<unsigned, unsigned> > stat_cascadeFFM1;
	std::vector<std::map<unsigned, unsigned> > stat_cascadeFFM2;
	std::vector<std::map<unsigned, unsigned> > stat_cascadeFFM3;
	std::vector<std::map<unsigned, unsigned> > stat_blockFFM0;
	std::vector<std::map<unsigned, unsigned> > stat_blockFFM1;
	std::vector<std::map<unsigned, unsigned> > stat_blockFFM2;

	std::vector<std::map<unsigned, unsigned> > stat_parity;
	std::vector<std::map<unsigned, unsigned> > stat_gate;
	std::vector<std::map<unsigned, unsigned> > stat_equal;

	/*
	Server* server = new Server(9000);

	if(! server->waitForClient())
		return 0;

	std::string data = server->receiveAllData();
	printf("DATA RECEIVED FROM CLIENT: %s\n", data.c_str());

	data = server->receiveAllData();
	printf("DATA RECEIVED FROM CLIENT: %s\n", data.c_str());

	if(!server->sendData("YEAH! WE GOT IT!"))
		return 0;

	data = server->receiveAllData();
	printf("DATA RECEIVED FROM CLIENT: %s\n", data.c_str());

	server->closeSocket();
	delete server;
	return 0;


		 printf("READING IN XML FILE\n");
		 std::ifstream xmlfile;
		 xmlfile.open(xmlFileName.c_str());
		 if (!infile.is_open())	{
		 fprintf(stderr, "[ERROR] -- Cannot open the xml file for import...exiting\n");
		 fprintf(stderr, "\n***************************************************\n\n");
		 exit(-1);
		 }

		 std::string xmlstring = "";
		 std::string xmlline;
		 while(getline(xmlfile, xmlline))
		 xmlstring += xmlline + "\n";

		 rapidxml::xml_document<> xmldoc;
		 char* cstr = new char[xmlstring.size() + 1];
		 strcpy(cstr, xmlstring.c_str());
		 xmldoc.parse<0>(cstr);

		 rapidxml::xml_node<> *xmlnode = xmldoc.first_node();
		 rapidxml::xml_attribute<> *xmlattr = xmlnode->first_attribute();
		 if(xmlattr != NULL){
		 std::cout<<"ATTR: "<<xmlattr->name()<<" VAL: "<<xmlattr->value()<<"\n";
		 }

		 while(xmlnode != 0) {
		 std::cout << xmlnode->name() << std::endl;
		 rapidxml::xml_node<> *child = xmlnode->first_node();
		 while (child != 0)
		 {
		 std::cout << child->name() << " " << child->value() << std::endl;
		 rapidxml::xml_node<> *child2 = child->first_node();
		 while (child2 != 0)
		 {
		 std::cout << child2->name() << " " << child2->value() << std::endl;
		 child2 = child2->next_sibling(); 
		 }

		 child = child->next_sibling(); 
		 }
		 xmlnode = xmlnode->next_sibling();
		 }
		 delete [] cstr;
	 */


	//**************************************************************************
	//* MKR-  Get options 
	//**************************************************************************
	if(argc == 5) 
		option = argv[4];



	//**************************************************************************
	//* MKR-  Preprocess the circuits in the database 
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

	//*************************************************************************
	//*  Preprocess library for boolean matching 
	//**************************************************************************
	gettimeofday(&lib_b, NULL); //--------------------------------------------
	printStatement("Performing Library preprocessing");
	CutFunction* functionCalc = new CutFunction();
	functionCalc->preProcessLibrary(primBase);
	gettimeofday(&lib_e, NULL); //--------------------------------------------
	//functionCalc->printLibrary();



	//outdb.open(outDatabase.c_str());
	//outdb<<aigComponentDefinition<<"\n";

	std::string file = "";
	std::vector<int> count; 
	std::vector<std::string> name;
	//Go through each circuit in the database and preprocess data/signature 
	//int sdfid = 100183;




	//*************************************************************************
	//*  Process Data base file and extract features from circuits
	//**************************************************************************
	while(getline(infile, file)){
		//Handles empty lines in files
		if(file == "\n")
			continue;


		printStatement("[MAIN] -- Processing File: " +  file);
		name.push_back(file);

		//*************************************************************************
		//* Import circuits and convert to AIG 
		//**************************************************************************
		gettimeofday(&igraph_b, NULL); //--------------------------------------------
		Graph* ckt = new Graph(file);
		ckt->importGraph(file, 0);
		stat_numCktInput.push_back(ckt->getNumInputs());
		stat_numCktOutput.push_back(ckt->getNumOutputs());
		gettimeofday(&igraph_e, NULL); //--------------------------------------------




		//*************************************************************************
		//* Replace LUT with combinational logic 
		//**************************************************************************
		gettimeofday(&rlut_b, NULL); //--------------------------------------------
		unsigned int numLUTs = SEQUENTIAL::replaceLUTs(ckt);
		stat_numLUTs.push_back(numLUTs);
		printf(" * Replaced %d LUTs\n", numLUTs);
		gettimeofday(&rlut_e, NULL); //--------------------------------------------



		//*************************************************************************
		//*  Searching for sequential layout/patterns
		//**************************************************************************
		//Count the number of enable based registers
		//std::map<unsigned, unsigned> rgcount;
		//SEQUENTIAL::findRegisterGroup(ffList, rgcount);
		//stat_reg.push_back(rgcount);

		//Count number of FF
		std::list<Vertex*> ffList;
		SEQUENTIAL::getFFList(ckt, ffList);
		stat_ffSize.push_back(ffList.size());

		//Count the number of feed back flip flops	
		gettimeofday(&fb_b, NULL); //--------------------------------------------
		std::list<InOut*> ffFeedbackList;
		SEQUENTIAL::getFFFeedbackList(ckt, ffFeedbackList);
		stat_numFFFeedback.push_back(ffFeedbackList.size());
		gettimeofday(&fb_e, NULL); //--------------------------------------------

		//Look for cascading FF input structure
		gettimeofday(&cas_b, NULL); //--------------------------------------------
		std::map<unsigned,unsigned> casFFM1; 
		std::map<unsigned,unsigned> casFFM2;
		std::map<unsigned,unsigned> casFFM3;
		SEQUENTIAL::cascadingFF(ckt, ffFeedbackList, 1, casFFM1);
		SEQUENTIAL::cascadingFF(ckt, ffFeedbackList,  2, casFFM2);
		SEQUENTIAL::cascadingFF(ckt, ffFeedbackList, 3, casFFM3);
		stat_cascadeFFM1.push_back(casFFM1);
		stat_cascadeFFM2.push_back(casFFM2);
		stat_cascadeFFM3.push_back(casFFM3);
		gettimeofday(&cas_e, NULL); //--------------------------------------------

		//Look for Block FF input structure
		gettimeofday(&blk_b, NULL); //--------------------------------------------
		std::map<unsigned,unsigned> blockFFM0;
		std::map<unsigned,unsigned> blockFFM1;
		std::map<unsigned,unsigned> blockFFM2;
		SEQUENTIAL::blockFF(ckt, ffFeedbackList, 0, blockFFM0);
		SEQUENTIAL::blockFF(ckt, ffFeedbackList, 1, blockFFM1);
		SEQUENTIAL::blockFF(ckt, ffFeedbackList, 2, blockFFM2);
		stat_blockFFM0.push_back(blockFFM0);
		stat_blockFFM1.push_back(blockFFM1);
		stat_blockFFM2.push_back(blockFFM2);
		gettimeofday(&blk_e, NULL); //--------------------------------------------

		//Look for possible counters based on strcutural features
		gettimeofday(&cnt_b, NULL); //--------------------------------------------
		std::map<unsigned,unsigned> counters;
		SEQUENTIAL::counterIdentification(ckt, ffFeedbackList, counters);
		stat_counter.push_back(counters);
		gettimeofday(&cnt_e, NULL); //--------------------------------------------

		SEQUENTIAL::deleteFFFeedbackList(ffFeedbackList);





		/*aigraph->convertGraph2AIG(ckt, true);
		//USING OPEN BABEL TO CHECK SIMILARITY
		int lastSlashIndex = file.find_last_of("/") + 1;
		std::string cname = file.substr(lastSlashIndex, file.length()-lastSlashIndex-4);
		ckt->exportGraphSDFV3000(cname, sdfid);
		sdfid++;
		 */

		//*************************************************************************
		//* AIG Translation 
		//**************************************************************************
		gettimeofday(&iaig_b, NULL); //--------------------------------------------
		AIG* aigraph = new AIG();
		aigraph->convertGraph2AIG(ckt, false);
		stat_aigSize.push_back(aigraph->getSize());
		stat_numInput.push_back(ckt->getNumInputs());
		stat_numOutput.push_back(ckt->getNumOutputs());
		//aigraph->printMap();
		//aigraph->print();
		gettimeofday(&iaig_e, NULL);//--------------------------------------------






		//*************************************************************************
		//* K Cut Enumeration 
		//**************************************************************************
		gettimeofday(&ce_b, NULL); //------------------------------------------------
		CutEnumeration* cut = new CutEnumeration (aigraph);
		cut->findKFeasibleCuts(k);
		gettimeofday(&ce_e, NULL); //------------------------------------------------





		//*************************************************************************
		//* Input correspondence 
		//**************************************************************************
		//Find input cut for FF nodes----------------------------
		gettimeofday(&outc_b, NULL); //------------------------------------------------
		std::vector<unsigned> nodes;
		aigraph->getFFInput(nodes);
		cut->findInputCut(nodes);  	//This stores the result in Cut2

		//Node, Set of inputs to the node
		std::map<unsigned, std::set<unsigned> > cutIn;
		cut->getCut2(cutIn);

		//Cut input size, count
		std::map<unsigned, unsigned> cutCountFF;

		std::map<unsigned, std::set<unsigned> >::iterator iCut;
		for(iCut = cutIn.begin(); iCut != cutIn.end(); iCut++){
			if(cutCountFF.find(iCut->second.size()) == cutCountFF.end())
				cutCountFF[iCut->second.size()] = 1;
			else
				cutCountFF[iCut->second.size()]++; 
		}
		stat_spCutCountFF.push_back(cutCountFF);
		gettimeofday(&outc_e, NULL); //------------------------------------------------


		//Find Input cut for output nodes
		gettimeofday(&ffc_b, NULL); //------------------------------------------------
		nodes.clear();
		cutIn.clear();
		aigraph->getOutInput(nodes);  
		cut->findInputCut(nodes);	//Sets the result in cut2

		//Cut input size, count
		std::map<unsigned, unsigned> cutCountOut;
		cut->getCut2(cutIn);
		for(iCut = cutIn.begin(); iCut != cutIn.end(); iCut++){
			if(cutCountOut.find(iCut->second.size()) == cutCountOut.end())
				cutCountOut[iCut->second.size()] = 1;
			else
				cutCountOut[iCut->second.size()]++; 
		}

		stat_spCutCountOut.push_back(cutCountOut);
		gettimeofday(&ffc_e, NULL); //------------------------------------------------





		//*************************************************************************
		//* Calculate Function based on K-Cuts 
		//**************************************************************************
		gettimeofday(&func_b, NULL);//-----------------------------------------------
		functionCalc->setParams(cut, aigraph);
		functionCalc->processAIGCuts_Perm(true);
		functionCalc->printUniqueFunctionStat();
		//functionCalc->processAIGCuts_Perm(true);
		gettimeofday(&func_e, NULL);//-----------------------------------------------





		//*************************************************************************
		//* Aggregate similar functions 
		//**************************************************************************
		std::map<unsigned, std::set<unsigned> >::iterator iMapS;
		std::set<unsigned>::iterator iSet;

		//DECODER AGGREGATION
		gettimeofday(&dec_b, NULL); //-----------------------------------------------
		std::map<unsigned, unsigned> decoderResult;
		std::map<unsigned, std::set<unsigned> > decoderIOResult;
		AGGREGATION::findDecoder(functionCalc, aigraph, decoderResult, decoderIOResult);
		printf("DECODER\n");
		for(iMapS = decoderIOResult.begin(); iMapS != decoderIOResult.end(); iMapS++){
			printf("OUTPUT: %3d\tINPUT: ", iMapS->first);
			for(iSet = iMapS->second.begin(); iSet != iMapS->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}
		gettimeofday(&dec_e, NULL);//------------------------------------------
		stat_decAgg.push_back(decoderResult);

		//ADDER AGGREGATION
		std::map<unsigned, unsigned> addResult;
		std::map<unsigned, unsigned> addAggResult;
		std::map<unsigned, std::set<unsigned> > addIOResult;
		std::map<unsigned, unsigned> carryResult;
		std::map<unsigned, unsigned> carryAggResult;
		std::map<unsigned, std::set<unsigned> > carryIOResult;
		gettimeofday(&add_b, NULL); //-----------------------------------------------
		AGGREGATION::findAdder(functionCalc, cut, aigraph, addResult, addAggResult, addIOResult);
		stat_adder.push_back(addResult);
		stat_adderAgg.push_back(addAggResult);
		printf("ADD\n");
		for(iMapS = addIOResult.begin(); iMapS != addIOResult.end(); iMapS++){
			printf("OUTPUT: %3d\tINPUT: ", iMapS->first);
			for(iSet = iMapS->second.begin(); iSet != iMapS->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}

		AGGREGATION::findCarry(functionCalc, cut, aigraph, carryResult, carryAggResult, carryIOResult);
		stat_carry.push_back(carryResult);
		stat_carryAgg.push_back(carryAggResult);
		gettimeofday(&add_e, NULL); //-----------------------------------------------


		//PARITY AGGREGATION
		gettimeofday(&par_b, NULL); //-----------------------------------------------
		std::map<unsigned, unsigned> parityResult;
		std::map<unsigned, std::set<unsigned> > parityIOResult;
		AGGREGATION::findParityTree(functionCalc, aigraph, parityResult, parityIOResult);
		printf("PARITY\n");
		for(iMapS = parityIOResult.begin(); iMapS != parityIOResult.end(); iMapS++){
			printf("OUTPUT: %3d\tINPUT: ", iMapS->first);
			for(iSet = iMapS->second.begin(); iSet != iMapS->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}
		stat_parity.push_back(parityResult);
		gettimeofday(&par_e, NULL); //-----------------------------------------------

		//GATE FUNCTION AGGREGATION
		gettimeofday(&gate_b, NULL); //-----------------------------------------------
		std::map<unsigned, unsigned> gateResult;
		std::map<unsigned, std::set<unsigned> > gateIOResult;
		AGGREGATION::findGateFunction(functionCalc, aigraph, gateResult, gateIOResult);
		printf("GATE\n");
		for(iMapS = gateIOResult.begin(); iMapS != gateIOResult.end(); iMapS++){
			printf("OUTPUT: %3d\tINPUT: ", iMapS->first);
			for(iSet = iMapS->second.begin(); iSet != iMapS->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}
		stat_gate.push_back(gateResult);
		gettimeofday(&gate_e, NULL); //-----------------------------------------------

		//EQUALITY FUNCTION AGGREGATION
		gettimeofday(&eq_b, NULL); //-----------------------------------------------
		std::map<unsigned, unsigned> equalResult;
		std::map<unsigned, std::set<unsigned> > equalIOResult;
		AGGREGATION::findEquality(functionCalc, aigraph, equalResult, equalIOResult);
		printf("EQUAL\n");
		for(iMapS = equalIOResult.begin(); iMapS != equalIOResult.end(); iMapS++){
			printf("OUTPUT: %3d\tINPUT: ", iMapS->first);
			for(iSet = iMapS->second.begin(); iSet != iMapS->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}
		stat_equal.push_back(equalResult);
		gettimeofday(&eq_e, NULL); //-----------------------------------------------


		//MULTIPLEXOR AGGREGATION
		gettimeofday(&mux_b, NULL); //-----------------------------------------------
		//mux size, count 
		std::map<unsigned, unsigned> muxResult2;
		std::map<unsigned, unsigned> muxResult3;
		std::map<unsigned, unsigned> muxResult4;
		std::map<unsigned, std::set<unsigned> > muxIOResult;
		AGGREGATION::findMux2(functionCalc, aigraph, muxResult2, muxResult3, muxResult4, muxIOResult);
		printf("MUX\n");
		for(iMapS = muxIOResult.begin(); iMapS != muxIOResult.end(); iMapS++){
			printf("OUTPUT: %3d\tINPUT: ", iMapS->first);
			for(iSet = iMapS->second.begin(); iSet != iMapS->second.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}

		stat_mux2.push_back(muxResult2);		
		stat_mux3.push_back(muxResult3);		
		stat_mux4.push_back(muxResult4);		
		gettimeofday(&mux_e, NULL);//------------------------------------------

		//std::vector<unsigned int> muxResult1;
		//AGGREGATION::findMux_Orig(functionCalc, aigraph, muxResult1);
		//AGGREGATION::findMux(functionCalc, aigraph, muxResult1);





		/*
			 std::map<unsigned long long, int> functionCount;
			 functionCalc->getFunctionCount(functionCount);
			 count.push_back(functionCount.size());
			 outdb<< file << "\n";
			 outdb<< functionCount.size() << "\n";
			 for(fcit = functionCount.begin(); fcit != functionCount.end(); fcit++){
			 outdb<< fcit->first << " " << fcit->second << "\n";
			 }
		 */



		//*************************************************************************
		//* Calculate Elapsed Time 
		//**************************************************************************
		std::vector<float> time;
		time.reserve(20);

		elapsedTime = (igraph_e.tv_sec - igraph_b.tv_sec) * 1000.0;
		elapsedTime += (igraph_e.tv_usec - igraph_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (iaig_e.tv_sec - iaig_b.tv_sec) * 1000.0;   
		elapsedTime += (iaig_e.tv_usec - iaig_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (ce_e.tv_sec - ce_b.tv_sec) * 1000.0;     
		elapsedTime += (ce_e.tv_usec - ce_b.tv_usec) / 1000.0;  
		time.push_back(elapsedTime/1000);

		elapsedTime = (outc_e.tv_sec - outc_b.tv_sec) * 1000.0;     
		elapsedTime += (outc_e.tv_usec - outc_b.tv_usec) / 1000.0;  
		time.push_back(elapsedTime/1000);


		elapsedTime = (ffc_e.tv_sec - ffc_b.tv_sec) * 1000.0;     
		elapsedTime += (ffc_e.tv_usec - ffc_b.tv_usec) / 1000.0;  
		time.push_back(elapsedTime/1000);

		elapsedTime = (func_e.tv_sec - func_b.tv_sec) * 1000.0;
		elapsedTime += (func_e.tv_usec - func_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (cas_e.tv_sec - cas_b.tv_sec) * 1000.0;
		elapsedTime += (cas_e.tv_usec - cas_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (blk_e.tv_sec - blk_b.tv_sec) * 1000.0;
		elapsedTime += (blk_e.tv_usec - blk_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (fb_e.tv_sec - fb_b.tv_sec) * 1000.0;
		elapsedTime += (fb_e.tv_usec - fb_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (cnt_e.tv_sec - cnt_b.tv_sec) * 1000.0;     
		elapsedTime += (cnt_e.tv_usec - cnt_b.tv_usec) / 1000.0;  
		time.push_back(elapsedTime/1000);

		elapsedTime = (rlut_e.tv_sec - rlut_b.tv_sec) * 1000.0;
		elapsedTime += (rlut_e.tv_usec - rlut_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (par_e.tv_sec - par_b.tv_sec) * 1000.0;
		elapsedTime += (par_e.tv_usec - par_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (gate_e.tv_sec - gate_b.tv_sec) * 1000.0;
		elapsedTime += (gate_e.tv_usec - gate_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (eq_e.tv_sec - eq_b.tv_sec) * 1000.0;
		elapsedTime += (eq_e.tv_usec - eq_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (mux_e.tv_sec - mux_b.tv_sec) * 1000.0;
		elapsedTime += (mux_e.tv_usec - mux_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (dec_e.tv_sec - dec_b.tv_sec) * 1000.0;
		elapsedTime += (dec_e.tv_usec - dec_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (add_e.tv_sec - add_b.tv_sec) * 1000.0;
		elapsedTime += (add_e.tv_usec - add_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);
		stat_time.push_back(time);

		delete aigraph;
		delete cut;
		delete ckt;
		functionCalc->reset();
	}

	delete functionCalc;

	//outdb << "END\n";
	//outdb.close();
	printStatement("Build Database Complete");
	//printf("[mainDB] -- Database Output File: %s\n\n", outDatabase.c_str() );




	//*************************************************************************
	//* Form the timing label 
	//**************************************************************************
	std::vector<std::string> tLabel;
	tLabel.reserve(20);
	tLabel.push_back("GRP-IM");
	tLabel.push_back("AIG-CN");
	tLabel.push_back("CUT");
	tLabel.push_back("OUT-C");
	tLabel.push_back("FF-C");
	tLabel.push_back("CUT-F");
	tLabel.push_back("FF-CAS");
	tLabel.push_back("FF-BLK");
	tLabel.push_back("FF-FB");
	tLabel.push_back("CNT");
	tLabel.push_back("LUT-R");
	tLabel.push_back("PAR-A");
	tLabel.push_back("GAT-A");
	tLabel.push_back("EQ-A");
	tLabel.push_back("MUX-A");
	tLabel.push_back("DEC-A");
	tLabel.push_back("ADD-A");









	//**************************************************************************
	//* MKR-  Print intermediate results for each circuit 
	//**************************************************************************
	for(unsigned int i = 0; i < name.size(); i++){
		printf("\n================================================================================\n");
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s\n", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		printf("================================================================================\n");

		int totalMux = 0;
		printf("\n2-1 MUX:\n");
		std::map<unsigned, unsigned>::iterator iMap;
		for(iMap = stat_mux2[i].begin(); iMap != stat_mux2[i].end(); iMap++){
			printf("\t%d-Bit mux...\t\t%d\n", iMap->first, iMap->second);
			totalMux+=iMap->second;
		}
		printf("\n3-1 MUX:\n");
		for(iMap = stat_mux3[i].begin(); iMap != stat_mux3[i].end(); iMap++){
			printf("\t%d-Bit mux...\t\t%d\n", iMap->first, iMap->second);
			totalMux+=iMap->second;
		}
		printf("\n4-1 MUX:\n");
		for(iMap = stat_mux4[i].begin(); iMap != stat_mux4[i].end(); iMap++){
			printf("\t%d-Bit mux...\t\t%d\n", iMap->first, iMap->second);
			totalMux+=iMap->second;
		}

		printf("\nDecoders\n");
		for(iMap = stat_decAgg[i].begin(); iMap != stat_decAgg[i].end(); iMap++)
			printf("\t%d-Bit decoders...\t\t%d\n", iMap->first, iMap->second);

		printf("\nAdders\n");
		for(iMap = stat_adder[i].begin(); iMap != stat_adder[i].end(); iMap++)
			printf("\t%d-Bit adder...\t\t%d\n", iMap->first, iMap->second);
		printf("\nAdderAgg\n");
		for(iMap = stat_adderAgg[i].begin(); iMap != stat_adderAgg[i].end(); iMap++)
			printf("\t%d-Bit adder...\t\t%d\n", iMap->first, iMap->second);

		printf("\nCarry\n");
		for(iMap = stat_carry[i].begin(); iMap != stat_carry[i].end(); iMap++)
			printf("\t%d-Bit adder...\t\t%d\n", iMap->first, iMap->second);
		printf("\nCarryAgg\n");
		for(iMap = stat_carryAgg[i].begin(); iMap != stat_carryAgg[i].end(); iMap++)
			printf("\t%d-Bit adder...\t\t%d\n", iMap->first, iMap->second);

		printf("\nParity\n");
		for(iMap = stat_parity[i].begin(); iMap != stat_parity[i].end(); iMap++)
			printf("\t%d-Bit parity tree...\t\t%d\n", iMap->first, iMap->second);

		printf("\nGate\n");
		for(iMap = stat_gate[i].begin(); iMap != stat_gate[i].end(); iMap++)
			printf("\t%d-Bit gate...\t\t%d\n", iMap->first, iMap->second);
		printf("\nequal\n");
		for(iMap = stat_equal[i].begin(); iMap != stat_equal[i].end(); iMap++)
			printf("\t%d-Bit gate...\t\t%d\n", iMap->first, iMap->second);

		stat_numMux.push_back(totalMux);
	}
	printf("\n\n");










	//**************************************************************************
	//* MKR-  Print additional statistics  
	//**************************************************************************
	printf("%-20s", "Circuit");
	printf("%5s", "CkPI");
	printf("%5s", "In");
	printf("%5s", "CkPO");
	printf("%8s", "|AIG|");
	printf("%8s", "|FF|");
	printf("%8s", "|MUX|");
	printf("%8s", "|FFL|");
	printf("%8s", "|LUTs|");
	printf("\n");
	printf("--------------------------------------------------------------------------------\n");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		printf("%5d", stat_numCktInput[i]);
		printf("%5d", stat_numInput[i]);
		printf("%5d", stat_numCktOutput[i]);
		printf("%8d", stat_aigSize[i]);
		printf("%8d", stat_ffSize[i]);
		printf("%8d", stat_numMux[i]);
		printf("%8d", stat_numFFFeedback[i]);
		printf("%8d", stat_numLUTs[i]);
		printf("\n");
	}
	printf("================================================================================\n");
	printf("\n\n\n");









	//**************************************************************************
	//* MKR- Print similarity results 
	//**************************************************************************
	std::vector<std::vector< std::vector<double> > > simTable;
	simTable.reserve(name.size());
	for(unsigned int i = 0; i < name.size(); i++){
		std::vector<std::vector<double> > sim(name.size());
		simTable.push_back(sim);
	}

	printf("[MAINDB] -- Calculating similarity for 2-1 Multiplexors\n");
	calculateSimilarity(name, stat_mux2, simTable);
	printf("[MAINDB] -- Calculating similarity for 3-1 Multiplexors\n");
	calculateSimilarity(name, stat_mux3, simTable);
	printf("[MAINDB] -- Calculating similarity for 4-1 Multiplexors\n");
	calculateSimilarity(name, stat_mux4, simTable);

	printf("[MAINDB] -- Calculating similarity for Output Output input dependency size\n");
	calculateSimilarity(name, stat_spCutCountOut, simTable);
	printf("[MAINDB] -- Calculating similarity for FF FF input dependency size\n");
	calculateSimilarity(name, stat_spCutCountFF, simTable);

	printf("[MAINDB] -- Calculating similarity Adder aggregation\n");
	calculateSimilarity(name, stat_adder, simTable);
	printf("[MAINDB] -- Calculating similarity Combined Adder aggregation\n");
	calculateSimilarity(name, stat_adderAgg, simTable);
	printf("[MAINDB] -- Calculating similarity carry aggregation\n");
	calculateSimilarity(name, stat_carry, simTable);
	printf("[MAINDB] -- Calculating similarity combined carry aggregation\n");
	calculateSimilarity(name, stat_carryAgg, simTable);

	printf("[MAINDB] -- Calculating similarity of parity tree aggregation\n");
	calculateSimilarity(name, stat_parity, simTable);
	printf("[MAINDB] -- Calculating similarity of gate function aggregation\n");
	calculateSimilarity(name, stat_gate, simTable);
	printf("[MAINDB] -- Calculating similarity of equality function aggregation\n");
	calculateSimilarity(name, stat_equal, simTable);

	printf("[MAINDB] -- Calculating similarity Sequential block M0\n");
	calculateSimilarity(name, stat_blockFFM0, simTable);
	printf("[MAINDB] -- Calculating similarity Sequential block M1\n");
	calculateSimilarity(name, stat_blockFFM1, simTable);
	printf("[MAINDB] -- Calculating similarity Sequential block M2\n");
	calculateSimilarity(name, stat_blockFFM2, simTable);

	printf("[MAINDB] -- Calculating similarity Sequential cascading block OFF: 1\n");
	calculateSimilarity(name, stat_cascadeFFM1, simTable);
	printf("[MAINDB] -- Calculating similarity Sequential cascading block OFF: 2\n");
	calculateSimilarity(name, stat_cascadeFFM2, simTable);
	printf("\n[MAINDB] -- Calculating similarity Sequential cascading block OFF: 3\n");
	calculateSimilarity(name, stat_cascadeFFM3, simTable);

	printf("[MAINDB] -- Calculating similarity Counter identification: 3\n");
	calculateSimilarity(name, stat_counter, simTable);

	printf("\n***********************************************************************\n");

	printf("FINAL SIM RESULT\n");
	printf("%-10s", "Circuits");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());

		for(unsigned int k = 0; k < name.size(); k++){
			double sum = 0.0;
			for(unsigned int q = 0; q < simTable[i][k].size(); q++){
				sum += simTable[i][k][q];
			}

			printf("%10.3f", (sum/simTable[i][k].size())*100.0);

		}
		printf("\n");
	}
	printf("\n");

	printf("Excel Format\n");
	printf("Circuits\t");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%s\t", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%s\t", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());

		for(unsigned int k = 0; k < name.size(); k++){
			double sum = 0.0;
			for(unsigned int q = 0; q < simTable[i][k].size(); q++){
				sum += simTable[i][k][q];
			}

			printf("%.3f\t", (sum/simTable[i][k].size())*100.0);

		}
		printf("\n");
	}
	printf("\n");




	simTable.clear();
	simTable.reserve(name.size());
	for(unsigned int i = 0; i < name.size(); i++){
		std::vector<std::vector<double> > sim(name.size());
		simTable.push_back(sim);
	}
	printf("[MAINDB] -- Calculating similarity for 2-1 Multiplexors\n");
	calculateSimilarity_size(name, stat_mux2, simTable);
	printf("[MAINDB] -- Calculating similarity for 3-1 Multiplexors\n");
	calculateSimilarity_size(name, stat_mux3, simTable);
	printf("[MAINDB] -- Calculating similarity for 4-1 Multiplexors\n");
	calculateSimilarity_size(name, stat_mux4, simTable);

	printf("[MAINDB] -- Calculating similarity for Output Output input dependency size\n");
	calculateSimilarity_size(name, stat_spCutCountOut, simTable);
	printf("[MAINDB] -- Calculating similarity for FF FF input dependency size\n");
	calculateSimilarity_size(name, stat_spCutCountFF, simTable);

	printf("[MAINDB] -- Calculating similarity of Adder aggregation\n");
	calculateSimilarity_size(name, stat_adder, simTable);
	printf("[MAINDB] -- Calculating similarity of Combined Adder aggregation\n");
	calculateSimilarity_size(name, stat_adderAgg, simTable);
	printf("[MAINDB] -- Calculating similarity of carry aggregation\n");
	calculateSimilarity_size(name, stat_carry, simTable);
	printf("[MAINDB] -- Calculating similarity of combined carry aggregation\n");
	calculateSimilarity_size(name, stat_carryAgg, simTable);

	printf("[MAINDB] -- Calculating similarity of parity tree aggregation\n");
	calculateSimilarity_size(name, stat_parity, simTable);
	printf("[MAINDB] -- Calculating similarity of gate function aggregation\n");
	calculateSimilarity_size(name, stat_gate, simTable);
	printf("[MAINDB] -- Calculating similarity of equality function aggregation\n");
	calculateSimilarity_size(name, stat_equal, simTable);

	printf("[MAINDB] -- Calculating similarity of Sequential block M0\n");
	calculateSimilarity_size(name, stat_blockFFM0, simTable);
	printf("[MAINDB] -- Calculating similarity of Sequential block M1\n");
	calculateSimilarity_size(name, stat_blockFFM1, simTable);
	printf("[MAINDB] -- Calculating similarity of Sequential block M2\n");
	calculateSimilarity_size(name, stat_blockFFM2, simTable);

	printf("[MAINDB] -- Calculating similarity of Sequential cascading block OFF: 1\n");
	calculateSimilarity_size(name, stat_cascadeFFM1, simTable);
	printf("[MAINDB] -- Calculating similarity of Sequential cascading block OFF: 2\n");
	calculateSimilarity_size(name, stat_cascadeFFM2, simTable);
	printf("[MAINDB] -- Calculating similarity of Sequential cascading block OFF: 3\n");
	calculateSimilarity_size(name, stat_cascadeFFM3, simTable);

	printf("[MAINDB] -- Calculating similarity of Counter identification: 3\n");
	calculateSimilarity_size(name, stat_counter, simTable);

	printf("Excel Format Size\n");
	printf("Circuits\t");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%s\t", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%s\t", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());

		for(unsigned int k = 0; k < name.size(); k++){
			double sum = 0.0;
			for(unsigned int q = 0; q < simTable[i][k].size(); q++){
				sum += simTable[i][k][q];
			}

			printf("%.3f\t", (sum/simTable[i][k].size())*100.0);

		}
		printf("\n");
	}
	printf("\n");

	//std::cout<<"\033[1;4;31m"<<"BLUE TEXT"<<"\033[0m"<<std::endl;





	//**************************************************************************
	//* MKR-  Print elapsed timing statistics  
	//**************************************************************************
	printf("\n\n\n");
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	printStatement("Execution Timing Statistics");


	printf("%-20s", "Circuit");
	for(unsigned int i = 0; i < tLabel.size(); i++)
		printf("%-10s", tLabel[i].c_str());
	printf("%-10s", "TOTAL");
	printf("\n");

	std::vector<float> totalTime;
	if(stat_time.size() != 0)
		totalTime.resize(stat_time[0].size(), 0);

	for(unsigned int i = 0; i < stat_time.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		double cTotal = 0.00;
		for(unsigned int k = 0; k < stat_time[i].size(); k++){
			printf("%-10.3f", stat_time[i][k]);
			totalTime[k]+=stat_time[i][k];
			cTotal += stat_time[i][k];
		}

		printf("%-10.3f", cTotal);
		printf("\n");
	}

	printf("------------------------------------------------------------------\n");
	printf("%-20s", "TOTAL:");
	float totaltotal = 0.0;
	for(unsigned int k = 0; k < totalTime.size(); k++){
		printf("%-10.3f", totalTime[k]);
		totaltotal +=totalTime[k];
	}
	printf("\nTotal Time:  %f\n", totaltotal);
	printf("\n\n[ --------***--------      END      --------***-------- ]\n\n");

	return 0;

}









void calculateSimilarity(std::vector<std::string>& name, 
		std::vector<std::map<unsigned, unsigned> >& fingerprint,
		std::vector<std::vector< std::vector<double> > >& simTable){

	/*
		 printf("%-10s", "Circuits");
		 for(unsigned int i = 0; i < name.size(); i++){
		 int lastSlashIndex = name[i].find_last_of("/") + 1;
		 printf("%10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		 }
		 printf("\n");
	 */

	for(unsigned int i = 0; i < name.size(); i++){
		/*
			 int lastSlashIndex = name[i].find_last_of("/") + 1;
			 printf("%-10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		 */

		for(unsigned int k = 0; k < name.size(); k++){

			double sim;
			if(fingerprint[i].size() == 0 and fingerprint[k].size() == 0)
				sim = 1.00;
			else
				sim = SIMILARITY::tanimotoWindow(fingerprint[i], fingerprint[k]);

			simTable[i][k].push_back(sim);
			//printf("%10.3f", sim*100);
		}
		//printf("\n");
	}
}




void calculateSimilarity_size(std::vector<std::string>& name, 
		std::vector<std::map<unsigned, unsigned> >& fingerprint,
		std::vector<std::vector< std::vector<double> > >& simTable){

	for(unsigned int i = 0; i < name.size(); i++){
		for(unsigned int k = 0; k < name.size(); k++){
			double sim;
			if(fingerprint[i].size() == 0 and fingerprint[k].size() == 0)
				sim = 1.00;
			else
				sim = SIMILARITY::tanimotoWindow_size(fingerprint[i], fingerprint[k]);

			simTable[i][k].push_back(sim);
		}
	}
}


#endif
