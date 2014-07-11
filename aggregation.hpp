/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  AGGREGATION.cpp
	@		--Aggregates Common components into higher structures
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef AGG_GUARD
#define AGG_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"
#include "graph_b.hpp"


namespace AGGREGATION{
	void findHA(CutFunction* cf, AIG* aigraph);
	void findDecoder(CutFunction* cf, AIG* aigraph);
	unsigned int findMux(CutFunction* cf, AIG* aigraph);
	void findMux2(CutFunction* cf, AIG* aigraph);
	void aggregateSignal(std::map<unsigned long, std::vector<std::vector<unsigned>*> >& pmap, std::vector<unsigned long>& andFunction, std::vector<std::set<unsigned> >& andSimIn,	std::vector<std::set<unsigned> >& andSimOut);
	int findNegInput(AIG* aigraph, unsigned out, std::vector<unsigned>& in);


	void findHA(CutFunction* cf, AIG* aigraph){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR ADDERS-------------------------------");
		const unsigned long xor1 = 0x6666666666666666;
		const	unsigned long xor2 = 0x9999999999999999;

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long, std::vector<std::vector<unsigned>*> > pmap;
		cf->getPortMap(pmap);

		printf(" * Parsing function database for possible Half Adders...");
		std::map<std::vector<unsigned>, std::vector<unsigned> > faOutput;
		if(pmap.find(xor1) != pmap.end()){
			for(unsigned int i = 0; i < pmap[xor1].size(); i++){
				unsigned int j = 0;
				//printf("INPUT: ");
				//for(j = 0; j < pmap[xor1][i]->size()-1; j++){
				//	printf("%d ", pmap[xor1][i]->at(j));
				//}
				//printf("\nOUTPUT:\t%d\n", pmap[xor1][i]->at(j));
				unsigned outnode = pmap[xor1][i]->at(j);

				//Is there an and gate that has the same inputs as the XOR gate
				std::vector<unsigned> andindex;
				andindex = aigraph->findNode(pmap[xor1][i]->at(0), pmap[xor1][i]->at(1));
				for(unsigned int j = 0; j < andindex.size(); j++){
					if(aigraph->hasChild(andindex[j], outnode)){
						//printf("THIS XOR IS A HA- S:%d  CO:%d\n\n", outnode, andindex[j]);
						//0- Sum, 1- CO
						std::vector<unsigned > sumCO;
						sumCO.push_back(outnode);
						sumCO.push_back(andindex[j]);

						//Inputs
						std::vector<unsigned> inputs;
						for(unsigned int k = 0; k < pmap[xor1][i]->size()-1; k++)
							inputs.push_back(pmap[xor1][i]->at(k));

						faOutput[inputs] = sumCO;
					}
				}
			}
		}


		if(pmap.find(xor2) != pmap.end()){
			for(unsigned int i = 0; i < pmap[xor2].size(); i++){
				unsigned int j = 0;
				//printf("INPUT: ");
				//for(j = 0; j < pmap[xor2][i]->size()-1; j++){
				//	printf("%d ", pmap[xor2][i]->at(j));
				//	}

				//printf("\nOUTPUT:\t%d\n", pmap[xor2][i]->at(j));
				unsigned outnode = pmap[xor2][i]->at(j);

				//Is there an and gate that has the same inputs as the XOR gate
				std::vector<unsigned> andindex;
				andindex = aigraph->findNode(pmap[xor2][i]->at(0), pmap[xor2][i]->at(1));
				for(unsigned int j = 0; j < andindex.size(); j++){
					if(aigraph->hasChild(andindex[j], outnode)){
						//printf("THIS XOR IS A HA- S:%d  CO:%d\n\n", outnode, andindex[j]);

						//0- Sum, 1- CO
						std::vector<unsigned> sumCO;
						sumCO.push_back(outnode);
						sumCO.push_back(andindex[j]);

						//Inputs
						std::vector<unsigned> inputs;
						for(unsigned int k = 0; k < pmap[xor2][i]->size()-1; k++)
							inputs.push_back(pmap[xor2][i]->at(k));

						faOutput[inputs] = sumCO;
					}
				}
			}
		}
		printf("done\n");



		std::map<std::vector<unsigned>, std::vector<unsigned> >::iterator outit;
		/*
			 printf("\n\nHALF ADDERS\n");
			 for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
			 printf("INPUTS: ");
			 for(unsigned int i = 0; i < outit->first.size(); i++)
			 printf("%d ", outit->first[i]);
			 printf("\nSUM:%d\tCO:%d\n", outit->second[0], outit->second[1]);
			 }
		 */

		printf("\n----------------------------------------------------------------\n");
		printf("Number of Possible Half Adders\t\t%d\n", (int) faOutput.size());
		printf("----------------------------------------------------------------\n\n");

















		/*



			 SEARCHING GREY BLOCKS



		 */

		printf(" * Parsing function database for Grey Blocks...");
		const	unsigned long grey= 0x8c8c8c8c8c8c8c8c;
		const	unsigned long grey2= 0x8a8a8a8a8a8a8a8a;
		const	unsigned long grey3= 0x0e0e0e0e0e0e0e0e;

		//Function, Vector of every set of inputs with that function. Last item is the output node
		cf->getPortMap(pmap);

		//List of inputs of sum components. Last index is the output
		std::vector<std::vector<unsigned> > gPorts;

		if(pmap.find(grey) != pmap.end()){
			for(unsigned int i = 0; i < pmap[grey].size(); i++){
				unsigned int j = 0;
				//printf("INPUT: ");
				std::vector<unsigned> ports;
				for(j = 0; j < pmap[grey][i]->size()-1; j++){
					//printf("%d ", pmap[grey][i]->at(j));
					ports.push_back(pmap[grey][i]->at(j));
				}
				//printf("\nOUTPUT:\t%d\n", pmap[grey][i]->at(j));
				unsigned outnode = pmap[grey][i]->at(j);
				ports.push_back(outnode);
				gPorts.push_back(ports);
			}
		}

		if(pmap.find(grey2) != pmap.end()){
			for(unsigned int i = 0; i < pmap[grey2].size(); i++){
				unsigned int j = 0;
				//printf("INPUT: ");
				std::vector<unsigned> ports;
				for(j = 0; j < pmap[grey2][i]->size()-1; j++){
					//printf("%d ", pmap[grey2][i]->at(j));
					ports.push_back(pmap[grey2][i]->at(j));
				}
				//printf("\nOUTPUT:\t%d\n", pmap[grey2][i]->at(j));
				unsigned outnode = pmap[grey2][i]->at(j);
				ports.push_back(outnode);
				gPorts.push_back(ports);
			}
		}

		if(pmap.find(grey3) != pmap.end()){
			for(unsigned int i = 0; i < pmap[grey3].size(); i++){
				unsigned int j = 0;
				//printf("INPUT: ");
				std::vector<unsigned> ports;
				for(j = 0; j < pmap[grey3][i]->size()-1; j++){
					//printf("%d ", pmap[grey3][i]->at(j));
					ports.push_back(pmap[grey3][i]->at(j));
				}
				//printf("\nOUTPUT:\t%d\n", pmap[grey3][i]->at(j));
				unsigned outnode = pmap[grey3][i]->at(j);
				ports.push_back(outnode);
				gPorts.push_back(ports);
			}
		}
		printf("done\n");

		/*			
						printf("PRINTING GBLOCKS\n");
						for(unsigned int j = 0; j < gPorts.size(); j++){
						printf("INPUT: ");
						unsigned int k = 0;
						for(k = 0; k < gPorts[j].size()-1; k++){
						printf("%d ", gPorts[j][k]);

						}
						printf("\nOUTPUT:\t%d\n", gPorts[j][k]);
						}
		 */
		printf(" * Aggregating Results...");	
		//Vector of vector of iterators. (Aggregated adders)
		std::map<unsigned long, std::string>::iterator it;
		std::vector<std::set<unsigned> > marked;
		int numHA = 0;
		int numGB = 0;


		//For each ha, see if there is a connecting gb
		for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
			//printf("Checking CO: %d\n", outit->second[1]);

			//Check to see which grey the co goes out to. 
			for(unsigned int z = 0; z < gPorts.size(); z++){
				for(unsigned int i = 0; i < gPorts[z].size()-1; i++){

					//If carry out is the same as input to grey block, mark
					bool foundGB = false;
					//printf("Comparing %d - %d\n", outit->second[1], gPorts[z][i]);
					if(outit->second[1] == gPorts[z][i]){
						for(unsigned int j = 0; j < marked.size(); j++){
							if(marked[j].find(outit->second[1]) != marked[j].end()){
								//printf("Cout found in set %d...inserting cout\n\n", j);
								marked[j].insert(gPorts[z][gPorts[z].size()-1]);
								foundGB = true;
								numGB++;
								break;
							}
							else if(marked[j].find(gPorts[z][gPorts[z].size()-1]) != marked[j].end()){
								//printf("GB not found in set %d...inserting gb output\n\n", j);
								marked[j].insert(outit->second[1]);
								foundGB = true;
								numHA++;
								break;
							}
						}
						if(!foundGB){
							//printf("No prev set found, creating new set\n\n");
							std::set<unsigned> n;
							n.insert(outit->second[1]);
							n.insert(gPorts[z][gPorts[z].size()-1]);
							marked.push_back(n);
							numHA++;
							numGB++;
							break;
						}
					}

				}
			}
		}
		printf("done\n");


