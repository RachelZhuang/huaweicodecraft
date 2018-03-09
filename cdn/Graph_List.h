#pragma once
#include <algorithm>
#include <vector>
#include <list>
#include<cmath>
#include <unordered_map>
using namespace std;
namespace solution {
    class Over {
    public:
        bool operator==(Over) { return true; }
        bool operator!=(Over) { return false; }
        bool operator< (Over) { return false; }
    };

    const Over OVER = Over();

	struct Graph_List {

	public:
		struct Node {
	public:
		int id;
		Node(int pid) { id = pid; }
		Node() {}
		Node(Over) { id = -1; }
		bool operator==(const Node& node) const { return id == node.id; }
		bool operator!=(const Node& node) const { return id != node.id; }
	};

	public:
		struct Edge {
	public:
		int id;
		Edge(int pid) { id = pid; }
	};

	public:
		struct Arc {
	public:
		int id;
		Arc(int pid) { id = pid; }
		Arc() {}
		Arc(Over) { id = -1; }
		bool operator!=(const Arc& arc) const { return id != arc.id; }
	};

			typedef unordered_map<int,int> NodeMap;
			typedef unordered_map<int, int> EdgeMap;
			typedef unordered_map<int, int> ArcMap;

			class NodeStore {
			public:
				int first_out_id;
				int prev_id, next_id;
			};

			class ArcStore {
			public:
				int target_id;
				int prev_out_id, next_out_id;
			};

			std::vector<NodeStore> nodes;

			int first_node_id;

			int first_free_node_id;

			std::vector<ArcStore> arcs;

			int first_free_arc;

		struct NodeItertor : public Node {
			const Graph_List* _graph;
		public:


			NodeItertor(const Graph_List& graph) {
				_graph=&graph;
				id=_graph->first_node_id;
			}

			NodeItertor& operator++() {
				id=_graph->nodes[id].next_id;
				return *this;
			}

		};

		struct ArcItertor : public Arc {
			const Graph_List* _graph;
		public:

			 ArcItertor(const Graph_List& graph) {
				_graph=&graph;
				 int n = _graph->first_node_id;
				 while (n != -1 &&  _graph->nodes[n].first_out_id == -1) {
					 n =  _graph->nodes[n].next_id;
				 }
				 id = (n == -1) ? -1 : _graph->nodes[n].first_out_id;
			}
			ArcItertor& operator++() {
				switch (_graph->arcs[id].next_out_id != -1)
				{
					case true:
						id = _graph->arcs[id].next_out_id;
						break;
					case false:
						int n = _graph->nodes[_graph->arcs[id ^ 1].target_id].next_id;
						while (n != -1 && _graph->nodes[n].first_out_id == -1) {
							n = _graph->nodes[n].next_id;
							break;
						}

						id = (n == -1) ? -1 : _graph->nodes[n].first_out_id;
				}
				return *this;
			}

		};
		struct InArcIterator : public Arc {
			const Graph_List* _graph;
		public:

			InArcIterator(const Graph_List& graph, const Node& node)
				 {
				_graph=&graph;
				int tmp = ( _graph->nodes[node.id].first_out_id);
				id = (tmp ^ 1);
				switch (id == -2)
				{
					case true:
						if (id == -2) id = -1;
						break;
					case false:

						break;
					default:
						break;
				}
			}


			InArcIterator& operator++() {
				id = ((_graph->arcs[id ^ 1].next_out_id) ^ 1);
				switch (id == -2)
				{
					case true:
						if (id == -2) id = -1;
						break;
					case false:

						break;
					default:
						break;
				}
				return *this;
			}

		};


			bool  compare(int a, int b) {
				if (a == b) return true;
				else return  false;
			}

			template<typename T>
			bool compare(T a, T b) {
				return true;
			}

			Node addSingleNode() {
				int n;

				switch (compare(first_free_node_id, -1))
				{
				case true:
					n = nodes.size();
					nodes.push_back(NodeStore());
					break;
				case false:
					n = first_free_node_id;
					first_free_node_id = nodes[n].next_id;
					break;
				default:
					break;
				}


				nodes[n].next_id = first_node_id;
				switch (compare(first_node_id, -1))
				{
				case true:
					break;
				case false:
					nodes[first_node_id].prev_id = n;
					break;
				default:
					break;
				}
				//   if (first_node_id != -1) nodes[first_node_id].prev_id = n;
				first_node_id = n;
				nodes[n].prev_id = -1;

				nodes[n].first_out_id = -1;

				return Node(n);
			}

			Edge addSingleEdge(Node u, Node v) {
				int n;
				switch (compare(first_free_arc, -1))
				{
				case true:
					n = arcs.size();
					arcs.push_back(ArcStore());
					arcs.push_back(ArcStore());
					break;
				case false:
					n = first_free_arc;
					first_free_arc = arcs[n].next_out_id;
					break;
				default:
					break;
				}

				arcs[n].target_id = u.id;
				arcs[n | 1].target_id = v.id;

				arcs[n].next_out_id = nodes[v.id].first_out_id;
				switch (compare(nodes[v.id].first_out_id, -1))
				{
				case true:
					break;
				case false:
					arcs[nodes[v.id].first_out_id].prev_out_id = n;
					break;
				default:
					break;
				}

				arcs[n].prev_out_id = -1;
				nodes[v.id].first_out_id = n;

				arcs[n | 1].next_out_id = nodes[u.id].first_out_id;
				switch (compare(nodes[u.id].first_out_id, -1))
				{
				case true:
					break;
				case false:
					arcs[nodes[u.id].first_out_id].prev_out_id = (n | 1);
					break;
				default:
					break;
				}

				arcs[n | 1].prev_out_id = -1;
				nodes[u.id].first_out_id = (n | 1);

				return Edge(n / 2);
			}


			void eraseSIngleEdge(const int& edgeID) {
				std::vector<Arc> av;
				av.push_back(Arc(edgeID* 2 + (true ? 1 : 0)));
				av.push_back(Arc(edgeID * 2 + (false ? 1 : 0)));

				int n = edgeID * 2;
				switch (compare(arcs[n].next_out_id, -1)) {
				case true:
					break;
				case false:
					arcs[arcs[n].next_out_id].prev_out_id = arcs[n].prev_out_id;
					break;


				}
				switch (compare(arcs[n].prev_out_id, -1)) {
				case true:
					nodes[arcs[n | 1].target_id].first_out_id = arcs[n].next_out_id;
					break;
				case false:
					arcs[arcs[n].prev_out_id].next_out_id = arcs[n].next_out_id;
					break;


				}
				switch (compare(arcs[n | 1].next_out_id, -1)) {
				case true:
					break;
				case false:
					arcs[arcs[n | 1].next_out_id].prev_out_id = arcs[n | 1].prev_out_id;

					break;


				}
				switch (compare(arcs[n | 1].prev_out_id, -1)) {
				case true:
					nodes[arcs[n].target_id].first_out_id = arcs[n | 1].next_out_id;
					break;
				case false:
					arcs[arcs[n | 1].prev_out_id].next_out_id = arcs[n | 1].next_out_id;


					break;


				}


				arcs[n].next_out_id = first_free_arc;
				first_free_arc = n;
				arcs[n].prev_out_id = -2;
				arcs[n | 1].prev_out_id = -2;
			}


			Graph_List(){
				first_node_id=(-1);
				first_free_node_id=(-1);
				first_free_arc=(-1);
			}

			~Graph_List() {

			}

	};
}