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
#include <cstring>
#include <iostream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

class Database{
	private:
		rapidxml::xml_document<> m_XML;
		rapidxml::xml_node<>* m_Root;


	public:
		Database();

		void initializeDatabase();

		void addCircuitEntry(std::string, std::list<std::map<unsigned, unsigned> >&, std::list<std::string>&);

		void importDatabase(std::string);
		void exportDatabase(std::string);

};


#endif
