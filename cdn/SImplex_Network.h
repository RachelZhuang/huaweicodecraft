#pragma once
#include<vector>
#include <iostream>
#include <sys/time.h>
using namespace std;
namespace solution {
    struct SimplexNetwork
	{

	private:
		enum StateOfArc {
			UPPER = -1,
			TREE = 0,
			LOWER = 1
		};


		enum DirectionOfArc {
			DOWN = -1,
			UP = 1
		};

	private:


		int num_of_node;
		int num_of_arc;
		int num_of_all_arc;
		int num_of_search_arc;



		int supply_of_supply;


		Graph_List::NodeMap nodeID_to_indexID;
		Graph_List::ArcMap arcID_to_indexID;
		vector<int> source_IDs;
		vector<int> target_IDs;
		
		vector<int> lower_straint_of_arc;
		vector<int> upper_straint_of_arc;
		vector<int> cap_straint_of_arc;
		vector<int> cost_straint_of_arc;
		vector<int> supply_straint_of_node;
		vector<int> flow_result_of_arc;
		vector<int> pi_of_arc;


		vector<int> parent_of_node;
		vector<int> pred_of_node;
		vector<int>thread_of_node;
		vector<int>rev_thread_of_node;
		vector<int> succ_num_of_node;
		vector<int> last_suuc_of_node;
		vector<signed char > pred_dir_of_node;
		vector<signed char>state_of_node;
		vector<int> dirty_revs_of_node;
		int root_of_node;


		int in_arc_id, join_id, u_in_id, v_in_id, u_out_id, v_out_id;
		int delta;

		const int MAX=std::numeric_limits<int>::max();
        const int INF=std::numeric_limits<int>::has_infinity ?std::numeric_limits<int>::infinity() : MAX;


    public:

        Graph_List graph;
        int size_of_block;
        const double FACTOR_OF_BLOCK_SIZE = 1.0;
        const int SIZE_OF_MIN_BLOCK = 10;
        int next_arc_id;

        inline int getNumberOfNodes(const Graph_List& g) {
            int num = 0;
            Graph_List::NodeItertor it(g);

            while(it != OVER){
                ++num;
                ++it;
            }
            return num;

        }
        inline int getNumberOfArcs(const Graph_List& g) {
            int num = 0;
            Graph_List::ArcItertor it(g);
            while(it != OVER){
                ++num;
                ++it;
            }
            return num;
        }

        SimplexNetwork(){

        }

        void init(){
            num_of_node = getNumberOfNodes(graph);
            num_of_arc = getNumberOfArcs(graph);
            int all_node_num = num_of_node + 1;
            int max_arc_num = num_of_arc + 2 * num_of_node;

            source_IDs.resize(max_arc_num);
            target_IDs.resize(max_arc_num);

            lower_straint_of_arc.resize(num_of_arc);
            upper_straint_of_arc.resize(num_of_arc);
            cap_straint_of_arc.resize(max_arc_num);
            cost_straint_of_arc.resize(max_arc_num);
            supply_straint_of_node.resize(all_node_num);
            flow_result_of_arc.resize(max_arc_num);
            pi_of_arc.resize(all_node_num);

            parent_of_node.resize(all_node_num);
            pred_of_node.resize(all_node_num);
            pred_dir_of_node.resize(all_node_num);
            thread_of_node.resize(all_node_num);
            rev_thread_of_node.resize(all_node_num);
            succ_num_of_node.resize(all_node_num);
            last_suuc_of_node.resize(all_node_num);
            state_of_node.resize(max_arc_num);

            int i = 0;
            Graph_List::NodeItertor n(graph);
            while (n != OVER){
                nodeID_to_indexID[n.id] = i;
                ++n; ++i;
            }

            const int skip_id = std::max(num_of_arc / num_of_node, 3);
            i = 0;
            int j = 0;
            Graph_List::ArcItertor a(graph);
            while(a != OVER){
                arcID_to_indexID[a.id] = i;
                source_IDs[i] = nodeID_to_indexID[graph.arcs[a.id^1].target_id];
                target_IDs[i] = nodeID_to_indexID[graph.arcs[a.id].target_id];
                if ((i += skip_id) >= num_of_arc) i = ++j;
                ++a;
            }


            for (int i = 0; i != num_of_node; ++i) {
                supply_straint_of_node[i] = 0;
            }
            for (int i = 0; i != num_of_arc; ++i) {
                lower_straint_of_arc[i] = 0;
                upper_straint_of_arc[i] = INF;
                cost_straint_of_arc[i] = 1;
            }
        }

