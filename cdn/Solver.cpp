#include <iostream>
#include <sys/time.h>
#include "Solver.h"

Solver::Solver(const int _net_number, const int _path_number, const int _consume_number, const int _deploy_cost, const vector<vector<int>>& _link_cap_cost, const vector<vector<int>>& _source_target_consume)
{
	net_number = _net_number;
	path_number =_path_number ;
	consume_number = _consume_number;//�����ڵ�������·�������ѽڵ���
	deploy_cost = _deploy_cost;//�����ɱ�
	total_cost = _deploy_cost*_consume_number;
	link_cap_cost = _link_cap_cost;
	source_target_consume = _source_target_consume;
}

Solver::~Solver()
{
}
int Solver::Objective(const vector<int> &genome) {


	vector<int> tmp;
    for (int i = 0; i<genome.size();i++){
        if (genome[i] == 1)
            tmp.push_back(directLinkedNodeIDs[i]);

    }
    adaptNetwork(tmp);
  //  timeval starttime,endtime;
////
 //   gettimeofday(&starttime,0);
    int tmpCost = getMinCostFlow(tmp.size());
 //   gettimeofday(&endtime,0);
////
 //   double timeuse = 1000000*(endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec -starttime.tv_usec;
//
//    cout<<timeuse /1000<<endl;//����1000�����к�����ʱ����������1000000�������뼶����ʱ����������1������΢�����ʱ

    recoverNetwork(tmp.size());

    return -1*tmpCost+net_number*deploy_cost;

}
vector<int> Solver::getDirectLinkedNodeIDs(){
	vector<int> res;
	for (int i = 0; i < source_target_consume.size(); i++) {
		res.push_back(source_target_consume[i][1]);
	}
	return res;
}
vector<int> Solver::getAllNodeIDs(){
    vector<int> res;
    for (int i = 0; i < net_number; i++) {
        res.push_back(i);
    }
    return res;
}


// Wrapper function uses a global to remember the object:
Solver* object_which_will_handle_signal;

void Solver::timer(int sig)
{
    if(SIGALRM == sig)
    {
        //printf("timer\n");
        // alarm(1);       //we contimue set the timer
        flag= false;
    }

    return ;
}
void timer_wrapper(int sig){
    return object_which_will_handle_signal->timer(sig);

}
vector<int> Solver::initchroms() {
    //directLinkedNodeIDs =getAllNodeIDs();

    directLinkedNodeIDs = getDirectLinkedNodeIDs();
    //directLinkedNodeIDs = getHigherDegVideoNodeIDs();


//	vector<int> higherDegVideoNodeIDs = getHigherDegVideoNodeIDs();
//
//	directLinkedNodeIDs.insert(directLinkedNodeIDs.end(), higherDegVideoNodeIDs.begin(), higherDegVideoNodeIDs.end());
//
//	sort(directLinkedNodeIDs.begin(), directLinkedNodeIDs.end());
//	directLinkedNodeIDs.erase(unique(directLinkedNodeIDs.begin(), directLinkedNodeIDs.end()), directLinkedNodeIDs.end());

    initialParam(1,directLinkedNodeIDs.size());
    object_which_will_handle_signal=this;

    signal(SIGALRM, timer_wrapper); //relate the signal and function


    alarm(89);       //trigger the timer
    while (flag){
        step();
    }
    vector<int> tmp=getBestIndidual(),res;
    // Now we print out the best genome that the GA found.
    //for(int i=0;)
    //genome2.
    for (int i = 0; i<tmp.size(); i++) {
        if (tmp[i] == 1) {
             //cout << directLinkedNodeIDs[i] << endl;
            res.push_back(directLinkedNodeIDs[i]);

        }

    }
    //cout << (Objective(tmp)-net_number*deploy_cost)*(-1) << endl;
    return res;


}
vector<int> Solver::FindPath(int pathnode) {
	vector<int> node_flow;
	vector<int> path;
	int tmpflow = 0, tonode = pathnode;
	path.push_back(pathnode);
	node_flow = gettonode(tonode);
	if (node_flow.size() != 0) {
		tmpflow = node_flow[1];
		path.push_back(node_flow[0]);
		tonode = node_flow[0];
		node_flow = gettonode(tonode);
		while (node_flow.size() != 0)
		{
			path.push_back(node_flow[0]);
			tonode = node_flow[0];
			if (tmpflow > node_flow[1])
				tmpflow = node_flow[1];
			node_flow = gettonode(tonode);
		}
	}
	if (tmpflow == 0) {
		vector<int> tmp;
		path.swap(tmp);
		return path;
	}
	path.push_back(tmpflow);
	return path;
}