		//This is mainly for carry ripple
		printf(" * Aggregating carry ripple architecture...");
		std::set<unsigned>::iterator sit;
		std::set<unsigned>::iterator sit2;
		for(unsigned int z = 0; z < gPorts.size(); z++){
			unsigned gout = gPorts[z][gPorts[z].size()-1];
			for(unsigned int i = 0; i < gPorts.size(); i++){
				if(z == i) continue;

				for(unsigned int j = 0; j < gPorts[i].size()-1; j++){
					//printf("Checking %d to %d\n", gPorts[i][j], gout);
					if(gout == gPorts[i][j])
					{
						//printf("FOUND %d to %d\n", gout, gPorts[i][gPorts[i].size()-1]);
						int index1 = -1;
						int index2 = -1;
						for(unsigned int k = 0; k < marked.size(); k++){
							if(marked[k].find(gout) != marked[k].end())
								index1 = k;
							else if(marked[k].find(gPorts[i][gPorts[i].size()-1]) != marked[k].end())
								index2 = k;
						}

						//printf("index1 %d, index2 %d\n", index1, index2);
						if(index1 != -1 && index2 != -1){
							//printf("ERASING INDEX %d\n", index1);
							for(sit = marked[index1].begin(); sit != marked[index1].end(); sit++){
								marked[index2].insert(*sit);
							}
							marked.erase(marked.begin() + index1);
						}
					}
				}
			}
		}
		printf("done\n");





		printf("\n\n");
		printf("[AGG] -- SEARCHING FOR ADDERS USING halfadder_gp BLOCKS-------------------------------\n");
		std::vector<unsigned long> hagp;
		std::map<unsigned long, std::string> hmap;
		cf->getHashMap(hmap);

