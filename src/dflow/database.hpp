/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
	@
	@      database.hpp
	@      
	@      @AUTHOR:Kevin Zeng
	@      Copyright 2012 – 2013 
	@      Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/


#ifndef DATABASE_GUARD
#define DATABASE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>

#include "similarity.hpp"

#include "../rapidxml/rapidxml.hpp"
#include "../rapidxml/rapidxml_print.hpp"

/*
enum fp_attr_t{
	eMUX21,
	eMUX31,
	eMUX41,
	eADDER,
	eADDERA,
	eCARRY,
	eCARRYA,
	ePARITY,
	eGATE,
	eEQUAL,
	eBLK0,
	eBLK1,
	eBLK2,
	eCAS1,
	eCAS2,
	eCAS3,
	eCNT,
	eOUTIN,
	eFFIN
}
*/


//Used to sort the id and name by the score
struct cScore{
	double score;
	unsigned id;
	std::string cName;
};


//Used to compare the cScore so that it is sorted by the score
struct setCompare{
	bool operator()(const cScore& lhs, const cScore& rhs) const{
		return lhs.score >= rhs.score;
	}
};



class CircuitFingerprint{
	public:
		std::string name;
		unsigned id;

		std::vector<std::map<unsigned, unsigned> >  fingerprint;
		CircuitFingerprint(){
			name = "";		
			fingerprint.reserve(18);
		};

		void print(){
			std::map<unsigned, unsigned>::iterator iMap;
			printf("Circuit: %s\n", name.c_str());
			for(unsigned int i = 0; i < fingerprint.size(); i++){
				printf("FP: \n");
				for(iMap = fingerprint[i].begin(); iMap != fingerprint[i].end(); iMap++)
					printf("\t%4d-bit\t%4d\n", iMap->first, iMap->second);
			}
		};
};




class Database{
	private:
		static const unsigned s_NumFeatures = 18;
		std::set<unsigned> m_IndexSkip;
		rapidxml::xml_document<> m_XML;
		rapidxml::xml_node<>* m_Root;
		std::list<CircuitFingerprint*> m_Datastore;

		//TODO:IF MODIFIED, MODIFY THE NUMBER ABOVE

	public:
		Database();
		~Database();

		void initializeDatabase();

		void addCircuitEntry(std::string, std::vector<std::map<unsigned, unsigned> >&, std::list<std::string>&);
		CircuitFingerprint* extractFingerprint(std::string&);
		
		void compareFingerprint(CircuitFingerprint*, std::set<cScore, setCompare>&);
		std::string formResultXML(std::set<cScore, setCompare>&); 

		bool importDatabase(std::string);
		void exportDatabase(std::string);

		void printXML();
		void printDatabase();

		bool verify_CircuitFingerprint();
		int string2int(const char*);
};


#endif
