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





Database::~Database(){
		std::list<CircuitFingerprint*>::iterator iList;
		for(iList = m_Datastore.begin(); iList != m_Datastore.end(); iList++)
			delete *iList;
}






void Database::initializeDatabase(){
	printf("[DATABASE] -- Initializing Database\n");

	//XML Declaration
	xml_node<>* decl = m_XML.allocate_node(node_declaration);
	decl->append_attribute(m_XML.allocate_attribute("version", "1.0"));
	decl->append_attribute(m_XML.allocate_attribute("encoding", "utf-8"));
	m_XML.append_node(decl);
	
	//XML ROOT NODE
	xml_node<> * dbNode = m_XML.allocate_node(node_element, "DATABASE");	
	m_XML.append_node(dbNode);
	m_Root = dbNode;
}







void Database::addCircuitEntry(std::string name, std::vector<std::map<unsigned, unsigned> >& fp, std::list<std::string>& features){
	printf("[DATABASE] -- Adding circuit entry into database\n");
	assert(features.size() == fp.size());

	CircuitFingerprint* cktfp = new CircuitFingerprint();
	cktfp->name = name;

	std::stringstream ss;
	xml_node<>* cktNode = m_XML.allocate_node(node_element, "CIRCUIT");
	cktNode->append_attribute(m_XML.allocate_attribute("name",m_XML.allocate_string(name.c_str()))) ;
	ss<<m_Datastore.size();
	cktNode->append_attribute(m_XML.allocate_attribute("id",m_XML.allocate_string(ss.str().c_str()))) ;

	std::list<std::string>::iterator iFeat;
	std::map<unsigned, unsigned>::iterator iMap;

	iFeat = features.begin();
	for(unsigned int i = 0; i < fp.size(); i++){
		xml_node<>* featureNode = m_XML.allocate_node(node_element, m_XML.allocate_string(iFeat->c_str()));

		for(iMap = fp[i].begin(); iMap != fp[i].end(); iMap++){
			xml_node<>* compNode= m_XML.allocate_node(node_element,"ATTR"); 
			ss.str("");
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

	
	cktfp->fingerprint = fp;
	cktfp->id = m_Datastore.size();
	m_Datastore.push_back(cktfp);

	m_Root->append_node(cktNode);
}





void Database::exportDatabase(std::string path){
	printf("[DATABASE] -- Exporting database into XML file\n");
	printf(" * Path: %s\n", path.c_str());

	std::ofstream ofs;
	ofs.open(path.c_str());

	ofs << m_XML;
	ofs.close();
}






bool Database::importDatabase(std::string path){
	printf("[DATABASE] -- Importing database from XML file\n");
	printf(" * FILE: %s\n", path.c_str());
	m_XML.clear();

	//Open XML File for parsing
	std::ifstream xmlfile;
	xmlfile.open(path.c_str());
	if (!xmlfile.is_open())	{
		fprintf(stderr, "[ERROR] -- Cannot open the xml file for import...exiting\n");
		fprintf(stderr, "\n***************************************************\n\n");
		exit(-1);
	}


	//Read in contents in the XML File
	std::string xmlstring = "";
	std::string xmlline;
	while(getline(xmlfile, xmlline))
		xmlstring += xmlline + "\n";

	xml_document<> xmldoc;
	char* cstr = new char[xmlstring.size() + 1];
	strcpy(cstr, xmlstring.c_str());


	//Parse the XML Data
	m_XML.parse<0>(cstr);
	//delete [] cstr;
	printXML();
	printf("[DATABASE] -- XML File imported. Parsing...\n");

	enum Error {
		eDatabaseNotEmpty,
		eNodeNull,
		eNoAttr, 
		eDATABASE_FE,
		eCIRCUIT_FE,
		eCNAME_FE, 
		eSC_FE,
		eFeature_FE,
		eCIRCUIT_ATTR_FE,
		eNoAttrSC
	};

	try{
		if(m_Datastore.size() != 0) throw eDatabaseNotEmpty;
		xml_node<>* rootNode= m_XML.first_node();
		if(rootNode == NULL) throw eNodeNull;

		//Make sure first node is DATABASE
		std::string rootNodeName = rootNode->name();
		if(rootNodeName != "DATABASE") throw eDATABASE_FE;
		xml_node<>* cktNode= rootNode->first_node();

		//Look through the circuits in the Database
		while (cktNode!= NULL){
			std::string cktNodeName = cktNode->name();
			if(cktNodeName!= "CIRCUIT") throw eCIRCUIT_FE;

			std::string cktName = "===";  
			int id = -2;

			//Get the name and ID of the circuit 
			xml_attribute<>* cktAttr = cktNode->first_attribute();
			if(cktAttr == NULL) throw eNoAttr;

			std::string cktAttrName = cktAttr->name();
			if(cktAttrName == "name") cktName = cktAttr->value(); 
			else if(cktAttrName == "id") id = string2int(cktAttr->value()); 
			else throw eCIRCUIT_ATTR_FE ;

			cktAttr = cktAttr->next_attribute();
			if(cktAttr == NULL) throw eNoAttr;
			
			cktAttrName = cktAttr->name();
			if(cktAttrName == "name") cktName = cktAttr->value(); 
			else if(cktAttrName == "id") id = string2int(cktAttr->value()); 
			else throw eCIRCUIT_ATTR_FE ;

			if(id < 0 || cktName == "===") throw eCIRCUIT_ATTR_FE;
			CircuitFingerprint* cktfp = new CircuitFingerprint();
			cktfp->id = id;
			cktfp->name = cktName;

			//Look through the fingerprint of each circuit
			xml_node<>* fpNode= cktNode->first_node();
			while (fpNode!= NULL){
				xml_node<>* attrNode =  fpNode->first_node();
				std::map<unsigned, unsigned> fingerprint;

				//Store the attribute of each fingerprint 
				while (attrNode!= NULL){
					int size = -2;
					int count = -2;

					xml_attribute<>* attrAttr = attrNode->first_attribute();
					if(attrAttr == NULL) throw eNoAttr;
					
					std::string attrAttrName = attrAttr->name();
					if(attrAttrName == "size") size = string2int(attrAttr->value());
					else if(attrAttrName == "count") count= string2int(attrAttr->value());
					else throw eFeature_FE;

					attrAttr = attrAttr->next_attribute();
					if(attrAttr == NULL) throw eNoAttr;

					attrAttrName = attrAttr->name();
					if(attrAttrName == "size") size = string2int(attrAttr->value());
					else if(attrAttrName == "count") count = string2int(attrAttr->value());
					else throw eFeature_FE;

					if(size == -2 || count == -2) throw eSC_FE;

					//Store the attribute into the fingerprint;
					fingerprint[size] = count;
					attrNode = attrNode->next_sibling();
				}
				
				//Store the fingerprint into the fingerprintlist	
				cktfp->fingerprint.push_back(fingerprint);
				fpNode= fpNode->next_sibling(); 
			}

			//Store the fingerprintlist of the circuit 
			m_Datastore.push_back(cktfp);
			cktNode = cktNode->next_sibling(); 
		}

	}

	catch (Error error){
		if(error == eNodeNull) printf("[ERROR] -- XML root node is empty\n");
		else if(error == eNodeNull) printf("[ERROR] -- Database is not empty. Aborting import\n");
		else if(error == eNoAttrSC) printf("[ERROR] -- XML node expected a size or count attribute \n");
		else if(error == eNoAttr) printf("[ERROR] -- XML node expected an attribute\n");
		else if(error == eDATABASE_FE) printf("[ERROR] -- XML File has a different format then expected (DATABASE)\n");
		else if(error == eCIRCUIT_FE) printf("[ERROR] -- XML File has a different format then expected (CIRCUIT)\n");
		else if(error == eCIRCUIT_ATTR_FE) printf("[ERROR] -- XML File has a different format then expected (CIRCUIT name or id attribute is missing)\n");
		else if(error == eFeature_FE) printf("[ERROR] -- XML File has a different format then expected (ATTR size or count attribute is missing)\n");
		else if(error == eCNAME_FE) printf("[ERROR] -- XML File has a different format then expected (Size Count has a value that is unknown)\n");
		return false;
	}


	printf("[DATABASE] -- Database import complete!\n");
	return true;
}









int Database::string2int(char* string){
		char *end;
    long  l;
    l = strtol(string, &end, 10);
    if (*string == '\0' || *end != '\0') 
        return -2;

   	return (int) l;
}





CircuitFingerprint* Database::extractFingerprint(std::string& xmlData){
	char* cstr = new char[xmlData.size() + 1];
	strcpy(cstr, xmlData.c_str());

	//Parse the XML Data
	xml_document<> ref;
	ref.parse<0>(cstr);
	
	
	enum Error {
		eNodeNull,
		eUnknownFeature
	};


	
	CircuitFingerprint* cktfp = new CircuitFingerprint();
	for(unsigned int i = 0; i < s_NumFeatures; i++){
		std::map<unsigned, unsigned> fingerprint;
		cktfp->fingerprint.push_back(fingerprint);
	}

	try{
		xml_node<>* cktNode= ref.first_node();
		if(cktNode == NULL) throw eNodeNull;
		std::string cktName = cktNode->name();
		cktfp->name = cktName;

		//Look through the fingerprint of each circuit
		xml_node<>* attrNode =  cktNode->first_node();

		//Store the attribute of each fingerprint 
		while (attrNode!= NULL){
			std::string attrNodeName = attrNode->name();
			
			if(attrNodeName == "Adder8"){
				int value = string2int(attrNode->value());
				if(value != 0) {
					cktfp->fingerprint[3][8] = value;
					cktfp->fingerprint[4][8] = value;
				}
			}
			else if(attrNodeName == "Adder16"){
				int value = string2int(attrNode->value());
				if(value != 0) {
					cktfp->fingerprint[3][16] = value;
					cktfp->fingerprint[4][16] = value;
				}
			}
			else if(attrNodeName == "Adder32"){
				int value = string2int(attrNode->value());
				if(value != 0) {
					cktfp->fingerprint[3][32] = value;
					cktfp->fingerprint[4][32] = value;
				}
			}
			else if(attrNodeName == "Mux2"){
				int value = string2int(attrNode->value());
				if(value != 0) 
					cktfp->fingerprint[0][1] = value;
			}
			else if(attrNodeName == "Mux4"){
				int value = string2int(attrNode->value());
				if(value != 0) 
					cktfp->fingerprint[2][1] = value;
			}
			else if(attrNodeName == "Outputs"){
				xml_node<>* inCountNode=  attrNode->first_node();
				std::map<unsigned, unsigned> outputFP;

				//Store the attribute of each fingerprint 
				while(inCountNode != NULL){
					std::string nName = inCountNode->name();
					if(nName == "InputCount")
						outputFP[string2int(inCountNode->value())]++;	

					inCountNode = inCountNode->next_sibling();
				}

				if(outputFP.size() != 0)
					cktfp->fingerprint[17] = outputFP; 
			}

			else throw eUnknownFeature;

			attrNode = attrNode->next_sibling();
		}
	}
	catch (Error error){

		if(error == eUnknownFeature) printf("[ERROR] -- There is an unknown feature in the XML File\n");
		else if(error == eNodeNull) printf("[ERROR] -- XML root node is empty\n");

		delete cktfp;
		return false;
	}

	return cktfp;

}





void Database::compareFingerprint(CircuitFingerprint* cktfp, std::set<cScore, setCompare>& results){

	std::list<CircuitFingerprint*>::iterator iList;
	std::set<cScore, setCompare>::iterator iSet;

	for(iList = m_Datastore.begin(); iList != m_Datastore.end(); iList++){
		double sum = 0.00;
		for(unsigned int i = 0; i < s_NumFeatures; i++){
			if(cktfp->fingerprint[i].size() == 0 && (*iList)->fingerprint[i].size() == 0)
				sum += 1.00;
			else
				sum += SIMILARITY::tanimotoWindow(cktfp->fingerprint[i], (*iList)->fingerprint[i]);
		}

		cScore cscore;
		cscore.score = sum/s_NumFeatures;
		cscore.cName = (*iList)->name;
		cscore.id = (*iList)->id;
		results.insert(cscore);
	}

		
	for(iSet = results.begin(); iSet != results.end(); iSet++)
		printf("CIRCUIT: %-15s\tID: %3d\tSCORE:%f\n", iSet->cName.c_str(), iSet->id, iSet->score);
	
}









std::string Database::formResultXML(std::set<cScore, setCompare>& result){
	//XML Declaration
	xml_document<> xmldoc;
	xml_node<>* decl = xmldoc.allocate_node(node_declaration);
	decl->append_attribute(xmldoc.allocate_attribute("version", "1.0"));
	decl->append_attribute(xmldoc.allocate_attribute("encoding", "utf-8"));
	xmldoc.append_node(decl);
	
	//XML ROOT NODE
	xml_node<> * matchNode= xmldoc.allocate_node(node_element, "Result");	
	xmldoc.append_node(matchNode);
	
	std::set<cScore, setCompare>::iterator iSet;
	std::stringstream ss;
	for(iSet = result.begin(); iSet != result.end(); iSet++){
		ss<<iSet->id;

		xml_node<>* idNode= xmldoc.allocate_node(node_element, "ID");
		idNode->value(xmldoc.allocate_string(ss.str().c_str()));
		ss.str("");
		matchNode->append_node(idNode);
	}

	ss<<xmldoc;

	return ss.str();
}








bool Database::verify_CircuitFingerprint(){
	std::list<CircuitFingerprint*>::iterator iList;
	std::map<unsigned, unsigned>::iterator iMap;

	std::set<unsigned> uniqueIDSet;

	
	for(iList = m_Datastore.begin(); iList != m_Datastore.end(); iList++){
		//Verify that all circuits have the same bitvector for the fingerprint
		if((*iList)->fingerprint.size() != s_NumFeatures){
			printf("[DATABASE] -- ERROR: Fingerprint Verification failed\n");
			printf(" * Unconformed size\n");
			return false;
		}

		//Verify that all circuits have unique IDs
		if(uniqueIDSet.find((*iList)->id) != uniqueIDSet.end()){
			printf("[DATABASE] -- ERROR: CID: %d has already been assigned\n", (*iList)->id);
			printf(" * IDs are not unique\n");
			return false;
		}
		else
			uniqueIDSet.insert((*iList)->id);
	}
		
	return true;
}





void Database::printXML(){
	std::cout << m_XML << "\n";
}








void Database::printDatabase(){
	std::list<CircuitFingerprint*>::iterator iList;
	std::map<unsigned, unsigned>::iterator iMap;

	for(iList = m_Datastore.begin(); iList != m_Datastore.end(); iList++){
		printf("Circuit: %s\n", (*iList)->name.c_str());
		for(unsigned int i = 0; i < (*iList)->fingerprint.size(); i++){
			printf("FP: \n");
			for(iMap = (*iList)->fingerprint[i].begin(); 
					iMap != (*iList)->fingerprint[i].end(); iMap++)
				printf("\t%4d-bit\t%4d\n", iMap->first, iMap->second);
		}
	}
}






