#ifndef _TOPOLOGY_H_
#define _TOPOLOGY_H_
#include "Init.h"
#include <queue>
#include <stack>
#include <limits.h>
#include <algorithm>
#include <unordered_map>
struct Edge {
	int to;
	int vol;
	int cost;
	int next;
};

struct cmp
{
	bool operator()(int x, int y) 
	{
		return x > y;
	}
};

class Topology:public GraphLinkedTable
{
	protected:
		static int gPre[MAX_NODE_NUM];
		static int gDist[MAX_NODE_NUM];
		static int gPath[MAX_NODE_NUM];
		static bool visitd[MAX_NODE_NUM];
		static int in[MAX_NODE_NUM];
	private:
		vector<edgev_v> gEdge;
		int _originEdgeNums;
		int _virtualSource;
		int _virtualSink;
		int _maxDemand;
		bool _spfa_BFS(int s, int t);
		bool _dfs(int s, int t, vector<int> route, int f, vector<vector<int>>& routes);
		int _minCostFlow(int s, int t);
		void _init(vector<int> deploy);
		void _reset(vector<int> deploy);
		int _nNode;
	public:
		Topology() {
			GraphLinkedTable();
			_virtualSource = -1;
			_virtualSink = -1;
			_originEdgeNums = 0;
			_maxDemand = 0;
		}
		void init();
		vector<vector<int>> _route(int s, int t);
		int minCostFlow(vector<int> deploy, vector<vector<int>>& path, vector<int>& exist);
		int minCostFlow(vector<int> deploy, vector<int>& existed);
};
#endif
