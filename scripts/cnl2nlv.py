#/usr/bin/python
import sys;


#Read in circuit file
circuitName = sys.argv[1];
nlv = "";


with open(circuitName, 'r') as fs:
	circuitName = circuitName[circuitName.rfind("/")+1:circuitName.rfind(".")];
	print "Reading circuit:" + circuitName;
	nlv = "module new " + circuitName + "\n";


	#Handle input and output ports first
	numInst = fs.readline().rstrip();
	inputLine = fs.readline().rstrip();
	outputLine = fs.readline().rstrip();
	print numInst;
	print inputLine;
	print outputLine;

	inputLineList= inputLine.split();
	numInput = int(inputLineList.pop(0));

	outputLineList= outputLine.split();
	numOutput = int(outputLineList.pop(0));

	for index in xrange(numInput):
		print(inputLineList[index*2] + " NAME: " + inputLineList[index*2+1]);
		nlv = nlv + "load port " + inputLineList[index*2+1] + " in \n";
	
	for index in xrange(numOutput):
		print(outputLineList[index*2] + " NAME: " + outputLineList[index*2+1]);
		nlv = nlv + "load port " + outputLineList[index*2+1] + " out \n";

	#Append the primitives and symbols into the file
	with open("../circuits/g/primitive/nlvprimitive.nlv", 'r') as nlvPrimitiveFS:
		primitive = nlvPrimitiveFS.read();
		nlv = nlv + primitive + "\n";
		nlvPrimitiveFS.close();
		
	tmpfs = fs;
	for line in fs:
		lineList = line.split();	
		lineList.pop(0);
		if lineList[0] == 'IN':
			continue;
		print "load inst " + lineList[1] + " " + lineList[0];
		nlv = nlv + "load inst " + lineList[1] + " " + lineList[0];

	fs.close();

#print nlv;
