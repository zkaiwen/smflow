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
#include "fingerprint.hpp"

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
	timeval fgp_b, fgp_e;
	timeval add_b, add_e;

	float elapsedTime;
	int k = 4;                              //k-Cut enumeration value 

	std::ofstream outdb;                    //Database output file stream
	std::map<std::string, std::set<unsigned long> > pDatabase;
	std::map<std::string, double> similarity;

	//function, count
	std::map<unsigned long, int>::iterator fcit;

	//Circuit statistic
	std::vector<std::map<std::string, int> > stat_compCount;  //Component, count
	std::vector<unsigned int> stat_wienerIndex;
	std::vector<unsigned int> stat_aigSize;
	std::vector<unsigned int> stat_ffSize;
	std::vector<unsigned int> stat_dspSize;
	std::vector<unsigned int> stat_numInput;
	std::vector<unsigned int> stat_numOutput;
	std::vector<unsigned int> stat_numMux;
	std::vector<unsigned int> stat_numReg;
	std::vector<unsigned int> stat_numLUTs;
	std::vector<unsigned int> stat_numMuxcy;
	std::vector<unsigned int> stat_numXorcy;
	std::vector<unsigned int> stat_numFFFeedback;

	//Mux Type (2-1, 4-1), mux size (2bit, 4bit), count
	std::vector<std::map<int, std::map<int, int> > >  stat_muxAgg;
	std::vector<std::map<int, int> >  stat_decAgg;
	std::vector<std::vector<int> >  stat_addAgg;

	//size/count
	std::vector<std::map<int, int> >  stat_reg;
	std::vector< std::vector<float> > stat_time;
	std::vector<float> stat_fingerprintTime;





	//**************************************************************************
	//* MKR-  Get options 
	//**************************************************************************
	if(argc == 5) 
		option = argv[4];

	verbose = false;
	if(option.find("v") != std::string::npos)
		verbose = true;



	//*************************************************************************
	//*  Preprocess library for boolean matching 
	//**************************************************************************
	printStatement("Performing Library preprocessing");
	CutFunction* functionCalc = new CutFunction();
	functionCalc->preProcessLibrary(primBase);
	functionCalc->printLibrary();



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
		std::map<int, int> rgcount;
		SEQUENTIAL::findRegisterGroup(ffList, rgcount);
		stat_reg.push_back(rgcount);
		gettimeofday(&reg_e, NULL); //--------------------------------------------





		//Begin conversion to AIG
		AIG* aigraph = new AIG();
		/*aigraph->convertGraph2AIG(ckt, true);
			int lastSlashIndex = file.find_last_of("/") + 1;
			std::string cname = file.substr(lastSlashIndex, file.length()-lastSlashIndex-2);
			ckt->exportGraphSDFV3000(cname, sdfid);
			sdfid++;
		 */

		gettimeofday(&iaig_b, NULL); //--------------------------------------------
		aigraph->convertGraph2AIG(ckt, false);
		gettimeofday(&iaig_e, NULL);

		stat_aigSize.push_back(aigraph->getSize());
		stat_numInput.push_back(ckt->getNumInputs());
		stat_numOutput.push_back(ckt->getNumOutputs());




		//PERFORM K CUT ENUMERATION
		gettimeofday(&ce_b, NULL); //------------------------------------------------
		CutEnumeration* cut = new CutEnumeration (aigraph);
		cut->findKFeasibleCuts(k);
		gettimeofday(&ce_e, NULL); //------------------------------------------------




		//PERFORM BOOLEAN MATCHING
		gettimeofday(&func_b, NULL);//-----------------------------------------------
		functionCalc->setParams(cut, aigraph);
		functionCalc->processAIGCuts(true);
		functionCalc->processAIGCuts_Perm(true);
		functionCalc->printFunctionCount();
		functionCalc->printUniqueFunctionStat();
		functionCalc->reset();

