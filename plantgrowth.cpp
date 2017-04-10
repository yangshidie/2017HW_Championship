#include"plantgrowth.h"
using namespace std;

vector<vector<int>> PG::getMinCost(vector<int> nodes, int &mincost, int time_threshold,	unordered_map<int, vector<int>> cmap)
{
	//	cout<<endl;
//	cout<<"--------------------------In PG--------------------"<<endl;

	vector<branchInfo> xin;
	int chosen;
	branchInfo curinfo;
	int iter=0;
	init(nodes);
	bool flag=1;
	chosen=0;
	vector<branchInfo> lastcur;
	do{
		stop = clock();

		printf("%fs\t%d\t%d\t%d", (double)(stop-start)/CLOCKS_PER_SEC, iter, fmin, finit);
		cout<<"\t"<<cur.size()<<"\t"<<xin.size()<<"\t"<<res[0].size()<<endl;
		if(((double)(stop-start))/CLOCKS_PER_SEC >time_threshold){
			cout<< ((double)(stop-start))/CLOCKS_PER_SEC << "s\n\n\n" << endl;
			break;			
		}	

		
		xin.clear();

		if(cur.size()!=0 &&flag)
		{
			curinfo=cur[chosen];
			xin=setnewbrgroup(curinfo.nodelist,cmap);
			lastcur=cur;
		}
		else
		{
			if(flag)
			{
				flag=0;
				cur=lastcur;
			}
			curinfo=cur[chosen];			
			cout<<"in setn"<<endl;
			xin=setngroup(curinfo.nodelist,cmap);
		}
		stop = clock();
//		cout<<"newgroup time:"<<(double)(stop-start)/CLOCKS_PER_SEC<<endl;
		cur.erase(cur.begin()+chosen);
		cout<<"xin.size()="<<xin.size()<<endl;
		if(xin.size()==0)
		{		
			if (cur.size() == 0)
			{
				if(flag==1)
				{
					cout<<"before flag"<<endl;
					continue;
				}
				else
					break;
			}
			curPro(cur);
			curbase(cur);
			chosen=chooseNewbase(cur);
			continue;
		}

		curPro(xin);                          //recalculate xin pro 
		cur.clear();
	 	for(size_t i=0;i<xin.size();i++)
		{
			cur.push_back(xin[i]);
		}

		clear(cur);	
		curPro(cur);
		curbase(cur);
		chosen = chooseNewbase(cur);
		iter++;

	}while(cur.size()!=0||flag);
//	cout<<"cursize="<<cur.size()<<endl;
	vector<vector<int>> path;
	vector<int> exist;
	int flowcost=T.minCostFlow(res[0],path,exist);
	mincost=flowcost+exist.size()*T.GetServerCost(); 
	cout<<"nsize=\t"<<nodes.size()<<endl;
	cout<<"rsize=\t"<<res[0].size()<<endl;
	cout<<"exist=\t"<<exist.size()<<endl;
	cout<<"SCost=\t"<<T.GetServerCost()<<endl;
	cout<<"CostS=\t"<<exist.size()*T.GetServerCost()<<endl; 
	cout<<"fcost=\t"<<flowcost<<endl;
	cout<<"fmin= \t"<<fmin<<endl;
	cout<<"--------------------------Out PG--------------------"<<endl;
	return path;
}

int findid(vector<int> node, int val)
{
	for(size_t i=0;i<node.size();i++)
		if(node[i]==val)
			return i;
	return -1;
}



vector<int> intersect(vector<int> a, vector<int> b)
 {	vector<int> intersection;
	 if (a[0] > b.back() || b[0] > a.back())
	{
     	return intersection;
    }

	int num=a.size()>b.size()?a.size():b.size();
	intersection.resize(num);
	int offset = 0;
	for (int i = 0, s = i; i < a.size() && s < b.size(); i++) 
	{
		while (a[i] > b[s])
		{
			s++;
		}
		if (a[i] == b[s]) 
		{
			intersection[offset++] = b[s++];
		}
		while (i < (a.size() - 1) && a[i] == a[i + 1]) 
		{
        	i++;
        }
	}
	if (intersection.size() == offset) 
	{
 		return intersection;
    }
	vector<int> duplicate;
	for(size_t i=0;i<offset;i++)
	{
		duplicate.push_back(intersection[i]);
	}
      return duplicate;
}

