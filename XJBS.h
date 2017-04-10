#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <vector>
#include "Topology.h"

using namespace std;

class XJBS{
public:
	vector<int> drop_repetition(Topology T, vector<int> pos, double start);
	vector<int> merge_small_cluster(Topology T, vector<int> pos, int nodeCount);
	void scanConKNeibor(Topology T, int id, int k, vector<int> &neibor);
};