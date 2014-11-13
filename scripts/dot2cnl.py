#!/usr/bin/python2.7

import pydot;
import sys;
import re;

def findNodeList(subgraph):
	nodeList = subgraph.get_node_list();
	if len(nodeList) != 0:
		return nodeList;

	subgraphList = subgraph.get_subgraph_list();

	for subgraph in subgraphList:
		nodeList = findNodeList(subgraph);
		if len(nodeList) != 0:
			return nodeList;
	
################################################################################
# START OF PYTHON PROGRAM
################################################################################

fileName = sys.argv[1];
print "[DOT2CNL] -- Reading in DOT File: " + fileName;

dotGraph = pydot.graph_from_dot_file(fileName);

#Get the nodes and edges of the graph
print "[DOT2CNL] -- Getting node and edge list"
nodeList = dotGraph.get_node_list();
edgeList = dotGraph.get_edge_list();

#Traverse down the subgraphs and get the node list that is not empty
if len(nodeList) == 0:
	subgraphList = dotGraph.get_subgraph_list();
	for subgraph in subgraphList:
		nodeList = findNodeList(subgraph);


nameTypeMap = {};
nameIDMap= {};
IDNameMap= {};
nameInMap = {};
nameOutMap = {};
nameInPortMap = {};
nameOutPortMap = {};

print "[DOT2CNL] -- Creating nodes"
vID = 0;
for node in nodeList:
	nodeName = re.search('"(.*?)"', node.get_name());
	nodeName = nodeName.group(1);
	

	label = re.search('label="(.*?)"', node.to_string());
	nodeLabel= label.group(1);

	if(nodeLabel.upper().find("IDATAACCESSOR") != -1):
		port= re.search('\'(.*?)\'', nodeLabel);
		if(nodeLabel.upper().find("}}}}") != -1):
			nameTypeMap[nodeName] = "IN";
		else:
			nameTypeMap[nodeName] = "OUT";

	elif(nodeLabel.upper().find("IWIRE") != -1):
		nameTypeMap[nodeName] = "WIRE";
		edgeWireList = dotGraph.get_edge(node);
		continue;

	elif(nodeLabel.upper().find("COMPOUNDARITHMETICNODE") != -1):
		#Get the mode
		nodeType = re.search('Mode: (.*?)\|', nodeLabel);
		nameTypeMap[nodeName] = nodeType.group(1);

	elif(nodeLabel.upper().find("PC:MUX") != -1):
		nameTypeMap[nodeName] = "MUX";

	elif(nodeLabel.upper().find("ISEQUAL") != -1):
		nameTypeMap[nodeName] = "EQUAL";
	else:
		nodeType = re.search('\}\|(.*?)\\\\n#', nodeLabel);
		nameTypeMap[nodeName] = nodeType.group(1);


	nameIDMap[nodeName] = vID;
	IDNameMap[vID] = nodeName;
	nameInMap[nodeName] = [];
	nameOutMap[nodeName] = [];
	nameInPortMap[nodeName] = [];
	nameOutPortMap[nodeName] = [];
	vID = vID + 1;
	#inport= re.search('\{([^}]*)\}', nodeLabel);
	#print "INPORT: " + inport.group(0);
	#outport= re.search('(?<=\|\{).*?\}', nodeLabel);
	#print "OUTPUT: " + outport.group(0);

	

wireSrc = {};
wireDst = {};

print "[DOT2CNL] -- Removing IWIRES"
for edge in edgeList:
	source = edge.get_source().split(":");
	dest= edge.get_destination().split(":");

	if nameTypeMap[source[0]] == "WIRE":
		wireDst[source[0]] = dest[0];
	elif nameTypeMap[dest[0]] == "WIRE":
		wireSrc[dest[0]] = source[0];




print "[DOT2CNL] -- Creating connections"
for edge in edgeList:
	source = edge.get_source().split(":");
	dest= edge.get_destination().split(":");

	#Check to see which side is the wirenode
	if nameTypeMap[source[0]] == "WIRE":
		try:
			nameInMap[dest[0]].index(nameIDMap[wireSrc[source[0]]]);
		except ValueError:
			nameInMap[dest[0]].append(nameIDMap[wireSrc[source[0]]]);
			nameInPortMap[dest[0]].append(dest[1]);

		try:
			nameOutMap[wireSrc[source[0]]].index(nameIDMap[dest[0]]);
		except ValueError:
			nameOutMap[wireSrc[source[0]]].append(nameIDMap[dest[0]]);
			nameOutPortMap[wireSrc[source[0]]].append(source[1]);


	elif nameTypeMap[dest[0]] == "WIRE":
		try:
			nameInMap[wireDst[dest[0]]].index(nameIDMap[source[0]]);
		except ValueError:
			nameInMap[wireDst[dest[0]]].append(nameIDMap[source[0]]);
			nameInPortMap[wireDst[dest[0]]].append(dest[1]);

		try:
			nameOutMap[source[0]].index(nameIDMap[wireDst[dest[0]]]);
		except ValueError:
			nameOutMap[source[0]].append(nameIDMap[wireDst[dest[0]]]);
			nameOutPortMap[source[0]].append(source[1]);





inputString = "";
outputString = "";
numIn = 0;
numOut = 0;
cnl = "";

print "[DOT2CNL] -- Creating CNL content"
for v,k in IDNameMap.iteritems():
	if nameTypeMap[k] == "IN":
		numIn = numIn + 1;
		inputString = inputString + repr(v) + " " + k + " ";
	elif nameTypeMap[k] == "OUT":
		numOut = numOut + 1;
		for item in xrange(len(nameInMap[k])):
			outputString = outputString + repr(nameInMap[k][item]) + " " + k + " ";
		continue;

	cnl = cnl + (repr(v) + " " + nameTypeMap[k].upper() + " " + k + " " + repr(len(nameInMap[k])) );

	for item in xrange(len(nameInMap[k])):
		cnl = cnl + (" " + repr(nameInMap[k][item]) + " " + nameInPortMap[k][item]);

	cnl = cnl + (" " + repr(len(nameOutMap[k])) );
	for item in xrange(len(nameOutMap[k])):
		cnl = cnl + (" " + repr(nameOutMap[k][item]) + " " + nameOutPortMap[k][item]);

	cnl = cnl + "\n";


inputString = repr(numIn) + " " + inputString + "\n";
outputString = repr(numOut) + " " + outputString + "\n";


cnl = repr(len(nameIDMap)-numOut) + "\n" + inputString + outputString + cnl; 


#Write out CNL File
cnlFileName = fileName.split("/");
cnlFileName = re.search('(^.*?)\.',cnlFileName[len(cnlFileName)-1]);
cnlFileName = cnlFileName.group(1);

pathName = re.search('^.*/', fileName);
pathName = pathName.group(0);

cnlFileName = pathName + cnlFileName + ".cnl";
cnlFile = open(cnlFileName, 'w');
cnlFile.write(cnl);

print "[DOT2CNL] -- DONE"


