		void SetUpperMap(unordered_map<int,int>& upperMap) {
            Graph_List::ArcItertor a(graph);
            while(a != OVER){
                upper_straint_of_arc[arcID_to_indexID[a.id]] = upperMap[(a.id / 2)];
                ++a;
            }


		}



		void SetCostMap(unordered_map<int, int>& costMap) {
            Graph_List::ArcItertor a(graph);
            while(a != OVER){
                cost_straint_of_arc[arcID_to_indexID[a.id]] = costMap[(a.id / 2)];
                ++a;
            }

		}



		void SetSupplyMap(unordered_map<int, int>& supplyMap) {
            Graph_List::NodeItertor n(graph);
            while(n != OVER){
                supply_straint_of_node[nodeID_to_indexID[n.id]] = supplyMap[(n.id)];
                ++n;
            }
		}



		void Run() {

            next_arc_id=0;
            supply_of_supply = 0;

            int i = 0;
            while(i != num_of_arc){
                cap_straint_of_arc[i] = upper_straint_of_arc[i];
                ++i;
            }

            int ART_COST=std::numeric_limits<int>::max() / 2 + 1;

            i=0;
            while(i != num_of_arc){
                flow_result_of_arc[i] = 0;
                state_of_node[i] = LOWER;
                ++i;
            }

            root_of_node = num_of_node;
            parent_of_node[root_of_node] = -1;
            pred_of_node[root_of_node] = -1;
            thread_of_node[root_of_node] = 0;
            rev_thread_of_node[0] = root_of_node;
            succ_num_of_node[root_of_node] = num_of_node + 1;
            last_suuc_of_node[root_of_node] = root_of_node - 1;
            supply_straint_of_node[root_of_node] = -supply_of_supply;
            pi_of_arc[root_of_node] = 0;

            num_of_search_arc = num_of_arc;
                num_of_all_arc = num_of_arc + num_of_node;
            int u=0,e = num_of_arc;
            while(u != num_of_node){
                parent_of_node[u] = root_of_node;
                pred_of_node[u] = e;
                thread_of_node[u] = u + 1;
                rev_thread_of_node[u + 1] = u;
                succ_num_of_node[u] = 1;
                last_suuc_of_node[u] = u;
                cap_straint_of_arc[e] = INF;
                state_of_node[e] = TREE;
                switch (compare(supply_straint_of_node[u] >= 0,true)){
                    case true:
                        pred_dir_of_node[u] = UP;
                        pi_of_arc[u] = 0;
                        source_IDs[e] = u;
                        target_IDs[e] = root_of_node;
                        flow_result_of_arc[e] = supply_straint_of_node[u];
                        cost_straint_of_arc[e] = 0;
                        break;
                    case false:
                        pred_dir_of_node[u] = DOWN;
                        pi_of_arc[u] = ART_COST;
                        source_IDs[e] = root_of_node;
                        target_IDs[e] = u;
                        flow_result_of_arc[e] = -supply_straint_of_node[u];
                        cost_straint_of_arc[e] = ART_COST;
                        break;
                }
                ++u; ++e;
            }

            size_of_block = std::max(int(FACTOR_OF_BLOCK_SIZE *
                                       std::sqrt(double(num_of_search_arc))),
                                   SIZE_OF_MIN_BLOCK);

            initialThePivots();

            while (findArcOfEntering()) {
                findNodeOfJoin();
                bool change = findArcOfLeaving();
                changeTheFlow(change);
                if (change) {
                    updateStructureOfTree();
                    updateThePotential();
                }
            }
        }