		//find halfadder_gp 
		printf(" * Parsing function database for possible halfadder_gp Blocks...");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("halfadder_gp") != std::string::npos)
				hagp.push_back(it->first);
		}

		std::vector<std::vector<unsigned> > hagpInSet;
		for(unsigned int i = 0; i < hagp.size(); i++){
			if(pmap.find(hagp[i]) != pmap.end()){
				for(unsigned int j = 0; j < pmap[hagp[i]].size(); j++){
					unsigned int k = 0;
					std::vector<unsigned> ports;

					//sort positive inputs (used as key)
					for(k = 0; k < pmap[hagp[i]][j]->size()-1; k++){
						unsigned in = pmap[hagp[i]][j]->at(k);
						ports.push_back(in);
					}
					ports.push_back(pmap[hagp[i]][j]->at(k));
					hagpInSet.push_back(ports);
				}
			}
		}
		printf("done\n");


		std::vector<std::vector<unsigned> > addbitIn;
		std::vector<std::vector<unsigned> > addbitOut;

		//For each ha, see if there is a halfadder_gp with same input 
		printf(" * Looking for blocks with similar inputs...");
		for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
			int inputCount = 0; 
			for(unsigned int i = 0; i < hagpInSet.size(); i++){
				//For each input of HAGP, compare INPUT of HA to see if HAGP matches
				for(unsigned int j = 0; j < hagpInSet[i].size()-1; j++){
					//for each input of HA
					for(unsigned int k = 0; k < outit->first.size(); k++){
						//printf("Comparing %d - %d\n", outit->first[k], hagpInSet[i][j]);
						if(outit->first[k] == hagpInSet[i][j]){
							inputCount++;
							break;
						}
					}

					if(inputCount == 2)	break;	
				}

				if(inputCount == 2){
					std::vector<unsigned> input;
					std::vector<unsigned> output;
					input.push_back(hagpInSet[i][0]);
					input.push_back(hagpInSet[i][1]);
					input.push_back(hagpInSet[i][2]);

					output.push_back(outit->second[1]);	
					output.push_back(hagpInSet[i][3]);

					addbitIn.push_back(input);
					addbitOut.push_back(output);

					break;	
				}

			}

			if(inputCount != 2){
				std::vector<unsigned> input;
				std::vector<unsigned> output;
				input.push_back(outit->first[0]);
				input.push_back(outit->first[1]);

				output.push_back(outit->second[1]);
				output.push_back(outit->second[0]);

				addbitIn.push_back(input);
				addbitOut.push_back(output);
			}
		}
		printf("done\n");

		/*
			 printf("Printing ha and hagp set\n");
			 for(unsigned int i = 0; i < addbitIn.size(); i++){
			 printf("Input:\t");
			 for(unsigned int j = 0; j < addbitIn[i].size(); j++){
			 printf("%d ", addbitIn[i][j]);
			 }

			 printf("\nOutput:\t");
			 for(unsigned int j = 0; j < addbitOut[i].size(); j++){
			 printf("%d ", addbitOut[i][j]);
			 }
			 printf("\n\n");

			 }
		 */
		//output index 0
		std::vector<std::set<unsigned> > marked2; 

		printf(" * Aggregating results...\n");
		for(unsigned int i = 0; i < addbitOut.size(); i++){
			for(unsigned int j = 0; j < addbitIn.size(); j++){
				if(i == j) continue;
				else if(addbitIn[j].size() == 2) continue;

				for(unsigned int k = 0; k < addbitIn[j].size(); k++){
					bool found = false;
					//printf("comparing %d %d\n", addbitOut[i][0], addbitIn[j][k]);
					if(addbitOut[i][0] == addbitIn[j][k] ||
							addbitOut[i][1] == addbitIn[j][k]){
						for(unsigned int a = 0; a < marked2.size(); a++){
							if(marked2[a].find(addbitOut[i][0]) != marked2[a].end()){
								//printf("found1\n");
								marked2[a].insert(addbitOut[j][0]);
								found = true;
								break;
							}
							else if(marked2[a].find(addbitOut[j][0]) != marked2[a].end()){
								//printf("found2\n");
								marked2[a].insert(addbitOut[i][0]);
								found = true;
								break;
							}
						}

						if(!found){
							//printf("not found...making new set\n");
							std::set<unsigned> mset;
							mset.insert(addbitOut[j][0]);
							mset.insert(addbitOut[i][0]);

							marked2.push_back(mset);
							break;
						}

					}
				}
			}
		}
		printf("done\n");




		printf("\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("\n----------------------------------------------------------------\n");
		printf("Result Search 1:\n");
		printf("Number of HA: %d, Number of GB: %d\n", numHA, numGB);
		printf("Number of possible adders:\t %d\n\n", (int) marked.size());
		for(unsigned int i = 0; i < marked.size(); i++){
			int nbit = 0; 
			for(outit = faOutput.begin(); outit != faOutput.end(); outit++){
				if(marked[i].find(outit->second[1]) != marked[i].end())
					nbit++;
			}
			printf("   %d-bit adder\n", nbit);
		}

		printf("----------------------------------------------------------------\n");
		printf("----------------------------------------------------------------\n");
		printf("Result Search 2:\n");
		printf("----------------------------------------------------------------\n");

		for(unsigned int i = 0; i < marked2.size(); i++){
			printf("   %d-bit adder\n", (int) marked2[i].size());

		}
		printf("----------------------------------------------------------------\n");




	}
































	void findDecoder(CutFunction* cf, AIG* aigraph){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR DECODERS-------------------------------");

		std::map<unsigned long, std::string>::iterator it;
		std::map<unsigned long, std::string> hmap;
		cf->getHashMap(hmap);

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long, std::vector<std::vector<unsigned>*> > pmap;
		cf->getPortMap(pmap);

		//Functions that are carry and sum operations
		std::vector<unsigned long> and3;
		//pair of input (key), count
		std::map<std::vector<unsigned>, int > agg2;
		std::map<std::vector<unsigned>, int > agg3;


		//find and3 gates
		printf(" * Parsing function database for decoder blocks...");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("and3") != std::string::npos)
				and3.push_back(it->first);
		}

		for(unsigned int i = 0; i < and3.size(); i++){
			if(pmap.find(and3[i]) != pmap.end()){
				for(unsigned int j = 0; j < pmap[and3[i]].size(); j++){
					unsigned int k = 0;
					std::vector<unsigned> ports;

					//sort positive inputs (used as key)
					for(k = 0; k < pmap[and3[i]][j]->size()-1; k++){
						unsigned in = pmap[and3[i]][j]->at(k);
						//printf("%d ", in);
						/*
							 if(in < 0) in*=-1;

							 unsigned int s;
							 for(s = 0; i < ports.size(); s++){
							 if(in < ports[s])
							 break;
							 }
							 ports.insert(ports.begin() + s, in);
						 */
						ports.push_back(in);

					}
					//printf("\n");

					if(agg3.find(ports) == agg3.end()){
						agg3[ports] = 1;
					}
					else
						agg3[ports]++;
				}
			}
		}
		printf("done\n");



		//Searching for 2-4 decoders
		printf(" * Looking for 2-4 decoder function...");
		for(unsigned int i = 2; i < aigraph->getSize(); i++){
			unsigned c1 = aigraph->getChild1(i);
			unsigned c2 = aigraph->getChild2(i);
			if(c1 == 0 || c2 == 0)
				continue;

			std::vector<unsigned> pair;
			unsigned small, large;
			c1 = c1 & 0xFFFFFFFE;
			c2 = c2 & 0xFFFFFFFE;

			if(c1 < c2){
				small = c1;
				large = c2;
			}
			else{
				small = c2;
				large = c1;
			}

			pair.push_back(small);
			pair.push_back(large);

			if(agg2.find(pair) == agg2.end()){
				agg2[pair] = 1;
			}
			else
				agg2[pair]++;
		}
		printf("done\n");

		std::map<std::vector<unsigned>, int >::iterator ait;
		/*
			 printf("aggregation:\n");
			 for(ait = agg2.begin(); ait != agg2.end(); ait++){
			 printf("PAIR: %d %d\tCOUNT: %d\n", ait->first[0], ait->first[1], ait->second);	

			 }
			 for(ait = agg3.begin(); ait != agg3.end(); ait++){
			 printf("PAIR: %d %d %d\tCOUNT: %d\n", ait->first[0], ait->first[1], ait->first[2], ait->second);	

			 }
		 */

		printf("\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("\n----------------------------------------------------------------\n");
		printf("Result Search:\n");
		printf("----------------------------------------------------------------\n");
		int numdec = 0; 
		for(ait = agg2.begin(); ait != agg2.end(); ait++){
			if(ait->second == 4){
				printf("   2x4 Decoder: PAIR: %d %d\n", ait->first[0], ait->first[1]);	
				numdec++;
			}
		}
		for(ait = agg3.begin(); ait != agg3.end(); ait++){
			if(ait->second == 8){
				printf("   3x8 Decoder: PAIR: %d %d %d\n", ait->first[0], ait->first[1], ait->first[2]);	
				numdec++;
			}
		}
		printf("\nNum decoders:\t%d\n", numdec);
		printf("----------------------------------------------------------------\n");


	}






























	unsigned int findMultiplexor(CutFunction* cf, AIG* aigraph, std::vector<unsigned int>& returnlist){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR MULTIPLEXORS -------------------------------\n");

		//Functions in hashmap that are mux operations
		std::map<unsigned long, std::string>::iterator it;
		std::map<unsigned long, std::string> hmap;
		std::vector<unsigned long> muxes;
		cf->getHashMap(hmap);
		printf(" * Parsing function database for multiplexor blocks...\n");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("mux") != std::string::npos)
				muxes.push_back(it->first);
		}

		printf("Multiplexor Bitslices found in hashmap database: %d\n", (int)muxes.size());

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long, std::vector<std::vector<unsigned>*> > pmap;
		cf->getPortMap(pmap);

		//List of inputs of mux components. Last index is the output
		std::vector<std::vector<unsigned> > muxlist;

		//Get the Inputs and outputs of all the mux components found in the circuit 
		for(unsigned int i = 0; i < muxes.size(); i++){
			if(pmap.find(muxes[i]) != pmap.end()){
				for(unsigned int j = 0; j < pmap[muxes[i]].size(); j++){
					unsigned int k = 0;
					//printf("INPUT: ");
					std::vector<unsigned> ports;
					for(k = 0; k < pmap[muxes[i]][j]->size()-1; k++){
						//printf("%d ", pmap[muxes[i]][j]->at(k));
						ports.push_back(pmap[muxes[i]][j]->at(k));
					}
					//printf("\nOUTPUT:\t%d\n", pmap[muxes[i]][j]->at(k));
					unsigned outnode = pmap[muxes[i]][j]->at(k);
					ports.push_back(outnode);
					muxlist.push_back(ports);
				}
			}
		}
		printf("done\n");
		printf("Number of mux bitslice: %d\n", (int)muxlist.size());

		//simlar input, set of output
		printf(" * Looking for blocks with similar inputs...");
		std::map<unsigned, std::set<unsigned> > muxSet;
		std::map<unsigned, std::set<unsigned> > ::iterator mit;
		std::map<unsigned, std::set<unsigned> > ::iterator mit2;
		for(unsigned int i = 0; i < muxlist.size(); i++){
			for(unsigned int k = i+1; k < muxlist.size(); k++){
				//if(k == i) continue;

				for(unsigned int j = 0; j < muxlist[i].size()-1; j++){
					for(unsigned int l = 0; l < muxlist[k].size()-1; l++){
						//printf("Comparing %d %d\n", muxlist[i][j] , muxlist[k][l]);
						if(muxlist[i][j] == muxlist[k][l]){
							if(muxSet.find(muxlist[i][j]) != muxSet.end())
								muxSet[muxlist[i][j]].insert(muxlist[k][muxlist[k].size()-1]);
							else{
								std::set<unsigned> outset;
								outset.insert(muxlist[i][muxlist[i].size()-1]);
								outset.insert(muxlist[k][muxlist[k].size()-1]);

								muxSet[muxlist[i][j]] = outset;
							}

							break;
						}
					}
				}
			}
		}
		printf("done\n");



		std::set<unsigned>::iterator setit;

		printf(" * Aggregating Results...");
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
			for(mit2 = muxSet.begin(); mit2 != muxSet.end(); mit2++){
				if(mit == mit2) continue;

				unsigned int count = 0;
				for(setit = mit2->second.begin(); setit != mit2->second.end(); setit++){
					if(mit->second.find(*setit) != mit->second.end())
						count++;
				}

				if(count == mit2->second.size()){
					muxSet.erase(mit2);
					mit2 = mit;
				}
			}
		}

		printf("done\n");




		printf("\n\n\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("\n----------------------------------------------------------------\n");
		printf("Result Search:\n");
		printf("----------------------------------------------------------------\n");
		printf("Number of bitslice mux:\t%d\n", (int)muxlist.size()); 
		//mux size, list of a list of outputs
		std::map<int, std::vector<std::vector<unsigned> > > muxCount;
		std::map<int, std::vector<std::vector<unsigned> > >::iterator cnt;
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
			printf("%2d-bit mux\n", (int) mit->second.size());
			printf("Inputs: %d\tOutputs: ", mit->first);
			for(setit = mit->second.begin(); setit != mit->second.end(); setit++){
				printf("%d ", *setit);

			}
			printf("\n\n");



			if(muxCount.find(mit->second.size()) == muxCount.end()){
				std::vector<std::vector<unsigned> > listOutList;
				std::vector<unsigned> outlist;
				for(setit = mit->second.begin(); setit != mit->second.end(); setit++)
					outlist.push_back(*setit);

				listOutList.push_back(outlist);
				muxCount[mit->second.size()] = listOutList;

			}
			else{
				std::vector<unsigned> outlist;
				for(setit = mit->second.begin(); setit != mit->second.end(); setit++)
					outlist.push_back(*setit);

				muxCount[mit->second.size()].push_back(outlist);
			}
		}


		printf("2-1 Mux Found:\t%d\n", (int) muxCount.size());
		for(cnt = muxCount.begin(); cnt != muxCount.end(); cnt++){
			printf("   %2d-bit mux.....%d\n", cnt->first, (int) cnt->second.size());
			returnlist.push_back(cnt->first);
			/*
				 for(unsigned int i = 0; i < cnt->second.size(); i++){
				 printf("    *   ");
				 for(unsigned int j = 0; j < cnt->second[i].size(); j++){
				 printf("%d ", cnt->second[i][j]);
				 }
				 printf("\n");
				 }
				 printf("\n\n");
			 */
		}



		/*
			 printf("4-1 Mux Found:\t%d\n", (int)muxset4.size());
			 for(muxset4it = muxset4.begin(); muxset4it != muxset4.end(); muxset4it++){
			 printf("    %2d-bit mux\n", (int) muxset4it->second.size());
			 printf("      OR4: ");
			 for(unsigned int i = 0; i < muxset4it->second.size(); i++){
			 printf("%d ", muxset4it->second[i]);
			 }
			 printf("\n");

			 }
		 */




		return muxlist.size();
	}






	unsigned int findMux(CutFunction* cf, AIG* aigraph, std::vector<unsigned int>& returnlist){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING FOR MUXES -------------------------------\n");

		//Functions in hashmap that are mux operations
		std::map<unsigned long, std::string>::iterator it;
		std::map<unsigned long, std::string> hmap;
		std::vector<unsigned long> muxes;
		cf->getHashMap(hmap);
		printf(" * Parsing function database for multiplexor blocks...\n");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("mux") != std::string::npos)
				muxes.push_back(it->first);
		}

		printf("Multiplexor Bitslices found in hashmap database: %d\n", (int)muxes.size());

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long, std::vector<std::vector<unsigned>*> > pmap;
		cf->getPortMap(pmap);

		//List of inputs of mux components. Last index is the output
		std::vector<std::vector<unsigned> > muxlist;

		//Get the Inputs and outputs of all the mux components found in the circuit 
		for(unsigned int i = 0; i < muxes.size(); i++){
			if(pmap.find(muxes[i]) != pmap.end()){
				for(unsigned int j = 0; j < pmap[muxes[i]].size(); j++){
					unsigned int k = 0;
					//printf("INPUT: ");
					std::vector<unsigned> ports;
					for(k = 0; k < pmap[muxes[i]][j]->size()-1; k++){
						//printf("%d ", pmap[muxes[i]][j]->at(k));
						ports.push_back(pmap[muxes[i]][j]->at(k));
					}
					//printf("\nOUTPUT:\t%d\n", pmap[muxes[i]][j]->at(k));
					unsigned outnode = pmap[muxes[i]][j]->at(k);
					ports.push_back(outnode);
					muxlist.push_back(ports);
				}
			}
		}
		printf("done\n");
		printf("Number of mux bitslice: %d\n", (int)muxlist.size());

		//simlar input, set of output
		printf(" * Looking for blocks with similar inputs...");
		std::map<unsigned, std::set<unsigned> > muxSet;
		std::map<unsigned, std::set<unsigned> > ::iterator mit;
		std::map<unsigned, std::set<unsigned> > ::iterator mit2;
		for(unsigned int i = 0; i < muxlist.size(); i++){
			for(unsigned int k = i+1; k < muxlist.size(); k++){
				//if(k == i) continue;

				for(unsigned int j = 0; j < muxlist[i].size()-1; j++){
					for(unsigned int l = 0; l < muxlist[k].size()-1; l++){
						//printf("Comparing %d %d\n", muxlist[i][j] , muxlist[k][l]);
						if(muxlist[i][j] == muxlist[k][l]){
							if(muxSet.find(muxlist[i][j]) != muxSet.end())
								muxSet[muxlist[i][j]].insert(muxlist[k][muxlist[k].size()-1]);
							else{
								std::set<unsigned> outset;
								outset.insert(muxlist[i][muxlist[i].size()-1]);
								outset.insert(muxlist[k][muxlist[k].size()-1]);

								muxSet[muxlist[i][j]] = outset;
							}

							break;
						}
					}
				}
			}
		}
		printf("done\n");



		std::set<unsigned>::iterator setit;

		printf(" * Aggregating Results...");
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
			for(mit2 = muxSet.begin(); mit2 != muxSet.end(); mit2++){
				if(mit == mit2) continue;

				unsigned int count = 0;
				for(setit = mit2->second.begin(); setit != mit2->second.end(); setit++){
					if(mit->second.find(*setit) != mit->second.end())
						count++;
				}

				if(count == mit2->second.size()){
					muxSet.erase(mit2);
					mit2 = mit;
				}
			}
		}

		printf("done\n");




		printf("\n\n\nCOMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("\n----------------------------------------------------------------\n");
		printf("Result Search:\n");
		printf("----------------------------------------------------------------\n");
		printf("Number of bitslice mux:\t%d\n", (int)muxlist.size()); 
		//mux size, list of a list of outputs
		std::map<int, std::vector<std::vector<unsigned> > > muxCount;
		std::map<int, std::vector<std::vector<unsigned> > >::iterator cnt;
		for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
			printf("%2d-bit mux\n", (int) mit->second.size());
			printf("Inputs: %d\tOutputs: ", mit->first);
			for(setit = mit->second.begin(); setit != mit->second.end(); setit++){
				printf("%d ", *setit);

			}
			printf("\n\n");



			if(muxCount.find(mit->second.size()) == muxCount.end()){
				std::vector<std::vector<unsigned> > listOutList;
				std::vector<unsigned> outlist;
				for(setit = mit->second.begin(); setit != mit->second.end(); setit++)
					outlist.push_back(*setit);

				listOutList.push_back(outlist);
				muxCount[mit->second.size()] = listOutList;

			}
			else{
				std::vector<unsigned> outlist;
				for(setit = mit->second.begin(); setit != mit->second.end(); setit++)
					outlist.push_back(*setit);

				muxCount[mit->second.size()].push_back(outlist);
			}
		}


		printf("2-1 Mux Found:\t%d\n", (int) muxCount.size());
		for(cnt = muxCount.begin(); cnt != muxCount.end(); cnt++){
			printf("   %2d-bit mux.....%d\n", cnt->first, (int) cnt->second.size());
			returnlist.push_back(cnt->first);
			/*
				 for(unsigned int i = 0; i < cnt->second.size(); i++){
				 printf("    *   ");
				 for(unsigned int j = 0; j < cnt->second[i].size(); j++){
				 printf("%d ", cnt->second[i][j]);
				 }
				 printf("\n");
				 }
				 printf("\n\n");
			 */
		}



		/*
			 printf("4-1 Mux Found:\t%d\n", (int)muxset4.size());
			 for(muxset4it = muxset4.begin(); muxset4it != muxset4.end(); muxset4it++){
			 printf("    %2d-bit mux\n", (int) muxset4it->second.size());
			 printf("      OR4: ");
			 for(unsigned int i = 0; i < muxset4it->second.size(); i++){
			 printf("%d ", muxset4it->second[i]);
			 }
			 printf("\n");

			 }
		 */




		return muxlist.size();
	}

















	void findMux2(CutFunction* cf, AIG* aigraph, std::map<int,std::map<int, int> >& sizeCountMap){
		printf("\n\n\n");
		printf("[AGG] -- SEARCHING MUXES METHOD 2-------------------------------\n");

		std::map<unsigned long, std::string>::iterator it;
		std::map<unsigned long, std::string> hmap;
		cf->getHashMap(hmap);

		//Function, Vector of every set of inputs with that function. Last item is the output node
		std::map<unsigned long, std::vector<std::vector<unsigned>*> > pmap;
		cf->getPortMap(pmap);

		//Find all or gates up to or4
		std::vector<unsigned long> orFunction;
		printf(" * Parsing function database for mux components...\n");
		for(it = hmap.begin(); it!=hmap.end(); it++){
			if(it->second.find("or") != std::string::npos && it->second.find("xor") == std::string::npos){
				orFunction.push_back(it->first);
			}
		}

		//Store input and output of orgate
		std::list<std::vector<unsigned> > orIn;
		std::list<std::vector<unsigned> >::iterator orinit;
		std::list<std::vector<unsigned> >::iterator orinit2;
		std::list<unsigned> orOut;
		std::list<unsigned>::iterator  oroutit;
		std::list<unsigned>::iterator  oroutit2;
		for(unsigned int i = 0; i < orFunction.size(); i++){
			if(pmap.find(orFunction[i]) != pmap.end()){
				for(unsigned int j = 0; j < pmap[orFunction[i]].size(); j++){
					unsigned int k = 0;
					//printf("\nINPUT: ");
					std::vector<unsigned> ports;
					for(k = 0; k < pmap[orFunction[i]][j]->size()-1; k++){
						//	printf("%d ", pmap[orFunction[i]][j]->at(k));
						ports.push_back(pmap[orFunction[i]][j]->at(k));
					}
					//printf("\nOUTPUT:\t%d", pmap[orFunction[i]][j]->at(k));

					//If input is not contained in an already found, store. 
					unsigned outnode = pmap[orFunction[i]][j]->at(k);
					orIn.push_back(ports);
					orOut.push_back(outnode);
				}
			}
		}
		printf("done\n");


		//Verify inputs are negated
		printf("\n\n * Verifying negated inputs of OR...");
		orinit = orIn.begin();
		oroutit = orOut.begin();

		while(orinit != orIn.end()){
			unsigned int numNeg = findNegInput(aigraph, *oroutit, *orinit);
			//printf("NUM NEGATIVE INPUT: %d\t insize: %d\n", numNeg, (*orinit).size());
			//printf("IN %d OUT %d\n", orIn.size(), orOut.size());
			if((*orinit).size() != numNeg){
				/*printf("Size Mismatch\nDeleting....:  ");
					for(unsigned int i = 0; i < (*orinit).size(); i++)
					printf("%d ", orinit->at(i));

					printf("  OUT: %d\n", *oroutit);
				 */
				orinit = orIn.erase(orinit);
				oroutit = orOut.erase(oroutit);
			}
			else{
				orinit++;
				oroutit++;
			}

		}
		printf("done\n");

		printf("\n\nPossible Mux or gates\n");	
		orinit = orIn.begin();
		oroutit = orOut.begin();

		while(orinit != orIn.end()){
			printf("INPUT: ");
			for(unsigned int i = 0; i < orinit->size(); i++){
				printf("%d ", orinit->at(i));
			}
			printf("\nOutput: %d\n", *oroutit);

			orinit++;
			oroutit++;
		}
		printf("\n\n");

		//Remove or gates contained within another. 
		/*
			 orinit = orIn.begin();
			 oroutit = orOut.begin();
			 printf(" * Removing Redundancies that are contained...");
			 while(orinit != orIn.end()){
			 orinit2 = orinit;
			 orinit2++;
			 oroutit2 = oroutit;
			 oroutit2++;

			 bool next = true;
			 while(orinit2 != orIn.end()){

			 unsigned int numMatch = 0;

			 for(unsigned int i = 0; i < orinit->size(); i++){
			 for(unsigned int j = 0; j < orinit2->size(); j++){
			 if(orinit->at(i) == orinit2->at(j)){
			 numMatch++;
			 break;
			 }
			 }
			 }

			 if(numMatch == orinit->size()){
			 orinit = orIn.erase(orinit);
			 oroutit = orOut.erase(oroutit);
			 next = false;
			 break;
			 }
			 else if(numMatch == orinit2->size()){
			 orinit2 = orIn.erase(orinit2);
			 oroutit2 = orOut.erase(oroutit2);
			 }
			 else{
			 oroutit2++;
			 orinit2++;
			 }
			 }

			 if(next){
			 orinit++;
			 oroutit++;
			 }
			 }
			 printf("done\n");
		 */



		printf(" * Searching for common signals\n");
		//Vector of select bits, list of orgates with those bits
		std::map<std::vector<int>, std::vector<std::list<std::vector<unsigned> >::iterator > > selectMap;
		std::map<std::vector<int>, std::vector<std::list<std::vector<unsigned> >::iterator > > ::iterator iSelectMap;

		std::map<int, std::map<std::vector<int>, int> >stats;
		std::map<int, std::map<std::vector<int>, int> >::iterator iStats;
		std::vector<std::vector<int> > selectBits;
		orinit = orIn.begin();
		oroutit = orOut.begin();

		//BFS
		while(orinit != orIn.end()){
			std::vector<std::list<unsigned int> > queue;
			std::vector<std::set<unsigned int> > marked;
			marked.reserve(orinit->size());
			queue.reserve(orinit->size());

			//Prepare a queue for each input
			for(unsigned int i = 0; i < orinit->size(); i++){
				std::list<unsigned int> startqueue;
				startqueue.push_back(orinit->at(i));
				//printf("START: %d INDEX: %d\n", orinit->at(i), i);
				queue.push_back(startqueue);

				std::set<unsigned int> startmark;
				startmark.insert(orinit->at(i));
				marked.push_back(startmark);
			}



			//Node id, count
			std::map<unsigned int, int> sameSignalCount;
			bool isQueueEmpty= false;
			unsigned int aigInputLimit = (2 * aigraph->getInputSize()) + 1;
			int numSameSignal = 0;
			unsigned int mask = 1;
			while(mask < orinit->size()){
				numSameSignal++;
				mask = mask << 1;
			}

			//printf("\nAIG SAME SIGNAL: %d\n", numSameSignal);
			//printf("Number of Inputs: %d\n", orinit->size());
			int numSameSignalCount= 0;
			std::vector<int> selectBit;
			while(!isQueueEmpty){
				isQueueEmpty = true;

				//Loop through each BFS
				for(unsigned int i = 0; i < orinit->size(); i++){

					//Check to see if queue is empty
					if(queue[i].empty())	continue;
					isQueueEmpty = false;


					//Pop the queue	
					unsigned int signal = queue[i].front() & 0xFFFFFFFE;
					//printf("SIGNAL POPPED: %d INDEX %d\n", signal, i);
					queue[i].pop_front();

					//Increase signal count
					if(sameSignalCount.find(signal) == sameSignalCount.end())
						sameSignalCount[signal] = 1;
					else
						sameSignalCount[signal]++;


					//Check to see if the number of same signals are as expected
					if(sameSignalCount[signal] == numSameSignal+1){
						numSameSignalCount++;
						selectBit.push_back(signal);
					}

					//Check to see if it is an input node
					if(signal <= aigInputLimit)	continue;

					//Search the childrens
					unsigned int c1 = aigraph->getChild1(signal);
					unsigned int c2 = aigraph->getChild2(signal);

					if(marked[i].find(c1) == marked[i].end()){
						marked[i].insert(c1);
						queue[i].push_back(c1);
					}
					if(marked[i].find(c2) == marked[i].end()){
						marked[i].insert(c2);
						queue[i].push_back(c2);
					}
				}
			}

			if(numSameSignalCount == 0){
				//printf("Not possible...Deleting from list...\n");
				orinit = orIn.erase(orinit);
				oroutit = orOut.erase(oroutit);
			}
			else{
				selectMap[selectBit].push_back(orinit);
				selectBits.push_back(selectBit);

				if(stats.find(orinit->size()) == stats.end()){
					std::map<std::vector<int>, int> count;
					count[selectBit] = 1;
					stats[orinit->size()] = count;
				}
				else{
					if(stats[orinit->size()].find(selectBit) == stats[orinit->size()].end()){
						stats[orinit->size()][selectBit] = 1;
					}
					else
						stats[orinit->size()][selectBit] ++;
				}

				


				orinit++;
				oroutit++;
			}


		}


		printf("Select Bits\n");
		for(unsigned int i = 0; i < selectBits.size(); i++){
			for(unsigned int k = 0; k < selectBits[i].size(); k++){
				printf("%d ", selectBits[i][k]);
			}
			printf(" |  ");
		}
		printf("\n");


		printf("\n\nPossible Mux or gates\n");	
		orinit = orIn.begin();
		oroutit = orOut.begin();

		while(orinit != orIn.end()){
			printf("INPUT: ");
			for(unsigned int i = 0; i < orinit->size(); i++){
				printf("%d ", orinit->at(i));
			}
			printf("\nOutput: %d\n", *oroutit);

			orinit++;
			oroutit++;
		}
		printf("\n\n");

		for(iStats = stats.begin(); iStats!=stats.end(); iStats++){
			std::map<std::vector<int>, int>::iterator iCount;
			printf("%d-1 Mux:\n", iStats->first);
			for(iCount = iStats->second.begin(); iCount!=iStats->second.end(); iCount++){
				printf("\t* %d-Bit Mux\n", iCount->second);
			}
		}
		




	}



	void aggregateSignal(std::map<unsigned long, std::vector<std::vector<unsigned>*> >& pmap, std::vector<unsigned long>& andFunction, std::vector<std::set<unsigned> >& andSimIn,	std::vector<std::set<unsigned> >& andSimOut){
		std::list<std::vector<unsigned> > andIn;
		std::list<std::vector<unsigned> >::iterator andinit;
		std::list<std::vector<unsigned> >::iterator andinit2;
		std::list<unsigned> andOut;
		std::list<unsigned>::iterator  andoutit;
		std::list<unsigned>::iterator  andoutit2;


		for(unsigned int i = 0; i < andFunction.size(); i++){
			if(pmap.find(andFunction[i]) != pmap.end()){
				for(unsigned int j = 0; j < pmap[andFunction[i]].size(); j++){
					unsigned int k = 0;
					//printf("INPUT: ");
					std::vector<unsigned> ports;
					for(k = 0; k < pmap[andFunction[i]][j]->size()-1; k++){
						//printf("%d ", pmap[andFunction[i]][j]->at(k));
						ports.push_back(pmap[andFunction[i]][j]->at(k));
					}

					//If input is not contained in an already found, store. 
					//printf("\nOUTPUT:\t%d\n", pmap[andFunction[i]][j]->at(k));
					unsigned outnode = pmap[andFunction[i]][j]->at(k);
					andIn.push_back(ports);
					andOut.push_back(outnode);
				}
			}
		}
		if(andIn.size() == 0)
			return;

		//std::vector<std::set<int> > andSimIn;
		//std::vector<std::set<int> > andSimOut;

		andoutit = andOut.begin();
		andoutit2 = andOut.begin();


		//Group outputs with similar inputs. 
		for(andinit = andIn.begin(); andinit != andIn.end(); andinit++){
			andinit2 = andinit;
			andoutit2 = andoutit;
			andoutit2++;
			for(++andinit2; andinit2 != andIn.end(); andinit2++){

				//Find similar inputs
				for(unsigned int i = 0; i < andinit->size(); i++){
					for(unsigned int j = 0; j < andinit2->size(); j++){
						if(andinit->at(i) == andinit2->at(j)){
							//Check if input has been found before
							bool foundInput = false;
							for(unsigned int q = 0; q < andSimIn.size(); q++){
								if(andSimIn[q].find(andinit->at(i)) != andSimIn[q].end()){
									if(andSimIn[q].size() != 1){
										continue;
									}
									andSimOut[q].insert(*andoutit);
									andSimOut[q].insert(*andoutit2);
									foundInput = true;
									break;
								}
							}

							if(!foundInput){
								std::set<unsigned> andSimSetIn;
								std::set<unsigned> andSimSetOut;
								andSimSetIn.insert(andinit->at(i));
								andSimSetOut.insert(*andoutit);
								andSimSetOut.insert(*andoutit2);
								andSimIn.push_back(andSimSetIn);
								andSimOut.push_back(andSimSetOut);
							}

						}
					}
				}

				andoutit2++;
			}
			andoutit++;
		}
		/*	
				std::set<int>::iterator andsetit;
				for(unsigned int i = 0; i < andSimIn.size(); i++){
				printf("\nSimilar Input: ");
				for(andsetit = andSimIn[i].begin(); andsetit != andSimIn[i].end(); andsetit++){
				printf("%d ", *andsetit);
				}
				printf("\nOutputs: ");
				for(andsetit = andSimOut[i].begin(); andsetit != andSimOut[i].end(); andsetit++){
				printf("%d ", *andsetit);
				}
				printf("\n");
				}
				printf("\n\n");
		 */

		//Remove redundancies

		if(andSimOut.size() == 0 && andSimIn.size() == 0){
			return;
		}

		//Different similar inputs with exactly the same output
		std::set<unsigned>::iterator andsimit;
		for(unsigned int i = 0; i < andSimOut.size()-1; i++){
			for(unsigned int j = i+1; j < andSimOut.size(); j++){

				//Check to see if Entire output is the same
				if(andSimOut[i].size() == andSimOut[j].size()){
					bool isDiff = false;
					for(andsimit = andSimOut[i].begin(); andsimit != andSimOut[i].end(); andsimit++){
						if(andSimOut[j].find(*andsimit) == andSimOut[j].end()){
							isDiff = true;
							break;
						}
					}
					if(!isDiff){
						andSimOut.erase(andSimOut.begin() + j);
						for(andsimit = andSimIn[j].begin(); andsimit != andSimIn[j].end(); andsimit++){
							andSimIn[i].insert(*andsimit);
						}
						andSimIn.erase(andSimIn.begin() + j);
					}
				}

			}		
		}


		//Combine similar inputs
		for(unsigned int i = 0; i < andSimIn.size()-1; i++){
			for(unsigned int j = i+1; j < andSimIn.size(); j++){

				//Check to see if Entire input is the same
				if(andSimIn[i].size() == andSimIn[j].size()){
					bool isDiff = false;
					for(andsimit = andSimIn[i].begin(); andsimit != andSimIn[i].end(); andsimit++){
						if(andSimIn[j].find(*andsimit) == andSimIn[j].end()){
							isDiff = true;
							break;
						}
					}
					if(!isDiff){
						for(andsimit = andSimOut[j].begin(); andsimit != andSimOut[j].end(); andsimit++){
							andSimOut[i].insert(*andsimit);
						}
						andSimOut.erase(andSimOut.begin() + j);
						andSimIn.erase(andSimIn.begin() + j);
						j--;
					}
				}

			}
		}
		printf("DONE\n");


	}

	int findNegInput(AIG* aigraph, unsigned out, std::vector<unsigned>& in){
		std::list<unsigned> queue;
		std::set<unsigned> mark;
		std::vector<unsigned> nodes;
		std::vector<unsigned*> nChilds;


		//Enqueue
		queue.push_back(out);
		mark.insert(out);
		int numNegative = 0;

		while(queue.size() != 0){
			unsigned qitem = queue.front();
			queue.pop_front();


			//If reached an input, return
			bool foundInput = false;
			bool isNegative = false;
			for(unsigned int i = 0; i < in.size(); i++){
				if(qitem & 0x1){
					qitem = qitem & 0xFFFFFFFE;
					isNegative = true;
				}

				unsigned input = in[i];
				input = input & 0xFFFFFFFE;

				if(qitem == in[i]){
					if(isNegative){
						numNegative++;
					}

					foundInput = true;
					break;
				}
			}

			if(foundInput) continue;

			nodes.push_back(qitem);


			//Enqueue childs
			unsigned c1node = aigraph->getChild1(qitem);
			unsigned c2node = aigraph->getChild2(qitem);
			unsigned c1 = c1node & 0xFFFFFFFE;
			unsigned c2 = c2node & 0xFFFFFFFE;

			if(mark.find(c1) == mark.end()){
				queue.push_back(c1node);
				mark.insert(c1);
			}
			if(mark.find(c2) == mark.end()){
				queue.push_back(c2node);
				mark.insert(c2);
			}
		}

		return numNegative;

	}


}



