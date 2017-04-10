#include <string>
#include <math.h>
#include "geneticAL.h"
#include <algorithm>


using namespace std;



bool comparison(PositionGene a, PositionGene b)
{
	return a.fit<b.fit;
}
/*void GA::setEvpop(vector<int> init_vec, int fit){

	for(int i = 0; i < init_vec.size(); i++){
		current[j].bit[init_vec[i]] = 1;
	}
	vector<vector<int>> route;
	vector<int> exists;
	current[0].fit= calCost(decode(current[j]), current[j]);

	hashTemp[init_vec] = init_vec;
	hashTemp[init_vec] = init_vec;
	hashFit[init_vec] = current[0].fit;
}*/
vector<vector<int>> GA::getBestServersPos()
{
	PositionGene* current = new struct PositionGene[maxPosNum];
	int i ;                                     
	vector<int> k;	
	//cout << "start" << endl;
//	cout << "程序开始于:" << ((double)(stop-start))/CLOCKS_PER_SEC << "s" << endl;
	int MaxCount = 400;
	int minCount = 300;
	int count = 0;
	srand(time(0));
	evpop(current);	
	sort(current, current+initNum, comparison);
	minCostValue = current[0].fit;
	//cout << "evpop" << endl;
	//Min = current[0].fit;

	//T.PrintConNodeInfo();
	for(i =0;i< this->iteration;i++)                        
	{            
		stop = clock();
		if(((double)(stop-start))/CLOCKS_PER_SEC > 89){
	//		cout << "程序耗时:" << ((double)(stop-start))/CLOCKS_PER_SEC << "s\n\n\n" << endl;
			break;			
		}	
		//	cout << i << "min is " << Min << endl;  
		//printf("%fs\t%d\t%d\t%d\t%d\t\n", (double)(stop-start)/CLOCKS_PER_SEC, i, minCostValue, initNum, current[initNum-1].fit);
		crossoverMethod(current, i);
		mutationMethod(current, i);                
		sort(current, current+initNum, comparison);

		if(current[0].fit == minCostValue){
			count++;	
		}
		else if(current[0].fit < minCostValue){
			
			minCostValue = current[0].fit;
			count = 0;
		}
		
		if(count > MaxCount && i > minCount) break;
	}  
	//cout << "i is " << i << "cross1 " << cross1 << "\tcross2 " << cross2 << "\tmu1 " << mu1 << "\tmu2 " << mu2 << endl;
	//sort(current, current+initNum, comparison);
	k = decode(current[0]);

	vector<vector<int>> route;
	//vector<vector<int>>& path = route;
	if( current[0].fit == zhilian_cost){
		cost = zhilian_cost;
		exists = T.GetAllConLinkNode();
		return route;
	}
	else{
		T.minCostFlow(k, exists); 
		//cout << "minCost" <<route.size() <<  endl;
		cost = calCost(k, current[0]);
	//	cout << "程序结束于:" << ((double)(stop-start))/CLOCKS_PER_SEC << "s" << endl;
	//	cout << "cost is " << cost << "\t \t \tsize is " << exists.size() << endl;
		return route;
	}
	
	


}

void *GA::evpop(PositionGene* current)   
{
	int i ,j;
	vector<int> a = T.GetAllConLinkNode();
	cout << a.size();
	int init_zhilian = 1;
	int init_count = 0;
	vector<int> init_vec;
	for(int j = 0; j < init_zhilian; j++){
		for(int i = 0; i < a.size(); i++){
			if(a[i] != -1){
				current[j + evpopInit].bit[a[i]] = 1;
				init_count++;
				init_vec.push_back(a[i]);
			}
		}
		vector<vector<int>> route;
		vector<int> exists;
		current[j + evpopInit].fit= init_vec.size() * T.GetServerCost();

		hashTemp[init_vec] = init_vec;
		hashTemp[init_vec] = init_vec;
		hashFit[init_vec] = current[j + evpopInit].fit;
	}
	zhilian_cost = current[evpopInit].fit;
//	cout << "init_zhilian fit is " << current[evpopInit].fit << endl;
	int N = 400;
	double random = rand()%(N+1)/(float)(N+1);
	for(j =init_zhilian;j<initNum; j++)                           
	{	
		
		generateGene:
		for(i =0;i<this->nodeNum; i++)                       
		{          
			random = rand()%(N+1)/(float)(N+1);
			current[j].bit[i]= (random < 0.8)?true:false;       //
		}  

		if(hashTemp.find(decode(current[j])) != hashTemp.end()){
			goto generateGene;
		}

		current[j].fit= calCost(decode(current[j]), current[j]);
		//cout << "j is " << j << "\t fit is " << current[j].fit << endl;
		if(current[j].fit < minCostValue) {
			memcpy(maxPos, current[j].bit, nodeNum);
			minCostValue = current[j].fit;
		}
	}
	
	return(0);                
}            

