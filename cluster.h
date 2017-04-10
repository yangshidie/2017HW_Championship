#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include "Topology.h"

class cluster
{
	private:
		vector<Topology> _clusters;
		int _mClusters;
		vector<vector<int> > _consumeIndex;
		vector<vector<int> > _nodeIndex;
		GraphLinkedTable table;
		static bool _consumeNode[MAX_NODE_NUM];
		void _init();
	public:
		cluster(GraphLinkedTable& vtable) {
			table = vtable;
			_mClusters = 0;
		}
		void split(int n);	
		cluster& setClustersNumber(int n);
		vector<Topology> getClusters();
		int getClustersNumber();
		vector<int> getClusterNode(int k);
		vector<int> getClustersConsume(int k);
};
#endif
