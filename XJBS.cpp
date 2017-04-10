#include "XJBS.h"



vector<int> XJBS::drop_repetition(Topology T, vector<int> pos, double start){
	bool boolConlink[1000];
	memcpy(boolConlink, boolConlink + 1000, 0);
	vector<int> new_pos = pos;
	
	return new_pos;
}

vector<int> XJBS::merge_small_cluster(Topology T, vector<int> pos, int nodeCount){
	int countConlink[nodeCount];
	memcpy(countConlink, countConlink + nodeCount, 0);
	int pos_size = pos.size();
	for(int i = 0; i < pos_size; i++){
		vector<int> idNeibor;
		 scanConKNeibor(T, pos[i], 3, idNeibor);
		for(int j = 0; j < idNeibor.size(); j++){
			countConlink[idNeibor[i]]++;
		}
	}
	vector<int> possible_pos;
	for(int i = 0; i < nodeCount; i++){
		if(countConlink[i] == pos_size){
			possible_pos.push_back(i);
		}
	}

	if(possible_pos.empty()){
		return pos;
	}
	else{
		int server_cost = T.GetServerCost();
		int min = server_cost;
		vector<int> result = pos;
		vector<vector<int>> path;
		vector<int> exists_buf;
		for(int i = 0; i < possible_pos.size(); i++){
			vector<int> buff_pos;
			buff_pos.push_back(possible_pos[i]);
			int cost = T.minCostFlow(buff_pos, path, exists_buf);
			cout << "****************merge basic cost is " << cost << endl;
			cost += server_cost;
			cout <<"\t\t\t all cost is " << cost << endl;
			if(min > cost){
				min = cost;
				result.swap(buff_pos);
			}
		}
		cout << " ****************** merge min cost is " << min << endl;
		return result;
	}
}
void XJBS::scanConKNeibor(Topology T, int id, int k, vector<int> &neibor){
	if(k==0) return;
	else{
		vector<int> idNeibor = T.GetNeiborNode(id);
		for(int i = 0; i < idNeibor.size(); i++){
			if(find(neibor.begin(), neibor.end(), idNeibor[i]) == neibor.end())
				neibor.push_back(idNeibor[i]);
			scanConKNeibor(T, idNeibor[i], k-1, neibor);
		}
	}
}