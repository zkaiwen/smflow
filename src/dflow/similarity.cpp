/*@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@ 
	@
	@  SIMILARITY.cpp
	@  
	@  @AUTHOR:Kevin Zeng
	@  Copyright 2012 – 2013 
	@  Virginia Polytechnic Institute and State University
	@
	@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@*/

#include "similarity.hpp"

int SIMILARITY::findMinDistance(std::map<unsigned, unsigned>& data, std::set<unsigned>& marked, unsigned value, std::map<unsigned, unsigned>::iterator& minIt){
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





double SIMILARITY::tanimotoWindow_size(std::map<unsigned, unsigned>& data1, std::map<unsigned,unsigned>& data2){
	if(data1.size() == 0 || data2.size() == 0){
		//return -1.0;
		return 0.0;
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

	std::map<unsigned, unsigned> temp;
	std::map<unsigned, unsigned> map;

	const double multiplier[windowSize+1] = {
		1.000, 0.9938, 0.9772, 0.9332, 0.8413, 0.6915
	};

	
	//Count the number of bits that are the same
	for(iMap = data1.begin(); iMap != data1.end(); iMap++){
		iTemp = data2.find(iMap->first);	

		if(iTemp != data2.end()){
			printf("Both sets have : %d\n", iTemp->first); 
			N_f1f2_ratio += 1.000;

			marked1.insert(iMap->first);
			marked2.insert(iTemp->first);
		}
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
				double ratio = multiplier[minDistance1];

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








double SIMILARITY::tanimotoWindow(std::map<unsigned, unsigned> data1, std::map<unsigned,unsigned> data2){
	if(data1.size() == 0 || data2.size() == 0){
		//return -1.0;
		return 0.0;
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

	const double multiplier[windowSize+1] = {
		1.000, 0.9938, 0.9772, 0.9332, 0.8413, 0.6915
	};
	
	/*
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
	
	printf("Data 1:\n");
	for(iMap = data1.begin(); iMap != data1.end(); iMap++)
		printf("%d-bit - %d\n", iMap->first, iMap->second);
	printf("Data 2:\n");
	for(iMap = data2.begin(); iMap != data2.end(); iMap++)
		printf("%d-bit - %d\n", iMap->first, iMap->second);
	 */

	//Count the number of bits that are the same
	for(iMap = data1.begin(); iMap != data1.end(); iMap++){
		iTemp = data2.find(iMap->first);	
		//printf("Looking for %d in data1...", iMap->first);

		if(iTemp != data2.end()){
			//printf("Both data has: %d\n", iTemp->first);
			double ratio;
			if(iMap->second > iTemp->second){
				ratio = ((double)iTemp->second / (double)iMap->second);
				marked2.insert(iTemp->first);
				iMap->second = iMap->second - iTemp->second;
				//printf("data 1- %d: has %d left unmapped\n", iMap->first, iMap->second);
			}
			else if(iTemp->second >  iMap->second){
				ratio = ((double)iMap->second / (double)iTemp->second);
				marked1.insert(iMap->first);
				iTemp->second = iTemp->second - iMap->second;
				//printf("data 2- %d: has %d left unmapped\n", iTemp->first, iTemp->second);
			}
			else{
				//printf("EQUAL COUNTS\n");
				ratio = 1.000;
				marked1.insert(iMap->first);
				marked2.insert(iTemp->first);
			}
			N_f1f2_ratio += ratio;
		}
		//else printf("\n");
	}

	//Go through the vector again and try and find similar matches in the bits (WINDOWING)
	//printf("\nWindowing on data1:\n");
	for(iMap = data1.begin(); iMap != data1.end(); iMap++){
		//printf("Checking %d\n", iMap->first);
		if(marked1.find(iMap->first) == marked1.end()){
			//printf("*Not Marked\n");
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
			


			if(iMap->second > iTemp->second){
				marked2.insert(iTemp->first);
				//printf("data 1- %d: has %d left unmapped\n", iMap->first, iMap->second-iTemp->second);
				ratio = ((double)iTemp->second / (double)iMap->second);
				iMap->second = iMap->second - iTemp->second;
			}
			else if(iTemp->second >  iMap->second){
				marked1.insert(iMap->first);
				//printf("data 2- %d: has %d left unmapped\n", iTemp->first, iTemp->second-iMap->second);
				ratio = ((double)iMap->second / (double)iTemp->second);
				iTemp->second = iTemp->second - iMap->second;
			}
			else{
				ratio = 1.000;
				//printf("EQUAL COUNTS\n");
				marked1.insert(iMap->first);
				marked2.insert(iTemp->first);
			}




				ratio = multiplier[minDistance1]*ratio;// * (1.0 - ratio);

				N_f1f2_ratio += ratio;
				marked1.insert(iMap->first);
				marked2.insert(iTemp->first);
			}
		}
		//printf("\n");
	}

	double denom = (N_f1+N_f2-N_f1f2_ratio);
	if(denom == 0.0)	return 0.0;

	return N_f1f2_ratio / denom;
}



double SIMILARITY::euclideanDistanceWNorm(std::vector<double>& f1, std::vector<double>&f2){
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


double SIMILARITY::euclideanDistance(std::vector<double>& f1, std::vector<double>&f2){
	assert(f1.size() == f2.size());

	double distance = 0.0;
	for(unsigned int i = 0; i < f1.size(); i++){
		double diff = (f1[i] - f2[i]);
		distance += (diff * diff) ;
	}

	return sqrt(distance);

}

unsigned SIMILARITY::hammingDistance(unsigned long func1, unsigned long func2){
	unsigned hamming = 0;
	unsigned long xnor = func1 ^ func2;
	xnor = ~xnor;

	unsigned long mask = 0x1;
	for(unsigned int i = 0; i < 64; i++)
		if((mask & xnor) > 0)
			hamming++;

	return hamming;
}








/*
	 double SIMILARITY::tanimoto2(std::vector<unsigned>& f1, std::vector<unsigned>&f2){
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
void getMuxFingerprint_naive(std::map<unsigned, std::map<unsigned, unsigned> >& muxes, std::vector<unsigned long long>& fingerprint){
printf("[SIMILARITY] -- Extracting fingerprint for Multiplexors\n");

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
printf("[SIMILARITY] -- Extracting fingerprint for Register\n");

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
*/