		int getTotalCost(){
			int c = 0;

			for (Graph_List::ArcItertor a(graph); a != OVER; ++a) {
				int i = arcID_to_indexID[a.id];
				c += (flow_result_of_arc[i]) * (cost_straint_of_arc[i]);
			}
			return c;
		}


		int getFolwOfEachArc(Graph_List::ArcItertor & a){
			return flow_result_of_arc[arcID_to_indexID[a.id]];
		}

        template <typename T>
        bool compare(T a, T b) {
            if (a == b) return true;
            else return false;
        }
        bool findArcOfEntering(){
            int c, min = 0;
            int cnt = size_of_block;
            int e = next_arc_id;
            while(e != num_of_search_arc){
                c = state_of_node[e] * (cost_straint_of_arc[e] + pi_of_arc[source_IDs[e]] - pi_of_arc[target_IDs[e]]);
                switch (compare(c < min, true)) {
                    case true:
                        min = c;
                        in_arc_id = e;
                        break;
                    case false:
                        break;
                }

                switch (compare(--cnt == 0, true)) {
                    case true:
                        if (min < 0) goto search_end;
                        cnt = size_of_block;
                        break;
                    case false:
                        break;
                }

                ++e;
            }
            e=0;
            while( e != next_arc_id){
                c = state_of_node[e] * (cost_straint_of_arc[e] + pi_of_arc[source_IDs[e]] - pi_of_arc[target_IDs[e]]);
                switch (compare(c<min,true)){
                    case true:
                        min = c;
                        in_arc_id = e;
                        break;
                }
                switch (compare(--cnt == 0,true)){
                    case true:
                        if (min < 0) goto search_end;
                        cnt = size_of_block;
                        break;
                }

                ++e;
            }

            if (min >= 0) return false;

            search_end:
            next_arc_id = e;
            return true;
        }
		void findNodeOfJoin() {
			int u = source_IDs[in_arc_id];
			int v = target_IDs[in_arc_id];
			while (u != v) {
                switch (compare(succ_num_of_node[u] < succ_num_of_node[v],true)){
                    case true:
                        u = parent_of_node[u];
                        break;
                    case false:
                        v = parent_of_node[v];
                        break;
                }

			}
			join_id = u;
		}


		bool findArcOfLeaving() {
			int first, second;
            switch (compare(state_of_node[in_arc_id] == LOWER,true)){
                case true:
                    first = source_IDs[in_arc_id];
                    second = target_IDs[in_arc_id];
                    break;
                case false:
                    first = target_IDs[in_arc_id];
                    second = source_IDs[in_arc_id];
                    break;
            }

			delta = cap_straint_of_arc[in_arc_id];
			int result = 0;
			int c, d;
			int e;

            int u = first;
            while(u != join_id){
                e = pred_of_node[u];
                d = flow_result_of_arc[e];
                switch (compare(pred_dir_of_node[u] == DOWN,true)){
                    case true:
                        c = cap_straint_of_arc[e];
                        d = c >= MAX ? INF : c - d;
                        break;
                }
                switch (compare(d < delta,true)){
                    case true:
                        delta = d;
                        u_out_id = u;
                        result = 1;
                        break;
                }

                u = parent_of_node[u];
            }
            u = second;
            while( u != join_id){
                e = pred_of_node[u];
                d = flow_result_of_arc[e];
                switch (compare(pred_dir_of_node[u] == UP,true)){
                    case true:
                        c = cap_straint_of_arc[e];
                        d = c >= MAX ? INF : c - d;
                        break;
                }
                switch (compare(d <= delta,true)){
                    case true:
                        delta = d;
                        u_out_id = u;
                        result = 2;
                        break;
                }
                u = parent_of_node[u];
            }
            switch (compare(result == 1,true)){
                case true:
                    u_in_id = first;
                    v_in_id = second;
                    break;
                case false:
                    u_in_id = second;
                    v_in_id = first;
                    break;
            }

			return result != 0;
		}

