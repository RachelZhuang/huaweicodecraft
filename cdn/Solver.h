#pragma once
#include <cstdio>
#include <unordered_map>
#include <signal.h>
#include <unistd.h>


#include "Graph_List.h"
#include "SImplex_Network.h"

#include "BLDE.h"
#include "lib_io.h"
#include "lib_time.h"

using namespace std;
using namespace solution;

class Solver:BLDE
{
public:

	Solver(const int _net_number,const int _path_number, const int _consume_number, const int _deploy_cost, const vector<vector<int>>& _link_cap_cost,const vector<vector<int>>& _source_target_consume);
	~Solver();
	void initNetwork();
	void adaptNetwork(vector<int> video_node_ids);
	void recoverNetwork(int video_node_number);
	vector<int> getHigherDegVideoNodeIDs();
	vector<int> getDirectLinkedNodeIDs();
	int getMinCostFlow(int video_node_number);
	static bool pairSortFun_1(std::pair<int, int> elem1, std::pair<int, int> elem2)
	{
		return elem1.second > elem2.second;//or return elem1.first > elem2.second  
	}
	static bool pairSortFun_2(pair<vector<int>, int> elem1, std::pair<vector<int>, int> elem2)
	{
		return elem1.second< elem2.second;//or return elem1.first > elem2.second  
	}
	

	string run();
	string convertToStandardFormat(const vector<vector<int>> flow_lines);
	vector<int> FindPath(int pathnode);
	vector<int> gettonode(int node);
	void recoverPath(vector<int> path);
	vector<vector<int>> getFinalPath(int video_node_number);

	vector<int> initchroms();
    int Objective(const vector<int> &);
    void timer(int sig);
    vector<int> getAllNodeIDs();

private:
	//����ڵ��������������ѽڵ㡣
	int net_number,path_number,consume_number;//����ڵ�������·�������ѽڵ���
	int deploy_cost;//����ɱ�
	int total_cost;
	vector<vector<int>> link_cap_cost;
	vector<vector<int>> source_target_consume;
	// Create a directed graph
//	DIGRAPH_TYPEDEFS(Graph_List);
    SimplexNetwork ns;
	//Graph_List g;
	vector<Graph_List::Node> nodes;
	Graph_List::Node sourceNode;
	//vector<Graph_List::Edge> 

    vector<int> adaptIDs;
	vector<Graph_List::Edge> edges;//���������䶯����Щ��
	vector<Graph_List::Edge> direct_link_edges;//�����ѵ�ֱ���ı�

	//Graph_List::EdgeMap<int> capacity, cost;
	//Graph_List::NodeMap<int> supply;

	unordered_map<int ,int>  capacity, cost;
	unordered_map<int ,int> supply;

	typedef pair<vector<int>, int> chrom;

	vector<chrom> popcurrent;                        	
	vector<chrom> popnext;

	vector<pair<int, int>> id_degs;
	vector<int> directLinkedNodeIDs;
	int consum_sum=0;//�ܵ���������
	vector<vector<int>> flow;

    bool flag=true;
};