vector<branchInfo> PG::setnewbrgroup(vector<int> node,	unordered_map<int, vector<int>> cm)
{		
/*
sort(node.begin(),node.end());
for(size_t i=0;i<node.size();i++)
{
	if(i%10==0)
		cout<<endl;
	cout<<node[i]<<"\t";
}*/
	vector<branchInfo> group;
	vector<int> newx;
	vector<int> curn;
	branchInfo br;
	vector<int> insect;
	int min=INT_MAX;
	
	vector<int> mpi;
	vector<int> mpj;
	vector<int> mpins;
	for(size_t i=0;i<node.size();i++)
	{
		mpi=cm[node[i]];
		curn=T.GetNeiborNode(node[i]);	
//cout<<"curn.size()"<<endl;
//for(size_t k=0;k<curn.size();k++)
//	cout<<curn[k]<<"\t";
//	cout<<endl;	
		if(curn.size()>0)
			insect=intersect(curn,node);
		else
			continue;
		if(insect.size()==0)
			continue;
//cout<<"ins.size()"<<endl;
//for(size_t k=0;k<insect.size();k++)
//	cout<<insect[k]<<"\t";
//	cout<<endl;	
		for(size_t j=0;j<insect.size();j++)	
		{
			mpj=cm[insect[j]];
			mpins=intersect(mpi,mpj);
			if(mpins.size()!=0)
			{	
				continue;
			}
			newx=node;
			int location=findid(newx,insect[j]);
			newx.erase(location+newx.begin());	//就删除任意一
			if(!hastested(newx)) 
			{
				br= lessthanfmin(newx);
				if(br.funValue!=-1)
				{
					group.push_back(br);
				}
			}
			newx=node;
			newx.erase(i+newx.begin());
			if(!hastested(newx))
			{	
				br= lessthanfmin(newx);
				if(br.funValue!=-1)
				{
					group.push_back(br);
				}	
			}		
		}
	}
	return group;
}


vector<branchInfo> PG::setngroup(vector<int> node,	unordered_map<int, vector<int>> cluster_map)
{		
	vector<branchInfo> group;
	vector<int> newx;
	vector<int> curn;
	branchInfo br;
	for(size_t i=0;i<node.size();i++)
	{
		curn=T.GetNeiborNode(node[i]);	
		for(size_t j=0;j<curn.size();j++)	
		{
			vector<int>::iterator loc=find(node.begin(),node.end(),curn[j]);
			if(loc==node.end())   
			{	
				newx=node;
				newx[i]=curn[j];
				if(!hastested(newx))
				{
					br= lessthanfmin(newx);
					if(br.funValue!=-1)
						group.push_back(br);
				}
			}
			else	
			{

				newx=node;
				int location=loc-node.begin();
				newx.erase(location+newx.begin());	//就删除任意一
				if(!hastested(newx)) 
				{
					br= lessthanfmin(newx);
					if(br.funValue!=-1)
					{
						group.push_back(br);
						cout<<"In else!"<<br.funValue<<endl;		
					}
				}
				newx=node;
				newx.erase(i+newx.begin());
				if(!hastested(newx))
				{	
					br= lessthanfmin(newx);
					if(br.funValue!=-1)
					{
						cout<<"In else!"<<br.funValue<<endl;
						group.push_back(br);
					}
				}		
			}
		}
	}
	return group;
}


branchInfo PG::lessthanfmin(vector<int> node)
{
	branchInfo xin;
	int s=havepath(node);
	if(s==-1)
	{	
		xin.funValue=-1;
		return xin;
	}
	xin.nodelist=node;
	xin.funValue=s;
	return xin;
}


int PG::havepath(vector<int> nodes)      
{
	vector<int> ex;
	int s=T.minCostFlow(nodes, ex);
	if(s>=finit)
	{
		return -1;
	}
	s+=T.GetServerCost()*ex.size();
	if(s>=finit)
	{
		return -1;
	}
	if(s<=fmin)
	{
		if(s<fmin)
		{
			fmin=s;
			res.clear();
		}	
		res.push_back(ex);
		return s;
	}
	
	return -1;
}


