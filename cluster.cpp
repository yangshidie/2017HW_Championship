#include "cluster.h"
bool cluster::_consumeNode[MAX_NODE_NUM];

/**
 * @brief _init 
 */
void cluster::_init()
{
	memset(_consumeNode, false, sizeof(_consumeNode));
}

/**
 * @brief split 
 *
 * @param {Interge} n
 */
void cluster::split(int n)
{
	_init();
	if (n <= 1) {
		return;
	}
	int nConsume = table.GetCNum();
	int costPerServer = table.GetServerCost();
	bool visited_Node[MAX_NODE_NUM];
	int consumeNodeIndexPos = rand() % nConsume;
	while (true) {
		vector<int> nodeIndex;
		vector<int> consumeIndex;
		unordered_map<int, int> nodeExisted;
		memset(visited_Node, false, sizeof(visited_Node));
		Topology* pTopo = new Topology();
		int neighborId = -1;
		pTopo->SetGraphPara(table.GetVNum(), 0, table.GetCNum(), costPerServer);

		if (!_consumeNode[consumeNodeIndexPos]) {
			neighborId = table.GetConLinkNode(consumeNodeIndexPos);
			int demand = table.GetConDemand(consumeNodeIndexPos);
			pTopo->InsertConsume(consumeNodeIndexPos, neighborId, demand);
			_consumeNode[consumeNodeIndexPos] = true;
			consumeIndex.push_back(consumeNodeIndexPos);
		} else {
			bool flag = false;
			for (int i = 0; i < nConsume; i++) {
				if (!_consumeNode[i]) {
					consumeNodeIndexPos = i;
					flag = true;		
					break;
				}
			}
			if (flag) continue;
		}
		
		if (neighborId == -1) {
			break;
		}

		bool nodeFull = false;
		queue<int> q;
		q.push(neighborId);
		nodeIndex.push_back(neighborId);
		nodeExisted[neighborId] = 1;
		while (!nodeFull && q.size() != 0) {
			int u = q.front();
			q.pop();
			visited_Node[u] = true;
			vector<int> bfs = table.GetNeiborNode(u);
			for (size_t k = 0; k < bfs.size(); k++) {
				int consumeId = table.GetNetLinkNode(bfs[k]);
				if (consumeId < nConsume) {
					if (!_consumeNode[consumeId]) {
						int demand = table.GetConDemand(consumeId);
						pTopo->InsertConsume(consumeId, bfs[k], demand);
						_consumeNode[consumeId] = true;
						consumeIndex.push_back(consumeId);
					}
				}
				if (!visited_Node[bfs[k]]) {
					q.push(bfs[k]);
					visited_Node[bfs[k]] = true;
					nodeIndex.push_back(bfs[k]);
					nodeExisted[bfs[k]] = 1;
				}
				if (nodeIndex.size() >= n) {
					nodeFull = true;
				}
			}
		}

		/**consider one consume cluster**/
		if (consumeIndex.size() == 1) {
			nodeIndex.clear();
			int neighborId = table.GetNetLinkNode(consumeIndex[0]);
			nodeIndex.push_back(neighborId);
			_nodeIndex.push_back(nodeIndex);
			_consumeIndex.push_back(consumeIndex);
			_clusters.push_back(*pTopo);
			continue;
		}

		/**consider the nodes in many consume cluster**/
		for (size_t p = 0; p < nodeIndex.size(); p++) {
			vector<int> neighborIds = table.GetNeiborNode(nodeIndex[p]);
			for (size_t w = 0; w < neighborIds.size(); w++) {
				if (nodeExisted.count(neighborIds[w]) != 0) {
					int cost = table.GetEdgecost(nodeIndex[p], neighborIds[w]);
					int bandwidth = table.GetEdgebw(nodeIndex[p], neighborIds[w]);
					pTopo->InsertOneEdge(nodeIndex[p], neighborIds[w], bandwidth, cost);
					pTopo->InsertEdgev_v(nodeIndex[p], neighborIds[w], bandwidth, cost);
				}
			}
		}

		consumeNodeIndexPos = consumeIndex.back();
		_consumeNode[consumeNodeIndexPos] = false;
		consumeIndex.pop_back();

		if (consumeIndex.size() != 0 && nodeIndex.size() != 0) {
			_nodeIndex.push_back(nodeIndex);
			_consumeIndex.push_back(consumeIndex);
			_clusters.push_back(*pTopo);
		}
	}
	_mClusters = _nodeIndex.size();
	_consumeIndex.resize(_mClusters + 1);
	_nodeIndex.resize(_mClusters + 1);

	int count = 0;
	for (size_t i = 0; i < nConsume; i++) {
		if (!_consumeNode[i]) {
			count++;
			cout<<i<<" ";
		}
	}
	cout<<endl;
	cout<<"count:"<<count<<endl;
	//cout<<_clusters.size()<<endl;
	//int k = 0;
	//for (int i = 0; i < _consumeIndex[k].size(); i++) {
		//cout<<_consumeIndex[k][i]<<" ";
		//cout<<_clusters[k].GetConLinkNode(_consumeIndex[k][i])<<endl;
	//}
	//for (int i = 0; i < _nodeIndex[k].size(); i++) {
		 //vector<int> neibor = _clusters[k].GetNeiborNode(_nodeIndex[k][i]);
		 //cout<<"source:"<<_nodeIndex[k][i]<<endl;
		 //for (int j = 0; j < neibor.size(); j++) {
			//cout<<neibor[j]<<endl;
		 //}
	//}
	//cout<<endl;
}


/**
 * @brief setClustersNumber 
 *
 * @param {interge} n
 *
 * @return {cluster}
 */
cluster& cluster::setClustersNumber(int n)
{
	_mClusters = n;
	return *this;
}

/**
 * @brief getClusters 
 *
 * @return {vector<Topolog>}
 */
vector<Topology> cluster::getClusters()
{
	return _clusters;
}

/**
 * @brief getClustersNumber 
 *
 * @return {interge}
 */
int cluster::getClustersNumber()
{
	return _mClusters;
}

/**
 * @brief getClusterNode 
 *
 * @param {interge} k
 *
 * @return vector<int>
 */
vector<int> cluster::getClusterNode(int k)
{
	if (k > _mClusters) {
		return vector<int>();
	}
	return _nodeIndex[k];
}

/**
 * @brief getClustersConsume 
 *
 * @param {interge} k
 *
 * @return {vector<int>}
 */
vector<int> cluster::getClustersConsume(int k)
{
	if (k > _mClusters) {
		return vector<int>();
	}
	return _consumeIndex[k];
}