vector<int> Solver::gettonode(int node) {
	vector<int> node_flow;
	for (size_t i = 0; i < flow.size(); i++) {
		if (flow[i][0] == node && flow[i][2] != 0) {
			node_flow.push_back(flow[i][1]);
			node_flow.push_back(flow[i][2]);
			return node_flow;
		}
	}
	vector<int> tmp;
	node_flow.swap(tmp);
	return node_flow;
}

void Solver::recoverPath(vector<int> path) {
	if(path.size() != 0){
		int minflow = path[path.size()-1];
		for (size_t i = 0; i < path.size() - 2; i++) {
			int fromnode = path[i];
			int tonode = path[i + 1];
			for (size_t j = 0; j < flow.size(); j++) {
				if (flow[j][0] == fromnode && flow[j][1] == tonode) {
					flow[j][2] -= minflow;
					break;
				}
			}
		}
	}
}
vector<vector<int>> Solver::getFinalPath(int video_node_number) {
	vector<vector<int>> flow_table;
	string str =  "";

	//NetworkSimplex ns(g);
//	Graph_List::EdgeMap<int> _capacity(g), _cost(g);
//	Graph_List::NodeMap<int> _supply(g);
//
//	for (size_t i = 0; i < edges.size(); i++) {
//		_capacity[edges[i]] = capacity[edges[i]];
//		_cost[edges[i]]=cost[edges[i]];
//	}
//	for (size_t i = 0; i < nodes.size(); i++) {
//		_supply[nodes[i]] = supply[nodes[i]];
//	}
	// Initialize NetworkSimplex algorithm object
    ns.init();
    ns.SetUpperMap(capacity);
    ns.SetCostMap(cost);
    ns.SetSupplyMap(supply);
	//ns.upperMap(capacity).costMap(cost).supplyMap(supply);

	// Run NetworkSimplex
	ns.Run();
	for (Graph_List::ArcItertor a(ns.graph); a != OVER; ++a) {
		if (ns.getFolwOfEachArc(a) != 0) {
			vector<int> flow_node;
			flow_node.push_back(ns.graph.arcs[a.id ^ 1].target_id);
			flow_node.push_back(ns.graph.arcs[a.id].target_id);
			flow_node.push_back(ns.getFolwOfEachArc(a));
			flow_table.push_back(flow_node);
			//printf("Edge %d: %d/%d goes from %d to %d\n", g.id(a), ns.flow(a), capacity[a], g.id(g.source(a)), g.id(g.target(a)));
		}
	}
    return flow_table;

    //cout << total_cost << endl;
//	int tmpCost = ns.totalCost() + (video_node_number*deploy_cost);
//	if (tmpCost <= total_cost)
//	{
//		total_cost = tmpCost;
//		return flow_table;
//	}
//	vector<vector<int>> tmp;
//	flow_table.swap(tmp);
//	return flow_table;

}

string Solver::convertToStandardFormat(const vector<vector<int>> flow_lines) {
	string res;
	flow = flow_lines;
	vector<vector<int>> path_to_str;
	vector<int> flow_path;
	flow_path = FindPath(net_number + consume_number);
	while (flow_path.size() != 0) {
		recoverPath(flow_path);
		path_to_str.push_back(flow_path);
		flow_path = FindPath(net_number + consume_number);
	}
	char s[10]; int path_num = path_to_str.size();
	sprintf(s, "%d", path_num);
	res += s;
	res += "\n\n";
	for (size_t i = 0; i < path_to_str.size(); i++) {
		for (size_t j = 1; j < path_to_str[i].size(); j++) {
			if (path_to_str[i][j] >= net_number&&j<path_to_str[i].size() - 1)
			{
				sprintf(s, "%d", (path_to_str[i][j] - net_number));
				res += s;
				res += " ";
			}
			else
			{
				sprintf(s, "%d", path_to_str[i][j]);
				res += s;
				res += " ";
			}
		}
		res += "\n";
	}
	return res;
}
string Solver::run() {
//	string res;
//	vector<int> best_video_IDs=initchroms();
//	adaptNetwork((best_video_IDs);
//	recoverNetwork(best_video_IDs.size());
//	return res;
	string res;
    initNetwork();
	vector<vector<int>> flow_line;
	vector<int> videoNodeIDs = initchroms();
    //vector<int> videoNodeIDs = getDirectLinkedNodeIDs();
    adaptNetwork(videoNodeIDs);
	flow_line = getFinalPath(videoNodeIDs.size());
	res = convertToStandardFormat(flow_line);
	return res.substr(0, res.size()-1);
}

