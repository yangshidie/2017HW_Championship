#include "Topology.h"
int Topology::gPre[MAX_NODE_NUM];
int Topology::gDist[MAX_NODE_NUM];
int Topology::gPath[MAX_NODE_NUM];
bool Topology::visitd[MAX_NODE_NUM];
int Topology::in[MAX_NODE_NUM];

/**
 * @brief _init 
 *
 * @param {vector<int>} deploy
 */
void Topology::_init(vector<int> deploy)
{
	_virtualSource = _nNode;
	_virtualSink = _nNode + 1;
	for (size_t i = 0; i < deploy.size(); i++) {  //add super to source
		InsertEdgev_v(_virtualSource, deploy[i], INT_MAX, 0);
		InsertEdgev_v(deploy[i], _virtualSource, 0, 0);
	}
	vector<v_consum> consumers = getConsume();
	//for (int i = 0; i < GetCNum(); i++) {  //增加消费节点到虚拟终点的链路
		for (size_t i = 0; i < consumers.size(); i++) {  //add sink to super
			if (consumers[i].con_id != -1) {
				int nearId = consumers[i].v_id;
				int bandwith = consumers[i].demand;
				//int nearId = GetConLinkNode(i);
				//int bandwith = GetConDemand(i);
				InsertEdgev_v(nearId, _virtualSink, bandwith, 0);
				InsertEdgev_v(_virtualSink, nearId, 0, 0);
			}
		}
}

/**
 * @brief _reset 
 *
 * @param {vector<int>} deploy
 */
void Topology::_reset(vector<int> deploy)
{
	gHead[_virtualSource] = -1;
	gHead[_virtualSink] = -1;
	vec_edge.assign(gEdge.begin(), gEdge.end());
	for (size_t i = 0; i < deploy.size(); i++) {
		while (gHead[deploy[i]] >= _originEdgeNums) {
			gHead[deploy[i]] = vec_edge[gHead[deploy[i]]].next;
		}
	}
	vector<v_consum> consumers = getConsume();
	//for (int i = 0; i < GetCNum(); i++) {  //增加消费节点到虚拟终点的链路
	for (size_t i = 0; i < consumers.size(); i++) {  
		if (consumers[i].con_id != -1) {
			int nearId = consumers[i].v_id;
			//int nearId = GetConLinkNode(i);
			while (gHead[nearId] >= _originEdgeNums) {
				gHead[nearId] = vec_edge[gHead[nearId]].next;
			}
		}
	}
	gEdgeCount = _originEdgeNums;
}

/**
 * @brief _spfa_BFS
 *
 * @param {interge} s
 * @param {interge} t
 *
 * @return {boolean}
 */
bool Topology::_spfa_BFS(int s, int t) 
{
	memset(gPre, -1, sizeof(int) * MAX_NODE_NUM);
	memset(in, 0, sizeof(int) * MAX_NODE_NUM);
	memset(gDist, 0x7F, sizeof(int) * MAX_NODE_NUM);
	memset(visitd, false, sizeof(visitd));
	gDist[s] = 0;
	queue<int> Q;
	Q.push(s);
	in[s]++;
	visitd[s] = true;
	int count = 0;
	//cout<<"s:"<<s<<endl;
	while (!Q.empty()) {
		int u = Q.front();
		Q.pop();
		for (int e = gHead[u]; e != -1; e = vec_edge[e].next) {
			int v = vec_edge[e].out;
			if (vec_edge[e].bandwith > 0 && gDist[u] + vec_edge[e].cost < gDist[v]) {
				gDist[v] = gDist[u] + vec_edge[e].cost;
				gPre[v] = u; 
				gPath[v] = e; 
				if (!visitd[v]) {
					Q.push(v);
					visitd[v] = true;
					//in[v]++;
					//if (in[v] > _nNode)
					  //return false;
				}
			}
		}
		visitd[u] = false;
	}
	if (gPre[t] == -1) {
		return false;
	}
	return true;
}

/**
 * @brief _minCostFlow 
 *
 * @param {interge} s
 * @param {interge} t
 *
 * @return {interge}
 */
int Topology::_minCostFlow(int s, int t) 
{
	int cost = 0;
	int flow = 0;
	while (_spfa_BFS(s, t)) {
		int f = INT_MAX;
		for (int u = t; u != s; u = gPre[u]) {
			if (vec_edge[gPath[u]].bandwith  < f) {
				f = vec_edge[gPath[u]].bandwith;
			}
		}
		flow += f;
		cost += gDist[t] * f;
		//vector<int> temp;
		for (int u = t; u != s; u = gPre[u]) {
			//temp.push_back(u);
			vec_edge[gPath[u]].bandwith -= f;   
			vec_edge[gPath[u]^1].bandwith += f; 
		}

		//if (f < INT_MAX) {
			//reverse(temp.begin(), temp.end());
			//temp.pop_back();
			//int consume = GetNetLinkNode(temp.back());
			//temp.push_back(consume);
			//temp.push_back(f);
			//path.push_back(temp);
		//}
	}
	if (flow < _maxDemand) {
		cost = 0;
	} else if (cost == 0 && flow == _maxDemand) {
		cost = -1;
	}
	return cost;
}

