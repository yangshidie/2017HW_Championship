#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <vector>
#include "Topology.h"
//#include <boost/functional/hash.hpp>

#define INIT_Min 0x7fffffff

using namespace std;

typedef struct PositionGene{
    bool bit[1000]; 
    int fit;                                   
}PositionGene;

static long long PowPow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000};

struct container_hash {
    std::size_t operator()(const vector<int> & c) const {
	int size = c.size()>10?10:c.size();
	long long key = 0;
	for(int i = 0; i < size; i++){
		key += c[i]*PowPow10[i];
	}
        return key;
    }
};

class GA{
    public:
	Topology T;
        int nodeNum;
        int iteration;
	bool maxPos[1000];
	int minCostValue;
	int initNum;
	int maxPosNum;
	int cross1;
	int cross2;
	int mu1;
	int mu2;
	double start, stop;
	int evpopInit;
	vector<int> nodeIdHash;
	unordered_map<int, int> idNodeHash;
	vector<int> exists;
	int cost;
	int zhilian_cost;
        GA(Topology T, int iteration, int initNum, int maxPosNum, double start, vector<int> nodeIdHash){
        		this->nodeIdHash = nodeIdHash;
        		for(int i = 0; i < nodeIdHash.size(); i++){
        			idNodeHash[nodeIdHash[i]] = i;
        		}
        		this->start = start;
		this->T = T;
		this->nodeNum = nodeIdHash.size();
		cout << "node id hash size is " << nodeIdHash.size();
		this->iteration = iteration;
		this->initNum = initNum;
		this->maxPosNum = maxPosNum;
		minCostValue = INIT_Min;
		maxPosNum = maxPosNum;
		cross1 = 0;
		cross2 = 0;
		mu1 = 0;
		mu2 = 0;
		evpopInit = 0;
		//cout << this->nodeNum << endl;
		//cout << this->iteration << endl;	
	};
	unordered_map<vector<int>, int, container_hash> hashFit;
	unordered_map<vector<int>, vector<int>, container_hash> hashTemp;
        	vector<vector<int>> getBestServersPos();
        	vector<int> getExists(){
        		return exists;
        	};
        	int getCost(){
        		return cost;
        	}
    private:
	int sumCost = 0;
	vector<int> rouletteSelect(PositionGene* current, int count, int start, int end);
	void code(vector<int> num, PositionGene& current);
        void *evpop(PositionGene* popcurrent);         
        vector<int> decode(PositionGene popcurrent);                
        int calCost(vector<int> x, PositionGene& current);
        void *crossoverMethod(PositionGene* popnext, int count);        
        void *mutationMethod(PositionGene* popnext, int count); 
	void singleMutation(bool* bit);                            
};
