/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
	@
	@  aigex.cpp
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

#include "aig.hpp"


#ifndef GE_GUARD
#define GE_GUARD

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



bool DFS(AIG* aig, unsigned start, unsigned end,  std::list<unsigned>& path, std::set<unsigned>& marked){
		start = start & 0xFFFFFFFE;
		end = end & 0xFFFFFFFE;
		marked.insert(start);
		path.push_back(start);
		//printf("Traversing start node: %d\n", start);

		if(start == end){
			printf("DFS -- Start is at the END node\n");
			return true;; //none lower than this
		}
		if(start <= aig->getInputSize()*2){
			//printf("DFS -- Start is INPUT\n");
		return false;

		}

		unsigned node1, node2, child1, child2;
		child1 = aig->getChild1(start);
		child2 = aig->getChild2(start);
		

		node1 = child1 & 0xFFFFFFFE;
		node2 = child2 & 0xFFFFFFFE;


		//printf("CHILD: %d %d\n", node1, node2);

		if(marked.find(node1) == marked.end()){
		 bool result = DFS(aig, node1, end, path, marked);
		 if (result) return true;

		}

		if(marked.find(node2) == marked.end()){
			bool result = DFS(aig, node2, end, path, marked);
		 	if (result) return true;
		}

		path.pop_back();
		
		return false;
	}

int main( int argc, char *argv[] )
{

		std::cout<<"\n============================================================================\n";
		std::cout<<  "[*]\tBegin AIG Explorer (AIGEX) command line tool \n";
		std::cout<<"============================================================================\n";


	if(argc != 2)
	{
		std::cout<<"\n################################################################\n"<<std::endl;
		printf("    INVALID ARGUMENTS: ./aigex <AIGER FILE> \n");
		std::cout<<"\n################################################################\n"<<std::endl;
		return 0;
	}

	printf(" * Importing AIGER FILE: %s\n", argv[1]);
	AIG* aig = new AIG();
	aig->importAIG(argv[1]);
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
			bool result = DFS(aig, source, contain, path, marked);
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
			bool result = DFS(aig, source, 0, path, marked);
			printf("FANIN NODES: ");
			std::set<unsigned>::iterator iSet;
			for(iSet = marked.begin(); iSet != marked.end(); iSet++)
				printf("%d ", *iSet);
			printf("\n");
		}

		else if(command == "print"){
			aig->print();
		}
		else if(command == "child"){
			unsigned source;
			std::cin >> source;

			unsigned node1, node2, child1, child2;
			if(source <= aig->getInputSize()*2)
				printf("INPUT\n");
			else{
				child1 = aig->getChild1(source);
				child2 = aig->getChild2(source);


				//node1 = child1 & 0xFFFFFFFE;
				//node2 = child2 & 0xFFFFFFFE;
				//printf("CHILD1: %3d\tCHILD2: %3d\n", node1, node2);
				printf("CHILD1: %3d\tCHILD2: %3d\n", child1, child2);
			}
		}
		else if(command == "parent"){
			unsigned source;
			std::cin >> source;
			std::list<unsigned> parentNodes;
			std::list<unsigned>::iterator iList;
			aig->getParents(source,parentNodes);
			printf("PARENT NODES OF %d: ", source);
			for(unsigned int i = 0; i < parentNodes.size(); i++)
			for(iList = parentNodes.begin(); iList != parentNodes.end(); iList++)
				printf("%d ", *iList);
			printf("\n");
		}
		else if(command == "sib"){
			unsigned source;
			std::cin >> source;
			std::vector<unsigned> sibNodes;
			aig->getSiblings(source,sibNodes);
			printf("SIBLING NODES OF %d: ", source);
			for(unsigned int i = 0; i < sibNodes.size(); i++)
				printf("%d ", sibNodes[i]);
			printf("\n");


		}



		printf("\n >> ");
		std::cin>>command;
		printf("\n");
	}

	delete aig;

	printf("-----------------------------------\n");	
	printf("   aigex Succesfully exited\n");	
	printf("-----------------------------------\n");	


	return 0;
}





#endif