void PG::init(vector<int> nodes)
{	
	tested.push_back(nodes);
	branchInfo x;
	x.nodelist=nodes;
	x.base=1;
	vector<int> ex;
	vector<int> ex1;
	int ls=T.minCostFlow(nodes, ex);
	int ls1=T.minCostFlow(ex, ex1);	
//	cout<<"node.size()="<<nodes.size()<<"\tex.size()="<<ex.size()<<"\tex1.size()="<<ex1.size()<<endl;
//	cout<<"-----------ex-----------------"<<endl;
//	sort(ex.begin(),ex.end());
//	for(size_t i=0;i<ex.size();i++)
//	{
//		if(i%10==0)
//			cout<<endl;
//		cout<<ex[i]<<"\t";
//	}
//	cout<<endl;
//	sort(ex1.begin(),ex1.end());
//	cout<<"-------------ex1---------------"<<endl;
//	for(size_t i=0;i<ex1.size();i++)
//	{
//		if(i%10==0)
//			cout<<endl;
//		cout<<ex1[i]<<"\t";
//	}
//	cout<<endl;
	cout<<"ls="<<ls<<"\tls1="<<ls1<<endl;	
	x.funValue=ls;
	int  scost=T.GetServerCost();
	for(size_t i=0;i<nodes.size();i++)
	x.funValue+=scost;
	x.pro=1;
	cur.push_back(x);
	fmin = x.funValue;
	finit = x.funValue;
//	cout<<"finit="<<finit<<endl;

	res.push_back(nodes);
}


int PG::chooseNewbase(vector<branchInfo>  brIn)     		//根据随机数选取新的基点
{
	if (brIn.size() == 1)
		return 0;
	float ran;
	srand((unsigned int)time(NULL));
	ran = (float)rand() / RAND_MAX;
	int res=0;
	for (size_t i = 1; i < brIn.size(); i++)
	{
		if (ran>brIn[i - 1].base&&ran <= brIn[i].base)
		{
			return i;
//			res=i;
//			break;
		}
	}

	return res;
}


void PG::curbase(vector<branchInfo> &brIn)
{
	float c = 0;
	for (size_t i = 0; i < brIn.size(); i++)
	{
		c += brIn[i].pro;
		brIn[i].base = c;
	}

}

void PG::curPro(vector<branchInfo> &brIn)
{
	if (brIn.size() == 1)
	{
		brIn[0].pro = 1;
		return;
	}		
	vector<int > drt;
	for (size_t i = 0; i < brIn.size(); i++)
	{
		if (brIn[i].funValue>finit)
		{
			drt.push_back(0);
			continue;
		}
		drt.push_back(finit - brIn[i].funValue);
	}
	int sum=0;
	for (size_t i = 0; i < drt.size(); i++)
	{
		sum += drt[i];
	}
	for (size_t i = 0; i < brIn.size(); i++)
		brIn[i].pro = (float)drt[i] / (float)sum;
//	for(size_t i=0;i<brIn.size();i++)
//	{
//		cout<<endl<<i<<"\t"<<brIn[i].base;
//	}
//	cout<<endl;

}


bool findvec(vector<vector<int>> input, vector<int> vec)
{
	for(size_t i=0;i<input.size();i++)
	{
		if(input[i]==vec)
			return true;
	}
	return false;
}

bool PG::hastested(vector<int> nodes)
{
	sort(nodes.begin(),nodes.end());
	if(findvec(tested,nodes))
		return true;
	tested.push_back(nodes);
	return false;
}



void PG::clear(vector<branchInfo> &brIn)
{
	for (size_t i = 0; i < brIn.size(); i++)
	{
		if (brIn[i].pro == 0)
		{
			brIn.erase(brIn.begin() + i);
		}
	}
}


vector<int> PG::getexistnodes()
{
	vector<int> exist;
	for(size_t i=0;i<res[0].size();i++)
		exist.push_back(res[0][i]);
	sort(exist.begin(),exist.end());
	return exist;
}


