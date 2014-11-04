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
//#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/vf2_sub_graph_iso.hpp>

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
#include "aggregation.hpp"
#include "similarity.hpp"

//using namespace boost;

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

void calculateSimilarity(std::vector<std::string>& name, 
		std::vector<std::map<unsigned, unsigned> >& fingerprint,
		std::vector<std::vector< std::vector<double> > >& simTable);

void calculateSimilarity_size(std::vector<std::string>& name, 
		std::vector<std::map<unsigned, unsigned> >& fingerprint,
		std::vector<std::vector< std::vector<double> > >& simTable);


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
	if(argc != 4)
	{

		printf("./xfpgeniusDB <primitive file> <database file> <output database>\n\n\n");
		return 0;
	}

	//**************************************************************************
	//* MKR-   Declarations
	//**************************************************************************
	std::string primBase = argv[1];          //Location of primitive circuits
	std::string dataBase = argv[2];          //Location of database file
	std::string outDatabase = argv[3];       //Output database file
	std::string option= "";                  //Command line options

	//Time tracking
	timeval iaig_b, iaig_e;
	timeval ce_b, ce_e;
	timeval func_b, func_e;
	timeval rlut_b, rlut_e ;
	timeval igraph_b, igraph_e;
	timeval count_b, count_e;
	timeval fb_b, fb_e;
	timeval reg_b, reg_e;
	timeval mux_b, mux_e;
	timeval dec_b, dec_e;
	//timeval fgp_b, fgp_e;
	timeval add_b, add_e;

	float elapsedTime;
	int k = 6;                              //k-Cut enumeration value 

	std::ofstream outdb;                    //Database output file stream
	std::map<std::string, std::set<unsigned long long> > pDatabase;
	std::map<std::string, double> similarity;

	//function, count
	std::map<unsigned long long, int>::iterator fcit;

	//Circuit statistic
	std::vector<std::map<std::string, int> > stat_compCount;  //Component, count
	std::vector<unsigned int> stat_wienerIndex;
	std::vector<unsigned int> stat_aigSize;
	std::vector<unsigned int> stat_ffSize;
	std::vector<unsigned int> stat_dspSize;
	std::vector<unsigned int> stat_numInput;
	std::vector<unsigned int> stat_numCktInput;
	std::vector<unsigned int> stat_numCktOutput;
	std::vector<unsigned int> stat_numOutput;
	std::vector<unsigned int> stat_numMux;
	std::vector<unsigned int> stat_numReg;
	std::vector<unsigned int> stat_numLUTs;
	std::vector<unsigned int> stat_numMuxcy;
	std::vector<unsigned int> stat_numXorcy;
	std::vector<unsigned int> stat_numFFFeedback;

	//Mux Type (2-1, 4-1), mux size (2bit, 4bit), count
	std::vector<std::map<unsigned, unsigned> >  stat_decAgg;
	std::vector<std::vector<unsigned> >  stat_addAgg;

	std::vector< std::vector<float> > stat_time;
	std::vector<float> stat_fingerprintTime;

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

	std::vector<unsigned > stat_f1;
	std::vector<unsigned > stat_f2;
	std::vector<unsigned > stat_f1f2;






	//**************************************************************************
	//* MKR-  Get options 
	//**************************************************************************
	if(argc == 5) 
		option = argv[4];

	verbose = false;
	if(option.find("v") != std::string::npos)
		verbose = true;




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
	printStatement("Performing Library preprocessing");
	CutFunction* functionCalc = new CutFunction();
	functionCalc->preProcessLibrary(primBase);
	//functionCalc->printLibrary();



	outdb.open(outDatabase.c_str());
	outdb<<aigComponentDefinition<<"\n";

	std::string file = "";
	std::vector<int> count; 
	std::vector<std::string> name;
	//Go through each circuit in the database and preprocess data/signature 
	//int sdfid = 100183;


	while(getline(infile, file)){
		//Handles empty lines in files
		if(file == "\n")
			continue;

		std::vector<float> time;
		time.reserve(10);

		printStatement("[MAIN] -- Processing File: " +  file);
		name.push_back(file);

		//Import circuit and convert to AIG
		gettimeofday(&igraph_b, NULL); //--------------------------------------------
		Graph* ckt = new Graph(file);
		ckt->importGraph(file, 0);
		gettimeofday(&igraph_e, NULL); //--------------------------------------------



		//Replace all the luts with combinational logic
		gettimeofday(&rlut_b, NULL); //--------------------------------------------
		unsigned int numLUTs = SEQUENTIAL::replaceLUTs(ckt);
		gettimeofday(&rlut_e, NULL); //--------------------------------------------
		printf(" * Replaced %d LUTs\n", numLUTs);
		stat_numLUTs.push_back(numLUTs);



		//Count the number of specific components
		std::map<int, Vertex*>::iterator cktit;
		std::map<std::string, int> compCount;   //Component, count
		unsigned int dspcount = 0;
		unsigned int muxcycount = 0;
		unsigned int xorcycount = 0;
		//select bit, count
		std::map<int,int> muxSelCount;

		gettimeofday(&count_b, NULL); //--------------------------------------------
		for(cktit = ckt->begin(); cktit != ckt->end(); cktit++){
			if(compCount.find(cktit->second->getType()) != compCount.end())
				compCount[cktit->second->getType()]++;
			else
				compCount[cktit->second->getType()] = 1; 

			std::string type = cktit->second->getType();
			if (type.find("MUXCY") != std::string::npos){
				muxcycount++;
				std::vector<Vertex*> input;
				std::vector<std::string> inputPort;

				cktit->second->getInput(input);
				cktit->second->getInputPorts(inputPort);

				assert(input.size() == 3);
				for(unsigned int i = 0; i < input.size(); i++){
					//printf("%s:%d   ", inputPort[i].c_str(), input[i]->getID());
					if(inputPort[i] == "S"){
						if(muxSelCount.find(input[i]->getID()) == muxSelCount.end())
							muxSelCount[input[i]->getID()] = 1;
						else
							muxSelCount[input[i]->getID()]++;
					}
				}
			}
			else if (type.find("XORCY") != std::string::npos)
				xorcycount++;
			else if (type.find("DSP") != std::string::npos)
				dspcount++;

		}


		std::list<Vertex*> ffList;
		SEQUENTIAL::getFFList(ckt, ffList);
		gettimeofday(&count_e, NULL); //--------------------------------------------

		/*
			 std::map<int,int>::iterator mscit;
			 for(mscit = muxSelCount.begin(); mscit != muxSelCount.end(); mscit++){
			 printf("SELECT BIT: %d\tCOUNT: %d\n", mscit->first, mscit->second);
			 }
		 */


		stat_compCount.push_back(compCount);
		stat_ffSize.push_back(ffList.size());
		stat_dspSize.push_back(dspcount);
		stat_numXorcy.push_back(xorcycount);
		stat_numMuxcy.push_back(muxcycount);



		//Count the number of feed back flip flops	
		gettimeofday(&fb_b, NULL); //--------------------------------------------
		stat_numFFFeedback.push_back(SEQUENTIAL::getNumFFFeedback(ckt, ffList));
		gettimeofday(&fb_e, NULL); //--------------------------------------------


		//Count the number of enable based registers
		gettimeofday(&reg_b, NULL); //--------------------------------------------
		std::map<unsigned, unsigned> rgcount;
		SEQUENTIAL::findRegisterGroup(ffList, rgcount);
		stat_reg.push_back(rgcount);
		gettimeofday(&reg_e, NULL); //--------------------------------------------


		stat_numCktInput.push_back(ckt->getNumInputs());
		stat_numCktOutput.push_back(ckt->getNumOutputs());

		//FIND COUNTERS
		//SEQUENTIAL::counterIdentification(ckt);
		std::map<unsigned,unsigned> casFFM1;
		std::map<unsigned,unsigned> casFFM2;
		std::map<unsigned,unsigned> casFFM3;
		std::map<unsigned,unsigned> blockFFM0;
		std::map<unsigned,unsigned> blockFFM1;
		std::map<unsigned,unsigned> blockFFM2;
		std::map<unsigned,unsigned> counters;
		SEQUENTIAL::cascadingFF(ckt, 1, casFFM1);
		SEQUENTIAL::cascadingFF(ckt, 2, casFFM2);
		SEQUENTIAL::cascadingFF(ckt, 3, casFFM3);
		SEQUENTIAL::blockFF(ckt, 0, blockFFM0);
		SEQUENTIAL::blockFF(ckt, 1, blockFFM1);
		SEQUENTIAL::blockFF(ckt, 2, blockFFM2);
		SEQUENTIAL::counterIdentification(ckt, counters);

		stat_cascadeFFM1.push_back(casFFM1);
		stat_cascadeFFM2.push_back(casFFM2);
		stat_cascadeFFM3.push_back(casFFM3);
		stat_blockFFM0.push_back(blockFFM0);
		stat_blockFFM1.push_back(blockFFM1);
		stat_blockFFM2.push_back(blockFFM2);
		stat_counter.push_back(counters);


		//Begin conversion to AIG
		AIG* aigraph = new AIG();
		/*aigraph->convertGraph2AIG(ckt, true);
			int lastSlashIndex = file.find_last_of("/") + 1;
			std::string cname = file.substr(lastSlashIndex, file.length()-lastSlashIndex-4);
			ckt->exportGraphSDFV3000(cname, sdfid);
			sdfid++;
		 */

		gettimeofday(&iaig_b, NULL); //--------------------------------------------
		aigraph->convertGraph2AIG(ckt, false);
		//aigraph->printMap();
		//aigraph->print();
		gettimeofday(&iaig_e, NULL);

		stat_aigSize.push_back(aigraph->getSize());
		stat_numInput.push_back(ckt->getNumInputs());
		stat_numOutput.push_back(ckt->getNumOutputs());



		/*
			 std::list<unsigned> out;
			 std::set<unsigned> in;
			 in.insert(2);
			 in.insert(4);
			 in.insert(6);
			 in.insert(8);
			 in.insert(10);
			 in.insert(12);
			 in.insert(14);
			 in.insert(16);
			 out.push_back(204);
			 aigraph->printSubgraph(out, in);
			 out.clear();
		 */







		//PERFORM K CUT ENUMERATION
		gettimeofday(&ce_b, NULL); //------------------------------------------------
		CutEnumeration* cut = new CutEnumeration (aigraph);
		cut->findKFeasibleCuts(k);
		//cut->print();



		/**********************************************************************
		 *
		 * Searching for output/FF input cut correspondence
		 *
		 **********************************************************************/

		//Find input cut for FF nodes----------------------------
		std::vector<unsigned> nodes;

		//Cut input size, count
		aigraph->getFFInput(nodes);
		cut->findInputCut(nodes);

		//Cut input size, count
		std::map<unsigned, unsigned> cutCountFF;

		//Node, Set of inputs to the node
		std::map<unsigned, std::set<unsigned> > cutIn;
		std::map<unsigned, std::set<unsigned> >::iterator iCut;
		cut->getCut2(cutIn);

		for(iCut = cutIn.begin(); iCut != cutIn.end(); iCut++){
			if(cutCountFF.find(iCut->second.size()) == cutCountFF.end())
				cutCountFF[iCut->second.size()] = 1;
			else
				cutCountFF[iCut->second.size()]++; 
		}
		stat_spCutCountFF.push_back(cutCountFF);


		//Find input cut for out nodes----------------------------
		nodes.clear();
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





		gettimeofday(&ce_e, NULL); //------------------------------------------------




		//PERFORM BOOLEAN MATCHING
		gettimeofday(&func_b, NULL);//-----------------------------------------------
		functionCalc->setParams(cut, aigraph);
		functionCalc->processAIGCuts(true);
		//functionCalc->processAIGCuts_Perm(true);
		gettimeofday(&func_e, NULL);//-----------------------------------------------
		functionCalc->printUniqueFunctionStat();






		/***************************************************************************
		 *
		 * Start aggregation
		 *
		 ***************************************************************************/
		//DECODER AGGREGATION
		gettimeofday(&dec_b, NULL); //-----------------------------------------------
		std::map<unsigned, unsigned> decoderResult;
		AGGREGATION::findDecoder(functionCalc, aigraph, decoderResult);
		gettimeofday(&dec_e, NULL);//------------------------------------------
		stat_decAgg.push_back(decoderResult);

		//ADDER AGGREGATION
		std::map<unsigned, unsigned> addResult;
		std::map<unsigned, unsigned> addAggResult;
		std::map<unsigned, unsigned> carryResult;
		std::map<unsigned, unsigned> carryAggResult;
		gettimeofday(&add_b, NULL); //-----------------------------------------------
		AGGREGATION::findAdder(functionCalc, cut, aigraph, addResult, addAggResult);
		stat_adder.push_back(addResult);
		stat_adderAgg.push_back(addAggResult);

		AGGREGATION::findCarry(functionCalc, cut, aigraph, carryResult, carryAggResult);
		stat_carry.push_back(carryResult);
		stat_carryAgg.push_back(carryAggResult);
		gettimeofday(&add_e, NULL); //-----------------------------------------------


		//PARITY AGGREGATION
		std::map<unsigned, unsigned> parityResult;
		AGGREGATION::findParityTree(functionCalc, aigraph, parityResult);
		stat_parity.push_back(parityResult);

		//GATE FUNCTION AGGREGATION
		std::map<unsigned, unsigned> gateResult;
		AGGREGATION::findGateFunction(functionCalc, aigraph, gateResult);
		stat_gate.push_back(gateResult);

		//EQUALITY FUNCTION AGGREGATION
		std::map<unsigned, unsigned> equalResult;
		AGGREGATION::findEquality(functionCalc, aigraph, equalResult);
		stat_equal.push_back(equalResult);


		//MULTIPLEXOR AGGREGATION
		gettimeofday(&mux_b, NULL); //-----------------------------------------------
		//mux size, count 
		std::map<unsigned, unsigned> muxResult2;
		std::map<unsigned, unsigned> muxResult3;
		std::map<unsigned, unsigned> muxResult4;
		AGGREGATION::findMux2(functionCalc, aigraph, muxResult2, muxResult3, muxResult4);

		stat_mux2.push_back(muxResult2);		
		stat_mux3.push_back(muxResult3);		
		stat_mux4.push_back(muxResult4);		
		gettimeofday(&mux_e, NULL);//------------------------------------------

		//std::vector<unsigned int> muxResult1;
		//AGGREGATION::findMux_Orig(functionCalc, aigraph, muxResult1);
		//AGGREGATION::findMux(functionCalc, aigraph, muxResult1);





		std::map<unsigned long long, int> functionCount;
		functionCalc->getFunctionCount(functionCount);
		count.push_back(functionCount.size());
		/*
			 outdb<< file << "\n";
			 outdb<< functionCount.size() << "\n";
			 for(fcit = functionCount.begin(); fcit != functionCount.end(); fcit++){
			 outdb<< fcit->first << " " << fcit->second << "\n";
			 }
		 */



		//Calculate elapsed time
		elapsedTime = (igraph_e.tv_sec - igraph_b.tv_sec) * 1000.0;
		elapsedTime += (igraph_e.tv_usec - igraph_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (iaig_e.tv_sec - iaig_b.tv_sec) * 1000.0;   
		elapsedTime += (iaig_e.tv_usec - iaig_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (ce_e.tv_sec - ce_b.tv_sec) * 1000.0;     
		elapsedTime += (ce_e.tv_usec - ce_b.tv_usec) / 1000.0;  
		time.push_back(elapsedTime/1000);

		elapsedTime = (func_e.tv_sec - func_b.tv_sec) * 1000.0;
		elapsedTime += (func_e.tv_usec - func_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (reg_e.tv_sec - reg_b.tv_sec) * 1000.0;
		elapsedTime += (reg_e.tv_usec - reg_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (fb_e.tv_sec - fb_b.tv_sec) * 1000.0;
		elapsedTime += (fb_e.tv_usec - fb_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (count_e.tv_sec - count_b.tv_sec) * 1000.0;
		elapsedTime += (count_e.tv_usec - count_b.tv_usec) / 1000.0;
		time.push_back(elapsedTime/1000);

		elapsedTime = (rlut_e.tv_sec - rlut_b.tv_sec) * 1000.0;
		elapsedTime += (rlut_e.tv_usec - rlut_b.tv_usec) / 1000.0;
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

	outdb << "END\n";
	outdb.close();
	printStatement("Build Database Complete");
	printf("[mainDB] -- Database Output File: %s\n\n", outDatabase.c_str() );








	//**************************************************************************
	//* MKR-  Print intermediate results for each circuit 
	//**************************************************************************
	//Counting aggregated muxes
	std::map<unsigned, std::map<unsigned, unsigned> >::iterator iMapM;
	std::map<unsigned, unsigned>::iterator iMap;
	std::vector<std::vector<unsigned long long> >fpRegs;
	std::vector<std::vector<unsigned long long> >fpMuxes;
	std::vector<std::vector<unsigned long long> >fingerprintList;
	fpRegs.reserve(name.size());
	fpMuxes.reserve(name.size());
	fingerprintList.reserve(name.size());

	for(unsigned int i = 0; i < name.size(); i++){
		printf("\n================================================================================\n");
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s\n", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		printf("================================================================================\n");

		/*
		//Get fingerprint for mux
		gettimeofday(&fgp_b, NULL);//------------------------------------------
		std::vector<unsigned long long> fingerprintMux;
		fingerprintMux.resize(12);
		SIMILARITY::getMuxFingerprint_naive(stat_muxAgg[i], fingerprintMux);
		// * 0-11 mux
		// * 12-15 reg
		fpMuxes.push_back(fingerprintMux);

		std::vector<unsigned long long> fingerprint;
		for(unsigned int k = 0; k < fingerprintMux.size(); k++)
		fingerprint.push_back(fingerprintMux[k]);

		gettimeofday(&fgp_e, NULL);//------------------------------------------
		elapsedTime = (fgp_e.tv_sec - fgp_b.tv_sec) * 1000.0;
		elapsedTime += (fgp_e.tv_usec - fgp_b.tv_usec) / 1000.0;




		gettimeofday(&fgp_b, NULL);//------------------------------------------
		std::vector<unsigned long long> fingerprintReg;
		fingerprintReg.resize(4);
		SIMILARITY::getRegFingerprint_naive(stat_reg[i], fingerprintReg);
		fpRegs.push_back(fingerprintReg);

		for(unsigned int k = 0; k < fingerprintReg.size(); k++)
		fingerprint.push_back(fingerprintReg[k]);
		fingerprintList.push_back(fingerprint);

		gettimeofday(&fgp_e, NULL);//------------------------------------------
		elapsedTime += (fgp_e.tv_sec - fgp_b.tv_sec) * 1000.0;
		elapsedTime += (fgp_e.tv_usec - fgp_b.tv_usec) / 1000.0;
		 */

		int totalMux = 0;
		int totalReg = 0;
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

		printf("\nregisters\n");
		for(iMap = stat_reg[i].begin(); iMap != stat_reg[i].end(); iMap++){
			printf("\t%d-bit Reg %7d\n", iMap->first, iMap->second);
			totalReg+=iMap->second;
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

		/*
			 std::map<unsigned, unsigned>::iterator iCount;
			 printf("Special Cut FF Input size Count\n");
			 for(iCount = stat_spCutCountFF[i].begin(); iCount != stat_spCutCountFF[i].end(); iCount++){
			 printf(" * Size: %3d\tCount: %3d\n", iCount->first, iCount->second);
			 }
			 printf("Special Cut Out Input size Count\n");
			 for(iCount = stat_spCutCountOut[i].begin(); iCount != stat_spCutCountOut[i].end(); iCount++){
			 printf(" * Size: %3d\tCount: %3d\n", iCount->first, iCount->second);
			 }
		 */



		stat_numMux.push_back(totalMux);
		stat_numReg.push_back(totalReg);
		stat_fingerprintTime.push_back(elapsedTime);

		/*
			 printf("\nMux Fingerprint:\t");
			 for(unsigned int i =  0; i < fingerprintMux.size(); i++){
			 printf("%llx ", fingerprintMux[i]);
			 }
			 printf("\n");

			 printf("Register fingerprint:\t");
			 for(unsigned int i =  0; i < fingerprintReg.size(); i++){
			 printf("%llx ", fingerprintReg[i]);
			 }
			 printf("\n");
		 */
	}
	printf("\n\n");










	//**************************************************************************
	//* MKR-  Print additional statistics  
	//**************************************************************************
	printf("%-20s", "Circuit");
	printf("%5s", "CkPI");
	printf("%5s", "In");
	printf("%5s", "CkPO");
	printf("%8s", "Func");
	//printf("%8s", "Wiener");
	printf("%8s", "|AIG|");
	printf("%8s", "|FF|");
	printf("%8s", "|MUX|");
	printf("%8s", "|REG|");
	printf("%8s", "|DSP|");
	printf("%8s", "|FFL|");
	printf("%8s", "|MCY|");
	printf("%8s", "|XCY|");
	printf("%8s", "|LUTs|");
	printf("\n");
	printf("--------------------------------------------------------------------------------\n");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		printf("%5d", stat_numCktInput[i]);
		printf("%5d", stat_numInput[i]);
		printf("%5d", stat_numCktOutput[i]);
		printf("%8d", count[i]);
		//printf("%8d", stat_wienerIndex[i]);
		printf("%8d", stat_aigSize[i]);
		printf("%8d", stat_ffSize[i]);
		printf("%8d", stat_numMux[i]);
		printf("%8d", stat_numReg[i]);
		printf("%8d", stat_dspSize[i]);
		printf("%8d", stat_numFFFeedback[i]);
		printf("%8d", stat_numMuxcy[i]);
		printf("%8d", stat_numXorcy[i]);
		printf("%8d", stat_numLUTs[i]);
		printf("\n");
	}
	printf("================================================================================\n");
	printf("\n\n\n");




	std::vector<std::vector< std::vector<double> > > simTable;
	simTable.reserve(name.size());
	for(unsigned int i = 0; i < name.size(); i++){
		std::vector<std::vector<double> > sim(name.size());
		simTable.push_back(sim);
	}





	//**************************************************************************
	//* MKR- Print similarity results 
	//**************************************************************************
	printf("[MAINDB] -- Calculating similarity for 2-1 Multiplexors\n");
	calculateSimilarity(name, stat_mux2, simTable);
	printf("[MAINDB] -- Calculating similarity for 3-1 Multiplexors\n");
	calculateSimilarity(name, stat_mux3, simTable);
	printf("[MAINDB] -- Calculating similarity for 4-1 Multiplexors\n");
	calculateSimilarity(name, stat_mux4, simTable);

	//printf("\n[MAINDB] -- Calculating similarity for Register fingerprint\n");
	//calculateSimilarity(name, stat_reg, simTable);

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
	printf("%-12s", "I Graph");
	printf("%-12s", "I AIG");
	printf("%-12s", "CE");
	printf("%-12s", "CF");
	printf("%-12s", "REG");
	printf("%-12s", "F-FF");
	printf("%-12s", "C-Count");
	printf("%-12s", "LUT RPLC");
	printf("%-12s", "MUX AGG");
	printf("%-12s", "DEC AGG");
	printf("%-12s", "ADD AGG");
	printf("\n") ;

	std::vector<float> totalTime;
	if(stat_time.size() != 0)
		totalTime.resize(stat_time[0].size()+1, 0);

	for(unsigned int i = 0; i < stat_time.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-4).c_str());
		for(unsigned int k = 0; k < stat_time[i].size(); k++){
			printf("%-12.3f", stat_time[i][k]);
			totalTime[k]+=stat_time[i][k];
		}

		//	printf("%-12.3f", stat_fingerprintTime[i]);
		//	totalTime[stat_time[i].size()]+=stat_fingerprintTime[i];
		printf("\n");
	}

	printf("------------------------------------------------------------------\n");
	printf("%-20s", "TOTAL:");
	float totaltotal = 0.0;
	for(unsigned int k = 0; k < totalTime.size(); k++){
		printf("%-12.3f", totalTime[k]);
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
