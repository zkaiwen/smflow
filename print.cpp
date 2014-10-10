/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
	@
	@      print.cpp 
	@	
	@      @AUTHOR:Kevin Zeng
	@      Copyright 2012 – 2013 
	@      Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#include "print.hpp"

/***************************************************************************
 *  printV
 *    Prints contents of a vector pointer of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    Vector pointer of ints
 ****************************************************************************/
void printV(std::string name, std::vector<int>* v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v->size(); i++){
		printf("%3d ", v->at(i));
	}
	printf("\n");

}



/***************************************************************************
 *  printV
 *    Prints contents of a vector of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    Vector of ints
 ****************************************************************************/
void printV(std::string name, std::vector<int> & v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v.size(); i++){
		printf("%3d ", v[i]);
	}
	printf("\n");

}



/***************************************************************************
 *  printV
 *    Prints contents of a 2D vector of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    2D Vector of ints
 ****************************************************************************/
void print2DV(std::string name, std::vector<std::vector<int> > & v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v.size(); i++){
		for(unsigned int j = 0; j < v[i].size(); j++){
			printf("%3d ", v[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}



/***************************************************************************
 *  print2DV
 *    Prints contents of a 2D vector pointer of ints
 *
 *  @PARAMS:
 *    name: Name of list to print out
 *    v:    2D vector pointer of ints
 ****************************************************************************/
void print2DV(std::string name, std::vector<std::vector<int>* > & v){
	printf("%s\n", name.c_str());
	for(unsigned int i = 0; i < v.size(); i++){
		for(unsigned int j = 0; j < v[i]->size(); j++){
			printf("%3d ", v[i]->at(j));
		}
		printf("\n");
	}
	printf("\n");
}


/*#############################################################################
 *
 * printIO 
 *  prints the input output mapping for a list of specific functions 
 *
 *#############################################################################*/
void printIO(std::map<unsigned long long, std::vector<InOut*> >& pmap, 
		std::vector<unsigned long long>& inOut){

	std::map<unsigned long long, std::vector<InOut*> >::iterator iPMAP;
	std::set<unsigned>::iterator iSet;
	int count= 0;
	for(unsigned int i = 0; i < inOut.size(); i++){
		iPMAP = pmap.find(inOut[i]);
		if(iPMAP != pmap.end()){
			for(unsigned int j = 0; j < iPMAP->second.size(); j++){
				printf("INPUT: ");
				for(iSet = iPMAP->second[j]->input.begin(); iSet != iPMAP->second[j]->input.end(); iSet++)
					printf("%d ", *iSet);

				printf("\t\tOUTPUT:\t%d\n", iPMAP->second[j]->output);

				count++;
			}
		}
	}
	printf("COUNT: %d ************************************************************** %d\n", count, count);
}



/*#############################################################################
 *
 * printAddList 
 * 	 Prints out the aggregated adder list 
 *
 *#############################################################################*/
void printAddList(std::list<std::set<unsigned> >& addIn,
		std::list<std::set<unsigned> >& addOut){

	std::list<std::set<unsigned> >::iterator iList1;
	std::list<std::set<unsigned> >::iterator iList2;
	iList2 = addOut.begin();
	//printf("\nPRINT ADD LIST\n------------------------------------------------------------------------\n\n");
	for(iList1 = addIn.begin(); iList1 != addIn.end(); iList1++){
		std::set<unsigned>::iterator iSet;
		printf("INPUT: ");
		for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
			printf("%d ", *iSet);


		printf("\nOUTPUT %d: ",(int) iList2->size());
		for(iSet = iList2->begin(); iSet != iList2->end(); iSet++)
			printf("%d ", *iSet);
		printf("\n\n");
		iList2++;

	}
}

void printListSetU(std::list<std::set<unsigned> >& plset){
	std::list<std::set<unsigned> >::iterator iList1;
	std::set<unsigned>::iterator iSet;
	for(iList1 = plset.begin(); iList1 != plset.end(); iList1++){
		for(iSet = iList1->begin(); iSet != iList1->end(); iSet++)
			printf("%d ", *iSet);
		printf("\n");
	}
}


void printSet(std::set<unsigned>& pset){
	std::set<unsigned>::iterator it;
	for(it = pset.begin(); it != pset.end(); it++){
		printf("%d ", *it);
	}
	printf("\n");

}