		void changeTheFlow(bool change) {
            switch (compare(delta > 0,true)){
                case true:
                    int val = state_of_node[in_arc_id] * delta;
                    flow_result_of_arc[in_arc_id] += val;
                    int u = source_IDs[in_arc_id];
                    while(u != join_id){
                        flow_result_of_arc[pred_of_node[u]] -= pred_dir_of_node[u] * val;
                        u = parent_of_node[u];
                    }

                    u = target_IDs[in_arc_id];
                    while (u != join_id){
                        flow_result_of_arc[pred_of_node[u]] += pred_dir_of_node[u] * val;
                        u = parent_of_node[u];
                    }
                    break;
            }
            switch (compare(change,true)){
                case true:
                    state_of_node[in_arc_id] = TREE;
                    state_of_node[pred_of_node[u_out_id]] =
                            (flow_result_of_arc[pred_of_node[u_out_id]] == 0) ? LOWER : UPPER;
                    break;
                case false:
                    state_of_node[in_arc_id] = -state_of_node[in_arc_id];
                    break;
            }

		}

		void updateStructureOfTree() {
			int old_rev_thread = rev_thread_of_node[u_out_id];
			int old_succ_num = succ_num_of_node[u_out_id];
			int old_last_succ = last_suuc_of_node[u_out_id];
			v_out_id = parent_of_node[u_out_id];

            switch (compare(u_in_id == u_out_id,true)){
                case true:
                    parent_of_node[u_in_id] = v_in_id;
                    pred_of_node[u_in_id] = in_arc_id;
                    pred_dir_of_node[u_in_id] = u_in_id == source_IDs[in_arc_id] ? UP : DOWN;


                    if (thread_of_node[v_in_id] != u_out_id) {
                        int after = thread_of_node[old_last_succ];
                        thread_of_node[old_rev_thread] = after;
                        rev_thread_of_node[after] = old_rev_thread;
                        after = thread_of_node[v_in_id];
                        thread_of_node[v_in_id] = u_out_id;
                        rev_thread_of_node[u_out_id] = v_in_id;
                        thread_of_node[old_last_succ] = after;
                        rev_thread_of_node[after] = old_last_succ;
                    }
                    break;
                case false:
                    int thread_continue = old_rev_thread == v_in_id ?
                                          thread_of_node[old_last_succ] : thread_of_node[v_in_id];

                    int stem = u_in_id;              				int par_stem = v_in_id;          				int next_stem;                				int last = last_suuc_of_node[u_in_id];  				int before, after = thread_of_node[last];
                    thread_of_node[v_in_id] = u_in_id;
                    dirty_revs_of_node.clear();
                    dirty_revs_of_node.push_back(v_in_id);
                    while (stem != u_out_id) {
                        next_stem = parent_of_node[stem];
                        thread_of_node[last] = next_stem;
                        dirty_revs_of_node.push_back(last);

                        before = rev_thread_of_node[stem];
                        thread_of_node[before] = after;
                        rev_thread_of_node[after] = before;

                        parent_of_node[stem] = par_stem;
                        par_stem = stem;
                        stem = next_stem;

                        last = last_suuc_of_node[stem] == last_suuc_of_node[par_stem] ?
                               rev_thread_of_node[par_stem] : last_suuc_of_node[stem];
                        after = thread_of_node[last];
                    }
                    parent_of_node[u_out_id] = par_stem;
                    thread_of_node[last] = thread_continue;
                    rev_thread_of_node[thread_continue] = last;
                    last_suuc_of_node[u_out_id] = last;

                    switch (compare(old_rev_thread != v_in_id,true)){
                        case true:
                            thread_of_node[old_rev_thread] = after;
                            rev_thread_of_node[after] = old_rev_thread;
                            break;
                    }

                    int i=0;
                    while (i != int(dirty_revs_of_node.size())){
                        int u = dirty_revs_of_node[i];
                        rev_thread_of_node[thread_of_node[u]] = u;
                        ++i;
                    }
                    int tmp_sc = 0, tmp_ls = last_suuc_of_node[u_out_id];
                    int u = u_out_id, p = parent_of_node[u];
                    while(u != u_in_id){
                        pred_of_node[u] = pred_of_node[p];
                        pred_dir_of_node[u] = -pred_dir_of_node[p];
                        tmp_sc += succ_num_of_node[u] - succ_num_of_node[p];
                        succ_num_of_node[u] = tmp_sc;
                        last_suuc_of_node[p] = tmp_ls;
                        u = p;
                        p = parent_of_node[u];
                    }
                    pred_of_node[u_in_id] = in_arc_id;
                    pred_dir_of_node[u_in_id] = u_in_id == source_IDs[in_arc_id] ? UP : DOWN;
                    succ_num_of_node[u_in_id] = old_succ_num;
                    break;
            }

			int up_limit_out = last_suuc_of_node[join_id] == v_in_id ? join_id : -1;
			int last_succ_out = last_suuc_of_node[u_out_id];
            int u = v_in_id;
            while (u != -1 && last_suuc_of_node[u] == v_in_id){
                last_suuc_of_node[u] = last_succ_out;
                u = parent_of_node[u];
            }

			if (join_id != old_rev_thread && v_in_id != old_rev_thread) {
                int u = v_out_id;
                while (u != up_limit_out && last_suuc_of_node[u] == old_last_succ){
                    last_suuc_of_node[u] = old_rev_thread;
                    u = parent_of_node[u];

                }

			}
			else if (last_succ_out != old_last_succ) {
                int u = v_out_id;
                while (u != up_limit_out && last_suuc_of_node[u] == old_last_succ){
                    last_suuc_of_node[u] = last_succ_out;
                    u = parent_of_node[u];
                }

			}

            u=v_in_id;
            while (u != join_id){
                succ_num_of_node[u] += old_succ_num;
                u = parent_of_node[u];
            }
            u=v_out_id;
            while (u != join_id){
                succ_num_of_node[u] -= old_succ_num;
                u = parent_of_node[u];
            }
		}