/*
//4-1 Mux search
printf("\n\n");
printf("[AGG] -- SEARCHING FOR MUXES 4-1-------------------------------\n");

//Functions that are carry and sum operations
std::vector<unsigned long> or4;
std::vector<unsigned long> and3;

printf(" * Parsing function database for mux components...");
for(it = hmap.begin(); it!=hmap.end(); it++){
if(it->second.find("and3") != std::string::npos)
and3.push_back(it->first);
else if(it->second.find("or4") != std::string::npos)
or4.push_back(it->first);
}


//List of inputs of sum components. Last index is the output
std::vector<std::vector<unsigned> > andList;
std::vector<std::vector<unsigned> > orList;

//printf("or\n");
//Get the Inputs and outputs of all the sum and carry componenets
for(unsigned int i = 0; i < or4.size(); i++){
if(pmap.find(or4[i]) != pmap.end()){
for(unsigned int j = 0; j < pmap[or4[i]].size(); j++){
unsigned int k = 0;
//printf("INPUT: ");
std::vector<unsigned> ports;
for(k = 0; k < pmap[or4[i]][j]->size()-1; k++){
//printf("%d ", pmap[or4[i]][j]->at(k));
ports.push_back(pmap[or4[i]][j]->at(k));
}
//printf("\nOUTPUT:\t%d\n", pmap[or4[i]][j]->at(k));
unsigned outnode = pmap[or4[i]][j]->at(k);
ports.push_back(outnode);
orList.push_back(ports);
}
}
}

//printf("and\n");
for(unsigned int i = 0; i < and3.size(); i++){
if(pmap.find(and3[i]) != pmap.end()){
for(unsigned int j = 0; j < pmap[and3[i]].size(); j++){
unsigned int k = 0;
//printf("INPUT: ");
std::vector<unsigned> ports;
for(k = 0; k < pmap[and3[i]][j]->size()-1; k++){
//printf("%d ", pmap[and3[i]][j]->at(k));
ports.push_back(pmap[and3[i]][j]->at(k));
}
//printf("\nOUTPUT:\t%d\n", pmap[and3[i]][j]->at(k));
unsigned outnode = pmap[and3[i]][j]->at(k);
ports.push_back(outnode);
andList.push_back(ports);
}
}
}
printf("\n");

//simlar input, set of output
//for each or
//or4 out, index of and3 in andlist
printf(" * Comparing Out of AND to In or OR...");
std::map<unsigned, std::vector<unsigned> > mux4to1;
std::map<unsigned, std::vector<unsigned> >::iterator mit41;
for(unsigned int i = 0; i < orList.size(); i++){
int inCount = 0;
std::vector<unsigned> andIndex;

//printf("Aggregating OR4: %d\n", orList[i][orList[i].size()-1]);
for(unsigned int l = 0; l < orList[i].size()-1; l++){
	for(unsigned int k = 0; k < andList.size(); k++){
		//if(orList[i][orList[i].size()-1]>433);
		//	printf("COMPARING: ORIN: %d ANDOUT %d\n", orList[i][l], andList[k][andList[k].size()-1]);
		if(andList[k][andList[k].size()-1] == orList[i][l]){
			//printf("Match\n");
			inCount++;
			andIndex.push_back(k);
			break;
		}
	}
}

if(inCount == 4){
	mux4to1[orList[i][orList[i].size()-1]] = andIndex;
}
}
printf("done\n");

//printf("RESULTS\n");
//input to and, count
std::map<std::set<unsigned>, std::vector<unsigned> > muxset4;
std::map<std::set<unsigned>, std::vector<unsigned> >::iterator muxset4it;
//For each orgate
printf(" * Aggregating Results...");
for(mit41 = mux4to1.begin(); mit41 != mux4to1.end(); mit41++){
	//printf("Or4 Output: %d\n", mit41->first);
	//printf("Andgates\n");

	//CHeck to see if mux4-1 structure is actual mux	
	//input, count
	std::map<unsigned, int> inCountMap;
	std::map<unsigned, int>::iterator icmit;
	bool notValid = false;
	//Keep track of number specific input shows up
	//For each and gate
	for(unsigned int i = 0; i < mit41->second.size(); i++){
		int index = mit41->second[i];
		//Count the number of inputs
		for(unsigned int j = 0; j < andList[index].size()-1; j++){
			if(inCountMap.find(andList[index][j]) == inCountMap.end())
				inCountMap[andList[index][j]] = 1;
			else
				inCountMap[andList[index][j]]++;
		}

		//skip first iteration
		if(i != 0){
			unsigned int numCounted = 0;	
			for(icmit = inCountMap.begin(); icmit != inCountMap.end(); icmit++){
				if(icmit->second == (int)(i+1))
					numCounted++;
			}
			if(numCounted != 2){
				//printf("PORTS MISMATCH####################################################\n");
				notValid = true;
				break;
			}
		}
	}

	//If input ports don't match up, go to next or gate
	if(notValid)
		continue;

	std::vector<unsigned> selectBits;
	//Find the select bits. Make sure there is two. 
	for(icmit = inCountMap.begin(); icmit != inCountMap.end(); icmit++){
		if(icmit->second == 4)
			selectBits.push_back(icmit->first);
	}

	if(selectBits.size() != 2){
		printf("possible select is not equal to two");
		continue;
	}


	//See if select bits already exist
	bool muxsetFound = false;
	for(muxset4it = muxset4.begin(); muxset4it != muxset4.end(); muxset4it++){

		if(muxset4it->first.find(selectBits[0]) != muxset4it->first.end() &&
				muxset4it->first.find(selectBits[1]) != muxset4it->first.end() ){
			muxset4it->second.push_back(mit41->first);
			muxsetFound = true;
			break;
		}
	}

	if(!muxsetFound){
		std::vector<unsigned> orout;
		std::set<unsigned> key;
		key.insert(selectBits[0]);
		key.insert(selectBits[1]);

		orout.push_back(mit41->first);
		muxset4[key] = orout;
	}

}




for(mit = muxSet.begin(); mit != muxSet.end(); mit++){
	for(mit2 = muxSet.begin(); mit2 != muxSet.end(); mit2++){
		if(mit == mit2) continue;

		unsigned int count = 0;
		for(setit = mit2->second.begin(); setit != mit2->second.end(); setit++){
			if(mit->second.find(*setit) != mit->second.end())
				count++;
		}

		if(count == mit2->second.size()){
			muxSet.erase(mit2);
			mit2 = mit;
		}
	}
}
printf("done\n");

//Check to see if 2-1 is not contained in 4-1	
printf(" * Checking for redundancies...");
for(mit = muxSet.begin(); mit != muxSet.end(); ){
	bool found = false;
	for(muxset4it = muxset4.begin(); muxset4it != muxset4.end(); muxset4it++){
		if(muxset4it->first.find(mit->first) != muxset4it->first.end()){
			muxSet.erase(mit);
			found = true;
			break;
		}
	}
	if(!found) mit++;
	else mit = muxSet.begin();
}
printf("done\n");
*/


#endif