/*
		std::list<unsigned> out;
		std::set<unsigned> in;
		out.push_back(2248);
		in.insert(236);
		in.insert(1384);
		in.insert(1656);
		in.insert(1816);
		aigraph->printSubgraph(out, in);
		out.clear();
		in.clear();
		out.push_back(3060);
		in.insert(262);
		in.insert(1368);
		in.insert(3030);
		in.insert(3032);
		aigraph->printSubgraph(out, in);
		out.clear();
		in.clear();
		out.push_back(2672);
		in.insert(1396);
		in.insert(2062);
		in.insert(2170);
		in.insert(2172);
		aigraph->printSubgraph(out, in);
		*/

		gettimeofday(&func_e, NULL);//-----------------------------------------------




		gettimeofday(&mux_b, NULL); //-----------------------------------------------
		//4-1         8 bit       2 of them
		//mux size, array size, count 
		std::map<int,std::map<int, int> > muxResult;
		AGGREGATION::findMux2(functionCalc, aigraph, muxResult);
		gettimeofday(&mux_e, NULL);//------------------------------------------
		stat_muxAgg.push_back(muxResult);		

		
		gettimeofday(&dec_b, NULL); //-----------------------------------------------
		std::map<int, int > decoderResult;
		AGGREGATION::findDecoder(functionCalc, aigraph, decoderResult);
		gettimeofday(&dec_e, NULL);//------------------------------------------
		stat_decAgg.push_back(decoderResult);
		
		gettimeofday(&add_b, NULL); //-----------------------------------------------
		std::vector<int> addResult;
		AGGREGATION::findAdder(functionCalc, aigraph, addResult);
		stat_addAgg.push_back(addResult);
		gettimeofday(&add_e, NULL); //-----------------------------------------------





			 std::map<unsigned long, int> functionCount;
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
	}

	outdb << "END\n";
	outdb.close();
	printStatement("Build Database Complete");
	printf("[mainDB] -- Database Output File: %s\n\n", outDatabase.c_str() );








	//**************************************************************************
	//* MKR-  Print intermediate results for each circuit 
	//**************************************************************************
	//Counting aggregated muxes
	std::map<int, std::map<int, int> >::iterator iMapM;
	std::map<int, int>::iterator iMap;
	std::vector<std::vector<unsigned long> >fpRegs;
	std::vector<std::vector<unsigned long> >fpMuxes;
	std::vector<std::vector<unsigned long> >fingerprintList;
	fpRegs.reserve(name.size());
	fpMuxes.reserve(name.size());
	fingerprintList.reserve(name.size());

	for(unsigned int i = 0; i < name.size(); i++){
		printf("================================================================================\n");
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s\n", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());

		//Get fingerprint for mux
		gettimeofday(&fgp_b, NULL);//------------------------------------------
		std::vector<unsigned long> fingerprintMux;
		fingerprintMux.resize(12);
		FINGERPRINT::getMuxFingerprint_naive(stat_muxAgg[i], fingerprintMux);
		/*
		* 0-11 mux
		* 12-15 reg
		*/
		fpMuxes.push_back(fingerprintMux);

		std::vector<unsigned long> fingerprint;
		for(unsigned int k = 0; k < fingerprintMux.size(); k++)
			fingerprint.push_back(fingerprintMux[k]);

		gettimeofday(&fgp_e, NULL);//------------------------------------------
		elapsedTime = (fgp_e.tv_sec - fgp_b.tv_sec) * 1000.0;
		elapsedTime += (fgp_e.tv_usec - fgp_b.tv_usec) / 1000.0;

		int totalMux = 0;
		for(iMapM = stat_muxAgg[i].begin(); iMapM != stat_muxAgg[i].end(); iMapM++){

			printf("\t%d-1 MUX:\n", iMapM->first);
			for(iMap = iMapM->second.begin(); iMap != iMapM->second.end(); iMap++){
				printf("\t\t%4d-Bit Mux %7d\n", iMap->first, iMap->second);
				totalMux+=iMap->second;
			}
		}



		printf("\nRegisters\n");
		int totalReg = 0;
		gettimeofday(&fgp_b, NULL);//------------------------------------------
		std::vector<unsigned long> fingerprintReg;
		fingerprintReg.resize(4);
		FINGERPRINT::getRegFingerprint_naive(stat_reg[i], fingerprintReg);
		fpRegs.push_back(fingerprintReg);

		for(unsigned int k = 0; k < fingerprintReg.size(); k++)
			fingerprint.push_back(fingerprintReg[k]);
		fingerprintList.push_back(fingerprint);

		gettimeofday(&fgp_e, NULL);//------------------------------------------
		elapsedTime += (fgp_e.tv_sec - fgp_b.tv_sec) * 1000.0;
		elapsedTime += (fgp_e.tv_usec - fgp_b.tv_usec) / 1000.0;

		for(iMap = stat_reg[i].begin(); iMap != stat_reg[i].end(); iMap++)
		{
			printf("\t%d-bit Reg %7d\n", iMap->first, iMap->second);
			totalReg+=iMap->second;
		}
		
		printf("\nDecoders\n");
		std::map<int, int>::iterator iMap;
		for(iMap = stat_decAgg[i].begin(); iMap != stat_decAgg[i].end(); iMap++){
			printf("\t%d-Bit decoders...\t\t%d\n", iMap->first, iMap->second);
		}





		stat_numMux.push_back(totalMux);
		stat_numReg.push_back(totalReg);
		stat_fingerprintTime.push_back(elapsedTime);

		printf("\nMux Fingerprint\n");
		//for(unsigned int i = fingerprintMux.size() - 1; i >= 0; i--){
		for(unsigned int i =  0; i < fingerprintMux.size(); i++){
			printf("%lx ", fingerprintMux[i]);
		}
		printf("\n");

		printf("Register fingerprint\n");
		for(unsigned int i =  0; i < fingerprintReg.size(); i++){
			printf("%lx ", fingerprintReg[i]);
		}
		printf("\n");
	}
	printf("\n\n");









	//**************************************************************************
	//* MKR-  Print additional statistics  
	//**************************************************************************
	printf("%-20s", "Circuit");
	printf("%5s", "In");
	//printf("%5s", "Out");
	printf("%8s", "Func");
	//printf("%8s", "Wiener");
	printf("%8s", "|AIG|");
	printf("%8s", "|FF|");
	printf("%8s", "|MUX|");
	printf("%8s", "|REG|");
	printf("%8s", "|FAC|");
	printf("%8s", "|FAS|");
	printf("%8s", "|HA|");
	printf("%8s", "|DSP|");
	printf("%8s", "|FFL|");
	printf("%8s", "|MCY|");
	printf("%8s", "|XCY|");
	printf("%8s", "|LUTs|");
	printf("\n");
	printf("--------------------------------------------------------------------------------\n");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
		printf("%5d", stat_numInput[i]);
		//printf("%5d", stat_numOutput[i]);
		printf("%8d", count[i]);
		//printf("%8d", stat_wienerIndex[i]);
		printf("%8d", stat_aigSize[i]);
		printf("%8d", stat_ffSize[i]);
		printf("%8d", stat_numMux[i]);
		printf("%8d", stat_numReg[i]);
		printf("%8d", stat_addAgg[i][0]);
		printf("%8d", stat_addAgg[i][1]);
		printf("%8d", stat_addAgg[i][2]);
		printf("%8d", stat_dspSize[i]);
		printf("%8d", stat_numFFFeedback[i]);
		printf("%8d", stat_numMuxcy[i]);
		printf("%8d", stat_numXorcy[i]);
		printf("%8d", stat_numLUTs[i]);
		printf("\n");
	}
	printf("================================================================================\n");
	printf("\n\n\n");


	
	
	
	




	//**************************************************************************
	//* MKR- Print similarity results 
	//**************************************************************************
	printf("REGISTER SIMILARITY MATRIX\n");
	printf("%-10s", "Circuits");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());

		for(unsigned int k = 0; k < name.size(); k++){
			double sim = FINGERPRINT::tanimoto(fpRegs[i], fpRegs[k]);
			printf("%10.3f", sim*100);
		}
		printf("\n");
	}
	
	printf("\nMULTIPLEXOR SIMILARITY MATRIX\n");
	printf("%-10s", "Circuits");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){

		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());

		for(unsigned int k = 0; k < name.size(); k++){
			double sim = FINGERPRINT::tanimoto(fpMuxes[i], fpMuxes[k]);
			printf("%10.3f", sim*100);
		}
		printf("\n");
	}
	
	printf("\nCombined\n");
	printf("%-10s", "Circuits");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-10s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());

		for(unsigned int k = 0; k < name.size(); k++){
			double sim = FINGERPRINT::tanimoto(fingerprintList[i], fingerprintList[k]);
			printf("%10.3f", sim*100);
		}
		printf("\n");
	}



	printf("\n\n%-15s", "Ecdn Distance");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%15s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-15s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());

		std::vector<double> f1;
		f1.reserve(14);
		f1.push_back((double)stat_numInput[i]);
		f1.push_back((double)count[i]);
		f1.push_back((double)stat_aigSize[i]);
		f1.push_back((double)stat_ffSize[i]);
		f1.push_back((double)stat_numMux[i]);
		f1.push_back((double)stat_numReg[i]);
		f1.push_back((double)stat_addAgg[i][0]);
		f1.push_back((double)stat_addAgg[i][1]);
		f1.push_back((double)stat_addAgg[i][2]);
		f1.push_back((double)stat_dspSize[i]);
		f1.push_back((double)stat_numFFFeedback[i]);
		/*
		f1.push_back(stat_numMuxcy[i]);
		f1.push_back(stat_numXorcy[i]);
		f1.push_back(stat_numLUTs[i]);
		*/

		for(unsigned int k = 0; k < name.size(); k++){
			std::vector<double> f2;
			f2.reserve(14);
			f2.push_back((double)stat_numInput[k]);
			f2.push_back((double)count[k]);
			f2.push_back((double)stat_aigSize[k]);
			f2.push_back((double)stat_ffSize[k]);
			f2.push_back((double)stat_numMux[k]);
			f2.push_back((double)stat_numReg[k]);
			f2.push_back((double)stat_addAgg[k][0]);
			f2.push_back((double)stat_addAgg[k][1]);
			f2.push_back((double)stat_addAgg[k][2]);
			f2.push_back((double)stat_dspSize[k]);
			f2.push_back((double)stat_numFFFeedback[k]);
			/*
			f2.push_back(stat_numMuxcy[k]);
			f2.push_back(stat_numXorcy[k]);
			f2.push_back(stat_numLUTs[k]);
			*/

			double sim = FINGERPRINT::euclideanDistance(f1, f2);
			printf("%15.3f", sim);
		}
		printf("\n");

	}

	printf("\n\n%-15s", "ENORM Distance");
	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%15s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
	}
	printf("\n");

	for(unsigned int i = 0; i < name.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-15s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());

		std::vector<double> f1;
		f1.reserve(14);
		f1.push_back((double)stat_numInput[i]);
		f1.push_back((double)count[i]);
		f1.push_back((double)stat_aigSize[i]);
		f1.push_back((double)stat_ffSize[i]);
		f1.push_back((double)stat_numMux[i]);
		f1.push_back((double)stat_numReg[i]);
		f1.push_back((double)stat_addAgg[i][0]);
		f1.push_back((double)stat_addAgg[i][1]);
		f1.push_back((double)stat_addAgg[k][2]);
		f1.push_back((double)stat_dspSize[i]);
		f1.push_back((double)stat_numFFFeedback[i]);
		/*
		f1.push_back(stat_numMuxcy[i]);
		f1.push_back(stat_numXorcy[i]);
		f1.push_back(stat_numLUTs[i]);
		*/

		for(unsigned int k = 0; k < name.size(); k++){
			std::vector<double> f2;
			f2.reserve(14);
			f2.push_back((double)stat_numInput[k]);
			f2.push_back((double)count[k]);
			f2.push_back((double)stat_aigSize[k]);
			f2.push_back((double)stat_ffSize[k]);
			f2.push_back((double)stat_numMux[k]);
			f2.push_back((double)stat_numReg[k]);
			f2.push_back((double)stat_addAgg[k][0]);
			f2.push_back((double)stat_addAgg[k][1]);
			f2.push_back((double)stat_addAgg[k][2]);
			f2.push_back((double)stat_dspSize[k]);
			f2.push_back((double)stat_numFFFeedback[k]);
			/*
			f2.push_back(stat_numMuxcy[k]);
			f2.push_back(stat_numXorcy[k]);
			f2.push_back(stat_numLUTs[k]);
			*/

			double sim = FINGERPRINT::euclideanDistanceWNorm(f1, f2);
			printf("%15.3f", sim);
		}
		printf("\n");

	}
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
	printf("%-12s", "Fngrprint");
	printf("\n") ;

	std::vector<float> totalTime;
	if(stat_time.size() != 0)
		totalTime.resize(stat_time[0].size()+1, 0);

	for(unsigned int i = 0; i < stat_time.size(); i++){
		int lastSlashIndex = name[i].find_last_of("/") + 1;
		printf("%-20s", name[i].substr(lastSlashIndex, name[i].length()-lastSlashIndex-2).c_str());
		for(unsigned int k = 0; k < stat_time[i].size(); k++){
			printf("%-12.3f", stat_time[i][k]);
			totalTime[k]+=stat_time[i][k];
		}

		printf("%-12.3f", stat_fingerprintTime[i]);
		totalTime[stat_time[i].size()]+=stat_fingerprintTime[i];
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







#endif
