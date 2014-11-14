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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

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

class CircuitFingerprint{
	public:
	std::string name;

	std::vector<std::map<unsigned, unsigned> >  fingerprint;
	CircuitFingerprint(){
		name = "";		
		fingerprint.reserve(19);
	};

	void print(){
		std::map<unsigned, unsigned>::iterator iMap;

		printf("Circuit: %s\n", name.c_str());
		for(unsigned int i = 0; i < fingerprint.size(); i++){
			printf("FP: \n");
			for(iMap = fingerprint[i].begin(); 
					iMap != fingerprint[i].end(); iMap++)
				printf("\t%4d-bit\t%4d\n", iMap->first, iMap->second);
		}
	};
};





class Database{
	private:
		rapidxml::xml_document<> m_XML;
		rapidxml::xml_node<>* m_Root;
		std::list<CircuitFingerprint*> m_Datastore;
		
		int string2int(char*);

		//TODO:IF MODIFIED, MODIFY THE NUMBER ABOVE
		static const unsigned numFeatures = 19;
		


	public:
		Database();
		~Database();

		void initializeDatabase();

		void addCircuitEntry(std::string, std::list<std::map<unsigned, unsigned> >&, std::list<std::string>&);
		
		CircuitFingerprint* extractFingerprint(std::string&);

		bool importDatabase(std::string);
		void exportDatabase(std::string);
		void printXML();
		void printDatabase();

		bool verify_CircuitFingerprint();


};


#endif