/**
 * @brief _dfs 
 *
 * @param {interge} s
 * @param {interge} t
 * @param {vector<int>} route
 * @param {interge} f
 * @param {vector<vector<int>>} routes
 *
 * @return {boolean}
 */
bool Topology::_dfs(int s, int t, vector<int> route, int f, vector<vector<int>>& routes)
{
	if (s == t) {
		vector<int> temp;
		int consume = GetNetLinkNode(vec_edge[route[route.size() - 2]].out);
		for (size_t k = 0; k < route.size() - 1; k++) {
			//cout<<vec_edge[route[k]].out<<" ";
			temp.push_back(vec_edge[route[k]].out);
		}
		//cout<<consume<<" "<<f<<endl;
		temp.push_back(consume);
		temp.push_back(f);
		routes.push_back(temp);
		for (size_t k = 0; k < route.size(); k++) {
			vec_edge[route[k] ^ 1].bandwith -= f;
		}
	}
	visitd[s] = true;
	int prev = f;
	for (int e = gHead[s]; e != -1; e = vec_edge[e].next) {
		if (!(e % 2)) {
			int out = vec_edge[e].out;
			if (!visitd[out] && vec_edge[e ^ 1].bandwith > 0) {
				f = min(vec_edge[e ^ 1].bandwith, f);
				route.push_back(e);
				visitd[out] = true;
				_dfs(out, t, route, f, routes);
				visitd[out] = false;
				route.pop_back();
				f = prev;
				for (size_t i = 0; i < route.size(); i++) {
					f = min(f, vec_edge[route[i] ^ 1].bandwith);
				}
			}
		}
	}
	return true;
}

/**
 * @brief _route 
 *
 * @param {interge} s
 * @param {interge} t
 *
 * @return {vector<vector<int>>}
 */
vector<vector<int>> Topology::_route(int s, int t)
{
	vector<int> route;
	vector<vector<int>> routes;
	memset(visitd, false, sizeof(visitd));
	_dfs(s, t, route, INT_MAX, routes);
	return routes;
}

/**
 * @brief init 
 */
void Topology::init()
{
	gEdge.assign(vec_edge.begin(), vec_edge.end());
	_originEdgeNums = gEdge.size();
	vector<v_consum> consumers = getConsume();
	_nNode = GetVNum();
	for (size_t i = 0; i < consumers.size(); i++) {
		if (consumers[i].con_id != -1) {
			_maxDemand += consumers[i].demand;
		}
	}
}

/**
 * @brief minCostFlow 
 *
 * @param {vector<int>} deploy
 * @param {vector<vector<int>>} path
 * @param {vector<int>} exist
 *
 * @return {interge}
 */
int Topology::minCostFlow(vector<int> deploy, vector<vector<int>>& path, vector<int>& exist)
{
	_init(deploy);
	if (_virtualSource == -1 || _virtualSink == -1) {
		cout<<"Must init() first"<<endl;
		return -1;
	}
	int cost = _minCostFlow(_virtualSource, _virtualSink);
	path = _route(_virtualSource, _virtualSink);
	unordered_map<int, int> existed;
	for (size_t i = 0; i < path.size(); i++) {
		if (existed.count(path[i][0]) == 0) {
			existed[path[i][0]]  = 1;
			exist.push_back(path[i][0]);
		}
	}
	_reset(deploy);
	if (cost == 0) {
		exist.resize(0);
		path.resize(0);
		return INT_MAX;
	} else if (cost == -1) {
		return 0;
	} else {
		return cost;
	}
}

/**
 * @brief minCostFlow 
 *
 * @param {vector<int>} deploy
 * @param {vector<int>} existed
 *
 * @return {interge}
 */
int Topology::minCostFlow(vector<int> deploy, vector<int>& existed)
{
	_init(deploy);
	if (_virtualSource == -1 || _virtualSink == -1) {
		cout<<"Must init() first"<<endl;
		return -1;
	}
	int cost = 0;
	int flow = 0;
	int s = _virtualSource;
	int t = _virtualSink;
	while (_spfa_BFS(s, t)) {
		int f = INT_MAX;
		for (int u = t; u != s; u = gPre[u]) {
			if (vec_edge[gPath[u]].bandwith  < f) {
				f = vec_edge[gPath[u]].bandwith;
			}
		}
		flow += f;
		cost += gDist[t] * f;
		for (int u = t; u != s; u = gPre[u]) {
			vec_edge[gPath[u]].bandwith -= f;   
			vec_edge[gPath[u]^1].bandwith += f; 
		}
	}
	for (int e = gHead[s]; e != -1; e = vec_edge[e].next) {
		if (vec_edge[e ^ 1].bandwith != 0) {
			existed.push_back(vec_edge[e].out);
		}
	}
	if (flow < _maxDemand) {
		cost = 0;
	} else if (cost == 0 && flow == _maxDemand) {
		cost = -1;
	}
	_reset(deploy);
	if (cost == 0) {
		return INT_MAX;
	} else if (cost == -1) {
		return 0;
	} else {
		return cost;
	}
}