vector<int> GA::decode(PositionGene current)  
{

	vector<int> selectNode;
	for(int i = 0; i < this->nodeNum; i++){
		if(current.bit[i]){
			selectNode.push_back(nodeIdHash[i]);
		}
	}
	return selectNode;                           
}                                     

void GA::code(vector<int> num, PositionGene& current){
	memset(current.bit, 0, sizeof(short int) * this->nodeNum);
	for(int i = 0; i < num.size(); i++){
		current.bit[idNodeHash[num[i]]] = true;
	}
}
int GA::calCost(vector<int> x, PositionGene& current)
{

	if(x.empty()){
		return INIT_Min;
	}
	int cost = INIT_Min;
	vector<int> exists;
	if(hashTemp.find(x) == hashTemp.end()) {	
		
		cost = T.minCostFlow(x, exists);
		if(cost != INIT_Min){
			cost += T.GetServerCost() * exists.size();
			code(exists, current);
		}
		hashTemp[x] = exists;
		hashTemp[exists] = exists;
		hashFit[exists] = cost;

	}
	else  {

		cost = hashFit[hashTemp[x]];
	}
	return(cost);            
} 

                                   

vector<int> GA::rouletteSelect(PositionGene* current, int count, int start, int end){
	int sum = 0;
	double lastSum = 0;
	double rate;
	int N = 400;
	vector<int> select;

	int allSum = 0;
	for(int i = start; i < end; i++){
		allSum += log(current[i].fit);
	}
	for(int j = 0; j < count; j++){
		double random = rand()%(N+1)/(float)(N+1);
		sum = 0;
		lastSum = 0;
		for(int i = start; i < end; i++){
			sum += 	log(current[i].fit);	
			
			rate = (1.0*sum)/allSum;
			if(random >= lastSum && random < (1.0*sum)/allSum){
				select.push_back(i);
				break;
			}
			
			lastSum = rate;
		}
		
	}
	return select;
}

/**
*	new crossoverMethod based rouletteSelect
**/

void GA::singleMutation(bool* bit){
	int num = rand() % 10;
	int random;
	for(int i = 0; i < num; i++){
		random = rand() % this->nodeNum;
		bit[random]=bit[random]?0:1;
	}
}

