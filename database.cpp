/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
	@
	@      database.cpp
	@      
	@      @AUTHOR:Kevin Zeng
	@      Copyright 2012 – 2013 
	@      Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "database.hpp"

using namespace rapidxml;

Database::Database(){

}

void Database::initializeDatabase(){
	printf("[DATABASE] -- Initializing Database\n");

	//XML Declaration
	xml_node<>* decl = m_XML.allocate_node(node_declaration);
	decl->append_attribute(m_XML.allocate_attribute("version", "1.0"));
	decl->append_attribute(m_XML.allocate_attribute("encoding", "utf-8"));
	m_XML.append_node(decl);
	
	//XML ROOT NODE
	xml_node<> * dbNode = m_XML.allocate_node(node_element, "Database");	
	m_XML.append_node(dbNode);
	m_Root = dbNode;


}


void Database::addCircuitEntry(std::string name, std::list<std::map<unsigned, unsigned> >& fp, std::list<std::string>& features){
	printf("[DATABASE] -- Adding circuit entry into database\n");
	assert(features.size() == fp.size());

	xml_node<>* cktNode = m_XML.allocate_node(node_element, "Circuit");
	cktNode->append_attribute(m_XML.allocate_attribute("name",m_XML.allocate_string(name.c_str()))) ;

	std::list<std::map<unsigned, unsigned> >::iterator iList;
	std::list<std::string>::iterator iFeat;
	std::map<unsigned, unsigned>::iterator iMap;

	iFeat = features.begin();
	for(iList = fp.begin(); iList != fp.end(); iList++){
		xml_node<>* featureNode = m_XML.allocate_node(node_element, iFeat->c_str());

		for(iMap = iList->begin(); iMap != iList->end(); iMap++){
			xml_node<>* compNode= m_XML.allocate_node(node_element,"ATTR"); 
			std::stringstream ss;
			ss<<iMap->first;
			std::string value = ss.str();
			printf("SIZE: S:|%s| I:%d\t\t", value.c_str(), iMap->first);
			compNode->append_attribute(m_XML.allocate_attribute("size", m_XML.allocate_string(value.c_str())));

			ss.str("");
			ss<<iMap->second;
			value = ss.str();
			printf("CNT: S:|%s| I:%d\n", value.c_str(), iMap->second);
			compNode->append_attribute(m_XML.allocate_attribute("count", m_XML.allocate_string(value.c_str())));

			featureNode->append_node(compNode);
		}
		cktNode->append_node(featureNode);

		iFeat++;
	}
	
	m_Root->append_node(cktNode);

}

void Database::exportDatabase(std::string path){
	printf("[DATABASE] -- Exporting database into XML file\n");
	printf(" * Path: %s\n", path.c_str());

	std::ofstream ofs;
	ofs.open(path.c_str());

	ofs << m_XML;
	ofs.close();
	std::cout << m_XML << "\n";

	exit(1);
}

void Database::importDatabase(std::string path){
	printf("[DATABASE] -- Importing database from XML file\n");
	m_XML.clear();
	std::ifstream xmlfile;
	xmlfile.open(path.c_str());
	if (!xmlfile.is_open())	{
		fprintf(stderr, "[ERROR] -- Cannot open the xml file for import...exiting\n");
		fprintf(stderr, "\n***************************************************\n\n");
		exit(-1);
	}

	std::string xmlstring = "";
	std::string xmlline;
	while(getline(xmlfile, xmlline))
		xmlstring += xmlline + "\n";

	xml_document<> xmldoc;
	char* cstr = new char[xmlstring.size() + 1];
	strcpy(cstr, xmlstring.c_str());

	m_XML.parse<0>(cstr);
}
