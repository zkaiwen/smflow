/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
  @
  @      aig.hpp
  @      
  @      @AUTHOR:Kevin Zeng
  @      Copyright 2012 – 2013 
  @      Virginia Polytechnic Institute and State University
  @
  @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef AIG_GUARD
#define AIG_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include "graph.hpp"

extern "C"{
#include "aiger.h"
}

//Use to compare keys for "HASH TABLE"
/*
   struct cmp
   {
   bool operator()(int const *a, int const *b)
   {
//printf("A: %d B:%d\nA:%d B:%d\n", a[0], b[0], a[1], b[1]);
if(b[0] < a[0] )
return true;

if(b[1] < a[1])
return true;

return false;
}
};
 */



class AIG{

	private:
		std::vector<unsigned> m_Outputs;
		std::vector<unsigned> m_FFInput;
		std::vector<unsigned> m_OutInput;
		std::vector<std::string> m_OutInputName;
		std::map<std::vector<unsigned >, unsigned > m_HashTable;
		aiger* m_Aiger;

		//Graph Node ID, AIG node ID
		std::map<int, unsigned> m_GateMap;
		unsigned m_Last;
		bool m_hasLUT;



	public:

		//Static Member Variables
		static std::string s_SourcePrim;

		//Constructors
		AIG();
		~AIG();
		void clearAIG();


		//Getters
		void getFFInput(std::vector<unsigned>&);
		void getOutInput(std::vector<unsigned>&);
		void getAIG(std::vector<std::vector<int>*>&);
		void getOutputs(std::vector<unsigned>&);
		unsigned int getOutputSize();
		unsigned int getInputSize();
		unsigned int getSize();
		unsigned getChild1(unsigned);
		unsigned getChild2(unsigned);
		unsigned getGNode(unsigned); //Gets AIG node given a Graph Node
		unsigned getAIGNode(unsigned); //Gets Graph node given a AIG Node
		//find all the nodes with the given inputs
		std::vector<unsigned> findNode(unsigned, unsigned );
		//See if there is another node with * as a child
		bool hasChild(unsigned, unsigned);


		//Setters
		void setPrimSource(std::string);


		//File
		void importAIG(std::string);
		void exportAIG(std::string);
		void writeAiger(std::string,bool);


		//Algorithm Methods given Graph
		void convertGraph2AIG(Graph*, bool sub);
		bool handleFF(int, Graph*);
		unsigned create_and2(unsigned, unsigned);
		unsigned create_input();
		void DFS(Vertex*, std::set<int>&);


		//Print	
		void print();
		void printOutputs();
		void printHash();
		void printMap();


		//List of outputs, input set
		void printSubgraph(std::list<unsigned>&, std::set<unsigned>&);

};



#endif