		void updateThePotential() {
			int sigma = pi_of_arc[v_in_id] - pi_of_arc[u_in_id] -
				pred_dir_of_node[u_in_id] * cost_straint_of_arc[in_arc_id];
			int end = thread_of_node[last_suuc_of_node[u_in_id]];
            int u = u_in_id;
            while (u != end){
                pi_of_arc[u] += sigma;
                u = thread_of_node[u];
            }

		}

		void initialThePivots() {
			int curr, total = 0;
			std::vector<Graph_List::Node> supply_nodes, demand_nodes;
            Graph_List::NodeItertor u(graph);
            while(u != OVER){
                curr = supply_straint_of_node[nodeID_to_indexID[u.id]];
                if (curr > 0) {
                    total += curr;
                    supply_nodes.push_back(u);
                }
                else if (curr < 0) {
                    demand_nodes.push_back(u);
                }
                ++u;
            }

			vector<int> arc_vector;
            int i=0;
            while( i != int(demand_nodes.size())) {
                Graph_List::Node v = demand_nodes[i];
                int c, min_cost = std::numeric_limits<int>::max();
                Graph_List::Arc min_arc = OVER;
                Graph_List::InArcIterator a(graph, v);
                while(a != OVER){
                    c = cost_straint_of_arc[arcID_to_indexID[a.id]];
                    if (c < min_cost) {
                        min_cost = c;
                        min_arc = a;
                    }
                    ++a;

                }
                if (min_arc != OVER) {
                    arc_vector.push_back(arcID_to_indexID[min_arc.id]);
                }
                i++;
            }
            i=0;
            while(i != int(arc_vector.size())){
                in_arc_id = arc_vector[i];
                switch (compare(state_of_node[in_arc_id] * (cost_straint_of_arc[in_arc_id] + pi_of_arc[source_IDs[in_arc_id]] -
                                                  pi_of_arc[target_IDs[in_arc_id]]) >= 0,true)){
                    case true:
                        continue;
                        break;
                    case false:
                        break;
                }
                findNodeOfJoin();
                bool change = findArcOfLeaving();
                //if (delta >= MAX) return false;
                changeTheFlow(change);
                switch (compare(change,true)){
                    case true:
                        updateStructureOfTree();
                        updateThePotential();
                        break;
                    case false:
                        break;
                }

                ++i;
            }

		}



	};
}