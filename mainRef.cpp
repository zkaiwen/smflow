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
		printf("./xfpgeniusRef <database file>\n\n\n");
		return 0;
	}





	//**************************************************************************
	//*  Declarations
	//**************************************************************************
	std::string database = argv[1];            //Location of database file
	std::string option= "";                    //Command line option

	//Time tracking
	timeval aig_b, aig_e;
	timeval ce_b, ce_e;
	timeval lib_b, lib_e;
	timeval func_b, func_e;
	timeval agg_b, agg_e;
	timeval cnt_b, cnt_e;

	float elapsedTime;
	int k = 6;                              //k-Cut enumeration value 


}




#endif
