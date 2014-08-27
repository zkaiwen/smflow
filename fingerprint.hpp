/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  FINGERPRINT.cpp
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#ifndef FIN_GUARD 
#define SEQ_GUARD

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include "graph.hpp"
#include "vertex.hpp"
#include "aig.hpp"


//Counter
namespace FINGERPRINT{
	void getMuxFingerprint_naive(std::map<int, std::map<int, int> >& muxes, std::vector<unsigned long>& fingerprint){
		printf("[FINGERPRINT] -- Extracting fingerprint for Multiplexors\n");

		std::map<int, std::map<int, int> >::iterator iMapM;
		std::map<int, int>::iterator iMap;
		unsigned int bIndex= 0;
		unsigned int limit = 64;

			for(iMapM = muxes.begin(); iMapM != muxes.end(); iMapM++){
				limit = 64;

				if(iMapM->first == 2) bIndex = 0;
				else if(iMapM->first == 3) bIndex = 4;
				else if(iMapM->first == 4) bIndex = 8;
				else{
					printf("UNKNOWN MUX SIZE\n");
					exit(1);
				}

				for(iMap = iMapM->second.begin(); iMap != iMapM->second.end(); iMap++){
					unsigned int muxbit = iMap->first;

					if(muxbit > limit){
						limit*=2;
						bIndex++;
					}

					fingerprint[bIndex] = fingerprint[bIndex] | (((unsigned long)1)<<(muxbit-1));
				}
			}
	}


	void getRegFingerprint_naive(std::map<int, int>& regs, std::vector<unsigned long>& fingerprint){
		printf("[FINGERPRINT] -- Extracting fingerprint for Register\n");

		std::map<int, int>::iterator iMap;
		unsigned int bIndex= 0;
		unsigned int limit = 64;
		for(iMap = regs.begin(); iMap != regs.end(); iMap++){
			unsigned int regbit = iMap->first;
			if(regbit > limit){
				limit*=2;
				bIndex++;
			}
			fingerprint[bIndex] = fingerprint[bIndex] | (((unsigned long)1)<<(regbit-1));
		}
	}
	
	
	float tanimoto(std::vector<unsigned long>& f1, std::vector<unsigned long>&f2){
		//Assert the bitlenght of the two fingerprints are the same
		assert(f1.size() == f2.size());
		double N_f1 = 0.0;
		std::set<unsigned int> f1Pos;
		unsigned long mask = 1;

		//Count the number of 1's in the first fingerprint
		for(unsigned int i = 0; i < f1.size(); i++){
			mask = 1;
			
			//Go through each bit
			for(unsigned int k = 0; k < 64; k++){
				//See if that bit is high
				if(f1[i] & mask){
					N_f1++;

					//Keep track of the position where the high is found
					f1Pos.insert((i*64)+(k+1));
				}
				mask = mask<<1;
			}
		}

		//Count the number of 1's in the second fingerprint
		double N_f2 = 0.0;
		double N_f1f2 = 0.0;
		for(unsigned int i = 0; i < f2.size(); i++){
			mask = 1;
			for(unsigned int k = 0; k < 64; k++){
				if(f2[i] & mask){
					N_f2++;

					//Check if the first set has the same bit high
					if(f1Pos.find((i*64)+(k+1)) != f1Pos.end())
						N_f1f2++;
				}
				
				mask = mask<<1;
			}
		}

		double denom = (N_f1+N_f2-N_f1f2);
		if(denom == 0.0)	return 0;
		return N_f1f2 / denom;
	}
	
	double euclideanDistanceWNorm(std::vector<double>& f1, std::vector<double>&f2){
		assert(f1.size() == f2.size());

		double f1norm = 0.0;
		double f2norm = 0.0;
		for(unsigned int i = 0; i < f1.size(); i++){
			f1norm += (f1[i] * f1[i]);
			f2norm += (f2[i] * f2[i]);
		}

		f1norm = sqrt(f1norm);
		f2norm = sqrt(f2norm);

		for(unsigned int i = 0; i < f1.size(); i++){
			f1[i] = f1[i] / f1norm;
			f2[i] = f2[i] / f2norm;
		}

		double distance = 0.0;
		for(unsigned int i = 0; i < f1.size(); i++){
			double diff = (f1[i] - f2[i]);
			distance += (diff * diff) ;
		}

		return sqrt(distance);

	}
	
	
	double euclideanDistance(std::vector<double>& f1, std::vector<double>&f2){
		assert(f1.size() == f2.size());

		double distance = 0.0;
		for(unsigned int i = 0; i < f1.size(); i++){
			double diff = (f1[i] - f2[i]);
			distance += (diff * diff) ;
		}

		return sqrt(distance);

	}


}





#endif