vector<int> Solver::getHigherDegVideoNodeIDs() {
	vector<int> video_node_ids;
	for (int i = 0; i < consume_number; i++) {
		video_node_ids.push_back(id_degs[i].first);
	}
	return video_node_ids;
}

void Solver::initNetwork() {	
	for (int i = 0; i < net_number; i++)
		nodes.push_back(ns.graph.addSingleNode());
	for (int i = 0; i < consume_number; i++)
		nodes.push_back(ns.graph.addSingleNode());
	sourceNode = ns.graph.addSingleNode();
	nodes.push_back(sourceNode);

    //int j=0;
	for (int i = 0; i < link_cap_cost.size(); i++) {
		int a = link_cap_cost[i][0], b = link_cap_cost[i][1], c = link_cap_cost[i][2], d = link_cap_cost[i][3];
		Graph_List::Edge a1 = ns.graph.addSingleEdge(nodes[a], nodes[b]);
        //j+=2;


		capacity[a1.id] = c;
		cost[a1.id] = d;
		supply[nodes[a].id] = 0;
		supply[nodes[b].id] = 0;

	}
	for (int i = 0; i <source_target_consume.size(); i++) {
		int a = source_target_consume[i][0], b = source_target_consume[i][1], c = source_target_consume[i][2];
		Graph_List::Edge a1 = ns.graph.addSingleEdge(nodes[a + net_number], nodes[b]);

		direct_link_edges.push_back(a1);
		capacity[a1.id] = c;
		cost[a1.id] = 0;
		supply[nodes[a + net_number].id] = -c;
		consum_sum += c;
	}
	supply[sourceNode.id] = consum_sum;




}
void Solver::adaptNetwork(vector<int> video_node_ids) {

	for (int i = 0; i < video_node_ids.size(); i++) {
		Graph_List::Edge a1 = ns.graph.addSingleEdge(nodes[video_node_ids[i]], sourceNode);
		//edges.push_back(a1);
		adaptIDs.push_back(a1.id);
		capacity[a1.id] = 10000;
		cost[a1.id] = 0;
	}

}
void Solver::recoverNetwork(int video_node_number) {
//	for (int i = 0; i <video_node_number; i++) {
//		g.erase(edges[edges.size()-1-i]);
//		//g.erase(g.edgeFromId(i));
//	}
	for(int i=0;i<adaptIDs.size();i++){
        ns.graph.eraseSIngleEdge(adaptIDs[i]);
		capacity.erase(adaptIDs[i]);
		cost.erase(adaptIDs[i]);
	}
    adaptIDs.clear();
//	//cout<<g.valid(g.edgeFromId(0));
//	auto it_1 = capacity.find(edges[edges.size()- video_node_number].id);
//	capacity.erase(it_1, capacity.end());
//	auto it_2 = cost.find(edges[edges.size()- video_node_number].id);
//	cost.erase(it_2, cost.end());


	//auto it_3 = edges.end();
	//edges.erase(it_3 - video_node_number, it_3);
}


int  Solver::getMinCostFlow(int video_node_number) {


//    timeval starttime,endtime;
//////
//    gettimeofday(&starttime,0);
	//NetworkSimplex ns(g);
    //ns.graph
    //ns.
	// Initialize NetworkSimplex algorithm object
    // Run NetworkSimplex
    ns.init();
    ns.SetUpperMap(capacity);
    ns.SetCostMap(cost);
    ns.SetSupplyMap(supply);



	ns.Run();

//    gettimeofday(&endtime,0);
//////
//    double timeuse = 1000000*(endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec -starttime.tv_usec;
////
//    cout<<timeuse /1000<<endl;//����1000�����к�����ʱ����������1000000�������뼶����ʱ����������1������΢�����ʱ
	int supply_sum = 0;//��Ӧ�������ܺ�
	for (Graph_List::ArcItertor a(ns.graph); a != OVER; ++a) {
		if (ns.getFolwOfEachArc(a) != 0) {
			//printf("Edge %d: %d/%d goes from %d to %d\n", g.id(a), ns.flow(a), capacity[a], g.id(g.source(a)), g.id(g.target(a)));
			if (ns.graph.arcs[a.id ^ 1].target_id== sourceNode.id) {
				supply_sum += ns.getFolwOfEachArc(a);
			}
		}
	}
	if (supply_sum != consum_sum)
		return net_number*deploy_cost;
	//cout << total_cost << endl;
	int tmpCost = ns.getTotalCost() + (video_node_number*deploy_cost);
	return tmpCost;


}

