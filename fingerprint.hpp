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
#define FIN_GUARD

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
	double tanimoto2(std::vector<unsigned>& f1, std::vector<unsigned>&f2);
	void getMuxFingerprint_naive(std::map<unsigned, std::map<unsigned, unsigned> >& muxes, std::vector<unsigned long long>& fingerprint){
		printf("[FINGERPRINT] -- Extracting fingerprint for Multiplexors\n");

		std::map<unsigned, std::map<unsigned, unsigned> >::iterator iMapM;
		std::map<unsigned, unsigned>::iterator iMap;
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

					fingerprint[bIndex] = fingerprint[bIndex] | (((unsigned long long)1)<<(muxbit-1));
				}
			}
	}


	void getRegFingerprint_naive(std::map<unsigned, unsigned>& regs, std::vector<unsigned long long>& fingerprint){
		printf("[FINGERPRINT] -- Extracting fingerprint for Register\n");

		std::map<unsigned, unsigned>::iterator iMap;
		unsigned int bIndex= 0;
		unsigned int limit = 64;
		for(iMap = regs.begin(); iMap != regs.end(); iMap++){
			unsigned int regbit = iMap->first;
			if(regbit > limit){
				limit*=2;
				bIndex++;
			}
			fingerprint[bIndex] = fingerprint[bIndex] | (((unsigned long long)1)<<(regbit-1));
		}
	}
	
	
	float tanimoto(std::vector<unsigned long long>& f1, std::vector<unsigned long long>&f2){
		//Assert the bitlenght of the two fingerprints are the same
		assert(f1.size() == f2.size());
		double N_f1 = 0.0;
		std::set<unsigned int> f1Pos;
		unsigned long long mask = 1;

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
	
	double cutInputFingerprint(std::map<unsigned, unsigned>& data1, std::map<unsigned, unsigned>& data2){
		std::map<unsigned, unsigned>::iterator it1;
		std::map<unsigned, unsigned>::iterator it2;

		if(data1.size() == 0 || data2.size() == 0){
			return -1.0;
		}

		//Find the largest size
		it1 = data1.end(); it1--;
		it2 = data2.end(); it2--;
		int lastVal1 = it1->first; 
		int lastVal2 = it2->first; 

		int vectorSize = (lastVal1 < lastVal2) ? lastVal2 : lastVal1;
		//form vectors
		std::vector<unsigned> f1(vectorSize+1, 0);
		std::vector<unsigned> f2(vectorSize+1, 0);

		//Fill in fingerprint with data
		for(it1 = data1.begin(); it1 != data1.end(); it1++)
			f1[it1->first] = it1->second;

		for(it2 = data2.begin(); it2 != data2.end(); it2++)
			f2[it2->first] = it2->second;

		double similarity = tanimoto2(f1, f2);
		
		return similarity;
	}
	



	int findMinDistance(std::map<unsigned, unsigned>& data, std::set<unsigned>& marked, unsigned value, std::map<unsigned, unsigned>::iterator& minIt){
				std::map<unsigned, unsigned>::iterator iMap;
				int minDiff = 10000;

				for(iMap= data.begin(); iMap!= data.end(); iMap++){
					//If the difference is greater than window size, continue;
					int difference = iMap->first - value;
					if(difference < 0) difference *= -1;


					if(difference < minDiff){
						//Check to see if it has been marked before
						if(marked.find(iMap->first) != marked.end()) continue;
						minDiff = difference;
						minIt = iMap;
					}
				}
				return minDiff;

	}


	double tanimotoWindow(std::map<unsigned, unsigned>& data1, std::map<unsigned,unsigned>& data2){
		if(data1.size() == 0 || data2.size() == 0){
			return -1.0;
		}
		double N_f1 = data1.size();
		double N_f2 = data2.size();

		//Count the number of 1's in the second fingerprint
		const int windowSize = 5;
		double N_f1f2_ratio = 0.0;

		std::map<unsigned, unsigned>::iterator iMap;
		std::map<unsigned, unsigned>::iterator iMap2;
		std::map<unsigned, unsigned>::iterator iTemp;
		std::map<unsigned, unsigned>::iterator iMapF;
		std::set<unsigned> marked1;
		std::set<unsigned> marked2;

		const double multiplier[6] = {
			1.00, 0.95, 0.90, 0.85, 0.80, 0.75
		};

		printf("\n\nBIT LOC1: ");
		for(iMap = data1.begin(); iMap != data1.end(); iMap++){
			printf("%d ", iMap->first);
		}
		printf("\t\tBIT CNT1: ");
		for(iMap = data1.begin(); iMap != data1.end(); iMap++){
			printf("%d ", iMap->second);
		}
		
		printf("\nBIT LOC2: ");
		for(iMap = data2.begin(); iMap != data2.end(); iMap++){
			printf("%d ", iMap->first);
		}
		printf("\t\tBIT CNT2: ");
		for(iMap = data2.begin(); iMap != data2.end(); iMap++){
			printf("%d ", iMap->second);
		}
		printf("\n");

		//Count the number of bits that are the same
		for(iMap = data1.begin(); iMap != data1.end(); iMap++){
			iTemp = data2.find(iMap->first);	
			printf("Looking for %d in data1...", iMap->first);

			if(iTemp != data2.end()){
				printf("found\n");
				double ratio;
				/*
				   if(iTemp->second < iMap->second)
				   ratio = (double)iTemp->second / (double) iMap->second; 
				   else
				   ratio = (double)iMap->second / (double) iTemp->second;
				 */
				if(iTemp->second < iMap->second){
					ratio = (double)iMap->second - (double) iTemp->second;
					ratio = ratio /(((double)iMap->second + (double) iTemp->second)/2);
				}
				else{
					ratio = (double)iTemp->second - (double) iMap->second;
					ratio = ratio /(((double)iMap->second + (double) iTemp->second)/2);
				}

				ratio = 1.0 - ratio;

				N_f1f2_ratio += ratio;
				marked1.insert(iMap->first);
				marked2.insert(iTemp->first);
			}
			else printf("\n");
		}

		//Go through the vector again and try and find similar matches in the bits (WINDOWING)
		for(iMap = data1.begin(); iMap != data1.end(); iMap++){
			if(marked1.find(iMap->first) == marked1.end()){
				int minDistance1 = findMinDistance(data2, marked2, iMap->first, iTemp);
				int minDistance2 = findMinDistance(data1, marked1, iTemp->first, iMapF);

				//There exists a shorter distance
				if(iMapF->first != iMap->first){
					continue;
				}
				else if(minDistance1 != minDistance2) continue;

				//Similar size found within window
				if(minDistance1 <= windowSize){
					double ratio;
					/*
					   if(iTemp->second < iMap->second) 
					   ratio = ((double)iTemp->second / (double)iMap->second)*multiplier[minDistance1];
					   else
					   ratio = ((double)iMap->second / (double)iTemp->second)*multiplier[minDistance1];
					 */

					if(iTemp->second < iMap->second){
						ratio = (double)iMap->second - (double) iTemp->second;
						ratio = ratio /(((double)iMap->second + (double) iTemp->second)/2);
					}
					else{
						ratio = (double)iTemp->second - (double) iMap->second;
						ratio = ratio /(((double)iMap->second + (double) iTemp->second)/2);
					}

					ratio = (1.0 - ratio) * multiplier[minDistance1];

					N_f1f2_ratio += ratio;
					marked1.insert(iMap->first);
					marked2.insert(iTemp->first);
				}
			}
		}

		double denom = (N_f1+N_f2-N_f1f2_ratio);
		if(denom == 0.0)	return 0.0;

		return N_f1f2_ratio / denom;
	}


	double tanimoto2(std::vector<unsigned>& f1, std::vector<unsigned>&f2){
		//Assert the bitlenght of the two fingerprints are the same
		assert(f1.size() == f2.size());
		double N_f1 = 0.0;
		std::set<unsigned int> f1Pos;

		//Count the number of 1's in the first fingerprint
		for(unsigned int i = 0; i < f1.size(); i++){
			if(f1[i] > 0){
				N_f1++;
				f1Pos.insert(i);
			}
		}

		//Count the number of 1's in the second fingerprint
		double N_f2 = 0.0;
		double N_f1f2 = 0.0;
		double N_f1f2_ratio = 0.0;
		for(unsigned int i = 0; i < f2.size(); i++){
			if(f2[i] > 0){
				N_f2++;

					//Check if the first set has the same bit high
					if(f1Pos.find(i) != f1Pos.end()){
						double ratio = (f2[i] < f1[i]) ? (f2[i] / f1[i]) : (f1[i] / f2[i]);
						N_f1f2_ratio += ratio;
						N_f1f2++;
					}
				}
		}

		double denom = (N_f1+N_f2-N_f1f2_ratio);
		if(denom == 0.0)	return 0.0;

		return N_f1f2_ratio / denom;
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

	unsigned hammingDistance(unsigned long func1, unsigned long func2){
		unsigned hamming = 0;
		unsigned long xnor = func1 ^ func2;
		xnor = ~xnor;

		unsigned long mask = 0x1;
		for(unsigned int i = 0; i < 64; i++)
			if((mask & xnor) > 0)
				hamming++;

		return hamming;
	}



}





#endif
