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


//System Includes
#include <stdlib.h>
#include <stdio.h>
#include <string>
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
#include "database.hpp"

//XML Includes
#include "rapidxml/rapidxml.hpp"


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

	if(argc < 2){
		printf("./xfpgeniusRef <database xml file>\n\n\n");
		return 0;
	}





	//**************************************************************************
	//*  Declarations
	//**************************************************************************
	std::string databaseFile = argv[1];            //Location of database file
	std::string option= "";                    //Command line option

	//Time tracking
	/*
	timeval aig_b, aig_e;
	timeval ce_b, ce_e;
	timeval lib_b, lib_e;
	timeval func_b, func_e;
	timeval agg_b, agg_e;
	timeval cnt_b, cnt_e;

	float elapsedTime;
	int k = 6;                              //k-Cut enumeration value 
	*/

	//*************************************************************************
	//*  Process Data base file
	//**************************************************************************
	Database* database = new Database();
	if(!database->importDatabase(databaseFile)){
		printStatement("EXITING");
		delete database;
		return 0;
	}
	database->printDatabase();
	if(!database->verify_CircuitFingerprint()){
		printf("[ERROR] -- Fingerprint feature size is not as expected\n");
		delete database;
		return 0;
	}
	
	//**************************************************************************
	//* MKR- CONECTING WITH FRONT ENDJ 
	//**************************************************************************
	

	Server* server = new Server(8000);
	if(! server->waitForClient()) return 0;


	std::string xmlData;
	while(1){
		xmlData = server->receiveAllData() ;
		if(xmlData == "SOCKET_CLOSE"){
			if(! server->waitForClient()) return 0;

			xmlData = server->receiveAllData() ;
			if(xmlData == "SOCKET_CLOSE"){
				printf("[MAIN] -- Client has disconnected twice in a row...exiting...\n");
				break;
			}
				
		}

		/*
			 printf("DATA RECEIVED FROM CLIENT: %s\n", dotData.c_str());
			 std::ofstream dos;                    //Database output file stream
			 dos.open("circuits/demo/dot/ckt.dot");
			 dos<<dotData;

			 server->closeSocket();
		 */

		//std::string refXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Circuit0><Adder8>1</Adder8><Adder16>0</Adder16><Adder32>0</Adder32><Mux2>1</Mux2><Mux4>0</Mux4><Outputs><Name>Out_2</Name><InputCount>2</InputCount><Name>Z_Out_2</Name><InputCount>3</InputCount><Name>Numeric_3</Name><InputCount>3</InputCount><Name>Out</Name><InputCount>3</InputCount><Name>Z_Out</Name><InputCount>2</InputCount></Outputs></Circuit0>";

		CircuitFingerprint* cktfp = database->extractFingerprint(xmlData);
		//cktfp->print();

		std::set<cScore, setCompare> result;
		database->compareFingerprint(cktfp, result);
		std::string xmlResult = database->formResultXML(result);
		if(!server->sendData(xmlResult)){
			printf("[MAIN] -- Writing to client seemed to have encountered an error...\n");
			break;
		}
		printf("RESULT: %s\n", xmlResult.c_str());

		delete cktfp;	
	}





	delete server;
	delete database;
	return 0;
}




#endif