void *GA::crossoverMethod (PositionGene* current, int count)             
{
	//double randomRate = rand()%(400+1)/(float)(400+1);
	if( initNum < maxPosNum - 4){
		int crossNum = 3;
		vector<int> select = rouletteSelect(current, 2 * crossNum - 1, 0, initNum/3);
		initNum += 2 * crossNum;
		int index1 = 0;
		int index2 = 0;
		for(int i = 0; i < crossNum - 1; i++){
			int random=(rand () %this->nodeNum);
			index1 = initNum - 2 * i - 1;
			index2 = initNum - 2 * i - 2;
			memcpy(current[index1].bit, current[2*i].bit, sizeof(bool) * random);
			memcpy(current[index1].bit + random, current[select[2 * i + 1]].bit + random,(this->nodeNum - random));
			

			memcpy(current[index2].bit, current[select[2 * i + 1]].bit, random);
			memcpy(current[index2].bit + random, current[2*i].bit + random,(this->nodeNum - random));

			while(hashTemp.find(decode(current[index1])) != hashTemp.end())	singleMutation(current[index1].bit);
			while(hashTemp.find(decode(current[index2])) != hashTemp.end())	singleMutation(current[index2].bit);
			
			
			current[index1].fit = calCost(decode(current[index1]), current[index1]); 
			current[index2].fit = calCost(decode(current[index2]), current[index2]); 
		}

		int random=(rand () %this->nodeNum);
		index1 = initNum - 2 * crossNum + 1;
		index2 = initNum - 2 * crossNum;
		int firstRow = rand() % 5;
		
		memcpy(current[index1].bit, current[firstRow].bit,random);
		memcpy(current[index1].bit + random, current[select[crossNum]].bit + random, (this->nodeNum - random));
		while(hashTemp.find(decode(current[index1])) != hashTemp.end())	singleMutation(current[index1].bit);
		current[index1].fit = calCost(decode(current[index1]), current[index1]); 

		memcpy(current[index2].bit, current[select[crossNum]].bit,random);
		memcpy(current[index2].bit + random, current[firstRow].bit + random,(this->nodeNum - random));
		while(hashTemp.find(decode(current[index2])) != hashTemp.end())	singleMutation(current[index2].bit);
		current[index2].fit = calCost(decode(current[index2]), current[index2]); 
	}
	else{	// 
		
		int random=(rand () %this->nodeNum);
		vector<int> select = rouletteSelect(current, 2, 0, initNum/2);
		struct PositionGene member1, member2;
		memcpy(member1.bit, current[select[0]].bit, random);
		memcpy(member1.bit + random, current[select[1]].bit + random, (this->nodeNum - random));
		while(hashTemp.find(decode(member1)) != hashTemp.end())	singleMutation(member1.bit);
		
		memcpy(member2.bit, current[select[1]].bit,random);
		memcpy(member2.bit + random, current[select[0]].bit + random, (this->nodeNum - random));
		while(hashTemp.find(decode(member1)) != hashTemp.end())	singleMutation(member2.bit);

		member1.fit = calCost(decode(member1), member1); 
		member2.fit = calCost(decode(member2), member2); 
		if(member1.fit < minCostValue || member2.fit < minCostValue) cross1++;
		
		current[initNum - 1]= member1;
		current[initNum - 2]= member2; 

		random=(rand () %this->nodeNum);
		int first = rand() % 10;
		select = rouletteSelect(current, 1, initNum/3, initNum/2);
		int index1 = first;
		int index2 = select[0];
		memcpy(member1.bit, current[index1].bit, random);
		memcpy(member1.bit + random, current[index2].bit + random,(this->nodeNum - random));
		while(hashTemp.find(decode(member1)) != hashTemp.end())	singleMutation(member1.bit);
		
		memcpy(member2.bit, current[index2].bit, random);
		memcpy(member2.bit + random, current[index1].bit + random,  (this->nodeNum - random));
		while(hashTemp.find(decode(member2)) != hashTemp.end())	singleMutation(member2.bit);

		member1.fit = calCost(decode(member1), member1); 
		member2.fit = calCost(decode(member2), member2); 
		if(member1.fit < minCostValue || member2.fit < minCostValue) cross2++;
		current[initNum - 3]= member1;
		current[initNum - 4]= member2; 

		random=(rand () %this->nodeNum);
		first = rand() % 10;
		select = rouletteSelect(current, 1, initNum/2, initNum/1.5);
		index1 = first;
		index2 = select[0];
		memcpy(member1.bit, current[index1].bit, random);
		memcpy(member1.bit + random, current[index2].bit + random,(this->nodeNum - random));
		while(hashTemp.find(decode(member1)) != hashTemp.end())	singleMutation(member1.bit);
		
		memcpy(member2.bit, current[index2].bit, random);
		memcpy(member2.bit + random, current[index1].bit + random,  (this->nodeNum - random));
		while(hashTemp.find(decode(member2)) != hashTemp.end())	singleMutation(member2.bit);

		member1.fit = calCost(decode(member1), member1); 
		member2.fit = calCost(decode(member2), member2); 
		if(member1.fit < minCostValue || member2.fit < minCostValue) mu1++;
		current[initNum - 5]= member1;
		current[initNum - 6]= member2; 
	}
	
    return(0);	
}

/**
*	mutation based on rouletteSelect()
**/
void *GA::mutationMethod (PositionGene* current, int count){
	double randomRate = rand()%(400+1)/(float)(400+1);
	int col;
	/*if(randomRate <  1)                             
	{
		col=rand()%this->nodeNum;                       
		vector<int> select = rouletteSelect(current, 1, 4, initNum);	
		current[select[0]].bit[col] = current[select[0]].bit[col]?0:1;
		while(hashTemp.find(decode(current[select[0]])) != hashTemp.end())	singleMutation(current[select[0]].bit);
		current[select[0]].fit= calCost(decode(current[select[0]]), current[select[0]]);   
		if(current[select[0]].fit < minCostValue) mu1++;
	}*/
	randomRate = rand()%(400+1)/(float)(400+1);
	if(randomRate < 1){
		int row = initNum<maxPosNum?initNum++:initNum - 7;
		vector<int> select = rouletteSelect(current,  1, 0, initNum/5);	
		int mutateNum = rand()%5;
		memcpy(current[row].bit, current[select[0]].bit,this->nodeNum);
		for(int i = 0; i < mutateNum; i++){
			int num = rand() % 5;
			vector<int> select = rouletteSelect(current, num, 0, initNum/4);	
			for(int j = 0; j < num; j++){
				col = rand() % (this->nodeNum);
				current[row].bit[col] = current[row].bit[col]?0:1;
			}
		} 	
		while(hashTemp.find(decode(current[row])) != hashTemp.end())	singleMutation(current[row].bit);
		current[row].fit= calCost(decode(current[row]), current[row]); 
		if(current[row].fit < minCostValue) mu2++;
		
	}

	return(0);
}
