/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
	@
	@  graphex.cpp
	@    Explores the AIG structure with inline commands
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <set>
#include <list>

#include "graph.hpp"


#ifndef AE_GUARD
#define AE_GUARD

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



bool DFS(Graph* ckt, Vertex* start, Vertex* end,  std::list<unsigned>& path, std::set<unsigned>& marked){
		marked.insert(start->getID());
		path.push_back(start->getID());
		//printf("Traversing start node: %d\n", start);

		if(end!= NULL)
		if(start->getID() == end->getID()){
			//printf("DFS -- Start is at the END node\n");
			return true;; //none lower than this
		}

		std::vector<Vertex*> in;
		start->getInput(in);


		for(unsigned int i = 0; i < in.size(); i++){
			if(marked.find(in[i]->getID()) == marked.end()){
		 		bool result = DFS(ckt, in[i], end, path, marked);
		 		if (result) return true;
			}
		}

		path.pop_back();
		
		return false;
	}

int main( int argc, char *argv[] )
{

		std::cout<<"\n============================================================================\n";
		std::cout<<  "[*]\tBegin Graph Explorer (GRAPHEX) command line tool \n";
		std::cout<<"============================================================================\n";


	if(argc != 2)
	{
		std::cout<<"\n################################################################\n"<<std::endl;
		printf("    INVALID ARGUMENTS: ./graphex <CNL FILE> \n");
		std::cout<<"\n################################################################\n"<<std::endl;
		return 0;
	}

	printf(" * Importing CNL FILE: %s\n", argv[1]);
	Graph* ckt = new Graph("");
	ckt->importGraph(argv[1], 0);
	printf(" * File Imported\n");
	printf("\n\nEnter Commands: \n");
	printf(" >> ");

	std::string command;
	std::cin >> command;
	while(command != "exit" && command != "quit"){
		if(command == "contains"){
			unsigned source, contain;
			std::cin >> source;
			std::cin >> contain;
			printf("Searching to see if %d is reachable from %d\n", contain, source);
			std::set<unsigned> marked;
			std::list<unsigned> path;
			bool result = DFS(ckt, ckt->getVertex(source), ckt->getVertex(contain), path, marked);
			if(result){
				printf(" * Node %d IS CONTAINED under %d\nPATH: ", contain, source);
				std::list<unsigned>::iterator iList;
				for(iList = path.begin(); iList != path.end(); iList++)
					printf("%d ", *iList);
				printf("\n");
			}
			else printf(" * Node %d IS NOT CONTAINED under %d\n", contain, source);
		}
		else if(command == "fanin"){
			unsigned source;
			std::cin >> source;
			std::set<unsigned> marked;
			std::list<unsigned> path;
			bool result = DFS(ckt, ckt->getVertex(source), NULL, path, marked);
			printf("FANIN NODES: ");
			std::set<unsigned>::iterator iSet;
			for(iSet = marked.begin(); iSet != marked.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}

		else if(command == "print"){
			ckt->print();
		}



		printf("\n >> ");
		std::cin>>command;
		printf("\n");
	}
	
	delete ckt;
	
	printf("-----------------------------------\n");	
	printf("   graphex Succesfully exited\n");	
	printf("-----------------------------------\n");	


	return 0;
}





#endif
