#include "deploy.h"
#include "geneticAL.h"
#include "plantgrowth.h"
#include "cluster.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "XJBS.h"

char* vec_to_int(vector<vector<int >> );


void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	XJBS xjbs;
	double start, stop;
	vector<vector<int>> route;
	vector<int> pos;
	int cost;	
	start = clock();
	Topology T;
	T.GraphInit(topo, line_num);
	T.init();

	/*
	int cosume_cluster = T.GetCNum() / 80 + 1;
	int cluster_consume_num = T.GetVNum() / cosume_cluster + T.GetCNum() / 10;
	cout << "cluster_consume_num is " << cluster_consume_num << endl;
	*/
	
	int cluster_consume_num;
	if(T.GetCNum() < 300){
		cluster_consume_num = 160;
	}
	else {
		cluster_consume_num = 180;
	}

	// = T.GetCNum() < 300?160:160;
	int ga_init_num = T.GetCNum() < 300?100:120;//90 + T.GetCNum() / 100 * 10; 	
	cluster* pCluster = new cluster(T);
	pCluster->split(cluster_consume_num);
	vector<Topology> clusters = pCluster->getClusters();
	
	int server_cost = T.GetServerCost();

	int cluster_num = pCluster->getClustersNumber();
//	cout << "cluster_num is " << cluster_num << endl;
	unordered_map<int, vector<int>> cluster_map;

	for(int i = 0; i < cluster_num; i++){
		vector<int> nodeId = pCluster->getClusterNode(i);
		vector<int> consumId = pCluster->getClustersConsume(i);
//		cout << nodeId.size() << endl;



		if(consumId.size() ==1){
			if(cluster_map.find(consumId[0]) == cluster_map.end()){
				vector<int> cluster_vec;
				cluster_vec.push_back(i);
				cluster_map[consumId[0]] = cluster_vec;
			}
			else{
				cluster_map[consumId[0]].push_back(i);
			}
			pos.insert(pos.end(), consumId.begin(), consumId.end());
			continue;
		}

		Topology T1 = clusters[i];
		T1.init();
//		cout << "i is " << i << "\tcluster c num is " << T1.GetCNum() << endl;
//		cout << "nodeid size is " << nodeId.size() << endl;
		GA ga(T1, 5000, ga_init_num, 1.5 * ga_init_num, start, nodeId);
		ga.getBestServersPos();
		int cost_buf = ga.getCost();
		vector<int> exists = ga.getExists();
		/*if(consumId.size() == 2 && cost_buf == consumId.size() * server_cost){
			cout << "merge cluster *********************" << endl;
			exists = xjbs.merge_small_cluster(T1, consumId, nodeId.size());
		}*/
		for(int j = 0; j < exists.size(); j++){
//			cout << consumId[j] << "\t";
			if(cluster_map.find(exists[j]) == cluster_map.end()){
				vector<int> cluster_vec;
				cluster_vec.push_back(i);
				cluster_map[exists[j]] = cluster_vec;
			}
			else{
				cluster_map[exists[j]].push_back(i);
			}
		}
	//	cout << i << "\n\t************************* cost is " << cost_buf << endl;
		pos.insert(pos.end(), exists.begin(), exists.end());
	}

	cout << "cluster_map size is " << cluster_map.size() << endl;

	unordered_map<int,vector<int>>::iterator it;
    	(*it).first;        //the key value
    	(*it).second;   //the mapped value
    	for(unordered_map<int,vector<int>>::iterator iter=cluster_map.begin();iter!=cluster_map.end();iter++)
          		cout<<"k="<<iter->first<<"\tc="<< iter->second[0] << "\ts=" << iter->second.size()<< endl;


	
	vector<vector<int>> path;
	vector<int> exists_buf;
	PG pg(T,start);
	int min = 0;
	cost = T.minCostFlow(pos, path, exists_buf); 
	cout<<"exists_buf"<<endl;
	sort(exists_buf.begin(),exists_buf.end());
	for(size_t i=0;i<exists_buf.size();i++)
	{
		cout<<exists_buf[i]<<"\t";
	}
	cout<<endl;
	int time_threshold = T.GetCNum()<300?88:70;		
	path = pg.getMinCost(exists_buf, min, time_threshold,cluster_map);
	stop = clock();
	cout<<"finit=" << T.GetCNum() * server_cost << endl;
	cout << "forigin= " << cost + exists_buf.size() * server_cost  << endl;
	cout << "plantgrowth= " << min << endl;
	char * topo_file = vec_to_int(path);
	stop = clock();
	cout << "***********************************\nTime :" << ((double)(stop-start))/CLOCKS_PER_SEC << "s" << endl;

	write_result(topo_file, filename);
}



char* vec_to_int(vector<vector<int >> path)
{
	char* in=(char*)malloc(sizeof(int)+1);
	string str;	
	int pi = path.size();
	sprintf(in, "%d", pi);
	str += in;
	str += "\n\n";
	for (size_t i = 0; i< path.size() ;i++)
	{
		for (size_t j = 0; j < path[i].size(); j++)
		{
			pi = path[i][j];
			sprintf(in, "%d", pi);
			str += in;
			if (j != path[i].size() - 1) {
				str += " ";
			}
		}
		if (i != path.size() - 1) {
			str += "\n";
		}
	}
	int len = str.length();
	char *topo = (char *)malloc((len + 1)*sizeof(char));
	strcpy(topo, str.c_str());
	free(in);
	return topo;
};

